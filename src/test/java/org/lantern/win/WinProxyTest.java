package org.lantern.win;

import static org.junit.Assert.*;

import org.junit.Test;

public class WinProxyTest {

    @Test
    public void testProxy() throws Exception {
        if (!System.getProperty("os.name").contains("indows")) {
            return;
        }
        final WinProxy proxifier = new WinProxy();
        final String testStr = "127.7.7.7:8888";
        
        proxifier.proxy(testStr);
        String proxy = proxifier.getProxy();
        assertEquals(testStr, proxy);
        proxifier.unproxy();
        proxy = proxifier.getProxy();
        assertEquals("", proxy);
        
        final String testPacName = "file://c:/proxy_on.pac";
        proxifier.setPacFile(testPacName);
        
        final String file = proxifier.getPacFile();
        assertEquals(testPacName, file);
        
        proxifier.noPacFile();
        
        //proxifier.setPacFile("");
        
        final String noPac = proxifier.getPacFile();
        assertEquals("", noPac);
    }

}
