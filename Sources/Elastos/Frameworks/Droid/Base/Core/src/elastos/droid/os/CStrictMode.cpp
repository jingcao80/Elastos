#include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/os/CStrictModeSpan.h"
#include "elastos/droid/os/CStrictModeViolationInfo.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CStrictModeVmPolicyBuilder.h"
#include "elastos/droid/os/CStrictModeThreadPolicyBuilder.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <pthread.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::IStrictModeSpan;
using Elastos::Droid::Os::CStrictModeSpan;
using Elastos::Droid::Os::IStrictModeViolationInfo;
using Elastos::Droid::Os::CStrictModeViolationInfo;
using Elastos::Droid::Os::IStrictModeThreadPolicyBuilder;
using Elastos::Droid::Os::CStrictModeThreadPolicyBuilder;
using Elastos::Droid::Os::IStrictModeVmPolicyBuilder;
using Elastos::Droid::Os::CStrictModeVmPolicyBuilder;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;

using Elastos::Core::StringUtils;
using Elastos::Core::IBlockGuard;
using Elastos::Core::CBlockGuard;
using Elastos::Core::EIID_IBlockGuardPolicy;
using Elastos::Core::ICloseGuard;
using Elastos::Core::CCloseGuard;
using Elastos::Core::EIID_ICloseGuardReporter;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

static void ViolationInfosDestructor(void* st)
{
    List<AutoPtr<IStrictModeViolationInfo> >* obj = static_cast<List<AutoPtr<IStrictModeViolationInfo> >*>(st);
    if (obj) {
        obj->Release();
    }
}

static pthread_key_t InitSGatheredViolations()
{
    pthread_key_t key;
    pthread_key_create(&key, ViolationInfosDestructor);
    return key;
}

static pthread_key_t InitSViolationsBeingTimed()
{
    pthread_key_t key;
    pthread_key_create(&key, NULL);
    return key;
}

static void ThreadAndroidPolicyDestructor(void* st)
{
    ICharSequence* callingPackage = static_cast<ICharSequence*>(st);
    if (callingPackage) {
        REFCOUNT_RELEASE(callingPackage)
    }
}

static void ThreadAndroidPolicyMakeKey()
{
    ASSERT_TRUE(pthread_key_create(&CStrictMode::sThreadAndroidPolicyKey, ThreadAndroidPolicyDestructor) == 0);
}

const String CStrictMode::TAG("StrictMode");
const Boolean CStrictMode::LOG_V = Logger::IsLoggable(TAG, Logger::VERBOSE);
const Boolean CStrictMode::IS_USER_BUILD = Build::TYPE.Equals("user");
const Boolean CStrictMode::IS_ENG_BUILD = Build::TYPE.Equals("eng");
const Int64 CStrictMode::MIN_LOG_INTERVAL_MS = 1000;
const Int64 CStrictMode::MIN_DIALOG_INTERVAL_MS = 30000;
const Int32 CStrictMode::MAX_SPAN_TAGS = 20;
const Int32 CStrictMode::MAX_OFFENSES_PER_LOOP = 10;
const Int32 CStrictMode::ALL_THREAD_DETECT_BITS =
            IStrictMode::DETECT_DISK_WRITE | IStrictMode::DETECT_DISK_READ | IStrictMode::DETECT_NETWORK | IStrictMode::DETECT_CUSTOM;
const Int32 CStrictMode::DETECT_VM_INSTANCE_LEAKS = 0x1000;
const Int32 CStrictMode::DETECT_VM_FILE_URI_EXPOSURE = 0x4000;
const Int32 CStrictMode::ALL_VM_DETECT_BITS =
            IStrictMode::DETECT_VM_CURSOR_LEAKS | IStrictMode::DETECT_VM_CLOSABLE_LEAKS |
            IStrictMode::DETECT_VM_ACTIVITY_LEAKS | DETECT_VM_INSTANCE_LEAKS |
            IStrictMode::DETECT_VM_REGISTRATION_LEAKS | DETECT_VM_FILE_URI_EXPOSURE;
const Int32 CStrictMode::THREAD_PENALTY_MASK =
            IStrictMode::PENALTY_LOG | IStrictMode::PENALTY_DIALOG | IStrictMode::PENALTY_DEATH |
            IStrictMode::PENALTY_DROPBOX | IStrictMode::PENALTY_GATHER |
            IStrictMode::PENALTY_DEATH_ON_NETWORK | IStrictMode::PENALTY_FLASH;
const Int32 CStrictMode::VM_PENALTY_MASK =
            IStrictMode::PENALTY_LOG | IStrictMode::PENALTY_DEATH | IStrictMode::PENALTY_DROPBOX;
const AutoPtr<IAtomicInteger32> CStrictMode::sDropboxCallsInFlight = InitsDropboxCallsInFlight();
Int32 CStrictMode::sVmPolicyMask = 0;
AutoPtr<CStrictMode::VmPolicy> CStrictMode::sVmPolicy = CStrictMode::VmPolicy::LAX;
const HashMap<Handle32, Int32> CStrictMode::EMPTY_CLASS_LIMIT_MAP;
const AutoPtr<CStrictMode::ThreadPolicy> CStrictMode::ThreadPolicy::LAX = new CStrictMode::ThreadPolicy(0);

pthread_key_t CStrictMode::sThisThreadSpanStateKey;
Boolean CStrictMode::sHaveThisThreadSpanStateKey = FALSE;
pthread_key_t CStrictMode::sThreadHandlerKey;
Boolean CStrictMode::sHaveThreadHandlerKey = FALSE;

pthread_key_t CStrictMode::sThreadAndroidPolicyKey;
pthread_once_t CStrictMode::sThreadAndroidPolicyKeyOnce = PTHREAD_ONCE_INIT;

CAR_INTERFACE_IMPL(CStrictMode::ThreadPolicy, Object, IStrictModeThreadPolicy);

CStrictMode::ThreadPolicy::ThreadPolicy(Int32 mask)
    : mMask(mask)
{
}

ECode CStrictMode::ThreadPolicy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("[StrictMode.ThreadPolicy; mask=") + StringUtils::ToString(mMask) + String("]");
    return NOERROR;
}

const AutoPtr<CStrictMode::VmPolicy> CStrictMode::VmPolicy::LAX = new CStrictMode::VmPolicy(0, CStrictMode::EMPTY_CLASS_LIMIT_MAP);

CAR_INTERFACE_IMPL(CStrictMode::VmPolicy, Object, IStrictModeVmPolicy);

CStrictMode::VmPolicy::VmPolicy(
    /* [in] */ Int32 mask,
    /* [in] */ HashMap<Handle32, Int32> classInstanceLimit)
    : mMask(mask)
    , mClassInstanceLimit(classInstanceLimit)
{
    // if (classInstanceLimit == null) {
    //     throw new NullPointerException("classInstanceLimit == null");
    // }
}

ECode CStrictMode::VmPolicy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str =  String("[StrictMode.VmPolicy; mask=") + StringUtils::ToString(mMask) + "]";
    return NOERROR;
}

pthread_key_t CStrictMode::sGatheredViolations = InitSGatheredViolations();

AutoPtr<IAtomicInteger32> CStrictMode::InitsDropboxCallsInFlight()
{
    AutoPtr<IAtomicInteger32> ids;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&ids);
    REFCOUNT_ADD(ids);
    return ids;
}

