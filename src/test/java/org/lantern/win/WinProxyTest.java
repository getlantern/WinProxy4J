package org.lantern.win;

import org.junit.Test;

public class WinProxyTest {

    @Test
    public void testProxy() throws Exception {
        final WinProxy proxifier = new WinProxy();
        proxifier.proxy("127.7.7.7:8888");
        proxifier.unproxy();
        
    }

}
