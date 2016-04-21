
#include "elastos/droid/app/LoadedPkg.h"
#include "elastos/droid/app/CInnerReceiver.h"
#include "elastos/droid/app/CInnerConnection.h"
#include "elastos/droid/app/CContextImpl.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/CInstrumentationHelper.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/ApplicationLoaders.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/pm/PackageManager.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/droid/DroidRuntime.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/ClassLoader.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::DroidRuntime;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Content::EIID_IPendingResult;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Pm::PackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::DisplayAdjustments;

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

const String LoadedPkg::TAG("LoadedPkg");


//==============================================================================
// LoadedPkg::ReceiverDispatcher::Args
//==============================================================================
CAR_INTERFACE_IMPL(LoadedPkg::ReceiverDispatcher::Args, BroadcastReceiver::PendingResult, IRunnable)

LoadedPkg::ReceiverDispatcher::Args::Args()
    : mOrdered(FALSE)
{}

LoadedPkg::ReceiverDispatcher::Args::~Args()
{}

ECode LoadedPkg::ReceiverDispatcher::Args::constructor(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser,
    /* [in] */ ReceiverDispatcher* rd)
{
    Int32 flags;
    intent->GetFlags(&flags);
    FAIL_RETURN(BroadcastReceiver::PendingResult::constructor(resultCode, resultData, resultExtras,
        rd->mRegistered ? TYPE_REGISTERED : TYPE_UNREGISTERED,
        ordered, sticky, IBinder::Probe(rd->mIIntentReceiver), sendingUser, flags))
    mCurIntent = intent;
    mOrdered = ordered;
    mHost = rd;
    return NOERROR;
}

ECode LoadedPkg::ReceiverDispatcher::Args::Run()
{
    AutoPtr<IBroadcastReceiver> receiver = mHost->mReceiver;
    Boolean ordered = mOrdered;
    if (CActivityThread::DEBUG_BROADCAST) {
        Int32 seq = -1;
        mCurIntent->GetInt32Extra(String("seq"), -1, &seq);
        String action;
        mCurIntent->GetAction(&action);
        Slogger::I(CActivityThread::TAG, "Dispatching broadcast %s seq=%d to %s",
            action.string(), seq, TO_CSTR(mHost->mReceiver));
        Slogger::I(CActivityThread::TAG, "  mRegistered=%d mOrderedHint=%d", mHost->mRegistered, ordered);
    }

    AutoPtr<IIActivityManager> mgr = ActivityManagerNative::GetDefault();
    AutoPtr<IIntent> intent = mCurIntent;
    mCurIntent = NULL;

    if (receiver == NULL || mHost->mForgotten) {
        if (mHost->mRegistered && ordered) {
            if (CActivityThread::DEBUG_BROADCAST) {
                Slogger::I(CActivityThread::TAG, "Finishing NULL broadcast to %d", mHost->mReceiver.Get());
            }
            SendFinished(mgr);
        }
        mHost = NULL;
        return NOERROR;
    }
    // Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, "broadcastReceiveReg");
    // try {
    AutoPtr<IClassLoader> cl = Object::GetClassLoader(receiver.Get());
    if (cl == NULL) {
        Slogger::W(TAG, "LoadedPkg::ReceiverDispatcher::Args::Run(): %s is not a CAR class.",
            TO_CSTR(receiver));
    }
    intent->SetExtrasClassLoader(cl);
    SetExtrasClassLoader(cl);
    receiver->SetPendingResult(this);
    ECode ec = receiver->OnReceive(mHost->mContext, intent);
    if (FAILED(ec)) {
        if (mHost->mRegistered && ordered) {
            if (CActivityThread::DEBUG_BROADCAST) {
                Slogger::I(CActivityThread::TAG, "Finishing failed broadcast to %p", mHost->mReceiver.Get());
            }

            SendFinished(mgr);
        }
        Boolean result;
        if (mHost->mInstrumentation == NULL ||
                (mHost->mInstrumentation->OnException(mHost->mReceiver, ec, &result), !result)) {
            // Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
            Slogger::E(CActivityThread::TAG, "Error receiving broadcast %s in %s ec: 0x%08x",
                TO_CSTR(intent), TO_CSTR(mHost->mReceiver), ec);
            mHost = NULL;
            return E_RUNTIME_EXCEPTION;
            // throw new RuntimeException(
            //     "Error receiving broadcast " + intent
            //     + " in " + mReceiver, e);
        }
    }
    // } catch (Exception e) {
    //     if (mRegistered && ordered) {
    //         // if (ActivityThread.DEBUG_BROADCAST) Slog.i(ActivityThread.TAG,
    //         //         "Finishing failed broadcast to " + mReceiver);
    //         SendFinished(mgr);
    //     }
    //     if (mInstrumentation == NULL ||
    //             !mInstrumentation->OnException(mReceiver, e)) {
    //         // Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    //         // throw new RuntimeException(
    //         //     "Error receiving broadcast " + intent
    //         //     + " in " + mReceiver, e);
    //         return E_RUNTIME_EXCEPTION;
    //     }
    // }

    AutoPtr<IPendingResult> result;
    receiver->GetPendingResult((IPendingResult**)&result);
    if (result != NULL) {
        Finish();
    }

    // Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    mHost = NULL;
    return NOERROR;
}

//==============================================================================
// LoadedPkg::ReceiverDispatcher
//==============================================================================
CAR_INTERFACE_IMPL(LoadedPkg::ReceiverDispatcher, Object, IReceiverDispatcher)

LoadedPkg::ReceiverDispatcher::ReceiverDispatcher()
    : mIIntentReceiver(NULL)
    , mRegistered(FALSE)
    , mForgotten(FALSE)
    , mHasUsed(FALSE)
{}

LoadedPkg::ReceiverDispatcher::~ReceiverDispatcher()
{
    if (mRegistered && mIIntentReceiver != NULL) {
        mIIntentReceiver->Release();
    }
}