ECode CStrictMode::SetThreadPolicy(
    /* [in] */ IStrictModeThreadPolicy* policy)
{
    CStrictMode::ThreadPolicy* tp = (CStrictMode::ThreadPolicy*)policy;
    SetThreadPolicyMask(tp->mMask);
    return NOERROR;
}

void CStrictMode::SetThreadPolicyMask(
    /* [in] */ Int32 policyMask)
{
    // In addition to the Java-level thread-local in Dalvik's
    // BlockGuard, we also need to keep a native thread-local in
    // Binder in order to propagate the value across Binder calls,
    // even across native-only processes.  The two are kept in
    // sync via the callback to onStrictModePolicyChange, below.
    SetBlockGuardPolicy(policyMask);

    // And set the Android native version...
    Binder::SetThreadStrictModePolicy(policyMask);
}

void CStrictMode::SetBlockGuardPolicy(
    /* [in] */ Int32 policyMask)
{
    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    if (policyMask == 0) {
        helper->GetLaxPolicy((IBlockGuardPolicy**)&policy);
        helper->SetThreadPolicy(policy);
        return;
    }

    policy = NULL;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);

    AutoPtr<IAndroidBlockGuardPolicy> androidPolicy;
    if (IAndroidBlockGuardPolicy::Probe(policy)) {
        androidPolicy = IAndroidBlockGuardPolicy::Probe(policy);
    }
    else {

        pthread_once(&sThreadAndroidPolicyKeyOnce, ThreadAndroidPolicyMakeKey);

        androidPolicy = (IAndroidBlockGuardPolicy*)pthread_getspecific(sThreadAndroidPolicyKey);
        if (androidPolicy == NULL) {
            androidPolicy = new AndroidBlockGuardPolicy(0);

            ASSERT_TRUE(pthread_setspecific(sThreadAndroidPolicyKey, androidPolicy.Get()) == 0);
            androidPolicy->AddRef();
        }
        helper->SetThreadPolicy(IBlockGuardPolicy::Probe(androidPolicy));
    }
    androidPolicy->SetPolicyMask(policyMask);
}

void CStrictMode::SetCloseGuardEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    AutoPtr<ICloseGuardReporter> reporter;
    helper->GetReporter((ICloseGuardReporter**)&reporter);
    if (reporter->Probe(EIID_IAndroidBlockGuardPolicy) != NULL) {
        AutoPtr<AndroidCloseGuardReporter> anRepoter = new AndroidCloseGuardReporter();
        helper->SetReporter((ICloseGuardReporter*)anRepoter.Get());
    }
    helper->SetEnabled(enabled);
    return;
}

CStrictMode::StrictModeViolation::StrictModeViolation(
    /* [in] */ Int32 policyState,
    /* [in] */ Int32 policyViolated,
    /* [in] */ const String& message)
{
    // TODO:
    // super(policyState, policyViolated, message);
}

CStrictMode::StrictModeNetworkViolation::StrictModeNetworkViolation(
    /* [in] */ Int32 policyMask)
    : StrictModeViolation(policyMask, IStrictMode::DETECT_NETWORK, String(NULL))
{
}

CStrictMode::StrictModeDiskReadViolation::StrictModeDiskReadViolation(
    /* [in] */ Int32 policyMask)
    : StrictModeViolation(policyMask, IStrictMode::DETECT_DISK_READ, String(NULL))
{
}

CStrictMode::StrictModeDiskWriteViolation::StrictModeDiskWriteViolation(
    /* [in] */ Int32 policyMask)
    : StrictModeViolation(policyMask, IStrictMode::DETECT_DISK_WRITE, String(NULL))
{
}

CStrictMode::StrictModeCustomViolation::StrictModeCustomViolation(
    /* [in] */ Int32 policyMask,
    /* [in] */ const String& name)
    : StrictModeViolation(policyMask, IStrictMode::DETECT_CUSTOM, String(NULL))
{
}

