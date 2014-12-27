#include "../stdafx.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#if !defined(_WINDOWS) || defined(__CYGWIN__)
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include "wincompat.h"
#endif
#if defined(__CYGWIN__) || defined(__MINGW32_VERSION)
#include "getaddrinfo.h"
#endif
#ifndef _MSC_EXTENSIONS
#include <unistd.h>
#endif
#include "utils.h"

/* character escaping */
#define PASSTHRU(c) (isalnum(c) || c == ' ' || c == '.' || c == '/')

#ifndef _WINDOWS
#define SET_ERRDESC(errdesc, errdesclen) (errdesc ? snprintf(errdesc, \
    errdesclen, "%s", strerror(errno)) : 0);
#else
#define SET_ERRDESC(errdesc, errdesclen) wsa_set_errdesc(errdesc, errdesclen);
#endif

#ifdef DEBUG_PROTOCOL
extern int debug_protocol;
#endif

/* size of first buffer malloc; start small to exercise grow routines */
#define	FIRSTSIZE	128

static FILE *log_stream = 0;
static const char *log_pname = 0;

#if defined(_WINDOWS) && !defined(_CYGWIN)
static void
wsa_set_errdesc(char *errdesc, int len)
{
	int err;

	if (!errdesc)
		return;
	switch (err = WSAGetLastError()) {
	case 10038:
		snprintf(errdesc, len, "Socket operation on non-socket");
		return;
	case 10061:
		snprintf(errdesc, len, "Connection refused");
		return;
	default:
		snprintf(errdesc, len, "WSA error %d", err);
		return;
	}
}
#endif


/* Modified to work properly on ALL systems, not just UNIX */
int
pvasprintf(char **ret, const char *fmt, va_list ap)
{
	char *buf = NULL;
	size_t bufsize = 0;
	char *newbuf = NULL;
	size_t nextsize = 0;
	int outsize = 0;

	bufsize = 0;
	for (;;) {
		if (bufsize == 0) {
			if ((buf = (char *)malloc(FIRSTSIZE)) == NULL) {
				*ret = NULL;
				return -1;
			}
			bufsize = 1;
		} else if ((newbuf = (char *)realloc(buf, nextsize)) != NULL) {
			buf = newbuf;
			bufsize = nextsize;
		} else {
			free(buf); buf = NULL;
			*ret = NULL;
			return -1;
		}

#ifdef _WINDOWS
		outsize = _vsnprintf(buf, bufsize, fmt, ap);
#else
		{
			va_list copy;
			va_copy(copy, ap);
			outsize = vsnprintf(buf, bufsize, fmt, copy);
			va_end(copy);
		}
#endif

		if (outsize == -1) {
			/* Clear indication that output was truncated, but no
			 * clear indication of how big buffer needs to be, so
			 * simply double existing buffer size for next time.
			 */
			nextsize = (int)bufsize * 2;

		} else if (outsize == (int)bufsize) {
			/* Output was truncated (since at least the \0 could
			 * not fit), but no indication of how big the buffer
			 * needs to be, so just double existing buffer size
			 * for next time.
			 */
			nextsize = (int)bufsize * 2;

		} else if (outsize > (int)bufsize) {
			/* Output was truncated, but we were told exactly how
			 * big the buffer needs to be next time. Add two chars
			 * to the returned size. One for the \0, and one to
			 * prevent ambiguity in the next case below.
			 */
			nextsize = outsize + 2;

		} else if (outsize == (int)bufsize - 1) {
			/* This is ambiguous. May mean that the output string
			 * exactly fits, but on some systems the output string
			 * may have been trucated. We can't tell.
			 * Just double the buffer size for next time.
			 */
			nextsize = (int)bufsize * 2;

		} else {
			/* Output was not truncated */
			break;
		}
	}
	*ret = buf;
	return (int)strlen(buf);
}

int
pasprintf(char **ret, const char *fmt, ...)
{
	va_list args;
	int outsize = 0;

	va_start(args, fmt);
	outsize = pvasprintf(ret, fmt, args);
	va_end(args);

	return outsize;
}