ECode LoadedPkg::ReceiverDispatcher::constructor(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* activityThread,
    /* [in] */ IInstrumentation* instrumentation,
    /* [in] */ Boolean registered)
{
    if (activityThread == NULL) {
        Slogger::E("LoadedPkg::ReceiverDispatcher", "Handler must not be null!");
        assert(activityThread != NULL);
        return E_NULL_POINTER_EXCEPTION;
    }

    mReceiver = receiver;
    mContext = context;
    mActivityThread = activityThread;
    mInstrumentation = instrumentation;
    mRegistered = registered;

    CInnerReceiver::New(this, !registered, (IIntentReceiver**)&mIIntentReceiver);
    // mLocation = new IntentReceiverLeaked(null);
    // mLocation.fillInStackTrace();
    return NOERROR;
}

ECode LoadedPkg::ReceiverDispatcher::Validate(
   /* [in] */ IContext* context,
   /* [in] */ IHandler* activityThread)
{
    if (mContext.Get() != context) {
        Slogger::E(LoadedPkg::TAG, "Receiver %p registered with differing Context (was %p now %p)"
                , mReceiver.Get(), mContext.Get(), context);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mActivityThread.Get() != activityThread) {
        Slogger::E(LoadedPkg::TAG, "Receiver %p registered with differing handler (was %p now %p)"
                , mReceiver.Get(), mActivityThread.Get(), activityThread);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IBroadcastReceiver> LoadedPkg::ReceiverDispatcher::GetIntentReceiver()
{
    return mReceiver;
}

AutoPtr<IIntentReceiver> LoadedPkg::ReceiverDispatcher::GetIIntentReceiver()
{
    AutoPtr<IIntentReceiver> r = mIIntentReceiver;
    if (!mHasUsed && !mRegistered) {
        // mIIntentReceiver hold ReceiverDispatcher, so release but not set to null!
        mIIntentReceiver->Release();
    }
    mHasUsed = TRUE;
    return r;
}

void LoadedPkg::ReceiverDispatcher::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    if (CActivityThread::DEBUG_BROADCAST) {
        Int32 seq = -1;
        intent->GetInt32Extra(String("seq"), -1, &seq);
        String action;
        intent->GetAction(&action);
        Slogger::I(CActivityThread::TAG, "Enqueueing broadcast %s seq=%d to %p"
                , action.string(),  seq, mReceiver.Get());
    }

    AutoPtr<Args> args = new Args();
    args->constructor(intent, resultCode, data, extras, ordered, sticky, sendingUser, this);
    Boolean result;
    mActivityThread->Post(args, &result);
    if (!result) {
        if (mRegistered && ordered) {
            AutoPtr<IIActivityManager> mgr = ActivityManagerNative::GetDefault();
            if (CActivityThread::DEBUG_BROADCAST) {
                Slogger::I(CActivityThread::TAG, "Finishing sync broadcast to %p", mReceiver.Get());
            }
            args->SendFinished(mgr);
        }
    }
}

//==============================================================================
// LoadedPkg::ServiceDispatcher::RunConnection
//==============================================================================

LoadedPkg::ServiceDispatcher::RunConnection::RunConnection(
   /* [in] */ ServiceDispatcher* serviceDispatcher,
   /* [in] */ IComponentName* name,
   /* [in] */ IBinder* service,
   /* [in] */ Int32 command)
    : mServiceDispatcher(serviceDispatcher)
    , mName(name)
    , mService(service)
    , mCommand(command)
{}

ECode LoadedPkg::ServiceDispatcher::RunConnection::Run()
{
    if (mCommand == 0) {
        return mServiceDispatcher->DoConnected(mName, mService);
    }
    else if (mCommand == 1) {
        return mServiceDispatcher->DoDeath(mName, mService);
    }
    return NOERROR;
}

//==============================================================================
// LoadedPkg::ServiceDispatcher::DeathMonitor
//==============================================================================

CAR_INTERFACE_IMPL(LoadedPkg::ServiceDispatcher::DeathMonitor, Object, IProxyDeathRecipient);

LoadedPkg::ServiceDispatcher::DeathMonitor::DeathMonitor(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service,
    /* [in] */ ServiceDispatcher* owner)
    : mName(name)
    , mService(service)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)owner;
    wrs->GetWeakReference((IWeakReference**)&mOwner);
}

//@Override
ECode LoadedPkg::ServiceDispatcher::DeathMonitor::ProxyDied()
{
    Slogger::W("LoadedPkg::ServiceDispatcher::DeathMonitor", " >> ProxyDied()");
    AutoPtr<IInterface> obj;
    mOwner->Resolve(EIID_IInterface, (IInterface**)&obj);
    IServiceDispatcher* sdObj = IServiceDispatcher::Probe(obj);
    if (sdObj) {
        ServiceDispatcher* sd = (ServiceDispatcher*)sdObj;
        return sd->Death(mName, mService);
    }

    return NOERROR;
}

//==============================================================================
// LoadedPkg::ServiceDispatcher
//==============================================================================
CAR_INTERFACE_IMPL(LoadedPkg::ServiceDispatcher, Object, IServiceDispatcher)

LoadedPkg::ServiceDispatcher::ServiceDispatcher(
    /* [in] */ IServiceConnection* conn,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* activityThread,
    /* [in] */ Int32 flags)
    : mConnection(conn)
    , mContext(context)
    , mActivityThread(activityThread)
    , mFlags(flags)
    , mDied(FALSE)
    , mForgotten(FALSE)
{
    CInnerConnection::New(this, (IIServiceConnection**)&mIServiceConnection);
//                mLocation = new ServiceConnectionLeaked(null);
//                mLocation.fillInStackTrace();
}

