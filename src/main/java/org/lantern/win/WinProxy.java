package org.lantern.win;

import java.io.File;

public class WinProxy {
    
    public WinProxy() {
        final String name = "winproxy4j";
        final File lib = new File(name + ".dll");
        if (!lib.isFile()) {
            throw new RuntimeException("Missing library at "+
                lib.getAbsolutePath());
        }
        System.loadLibrary(name);
    }

    public native boolean proxy(final String endpoint);
    
    public native boolean unproxy();
    
    public native String getProxy();
}
