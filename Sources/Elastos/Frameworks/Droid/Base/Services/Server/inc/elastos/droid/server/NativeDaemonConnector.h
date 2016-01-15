
#ifndef __ELASTOS_DROID_SERVER_NATIVEDAEMONCONNECTOR_H__
#define __ELASTOS_DROID_SERVER_NATIVEDAEMONCONNECTOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "NativeDaemonEvent.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/etl/List.h>
#include <elastos/Queue.h>  // TODO delete
#include <elastos/BlockingQueue.h>  // TODO delete
#include <elastos/core/StringBuilder.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Queue;  // TODO delete
using Elastos::Utility::BlockingQueue; // TODO delete
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Core::StringBuilder;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IOutputStream;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;


namespace Elastos {
namespace Droid {
namespace Server {

class NativeDaemonConnector
    : public ElRefBase
    , public IRunnable
    , public IHandlerCallback
    , public IWeakReferenceSource
{
public:
    /**
     * Command builder that handles argument list building.
     */
    class Command : public ElRefBase
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
    class ResponseQueue : public ElRefBase
    {
    private:
        class PendingCmd : public ElRefBase
        {
        public:
            PendingCmd(
                    /* [in] */ Int32 c,
                    /* [in] */ const String& r);
        public:
            Int32 mCmdNum;
            // public BlockingQueue<NativeDaemonEvent> responses = new ArrayBlockingQueue<NativeDaemonEvent>(10);
            //AutoPtr<IBlockingQueue> mResponses;
            Queue< AutoPtr<NativeDaemonEvent> > mResponses; // TODO delete
            Object mResponsesLock;

            String mRequest;
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
    NativeDaemonConnector(
        /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
        /* [in] */ const String& socket,
        /* [in] */ Int32 responseQueueSize,
        /* [in] */ const String& logTag,
        /* [in] */ Int32 maxLogSize);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [in] */ InterfaceID* pIID);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

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

    CARAPI DoCommand(
        /* [in] */ const String& cmd,
        /* [out] */ List<String>& responses);

    CARAPI DoListCommand(
        /* [in] */ const String& cmd,
        /* [in] */ Int32 expectedCode,
        /* [out, callee] */ ArrayOf<String>** responses);

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

    CARAPI MakeCommand(
        /* [in] */ StringBuilder& builder,
        /* [in] */ const String& cmd,
        /* [in] */ ArrayOf<IInterface*>* args);

//    CARAPI_(void) Log(
//        /* [in] */ const String& logstring);
//
//    CARAPI_(void) Loge(
//        /* [in] */ const String& logstring);

private:
    static const Boolean LOGD;

    static const Int32 DEFAULT_TIMEOUT = 1 * 60 * 1000; /* 1 minute */
    static const Int64 WARN_EXECUTE_DELAY_MS = 500; /* .5 sec */

    static const Int32 BUFFER_SIZE = 4096;

    String TAG;

    String mSocket;
    AutoPtr<IOutputStream> mOutputStream;
    // AutoPtr<ILocalLog> mLocalLog;

    AutoPtr<ResponseQueue> mResponseQueue;

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
