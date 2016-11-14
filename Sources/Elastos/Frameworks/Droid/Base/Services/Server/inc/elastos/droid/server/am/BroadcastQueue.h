
#ifndef __ELASTOS_DROID_SERVER_AM_BROADCASTQUEUE_H__
#define __ELASTOS_DROID_SERVER_AM_BROADCASTQUEUE_H__

#include "elastos/droid/server/am/BroadcastRecord.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Runnable.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

/**
 * BROADCASTS
 *
 * We keep two broadcast queues and associated bookkeeping, one for those at
 * foreground priority, and one for normal (background-priority) broadcasts.
 */
class BroadcastQueue
    : public Object
{
private:
    class BroadcastHandler : public Handler
    {
    public:
        TO_STRING_IMPL("BroadcastQueue::BroadcastHandler")

        BroadcastHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ BroadcastQueue* host)
            : Handler(looper, NULL, TRUE)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        BroadcastQueue* mHost;
    };

    class AppNotResponding : public Runnable
    {
    public:
        AppNotResponding(
            /* [in] */ ProcessRecord* app,
            /* [in] */ const String& annotation,
            /* [in] */ BroadcastQueue* host);

        CARAPI Run();

    private:
        AutoPtr<ProcessRecord> mApp;
        String mAnnotation;
        BroadcastQueue* mHost;
    };

public:
    BroadcastQueue(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IHandler* handler,
        /* [in] */ const String& name,
        /* [in] */ Int64 timeoutPeriod,
        /* [in] */ Boolean allowDelayBehindServices);

    CARAPI_(Boolean) IsPendingBroadcastProcessLocked(
        /* [in] */ Int32 pid);

    CARAPI_(void) EnqueueParallelBroadcastLocked(
        /* [in] */ BroadcastRecord* r);

    CARAPI_(void) EnqueueOrderedBroadcastLocked(
        /* [in] */ BroadcastRecord* r);

    CARAPI_(Boolean) ReplaceParallelBroadcastLocked(
        /* [in] */ BroadcastRecord* r);

    CARAPI_(Boolean) ReplaceOrderedBroadcastLocked(
        /* [in] */ BroadcastRecord* r);

    CARAPI SendPendingBroadcastsLocked(
        /* [in] */ ProcessRecord* app,
        /* [out] */ Boolean* result);

    CARAPI_(void) SkipPendingBroadcastLocked(
        /* [in] */ Int32 pid);

    CARAPI_(void) SkipCurrentReceiverLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI_(void) ScheduleBroadcastsLocked();

    CARAPI_(AutoPtr<BroadcastRecord>) GetMatchingOrderedReceiver(
        /* [in] */ IBinder* receiver);

    CARAPI_(Boolean) FinishReceiverLocked(
        /* [in] */ BroadcastRecord* r,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras,
        /* [in] */ Boolean resultAbort,
        /* [in] */ Boolean waitForServices);

    CARAPI_(void) BackgroundServicesFinishedLocked(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<BroadcastRecord>) GetProcessingBroadcast();

    CARAPI ProcessNextBroadcast(
        /* [in] */ Boolean fromMsg);

    CARAPI_(void) SetBroadcastTimeoutLocked(
        /* [in] */ Int64 timeoutTime);

    CARAPI_(void) CancelBroadcastTimeoutLocked();

    CARAPI_(void) BroadcastTimeoutLocked(
        /* [in] */ Boolean fromMsg);

    CARAPI_(void) LogBroadcastReceiverDiscardLocked(
        /* [in] */ BroadcastRecord* r);

    CARAPI_(Boolean) DumpLocked(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 opti,
        /* [in] */ Boolean dumpAll,
        /* [in] */ const String& dumpPackage,
        /* [in] */ Boolean needSep);

private:
    CARAPI ProcessCurBroadcastLocked(
        /* [in] */ BroadcastRecord* r,
        /* [in] */ ProcessRecord* app);

    static CARAPI PerformReceiveLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ IIntentReceiver* receiver,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI_(void) DeliverToRegisteredReceiverLocked(
        /* [in] */ BroadcastRecord* r,
        /* [in] */ BroadcastFilter* filter,
        /* [in] */ Boolean ordered);

    CARAPI_(AutoPtr<List<String> >) GetWhiteList();

    CARAPI_(void) AddBroadcastToHistoryLocked(
        /* [in] */ BroadcastRecord* r);

public:
    static const String TAG;
    static const String TAG_MU;
    static const Boolean DEBUG_BROADCAST;
    static const Boolean DEBUG_BROADCAST_LIGHT;
    static const Boolean DEBUG_MU;

    static const Int32 MAX_BROADCAST_HISTORY;
    static const Int32 MAX_BROADCAST_SUMMARY_HISTORY;

    static const Int32 BROADCAST_INTENT_MSG;
    static const Int32 BROADCAST_TIMEOUT_MSG;

    CActivityManagerService* mService;

    /**
     * Recognizable moniker for this queue
     */
    const String mQueueName;

    /**
     * Timeout period for this queue's broadcasts
     */
    const Int32 mTimeoutPeriod;

    /**
     * If true, we can delay broadcasts while waiting services to finish in the previous
     * receiver's process.
     */
    const Boolean mDelayBehindServices;

    /**
     * Lists of all active broadcasts that are to be executed immediately
     * (without waiting for another broadcast to finish).  Currently this only
     * contains broadcasts to registered receivers, to avoid spinning up
     * a bunch of processes to execute IntentReceiver components.  Background-
     * and foreground-priority broadcasts are queued separately.
     */
    List< AutoPtr<BroadcastRecord> > mParallelBroadcasts;

    /**
     * List of all active broadcasts that are to be executed one at a time.
     * The object at the top of the list is the currently activity broadcasts;
     * those after it are waiting for the top to finish.  As with parallel
     * broadcasts, separate background- and foreground-priority queues are
     * maintained.
     */
    List< AutoPtr<BroadcastRecord> > mOrderedBroadcasts;

    /**
     * Historical data of past broadcasts, for debugging.
     */
    AutoPtr< ArrayOf<BroadcastRecord*> > mBroadcastHistory;

    /**
     * Summary of historical data of past broadcasts, for debugging.
     */
    AutoPtr< ArrayOf<IIntent*> > mBroadcastSummaryHistory;

    /**
     * Set when we current have a BROADCAST_INTENT_MSG in flight.
     */
    Boolean mBroadcastsScheduled;

    /**
     * True if we have a pending unexpired BROADCAST_TIMEOUT_MSG posted to our handler.
     */
    Boolean mPendingBroadcastTimeoutMessage;

    /**
     * Intent broadcasts that we have tried to start, but are
     * waiting for the application's process to be created.  We only
     * need one per scheduling class (instead of a list) because we always
     * process broadcasts one at a time, so no others can be started while
     * waiting for this one.
     */
    AutoPtr<BroadcastRecord> mPendingBroadcast;

    /**
      * Intent broadcast that we are currently processing
      */
    AutoPtr<BroadcastRecord> mCurrentBroadcast;

    /**
     * The receiver index that is pending, to restart the broadcast if needed.
     */
    Int32 mPendingBroadcastRecvIndex;

    static AutoPtr<List<String> > mQuickbootWhiteList;

    AutoPtr<BroadcastHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Server::Am::BroadcastQueue*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SERVER_AM_BROADCASTQUEUE_H__