ECode CStrictMode::GetThreadPolicyMask(
    /* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask);

    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
    policy->GetPolicyMask(mask);
    return NOERROR;
}

ECode CStrictMode::GetThreadPolicy(
    /* [out] */ IStrictModeThreadPolicy** policy)
{
    VALIDATE_NOT_NULL(policy)
    // TODO: this was a last minute Gingerbread API change (to
    // introduce VmPolicy cleanly) but this isn't particularly
    // optimal for users who might call this method often.  This
    // should be in a thread-local and not allocate on each call.
    Int32 mask;
    GetThreadPolicyMask(&mask);
    AutoPtr<ThreadPolicy> tp = new ThreadPolicy(mask);
    *policy = (IStrictModeThreadPolicy*)tp.Get();
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

ECode CStrictMode::AllowThreadDiskWrites(
    /* [out] */ IStrictModeThreadPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    Int32 oldPolicyMask;
    GetThreadPolicyMask(&oldPolicyMask);
    Int32 newPolicyMask = oldPolicyMask & ~(IStrictMode::DETECT_DISK_WRITE | IStrictMode::DETECT_DISK_READ);
    if (newPolicyMask != oldPolicyMask) {
        SetThreadPolicyMask(newPolicyMask);
    }
    AutoPtr<ThreadPolicy> tp = new ThreadPolicy(oldPolicyMask);
    *policy = (IStrictModeThreadPolicy*)tp.Get();
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

ECode CStrictMode::AllowThreadDiskReads(
    /* [out] */ IStrictModeThreadPolicy** policy)
{
    VALIDATE_NOT_NULL(policy)
    Int32 oldPolicyMask;
    GetThreadPolicyMask(&oldPolicyMask);
    Int32 newPolicyMask = oldPolicyMask & ~(IStrictMode::DETECT_DISK_READ);
    if (newPolicyMask != oldPolicyMask) {
        SetThreadPolicyMask(newPolicyMask);
    }
    AutoPtr<ThreadPolicy> tp = new ThreadPolicy(oldPolicyMask);
    *policy = (IStrictModeThreadPolicy*)tp.Get();
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

Boolean CStrictMode::AmTheSystemServerProcess()
{
    // Fast path.  Most apps don't have the system server's UID.
    if (Process::MyUid() != IProcess::SYSTEM_UID) {
        return FALSE;
    }

    // The settings app, though, has the system server's UID so
    // look up our stack to see if we came from the system server.
    //TODO:
    // Throwable stack = new Throwable();
    // stack.fillInStackTrace();
    // for (StackTraceElement ste : stack.getStackTrace()) {
    //     String clsName = ste.getClassName();
    //     if (clsName != null && clsName.startsWith("com.android.server.")) {
    //         return true;
    //     }
    // }
    return FALSE;
}

ECode CStrictMode::ConditionallyEnableDebugLogging(
    /* [out] */ Boolean* isLogging)
{
    VALIDATE_NOT_NULL(isLogging);
    Boolean doFlashes;
    SystemProperties::GetBoolean(VISUAL_PROPERTY, FALSE, &doFlashes);
    doFlashes = doFlashes && !AmTheSystemServerProcess();
    Boolean suppress;
    SystemProperties::GetBoolean(DISABLE_PROPERTY, FALSE, &suppress);

    // For debug builds, log event loop stalls to dropbox for analysis.
    // Similar logic also appears in ActivityThread.java for system apps.
    if (!doFlashes && (IS_USER_BUILD || suppress)) {
        SetCloseGuardEnabled(FALSE);
        *isLogging = FALSE;
        return NOERROR;
    }

    // Eng builds have flashes on all the time.  The suppression property
    // overrides this, so we force the behavior only after the short-circuit
    // check above.
    if (IS_ENG_BUILD) {
        doFlashes = TRUE;
    }

    // Thread policy controls BlockGuard.
    Int32 threadPolicyMask = IStrictMode::DETECT_DISK_WRITE |
            IStrictMode::DETECT_DISK_READ |
            IStrictMode::DETECT_NETWORK;

    if (!IS_USER_BUILD) {
        threadPolicyMask |= IStrictMode::PENALTY_DROPBOX;
    }
    if (doFlashes) {
        threadPolicyMask |= IStrictMode::PENALTY_FLASH;
    }

    SetThreadPolicyMask(threadPolicyMask);

    // VM Policy controls CloseGuard, detection of Activity leaks,
    // and instance counting.
    if (IS_USER_BUILD) {
        SetCloseGuardEnabled(FALSE);
    } else {
        AutoPtr<IStrictModeVmPolicyBuilder> policyBuilder;
        CStrictModeVmPolicyBuilder::New((IStrictModeVmPolicyBuilder**)&policyBuilder);
        policyBuilder->DetectAll();
        policyBuilder->PenaltyDropBox();
        if (IS_ENG_BUILD) {
            policyBuilder->PenaltyLog();
        }
        AutoPtr<IStrictModeVmPolicy> policy;
        policyBuilder->Build((IStrictModeVmPolicy**)&policy);
        SetVmPolicy(policy.Get());
        Boolean isEnable;
        SetCloseGuardEnabled((VmClosableObjectLeaksEnabled(&isEnable),isEnable));
    }
    *isLogging = TRUE;
    return NOERROR;
}

ECode CStrictMode::EnableDeathOnNetwork()
{
    Int32 oldPolicy;
    GetThreadPolicyMask(&oldPolicy);
    Int32 newPolicy = oldPolicy | IStrictMode::DETECT_NETWORK | IStrictMode::PENALTY_DEATH_ON_NETWORK;
    SetThreadPolicyMask(newPolicy);
    return NOERROR;
}

Int32 CStrictMode::ParsePolicyFromMessage(
    /* [in] */ const String& message)
{
    if (message.IsNull() || !message.StartWith("policy=")) {
        return 0;
    }
    Int32 spaceIndex = message.IndexOf(' ');
    if (spaceIndex == -1) {
        return 0;
    }
    String policyString = message.Substring(7, spaceIndex);
    // try {
        return StringUtils::ParseInt32(policyString);
    // } catch (NumberFormatException e) {
    //     return 0;
    // }
}

Int32 CStrictMode::ParseViolationFromMessage(
    /* [in] */ const String& message)
{
    if (message.IsNull()) {
        return 0;
    }
    Int32 violationIndex = message.IndexOf("violation=");
    if (violationIndex == -1) {
        return 0;
    }
    Int32 numberStartIndex = violationIndex + String("violation=").GetLength();
    Int32 numberEndIndex = message.IndexOf(' ', numberStartIndex);
    if (numberEndIndex == -1) {
        numberEndIndex = message.GetLength();
    }
    String violationString = message.Substring(numberStartIndex, numberEndIndex);
    // try {
        return StringUtils::ParseInt32(violationString);
    // } catch (NumberFormatException e) {
    //     return 0;
    // }
}

pthread_key_t CStrictMode::sViolationsBeingTimed = InitSViolationsBeingTimed();

static void HandlerDestructor(void* st)
{
    IHandler* obj = static_cast<IHandler*>(st);
    if (obj) {
        obj->Release();
    }
}

AutoPtr<IHandler> CStrictMode::GetThreadHandler()
{
    if (!sHaveThreadHandlerKey) {
        ASSERT_TRUE(pthread_key_create(&sThreadHandlerKey, HandlerDestructor) == 0);
        sHaveThreadHandlerKey = TRUE;
    }

    AutoPtr<IHandler> instance = (IHandler*)pthread_getspecific(sThreadHandlerKey);
    if (instance == NULL) {
        CHandler::New((IHandler**)&instance);
        ASSERT_TRUE(pthread_setspecific(sThreadHandlerKey, instance.Get()) == 0);
        instance->AddRef();
    }

    return instance;
}

Boolean CStrictMode::TooManyViolationsThisLoop()
{
    AutoPtr<List<AutoPtr<IStrictModeViolationInfo> > > violations = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sViolationsBeingTimed);
    if (violations == NULL) {
        violations = new List<AutoPtr<IStrictModeViolationInfo> >();
        violations->AddRef();
        pthread_setspecific(sViolationsBeingTimed, violations.Get());
    }
    return violations->GetSize() >= MAX_OFFENSES_PER_LOOP;
}

CAR_INTERFACE_IMPL_2(CStrictMode::AndroidBlockGuardPolicy, Object, IAndroidBlockGuardPolicy, IBlockGuardPolicy);

CStrictMode::AndroidBlockGuardPolicy::AndroidBlockGuardPolicy(
    /* [in] */ Int32 policyMask)
    : mPolicyMask(policyMask)
{
}

ECode CStrictMode::AndroidBlockGuardPolicy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = String("AndroidBlockGuardPolicy; mPolicyMask=") + StringUtils::ToString(mPolicyMask);
    return NOERROR;
}

// Part of BlockGuard.Policy interface:
ECode CStrictMode::AndroidBlockGuardPolicy::GetPolicyMask(
/* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask);
    *mask = mPolicyMask;
    return NOERROR;
}

// Part of BlockGuard.Policy interface:
ECode CStrictMode::AndroidBlockGuardPolicy::OnWriteToDisk()
{
    if ((mPolicyMask & DETECT_DISK_WRITE) == 0) {
        return NOERROR;
    }
    if (TooManyViolationsThisLoop()) {
        return NOERROR;
    }
    //TODO:
    // BlockGuard.BlockGuardPolicyException e = new StrictModeDiskWriteViolation(mPolicyMask);
    // e.fillInStackTrace();
    // startHandlingViolationException(e);
    return E_NOT_IMPLEMENTED;
}

ECode CStrictMode::AndroidBlockGuardPolicy::OnCustomSlowCall(
/* [in] */ const String& name)
{
    if ((mPolicyMask & DETECT_CUSTOM) == 0) {
        return NOERROR;
    }
    if (TooManyViolationsThisLoop()) {
        return NOERROR;
    }
    //TODO:
    // BlockGuard.BlockGuardPolicyException e = new StrictModeCustomViolation(mPolicyMask, name);
    // e.fillInStackTrace();
    // StartHandlingViolationException(e);
    return E_NOT_IMPLEMENTED;
}

ECode CStrictMode::AndroidBlockGuardPolicy::OnReadFromDisk()
{
    if ((mPolicyMask & DETECT_DISK_READ) == 0) {
        return NOERROR;
    }
    if (TooManyViolationsThisLoop()) {
        return NOERROR;
    }
    // BlockGuard.BlockGuardPolicyException e = new StrictModeDiskReadViolation(mPolicyMask);
    // e.fillInStackTrace();
    // StartHandlingViolationException(e);
    return E_NOT_IMPLEMENTED;
}

ECode CStrictMode::AndroidBlockGuardPolicy::OnNetwork()
{
    if ((mPolicyMask & DETECT_NETWORK) == 0) {
        return NOERROR;
    }
    if ((mPolicyMask & PENALTY_DEATH_ON_NETWORK) != 0) {
        //throw new NetworkOnMainThreadException();
        return E_NETWORK_ERROR_EXCEPTION;
    }
    if (TooManyViolationsThisLoop()) {
        return NOERROR;
    }
    //TODO:
    // BlockGuard.BlockGuardPolicyException e = new StrictModeNetworkViolation(mPolicyMask);
    // e.fillInStackTrace();
    // startHandlingViolationException(e);
    return E_NOT_IMPLEMENTED;
}

ECode CStrictMode::AndroidBlockGuardPolicy::SetPolicyMask(
/* [in] */ Int32 policyMask)
{
    mPolicyMask = policyMask;
    return NOERROR;
}

ECode CStrictMode::AndroidBlockGuardPolicy::HandleViolationWithTimingAttempt(
/* [in] */ IStrictModeViolationInfo* info)
{
    AutoPtr<ILooper> looper = Looper::GetMainLooper();

    // Without a Looper, we're unable to time how long the
    // violation takes place.  This case should be rare, as
    // most users will care about timing violations that
    // happen on their main UI thread.  Note that this case is
    // also hit when a violation takes place in a Binder
    // thread, in "gather" mode.  In this case, the duration
    // of the violation is computed by the ultimate caller and
    // its Looper, if any.
    //
    // Also, as a special short-cut case when the only penalty
    // bit is death, we die immediately, rather than timing
    // the violation's duration.  This makes it convenient to
    // use in unit tests too, rather than waiting on a Looper.
    //
    // TODO: if in gather mode, ignore Looper.myLooper() and always
    //       go into this immediate mode?
    CStrictModeViolationInfo* cinfo = (CStrictModeViolationInfo*)info;
    if (looper == NULL ||
        (cinfo->mPolicy & THREAD_PENALTY_MASK) == IStrictMode::PENALTY_DEATH) {
        cinfo->mDurationMillis = -1;  // unknown (redundant, already set)
        HandleViolation(info);
        return NOERROR;
    }

    AutoPtr<List<AutoPtr<IStrictModeViolationInfo> > > records = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sViolationsBeingTimed);
    if (records == NULL) {
        records = new List<AutoPtr<IStrictModeViolationInfo> >();
        records->AddRef();
        pthread_setspecific(sViolationsBeingTimed, records.Get());
    }
    else if (records->GetSize() >= MAX_OFFENSES_PER_LOOP) {
        // Not worth measuring.  Too many offenses in one loop.
        return NOERROR;
    }

    records->PushBack(info);
    if (records->GetSize() > 1) {
        // There's already been a violation this loop, so we've already
        // registered an idle handler to process the list of violations
        // at the end of this Looper's loop.
        return NOERROR;
    }

    if((cinfo->mPolicy & IStrictMode::PENALTY_FLASH) != 0) {
        if(sWindowManager == NULL) {
            AutoPtr<IInterface> service = ServiceManager::GetService(String("window"));
            sWindowManager = IIWindowManager::Probe(service);;
        }
        sWindowManager->ShowStrictModeViolation(TRUE);
    }
    else {
        sWindowManager = NULL;
    }

    // // We post a runnable to a Handler (== delay 0 ms) for
    // // measuring the end time of a violation instead of using
    // // an IdleHandler (as was previously used) because an
    // // IdleHandler may not run for quite a long period of time
    // // if an ongoing animation is happening and continually
    // // posting ASAP (0 ms) animation steps.  Animations are
    // // throttled back to 60fps via SurfaceFlinger/View
    // // invalidates, _not_ by posting frame updates every 16
    // // milliseconds.
    //TODO: threadHandler.get().postAtFrontOfQueue(new Runnable() {
    //         public void run() {
    //             long loopFinishTime = SystemClock.uptimeMillis();

    //             // Note: we do this early, before handling the
    //             // violation below, as handling the violation
    //             // may include PENALTY_DEATH and we don't want
    //             // to keep the red border on.
    //             if (sWindowManager != null) {
    //                 try {
    //                     sWindowManager.showStrictModeViolation(false);
    //                 } catch (RemoteException unused) {
    //                 }
    //             }

    //             for (int n = 0; n < records.size(); ++n) {
    //                 ViolationInfo v = records.get(n);
    //                 v.violationNumThisLoop = n + 1;
    //                 v.durationMillis =
    //                         (int) (loopFinishTime - v.violationUptimeMillis);
    //                 handleViolation(v);
    //             }
    //             records.clear();
    //         }
    //     });

    return E_NOT_IMPLEMENTED;
}

