package org.lantern.win;

public class WindowsProxifier {
    
    public WindowsProxifier() {
        System.loadLibrary("JniWindowsProxy");
    }

    public native boolean proxy(final String endpoint);
    
    public native boolean unproxy(final String endpoint);
}
