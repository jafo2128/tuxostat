// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX
#define __STDAFX

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#ifdef WINCE
	int errno;
	#define strerror(err) "strerror() Not Supported in Windows CE"
	#define abort() exit(1)
	#define EAGAIN 35
#else
	#include <sys/types.h>
	#include <errno.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef _WINDOWS
// Defines & Include for Windows only

	//use zeroconf
	#ifndef USE_ZEROCONF
	#define USE_ZEROCONF
	#endif
	//use runtime linking for zeroconf
	#ifndef ZEROCONF_RUNTIME_LINKING
	#define ZEROCONF_RUNTIME_LINKING
	#endif

	// Modify the following defines if you have to target a platform prior to the ones specified below.
	// Refer to MSDN for the latest info on corresponding values for different platforms.
	#ifndef WINVER				// Allow use of features specific to Windows XP or later.
	#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
	#endif

	#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
	#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
	#endif						

	#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
	#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
	#endif

	#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
	#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
	#endif

	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#ifndef _CYGWIN
		#include <winsock2.h>
		#include <ws2tcpip.h>
	#endif
	#if !defined(WINCE) && defined(_MSC_EXTENSIONS)
		#include <Wspiapi.h>
	#endif

	#include <windows.h>
	#include <winbase.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <assert.h>
	#include <math.h>

	#ifdef _MSC_EXTENSIONS
		#include "snprintf.h"
	#endif
	#ifndef _CYGWIN
		#include "wincompat.h"
	#endif

	#define strdup _strdup

	#define CCONV __stdcall
	#define SLEEP(dlay) Sleep(dlay);
	#define ZEROMEM(var,size) ZeroMemory(var, size);

	#ifdef PHIDGET21_EXPORTS
	#ifndef PHIDGET21_API
	#define PHIDGET21_API 
	#endif
	#else
	#ifndef PHIDGET21_API
	#define PHIDGET21_API __declspec(dllimport)
	#endif
	#endif
	
	typedef SYSTEMTIME TIME;
	typedef unsigned __int64 __uint64;

	#ifndef DEPRECATE_WIN
	#define DEPRECATE_WIN(depmsg) __declspec(deprecated( depmsg ))
	#endif
	#ifndef DEPRECATE_GCC
	#define DEPRECATE_GCC
	#endif
	
#else
// Defines & Include for both Mac and Linux
	#include <semaphore.h>
	#include <time.h>
	#include <stdio.h>
	#include <stdarg.h>
	#include <stdlib.h>
	#include <string.h>
	#include <pthread.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <ctype.h>
	#include <math.h>

	#define CCONV
	#define SLEEP(dlay) usleep(dlay*1000);
	#define ZEROMEM(var,size) memset(var, 0, size);
	#ifndef PHIDGET21_API
	#define PHIDGET21_API
	#endif
	typedef void *HANDLE;
	#define INFINITE 0xFFFFFFFF
	typedef long *LPDWORD;
	#define STILL_ACTIVE 0x00000103L
	#define WAIT_TIMEOUT 258L
	#define WAIT_ABANDONED 0x00000080
	#define WAIT_OBJECT_0 0x00000000L
	#define WAIT_FAILED 0xFFFFFFFF
	typedef void *OVERLAPPED;
	//typedef long long __int64;
	typedef unsigned long long __uint64;
	
	typedef struct timeval TIME;

	#ifndef DEPRECATE_GCC
	#define DEPRECATE_GCC __attribute__ ((deprecated))
	#endif
	#ifndef DEPRECATE_WIN
	#define DEPRECATE_WIN(depmsg)
	#endif
	
	#ifdef _MACOSX
		// Defines & Include for Mac only
		#include <CoreFoundation/CoreFoundation.h>
		#include <IOKit/IOKitLib.h>
		#include <IOKit/IOCFPlugIn.h>
		#include <IOKit/usb/IOUSBLib.h>
		#include <mach/mach.h>

		//use zeroconf
		#ifndef USE_ZEROCONF
		#define USE_ZEROCONF
		#endif

	#elif _LINUX
		// Defines & Include for Linux Only
		#include "libusb/usb.h"
		#include <assert.h>

		//use zeroconf
		#ifndef USE_ZEROCONF
		#define USE_ZEROCONF
		#endif
		//use runtime linking for zeroconf
		#ifndef ZEROCONF_RUNTIME_LINKING
		#define ZEROCONF_RUNTIME_LINKING
		#endif
	#endif
#endif

#ifndef round
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif

#define round_double(x, decimals) (double)((double)round(x * (double)(pow(10, decimals))) / (double)(pow(10, decimals)))

#include "clog.h"
#include "pdictclient.h"
#include "cphidget.h"

#endif


