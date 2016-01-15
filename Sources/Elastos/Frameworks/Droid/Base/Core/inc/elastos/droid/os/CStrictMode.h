#ifndef __ELASTOS_DROID_OS_CSTRICTMODE_H__
#define __ELASTOS_DROID_OS_CSTRICTMODE_H__

#include "_Elastos_Droid_Os_CStrictMode.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICloseGuardReporter;
using Elastos::Droid::Os::IStrictModeSpan;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Droid::Os::IStrictModeVmPolicy;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;

DEFINE_HASH_FUNC_FOR_PTR_USING_ADDR(ClassID)

namespace Elastos {
namespace Droid {
namespace Os {

class CStrictModeSpan;

CarClass(CStrictMode)
{
public:
    class ThreadPolicy
        : public ElRefBase
        , public IStrictModeThreadPolicy
    {
    public:
        CARAPI ToString(
            /* [out] */ String* str);

        ThreadPolicy(
            /* [in] */ Int32 mask);

        CAR_INTERFACE_DECL();

    public:
        /**
         * The default, lax policy which doesn't catch anything.
         */
        static const AutoPtr<ThreadPolicy> LAX;

        Int32 mMask;
    };

    /**
     * {@link StrictMode} policy applied to all threads in the virtual machine's process.
     *
     * <p>The policy is enabled by {@link #setVmPolicy}.
     */
    class VmPolicy
        : public ElRefBase
        , public IStrictModeVmPolicy
    {
    public:
        VmPolicy(
            /* [in] */ Int32 mask,
            /* [in] */ HashMap<Handle32, Int32> classInstanceLimit);

        CARAPI ToString(
            /* [out] */ String* str);

        CAR_INTERFACE_DECL();

    public:
        /**
         * The default, lax policy which doesn't catch anything.
         */
        static const AutoPtr<VmPolicy> LAX;

        Int32 mMask;

        // Map from class to max number of allowed instances in memory.
        HashMap<Handle32, Int32> mClassInstanceLimit;
    };

    class StrictModeViolation //TODO: : public BlockGuard.BlockGuardPolicyException
    {
    public:
        StrictModeViolation(
            /* [in] */ Int32 policyState,
            /* [in] */ Int32 policyViolated,
            /* [in] */ const String& message);
    };

    class StrictModeNetworkViolation : public StrictModeViolation
    {
    public:
        StrictModeNetworkViolation(
            /* [in] */ Int32 policyMask);
    };

    class ThreadSpanState
        : public ElRefBase
    {
    public:
        AutoPtr<CStrictModeSpan> mActiveHead;    // doubly-linked list.
        Int32 mActiveSize;
        AutoPtr<CStrictModeSpan> mFreeListHead;  // singly-linked list.  only changes at head.
        Int32 mFreeListSize;
    };

private:
    class StrictModeDiskReadViolation : public StrictModeViolation
    {
    public:
        StrictModeDiskReadViolation(
            /* [in] */ Int32 policyMask);
    };

    class StrictModeDiskWriteViolation : public StrictModeViolation
    {
    public:
        StrictModeDiskWriteViolation(
            /* [in] */ Int32 policyMask);
    };

    class StrictModeCustomViolation : public StrictModeViolation
    {
    public:
        StrictModeCustomViolation(
            /* [in] */ Int32 policyMask,
            /* [in] */ const String& name);
    };

    class AndroidBlockGuardPolicy
        : public ElRefBase
        , public IAndroidBlockGuardPolicy
    {
    public:
        CAR_INTERFACE_DECL();

        AndroidBlockGuardPolicy(
            /* [in] */ Int32 policyMask);

        CARAPI ToString(
            /* [out] */ String* str);

        // Part of BlockGuard.Policy interface:
        CARAPI GetPolicyMask(
            /* [out] */ Int32* mask);

        // Part of BlockGuard.Policy interface:
        CARAPI OnWriteToDisk();

        CARAPI OnCustomSlowCall(
            /* [in] */ const String& name);

        CARAPI OnReadFromDisk();

        CARAPI OnNetwork();

        CARAPI SetPolicyMask(
            /* [in] */ Int32 policyMask);

        // Start handling a violation that just started and hasn't
        // actually run yet (e.g. no disk write or network operation
        // has yet occurred).  This sees if we're in an event loop
        // thread and, if so, uses it to roughly measure how long the
        // violation took.
        // void startHandlingViolationException(BlockGuard.BlockGuardPolicyException e);

        // Attempts to fill in the provided ViolationInfo's
        // durationMillis field if this thread has a Looper we can use
        // to measure with.  We measure from the time of violation
        // until the time the looper is idle again (right before
        // the next epoll_wait)
        CARAPI HandleViolationWithTimingAttempt(
            /* [in] */ IStrictModeViolationInfo* info);

        // Note: It's possible (even quite likely) that the
        // thread-local policy mask has changed from the time the
        // violation fired and now (after the violating code ran) due
        // to people who push/pop temporary policy in regions of code,
        // hence the policy being passed around.
        CARAPI HandleViolation(
            /* [in] */ IStrictModeViolationInfo* info);

    private:
        Int32 mPolicyMask;

        // Map from violation stacktrace hashcode -> uptimeMillis of
        // last violation.  No locking needed, as this is only
        // accessed by the same thread.
        /*final*/ HashMap<Int32, Int64> mLastViolationTime;
    };

    class AndroidCloseGuardReporter
        : public ElRefBase
        , public ICloseGuardReporter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Report(
            /* [in] */ const String& message//,
            /* [in] */ /*Throwable allocationSite*/);
    };