ECode LoadedPkg::ServiceDispatcher::Validate(
   /* [in] */ IContext* context,
   /* [in] */ IHandler* handler)
{
    if (mContext.Get() != context) {
        Slogger::E("LoadedPkg::ServiceDispatcher",
            "ServiceConnection %p registered with differing Context (was %p new %p)",
            mConnection.Get(), mContext.Get(), context);
        return E_RUNTIME_EXCEPTION;
    }

    if (mActivityThread.Get() != handler) {
        Slogger::E("LoadedPkg::ServiceDispatcher",
            "ServiceConnection %p registered with differing handler (was %p new %p)",
            mConnection.Get(), mActivityThread.Get(), handler);
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

void LoadedPkg::ServiceDispatcher::DoForget()
{
    AutoLock lock(this);

    HashMap<AutoPtr<IComponentName>, AutoPtr<ConnectionInfo> >::Iterator it;
    for (it = mActiveConnections.Begin(); it != mActiveConnections.End(); ++it) {
        AutoPtr<ConnectionInfo> ci = it->mSecond;
        AutoPtr<IProxy> proxy = (IProxy*)ci->mBinder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean res = FALSE;
            proxy->UnlinkToDeath(ci->mDeathMonitor, 0, &res);
        }
    }

    mActiveConnections.Clear();
    mForgotten = TRUE;
}

AutoPtr<IServiceConnection> LoadedPkg::ServiceDispatcher::GetServiceConnection()
{
    return mConnection;
}

AutoPtr<IIServiceConnection> LoadedPkg::ServiceDispatcher::GetIServiceConnection()
{
    return mIServiceConnection;
}

Int32 LoadedPkg::ServiceDispatcher::GetFlags()
{
    return mFlags;
}

ECode LoadedPkg::ServiceDispatcher::Connected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (mActivityThread != NULL) {
        AutoPtr<RunConnection> con = new RunConnection(this, name, service, 0);
        Boolean result;
        return mActivityThread->Post(con, &result);
    }
    else {
        return DoConnected(name, service);
    }
    return NOERROR;
}

ECode LoadedPkg::ServiceDispatcher::DoConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<ConnectionInfo> old;
    AutoPtr<ConnectionInfo> info;

    {
        AutoLock lock(this);

        if (mForgotten) {
            // We unbound before receiving the connection; ignore
            // any connection received.
            return NOERROR;
        }

        HashMap<AutoPtr<IComponentName>, AutoPtr<ConnectionInfo> >::Iterator it =
            mActiveConnections.Find(name);
        if (it != mActiveConnections.End()) {
            old = it->mSecond;
        }
        if (old != NULL && old->mBinder.Get() == service) {
            // Huh, already have this one.  Oh well!
            return NOERROR;
        }

        if (service != NULL) {
            // A new service is being connected... set it all up.
            mDied = FALSE;
            info = new ConnectionInfo();
            info->mBinder = service;
            info->mDeathMonitor = new DeathMonitor(name, service, this);
            AutoPtr<IProxy> proxy = (IProxy*)service->Probe(EIID_IProxy);
            if (proxy != NULL) proxy->LinkToDeath(info->mDeathMonitor, 0);
            mActiveConnections[name] = info;
        }
        else {
            // The named service is being disconnected... clean up.
            mActiveConnections.Erase(name);
        }

        if (old != NULL) {
            AutoPtr<IProxy> proxy = (IProxy*)old->mBinder->Probe(EIID_IProxy);
            if (proxy != NULL) {
                Boolean res;
                proxy->UnlinkToDeath(old->mDeathMonitor, 0, &res);
            }
        }
    }

    // If there was an old service, it is not disconnected.
    if (old != NULL) {
        FAIL_RETURN(mConnection->OnServiceDisconnected(name));
    }
    // If there is a new service, it is now connected.
    if (service != NULL) {
        FAIL_RETURN(mConnection->OnServiceConnected(name, service));
    }
    return NOERROR;
}

ECode LoadedPkg::ServiceDispatcher::Death(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<ConnectionInfo> old;

    {
        AutoLock lock(this);
        mDied = true;

        HashMap<AutoPtr<IComponentName>, AutoPtr<ConnectionInfo> >::Iterator it;
        it = mActiveConnections.Find(name);
        if (it != mActiveConnections.End()) {
            old = it->mSecond;
            mActiveConnections.Erase(it);
        }

        if (old == NULL || old->mBinder.Get() != service) {
            // Death for someone different than who we last
            // reported...  just ignore it.
            return NOERROR;
        }

        AutoPtr<IProxy> proxy = (IProxy*)old->mBinder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean res;
            proxy->UnlinkToDeath(old->mDeathMonitor, 0, &res);
        }
    }

    if (mActivityThread != NULL) {
        AutoPtr<RunConnection> con = new RunConnection(this, name, service, 1);
        Boolean result;
        return mActivityThread->Post(con, &result);
    }
    else {
        return DoDeath(name, service);
    }
}

ECode LoadedPkg::ServiceDispatcher::DoDeath(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return mConnection->OnServiceDisconnected(name);
}

//==============================================================================
// LoadedPkg
//==============================================================================
CAR_INTERFACE_IMPL(LoadedPkg, Object, ILoadedPkg)

AutoPtr<IApplication> LoadedPkg::GetApplication()
{
    return mApplication;
}

LoadedPkg::LoadedPkg()
    : mSecurityViolation(FALSE)
    , mIncludeCode(FALSE)
    , mRegisterPackage(FALSE)
    , mClientCount(0)
{
    mDisplayAdjustments = new DisplayAdjustments();
}

LoadedPkg::~LoadedPkg()
{
    mReceivers.Clear();
    mUnregisteredReceivers.Clear();

    mServices.Clear();
    mUnboundServices.Clear();
}

ECode LoadedPkg::constructor(
    /* [in] */ IActivityThread* activityThread,
    /* [in] */ IApplicationInfo* inAInfo,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IClassLoader* baseLoader,
    /* [in] */ Boolean securityViolation,
    /* [in] */ Boolean includeCode,
    /* [in] */ Boolean registerPackage)
{
    Int32 myUid = Process::MyUid();
    AutoPtr<IApplicationInfo> aInfo = AdjustNativeLibraryPaths(inAInfo);

    mActivityThread = activityThread;
    mApplicationInfo = aInfo;
    CApplicationInfo* info = (CApplicationInfo*)aInfo.Get();
    mPackageName = info->mPackageName;
    mAppDir = info->mSourceDir;
    mResDir = info->mUid == myUid ? info->mSourceDir : info->mPublicSourceDir;
    mSplitAppDirs = info->mSplitSourceDirs;
    mSplitResDirs = info->mUid == myUid ? info->mSplitSourceDirs : info->mSplitPublicSourceDirs;
    mOverlayDirs = info->mResourceDirs;
    if (!UserHandle::IsSameUser(info->mUid, myUid) && !Process::IsIsolated()) {
        info->mDataDir = PackageManager::GetDataDirForUser(UserHandle::GetUserId(myUid),
             mPackageName);
    }

    mSharedLibraries = info->mSharedLibraryFiles;
    mDataDir = info->mDataDir;
    if (!mDataDir.IsNull()) {
        CFile::New(mDataDir, (IFile**)&mDataDirFile);
    }
    else {
        mDataDirFile = NULL;
    }

    mLibDir = info->mNativeLibraryDir;
    mBaseClassLoader = baseLoader;
    mSecurityViolation = securityViolation;
    mIncludeCode = includeCode;
    mRegisterPackage = registerPackage;
    mDisplayAdjustments->SetCompatibilityInfo(compatInfo);
    assert(compatInfo != NULL);
    return NOERROR;
}