int
pd_getline(char *buf, unsigned int bufsize, int *bufcur,
    int *buflen, int(*readfunc)(int, void *, unsigned int, char *,
    int), int(*closefunc)(int, char *, int), int readfd, char **line,
    char *errdesc, int errlen)
{
	char *eol;
	unsigned int srcsize;
	int linelen;
	int linesize = 1024;

	linelen = 0;
 	if (!(*line = malloc(linesize))) {
		if (closefunc)
			(void) closefunc(readfd, errdesc, errlen);
		return 0;
	}
	**line = 0;
top:
	while (*bufcur < *buflen) {	
		if (!(eol = strchr(buf + *bufcur, '\n'))) {
			if (!(eol = memchr(buf + *bufcur, '\0', *buflen -
			    *bufcur))) {
				/* couldn't find '\n' or '\0' */
				srcsize = *buflen - *bufcur; 
			} else {
				/* found '\0' */
				srcsize = eol - buf - *bufcur + 1;
			}
		} else { 
			/* found '\n' */
			srcsize = eol - buf - *bufcur + 1;
			*eol = 0;
		}
		while ((int)srcsize > (linesize - linelen)) {
			char *newline;

			linesize *= 2;
			if ((newline = realloc(*line, linesize)) == NULL) {
				free(*line); *line = NULL;
				*line = NULL;
				if (closefunc)
					(void) closefunc(readfd, errdesc,
					    errlen);
				return 0;
			}
			*line = newline;
		}
		memcpy(*line + linelen, buf + *bufcur, srcsize);
		linelen += srcsize;
		*bufcur += srcsize;		
		if (eol) {
			if ((eol = strchr(*line, '\r')) != NULL)
				*eol = 0;
#ifndef _MSC_EXTENSIONS
#ifdef DEBUG_PROTOCOL
			if (debug_protocol) {
				write(1, "R: ", 3);
				write(1, *line, strlen(*line));
				write(1, "\n", 1);
			}
#endif
#endif
			/*
			 * XXX This won't work on read boundary.  Can
			 * we just remove it?
			 */
			/* Flash client sent NULL after CR; ignore it */
			if (!buf[*bufcur])
				(*bufcur)++;
			return 1;
		}
	}
	*bufcur = 0;
	if ((*buflen = readfunc(readfd, buf, bufsize - 1, errdesc, errlen)) <=
	    0) {
		free(*line); *line = NULL;
		*line = NULL;
		if (closefunc)
			(void) closefunc(readfd, errdesc, errlen);
		return 0;
	}
	buf[*buflen] = 0;
	goto top;
}

int
pd_getline_simple(int fd, char **line)
{
	int bufcur = 0;
	int buflen = 0;
	char buf[2];

	return pd_getline(buf, sizeof (buf), &bufcur, &buflen, pu_read, 0, fd,
	    line, 0, 0);
}

int
getmatchsub(const char *line, char **subp, const regmatch_t pmatch[], int n)
{
	int len;

	len = (pmatch[n].rm_so >= 0) ? pmatch[n].rm_eo - pmatch[n].rm_so : 0;
	if (subp) {
		if (len) {
			if (!(*subp = malloc(len + 1)))
				return len;
			memcpy(*subp, line + pmatch[n].rm_so, len);
			subp[0][len] = 0;
		} else {
			*subp = NULL;
		}
	}

	return len;
}

