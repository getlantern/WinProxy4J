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
    }

}
