
#ifndef __ELASTOS_DROID_SERVER_REMOTE_SERVERSOCKETTIMER_H__
#define __ELASTOS_DROID_SERVER_REMOTE_SERVERSOCKETTIMER_H__

#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::IO::IFile;
using Elastos::IO::IOutputStream;
using Elastos::Net::ISocket;
using Elastos::Net::IServerSocket;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::HandlerBase;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class ServerSocketTimer : public ThreadBase
{
private:
    /********************************************************
     * class TimerThread is used to tick thread alive time
     ********************************************************/
    class TimerThread : public HandlerThread
    {
    private:
        class TimerThreadHandler : public HandlerBase
        {
        public:
            TO_STRING_IMPL("ServerSocketTimer::TimerThreadHandler")

            TimerThreadHandler(
                /* [in] */ ILooper* looper,
                /* [in] */ TimerThread* host);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            TimerThread* mHost;
        };

    public:
        TimerThread(
            /* [in] */ const String& name,
            /* [in] */ ServerSocketTimer* host);

        /**
         * Callback fired when the time is up.
         */
        CARAPI_(void) OnFinish();

        /**
         * Start the countdown.
         */
        CARAPI_(void) StartCountDown();

        /** reset  mStopTimeInFuture */
        CARAPI_(void) ResetTimer();

    private:
        static const Int32 MSG;

        /** Millis since epoch when alarm should stop. */
        const Int64 mMillisInFuture;//20mins

        /** The interval in millis that the user receives callbacks */
        const Int64 mCountdownInterval;//7mins

        Int64 mStopTimeInFuture;

        /** handles timer counting down */
        AutoPtr<IHandler> mHandler;

        Object mLock;
        ServerSocketTimer* mHost;
    };

public:
    /********************************************************
     * class constructor
     ********************************************************/
    ServerSocketTimer(
        /* [in] */ IContext* context);

    CARAPI_(void) ServerSocketTimerFinish();

    CARAPI Run();

    CARAPI_(Int32) GetSocketPort();

private:
    CARAPI_(void) AcceptCmd();

    CARAPI_(void) FactoryCmd(
        /* [in] */ const String& clientCmd,
        /* [in] */ IOutputStream* outstream);

    CARAPI SendFile(
        /* [in] */ const String& filepath,
        /* [in] */ IOutputStream* outstream);

    CARAPI ReceiveFile(
        /* [in] */ const String& filepath,
        /* [in] */ ISocket* clientSocket);

    CARAPI_(void) HandleCmd();

    static CARAPI_(AutoPtr<IFile>) GetCalledFile(
        /* [in] */ const String& name);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 CMD_TIMEOUT_SEC;
    static const Int32 BUFFER_SIZE;
    static const String MNT_SDCARD;
    static const String DOWNLOADACCESS;
    static const String UPLOADACCESS;
    static const String READYFORRECEIVE;

    AutoPtr<TimerThread> mTimerThread;
    AutoPtr<IServerSocket> mServerSocket;
    AutoPtr<IContext> mContext;
    Int32 mSocketPort;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_SERVERSOCKETTIMER_H__
