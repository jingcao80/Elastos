
#include "elastos/droid/server/am/CServiceRecord.h"
#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/IntentBindRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/R.h>
#include <elastos/droid/server/LocalServices.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::CNotificationManagerHelper;
using Elastos::Droid::App::INotificationManagerHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Server::Notification::EIID_INotificationManagerInternal;
using Elastos::Droid::Server::Notification::INotificationManagerInternal;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IAppendable;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 CServiceRecord::MAX_DELIVERY_COUNT;
const Int32 CServiceRecord::MAX_DONE_EXECUTING_COUNT;

//==============================================================================
// CServiceRecord::StartItem
//==============================================================================

CServiceRecord::StartItem::StartItem(
    /* [in] */ CServiceRecord* sr,
    /* [in] */ Boolean taskRemoved,
    /* [in] */ Int32 id,
    /* [in] */ IIntent* intent,
    /* [in] */ NeededUriGrants* neededGrants)
    : mSr(sr)
    , mTaskRemoved(taskRemoved)
    , mId(id)
    , mIntent(intent)
    , mNeededGrants(neededGrants)
    , mDeliveredTime(0)
    , mDeliveryCount(0)
    , mDoneExecutingCount(0)
{
}

AutoPtr<UriPermissionOwner> CServiceRecord::StartItem::GetUriPermissionsLocked()
{
    if (mUriPermissions == NULL) {
        mUriPermissions = new UriPermissionOwner(mSr->mAms, this);
    }
    return mUriPermissions;
}

ECode CServiceRecord::StartItem::RemoveUriPermissionsLocked()
{
    if (mUriPermissions != NULL) {
        mUriPermissions->RemoveUriPermissionsLocked();
        mUriPermissions = NULL;
    }
    return NOERROR;
}

ECode CServiceRecord::StartItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

String CServiceRecord::StartItem::ToString()
{
    if (mStringName != NULL) {
        return mStringName;
    }
    StringBuilder sb(128);
    sb.Append("ServiceRecord{");
    sb.Append(StringUtils::ToHexString((Int32)mSr));
    sb.AppendChar(' ');
    sb.Append(mSr->mShortName);
    sb.Append(" StartItem ");
    sb.Append(StringUtils::ToHexString((Int32)this));
    sb.Append(" id=");
    sb.Append(mId);
    sb.AppendChar('}');
    return mStringName = sb.ToString();
}

//==============================================================================
// CServiceRecord::PostNotificationRunnable
//==============================================================================