ECode CStrictMode::AndroidBlockGuardPolicy::HandleViolation(
/* [in] */ IStrictModeViolationInfo* info)
{
    if (info == NULL /*|| info.crashInfo == null || info.crashInfo.stackTrace == null*/) {
        //Log.wtf(TAG, "unexpected null stacktrace");
        return NOERROR;
    }

    if (LOG_V) Logger::D(TAG, String("handleViolation; policy=") + StringUtils::ToString(((CStrictModeViolationInfo*)info)->mPolicy));

    CStrictModeViolationInfo* cinfo = (CStrictModeViolationInfo*)info;
    if ((cinfo->mPolicy & PENALTY_GATHER) != 0) {
        AutoPtr< List<AutoPtr<IStrictModeViolationInfo> > > violations = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sGatheredViolations);
        if (violations == NULL) {
            violations = new List<AutoPtr<IStrictModeViolationInfo> >();
            violations->AddRef();
            pthread_setspecific(sGatheredViolations, violations.Get());
        }
        else if (violations->GetSize() >= 5) {
            // Too many.  In a loop or something?  Don't gather them all.
            return NOERROR;
        }
        // for (ViolationInfo previous : violations) {
        //     if (info.crashInfo.stackTrace.equals(previous.crashInfo.stackTrace)) {
        //         // Duplicate. Don't log.
        //         return;
        //     }
        // }
        violations->PushBack(info);
        return NOERROR;
    }

    // Not perfect, but fast and good enough for dup suppression.
    Int32 crashFingerprint = Object::GetHashCode(info);
    Int64 lastViolationTime = 0;
    if (mLastViolationTime.Find(crashFingerprint) != mLastViolationTime.End()) {
        lastViolationTime = mLastViolationTime[crashFingerprint];
    }
    Int64 now = SystemClock::GetUptimeMillis();
    mLastViolationTime[crashFingerprint] = now;
    Int64 timeSinceLastViolationMillis = lastViolationTime == 0 ?
            0x7FFFFFFFFFFFFFFFL /*Long.MAX_VALUE*/ : (now - lastViolationTime);

    if ((cinfo->mPolicy & PENALTY_LOG) != 0 &&
        timeSinceLastViolationMillis > MIN_LOG_INTERVAL_MS) {
        if (cinfo->mDurationMillis != -1) {
            // Log.d(TAG, "StrictMode policy violation; ~duration=" +
            //       info.durationMillis + " ms: " + info.crashInfo.stackTrace);
        } else {
            // Log.d(TAG, "StrictMode policy violation: " + info.crashInfo.stackTrace);
        }
    }

    // The violationMaskSubset, passed to ActivityManager, is a
    // subset of the original StrictMode policy bitmask, with
    // only the bit violated and penalty bits to be executed
    // by the ActivityManagerService remaining set.
    Int32 violationMaskSubset = 0;

    if ((cinfo->mPolicy & PENALTY_DIALOG) != 0 &&
        timeSinceLastViolationMillis > MIN_DIALOG_INTERVAL_MS) {
        violationMaskSubset |= PENALTY_DIALOG;
    }

    if ((cinfo->mPolicy & PENALTY_DROPBOX) != 0 && lastViolationTime == 0) {
        violationMaskSubset |= PENALTY_DROPBOX;
    }

    if (violationMaskSubset != 0) {
        Int32 violationBit = 0;//TODO ParseViolationFromMessage(info.crashInfo.exceptionMessage);
        violationMaskSubset |= violationBit;
        Int32 savedPolicyMask;
        AutoPtr<IStrictMode> helper;
        CStrictMode::AcquireSingleton((IStrictMode**)&helper);
        helper->GetThreadPolicyMask(&savedPolicyMask);

        const Boolean justDropBox = (cinfo->mPolicy & THREAD_PENALTY_MASK) == PENALTY_DROPBOX;
        if (justDropBox) {
            // If all we're going to ask the activity manager
            // to do is dropbox it (the common case during
            // platform development), we can avoid doing this
            // call synchronously which Binder data suggests
            // isn't always super fast, despite the implementation
            // in the ActivityManager trying to be mostly async.
            DropboxViolationAsync(violationMaskSubset, info);
            return NOERROR;
        }

        // Normal synchronous call to the ActivityManager.
        //try {
            // First, remove any policy before we call into the Activity Manager,
            // otherwise we'll infinite recurse as we try to log policy violations
            // to disk, thus violating policy, thus requiring logging, etc...
            // We restore the current policy below, in the finally block.
            SetThreadPolicyMask(0);

            //TODO: ActivityManagerNative.getDefault().handleApplicationStrictModeViolation(
            //     RuntimeInit.getApplicationObject(),
            //     violationMaskSubset,
            //     info);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "RemoteException trying to handle StrictMode violation", e);
        //} finally {
            // Restore the policy.
            SetThreadPolicyMask(savedPolicyMask);
        //}
    }

    if ((cinfo->mPolicy & PENALTY_DEATH) != 0) {
        ExecuteDeathPenalty(info);
    }

    return NOERROR;
}

