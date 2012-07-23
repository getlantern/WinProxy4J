#include <jni.h>
#include "org_lantern_win_WindowsProxifier.h"
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

JNIEXPORT jboolean JNICALL Java_org_lantern_win_WindowsProxifier_proxy(JNIEnv * env, jobject jobj, jstring proxy) {
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

int main(int argc, char *argv[])
{
	 //SetConnectionProxy("127.1.1.1:9999", NULL);
}


/*
LPWSTR GetSzwStringCharsFromHeap(JNIEnv* env, HANDLE hHeap, jstring jstr) {
	LPWSTR lpwResult = NULL;
	jsize jStrLen;

	if (jstr==NULL)
		goto finished;
	jStrLen= env->GetStringLength(jstr);

    lpwResult = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, (jStrLen+1)*sizeof(WCHAR));
	if (lpwResult==NULL) {
		fireJavaExceptionForSystemErrorCode(env, GetLastError());
		goto finished;
	}
    env->GetStringRegion(jstr, 0L, jStrLen, lpwResult);

    finished:
        return lpwResult;
}
*/

//(Callers should use (*env)->ExceptionCheck(env) to see if this function actually succeeded).