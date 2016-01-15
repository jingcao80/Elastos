/*
 * ~/libcore/luni/src/test/java/libcore/java/io/InterruptedStreamTest.java
 */

#include <stdarg.h>

#include <elautoptr.h>
#include <elastos/coredef.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

#include "Thread.h"

using namespace Elastos;
using namespace Elastos::IO;
using namespace Elastos::Core;
using namespace Elastos::Utility::Zip;
using namespace Elastosx::Crypto;
using namespace Elastos::Net;

using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Zip::CCRC32;
using Elastos::Core::IAppendable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileDescriptor;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Elastos::IO::IInputStream;
using Elastos::Utility::Zip::ICheckedInputStream;
using Elastos::Utility::Zip::CCheckedInputStream;
using Elastos::Net::ISocket;
using Elastos::Net::CSocket;
using Elastos::IO::Channels::IServerSocketChannel;
using Elastos::IO::Channels::IServerSocketChannelHelper;
using Elastos::IO::Channels::ISocketChannelHelper;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IServerSocket;
using Elastos::IO::Channels::ISocketChannel;

namespace Elastos {
namespace HelloCar {

class MyThread : public Thread
{
    AutoPtr<IThread> mythd;
public:
    MyThread(IThread *toInterrupt);
    Run();
    ~MyThread();
};

static IThread *interruptMeLater();


static void assertEquals(const char *info, Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d. %s\n", aspect, test, info);
    assert(aspect == test);
}

static void assertEquals(const char *info, String aspect, String test)
{
    printf("aspect: %s, test: %s. %s\n", aspect.string(), test.string(), info);
    assert(aspect.Equals(test) == 0);
}

#if 0
/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package libcore.java.io;

import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PipedReader;
import java.io.PipedWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.ClosedByInterruptException;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.Pipe;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.WritableByteChannel;
import junit.framework.TestCase;

/**
 * Test that interrupting a thread blocked on I/O causes that thread to throw
 * an InterruptedIOException.
 */
public final class InterruptedStreamTest extends TestCase {

    private static final int BUFFER_SIZE = 1024 * 1024;

    private Socket[] sockets;

    @Override protected void setUp() throws Exception {
        // Clear the interrupted bit to make sure an earlier test did
        // not leave us in a bad state.
        Thread.interrupted();
        super.tearDown();
    }
#endif


#if 0
    @Override protected void tearDown() throws Exception {
        if (sockets != null) {
            sockets[0].close();
            sockets[1].close();
        }
        Thread.interrupted(); // clear interrupted bit
        super.tearDown();
    }

    public void testInterruptPipedInputStream() throws Exception {
        PipedOutputStream out = new PipedOutputStream();
        PipedInputStream in = new PipedInputStream(out);
        testInterruptInputStream(in);
    }

    public void testInterruptPipedOutputStream() throws Exception {
        PipedOutputStream out = new PipedOutputStream();
        new PipedInputStream(out);
        testInterruptOutputStream(out);
    }

    public void testInterruptPipedReader() throws Exception {
        PipedWriter writer = new PipedWriter();
        PipedReader reader = new PipedReader(writer);
        testInterruptReader(reader);
    }

    public void testInterruptPipedWriter() throws Exception {
        final PipedWriter writer = new PipedWriter();
        new PipedReader(writer);
        testInterruptWriter(writer);
    }

    public void testInterruptReadablePipeChannel() throws Exception {
        testInterruptReadableChannel(Pipe.open().source());
    }

    public void testInterruptWritablePipeChannel() throws Exception {
        testInterruptWritableChannel(Pipe.open().sink());
    }

    public void testInterruptReadableSocketChannel() throws Exception {
        sockets = newSocketChannelPair();
        testInterruptReadableChannel(sockets[0].getChannel());
    }