ECode LoadedPkg::constructor(
    /* [in] */ IActivityThread* activityThread)
{
    mActivityThread = activityThread;
    CApplicationInfo::New((IApplicationInfo**)&mApplicationInfo);
    String pkgName("android");
    IPackageItemInfo::Probe(mApplicationInfo)->SetPackageName(pkgName);
    mPackageName = pkgName;
    mSecurityViolation = FALSE;
    mIncludeCode = TRUE;
    mRegisterPackage = FALSE;
    mClassLoader = ClassLoader::GetSystemClassLoader();
    mResources = CResources::GetSystem();
    return NOERROR;
}

ECode LoadedPkg::InstallSystemApplicationInfo(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ IClassLoader* classLoader)
{
    String pkgName;
    IPackageItemInfo::Probe(info)->GetPackageName(&pkgName);
    assert(pkgName.Equals("android"));
    mApplicationInfo = info;
    mClassLoader = classLoader;
    return NOERROR;
}

AutoPtr<IApplicationInfo> LoadedPkg::AdjustNativeLibraryPaths(
    /* [in] */ IApplicationInfo* aInfo)
{
    // If we're dealing with a multi-arch application that has both
    // 32 and 64 bit shared libraries, we might need to choose the secondary
    // depending on what the current runtime's instruction set is.
    CApplicationInfo* info = (CApplicationInfo*)aInfo;
    if (info->mPrimaryCpuAbi != NULL && info->mSecondaryCpuAbi != NULL) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        String runtimeIsa = DroidRuntime::GetRuntime()->GetInstructionSetString();
        String secondaryIsa;
        system->GetInstructionSet(info->mSecondaryCpuAbi, &secondaryIsa);

        // If the runtimeIsa is the same as the primary isa, then we do nothing.
        // Everything will be set up correctly because info.nativeLibraryDir will
        // correspond to the right ISA.
        if (runtimeIsa.Equals(secondaryIsa)) {
            AutoPtr<IApplicationInfo> modified;
            CApplicationInfo::New(info, (IApplicationInfo**)&modified);
            String dir;
            modified->GetSecondaryNativeLibraryDir(&dir);
            modified->SetNativeLibraryDir(dir);
            return modified;
        }
    }

    return aInfo;
}

ECode LoadedPkg::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode LoadedPkg::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mApplicationInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

Boolean LoadedPkg::IsSecurityViolation()
{
    return mSecurityViolation;
}

ECode LoadedPkg::IsSecurityViolation(
    /* [out] */ Boolean* bval)
{
    VALIDATE_NOT_NULL(bval)
    *bval = mSecurityViolation;
    return NOERROR;
}

ECode LoadedPkg::GetCompatibilityInfo(
    /* [out] */ ICompatibilityInfo** info)
{
    return mDisplayAdjustments->GetCompatibilityInfo(info);
}

ECode LoadedPkg::SetCompatibilityInfo(
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    return mDisplayAdjustments->SetCompatibilityInfo(compatInfo);
}

AutoPtr<ArrayOf<String> > LoadedPkg::GetLibrariesFor(
    /* [in] */ const String& packageName)
{
    AutoPtr<IApplicationInfo> ai;

    ECode ec = CActivityThread::GetPackageManager()->GetApplicationInfo(packageName,
        IPackageManager::GET_SHARED_LIBRARY_FILES, UserHandle::GetMyUserId(),
        (IApplicationInfo**)&ai);
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to GetLibrariesFor %s", packageName.string());
    }

    if (ai == NULL) {
        return NULL;
    }

    AutoPtr<ArrayOf<String> > libs;
    ai->GetSharedLibraryFiles((ArrayOf<String>**)&libs);
    return libs;
}

String LoadedPkg::GetModulePath(
    /* [in] */ const String& appSourceDir,
    /* [in] */ const String& packageName)
{
    StringBuilder sb;
    if (appSourceDir.EndWith(".epk")) {
        sb.Append("/data/elastos/");
        sb.Append(packageName);
        sb.Append(".eco");
    }
    else if (appSourceDir.EndWith("framework-res.apk")) {
        sb.Append("/system/lib/Elastos.Droid.Server.eco");
    }
    else {
        sb.Append("/data/data/com.elastos.runtime/elastos/");
        sb.Append(packageName);
        sb.Append("/");
        sb.Append(packageName);
        sb.Append(".eco");
    }
    return sb.ToString();
}

