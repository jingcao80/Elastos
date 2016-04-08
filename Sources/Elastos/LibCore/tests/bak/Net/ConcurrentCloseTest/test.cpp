
#include "test.h"

// This server accepts connections, but doesn't read or write anything.
// It holds on to the Socket connecting to the client so it won't be GCed.
// Call "close" to close both the server socket and its client connection.
static class SilentServer {
    private final ServerSocket ss;
    private Socket client;

    public SilentServer() throws IOException {
        ss = new ServerSocket(0);
        new Thread(new Runnable() {
            public void run() {
                try {
                    client = ss.accept();
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        }).start();
    }

    public SocketAddress getLocalSocketAddress() {
        return ss.getLocalSocketAddress();
    }

    public void close() throws IOException {
        client.close();
        ss.close();
    }
};

// This thread calls the "close" method on the supplied T after 2s.
// static class Killer<T> extends Thread {
//     private final T s;

//     public Killer(T s) {
//         this.s = s;
//     }

//     public void run() {
//         try {
//             System.err.println("sleep...");
//             Thread.sleep(2000);
//             System.err.println("close...");
//             s.getClass().getMethod("close").invoke(s);
//         } catch (Exception ex) {
//             ex.printStackTrace();
//         }
//     }
// };

int CTest::test_accept(int argc, char* argv[])
{
    // ServerSocket ss = new ServerSocket(0);
    // new Killer(ss).start();
    // try {
    //     System.err.println("accept...");
    //     Socket s = ss.accept();
    //     fail("accept returned " + s + "!");
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // }
    return 0;
}

int CTest::test_connect(int argc, char* argv[])
{
    // StuckServer ss = new StuckServer(false);
    // Socket s = new Socket();
    // new Killer(s).start();
    // try {
    //     System.err.println("connect...");
    //     s.connect(ss.getLocalSocketAddress());
    //     fail("connect returned: " + s + "!");
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // } finally {
    //     ss.close();
    // }
    return 0;
}

int CTest::test_connect_timeout(int argc, char* argv[])
{
    // StuckServer ss = new StuckServer(false);
    // Socket s = new Socket();
    // new Killer(s).start();
    // try {
    //     System.err.println("connect (with timeout)...");
    //     s.connect(ss.getLocalSocketAddress(), 3600 * 1000);
    //     fail("connect returned: " + s + "!");
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // } finally {
    //     ss.close();
    // }
    return 0;
}

int CTest::test_connect_nonBlocking(int argc, char* argv[])
{
    // StuckServer ss = new StuckServer(false);
    // SocketChannel s = SocketChannel.open();
    // new Killer(s.socket()).start();
    // try {
    //     System.err.println("connect (non-blocking)...");
    //     s.configureBlocking(false);
    //     s.connect(ss.getLocalSocketAddress());
    //     while (!s.finishConnect()) {
    //         // Spin like a mad thing!
    //     }
    //     fail("connect returned: " + s + "!");
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // } catch (AsynchronousCloseException alsoOkay) {
    //     // See below.
    // } catch (ClosedChannelException alsoOkay) {
    //     // For now, I'm assuming that we're happy as long as we get any reasonable exception.
    //     // It may be that we're supposed to guarantee only one or the other.
    // } finally {
    //     ss.close();
    // }
    return 0;
}

int CTest::test_read(int argc, char* argv[])
{
    // SilentServer ss = new SilentServer();
    // Socket s = new Socket();
    // s.connect(ss.getLocalSocketAddress());
    // new Killer(s).start();
    // try {
    //     System.err.println("read...");
    //     int i = s.getInputStream().read();
    //     fail("read returned: " + i);
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // }
    // ss.close();
    return 0;
}

int CTest::test_read_multiple(int argc, char* argv[])
{
    // SilentServer ss = new SilentServer();
    // final Socket s = new Socket();
    // s.connect(ss.getLocalSocketAddress());

    // // We want to test that we unblock *all* the threads blocked on a socket, not just one.
    // // We know the implementation uses the same mechanism for all blocking calls, so we just
    // // test read(2) because it's the easiest to test. (recv(2), for example, is only accessible
    // // from Java via a synchronized method.)
    // final ArrayList<Thread> threads = new ArrayList<Thread>();
    // final List<Throwable> thrownExceptions = new CopyOnWriteArrayList<Throwable>();
    // for (int i = 0; i < 10; ++i) {
    //     Thread t = new Thread(new Runnable() {
    //         public void run() {
    //             try {
    //                 try {
    //                     System.err.println("read...");
    //                     int i = s.getInputStream().read();
    //                     fail("read returned: " + i);
    //                 } catch (SocketException expected) {
    //                     assertEquals("Socket closed", expected.getMessage());
    //                 }
    //             } catch (Throwable ex) {
    //                 thrownExceptions.add(ex);
    //             }
    //         }
    //     });
    //     threads.add(t);
    // }
    // for (Thread t : threads) {
    //     t.start();
    // }
    // new Killer(s).start();
    // for (Thread t : threads) {
    //     t.join();
    // }
    // for (Throwable exception : thrownExceptions) {
    //     throw exception;
    // }

    // ss.close();
    return 0;
}

int CTest::test_recv(int argc, char* argv[])
{
    // DatagramSocket s = new DatagramSocket();
    // byte[] buf = new byte[200];
    // DatagramPacket p = new DatagramPacket(buf, 200);
    // new Killer(s).start();
    // try {
    //     System.err.println("receive...");
    //     s.receive(p);
    //     fail("receive returned!");
    // } catch (SocketException expected) {
    //     assertEquals("Socket closed", expected.getMessage());
    // }
    return 0;
}

int CTest::test_write(int argc, char* argv[])
{
    // final SilentServer ss = new SilentServer();
    // Socket s = new Socket();
    // s.connect(ss.getLocalSocketAddress());
    // new Killer(s).start();
    // try {
    //     System.err.println("write...");
    //     // We just keep writing here until all the buffers are full and we block,
    //     // waiting for the server to read (which it never will). If the asynchronous close
    //     // fails, we'll see a test timeout here.
    //     while (true) {
    //         byte[] buf = new byte[256*1024];
    //         s.getOutputStream().write(buf);
    //     }
    // } catch (SocketException expected) {
    //     // We throw "Connection reset by peer", which I don't _think_ is a problem.
    //     // assertEquals("Socket closed", expected.getMessage());
    // }
    // ss.close();
    return 0;
}