void CStrictMode::ExecuteDeathPenalty(
    /* [in] */ IStrictModeViolationInfo* info)
{
    // int violationBit = parseViolationFromMessage(info.crashInfo.exceptionMessage);
    // throw new StrictModeViolation(info.policy, violationBit, null);
}

CStrictMode::MyThread::MyThread(
    /* [in] */ const String& str)
{
    Thread::constructor(str);
}

ECode CStrictMode::MyThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    //try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        if (am == NULL) {
            Logger::D(TAG, String("No activity manager; failed to Dropbox violation."));
        } else {
            //TODO: am->HandleApplicationStrictModeViolation(
            //     RuntimeInit.getApplicationObject(),
            //     violationMaskSubset,
            //     info);
        }
    //} catch (RemoteException e) {
    //    Log.e(TAG, "RemoteException handling StrictMode violation", e);
    //}
    Int32 outstanding;
    sDropboxCallsInFlight->DecrementAndGet(&outstanding);
    if (LOG_V) Logger::D(TAG, String("Dropbox complete; in-flight=") + StringUtils::ToString(outstanding));
    return NOERROR;
}

void CStrictMode::DropboxViolationAsync(
    /* [in] */ Int32 violationMaskSubset,
    /* [in] */ IStrictModeViolationInfo* info)
{
    Int32 outstanding;
    sDropboxCallsInFlight->IncrementAndGet(&outstanding);
    if (outstanding > 20) {
        // What's going on?  Let's not make make the situation
        // worse and just not log.
        Int32 outV;
        sDropboxCallsInFlight->DecrementAndGet(&outV);
        return;
    }

    if (LOG_V) Logger::D(TAG, String("Dropboxing async; in-flight=") + StringUtils::ToString(outstanding));
    AutoPtr<IThread> myThread = new MyThread(String("callActivityManagerForStrictModeDropbox"));
    myThread->Start();
}

CAR_INTERFACE_IMPL(CStrictMode::AndroidCloseGuardReporter, Object, ICloseGuardReporter);

ECode CStrictMode::AndroidCloseGuardReporter::Report(
    /* [in] */ const String& message,
    /* [in] */ IThrowable* allocationSite)
{
    AutoPtr<IStrictMode> sMode;
    CStrictMode::AcquireSingleton((IStrictMode**)&sMode);
    return sMode->OnVmPolicyViolation(message);
}

ECode CStrictMode::HasGatheredViolations(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    List<AutoPtr<IStrictModeViolationInfo> >* violations = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sGatheredViolations);
    *result = violations != NULL;
    return NOERROR;
}

ECode CStrictMode::ClearGatheredViolations()
{
    List<AutoPtr<IStrictModeViolationInfo> >* violations = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sGatheredViolations);
    if (violations) {
        violations->Clear();
    }
    return NOERROR;
}

ECode CStrictMode::ConditionallyCheckInstanceCounts()
{
    AutoPtr<IStrictModeVmPolicy> obj;
    GetVmPolicy((IStrictModeVmPolicy**)&obj);

    CStrictMode::VmPolicy* policy = (CStrictMode::VmPolicy*)obj.Get();
    if (policy->mClassInstanceLimit.GetSize() == 0) {
        return NOERROR;
    }
    // Runtime.getRuntime().gc();
    // // Note: classInstanceLimit is immutable, so this is lock-free
    // for (Map.Entry<Class, Integer> entry : policy.classInstanceLimit.entrySet()) {
    //     Class klass = entry.getKey();
    //     int limit = entry.getValue();
    //     long instances = VMDebug.countInstancesOfClass(klass, false);
    //     if (instances <= limit) {
    //         continue;
    //     }
    //     Throwable tr = new InstanceCountViolation(klass, instances, limit);
    //     onVmPolicyViolation(tr.getMessage(), tr);
    // }
    return E_NOT_IMPLEMENTED;
}

Int64 CStrictMode::sLastInstanceCountCheckMillis = 0;
Boolean CStrictMode::sIsIdlerRegistered = FALSE;
AutoPtr<CStrictMode::MessageQueueIdleHandler> CStrictMode::sProcessIdleHandler;

CAR_INTERFACE_IMPL(CStrictMode::MessageQueueIdleHandler, Object, IIdleHandler);

CStrictMode::MessageQueueIdleHandler::MessageQueueIdleHandler(
    /* [in] */ CStrictMode* host)
    : mHost(host)
{
}

ECode CStrictMode::MessageQueueIdleHandler::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 now = SystemClock::GetUptimeMillis();
    if (now - sLastInstanceCountCheckMillis > 30 * 1000) {
        sLastInstanceCountCheckMillis = now;
        mHost->ConditionallyCheckInstanceCounts();
    }
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CStrictMode, Singleton, IStrictMode)

CAR_SINGLETON_IMPL(CStrictMode)