    public void testInterruptWritableSocketChannel() throws Exception {
        sockets = newSocketChannelPair();
        testInterruptWritableChannel(sockets[0].getChannel());
    }
#endif


#if 0
    /**
     * Returns a pair of connected sockets backed by NIO socket channels.
     */
    private Socket[] newSocketChannelPair() throws IOException {
        ServerSocketChannel serverSocketChannel = ServerSocketChannel.open();
        serverSocketChannel.socket().bind(new InetSocketAddress(0));
        SocketChannel clientSocketChannel = SocketChannel.open();
        clientSocketChannel.connect(serverSocketChannel.socket().getLocalSocketAddress());
        SocketChannel server = serverSocketChannel.accept();
        serverSocketChannel.close();
        return new Socket[] { clientSocketChannel.socket(), server.socket() };
    }
#endif
static ArrayOf<ISocket> *newSocketChannelPair()
{
    ECode ec;
    AutoPtr<IServerSocketChannelHelper> issch;
    AutoPtr<IServerSocketChannel> serverSocketChannel;
    AutoPtr<IServerSocket> serverSocket;
    AutoPtr<IInetSocketAddress> isa;

/*
    CServerSocketChannelHelper::AcquireSingleton((IServerSocketChannelHelper **)&issch);

    issch->Open((IServerSocketChannel **)&serverSocketChannel);
    serverSocketChannel->GetSocket((IServerSocket **)&serverSocket);

    CInetSocketAddress::New(0, (IInetSocketAddress **)&isa);

    AutoPtr<ISocketAddress> localAddr;

    localAddr = ISocketAddress::Probe((IInetSocketAddress *)isa);
    serverSocket->Bind((ISocketAddress *)localAddr);

    AutoPtr<ISocketChannelHelper> isch;
    AutoPtr<ISocketChannel> clientSocketChannel;
    AutoPtr<ISocketAddress> address;
    Boolean result;

    CSocketChannelHelper::AcquireSingleton((ISocketChannelHelper **)&isch);
    isch->Open((ISocketChannel **)&clientSocketChannel);

    serverSocket->GetLocalSocketAddress((ISocketAddress **)&address);
    clientSocketChannel->Connect((ISocketAddress *)address, &result);

    AutoPtr<ISocketChannel> server;
    serverSocketChannel->Accept((ISocketChannel **)&server);

    serverSocketChannel->Close();

    AutoPtr<ISocket> socket1;
    AutoPtr<ISocket> socket2;

    ArrayOf<ISocket> ret;
    ret = ArrayOf<ISocket>::Alloc(2);

    clientSocketChannel->GetSocket((ISocket **)&socket1);
    server->GetSocket((ISocket **)&socket2);

    (*ret)[0] = socket1;
    (*ret)[1] = socket2;

    return ret;
*/
}

#if 0
    private void testInterruptInputStream(final InputStream in) throws Exception {
        Thread thread = interruptMeLater();
        try {
            in.read();
            fail();
        } catch (InterruptedIOException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private void testInterruptReader(final PipedReader reader) throws Exception {
        Thread thread = interruptMeLater();
        try {
            reader.read();
            fail();
        } catch (InterruptedIOException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private void testInterruptReadableChannel(final ReadableByteChannel channel) throws Exception {
        Thread thread = interruptMeLater();
        try {
            channel.read(ByteBuffer.allocate(BUFFER_SIZE));
            fail();
        } catch (ClosedByInterruptException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private void testInterruptOutputStream(final OutputStream out) throws Exception {
        Thread thread = interruptMeLater();
        try {
            // this will block when the receiving buffer fills up
            while (true) {
                out.write(new byte[BUFFER_SIZE]);
            }
        } catch (InterruptedIOException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private void testInterruptWriter(final PipedWriter writer) throws Exception {
        Thread thread = interruptMeLater();
        try {
            // this will block when the receiving buffer fills up
            while (true) {
                writer.write(new char[BUFFER_SIZE]);
            }
        } catch (InterruptedIOException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private void testInterruptWritableChannel(final WritableByteChannel channel) throws Exception {
        Thread thread = interruptMeLater();
        try {
            // this will block when the receiving buffer fills up
            while (true) {
                channel.write(ByteBuffer.allocate(BUFFER_SIZE));
            }
        } catch (ClosedByInterruptException expected) {
        } catch (ClosedChannelException expected) {
        } finally {
            confirmInterrupted(thread);
        }
    }

    private Thread interruptMeLater() throws Exception {
        final Thread toInterrupt = Thread.currentThread();
        Thread thread = new Thread(new Runnable () {
            @Override public void run() {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ex) {
                }
                toInterrupt.interrupt();
            }
        });
        thread.start();
        return thread;
    }

    private static void confirmInterrupted(Thread thread) throws InterruptedException {
        // validate and clear interrupted bit before join
        try {
            assertTrue(Thread.interrupted());
        } finally {
            thread.join();
        }
    }
}
#endif

static IThread *interruptMeLater()
{

    AutoPtr<IThread> toInterrupt = Thread::GetCurrentThread();
    IThread *thread = new MyThread(toInterrupt);

    thread->Start();

    REFCOUNT_ADD(thread)
    return thread;
}


MyThread::MyThread(IThread *toInterrupt)
{
    mythd = toInterrupt;
    REFCOUNT_ADD(mythd);

    Thread::constructor();
}

MyThread::~MyThread()
{
    mythd->Release();
}

ECode MyThread::Run()
{
    printf("MyThread::Run\n");
    Thread::Sleep(1000);
    mythd->Interrupt();

    return Thread::Run();
}



//==============================================================================

int mainInterruptedStreamTest(int argc, char *argv[])
{
    printf("\n==== libcore/math/InterruptedStreamTest ====\n");
    printf("\n==== end of libcore/math/InterruptedStreamTest ====\n");

    return 0;
}

}
}