ECode LoadedPkg::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);

    AutoLock lock(this);

    if (mClassLoader != NULL) {
        *loader = mClassLoader;
        REFCOUNT_ADD(*loader);
        return NOERROR;
    }

    if (mIncludeCode && !mPackageName.Equals("android")) {
        // Avoid the binder call when the package is the current application package.
        // The activity manager will perform ensure that dexopt is performed before
        // spinning up the process.
        // if (!Objects.Equals(mPackageName, CActivityThread::GetCurrentPackageName())) {
        //     String isa = DroidRuntime::GetRuntime()->GetInstructionSetString();
        //     CActivityThread::GetPackageManager()->PerformDexOptIfNeeded(mPackageName, isa);
        // }

        if (mRegisterPackage) {
            ActivityManagerNative::GetDefault()->AddPackageDependency(mPackageName);
        }

        List<String> zipPaths, libPaths;
        zipPaths.PushBack(mAppDir);
        // if (mSplitAppDirs != NULL) {
        //     for (Int32 i = 0; i < mSplitAppDirs->GetLength(); ++i) {
        //         zipPaths.PushBack((*mSplitAppDirs)[i]);
        //     }
        // }

        libPaths.PushBack(mLibDir);

        // /*
        //  * The following is a bit of a hack to inject
        //  * instrumentation into the system: If the app
        //  * being started matches one of the instrumentation names,
        //  * then we combine both the "instrumentation" and
        //  * "instrumented" app into the path, along with the
        //  * concatenation of both apps' shared library lists.
        //  */
        // CActivityThread* at = (CActivityThread*)mActivityThread;
        // String instrumentationPackageName = at->mInstrumentationPackageName;
        // String instrumentationAppDir = at->mInstrumentationAppDir;
        // AutoPtr<ArrayOf<String> > instrumentationSplitAppDirs = at->mInstrumentationSplitAppDirs;
        // String instrumentationLibDir = at->mInstrumentationLibDir;

        // String instrumentedAppDir = at->mInstrumentedAppDir;
        // AutoPtr<ArrayOf<String> > instrumentedSplitAppDirs = at->mInstrumentedSplitAppDirs;
        // String instrumentedLibDir = at->mInstrumentedLibDir;
        // AutoPtr<ArrayOf<String> > instrumentationLibs;

        // if (mAppDir.Equals(instrumentationAppDir) || mAppDir.Equals(instrumentedAppDir)) {
        //     zipPaths.Clear();
        //     zipPaths.PushBack(instrumentationAppDir);
        //     if (instrumentationSplitAppDirs != NULL) {
        //         for (Int32 i = 0; i < instrumentationSplitAppDirs->GetLength(); ++i) {
        //             zipPaths.PushBack((*instrumentationSplitAppDirs)[i]);
        //         }
        //     }
        //     zipPaths.PushBack(instrumentedAppDir);
        //     if (instrumentedSplitAppDirs != NULL) {
        //         for (Int32 i = 0; i < instrumentedSplitAppDirs->GetLength(); ++i) {
        //             zipPaths.PushBack((*instrumentedSplitAppDirs)[i]);
        //         }
        //     }

        //     libPaths.Clear();
        //     libPaths.PushBack(instrumentationLibDir);
        //     libPaths.PushBack(instrumentedLibDir);

        //     if (!instrumentedAppDir.Equals(instrumentationAppDir)) {
        //         instrumentationLibs = GetLibrariesFor(instrumentationPackageName);
        //     }
        // }

        // if (mSharedLibraries != NULL) {
        //     List<String>::Iterator it;
        //     for (Int32 i = 0; i < mSharedLibraries->GetLength(); ++i) {
        //         String lib = (*mSharedLibraries)[i];
        //         it = Find(zipPaths.Begin(), zipPaths.End(), lib);
        //         if (it != zipPaths.End()) {
        //             zipPaths.PushFront(lib);
        //         }
        //     }
        // }

        // if (instrumentationLibs != NULL) {
        //     List<String>::Iterator it;
        //     for (Int32 i = 0; i < instrumentationLibs->GetLength(); ++i) {
        //         String lib = (*instrumentationLibs)[i];
        //         it = Find(zipPaths.Begin(), zipPaths.End(), lib);
        //         if (it != zipPaths.End()) {
        //             zipPaths.PushFront(lib);
        //         }
        //     }
        // }

        AutoPtr<IFileHelper> fh;
        CFileHelper::AcquireSingleton((IFileHelper**)&fh);
        String psep;
        fh->GetPathSeparator(&psep);

        // String zip = TextUtils::Join(psep, &zipPaths);
        String zip = GetModulePath(mAppDir, mPackageName);
        String lib = TextUtils::Join(psep, &libPaths);

        /*
         * With all the combination done (if necessary, actually
         * create the class loader.
         */
        // Slogger::V(TAG, "GetClassLoader for %s : Class path: %s, JNI path: %s",
        //     mPackageName.string(), zip.string(), lib.string());

        // Temporarily disable logging of disk reads on the Looper thread
        // as this is early and necessary.
        // StrictMode.ThreadPolicy oldPolicy = StrictMode.allowThreadDiskReads();
        mClassLoader = ApplicationLoaders::GetDefault()->GetClassLoader(zip, lib, mBaseClassLoader);
    //     StrictMode.setThreadPolicy(oldPolicy);
    }
    else {
        if (mBaseClassLoader == NULL) {
            mClassLoader = ClassLoader::GetSystemClassLoader();
        }
        else {
            mClassLoader = mBaseClassLoader;
        }
    }

    Slogger::V(TAG, "GetClassLoader for %s : %s", mPackageName.string(), TO_CSTR(mClassLoader));
    *loader = mClassLoader;
    REFCOUNT_ADD(*loader)
    return NOERROR;
}

void LoadedPkg::InitializeJavaContextClassLoader()
{
    // IPackageManager pm = ActivityThread.getPackageManager();
    // android.content.pm.PackageInfo pi;
    // try {
    //     pi = pm.getPackageInfo(mPackageName, 0, UserHandle.myUserId());
    // } catch (RemoteException e) {
    //     throw new IllegalStateException("Unable to get package info for "
    //             + mPackageName + "; is system dying?", e);
    // }
    // if (pi == null) {
    //     throw new IllegalStateException("Unable to get package info for "
    //             + mPackageName + "; is package not installed?");
    // }
    /*
     * Two possible indications that this package could be
     * sharing its virtual machine with other packages:
     *
     * 1.) the sharedUserId attribute is set in the manifest,
     *     indicating a request to share a VM with other
     *     packages with the same sharedUserId.
     *
     * 2.) the application element of the manifest has an
     *     attribute specifying a non-default process name,
     *     indicating the desire to run in another packages VM.
     */
    // boolean sharedUserIdSet = (pi.sharedUserId != null);
    // boolean processNameNotDefault =
    //     (pi.applicationInfo != null &&
    //      !mPackageName.equals(pi.applicationInfo.processName));
    // boolean sharable = (sharedUserIdSet || processNameNotDefault);
    // ClassLoader contextClassLoader =
    //     (sharable)
    //     ? new WarningContextClassLoader()
    //     : mClassLoader;
    // Thread.currentThread().setContextClassLoader(contextClassLoader);
}

ECode LoadedPkg::GetAppDir(
    /* [out] */ String* appDir)
{
    VALIDATE_NOT_NULL(appDir);
    *appDir = mAppDir;
    return NOERROR;
}