ECode CStrictMode::SetVmPolicy(
    /* [in] */ IStrictModeVmPolicy* policy)
{
    AutoLock lock(classLock);
    {
        sVmPolicy = (CStrictMode::VmPolicy*)policy;
        sVmPolicyMask = sVmPolicy->mMask;
        Boolean isEnabled;
        VmClosableObjectLeaksEnabled(&isEnabled);
        SetCloseGuardEnabled(isEnabled);

        AutoPtr<ILooper> looper = Looper::GetMainLooper();
        if (looper != NULL) {
            AutoPtr<IMessageQueue> mq;
            looper->GetQueue((IMessageQueue**)&mq);
            if (sVmPolicy->mClassInstanceLimit.GetSize() == 0 ||
                (sVmPolicyMask & CStrictMode::VM_PENALTY_MASK) == 0) {
                mq->RemoveIdleHandler((IIdleHandler*)sProcessIdleHandler);
                sIsIdlerRegistered = FALSE;
            } else if (!sIsIdlerRegistered) {
                mq->AddIdleHandler((IIdleHandler*)sProcessIdleHandler);
                sIsIdlerRegistered = TRUE;
            }
        }
    }
    return NOERROR;
}

ECode CStrictMode::GetVmPolicy(
    /* [out] */ IStrictModeVmPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    AutoLock lock(classLock);
    {
        *policy = sVmPolicy.Get();
        REFCOUNT_ADD(*policy)
    }
    return NOERROR;
}

ECode CStrictMode::EnableDefaults()
{
    AutoPtr<IStrictModeThreadPolicyBuilder> stb;
    CStrictModeThreadPolicyBuilder::New((IStrictModeThreadPolicyBuilder**)&stb);
    stb->DetectAll();
    stb->PenaltyLog();
    AutoPtr<IStrictModeThreadPolicy> stp;
    stb->Build((IStrictModeThreadPolicy**)&stp);
    SetThreadPolicy(stp.Get());

    AutoPtr<IStrictModeVmPolicyBuilder> svb;
    CStrictModeVmPolicyBuilder::New((IStrictModeVmPolicyBuilder**)&svb);
    svb->DetectAll();
    svb->PenaltyLog();
    AutoPtr<IStrictModeVmPolicy> svp;
    svb->Build((IStrictModeVmPolicy**)&svp);
    SetVmPolicy(svp.Get());

    return NOERROR;
}

ECode CStrictMode::VmSqliteObjectLeaksEnabled(
   /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = (sVmPolicyMask & IStrictMode::DETECT_VM_CURSOR_LEAKS) != 0;
    return NOERROR;
}

ECode CStrictMode::VmClosableObjectLeaksEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = (sVmPolicyMask & IStrictMode::DETECT_VM_CLOSABLE_LEAKS) != 0;
    return NOERROR;
}

ECode CStrictMode::VmRegistrationLeaksEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = (sVmPolicyMask & IStrictMode::DETECT_VM_REGISTRATION_LEAKS) != 0;
    return NOERROR;
}

ECode CStrictMode::VmFileUriExposureEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = (sVmPolicyMask & DETECT_VM_FILE_URI_EXPOSURE) != 0;
    return NOERROR;
}

ECode CStrictMode::OnSqliteObjectLeaked(
   /* [in] */ const String& message //,
   /* [in] */ /*Throwable originStack*/)
{
    return OnVmPolicyViolation(message);
}

ECode CStrictMode::OnWebViewMethodCalledOnWrongThread(
   /* [in] */ /*Throwable originStack*/)
{
    return OnVmPolicyViolation(String(NULL));
}

ECode CStrictMode::OnIntentReceiverLeaked(
   /* [in] */ /*Throwable originStack*/)
{
    return OnVmPolicyViolation(String(NULL));
}

ECode CStrictMode::OnServiceConnectionLeaked(
   /* [in] */ /*Throwable originStack*/)
{
    return OnVmPolicyViolation(String(NULL));
}

ECode CStrictMode::OnFileUriExposed(
   /* [in] */ const String& location)
{
    String message("file:// Uri exposed through ");
    message += location;
    return OnVmPolicyViolation(message/*, new Throwable(message)*/);
}

/*const*/ HashMap<Int32, Int64> CStrictMode::sLastVmViolationTime;

ECode CStrictMode::OnVmPolicyViolation(
   /* [in] */ const String& message //,
   /* [in] */ /*Throwable originStack*/)
{
    const Boolean penaltyDropbox = (sVmPolicyMask & IStrictMode::PENALTY_DROPBOX) != 0;
    const Boolean penaltyDeath = (sVmPolicyMask & IStrictMode::PENALTY_DEATH) != 0;
    const Boolean penaltyLog = (sVmPolicyMask & IStrictMode::PENALTY_LOG) != 0;
    AutoPtr<IStrictModeViolationInfo> info;
    CStrictModeViolationInfo::New(/*originStack, */sVmPolicyMask, (IStrictModeViolationInfo**)&info);

    // Erase stuff not relevant for process-wide violations
    CStrictModeViolationInfo* cinfo = (CStrictModeViolationInfo*)info.Get();
    cinfo->mNumAnimationsRunning = 0;
    cinfo->mTags = NULL;
    cinfo->mBroadcastIntentAction = NULL;

    Int32 fingerprint = Object::GetHashCode(info);
    const Int64 now = SystemClock::GetUptimeMillis();
    Int64 lastViolationTime = 0;
    Int64 timeSinceLastViolationMillis = 0x7FFFFFFFFFFFFFFFL; // Long.MAX_VALUE;

    {
        AutoLock lock(sLastVmViolationTimeLock);
        if (sLastVmViolationTime.Find(fingerprint) != sLastVmViolationTime.End()) {
            lastViolationTime = sLastVmViolationTime[fingerprint];
            timeSinceLastViolationMillis = now - lastViolationTime;
        }
        if (timeSinceLastViolationMillis > MIN_LOG_INTERVAL_MS) {
            sLastVmViolationTime[fingerprint] = now;
        }
    }

    if (penaltyLog && timeSinceLastViolationMillis > MIN_LOG_INTERVAL_MS) {
        //Log.e(TAG, message, originStack);
    }

    Int32 violationMaskSubset = IStrictMode::PENALTY_DROPBOX | (ALL_VM_DETECT_BITS & sVmPolicyMask);

    if (penaltyDropbox && !penaltyDeath) {
        // Common case for userdebug/eng builds.  If no death and
        // just dropboxing, we can do the ActivityManager call
        // asynchronously.
        DropboxViolationAsync(violationMaskSubset, info);
        return NOERROR;
    }

    if (penaltyDropbox && lastViolationTime == 0) {
        // The violationMask, passed to ActivityManager, is a
        // subset of the original StrictMode policy bitmask, with
        // only the bit violated and penalty bits to be executed
        // by the ActivityManagerService remaining set.
        Int32 savedPolicyMask;
        GetThreadPolicyMask(&savedPolicyMask);
        //try {
            // First, remove any policy before we call into the Activity Manager,
            // otherwise we'll infinite recurse as we try to log policy violations
            // to disk, thus violating policy, thus requiring logging, etc...
            // We restore the current policy below, in the finally block.
            SetThreadPolicyMask(0);

            //TODO: ActivityManagerNative::GetDefault()->HandleApplicationStrictModeViolation(
            //     RuntimeInit.getApplicationObject(),
            //     violationMaskSubset,
            //     info);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "RemoteException trying to handle StrictMode violation", e);
        //} finally {
            // Restore the policy.
            SetThreadPolicyMask(savedPolicyMask);
        //}
    }

    if (penaltyDeath) {
        //System.err.println("StrictMode VmPolicy violation with POLICY_DEATH; shutting down.");
        Process::KillProcess(Process::MyPid());
        //System::Exit(10);
    }
    return NOERROR;
}