int CCONV
stream_server_connect(const char *dest, const char *svcname, int *fdp,
     char *errdesc, int errdesclen)
{
	struct addrinfo hints, *res, *res0;
	int error;
	int s=0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((error = getaddrinfo(dest, svcname, &hints, &res0)) != 0) {
		if (errdesc)
			snprintf(errdesc, errdesclen, "getaddrinfo: %s",
			    gai_strerror(error));
		return 0;
	}
	for (res = res0; res; res = res->ai_next) {
	        if ((s = (int)socket(res->ai_family, res->ai_socktype,
 		    res->ai_protocol)) <= 0) {
			SET_ERRDESC(errdesc, errdesclen);
			continue;
	        }

	        if (connect(s, res->ai_addr, (int) res->ai_addrlen) < 0) {
			SET_ERRDESC(errdesc, errdesclen);
#ifndef _WINDOWS
			close(s);
#else
			closesocket(s);
#endif
			s = -1;
			continue;
	        }

	        break;
	}
	if (s < 0) {
		SET_ERRDESC(errdesc, errdesclen);
		return 0;
	}
	if (fdp)
		*fdp = s;
	freeaddrinfo(res0);
	return 1;
}

/*
 * Accepts a TCP connection on a given port and invokes the given
 * callback to handle it. This is where the main server thread sits.
 */
int
stream_server_accept(int port, void(*clfunc)(int fd, const char *addr,
    int port), char *errdesc, int errdesclen)
{
	struct sockaddr_in sin = { 0 };
	struct sockaddr_in cin = { 0 };
	const int opt = 1;
	socklen_t cl;
	char *addr;
	int fd;
	int s;

	if (!(s = (int)socket(PF_INET, SOCK_STREAM, 0))) {
		SET_ERRDESC(errdesc, errdesclen);
		return 0;
	}
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof (opt));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;
	if (bind(s, (struct sockaddr *)&sin, sizeof (sin)) != 0) {
		SET_ERRDESC(errdesc, errdesclen);
		return 0;
	}
	if (listen(s, 5) != 0) {
		SET_ERRDESC(errdesc, errdesclen);
		return 0;
	}
	for (;;) {
		cl = sizeof(cin);
		while ((fd = (int)accept(s, (struct sockaddr *)&cin, &cl)) < 0 &&
		    errno == EAGAIN)
			;
		if (fd < 0) {
			SET_ERRDESC(errdesc, errdesclen);
			return 0;
		}
		setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt,
		    sizeof (opt));
		addr = strdup(inet_ntoa(cin.sin_addr));
		clfunc(fd, addr, ntohs(cin.sin_port));
		free(addr); addr = NULL;
	}
	/*NOTREACHED*/
}

int
pu_write(int fd, const void *buf, unsigned int len, char *errdesc, int edlen)
{
	static pthread_mutex_t *writelock = 0;
	int res = 0;
	int olen = len;

	if (fd == -1)
		return len;
#ifndef _MSC_EXTENSIONS
	if (!writelock) {
		if (!(writelock = malloc(sizeof (*writelock))))
			return 0;
		pthread_mutex_init(writelock, NULL);
	}
	pthread_mutex_lock(writelock);
	do {
		if (res > 0) {
			buf += res;
			len -= res;
		}
		if (len)
			res = send(fd, buf, len, 0);
		else
			res = olen;
	} while (len && (res >= 0 || (res == -1 && errno == EINTR)));
	pthread_mutex_unlock(writelock);
#else
	res = send(fd, buf, len, 0);
#endif

	if (res != olen) {
		SET_ERRDESC(errdesc, edlen);
		return 0;
	} else
		return 1;
}

int
pu_read(int fd, void *buf, unsigned int len, char *errdesc, int edlen)
{
	int res;
#ifdef DEBUG_RECV
	int i = 1;
#endif
	if (fd == -1)
		return 0;

#ifndef _MSC_EXTENSIONS
	do {
		res = recv(fd, buf, len, 0);
#ifdef DEBUG_RECV
		if (res <= 0) {
			printf("recv: %d (%d %d %s)\n", res, errno, len,
			    strerror(errno));
		}
#endif
	} while (res < 0 && (errno == EINTR || errno == EAGAIN));
#else
	res = recv(fd, buf, len, 0);
#endif
#ifdef DEBUG_RECV
	if (i) {
		write(1, "recv ", 5);
		if (res > 0)
			write(1, buf, res);
		else
			write(1, "NOTHING", 7);
		write(1, "\n", 1);
	}
#endif
	if (res <= 0) {
		SET_ERRDESC(errdesc, edlen);
		return 0;
	} else
		return res;
}

