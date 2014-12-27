#include "stdafx.h"
#include "pdictserver.h"
#include "utils.h"
#include "phidgetinterface.h"
#include "PhidgetWebservice21.h"
#include "md5.h"
#include "zeroconf.h"

#define WRITE(fd, msg) send(fd, msg, strlen(msg), 0)

/*
*	Phidget WebService Protocol version history
*	1.0 - Initial version
*
*/
const char *protocol_ver = "1.0";

char defaultName[1024] = ""; //computer name

int port = 5001;
const char *serverName = defaultName; //for mDNS
const char *password = NULL;

int print_debug_messages = FALSE;

pdict_t *pd;
pthread_mutex_t pd_mutex;
pthread_mutex_t pd_mutex_mutex;
pthread_t locking_thread = 0;
int lock_count = 0;

/*
#ifdef DEBUG
static FILE *file = 0;
void * phi_malloc(size_t size, const char *source, int line) {
	void *ptr = malloc(size);
	if(!file)
		file = fopen("memleak.log","w");
	fprintf(file, "%x malloc - %s:%d\n", ptr, source, line); fflush(stdout);
	return ptr;
}

void phi_free(void *ptr, const char *source, int line) {
	if(!file)
		file = fopen("memleak.log","w");
	fprintf(file, "%x free - %s:%d\n", ptr, source, line); fflush(stdout);
	free(ptr); ptr = NULL;
}
#endif
*/

char policy[1024];

char * get_policy_file() {
	DPRINT("Sending policy file...");
	sprintf(policy,"<?xml version=\"1.0\"?>\n"
							"<cross-domain-policy>\n"
							"\t<allow-access-from domain=\"*\" to-ports=\"%d\" />\n"
						   "</cross-domain-policy>",port);
	return policy;
}

void
pd_lock(void *pd_lock) {
	pthread_mutex_lock(&pd_mutex_mutex);
	if(locking_thread != pthread_self())
	{		
		pthread_mutex_unlock(&pd_mutex_mutex);
		(void) pthread_mutex_lock(pd_lock);
		pthread_mutex_lock(&pd_mutex_mutex);
		locking_thread = pthread_self();
	}
	lock_count++;
	pthread_mutex_unlock(&pd_mutex_mutex);
}

void
pd_unlock(void *pd_lock) {
	pthread_mutex_lock(&pd_mutex_mutex);
	if(locking_thread == pthread_self())
	{
		lock_count--;
		if(lock_count == 0)
		{
			(void) pthread_mutex_unlock(pd_lock);
			locking_thread = 0;
		}
	}
	pthread_mutex_unlock(&pd_mutex_mutex);
}