ECode CStrictMode::WriteGatheredViolationsToParcel(
    /* [in] */ IParcel* p)
{
    List<AutoPtr<IStrictModeViolationInfo> >* violations = (List<AutoPtr<IStrictModeViolationInfo> >*)pthread_getspecific(sGatheredViolations);
    if (violations == NULL) {
        p->WriteInt32(0);
    } else {
        p->WriteInt32(violations->GetSize());
        List<AutoPtr<IStrictModeViolationInfo> >::Iterator it = violations->Begin();
        for (Int32 i = 0; it != violations->End(); ++it) {
            Int32 start, end;
            p->GetDataPosition(&start);
            (*it)->WriteToParcel(p, 0 /* unused flags? */);
            p->GetDataPosition(&end);
            Int32 size = end - start;
            if (size > 10 * 1024) {
                Logger::D(TAG, "Wrote violation #%d of %d: %d bytes", i,  violations->GetSize(), size);
            }
            ++i;
        }
        if (LOG_V) Logger::D(TAG, "wrote violations to response parcel; num=%d", violations->GetSize());
        violations->Clear(); // somewhat redundant, as we're about to null the threadlocal
    }

    return NOERROR;
}

ECode CStrictMode::ReadAndHandleBinderCallViolations(
        /* [in] */ IParcel* p)
{
    // Our own stack trace to append
    // TODO
    // StringWriter sw = new StringWriter();
    // PrintWriter pw = new FastPrintWriter(sw, false, 256);
    // new LogStackTrace().printStackTrace(pw);
    // pw.flush();
    // String ourStack = sw.toString();

    Int32 policyMask;
    GetThreadPolicyMask(&policyMask);
    Boolean currentlyGathering = (policyMask & PENALTY_GATHER) != 0;

    Int32 numViolations;
    p->ReadInt32(&numViolations);
    for (Int32 i = 0; i < numViolations; ++i) {
        if (LOG_V) Logger::D(TAG, String("strict mode violation stacks read from binder call.  i=") + StringUtils::ToString(i));
        AutoPtr<IStrictModeViolationInfo> info;
        CStrictModeViolationInfo::New(p, !currentlyGathering, (IStrictModeViolationInfo**)&info);

        //TODO:
        // if (info.crashInfo.stackTrace != null && info.crashInfo.stackTrace.length() > 10000) {
        //     String front = info.crashInfo.stackTrace.substring(256);
        //     // 10000 characters is way too large for this to be any sane kind of
        //     // strict mode collection of stacks.  We've had a problem where we leave
        //     // strict mode violations associated with the thread, and it keeps tacking
        //     // more and more stacks on to the violations.  Looks like we're in this casse,
        //     // so we'll report it and bail on all of the current strict mode violations
        //     // we currently are maintaining for this thread.
        //     // First, drain the remaining violations from the parcel.
        //     while (i < numViolations) {
        //         info = new ViolationInfo(p, !currentlyGathering);
        //         i++;
        //     }
        //     // Next clear out all gathered violations.
        //     clearGatheredViolations();
        //     // Now report the problem.
        //     Slog.wtfStack(TAG, "Stack is too large: numViolations=" + numViolations
        //             + " policy=#" + Integer.toHexString(policyMask)
        //             + " front=" + front);
        //     return;
        // }
        // info.crashInfo.stackTrace += "# via Binder call with stack:\n" + ourStack;

        AutoPtr<IBlockGuard> helper;
        CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
        AutoPtr<IBlockGuardPolicy> policy;
        helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
        if(policy != NULL && policy->Probe(EIID_IAndroidBlockGuardPolicy) != NULL) {
            AutoPtr<IAndroidBlockGuardPolicy> androidPolicy = IAndroidBlockGuardPolicy::Probe(policy.Get());
            androidPolicy->HandleViolationWithTimingAttempt(info);
        }
    }
    return NOERROR;
}

void CStrictMode::OnBinderStrictModePolicyChange(
        /* [in] */ Int32 newPolicy)
{
    SetBlockGuardPolicy(newPolicy);
}

CAR_INTERFACE_IMPL(CStrictMode::NoOpSpan, Object, IStrictModeSpan);

ECode CStrictMode::NoOpSpan::Finish()
{
    // Do nothing.
    return NOERROR;
}

const AutoPtr<CStrictMode::NoOpSpan> CStrictMode::NO_OP_SPAN = new CStrictMode::NoOpSpan();

static void ThreadSpanStateDestructor(void* st)
{
    CStrictMode::ThreadSpanState* obj = static_cast<CStrictMode::ThreadSpanState*>(st);
    if (obj) {
        obj->Release();
    }
}

AutoPtr<CStrictMode::ThreadSpanState> CStrictMode::GetThisThreadSpanState()
{
    if (!sHaveThisThreadSpanStateKey) {
        ASSERT_TRUE(pthread_key_create(&sThisThreadSpanStateKey, ThreadSpanStateDestructor) == 0);
        sHaveThisThreadSpanStateKey = TRUE;
    }

    AutoPtr<CStrictMode::ThreadSpanState> instance = (CStrictMode::ThreadSpanState*)pthread_getspecific(sThisThreadSpanStateKey);
    if (instance == NULL) {
        instance = new CStrictMode::ThreadSpanState();
        instance->AddRef();
        ASSERT_TRUE(pthread_setspecific(sThisThreadSpanStateKey, instance.Get()) == 0);
    }

    return instance;
}

AutoPtr<IIWindowManager> CStrictMode::sWindowManager;

ECode CStrictMode::EnterCriticalSpan(
    /* [in] */ const String& name,
    /* [out] */ IStrictModeSpan** span)
{
    VALIDATE_NOT_NULL(span);

    if (IS_USER_BUILD) {
        *span = (IStrictModeSpan*)NO_OP_SPAN.Get();
        REFCOUNT_ADD(*span);
        return NOERROR;
    }
    if (name.IsNull() || name.IsEmpty()) {
        //throw new IllegalArgumentException("name must be non-null and non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ThreadSpanState> state = GetThisThreadSpanState();
    AutoPtr<IStrictModeSpan> _span;
    CStrictModeSpan* cspan;
    {
        AutoLock lock(stateLock);
        if (state->mFreeListHead != NULL) {
            _span = state->mFreeListHead;
            cspan = (CStrictModeSpan*)_span.Get();
            state->mFreeListHead = cspan->mNext;
            state->mFreeListSize--;
        } else {
            // Shouldn't have to do this often.
            CStrictModeSpan::New((Handle32)state.Get(), (IStrictModeSpan**)&_span);
            cspan = (CStrictModeSpan*)_span.Get();
        }
        cspan->mName = name;
        cspan->mCreateMillis = SystemClock::GetUptimeMillis();
        cspan->mNext = state->mActiveHead;
        cspan->mPrev = NULL;
        state->mActiveHead = cspan;
        state->mActiveSize++;
        if (cspan->mNext != NULL) {
            cspan->mNext->mPrev = cspan;
        }
        if (LOG_V) Logger::D(TAG, String("Span enter=") + name + String("; size=") + StringUtils::ToString(state->mActiveSize));
    }
    *span = _span;
    REFCOUNT_ADD(*span);
    return NOERROR;
}

ECode CStrictMode::NoteSlowCall(
    /* [in] */ const String& name)
{
    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);

    if(policy == NULL || policy->Probe(EIID_IAndroidBlockGuardPolicy) == NULL) {
        // StrictMode not enabled.
        return NOERROR;
    }

    AutoPtr<IAndroidBlockGuardPolicy> androidPolicy = IAndroidBlockGuardPolicy::Probe(policy.Get());
    androidPolicy->OnCustomSlowCall(name);
    return NOERROR;
}