ECode CServiceRecord::PostNotificationRunnable::Run()
{
    AutoPtr<INotificationManagerInternal> nm = INotificationManagerInternal::Probe(
        LocalServices::GetService(EIID_INotificationManagerInternal));
    if (nm == NULL) {
        return NOERROR;
    }
    // try {
    Int32 icon;
    mLocalForegroundNoti->GetIcon(&icon);
    if (icon == 0) {
        // It is not correct for the caller to supply a notification
        // icon, but this used to be able to slip through, so for
        // those dirty apps give it the app's icon.
        IPackageItemInfo* appInfo = IPackageItemInfo::Probe(mHost->mAppInfo);
        Int32 appIcon;
        appInfo->GetIcon(&appIcon);
        mLocalForegroundNoti->SetIcon(appIcon);

        // Do not allow apps to present a sneaky invisible content view either.
        mLocalForegroundNoti->SetContentView(NULL);
        mLocalForegroundNoti->SetBigContentView(NULL);
        AutoPtr<IPackageManager> pm;
        mHost->mAms->mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<ICharSequence> appName;
        appInfo->LoadLabel(pm, (ICharSequence**)&appName);
        String packageName;
        appInfo->GetPackageName(&packageName);
        if (appName == NULL) {
             appName = CoreUtils::Convert(packageName);
        }
        AutoPtr<IContext> ctx;
        if (FAILED(mHost->mAms->mContext->CreatePackageContext(
                packageName, 0, (IContext**)&ctx))) {
             mLocalForegroundNoti->SetIcon(0);
        }
        else {
            AutoPtr<IIntent> runningIntent;
            CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS,
                (IIntent**)&runningIntent);
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            AutoPtr<IUri> uri;
            uriHelper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
            runningIntent->SetData(uri);
            AutoPtr<IPendingIntentHelper> piHelper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
            AutoPtr<IPendingIntent> pi;
            piHelper->GetActivity(mHost->mAms->mContext, 0, runningIntent,
                IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pi);
            AutoPtr<IResources> res;
            mHost->mAms->mContext->GetResources((IResources**)&res);
            Int32 color;
            res->GetColor(R::color::system_notification_accent_color, &color);
            mLocalForegroundNoti->SetColor(color);
            AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
            params->Set(0, appName);
            String title, text;
            mHost->mAms->mContext->GetString(
                R::string::app_running_notification_title, params, &title);
            mHost->mAms->mContext->GetString(
                R::string::app_running_notification_text, params, &text);
            mLocalForegroundNoti->SetLatestEventInfo(ctx,
                CoreUtils::Convert(title), CoreUtils::Convert(text), pi);
        }
    }
    mLocalForegroundNoti->GetIcon(&icon);
    if (icon == 0) {
        // Notifications whose icon is 0 are defined to not show
        // a notification, silently ignoring it.  We don't want to
        // just ignore it, we want to prevent the service from
        // being foreground.
        Slogger::E(CActivityManagerService::TAG, "icon must be non-zero");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > outId = ArrayOf<Int32>::Alloc(1);
    AutoPtr<ArrayOf<Int32> > outIdTmp;
    ECode ec = nm->EnqueueNotification(mLocalPackageName, mLocalPackageName, mAppUid, mAppPid,
           String(NULL), mLocalForegroundId, mLocalForegroundNoti, outId, mHost->mUserId, (ArrayOf<Int32>**)&outIdTmp);
    outId = outIdTmp;
    if (FAILED(ec)) {
    // } catch (RuntimeException e) {
        Slogger::W(CActivityManagerService::TAG,
                "Error showing notification for service");
        // If it gave us a garbage notification, it doesn't
        // get to be foreground.
        mHost->mAms->SetServiceForeground(mHost->mName, mHost,
                0, NULL, TRUE);
        StringBuilder errorsb("Bad notification for startForeground: ");
        errorsb += (Int32)ec;
        mHost->mAms->CrashApplication(mAppUid, mAppPid, mLocalPackageName,
                errorsb.ToString());
    }
    return NOERROR;
}

//==============================================================================
// CServiceRecord::CancelNotificationRunnable
//==============================================================================

ECode CServiceRecord::CancelNotificationRunnable::Run()
{
    AutoPtr<INotificationManagerHelper> nmHelper;
    CNotificationManagerHelper::AcquireSingleton((INotificationManagerHelper**)&nmHelper);
    AutoPtr<IINotificationManager> inm;
    nmHelper->GetService((IINotificationManager**)&inm);
    if (inm == NULL) {
        return NOERROR;
    }
    // try {
    ECode ec = inm->CancelNotificationWithTag(mLocalPackageName, String(NULL),
                   mLocalForegroundId, mHost->mUserId);
    if (FAILED(ec)) {
    // } catch (RuntimeException e) {
    Slogger::W(CActivityManagerService::TAG,
               "Error canceling notification for service");
    // } catch (RemoteException e) {
    }
    return NOERROR;
}

//==============================================================================
// CServiceRecord::StripForegroundServiceFlagNotificationRunnable
//==============================================================================

ECode CServiceRecord::StripForegroundServiceFlagNotificationRunnable::Run()
{
    AutoPtr<INotificationManagerInternal> nm = INotificationManagerInternal::Probe(
        LocalServices::GetService(EIID_INotificationManagerInternal));
    if (nm == NULL) {
        return NOERROR;
    }
    return nm->RemoveForegroundServiceFlagFromNotification(
        mLocalPackageName, mLocalForegroundId, mLocalUserId);
}

//==============================================================================
// CServiceRecord
//==============================================================================

CAR_INTERFACE_IMPL(CServiceRecord, Object, IBinder)
CAR_OBJECT_IMPL(CServiceRecord)

