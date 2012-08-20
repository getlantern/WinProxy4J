#include "WinProxy.h"
#include <Windows.h>
#include <wininet.h>
#include <iostream>
#include <ras.h>
#include <tchar.h>
#pragma comment( lib, "wininet" )
#pragma comment( lib, "rasapi32" )

/*
 * NDEBUG is not defined for release builds.
 */
#ifndef NDEBUG
#define log_debug(s)                       \
    do {                                                        \
        if (1) \
        {                      \
            std::ostringstream oss; \
            oss << s; \
            std::cout << oss.str() << std::endl;                \
        } \
    } while (0)
#else
    #define log_debug(s) /* */
#endif


jstring lastError(JNIEnv * env) {
    char msg[120];
    int lastError = GetLastError();
    sprintf_s(msg, "Last Error: %d", lastError);
    return env->NewStringUTF(msg);
}

//// Options used in INTERNET_PER_CONN_OPTON struct
//#define INTERNET_PER_CONN_FLAGS                         1
//#define INTERNET_PER_CONN_PROXY_SERVER                  2
//#define INTERNET_PER_CONN_PROXY_BYPASS                  3
//#define INTERNET_PER_CONN_AUTOCONFIG_URL                4
//#define INTERNET_PER_CONN_AUTODISCOVERY_FLAGS           5
//etc.
//// PER_CONN_FLAGS
//#define PROXY_TYPE_DIRECT                               0x00000001   // direct to net
//#define PROXY_TYPE_PROXY                                0x00000002   // via named proxy
//#define PROXY_TYPE_AUTO_PROXY_URL                       0x00000004   // autoproxy URL
//#define PROXY_TYPE_AUTO_DETECT                          0x00000008   // use autoproxy detection

// Figure out which Dial-Up or VPN connection is active; in a normal LAN connection, this should
// return NULL
LPTSTR FindActiveConnection() {
    DWORD dwCb = sizeof(RASCONN);
    DWORD dwErr = ERROR_SUCCESS;
    DWORD dwRetries = 5;
    DWORD dwConnections = 0;
    RASCONN* lpRasConn = NULL;
    RASCONNSTATUS rasconnstatus;
    rasconnstatus.dwSize = sizeof(RASCONNSTATUS);

    //
    // Loop through in case the information from RAS changes between calls.
    //
    while (dwRetries--) {
        // If the memory is allocated, free it.
        if (NULL != lpRasConn) {
            HeapFree(GetProcessHeap(), 0, lpRasConn);
            lpRasConn = NULL;
        }

        // Allocate the size needed for the RAS structure.
        lpRasConn = (RASCONN*)HeapAlloc(GetProcessHeap(), 0, dwCb);
        if (NULL == lpRasConn) {
            dwErr = ERROR_NOT_ENOUGH_MEMORY;
            break;
        }

        // Set the structure size for version checking purposes.
        lpRasConn->dwSize = sizeof(RASCONN);

        // Call the RAS API then exit the loop if we are successful or an unknown
        // error occurs.
        dwErr = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);
        if (ERROR_INSUFFICIENT_BUFFER != dwErr) {
            break;
        }
    }
    //
    // In the success case, print the names of the connections.
    //
    if (ERROR_SUCCESS == dwErr) {
        DWORD i;
        for (i = 0; i < dwConnections; i++) {
            RasGetConnectStatus(lpRasConn[i].hrasconn, &rasconnstatus);
            if (rasconnstatus.rasconnstate == RASCS_Connected){
                return lpRasConn[i].szEntryName;
            }

        }
    }
    return NULL; // Couldn't find an active dial-up/VPN connection; return NULL
}

char* jstringToChar(JNIEnv* env, const jstring& proxyStr) {
	// We have to copy the array to a non-const char* here to pass it to InternetSetOptions.
	const char* raw = env->GetStringUTFChars(proxyStr, NULL);
    if (raw == NULL) {
        return NULL; // OutOfMemoryError already thrown
	}
	const int len = env->GetStringUTFLength(proxyStr);
	char* proxyAddressStr = new char[len+1];
	memcpy (&proxyAddressStr[0], raw, len) ;
	proxyAddressStr[len] = 0;

	log_debug("Copied proxy str: " << proxyAddressStr);
	env->ReleaseStringUTFChars(proxyStr, raw);
	return proxyAddressStr;
}