    class MessageQueueIdleHandler
        : public ElRefBase
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        MessageQueueIdleHandler(
            /* [in] */ CStrictMode* host);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    public:
        CStrictMode* mHost;
    };

    class NoOpSpan
        : public ElRefBase
        , public IStrictModeSpan
    {
    public:
        CARAPI Finish();

        CAR_INTERFACE_DECL();
    };

    // Dummy throwable, for now, since we don't know when or where the
    // leaked instances came from.  We might in the future, but for
    // now we suppress the stack trace because it's useless and/or
    // misleading.
    class InstanceCountViolation  /*extends Throwable */
    {
    public:
        InstanceCountViolation(
            /* [in] */ Handle32 klass,
            /* [in] */ Int64 instances,
            /* [in] */ Int32 limit);

    private:
        // TODO: static const StackTraceElement[] FAKE_STACK = {
        //     new StackTraceElement("android.os.StrictMode", "setClassInstanceLimit",
        //                           "StrictMode.java", 1)
        // };

    protected:
        Handle32 mClass;
        Int64 mInstances;
        Int32 mLimit;
    };

    class InstanceTracker
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL();

        InstanceTracker(
            /* [in] */ IInterface* instance);

        ~InstanceTracker();

        static CARAPI_(Int32) GetInstanceCount(
            /* [in] */ ClassID* klass);

    private:
        static HashMap<ClassID*, AutoPtr<IInteger32> > sInstanceCounts;

        ClassID* mKlass;

        static Object sInstanceCountsLock;
    };

    class MyThread : public ThreadBase
    {
    public:
        MyThread(
            /* [in] */ const String& str);

        CARAPI Run();
    };

public:
    CARAPI SetThreadPolicy(
        /* [in] */ Handle32 policy);

    CARAPI GetThreadPolicyMask(
        /* [out] */ Int32* mask);

    CARAPI GetThreadPolicy(
        /* [out] */ Handle32* policy);

    CARAPI AllowThreadDiskWrites(
        /* [out] */ Handle32* policy);

    CARAPI AllowThreadDiskReads(
        /* [out] */ Handle32* policy);

    CARAPI ConditionallyEnableDebugLogging(
        /* [out] */ Boolean* isLogging);

    CARAPI EnableDeathOnNetwork();

    CARAPI HasGatheredViolations(
        /* [out] */ Boolean* result);

    CARAPI ClearGatheredViolations();

    CARAPI ConditionallyCheckInstanceCounts();

    CARAPI SetVmPolicy(
        /* [in] */ Handle32 policy);