CServiceRecord::CServiceRecord()
    : mAms(NULL)
    , mUserId(0)
    , mExported(FALSE)
    , mCreateTime(0)
    , mApp(0)
    , mIsolatedProc(0)
    , mDelayed(FALSE)
    , mIsForeground(FALSE)
    , mForegroundId(0)
    , mLastActivity(0)
    , mStartingBgTimeout(0)
    , mStartRequested(FALSE)
    , mDelayedStop(FALSE)
    , mStopIfKilled(FALSE)
    , mCallStart(FALSE)
    , mExecuteNesting(0)
    , mExecuteFg(FALSE)
    , mExecutingStart(0)
    , mCreatedFromFg(FALSE)
    , mCrashCount(0)
    , mTotalRestartCount(0)
    , mRestartCount(0)
    , mRestartDelay(0)
    , mRestartTime(0)
    , mNextRestartTime(0)
    , mLastStartId(0)
{
}

CServiceRecord::~CServiceRecord()
{
    mDeliveredStarts.Clear();
    mPendingStarts.Clear();
}

ECode CServiceRecord::constructor(
    /* [in] */ CActivityManagerService* ams,
    /* [in] */ IBatteryStatsImplUidPkgServ* servStats,
    /* [in] */ IComponentName* name,
    /* [in] */ IIntentFilterComparison* intent,
    /* [in] */ IServiceInfo* sInfo,
    /* [in] */ Boolean callerIsFg,
    /* [in] */ IRunnable* restarter)
{
    VALIDATE_NOT_NULL(name);
    VALIDATE_NOT_NULL(sInfo);

    mAms = ams;
    mStats = servStats;
    mName = name;
    name->FlattenToShortString(&mShortName);
    mIntent = intent;
    mServiceInfo = sInfo;
    IComponentInfo* serverInfo = IComponentInfo::Probe(sInfo);
    serverInfo->GetApplicationInfo((IApplicationInfo**)&mAppInfo);
    IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&mPackageName);
    serverInfo->GetProcessName(&mProcessName);
    sInfo->GetPermission(&mPermission);
    Int32 uid;
    mAppInfo->GetUid(&uid);
    serverInfo->GetExported(&mExported);
    mRestarter = restarter;
    mCreateTime = SystemClock::GetElapsedRealtime();
    mLastActivity = SystemClock::GetUptimeMillis();
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    uhHelper->GetUserId(uid, &mUserId);
    mCreatedFromFg = callerIsFg;
    return NOERROR;
}

void CServiceRecord::DumpStartList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ List<AutoPtr<StartItem> >* list,
    /* [in] */ Int32 now)
{
    List<AutoPtr<StartItem> >::Iterator it = list->Begin();
    for (Int32 i = 0; it != list->End(); ++it) {
        AutoPtr<StartItem> si = *it;
        pw->Print(prefix);
        pw->Print(String("#"));
        pw->Print(i);
        pw->Print(String(" id="));
        pw->Print(si->mId);
        if (now != 0) {
            pw->Print(String(" dur="));
            TimeUtils::FormatDuration(si->mDeliveredTime, now, pw);
        }
        if (si->mDeliveryCount != 0) {
            pw->Print(String(" dc="));
            pw->Print(si->mDeliveryCount);
        }
        if (si->mDoneExecutingCount != 0) {
            pw->Print(String(" dxc="));
            pw->Print(si->mDoneExecutingCount);
        }
        pw->Println(String(""));
        pw->Print(prefix);
        pw->Print(String("  intent="));
        if (si->mIntent != NULL)
            pw->Println(si->mIntent);
        else
            pw->Println(String("NULL"));
        if (si->mNeededGrants != NULL) {
            pw->Print(prefix);
            pw->Print(String("  neededGrants="));
            pw->Println(/*si->mNeededGrants*/);
        }
        if (si->mUriPermissions != NULL) {
            si->mUriPermissions->Dump(pw, prefix);
        }
    }
}

void CServiceRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("intent={"));
    AutoPtr<IIntent> intent;
    mIntent->GetIntent((IIntent**)&intent);
    String str;
    intent->ToShortString(FALSE, TRUE, FALSE, TRUE, &str);
    pw->Print(str);
    pw->PrintCharln('}');
    pw->Print(prefix);
    pw->Print(String("mPackageName="));
    pw->Println(mPackageName);
    pw->Print(prefix);
    pw->Print(String("mProcessName="));
    pw->Println(mProcessName);
    if (mPermission != NULL) {
        pw->Print(prefix);
        pw->Print(String("mPermission="));
        pw->Println(mPermission);
    }
    Int64 now = SystemClock::GetUptimeMillis();
    Int64 nowReal = SystemClock::GetElapsedRealtime();
    if (mAppInfo != NULL) {
        pw->Print(prefix);
        pw->Print(String("baseDir="));
        String sourceDir, publicSourceDir, dataDir;
        mAppInfo->GetSourceDir(&sourceDir);
        mAppInfo->GetPublicSourceDir(&publicSourceDir);
        mAppInfo->GetDataDir(&dataDir);
        pw->Println(sourceDir);
        if (!sourceDir.Equals(publicSourceDir)) {
            pw->Print(prefix);
            pw->Print(String("resDir="));
            pw->Println(publicSourceDir);
        }
        pw->Print(prefix);
        pw->Print(String("dataDir="));
        pw->Println(dataDir);
    }
    pw->Print(prefix);
    pw->Print(String("mApp="));
    pw->Println(TO_STR(mApp));
    if (mIsolatedProc != NULL) {
        pw->Print(prefix);
        pw->Print(String("mIsolatedProc="));
        pw->Println(mIsolatedProc->ToString());
    }
    if (mDelayed) {
        pw->Print(prefix);
        pw->Print(String("mDelayed="));
        pw->Println(mDelayed);
    }
    if (mIsForeground || mForegroundId != 0) {
        pw->Print(prefix);
        pw->Print(String("mIsForeground="));
        pw->Print(mIsForeground);
        pw->Print(String(" mForegroundId="));
        pw->Print(mForegroundId);
        pw->Print(String(" mForegroundNoti="));
        pw->Println(mForegroundNoti);
    }
    pw->Print(prefix);
    pw->Print(String("mCreateTime="));
    TimeUtils::FormatDuration(mCreateTime, nowReal, pw);
    pw->Print(String(" mStartingBgTimeout="));
    TimeUtils::FormatDuration(mStartingBgTimeout, now, pw);
    pw->Println();
    pw->Print(prefix);
    pw->Print(String("mLastActivity="));
    TimeUtils::FormatDuration(mLastActivity, now, pw);
    pw->Print(String(" mRestartTime="));
    TimeUtils::FormatDuration(mRestartTime, now, pw);
    pw->Print(String(" mCreatedFromFg="));
    pw->Println(mCreatedFromFg);
    if (mStartRequested || mDelayedStop || mLastStartId != 0) {
        pw->Print(prefix);
        pw->Print(String("mStartRequested="));
        pw->Print(mStartRequested);
        pw->Print(String(" mDelayedStop="));
        pw->Print(mDelayedStop);
        pw->Print(String(" mStopIfKilled="));
        pw->Print(mStopIfKilled);
        pw->Print(String(" mCallStart="));
        pw->Print(mCallStart);
        pw->Print(String(" mLastStartId="));
        pw->Println(mLastStartId);
    }
    if (mExecuteNesting != 0) {
        pw->Print(prefix);
        pw->Print(String("mExecuteNesting="));
        pw->Print(mExecuteNesting);
        pw->Print(String(" mExecuteFg="));
        pw->Print(mExecuteFg);
        pw->Print(String(" mExecutingStart="));
        TimeUtils::FormatDuration(mExecutingStart, now, pw);
        pw->Println();
    }
    if (mCrashCount != 0 || mRestartCount != 0
        || mRestartDelay != 0 || mNextRestartTime != 0) {
        pw->Print(prefix);
        pw->Print(String("mRestartCount="));
        pw->Print(mRestartCount);
        pw->Print(String(" mRestartDelay="));
        TimeUtils::FormatDuration(mRestartDelay, now, pw);
        pw->Print(String(" mNextRestartTime="));
        TimeUtils::FormatDuration(mNextRestartTime, now, pw);
        pw->Print(String(" mCrashCount="));
        pw->Println(mCrashCount);
    }
    if (mDeliveredStarts.GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("Delivered Starts:"));
        DumpStartList(pw, prefix, &mDeliveredStarts, now);
    }
    if (mPendingStarts.GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("Pending Starts:"));
        DumpStartList(pw, prefix, &mPendingStarts, 0);
    }
    if (mBindings.GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("Bindings:"));
        HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IntentBindRecord> >::Iterator it;
        Int32 i = 0;
        for (it = mBindings.Begin(); it != mBindings.End(); ++it, i++) {
            AutoPtr<IntentBindRecord> b = it->mSecond;
            pw->Print(prefix);
            pw->Print(String("* IntentBindRecord{"));
            pw->Print(StringUtils::ToString((Int32)b.Get(), 16));
            if ((b->CollectFlags() & IContext::BIND_AUTO_CREATE) != 0) {
                IAppendable::Probe(pw)->Append(CoreUtils::Convert(String(" CREATE")));
            }
            pw->Println(String("}:"));
            b->DumpInService(pw, prefix + "  ");
        }
    }
    if (mConnections.GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("All Connections:"));
        ConnectionIterator it = mConnections.Begin();
        for (; it != mConnections.End(); ++it) {
            AutoPtr<List<AutoPtr<ConnectionRecord> > > c = it->mSecond;
            List<AutoPtr<ConnectionRecord> >::Iterator iter;
            for (iter = c->Begin(); iter != c->End(); ++iter) {
                pw->Print(prefix);
                pw->Print(String("  "));
                pw->Println((*iter)->ToString());
            }
        }
    }
}