ECode CStrictMode::NoteDiskRead()
{
    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);

    AutoPtr<IAndroidBlockGuardPolicy> androidPolicy = IAndroidBlockGuardPolicy::Probe(policy.Get());
    if(androidPolicy == NULL) {
        // StrictMode not enabled.
        return NOERROR;
    }

    policy->OnReadFromDisk();
    return NOERROR;
}

ECode CStrictMode::NoteDiskWrite()
{
    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
    AutoPtr<IAndroidBlockGuardPolicy> androidPolicy = IAndroidBlockGuardPolicy::Probe(policy.Get());
    if(androidPolicy == NULL) {
        // StrictMode not enabled.
        return NOERROR;
    }

    policy->OnWriteToDisk();
    return NOERROR;
}

/*const */HashMap<ClassID*, AutoPtr<IInteger32> > CStrictMode::sExpectedActivityInstanceCount;

ECode CStrictMode::TrackActivity(
    /* [in] */ IInterface* instance,
    /* [out] */ IInterface** act)
{
    VALIDATE_NOT_NULL(act);
    AutoPtr<InstanceTracker> ins = new InstanceTracker(instance);
    *act = TO_IINTERFACE(ins);
    REFCOUNT_ADD(*act);
    return NOERROR;
}

ECode CStrictMode::IncrementExpectedActivityCount(
    /* [in] */ ClassID* klass)
{
    if (klass == NULL) {
        return NOERROR;
    }

    AutoLock lock(classLock);
    {
        if ((sVmPolicy->mMask & IStrictMode::DETECT_VM_ACTIVITY_LEAKS) == 0) {
            return NOERROR;
        }
        HashMap<ClassID*, AutoPtr<IInteger32> >::Iterator it = sExpectedActivityInstanceCount.Find(klass);
        Int32 value;
        Int32 newExpected = (it != sExpectedActivityInstanceCount.End()) ? (it->mSecond->GetValue(&value), value + 1) : 1;
        AutoPtr<IInteger32> r;
        CInteger32::New(newExpected, (IInteger32**)&r);
        sExpectedActivityInstanceCount[klass] = r;
    }
    return NOERROR;
}

ECode CStrictMode::DecrementExpectedActivityCount(
    /* [in] */ ClassID* klass)
{
    if (klass == NULL) {
        return NOERROR;
    }

    Int32 limit;
    AutoLock lock(classLock);
    {
        if ((sVmPolicy->mMask & IStrictMode::DETECT_VM_ACTIVITY_LEAKS) == 0) {
            return NOERROR;
        }

        HashMap<ClassID*, AutoPtr<IInteger32> >::Iterator it = sExpectedActivityInstanceCount.Find(klass);
        Int32 expected;
        Int32 newExpected = (it != sExpectedActivityInstanceCount.End() && (it->mSecond->GetValue(&expected), expected != 0)) ?
              (expected - 1) : 0;

        if (newExpected == 0 && it != sExpectedActivityInstanceCount.End()) {
            sExpectedActivityInstanceCount.Erase(it);
        } else {
            AutoPtr<IInteger32> r;
            CInteger32::New(newExpected, (IInteger32**)&r);
            sExpectedActivityInstanceCount[klass] = r;
        }

        // Note: adding 1 here to give some breathing room during
        // orientation changes.  (shouldn't be necessary, though?)
        limit = newExpected + 1;
    }

    // Quick check.
    Int32 actual = CStrictMode::InstanceTracker::GetInstanceCount(klass);
    if (actual <= limit) {
        return NOERROR;
    }

    // Do a GC and explicit count to double-check.
    // This is the work that we are trying to avoid by tracking the object instances
    // explicity.  Running an explicit GC can be expensive (80ms) and so can walking
    // the heap to count instance (30ms).  This extra work can make the system feel
    // noticeably less responsive during orientation changes when activities are
    // being restarted.  Granted, it is only a problem when StrictMode is enabled
    // but it is annoying.
// TODO:
    // Runtime.getRuntime().gc();

    // long instances = VMDebug.countInstancesOfClass(klass, false);
    // if (instances > limit) {
    //     Throwable tr = new InstanceCountViolation(klass, instances, limit);
    //     onVmPolicyViolation(tr.getMessage(), tr);
    // }
    return E_NOT_IMPLEMENTED;
}

CStrictMode::InstanceCountViolation::InstanceCountViolation(
    /* [in] */ Handle32 klass,
    /* [in] */ Int64 instances,
    /* [in] */ Int32 limit)
    : mClass(klass)
    , mInstances(instances)
    , mLimit(limit)
{
    //TODO:super(klass.toString() + "; instances=" + instances + "; limit=" + limit);
    //setStackTrace(FAKE_STACK);
}

HashMap<ClassID*, AutoPtr<IInteger32> > CStrictMode::InstanceTracker::sInstanceCounts;

CStrictMode::InstanceTracker::InstanceTracker(
    /* [in] */ IInterface* instance)
{
    AutoPtr<IObject> obj = IObject::Probe(instance);
    obj->GetClassID(mKlass);
    {
        AutoLock lock(sInstanceCountsLock);
        HashMap<ClassID*, AutoPtr<IInteger32> >::Iterator it = sInstanceCounts.Find(mKlass);
        Int32 value;
        const Int32 newValue = (it != sInstanceCounts.End()) ? (it->mSecond->GetValue(&value), value + 1) : 1;
        AutoPtr<IInteger32> r;
        CInteger32::New(newValue, (IInteger32**)&r);
        sInstanceCounts[mKlass] = r;
    }
}

CStrictMode::InstanceTracker::~InstanceTracker()
{
    // try {
    {
        AutoLock lock(sInstanceCountsLock);
        HashMap<ClassID*, AutoPtr<IInteger32> >::Iterator it = sInstanceCounts.Find(mKlass);
        if (it != sInstanceCounts.End()) {
            Int32 value;
            it->mSecond->GetValue(&value);
            const Int32 newValue = value - 1;
            if (newValue > 0) {
                AutoPtr<IInteger32> r;
                CInteger32::New(newValue, (IInteger32**)&r);
                sInstanceCounts[mKlass] = r;
            } else {
                sInstanceCounts.Erase(it);
            }
        }
    }
    // } finally {
    //     super.finalize();
    // }
}

Object CStrictMode::InstanceTracker::sInstanceCountsLock;

Int32 CStrictMode::InstanceTracker::GetInstanceCount(
    /* [in] */ ClassID* klass)
{
    {
        AutoLock lock(sInstanceCountsLock);
        HashMap<ClassID*, AutoPtr<IInteger32> >::Iterator it = sInstanceCounts.Find(klass);
        Int32 value;
        return it != sInstanceCounts.End() ? (it->mSecond->GetValue(&value),value) : 0;
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
