
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCCHANNEL_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCCHANNEL_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::IStack;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class ECO_PUBLIC AsyncChannel
    : public Object
    , public IAsyncChannel
{
private:
    /**
     * Helper class to send messages synchronously
     */
    class ECO_LOCAL SyncMessenger : public Object
    {
        friend class AsyncChannel;
    private:
        class SyncHandler
            : public Handler
        {
        public:
            TO_STRING_IMPL("AsyncChannel::SyncHandler")

            /** Constructor */
            SyncHandler(
                /* [in] */ ILooper* looper)
                : Handler(looper)
            {}

            /** Handle of the reply message */
            // @Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        public:
            /** The object used to wait/notify */
            Object mLockObject;
            /** The resulting message */
            AutoPtr<IMessage> mResultMsg;
        };

    private:
        /** private constructor */
        SyncMessenger()
        {}

        /**
         * @return the SyncMessenger
         */
        static CARAPI_(AutoPtr<SyncMessenger>) Obtain();

        /**
         * Recycle this object
         */
        CARAPI_(void) Recycle();

        /**
         * Send a message synchronously.
         *
         * @param msg to send
         * @return result message or null if an error occurs
         */
        static CARAPI_(AutoPtr<IMessage>) SendMessageSynchronously(
            /* [in] */ IMessenger* dstMessenger,
            /* [in] */ IMessage* msg);

    private:
        /** A stack of SyncMessengers */
        static AutoPtr<IStack> sStack;
        /** A number of SyncMessengers created */
        static Int32 sCount;
        /** The handler thread */
        AutoPtr<IHandlerThread> mHandlerThread;
        /** The handler that will receive the result */
        AutoPtr<SyncHandler> mHandler;
        /** The messenger used to send the message */
        AutoPtr<IMessenger> mMessenger;
    };

    class ECO_LOCAL AsyncChannelConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        AsyncChannelConnection(
            /* [in] */ IWeakReference* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class ECO_LOCAL ConnectAsyncRunnable : public Runnable
    {
    public:
        ConnectAsyncRunnable(
            /* [in] */ IWeakReference* wr,
            /* [in] */ IContext* srcContext,
            /* [in] */ IHandler* srcHandler,
            /* [in] */ const String& dstPackageName,
            /* [in] */ const String& dstClassName);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<IContext> mSrcCtx;
        AutoPtr<IHandler> mSrcHdlr;
        String mDstPackageName;
        String mDstClassName;
    };

    class ECO_LOCAL DeathMonitor
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        DeathMonitor(
            /* [in] */ AsyncChannel* owner);

        ~DeathMonitor();

        CARAPI ProxyDied();

        CARAPI_(void) LinkToDeath();

    private:
        AsyncChannel* mOwner;
        Boolean mIsLinked;
    };

public:
    AsyncChannel();

    virtual ~AsyncChannel();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Connect handler to named package/class synchronously.
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstPackageName is the destination package name
     * @param dstClassName is the fully qualified class name (i.e. contains
     *            package name)
     *
     * @return STATUS_SUCCESSFUL on success any other value is an error.
     */
    CARAPI ConnectSrcHandlerToPackageSync(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ const String& dstPackageName,
        /* [in] */ const String& dstClassName,
        /* [out] */ Int32* status);

    /**
     * Connect a handler to Messenger synchronously.
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstMessenger is the hander to send messages to.
     *
     * @return STATUS_SUCCESSFUL on success any other value is an error.
     */
    CARAPI ConnectSync(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IMessenger* dstMessenger,
        /* [out] */ Int32* status);

    /**
     * connect two local Handlers synchronously.
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstHandler is the hander to send messages to.
     *
     * @return STATUS_SUCCESSFUL on success any other value is an error.
     */
    CARAPI ConnectSync(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IHandler* dstHandler,
        /* [out] */ Int32* status);

    /**
     * Connect handler and messenger.
     *
     * Sends a CMD_CHANNEL_HALF_CONNECTED message to srcHandler when complete.
     *      msg.arg1 = status
     *      msg.obj = the AsyncChannel
     *
     * @param srcContext
     * @param srcHandler
     * @param dstMessenger
     */
    CARAPI Connect(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IMessenger* dstMessenger);

    /**
     * Connect handler to messenger. This method is typically called
     * when a server receives a CMD_CHANNEL_FULL_CONNECTION request
     * and initializes the internal instance variables to allow communication
     * with the dstMessenger.
     *
     * @param srcContext
     * @param srcHandler
     * @param dstMessenger
     */
    CARAPI Connected(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IMessenger* dstMessenger);

    /**
     * Connect two local Handlers.
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstHandler is the hander to send messages to.
     */
    CARAPI Connect(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IHandler* dstHandler);

    CARAPI Connect(
        /* [in] */ IAsyncService* srcAsyncService,
        /* [in] */ IMessenger* dstMessenger);

    /**
     * Fully connect two local Handlers synchronously.
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstHandler is the hander to send messages to.
     *
     * @return STATUS_SUCCESSFUL on success any other value is an error.
     */
    CARAPI FullyConnectSync(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ IHandler* dstHandler,
        /* [out] */ Int32* status);

    /**
     * Connect handler to named package/class.
     *
     * Sends a CMD_CHANNEL_HALF_CONNECTED message to srcHandler when complete.
     *      msg.arg1 = status
     *      msg.obj = the AsyncChannel
     *
     * @param srcContext is the context of the source
     * @param srcHandler is the hander to receive CONNECTED & DISCONNECTED
     *            messages
     * @param dstPackageName is the destination package name
     * @param dstClassName is the fully qualified class name (i.e. contains
     *            package name)
     */
    CARAPI Connect(
        /* [in] */ IContext* srcContext,
        /* [in] */ IHandler* srcHandler,
        /* [in] */ const String& dstPackageName,
        /* [in] */ const String& dstClassName);

    /**
     * To close the connection call when handler receives CMD_CHANNEL_DISCONNECTED
     */
    CARAPI Disconnected();

    /**
     * Disconnect
     */
    CARAPI Disconnect();

    /**
     * Send a message to the destination handler.
     *
     * @param msg
     */
    CARAPI SendMessage(
        /* [in] */ IMessage* msg);

    /**
     * Send a message to the destination handler
     *
     * @param what
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what);

    /**
     * Send a message to the destination handler
     *
     * @param what
     * @param arg1
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    /**
     * Send a message to the destination handler
     *
     * @param what
     * @param arg1
     * @param arg2
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Send a message to the destination handler
     *
     * @param what
     * @param arg1
     * @param arg2
     * @param obj
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    /**
     * Send a message to the destination handler
     *
     * @param what
     * @param obj
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Reply to srcMsg sending dstMsg
     *
     * @param srcMsg
     * @param dstMsg
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ IMessage* dstMsg);

    /**
     * Reply to srcMsg
     *
     * @param srcMsg
     * @param what
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ Int32 what);

    /**
     * Reply to srcMsg
     *
     * @param srcMsg
     * @param what
     * @param arg1
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    /**
     * Reply to srcMsg
     *
     * @param srcMsg
     * @param what
     * @param arg1
     * @param arg2
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Reply to srcMsg
     *
     * @param srcMsg
     * @param what
     * @param arg1
     * @param arg2
     * @param obj
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    /**
     * Reply to srcMsg
     *
     * @param srcMsg
     * @param what
     * @param obj
     */
    CARAPI ReplyToMessage(
        /* [in] */ IMessage* srcMsg,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Send the Message synchronously.
     *
     * @param msg to send
     * @return reply message or null if an error.
     */
    CARAPI SendMessageSynchronously(
        /* [in] */ IMessage* msg,
        /* [out] */ IMessage** resultMsg);

    /**
     * Send the Message synchronously.
     *
     * @param what
     * @return reply message or null if an error.
     */
    CARAPI SendMessageSynchronously(
        /* [in] */ Int32 what,
        /* [out] */ IMessage** resultMsg);

    /**
     * Send the Message synchronously.
     *
     * @param what
     * @param arg1
     * @return reply message or null if an error.
     */
    CARAPI SendMessageSynchronously(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [out] */ IMessage** resultMsg);

    /**
     * Send the Message synchronously.
     *
     * @param what
     * @param arg1
     * @param arg2
     * @return reply message or null if an error.
     */
    CARAPI SendMessageSynchronously(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [out] */ IMessage** resultMsg);

    /**
     * Send the Message synchronously.
     *
     * @param what
     * @param obj
     * @return reply message or null if an error.
     */
    CARAPI SendMessageSynchronously(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** resultMsg);

private:
    /**
     * Reply to the src handler that we're half connected.
     * see: CMD_CHANNEL_HALF_CONNECTED for message contents
     *
     * @param status to be stored in msg.arg1
     */
    ECO_LOCAL CARAPI_(void) ReplyHalfConnected(
        /* [in] */ Int32 status);

    /**
     * Reply to the src handler that we are disconnected
     * see: CMD_CHANNEL_DISCONNECTED for message contents
     *
     * @param status to be stored in msg.arg1
     */
    ECO_LOCAL CARAPI_(void) ReplyDisconnected(
        /* [in] */ Int32 status);

private:
    /** Log tag */
    ECO_LOCAL static const String TAG;

    /** Enable to turn on debugging */
    ECO_LOCAL static const Boolean DBG;

    /** Context for source */
    AutoPtr<IContext> mSrcContext;

    /** Handler for source */
    AutoPtr<IHandler> mSrcHandler;

    /** Messenger for source */
    AutoPtr<IMessenger> mSrcMessenger;

    /** Messenger for destination */
    AutoPtr<IMessenger> mDstMessenger;

    /** Service connection */
    AutoPtr<IServiceConnection> mConnection;

    AutoPtr<DeathMonitor> mDeathMonitor;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_ASYNCCHANNEL_H__