AutoPtr<IServiceState> CServiceRecord::GetTracker()
{
    if (mTracker != NULL) {
        return mTracker;
    }
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_PERSISTENT) == 0) {
        String packageName, processName, name;
        IPackageItemInfo::Probe(mServiceInfo)->GetPackageName(&packageName);
        IComponentInfo::Probe(mServiceInfo)->GetProcessName(&processName);
        IPackageItemInfo::Probe(mServiceInfo)->GetName(&name);
        Int32 uid, versionCode;
        appInfo->GetUid(&uid);
        appInfo->GetVersionCode(&versionCode);
        mTracker = mAms->mProcessStats->GetServiceStateLocked(packageName,
                uid, versionCode, processName, name);
        mTracker->ApplyNewOwner((IBinder*)this);
    }
    return mTracker;
}

void CServiceRecord::ForceClearTracker()
{
    if (mTracker != NULL) {
        mTracker->ClearCurrentOwner((IBinder*)this, TRUE);
        mTracker = NULL;
    }
}

void CServiceRecord::MakeRestarting(
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    if (mRestartTracker == NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 flags;
        appInfo->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_PERSISTENT) == 0) {
            String packageName, processName, name;
            IPackageItemInfo::Probe(mServiceInfo)->GetPackageName(&packageName);
            IComponentInfo::Probe(mServiceInfo)->GetProcessName(&processName);
            IPackageItemInfo::Probe(mServiceInfo)->GetName(&name);
            Int32 uid, versionCode;
            appInfo->GetUid(&uid);
            appInfo->GetVersionCode(&versionCode);
            mRestartTracker = mAms->mProcessStats->GetServiceStateLocked(packageName,
                    uid, versionCode, processName, name);
        }
        if (mRestartTracker == NULL) {
            return;
        }
    }
    mRestartTracker->SetRestarting(TRUE, memFactor, now);
}


AutoPtr<AppBindRecord> CServiceRecord::RetrieveAppBindingLocked(
    /* [in] */ IIntent* intent,
    /* [in] */ ProcessRecord* app)
{
    AutoPtr<IIntentFilterComparison> filter;
    CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&filter);
    AutoPtr<IntentBindRecord> i = mBindings[filter];
    if (i == NULL) {
        i = new IntentBindRecord(this, filter);
        mBindings[filter] = i;
    }
    AutoPtr<AppBindRecord> a = i->mApps[app];
    if (a != NULL) {
        return a;
    }
    a = new AppBindRecord(this, i, app);
    i->mApps[app] = a;
    return a;
}