int
pu_close(int fd, char *errdesc, int edlen)
{
	int res=0;
	if(fd == -1) return 0;
#ifndef _WINDOWS
	res = close(fd);
#else
	res = closesocket(fd);
#endif
	SET_ERRDESC(errdesc, edlen);
	return res;
}

static const char *
pu_log_level_str(pu_log_level_t l)
{
	switch (l) {
	case PUL_ERR:
		return "ERR";
	case PUL_CRIT:
		return "CRIT";
	case PUL_WARN:
		return "WARN";
	case PUL_INFO:
		return "INFO";
	case PUL_DEBUG:
		return "DEBUG";
	}
	return "???";
}

void
pu_log_stream(FILE *str)
{
	log_stream = str;
}

void
pu_log_pname(const char *newname)
{
	log_pname = newname;
}

void
pu_log(pu_log_level_t l, int s, const char *entry)
{
#ifdef WINCE
	if (!log_stream)
		log_stream = stdout;
	fprintf(log_stream, "%s%s%d/%s %s\n", log_pname ? log_pname : "",
	    log_pname ? " " : "", s, pu_log_level_str(l), entry);
	fflush(log_stream);
#else
	char date[50];
 #ifndef _WINDOWS
	struct tm tm;
 #endif
	struct tm *tmp;
	time_t t;

	if (!log_stream)
		log_stream = stdout;

	time(&t);
 #ifndef _WINDOWS
	localtime_r(&t, &tm);
	tmp = &tm;
 #else
	tmp = localtime(&t);
 #endif
	if (!strftime(date, sizeof (date), "%c", tmp))
		strncpy(date, "?", sizeof (date));
	fprintf(log_stream, "%s %s%s%d/%s %s\n", date, log_pname ? log_pname :
	    "", log_pname ? " " : "", s, pu_log_level_str(l), entry);
	fflush(log_stream);
#endif
}

static int
hexval(unsigned char c)
{
	if (isdigit(c))
		return c - '0';
	c = tolower(c);
	if (c <= 'f')
		return c - 'a' + 10;
	return 0;
}

int
unescape(const char *src, char **dstp, unsigned int *dlenp)
{
	char *dst;
	size_t slen;
	size_t dlen;
	size_t i;

	for (i = 0, dlen = 0, slen = strlen(src); i < slen; i++, dlen++)
		if (src[i] == '\\')
			i += 3;

	if (!(dst = malloc(dlen + 1)))
		return 0;
	for (i = 0, dlen = 0, slen = strlen(src); i < slen; i++, dlen++)
		if (src[i] == '\\') {
			dst[dlen] = hexval((unsigned)src[i + 2]) * 16 +
			    hexval((unsigned)src[i + 3]);
			i += 3;
		} else
			dst[dlen] = src[i];
	if (dlenp)
		*dlenp = (unsigned int)dlen;
	dst[dlen] = 0;

	*dstp = dst;
	return 1;
}

static unsigned char
hexchar(unsigned char c)
{
	if (c > 16)
		return 'f';
	if (c >= 10)
		return c - 10 + 'a';
	return c + '0';
}

int
escape(const char *src, unsigned int slen, char **dstp)
{
	char *dst;
	size_t dlen;
	size_t i;

	if (!slen)
		slen = (int)strlen(src);
	for (i = 0, dlen = 0; i < slen; i++)
		if (PASSTHRU(src[i]))
			dlen++;
		else
			dlen += 4;

	if (!(dst = malloc(dlen + 1)))
		return 0;
	for (i = 0, dlen = 0; i < slen; i++)
		if (!PASSTHRU(src[i])) {
			dst[dlen++] = '\\';
			dst[dlen++] = 'x';
			dst[dlen++] = hexchar((unsigned char)src[i] / 16);
			dst[dlen++] = hexchar((unsigned char)src[i] % 16);
		} else {
			dst[dlen++] = src[i];
		}
	dst[dlen++] = 0;

	*dstp = dst;
	return 1;
}