static int authenticate(pds_session_t *pdss)
{
	char buf[100];
	char errdesc[1024];
	if(password != NULL)
	{
		int randomHash = rand();
		md5_state_t state;
		md5_byte_t digest[16];
		
		int di;
		char *line;
		int res;

tryagain:
		DPRINT("Authenticating...");
		
		snprintf(buf, sizeof(buf), "999 %d\n", randomHash);
		if(!(pu_write(pdss->pdss_wfd, buf, strlen(buf) + pdss->pdss_client_cr_null, errdesc, sizeof(errdesc))))
		{
			DPRINT("Error in pu_write: %s", errdesc);
			return EPHIDGET_UNEXPECTED;
		}
		
		pdss->pdss_errdesc[0] = 0;
		res = pd_getline(pdss->pdss_readbuf, sizeof (pdss->pdss_readbuf),
			&pdss->pdss_bufcur, &pdss->pdss_buflen, pdss->pdss_read,
			pdss->pdss_close, pdss->pdss_rfd, &line,
			pdss->pdss_errdesc, sizeof (pdss->pdss_errdesc));
			
		if(!res)
		{
			DPRINT("Error in pd_getline: %s", errdesc);
			return EPHIDGET_UNEXPECTED;
		}
		
		if(strncmp(line, "997", 3))
		{
			if(!strcmp(line, "need nulls"))
			{
				pdss->pdss_client_cr_null = 1;
				goto tryagain;
			}
			if(!strcmp(line, "<policy-file-request/>"))
			{
				pdss->pdss_client_cr_null = 1;
				get_policy_file();
				if(!(pu_write(pdss->pdss_wfd, policy, strlen(policy) + pdss->pdss_client_cr_null, errdesc, sizeof(errdesc))))
				{
					DPRINT("Error in pu_write: %s", errdesc);
					return EPHIDGET_UNEXPECTED;
				}
				goto tryagain;
			}
			DPRINT("Recieved unexpected data in authenticate: %s", line);
			return EPHIDGET_UNEXPECTED;
		}
		
		sprintf(buf,"%d%s",randomHash,password);
		
		md5_init(&state);
		md5_append(&state, (const md5_byte_t *)buf, strlen(buf));
		md5_finish(&state, digest);
		
		for (di = 0; di < 16; ++di)
			sprintf((buf) + di * 2, "%02x", digest[di]);
		
		// knock off the \n
		line[sizeof(line)-1] = '\0';
			
		DPRINT("calculated password hash: %s",(buf));
		DPRINT("recieved password hash: %s",(line+4));
		
		if(strncmp(buf,(line+4),100)) {					
			snprintf(buf, sizeof(buf), "998 Authorization failed\n");
			if(!(pu_write(pdss->pdss_wfd, buf, strlen(buf) + pdss->pdss_client_cr_null, errdesc, sizeof(errdesc))))
			{
				DPRINT("Error in accept_cb: %s", errdesc);
				return EPHIDGET_UNEXPECTED;
			}
			return EPHIDGET_UNEXPECTED;
		}
		snprintf(buf, sizeof(buf), "996 Authentication passed\n");
		if(!(pu_write(pdss->pdss_wfd, buf, strlen(buf) + pdss->pdss_client_cr_null, errdesc, sizeof(errdesc))))
		{
			DPRINT("Error in accept_cb: %s", errdesc);
			return EPHIDGET_UNEXPECTED;
		}
	}
	else
	{
		DPRINT("No Authentication");
		snprintf(buf, sizeof(buf), "996 No need to authenticate\n");
		if(!(pu_write(pdss->pdss_wfd, buf, strlen(buf) + pdss->pdss_client_cr_null, errdesc, sizeof(errdesc))))
		{
			DPRINT("Error in accept_cb: %s", errdesc);
			return EPHIDGET_UNEXPECTED;
		}
	}
	return EPHIDGET_OK;
}

/*
 * This is called from stream_server_accept via a function pointer on a new connection
 * initialized a new pdss (phidget dictionary server session), authenticates the connection
 * and starts the pds_session_serve thread.
 */
static void
accept_cb(int fd, const char *claddr, int clport)
{
	const pds_session_t *pdss;
	char errdesc[1024];
	pthread_t t;

	if (!(pdss = pds_session_alloc(pd, pd_lock, pd_unlock, &pd_mutex, fd, pu_read, fd, pu_write, pu_close,
		pu_log, NULL, errdesc, sizeof (errdesc)))) {
		/* XXX log error */
		DPRINT("pds_session_alloc");
		(void) WRITE(fd, "500 temporarily unavailable\n");
		pu_close(fd, errdesc, sizeof (errdesc));
		return;
	}
	
	DPRINT("Accepted");
	
	/* first thing - Authenticate */
	if(authenticate((pds_session_t *)pdss)){
		DPRINT("Authentication failed - closing connection");
		pu_close(fd, errdesc, sizeof (errdesc));
		return;
	}
	
	pthread_create(&t, NULL, (void *(*)(void *))pds_session_serve,
	    (void *)pdss);
	//start_phidget();
}

static void print_help()
{
	printf("'phidgetwebservice21' is a Phidget and Dictionary server from Phidgets Inc. "
		"See www.phidgets.com for more information.\n\n");
	printf("Usage: phidgetwebservice21 [OPTION]\n\n");
	printf("All parameters are optional. The default parameters are: port=5001, "
		"ServerName=PhidgetWebService and no password\n\n");
	printf("Options:\n");
	printf("  -p      Port\n");
	printf("  -n      Server Name\n");
	printf("  -P      Password\n");
	printf("  -v      Debug mode\n");
	printf("  -h      Display this help\n");
}

void sighandler (int n)
{
    DPRINT("got a signal. exiting.");
    UninitializeZeroconf();
	exit(0);
}

