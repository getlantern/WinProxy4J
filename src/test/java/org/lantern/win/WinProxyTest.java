package org.lantern.win;

import static org.junit.Assert.*;

import org.junit.Test;

public class WinProxyTest {

    @Test
    public void testProxy() throws Exception {
        final WinProxy proxifier = new WinProxy();
        final String testStr = "127.7.7.7:8888";
        
        //System.out.println("PROXYING!!");
        proxifier.proxy(testStr);
        
        
        //System.out.println("DONE PROXYING");
        String proxy = proxifier.getProxy();
        //System.out.println(proxy);
        assertEquals(testStr, proxy);
        proxifier.unproxy();
        proxy = proxifier.getProxy();
        assertEquals("", proxy);
    }

}
