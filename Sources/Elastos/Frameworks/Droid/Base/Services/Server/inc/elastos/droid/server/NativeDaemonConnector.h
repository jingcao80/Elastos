
#ifndef __ELASTOS_DROID_SERVER_NATIVEDAEMONCONNECTOR_H__
#define __ELASTOS_DROID_SERVER_NATIVEDAEMONCONNECTOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/NativeDaemonEvent.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Core::StringBuilder;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Generic connector class for interfacing with a native daemon which uses the
 * {@code libsysutils} FrameworkListener protocol.
 */
class NativeDaemonConnector
    : public Object
    , public IRunnable
    , public IHandlerCallback
{
public:
    /**
     * Wrapper around argument that indicates it's sensitive and shouldn't be
     * logged.
     */
    class SensitiveArg
        : public Object
        , public ISensitiveArg
    {
    public:
        CAR_INTERFACE_DECL()

        SensitiveArg(
            /* [in] */ IInterface* arg)
        {
            mArg = arg;
        }

        //@Override
        CARAPI ToString(
            /* [out] */ String* str)
        {
            VALIDATE_NOT_NULL(str)
            *str = Object::ToString(mArg);
            return NOERROR;
        }
    private:
        AutoPtr<IInterface> mArg;
    };

    /**
     * Command builder that handles argument list building. Any arguments must
     * be separated from base command so they can be properly escaped.
     */
    class Command
        : public Object
    {
        friend class NativeDaemonConnector;

    public:
        Command(
            /* [in] */ const String& cmd,
            /* [in] */ ArrayOf<IInterface*>* args);

        CARAPI_(void) AppendArg(
            /* [in] */ IInterface* arg);

    private:
        String mCmd;
        List< AutoPtr<IInterface> > mArguments;
    };

private:
    class ResponseQueue : public Object
    {
    private:
        class PendingCmd : public Object
        {
        public:
            PendingCmd(
                    /* [in] */ Int32 cmdNum,
                    /* [in] */ const String& logCmd);
        public:
            Int32 mCmdNum;
            String mLogCmd;
            AutoPtr<IBlockingQueue> mResponses; //new ArrayBlockingQueue<NativeDaemonEvent>(10);

            // The availableResponseCount member is used to track when we can remove this
            // instance from the ResponseQueue.
            // This is used under the protection of a sync of the mPendingCmds object.
            // A positive value means we've had more writers retreive this object while
            // a negative value means we've had more readers.  When we've had an equal number
            // (it goes to zero) we can remove this object from the mPendingCmds list.
            // Note that we may have more responses for this command (and more readers
            // coming), but that would result in a new PendingCmd instance being created
            // and added with the same cmdNum.
            // Also note that when this goes to zero it just means a parity of readers and
            // writers have retrieved this object - not that they are done using it.  The
            // responses queue may well have more responses yet to be read or may get more
            // responses added to it.  But all those readers/writers have retreived and
            // hold references to this instance already so it can be removed from
            // mPendingCmds queue.
            Int32 mAvailableResponseCount;
        };

    public:
        ResponseQueue(
            /* [in] */ Int32 maxCount);

        CARAPI_(void) Add(
            /* [in] */ Int32 cmdNum,
            /* [in] */ NativeDaemonEvent* response);

        CARAPI_(AutoPtr<NativeDaemonEvent>) Remove(
            /* [in] */ Int32 cmdNum,
            /* [in] */ Int32 timeoutMs,
            /* [in] */ const String& origCmd);

        CARAPI_(void) Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<IInterface*>* args);

    private:
        List< AutoPtr<PendingCmd> > mPendingCmds;
        Object mPendingCmdsLock;
        Int32 mMaxCount;
    };

public:
    CAR_INTERFACE_DECL()

    NativeDaemonConnector(
        /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
        /* [in] */ const String& socket,
        /* [in] */ Int32 responseQueueSize,
        /* [in] */ const String& logTag,
        /* [in] */ Int32 maxLogSize,
        /* [in] */ IPowerManagerWakeLock* wl);

    NativeDaemonConnector(
        /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
        /* [in] */ const String& socket,
        /* [in] */ Int32 responseQueueSize,
        /* [in] */ const String& logTag,
        /* [in] */ Int32 maxLogSize,
        /* [in] */ IPowerManagerWakeLock* wl,
        /* [in] */ ILooper* looper);

    CARAPI Run();

    CARAPI HandleMessage(
       /* [in] */ IMessage* msg,
       /* [out] */ Boolean* result);

    CARAPI Execute(
        /* [in] */ Command* cmd,
        /* [out] */ NativeDaemonEvent** event);

    CARAPI Execute(
        /* [in] */ const String& cmd,
        /* [in] */ const String& arg,
        /* [out] */ NativeDaemonEvent** event);

    CARAPI Execute(
        /* [in] */ const String& cmd,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out] */ NativeDaemonEvent** event);

    CARAPI ExecuteForList(
        /* [in] */ Command* cmd,
        /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events);

    CARAPI ExecuteForList(
        /* [in] */ const String& cmd,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events);

    CARAPI Execute(
        /* [in] */ Int32 timeout,
        /* [in] */ const String& cmd,
        /* [in] */ ArrayOf<IInterface*>* args,
        /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events);

    static CARAPI_(void) AppendEscaped(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& arg);

    CARAPI_(void) Monitor();

//    CARAPI_(void) Dump(
//        /* [in] */ IFileDescriptor* fd,
//        /* [in] */ IPrintWriter* pw,
//        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI ListenToSocket();

    AutoPtr<ILocalSocketAddress> DetermineSocketAddress();

    CARAPI MakeCommand(
        /* [in] */ StringBuilder& rawBuilder,
        /* [in] */ StringBuilder& logBuilder,
        /* [in] */ Int32 sequenceNumber,
        /* [in] */ const String& cmd,
        /* [in] */ ArrayOf<IInterface*>* args);

//    CARAPI_(void) Log(
//        /* [in] */ const String& logstring);
//
//    CARAPI_(void) Loge(
//        /* [in] */ const String& logstring);

private:
    static const Boolean LOGD;
    static const Boolean VDBG;

    static const Int32 DEFAULT_TIMEOUT = 1 * 60 * 1000; /* 1 minute */
    static const Int64 WARN_EXECUTE_DELAY_MS = 500; /* .5 sec */

    static const Int32 BUFFER_SIZE = 4096;

    String TAG;

    String mSocket;
    AutoPtr<IOutputStream> mOutputStream;
    // AutoPtr<ILocalLog> mLocalLog;

    AutoPtr<ResponseQueue> mResponseQueue;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<ILooper> mLooper;

    AutoPtr<INativeDaemonConnectorCallbacks> mCallbacks;
    AutoPtr<IHandler> mCallbackHandler;

    AutoPtr<IAtomicInteger32> mSequenceNumber;

    /** Lock held whenever communicating with native daemon. */
    Object mDaemonLock;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NATIVEDAEMONCONNECTOR_H__
