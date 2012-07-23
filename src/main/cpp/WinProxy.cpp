#include "WinProxy.h"
#include <Windows.h>
#include <WinInet.h>

wchar_t* JavaToWSZ(JNIEnv* env, jstring string) {
    if (string == NULL)
        return NULL;
    int len = env->GetStringLength(string);
    const jchar* raw = env->GetStringChars(string, NULL);
    if (raw == NULL)
        return NULL;

    wchar_t* wsz = new wchar_t[len+1];
    memcpy(wsz, raw, len*2);
    wsz[len] = 0;

    env->ReleaseStringChars(string, raw);

    return wsz;
}

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_unproxy(JNIEnv * env, jobject jobj) {
	jstring blank = env->NewStringUTF("");
	return Java_org_lantern_win_WinProxy_proxy(env, jobj, blank);
}

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WinProxy_proxy(JNIEnv * env, jobject jobj, jstring proxy) {
	wchar_t* proxyAdressStr = JavaToWSZ(env, proxy);
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
    conn_options.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT|PROXY_TYPE_PROXY;  
   
    conn_options.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;  

	//char text[] = proxyAdressStr;
	//wchar_t wtext[20];
    //mbstowcs(wtext, proxyAdressStr, strlen(proxyAdressStr));
    ///LPWSTR ptr = TEXT("127.1.1.1:9999");
	//LPSTR ptr = proxyAdressStr;
	//LPWSTR ptr = TEXT(proxyAdressStr);
    conn_options.pOptions[1].Value.pszValue = proxyAdressStr;  
    conn_options.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;

	LPWSTR loc = TEXT("local");
    conn_options.pOptions[2].Value.pszValue = loc;    
      
    bReturn = InternetSetOption(NULL,INTERNET_OPTION_PER_CONNECTION_OPTION, &conn_options, dwBufferSize);  
      
    delete [] conn_options.pOptions;  
      
    InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);  
    InternetSetOption(NULL, INTERNET_OPTION_REFRESH , NULL, 0);  
    return bReturn;  
} 
