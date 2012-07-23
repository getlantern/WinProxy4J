#include "WinProxy.h"
#include <Windows.h>
#include <WinInet.h>
#include <iostream>

jstring lastError(JNIEnv * env) {
    char msg[120];
    int lastError = GetLastError();
    sprintf_s(msg, "Last Error: %d", lastError);
    return env->NewStringUTF(msg);
}

jboolean proxy(JNIEnv* env, const jobject& jobj, const jstring& proxyStr, const int flags) {

	// We have to copy the array to a non-const char* here to pass it to InternetSetOptions.
	const char* raw = env->GetStringUTFChars(proxyStr, NULL);
    if (raw == NULL) {
        return NULL; // OutOfMemoryError already thrown
	}
	const int len = env->GetStringUTFLength(proxyStr);
	char* proxyAddressStr = new char[len+1];
	memcpy (&proxyAddressStr[0], raw, len) ;
	proxyAddressStr[len] = 0;

	//printf("Copied proxy str! (%s)\n", proxyAddressStr);

	INTERNET_PER_CONN_OPTION_LIST conn_options;  
    BOOL    bReturn;  
    DWORD   dwBufferSize = sizeof(conn_options);          
    conn_options.dwSize = dwBufferSize;  
    conn_options.pszConnection = NULL;//connNameStr;//NULL == LAN  
      
    conn_options.dwOptionCount = 3;  
    conn_options.pOptions = new INTERNET_PER_CONN_OPTION[3];  
   
    if(!conn_options.pOptions)    
        return FALSE;     
      
    conn_options.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;  
    conn_options.pOptions[0].Value.dwValue = flags;  
   
    conn_options.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;  

    conn_options.pOptions[1].Value.pszValue = proxyAddressStr;  
    conn_options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;

	LPSTR loc = TEXT("local");
    conn_options.pOptions[2].Value.pszValue = loc;    
      
    bReturn = InternetSetOption(NULL,INTERNET_OPTION_PER_CONNECTION_OPTION, &conn_options, dwBufferSize);  
      
    delete [] conn_options.pOptions;  
      
    InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);  
    InternetSetOption(NULL, INTERNET_OPTION_REFRESH , NULL, 0);  

	env->ReleaseStringUTFChars(proxyStr, raw);
	delete [] proxyAddressStr;
    return bReturn;  
} 

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_unproxy(JNIEnv * env, jobject jobj) {
	jstring blank = env->NewStringUTF("");
	return proxy(env, jobj, blank, PROXY_TYPE_DIRECT);
}

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_proxy(JNIEnv * env, jobject jobj, jstring proxyStr) {
	return proxy(env, jobj, proxyStr, PROXY_TYPE_DIRECT|PROXY_TYPE_PROXY);
}

JNIEXPORT jstring JNICALL Java_org_lantern_win_WinProxy_getProxy(JNIEnv * env, jobject jobj) {

	//std::cout << "In getProxy" << std::endl;
    INTERNET_PER_CONN_OPTION_LIST    list;
    INTERNET_PER_CONN_OPTION         options[1];
    unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

    options[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
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

		//std::cout << "Returning actual value" << std::endl;
		return ret;
    }
	//std::cout << "Returning new string utf" << std::endl;
    return env->NewStringUTF("");
}

