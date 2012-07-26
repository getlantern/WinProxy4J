package org.lantern.win;

import java.io.Closeable;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Class that wraps native calls to wininet.dll InternetSetOption and 
 * InternetQueryOption functions to modifying proxy settings. This is 
 * necessary to ensure other programs receive notifications of proxy setting
 * changes, which direct modifications to the registry will not accomplish.
 */
public class WinProxy {
    
    public WinProxy() {
        this(new File("."));
    }
    
    public WinProxy(final File libDir) {
        final File lib = new File(libDir, "winproxy4j.dll");
        //if (!lib.isFile()) {
        // We just copy it every time since it's a small file and this 
        // allows us not to worry about versions.
            copyFromJar(lib);
            if (!lib.isFile()) {
                throw new RuntimeException("Missing library at "+
                    lib.getAbsolutePath());
            }
        //}
        System.load(lib.getAbsolutePath());
        //System.loadLibrary(name);
    }

    public native boolean proxy(final String endpoint);
    
    public native boolean unproxy();
    
    public native String getProxy();
    

    private void copyFromJar(final File dest) {
        InputStream is = null;
        OutputStream os = null;
        try {
            is = getClass().getClassLoader().getResourceAsStream(dest.getName());
            os = new FileOutputStream(dest);
            copy(is, os);
        } catch (final IOException e) {
        } finally {
            closeQuietly(is);
            closeQuietly(os);
        }
        
    }
    
    private int copy(final InputStream is, final OutputStream os) 
        throws IOException {
        if (is == null) {
            throw new NullPointerException("null input stream.");
        }
        if (os == null) {
            throw new NullPointerException("null output stream.");
        }
        final byte[] buffer = new byte[4096];
        int count = 0;
        int n = 0;
        while (-1 != (n = is.read(buffer))) {
            os.write(buffer, 0, n);
            count += n;
        }
        return count;
    }

    private void closeQuietly(final Closeable stream) {
        if (stream != null) {
            try {
                stream.close();
            } catch (final IOException e) {
            }
        }
    }
}