int
main(int argc, char* argv[])
{
	const pds_session_t *pdss;
	char errdesc[1024];
	int i;

#ifdef _WINDOWS
	{
		WSADATA wsad;
		WORD wsav;

		wsav = MAKEWORD(2, 2);
		WSAStartup(wsav, &wsad);
	}
#endif

#ifndef WINCE
	signal (SIGTERM, sighandler);
	signal (SIGINT, sighandler);
#endif

	//TODO: enable -- options (or get rid of them)
	for(i=0;i<argc;i++)
	{
		if((argv[i][0] == '-') && (strlen(argv[i]) == 2))
		{
			switch(argv[i][1])
			{
				case 'p': // Port
					if((i+1)<argc && argv[i+1])
						port = strtol(argv[i+1], NULL, 10);
					break;
				case 'n': // Server name
					if((i+1)<argc && argv[i+1])
						serverName = argv[i+1];
					break;
				case 'h': // Help
					print_help();
					exit(0);
				case 'P': // Password
					if((i+1)<argc && argv[i+1])
						password = argv[i+1];
					break;
				case 'v': // Verbose
					print_debug_messages = TRUE;
					break;
				default:
					printf("Invalid option %s specified.\n\n",argv[i]);
					print_help();
					exit(0);
			}
		}
	}
	

	if (!(pd = pdict_alloc())) {
		DPRINT("pdict_alloc");
		return 1;
	}
	if (pthread_mutex_init(&pd_mutex, NULL) != 0) {
		DPRINT("pthread_mutex_init");
		return 1;
	}
	if (pthread_mutex_init(&pd_mutex_mutex, NULL) != 0) {
		DPRINT("pthread_mutex_init");
		return 1;
	}

	if (!pds_init()) {
		DPRINT("pds_init");
		return 1;
	}

	if (!(pdss = pds_session_alloc(pd, pd_lock, pd_unlock, &pd_mutex, 0,
	    pu_read, 1, pu_write, pu_close,
		pu_log, NULL, errdesc, sizeof (errdesc)))) {
		DPRINT("pds_session_alloc");
		return 1;
	}
	/*pthread_create(&t, NULL, (void *(*)(void *))pds_session_serve,
	    (void *)pdss);*/

	start_phidget((pds_session_t *)pdss);

	if (!stream_server_accept(port, accept_cb, errdesc, sizeof (errdesc))) {
		fprintf(stderr, "stream_server_accept: %s\n", errdesc);
		return 1;
	}
	return EPHIDGET_OK;
}


int add_key(pds_session_t *pdss, const char *key, const char *val)
{
	int res;
	char *null_val = "\001";
	pd_lock(&pd_mutex);
	if(val[0] == '\0')
		res = pdict_add(pd, key, null_val, NULL);
	else
		res = pdict_add(pd, key, val, NULL);
	pd_unlock(&pd_mutex);
	if (!res)
		return EPHIDGET_UNEXPECTED;
	else
		return EPHIDGET_OK;
}

int remove_key(pds_session_t *pdss, const char *key)
{
	char *keypattern = malloc(1024);
	pd_lock(&pd_mutex);
	snprintf(keypattern, 1024, "remove %s",key);
	//TODO: we need to provide a valid write handle in case proccess_line fails
	pds_process_line(pdss, keypattern);
	free(keypattern); keypattern = NULL;
	pd_unlock(&pd_mutex);
		return EPHIDGET_OK;
}

int add_listener(pds_session_t *pdss, const char *kpat, pdl_notify_func_t notfiyme, void *ptr)
{
	int res = 0;
	pd_lock(&pd_mutex);
	res = pdict_add_persistent_change_listener(pd, kpat, notfiyme, ptr);
	pd_unlock(&pd_mutex);
	if (!res)
		return EPHIDGET_UNEXPECTED;
	else
		return EPHIDGET_OK;
}

//Windows CE stuff
#ifdef WINCE
int _tmain(int argc, _TCHAR* argv[])
{
	int i, j;
	char *inputString[20];
	for(i=0;i<argc;i++)
	{
		inputString[i] = (char*)malloc(wcslen(argv[i])+1);
		for(j=0;j<wcslen(argv[i]);j++)
		{
			inputString[i][j] = (char)argv[i][j];
		}
		inputString[i][j]=0;
	}
	main(argc, (char **)inputString);
}
#endif