jboolean proxy(JNIEnv* env, const jobject& jobj, const jstring& proxyStr, const int flags) {
	char* proxyAddressStr = jstringToChar(env, proxyStr);

	INTERNET_PER_CONN_OPTION_LIST options;  
    BOOL    bReturn;  
    DWORD   dwBufferSize = sizeof(options);          
    options.dwSize = dwBufferSize;  
    options.pszConnection = NULL;//connNameStr;//NULL == LAN  
      
    options.dwOptionCount = 3;  
    options.pOptions = new INTERNET_PER_CONN_OPTION[3];  
   
    if(!options.pOptions)    
        return FALSE;     
      
    options.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;  
    options.pOptions[0].Value.dwValue = flags;  
   
    options.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;  

    options.pOptions[1].Value.pszValue = proxyAddressStr;  
    options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;

	LPSTR loc = TEXT("local");
    options.pOptions[2].Value.pszValue = loc;    
      
    bReturn = InternetSetOption(NULL,INTERNET_OPTION_PER_CONNECTION_OPTION, &options, dwBufferSize);  
      
    delete [] options.pOptions;  
      
    InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);  
    InternetSetOption(NULL, INTERNET_OPTION_REFRESH , NULL, 0);  
	delete [] proxyAddressStr;
    return bReturn;  
} 

jboolean setPacFile(char* proxyAddressStr, const int flags) {
	INTERNET_PER_CONN_OPTION_LIST options;  
    BOOL    bReturn;  
    DWORD   dwBufferSize = sizeof(options);          
    options.dwSize = dwBufferSize;  
    options.pszConnection = FindActiveConnection();//connNameStr;//NULL == LAN  
      
    options.dwOptionCount = 2;  
    options.pOptions = new INTERNET_PER_CONN_OPTION[2];  
   
    if(!options.pOptions) {    
        return FALSE;     
	}
      
    options.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;  
    options.pOptions[0].Value.dwValue = flags;  
   
    options.pOptions[1].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;  

    options.pOptions[1].Value.pszValue = proxyAddressStr;  
    //options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
    //options.pOptions[2].Value.pszValue = TEXT("local");    
      
    bReturn = InternetSetOption(NULL,INTERNET_OPTION_PER_CONNECTION_OPTION, &options, dwBufferSize);  
      
    delete [] options.pOptions;  
      
    InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);  
    InternetSetOption(NULL, INTERNET_OPTION_REFRESH , NULL, 0);  
	
    return bReturn;  
} 

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_unproxy(JNIEnv * env, jobject jobj) {
	jstring blank = env->NewStringUTF("");
	return proxy(env, jobj, blank, PROXY_TYPE_DIRECT);
}

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_proxy(JNIEnv * env, jobject jobj, jstring proxyStr) {
	return proxy(env, jobj, proxyStr, PROXY_TYPE_DIRECT|PROXY_TYPE_PROXY);
}

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_setPacFile(JNIEnv * env, jobject jobj, jstring proxyStr) {
	log_debug("Setting pac file");
	char* proxyAddressStr = jstringToChar(env, proxyStr);

	if (sizeof(proxyAddressStr) == 0) {
		proxyAddressStr = NULL;
	}
	const jboolean returnVal = setPacFile(proxyAddressStr, PROXY_TYPE_AUTO_PROXY_URL);
	delete [] proxyAddressStr;
	return returnVal;
}

jstring getProxyOption(JNIEnv* env, const jobject& jobj, const int flags) {
	log_debug("In getProxy");
    INTERNET_PER_CONN_OPTION_LIST    list;
    INTERNET_PER_CONN_OPTION         options[1];
    unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

    options[0].dwOption = flags;
    list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    list.pszConnection = NULL;
    list.dwOptionCount = 1;
    list.dwOptionError = 0;
    list.pOptions = options;

    if(!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, &nSize)) {
		return lastError(env);
	}

    if (options[0].Value.pszValue != NULL) {
        const jstring ret = env->NewStringUTF(options[0].Value.pszValue);
        GlobalFree(options[0].Value.pszValue);

		log_debug("Returning actual value");
		return ret;
    }
	log_debug("Returning new string utf");
    return env->NewStringUTF("");
}

JNIEXPORT jstring JNICALL Java_org_lantern_win_WinProxy_getProxy(JNIEnv * env, jobject jobj) {
	return getProxyOption(env, jobj, INTERNET_PER_CONN_PROXY_SERVER);
}

JNIEXPORT jstring JNICALL Java_org_lantern_win_WinProxy_getPacFile(JNIEnv * env, jobject jobj) {
	return getProxyOption(env, jobj, INTERNET_PER_CONN_AUTOCONFIG_URL);
}