ECode LoadedPkg::GetLibDir(
    /* [out] */ String* libDir)
{
     VALIDATE_NOT_NULL(libDir);
     *libDir = mLibDir;
     return NOERROR;
}

ECode LoadedPkg::GetResDir(
    /* [out] */ String* resDir)
{
    VALIDATE_NOT_NULL(resDir);
    *resDir = mResDir;
    return NOERROR;
}

ECode LoadedPkg::GetSplitAppDirs(
    /* [out, callee] */ ArrayOf<String>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = mSplitAppDirs;
    REFCOUNT_ADD(*dirs)
    return NOERROR;
}

ECode LoadedPkg::GetSplitResDirs(
    /* [out, callee] */ ArrayOf<String>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = mSplitResDirs;
    REFCOUNT_ADD(*dirs)
    return NOERROR;
}

ECode LoadedPkg::GetOverlayDirs(
    /* [out, callee] */ ArrayOf<String>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = mOverlayDirs;
    REFCOUNT_ADD(*dirs)
    return NOERROR;
}

ECode LoadedPkg::GetDataDir(
    /* [out] */ String* dataDir)
{
    VALIDATE_NOT_NULL(dataDir);
    *dataDir = mDataDir;
    return NOERROR;
}

ECode LoadedPkg::GetDataDirFile(
    /* [out] */ IFile** dirFile)
{
    VALIDATE_NOT_NULL(dirFile);
    *dirFile = mDataDirFile;
    REFCOUNT_ADD(*dirFile);
    return NOERROR;
}

ECode LoadedPkg::GetAssets(
    /* [in] */ IActivityThread* mainThread,
    /* [out] */ IAssetManager** manager)
{
    AutoPtr<IResources> res;
    FAIL_RETURN(GetResources(mainThread, (IResources**)&res));
    return res->GetAssets(manager);
}