    CARAPI GetVmPolicy(
        /* [out] */ Handle32* policy);

    CARAPI EnableDefaults();

    CARAPI VmSqliteObjectLeaksEnabled(
       /* [out] */ Boolean* isEnabled);

    CARAPI VmClosableObjectLeaksEnabled(
        /* [out] */ Boolean* isEnabled);

    CARAPI VmRegistrationLeaksEnabled(
        /* [out] */ Boolean* isEnabled);

    CARAPI OnSqliteObjectLeaked(
       /* [in] */ const String& message//,
       /* [in] */ /*Throwable originStack*/);

    CARAPI OnWebViewMethodCalledOnWrongThread(
       /* [in] */ /*Throwable originStack*/);

    CARAPI OnIntentReceiverLeaked(
       /* [in] */ /*Throwable originStack*/);

    CARAPI OnServiceConnectionLeaked(
       /* [in] */ /*Throwable originStack*/);

    CARAPI OnVmPolicyViolation(
       /* [in] */ const String& message//,
       /* [in] */ /*Throwable originStack*/);

    CARAPI WriteGatheredViolationsToParcel(
        /* [in] */ IParcel* p);

    CARAPI ReadAndHandleBinderCallViolations(
        /* [in] */ IParcel* p);

    CARAPI EnterCriticalSpan(
        /* [in] */ const String& name,
        /* [out] */ IStrictModeSpan** span);

    CARAPI NoteSlowCall(
        /* [in] */ const String& name);

    CARAPI NoteDiskRead();

    CARAPI NoteDiskWrite();

    CARAPI TrackActivity(
        /* [in] */ IInterface* instance,
        /* [out] */ IInterface** act);

    CARAPI IncrementExpectedActivityCount(
        /* [in] */ ClassID* klass);

    CARAPI DecrementExpectedActivityCount(
        /* [in] */ ClassID* klass);

private:
    friend class CStrictModeViolationInfo;

    static AutoPtr<IAtomicInteger32> InitsDropboxCallsInFlight();

    static CARAPI_(void) SetThreadPolicyMask(
        /* [in] */ Int32 policyMask);

    // Sets the policy in Dalvik/libcore (BlockGuard)
    static CARAPI_(void) SetBlockGuardPolicy(
        /* [in] */ Int32 policyMask);

    // Sets up CloseGuard in Dalvik/libcore
    static CARAPI_(void) SetCloseGuardEnabled(
        /* [in] */ Boolean enabled);

    // We don't want to flash the screen red in the system server
    // process, nor do we want to modify all the call sites of
    // conditionallyEnableDebugLogging() in the system server,
    // so instead we use this to determine if we are the system server.
    static CARAPI_(Boolean) AmTheSystemServerProcess();

    /**
     * Parses the BlockGuard policy mask out from the Exception's
     * getMessage() String value.  Kinda gross, but least
     * invasive.  :/
     *
     * Input is of the following forms:
     *     "policy=137 violation=64"
     *     "policy=137 violation=64 msg=Arbitrary text"
     *
     * Returns 0 on failure, which is a valid policy, but not a
     * valid policy during a violation (else there must've been
     * some policy in effect to violate).
     */
    static CARAPI_(Int32) ParsePolicyFromMessage(
        /* [in] */ const String& message);

    /**
     * Like parsePolicyFromMessage(), but returns the violation.
     */
    static CARAPI_(Int32) ParseViolationFromMessage(
        /* [in] */ const String& message);

    //static CARAPI_(List<AutoPtr<IStrictModeViolationInfo> >) GetViolationsBeingTimed();

    static CARAPI_(AutoPtr<IHandler>) GetThreadHandler();

    static CARAPI_(Boolean) TooManyViolationsThisLoop();

    static CARAPI_(void) ExecuteDeathPenalty(
        /* [in] */ IStrictModeViolationInfo* info);