Boolean CServiceRecord::HasAutoCreateConnections()
{
    // XXX should probably keep a count of the number of auto-create
    // connections directly in the service.
    ConnectionIterator it = mConnections.Begin();
    for (; it != mConnections.End(); ++it) {
        AutoPtr<List<AutoPtr<ConnectionRecord> > > cr = it->mSecond;
        List<AutoPtr<ConnectionRecord> >::Iterator iter;
        for (iter = cr->Begin(); iter != cr->End(); ++iter) {
            if (((*iter)->mFlags & IContext::BIND_AUTO_CREATE) != 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

ECode CServiceRecord::ResetRestartCounter()
{
    mRestartCount = 0;
    mRestartDelay = 0;
    mRestartTime = 0;
    return NOERROR;
}

AutoPtr<CServiceRecord::StartItem> CServiceRecord::FindDeliveredStart(
    /* [in] */ Int32 id,
    /* [in] */ Boolean remove)
{
    List<AutoPtr<StartItem> >::Iterator it = mDeliveredStarts.Begin();
    for (; it != mDeliveredStarts.End(); ++it) {
        AutoPtr<StartItem> si = *it;
        if (si->mId == id) {
            if (remove) mDeliveredStarts.Erase(it);
            return si;
        }
    }

    AutoPtr<StartItem> item;
    return item;
}

Int32 CServiceRecord::GetLastStartId()
{
    return mLastStartId;
}

Int32 CServiceRecord::MakeNextStartId()
{
    mLastStartId++;
    if (mLastStartId < 1) {
        mLastStartId = 1;
    }
    return mLastStartId;
}

ECode CServiceRecord::PostNotification()
{
    Int32 appUid;
    mAppInfo->GetUid(&appUid);
    const Int32 appPid = mApp->mPid;
    if (mForegroundId != 0 && mForegroundNoti != NULL) {
        // Do asynchronous communication with notification manager to
        // avoid deadlocks.
        const String localPackageName = mPackageName;
        const Int32 localForegroundId = mForegroundId;
        AutoPtr<INotification> localForegroundNoti = mForegroundNoti;
        AutoPtr<IRunnable> runnable = new PostNotificationRunnable(
            this, localPackageName, appUid, appPid, localForegroundId, localForegroundNoti);
        Boolean posted;
        mAms->mHandler->Post(runnable, &posted);
    }
    return NOERROR;
}

ECode CServiceRecord::CancelNotification()
{
    if (mForegroundId != 0) {
        // Do asynchronous communication with notification manager to
        // avoid deadlocks.
        const String localPackageName = mPackageName;
        const Int32 localForegroundId = mForegroundId;
        AutoPtr<IRunnable> runnable = new CancelNotificationRunnable(this, localPackageName, localForegroundId);
        Boolean posted;
        mAms->mHandler->Post(runnable, &posted);
    }
    return NOERROR;
}

ECode CServiceRecord::StripForegroundServiceFlagFromNotification()
{
    if (mForegroundId == 0) {
        return NOERROR;
    }

    Int32 localForegroundId = mForegroundId;
    Int32 localUserId = mUserId;
    String localPackageName = mPackageName;

    // Do asynchronous communication with notification manager to
    // avoid deadlocks.
    AutoPtr<IRunnable> runnable = new StripForegroundServiceFlagNotificationRunnable(
        localPackageName, localUserId, localForegroundId);
    Boolean posted;
    mAms->mHandler->Post(runnable, &posted);
    return NOERROR;
}


ECode CServiceRecord::ClearDeliveredStartsLocked()
{
    List<AutoPtr<StartItem> >::Iterator it = mDeliveredStarts.Begin();
    for (; it != mDeliveredStarts.End(); ++it) {
        AutoPtr<StartItem> si = *it;
        si->RemoveUriPermissionsLocked();
    }

    mDeliveredStarts.Clear();
    return NOERROR;
}

ECode CServiceRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String CServiceRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }

    StringBuilder sb(128);
    sb += "ServiceRecord{";
    sb += StringUtils::ToHexString((Int32)this);
    sb += " u";
    sb += mUserId;
    sb += ' ';
    sb += mShortName;
    sb += '}';
    return mStringName = sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