ECode LoadedPkg::GetResources(
    /* [in] */ IActivityThread* mainThread,
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;
    VALIDATE_NOT_NULL(mainThread);

    if (mResources == NULL) {
        CApplicationInfo* cai = (CApplicationInfo*)mApplicationInfo.Get();
        AutoPtr<IContextImpl> contextImpl;
        mainThread->GetSystemContext((IContextImpl**)&contextImpl);
        ((CActivityThread*)mainThread)->GetTopLevelResources(
            mResDir, mSplitResDirs, mOverlayDirs, cai->mSharedLibraryFiles,
            IDisplay::DEFAULT_DISPLAY, NULL, this, IContext::Probe(contextImpl),
            mPackageName, (IResources**)&mResources);
    }
    *res = mResources.Get();
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode LoadedPkg::MakeApplication(
    /* [in] */ Boolean forceDefaultAppClass,
    /* [in] */ IInstrumentation* instrumentation,
    /* [out] */ IApplication** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (mApplication != NULL) {
        *result = mApplication;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    CActivityThread* activityThread = (CActivityThread*)mActivityThread;
    AutoPtr<IApplication> app;
    String appClass;
    FAIL_RETURN(mApplicationInfo->GetClassName(&appClass));
    if (forceDefaultAppClass || (appClass.IsNull())) {
        appClass = String("Elastos.Droid.App.CApplication");/* "android.app.Application" */;

        Slogger::I(TAG, " >> MakeApplication: mPackageName %s, appClass %s",
            mPackageName.string(), appClass.string());

        //    try {
        AutoPtr<IClassLoader> cl;
        GetClassLoader((IClassLoader**)&cl);
        if (!mPackageName.Equals("android")) {
            InitializeJavaContextClassLoader();
        }

        AutoPtr<IContextImpl> appContext;
        CContextImpl::CreateAppContext(mActivityThread, this, (IContextImpl**)&appContext);
        IContext* ctx = IContext::Probe(appContext);
        ECode ec = activityThread->mInstrumentation->NewApplication(
                cl, appClass, ctx, (IApplication**)&app);
        if (FAILED(ec)) {
            Boolean bval;
            activityThread->mInstrumentation->OnException(app, ec, &bval);
            if (!bval) {
                Slogger::E(TAG, "Unable to instantiate application %s, ec: 0x%08x", appClass.string(), ec);
                *result = NULL;
                return E_RUNTIME_EXCEPTION;
            }
        }

        ((CContextImpl*)appContext.Get())->SetOuterContext(IContext::Probe(app));
    }
    else {
        //    try {
        String appSourceDir, packageName;
        mApplicationInfo->GetSourceDir(&appSourceDir);
        IPackageItemInfo::Probe(mApplicationInfo)->GetPackageName(&packageName);
        String path = LoadedPkg::GetModulePath(appSourceDir, packageName);;

        AutoPtr<IClassLoader> cl;
        CPathClassLoader::New(path, ClassLoader::GetSystemClassLoader(), (IClassLoader**)&cl);

        Slogger::I(TAG, " >> MakeApplication: appClass %s, classLoader:%s", appClass.string(), TO_CSTR(cl));

        AutoPtr<IClassInfo> classInfo;
        ECode ec = cl->LoadClass(appClass, (IClassInfo**)&classInfo);
        if (FAILED(ec)) {
            Slogger::E(TAG, "HandleBindApplication: LoadClass %s in failed.", appClass.string(), TO_CSTR(cl));
            return E_RUNTIME_EXCEPTION;
        }

        AutoPtr<IContextImpl> appContext;
        CContextImpl::CreateAppContext(mActivityThread, this, (IContextImpl**)&appContext);
        IContext* ctx = IContext::Probe(appContext);
        AutoPtr<IInstrumentationHelper> helper;
        CInstrumentationHelper::AcquireSingleton((IInstrumentationHelper**)&helper);
        ec = helper->NewApplication(classInfo, ctx, (IApplication**)&app);
        if (FAILED(ec)) {
            Boolean bval;
            activityThread->mInstrumentation->OnException(app, ec, &bval);
            if (!bval) {
                Slogger::E(TAG, "Unable to instantiate application %s, ec: 0x%08x", appClass.string(), ec);
                *result = NULL;
                return E_RUNTIME_EXCEPTION;
            }
        }

        ((CContextImpl*)appContext.Get())->SetOuterContext(IContext::Probe(app));
    }

    activityThread->mAllApplications.PushBack(app);
    mApplication = app;

    if (instrumentation != NULL) {
        ECode ec = instrumentation->CallApplicationOnCreate(app);
        if (FAILED(ec)) {
            Boolean val;
            instrumentation->OnException(app, ec, &val);
            if (!val) {
                Slogger::E(TAG, "Unable to create application %s.", TO_CSTR(app));
                return E_RUNTIME_EXCEPTION;
            }
        }
    }

    // Rewrite the R 'constants' for all library apks.
    AutoPtr<ISparseArray> packageIdentifiers;
    AutoPtr<IAssetManager> assetMgr;
    GetAssets(mActivityThread, (IAssetManager**)&assetMgr);
    ((CAssetManager*)assetMgr.Get())->GetAssignedPackageIdentifiers((ISparseArray**)&packageIdentifiers);
    Int32 N;
    packageIdentifiers->GetSize(&N);
    AutoPtr<IClassLoader> cl;
    GetClassLoader((IClassLoader**)&cl);
    for (Int32 i = 0; i < N; i++) {
        Int32 id;
        packageIdentifiers->KeyAt(i, &id);
        if (id == 0x01 || id == 0x7f) {
            continue;
        }
        AutoPtr<IInterface> valueObj;
        packageIdentifiers->ValueAt(i, (IInterface**)&valueObj);
        RewriteRValues(cl, Object::ToString(valueObj), id);
    }

    *result = app;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LoadedPkg::RewriteRValues(
    /* [in] */ IClassLoader* cl,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 id)
{
    assert(0 && "TODO");
    // final Class<?> rClazz;
    // try {
    //     rClazz = cl.loadClass(packageName + ".R");
    // } catch (ClassNotFoundException e) {
    //     // This is not necessarily an error, as some packages do not ship with resources
    //     // (or they do not need rewriting).
    //     Log.i(TAG, "No resource references to update in package " + packageName);
    //     return;
    // }

    // final Method callback;
    // try {
    //     callback = rClazz.getMethod("onResourcesLoaded", int.class);
    // } catch (NoSuchMethodException e) {
    //     // No rewriting to be done.
    //     return;
    // }

    // Throwable cause;
    // try {
    //     callback.invoke(null, id);
    //     return;
    // } catch (IllegalAccessException e) {
    //     cause = e;
    // } catch (InvocationTargetException e) {
    //     cause = e.getCause();
    // }

    // throw new RuntimeException("Failed to rewrite resource references for " + packageName,
    //         cause);
    return E_RUNTIME_EXCEPTION;
}

ECode LoadedPkg::RemoveContextRegistrations(
    /* [in] */ IContext* context,
    /* [in] */ const String& who,
    /* [in] */ const String& what)
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();

    // final boolean reportRegistrationLeaks = StrictMode.vmRegistrationLeaksEnabled();
    HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> >::Iterator it = mReceivers.Find(context);
    AutoPtr<ReceiverMap> rmap;
    if (it != mReceivers.End()) {
        rmap = it->mSecond;
        mReceivers.Erase(it);
    }

    if (rmap != NULL) {
        ReceiverMapIterator it = rmap->Begin();
        while (it != rmap->End()) {
            AutoPtr<ReceiverDispatcher> rd = it->mSecond;

            // IntentReceiverLeaked leak = new IntentReceiverLeaked(
            //         what + " " + who + " has leaked IntentReceiver "
            //         + rd.getIntentReceiver() + " that was " +
            //         "originally registered here. Are you missing a " +
            //         "call to unregisterReceiver()?");
            // leak.setStackTrace(rd.getLocation().getStackTrace());
            Slogger::E(TAG, "%s has leaked IntentReceiver that was originally registered here."
                " Are you missing a call to unregisterReceiver()?", who.string());
            // if (reportRegistrationLeaks) {
            //     StrictMode.onIntentReceiverLeaked(leak);
            // }

            // try {
            AutoPtr<IIntentReceiver> ir = rd->GetIIntentReceiver();
            am->UnregisterReceiver(ir);
            // } catch (RemoteException e) {
            //     // system crashed, nothing we can do
            // }
        }
    }

    it = mUnregisteredReceivers.Find(context);
    if (it != mUnregisteredReceivers.End()) {
        mUnregisteredReceivers.Erase(it);
    }

    // Slogger::I(TAG, "RemoveContextRegistrations:%p, %s, %s, mServices:%d, mUnboundServices:%d",
    //     context, who.string(), what.string(), mServices.GetSize(), mUnboundServices.GetSize());

    AutoPtr<ServiceMap> smap;
    HashMap<AutoPtr<IContext>, AutoPtr<ServiceMap> >::Iterator sit;

    sit = mServices.Find(context);
    if (sit != mServices.End()) {
        smap = sit->mSecond;
        mServices.Erase(sit);
    }

    if (smap != NULL) {
        Boolean bv;
        ServiceMapIterator it = smap->Begin();
        while (it != smap->End()) {
            AutoPtr<ServiceDispatcher> sd = it->mSecond;

            // ServiceConnectionLeaked leak = new ServiceConnectionLeaked(
            //         what + " " + who + " has leaked ServiceConnection "
            //         + sd.getServiceConnection() + " that was originally bound here");
            // leak.setStackTrace(sd.getLocation().getStackTrace());
            Slogger::E(TAG, "%s has leaked ServiceConnection that was originally bound here.", who.string());
            // if (reportRegistrationLeaks) {
            //     StrictMode.onServiceConnectionLeaked(leak);
            // }

            // try {
            AutoPtr<IIServiceConnection> sc = sd->GetIServiceConnection();
            am->UnbindService(sc, &bv);
            // } catch (RemoteException e) {
            //     // system crashed, nothing we can do
            // }
            sd->DoForget();
        }
    }

    sit = mUnboundServices.Find(context);
    if (sit != mUnboundServices.End()) {
        mUnboundServices.Erase(sit);
    }

    return NOERROR;
}

ECode LoadedPkg::GetReceiverDispatcher(
    /* [in] */ IBroadcastReceiver* r,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IInstrumentation* instrumentation,
    /* [in] */ Boolean registered,
    /* [out] */ IIntentReceiver** ir)
{
    VALIDATE_NOT_NULL(ir);
    *ir = NULL;

    AutoLock lock(mReceiversLock);

    AutoPtr<ReceiverDispatcher> rd;
    AutoPtr<ReceiverMap> map;
    if (registered) {
        HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> >::Iterator it = mReceivers.Find(context);
        if (it != mReceivers.End()) {
            map = it->mSecond;
            if (map.Get() != NULL){
                ReceiverMapIterator ite = map->Find(r);
                if (ite != map->End()) {
                    rd = ite->mSecond;
                }
            }
        }
    }

    if (rd == NULL){
        rd = new ReceiverDispatcher();
        FAIL_RETURN(rd->constructor(r, context, handler, instrumentation, registered))
        if (registered) {
            if (map == NULL) {
                map = new ReceiverMap();
                mReceivers[context] = map;
            }
            (*map)[r] = rd;
        }
    }
    else {
        FAIL_RETURN(rd->Validate(context, handler));
    }

    rd->mForgotten = FALSE;
    AutoPtr<IIntentReceiver> _ir = rd->GetIIntentReceiver();
    *ir = _ir;
    REFCOUNT_ADD(*ir);
    return NOERROR;
}

ECode LoadedPkg::ForgetReceiverDispatcher(
    /* [in] */ IContext* context,
    /* [in] */ IBroadcastReceiver* r,
    /* [out] */ IIntentReceiver** ir)
{
    VALIDATE_NOT_NULL(ir);
    *ir = NULL;

    AutoLock lock(mReceiversLock);

    AutoPtr<ReceiverDispatcher> rd;
    AutoPtr<ReceiverMap> map, holder;
    HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> >::Iterator it = mReceivers.Find(context);
    if (it != mReceivers.End()) map = it->mSecond;
    if (map != NULL) {
        ReceiverMapIterator ite = map->Find(r);
        if (ite != map->End()) rd = ite->mSecond;
        if (rd != NULL) {
            map->Erase(ite);
            if (map->IsEmpty()) {
                mReceivers.Erase(it);
            }
            Boolean b;
            r->GetDebugUnregister(&b);
            if (b) {
                HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> >::Iterator iter = mUnregisteredReceivers.Find(context);
                if (iter != mUnregisteredReceivers.End()) holder = iter->mSecond;
                if (holder == NULL) {
                    holder = new ReceiverMap();
                    mUnregisteredReceivers[context] = holder;
                }
                (*holder)[r] = rd;
            }
            rd->mForgotten = TRUE;
            AutoPtr<IIntentReceiver> _ir = rd->GetIIntentReceiver();
            *ir = _ir;
            REFCOUNT_ADD(*ir);
            return NOERROR;
        }
    }

    HashMap<AutoPtr<IContext>, AutoPtr<ReceiverMap> >::Iterator iter =  mUnregisteredReceivers.Find(context);
    if (iter != mUnregisteredReceivers.End()) holder = iter->mSecond;
    if (holder != NULL) {
        ReceiverMapIterator ite = holder->Find(r);
        if (ite != holder->End()) rd = ite->mSecond;
        if (rd != NULL) {
            Slogger::E(TAG, "Unregistering Receiver %s that was already unregistered", TO_CSTR(r));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    if (context == NULL) {
        Slogger::E(TAG, "Unbinding Receiver %s  from Context that is no longer in use.", TO_CSTR(r));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else {
        Slogger::E(TAG, "Receiver not registered: %s", TO_CSTR(r));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode LoadedPkg::GetServiceDispatcher(
    /* [in] */ IServiceConnection* c,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 flags,
    /* [out] */ IIServiceConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mServicesLock);

    AutoPtr<ServiceDispatcher> sd;
    AutoPtr< ServiceMap > map = mServices[context];
    if (map != NULL) {
        sd = (*map)[c];
    }
    if (sd == NULL) {
        sd = new ServiceDispatcher(c, context, handler, flags);
        if (map == NULL) {
            map = new ServiceMap();
            mServices[context] = map;
        }
        (*map)[c] = sd;
    } else {
        sd->Validate(context, handler);
    }
    AutoPtr<IIServiceConnection> con = sd->GetIServiceConnection();
    *result = con.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LoadedPkg::ForgetServiceDispatcher(
    /* [in] */ IContext* context,
    /* [in] */ IServiceConnection* c,
    /* [out] */ IIServiceConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mServicesLock);

    AutoPtr<ServiceDispatcher> sd;
    AutoPtr<ServiceMap> map;
    HashMap<AutoPtr<IContext>, AutoPtr<ServiceMap> >::Iterator it = mServices.Find(context);
    if (it != mServices.End()) {
        map = it->mSecond;
    }
    if (map != NULL) {
        ServiceMapIterator sdit = map->Find(c);
        if (sdit != map->End()) {
            sd = sdit->mSecond;
        }
        if (sd != NULL) {
            map->Erase(sdit);
            sd->DoForget();
            if (map->Begin() == map->End()) {
                mServices.Erase(it);
                map = NULL;
            }
            if ((sd->GetFlags() & IContext::BIND_DEBUG_UNBIND) != 0) {
                AutoPtr<ServiceMap> holder = mUnboundServices[context];
                if (holder == NULL) {
                    holder = new ServiceMap();
                    mUnboundServices[context] = holder;
                }
//                RuntimeException ex = new IllegalArgumentException(
//                        "Originally unbound here:");
//                ex.fillInStackTrace();
//                sd.setUnbindLocation(ex);
                (*holder)[c] = sd;
            }
            AutoPtr<IIServiceConnection> con = sd->GetIServiceConnection();
            *result = con.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    AutoPtr<ServiceMap> holder;
    it = mUnboundServices.Find(context);
    if (it != mUnboundServices.End()) {
        holder = it->mSecond;
    }
    if (holder != NULL) {
        ServiceMapIterator sdit = holder->Find(c);
        if (sdit != holder->End()) {
            sd = sdit->mSecond;
        }
        if (sd != NULL) {
//            RuntimeException ex = sd.getUnbindLocation();
//            throw new IllegalArgumentException(
//                    "Unbinding Service " + c
//                    + " that was already unbound", ex);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    if (context == NULL) {
//        throw new IllegalStateException("Unbinding Service " + c
//                + " from Context that is no longer in use: " + context);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else {
//        throw new IllegalArgumentException("Service not registered: " + c);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

}
}
}