    /**
     * Called from android_util_Binder.cpp's
     * android_os_Parcel_enforceInterface when an incoming Binder call
     * requires changing the StrictMode policy mask.  The role of this
     * function is to ask Binder for its current (native) thread-local
     * policy value and synchronize it to libcore's (Java)
     * thread-local policy value.
     */
    static CARAPI_(void) OnBinderStrictModePolicyChange(
        /* [in] */ Int32 newPolicy);

    /**
     * In the common case, as set by conditionallyEnableDebugLogging,
     * we're just dropboxing any violations but not showing a dialog,
     * not loggging, and not killing the process.  In these cases we
     * don't need to do a synchronous call to the ActivityManager.
     * This is used by both per-thread and vm-wide violations when
     * applicable.
     */
    static CARAPI_(void) DropboxViolationAsync(
        /* [in] */ Int32 violationMaskSubset,
        /* [in] */ IStrictModeViolationInfo* info);

    static CARAPI_(AutoPtr<ThreadSpanState>) GetThisThreadSpanState();

private:
    friend class CStrictModeThreadPolicyBuilder;
    friend class CStrictModeVmPolicyBuilder;

    static const String TAG;
    static const Boolean LOG_V;

    static const Boolean IS_USER_BUILD;
    static const Boolean IS_ENG_BUILD;

    // Only log a duplicate stack trace to the logs every second.
    static const Int64 MIN_LOG_INTERVAL_MS;

    // Only show an annoying dialog at most every 30 seconds
    static const Int64 MIN_DIALOG_INTERVAL_MS;

    // How many Span tags (e.g. animations) to report.
    static const Int32 MAX_SPAN_TAGS;

    // How many offending stacks to keep track of (and time) per loop
    // of the Looper.
    static const Int32 MAX_OFFENSES_PER_LOOP;

    static const Int32 ALL_THREAD_DETECT_BITS;

    /**
     * @hide
     */
    static const Int32 DETECT_VM_INSTANCE_LEAKS;  // for VmPolicy

    static const Int32 ALL_VM_DETECT_BITS;

    /**
     * Mask of all the penalty bits valid for thread policies.
     */
    static const Int32 THREAD_PENALTY_MASK;

    /**
     * Mask of all the penalty bits valid for VM policies.
     */
    static const Int32 VM_PENALTY_MASK;

    static const Int32 DETECT_VM_FILE_URI_EXPOSURE;// = 0x4000;  // for VmPolicy

    // TODO: wrap in some ImmutableHashMap thing.
    // Note: must be before static initialization of sVmPolicy.
    static const HashMap<Handle32, Int32> EMPTY_CLASS_LIMIT_MAP;

    /**
     * The current VmPolicy in effect.
     *
     * TODO: these are redundant (mask is in VmPolicy).  Should remove sVmPolicyMask.
     */
    static /*volatile*/ Int32 sVmPolicyMask;
    static /*volatile*/ AutoPtr<VmPolicy> sVmPolicy;

    /**
     * The number of threads trying to do an async dropbox write.
     * Just to limit ourselves out of paranoia.
     */
    static const AutoPtr<IAtomicInteger32> sDropboxCallsInFlight;

    // Map from VM violation fingerprint to uptime millis.
    static /*const*/ HashMap<Int32, Int64> sLastVmViolationTime;

    // Guarded by StrictMode.class
    static /*const */HashMap<ClassID*, AutoPtr<IInteger32> > sExpectedActivityInstanceCount;

    static Int64 sLastInstanceCountCheckMillis;
    static Boolean sIsIdlerRegistered; // guarded by StrictMode.class

    static AutoPtr<MessageQueueIdleHandler> sProcessIdleHandler;

    static const AutoPtr<CStrictMode::NoOpSpan> NO_OP_SPAN;

    static pthread_key_t sThisThreadSpanStateKey;
    static Boolean sHaveThisThreadSpanStateKey;

    static pthread_key_t sThreadHandlerKey;
    static Boolean sHaveThreadHandlerKey;

    static pthread_key_t sGatheredViolations;
    static pthread_key_t sViolationsBeingTimed;

    static AutoPtr<IIWindowManager> sWindowManager;

    Object classLock;
    Object stateLock;
    Object sLastVmViolationTimeLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODE_H__
