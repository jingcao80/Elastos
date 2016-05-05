
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/server/notification/NotificationManagerService.h"
#include "elastos/droid/server/notification/CConditionProviders.h"
#include "elastos/droid/server/notification/CNotificationListeners.h"
#include "elastos/droid/server/notification/CNotificationManagerBinderService.h"
#include "elastos/droid/server/notification/CStatusBarNotificationHolder.h"
#include "elastos/droid/server/notification/RankingReconsideration.h"
#include "elastos/droid/server/notification/ValidateNotificationPeople.h"
#include "elastos/droid/server/notification/ZenLog.h"
#include "elastos/droid/server/lights/LightsManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
// #include "elastos/droid/internal/utility/CFastXmlSerializer.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CNotificationHelper;
using Elastos::Droid::App::INotificationHelper;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::App::IProfileGroup;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::App::EIID_IINotificationManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::Pm::CParceledListSlice;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
// using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Media::CAudioSystem;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::IIRingtonePlayer;
using Elastos::Droid::Media::CAudioAttributesHelper;
using Elastos::Droid::Media::IAudioAttributesHelper;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::IMediaSessionManager;
using Elastos::Droid::Media::Session::EIID_IMediaSessionManagerOnActiveSessionsChangedListener;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::Lights::EIID_ILightsManager;
using Elastos::Droid::Server::Lights::LightsManager;
using Elastos::Droid::Server::StatusBar::EIID_IStatusBarManagerInternal;
using Elastos::Droid::Service::Notification::CNotificationRankingUpdate;
using Elastos::Droid::Service::Notification::CStatusBarNotification;
using Elastos::Droid::Service::Notification::EIID_IIStatusBarNotificationHolder;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Service::Notification::INotificationListenerServiceRanking;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::R;
using Elastos::Core::CInteger32;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::IDeque;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::Objects;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::IExecutors;
using Elastos::Utility::Concurrent::CExecutors;
using Elastos::Utility::Concurrent::IFuture;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String NotificationManagerService::TAG("NotificationService");
const Boolean NotificationManagerService::DBG = TRUE;

const Int32 NotificationManagerService::MAX_PACKAGE_NOTIFICATIONS = 50;

const Int32 NotificationManagerService::MESSAGE_TIMEOUT;
const Int32 NotificationManagerService::MESSAGE_SAVE_POLICY_FILE;
const Int32 NotificationManagerService::MESSAGE_RECONSIDER_RANKING;
const Int32 NotificationManagerService::MESSAGE_RANKING_CONFIG_CHANGE;
const Int32 NotificationManagerService::MESSAGE_SEND_RANKING_UPDATE;
const Int32 NotificationManagerService::MESSAGE_LISTENER_HINTS_CHANGED;
const Int32 NotificationManagerService::MESSAGE_LISTENER_NOTIFICATION_FILTER_CHANGED;

const Int32 NotificationManagerService::LONG_DELAY = 3500; // 3.5 seconds
const Int32 NotificationManagerService::SHORT_DELAY = 2000; // 2 seconds

AutoPtr<ArrayOf<Int64> > InitDEFAULT_VIBRATE_PATTERN()
{
    AutoPtr<ArrayOf<Int64> > result = ArrayOf<Int64>::Alloc(4);
    result->Set(0, 0);
    result->Set(1, 250);
    result->Set(2, 250);
    result->Set(3, 250);
    return result;
}
const AutoPtr<ArrayOf<Int64> > NotificationManagerService::DEFAULT_VIBRATE_PATTERN = InitDEFAULT_VIBRATE_PATTERN();//{0, 250, 250, 250};
const Int32 NotificationManagerService::VIBRATE_PATTERN_MAXLEN = 8 * 2 + 1; // up to eight bumps

const Int32 NotificationManagerService::DEFAULT_STREAM_TYPE = IAudioManager::STREAM_NOTIFICATION;
const Boolean NotificationManagerService::SCORE_ONGOING_HIGHER = FALSE;

const Int32 NotificationManagerService::JUNK_SCORE = -1000;
const Int32 NotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER = 10;
const Int32 NotificationManagerService::SCORE_DISPLAY_THRESHOLD = INotification::PRIORITY_MIN * 10 /*NotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER*/;
const Int32 NotificationManagerService::SCORE_INTERRUPTION_THRESHOLD = INotification::PRIORITY_LOW * 10 /*NotificationManagerService::NOTIFICATION_PRIORITY_MULTIPLIER*/;

const Boolean NotificationManagerService::ENABLE_BLOCKED_NOTIFICATIONS = TRUE;
const Boolean NotificationManagerService::ENABLE_BLOCKED_TOASTS = TRUE;
const Int32 NotificationManagerService::MATCHES_CALL_FILTER_CONTACTS_TIMEOUT_MS = 3000;
const Float NotificationManagerService::MATCHES_CALL_FILTER_TIMEOUT_AFFINITY = ValidateNotificationPeople::STARRED_CONTACT;

const Int32 NotificationManagerService::DB_VERSION = 1;

const String NotificationManagerService::TAG_BODY("notification-policy");
const String NotificationManagerService::ATTR_VERSION("version");

const String NotificationManagerService::TAG_BLOCKED_PKGS("blocked-packages");
const String NotificationManagerService::TAG_PACKAGE("package");
const String NotificationManagerService::ATTR_NAME("name");

const Int32 NotificationManagerService::MY_UID = Process::MyUid();
const Int32 NotificationManagerService::MY_PID = Process::MyPid();
const Int32 NotificationManagerService::REASON_DELEGATE_CLICK = 1;
const Int32 NotificationManagerService::REASON_DELEGATE_CANCEL = 2;
const Int32 NotificationManagerService::REASON_DELEGATE_CANCEL_ALL = 3;
const Int32 NotificationManagerService::REASON_DELEGATE_ERROR = 4;
const Int32 NotificationManagerService::REASON_PACKAGE_CHANGED = 5;
const Int32 NotificationManagerService::REASON_USER_STOPPED = 6;
const Int32 NotificationManagerService::REASON_PACKAGE_BANNED = 7;
const Int32 NotificationManagerService::REASON_NOMAN_CANCEL = 8;
const Int32 NotificationManagerService::REASON_NOMAN_CANCEL_ALL = 9;
const Int32 NotificationManagerService::REASON_LISTENER_CANCEL = 10;
const Int32 NotificationManagerService::REASON_LISTENER_CANCEL_ALL = 11;
const Int32 NotificationManagerService::REASON_GROUP_SUMMARY_CANCELED = 12;
const String NotificationManagerService::IS_FILTERED_QUERY;// = NotificationTable.NORMALIZED_TEXT + "=? AND " +
        // PackageTable.PACKAGE_NAME + "=?";

static AutoPtr<IUri> InitFilterUri()
{
    AutoPtr<IUriBuilder> ub;
    CUriBuilder::New((IUriBuilder**)&ub);
    ub->Scheme(IContentResolver::SCHEME_CONTENT);
    // ub->Authority(ISpamFilter::AUTHORITY);
    ub->AppendPath(String("message"));
    AutoPtr<IUri> res;
    ub->Build((IUri**)&res);
    return res;
}

AutoPtr<IUri> NotificationManagerService::FILTER_MSG_URI = InitFilterUri();

static AutoPtr<IUri> InitUpdateUri()
{
    AutoPtr<IUriBuilder> ub;
    NotificationManagerService::FILTER_MSG_URI->BuildUpon((IUriBuilder**)&ub);
    ub->AppendEncodedPath(String("inc_count"));
    AutoPtr<IUri> res;
    ub->Build((IUri**)&res);
    return res;
}

AutoPtr<IUri> NotificationManagerService::UPDATE_MSG_URI = InitUpdateUri();

//===============================================================================
//                  NotificationManagerService::SpamExecutorRunnable
//===============================================================================

NotificationManagerService::SpamExecutorRunnable::SpamExecutorRunnable(
    /* [in] */ Int32 notifId,
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
    , mNotifId(notifId)
{}

ECode NotificationManagerService::SpamExecutorRunnable::Run()
{
    AutoPtr<IUriHelper> hlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
    AutoPtr<IUri> updateUri;
    hlp->WithAppendedPath(UPDATE_MSG_URI, StringUtils::ToString(mNotifId), (IUri**)&updateUri);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    Int32 res = 0;
    resolver->Update(updateUri, cv,
            String(NULL), NULL, &res);
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::BinderService
//===============================================================================

CAR_INTERFACE_IMPL_2(NotificationManagerService::BinderService, Object, IINotificationManager, IBinder);

NotificationManagerService::BinderService::BinderService()
{}

NotificationManagerService::BinderService::~BinderService()
{}

ECode NotificationManagerService::BinderService::constructor(
    /* [in] */ ISystemService* host)
{
    mHost = (NotificationManagerService*)host;
    return NOERROR;
}

ECode NotificationManagerService::BinderService::EnqueueToast(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback,
    /* [in] */ Int32 duration)
{
    if (DBG) {
        Slogger::I(TAG, "enqueueToast pkg=%s, callback=%p, duration=%d", pkg.string(), callback, duration);
    }

    if (pkg.IsNull() || callback == NULL) {
        Slogger::E(TAG, "Not doing toast. pkg=%s, callback=%p", pkg.string(), callback);
        return NOERROR;
    }

    Boolean isSystemToast = mHost->IsCallerSystem() || pkg.Equals("android");

    if (ENABLE_BLOCKED_TOASTS && !mHost->NoteNotificationOp(pkg, Binder::GetCallingUid())) {
        if (!isSystemToast) {
            StringBuilder buf;
            buf += "Suppressing toast from package ";
            buf += pkg;
            buf += " by user request.";
            Slogger::E(TAG, buf.ToString());
            return NOERROR;
        }
    }

    synchronized(mHost->mToastQueue) {
        Int32 callingPid = Binder::GetCallingPid();
        Int64 callingId = Binder::ClearCallingIdentity();

        // try {
        AutoPtr<NotificationManagerService::ToastRecord> record;
        Int32 index = mHost->IndexOfToastLocked(pkg, callback);

        // If it's already in the queue, we update it in place, we don't
        // move it to the end of the queue.
        if (index >= 0) {
            AutoPtr<IInterface> obj;
            mHost->mToastQueue->Get(index, (IInterface**)&obj);
            record = (NotificationManagerService::ToastRecord*)IObject::Probe(obj);
            record->Update(duration);
        }
        else {
            // Limit the number of toasts that any given package except the android
            // package can enqueue.  Prevents DOS attacks and deals with leaks.
            if (!isSystemToast) {
                Int32 count = 0;
                Int32 N;
                mHost->mToastQueue->GetSize(&N);
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<IInterface> obj;
                    mHost->mToastQueue->Get(i, (IInterface**)&obj);
                    AutoPtr<NotificationManagerService::ToastRecord> r = (NotificationManagerService::ToastRecord*)IObject::Probe(obj);
                    if (r->mPkg.Equals(pkg)) {
                        count++;

                        if (count >= MAX_PACKAGE_NOTIFICATIONS) {
                            StringBuilder buf;
                            buf += "Package has already posted ";
                            buf += count;
                            buf += " toasts. Not showing more. Package=";
                            buf += pkg;
                            Logger::E(TAG, buf.ToString());
                            Binder::RestoreCallingIdentity(callingId);
                            return NOERROR;
                        }
                    }
                }
            }

            record = new NotificationManagerService::ToastRecord(callingPid, pkg, callback, duration);
            mHost->mToastQueue->Add(TO_IINTERFACE(record));
            Int32 size;
            mHost->mToastQueue->GetSize(&size);
            index = size -1;
            mHost->KeepProcessAliveLocked(callingPid);
        }

        // If it's at index 0, it's the current toast.  It doesn't matter if it's
        // new or just been updated.  Call back and tell it to show itself.
        // If the callback fails, this will remove it from the list, so don't
        // assume that it's valid after this.
        if (index == 0) {
            mHost->ShowNextToastLocked();
        }

        Binder::RestoreCallingIdentity(callingId);
    }

    return NOERROR;
}

ECode NotificationManagerService::BinderService::CancelToast(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback)
{
    Slogger::I(TAG, "cancelToast pkg=%s, callback=%p", pkg.string(), callback);

    if (pkg.IsNull() || callback == NULL) {
        Slogger::E(TAG, "Not cancelling notification. pkg=%s, callback=%p", pkg.string(), callback);
        return NOERROR;
    }

    synchronized(mHost->mToastQueue) {
        Int64 callingId = Binder::ClearCallingIdentity();
        // try {
        Int32 index = mHost->IndexOfToastLocked(pkg, callback);
        if (index >= 0) {
            mHost->CancelToastLocked(index);
        }
        else {
            Slogger::W(TAG, "Toast already cancelled. pkg=%s, callback=%p", pkg.string(), callback);
        }

        Binder::RestoreCallingIdentity(callingId);
    }

    return NOERROR;
}

ECode NotificationManagerService::BinderService::EnqueueNotificationWithTag(
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ ArrayOf<Int32>* inIdReceived,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** outIdReceived)
{
    return mHost->EnqueueNotificationInternal(pkg, opPkg,
            Binder::GetCallingUid(),
            Binder::GetCallingPid(),
            tag, id, notification, inIdReceived, userId, outIdReceived);
}

ECode NotificationManagerService::BinderService::CancelNotificationWithTag(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    mHost->CheckCallerIsSystemOrSameApp(pkg);

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 tmpUserId;
    activityMgrHelper->HandleIncomingUser(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, TRUE, FALSE,
            String("cancelNotificationWithTag"), pkg, &tmpUserId);

    // Don't allow client applications to cancel foreground service notis.
    mHost->CancelNotification(Binder::GetCallingUid(),
            Binder::GetCallingPid(),
            pkg, tag, id, 0, Binder::GetCallingUid() == IProcess::SYSTEM_UID ?
            0 : INotification::FLAG_FOREGROUND_SERVICE,
            FALSE, tmpUserId, REASON_NOMAN_CANCEL, NULL);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::CancelAllNotifications(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    mHost->CheckCallerIsSystemOrSameApp(pkg);

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);

    Int32 tmpUserId;
    activityMgrHelper->HandleIncomingUser(Binder::GetCallingPid(),
            Binder::GetCallingUid(), userId, TRUE, FALSE,
            String("cancelAllNotifications"), pkg, &tmpUserId);

    // Calling from user space, don't allow the canceling of actively
    // running foreground services.
    mHost->CancelAllNotificationsInt(Binder::GetCallingUid(),
            Binder::GetCallingPid(),
            pkg, 0, INotification::FLAG_FOREGROUND_SERVICE, TRUE, tmpUserId,
            REASON_NOMAN_CANCEL_ALL, NULL);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetNotificationsEnabledForPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean enabled)
{
    mHost->CheckCallerIsSystem();

    mHost->SetNotificationsEnabledForPackageImpl(pkg, uid, enabled);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::AreNotificationsEnabledForPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    mHost->CheckCallerIsSystem();
    Int32 data;
    mHost->mAppOps->CheckOpNoThrow(IAppOpsManager::OP_POST_NOTIFICATION, uid, pkg, &data);
    *res = data == IAppOpsManager::MODE_ALLOWED;
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetPackagePriority(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 priority)
{
    mHost->CheckCallerIsSystem();
    mHost->mRankingHelper->SetPackagePriority(pkg, uid, priority);
    mHost->SavePolicyFile();
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetPackagePriority(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->CheckCallerIsSystem();
    return mHost->mRankingHelper->GetPackagePriority(pkg, uid, result);
}

ECode NotificationManagerService::BinderService::SetPackageVisibilityOverride(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 visibility)
{
    mHost->CheckCallerIsSystem();
    mHost->mRankingHelper->SetPackageVisibilityOverride(pkg, uid, visibility);
    mHost->SavePolicyFile();
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetPackageVisibilityOverride(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->CheckCallerIsSystem();
    return mHost->mRankingHelper->GetPackageVisibilityOverride(pkg, uid, result);
}

ECode NotificationManagerService::BinderService::SetShowNotificationForPackageOnKeyguard(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 status)
{
    CheckCallerIsSystem();
    mHost->mRankingHelper->SetShowNotificationForPackageOnKeyguard(pkg, uid, status);
    mHost->SavePolicyFile();
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetShowNotificationForPackageOnKeyguard(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnforceSystemOrSystemUI(String("INotificationManager.getShowNotificationForPackageOnKeyguard"));
    return mHost->mRankingHelper->GetShowNotificationForPackageOnKeyguard(pkg, uid, result);
}

ECode NotificationManagerService::BinderService::GetActiveNotifications(
    /* [in] */ const String& callingPkg,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ns)
{
    VALIDATE_NOT_NULL(ns);

    // enforce() will ensure the calling uid has the correct permission
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    context->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_NOTIFICATIONS,
            String("NotificationManagerService.getActiveNotifications"));

    AutoPtr< ArrayOf<IStatusBarNotification*> > tmp;
    Int32 uid = Binder::GetCallingUid();

    // noteOp will check to make sure the callingPkg matches the uid
    Int32 result;
    mHost->mAppOps->NoteOpNoThrow(IAppOpsManager::OP_ACCESS_NOTIFICATIONS, uid, callingPkg, &result);
    if (result == IAppOpsManager::MODE_ALLOWED) {
        synchronized(mHost->mNotificationList) {
            Int32 N;
            mHost->mNotificationList->GetSize(&N);
            tmp = ArrayOf<IStatusBarNotification*>::Alloc(N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mHost->mNotificationList->Get(i, (IInterface**)&obj);
                AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
                (*tmp)[i] = record->mSbn;
            }
        }
    }

    *ns = tmp;
    REFCOUNT_ADD(*ns);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetHistoricalNotifications(
    /* [in] */ const String& callingPkg,
    /* [in] */ Int32 count,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ns)
{
    VALIDATE_NOT_NULL(ns);

    // enforce() will ensure the calling uid has the correct permission
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    context->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_NOTIFICATIONS,
            String("NotificationManagerService.getHistoricalNotifications"));

    AutoPtr< ArrayOf<IStatusBarNotification*> > tmp;
    Int32 uid = Binder::GetCallingUid();

    // noteOp will check to make sure the callingPkg matches the uid
    Int32 result;
    mHost->mAppOps->NoteOpNoThrow(IAppOpsManager::OP_ACCESS_NOTIFICATIONS, uid, callingPkg, &result);
    if (result == IAppOpsManager::MODE_ALLOWED) {
        synchronized(mHost->mArchive) {
            mHost->mArchive->GetArray(count, (ArrayOf<IStatusBarNotification*>**)&tmp);
        }
    }
    *ns = tmp;
    REFCOUNT_ADD(*ns);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::RegisterListener(
    /* [in] */ IINotificationListener* listener,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userid)
{
    EnforceSystemOrSystemUI(String("INotificationManager.registerListener"));
    mHost->mListeners->RegisterService(listener, component, userid);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::UnregisterListener(
    /* [in] */ IINotificationListener* listener,
    /* [in] */ Int32 userid)
{
    mHost->mListeners->UnregisterService(listener, userid);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::CancelNotificationsFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ ArrayOf<String>* keys)
{
    const Int32 callingUid = Binder::GetCallingUid();
    const Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        if (keys != NULL) {
            const Int32 N = keys->GetLength();
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mHost->mNotificationsByKey->Get(CoreUtils::Convert((*keys)[i]), (IInterface**)&obj);
                AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
                if (r == NULL) continue;
                Int32 userId;
                r->mSbn->GetUserId(&userId);
                if (userId != info->mUserid && userId != IUserHandle::USER_ALL &&
                        !mHost->mUserProfiles->IsCurrentProfile(userId)) {
                    Binder::RestoreCallingIdentity(identity);
                    Slogger::E(TAG, "Disallowed call from listener: %p", info->mService.Get());
                    return E_SECURITY_EXCEPTION;
                    // throw new SecurityException("Disallowed call from listener: "
                    //         + info.service);
                }
                String pkg, tag;
                Int32 id;
                r->mSbn->GetPackageName(&pkg);
                r->mSbn->GetTag(&tag);
                r->mSbn->GetId(&id);
                CancelNotificationFromListenerLocked(info, callingUid, callingPid,
                        pkg, tag, id, userId);
            }
        }
        else {
            mHost->CancelAllLocked(callingUid, callingPid, info->mUserid,
                    REASON_LISTENER_CANCEL_ALL, info, info->SupportsProfiles());
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

void NotificationManagerService::BinderService::CancelNotificationFromListenerLocked(
    /* [in] */ ManagedServices::ManagedServiceInfo* info,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    mHost->CancelNotification(callingUid, callingPid, pkg, tag, id, 0,
            INotification::FLAG_ONGOING_EVENT | INotification::FLAG_FOREGROUND_SERVICE,
            TRUE, userId, REASON_LISTENER_CANCEL, info);
}

ECode NotificationManagerService::BinderService::CancelNotificationFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    const Int32 callingUid = Binder::GetCallingUid();
    const Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        if (info->SupportsProfiles()) {
            Logger::E(TAG, "Ignoring deprecated cancelNotification(pkg, tag, id) from %p use cancelNotification(key) instead."
                    , info->mComponent.Get());
        }
        else {
            CancelNotificationFromListenerLocked(info, callingUid, callingPid,
                    pkg, tag, id, info->mUserid);
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetActiveNotificationsFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ Int32 trim,
    /* [out] */ IParceledListSlice** slice)
{
    VALIDATE_NOT_NULL(slice);

    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        const Boolean getKeys = keys != NULL;
        Int32 size;
        mHost->mNotificationList->GetSize(&size);
        const Int32 N = getKeys ? keys->GetLength() : size;
        AutoPtr<IArrayList> list;
        CArrayList::New(N, (IArrayList**)&list);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<NotificationRecord> r;
            if (getKeys) {
                AutoPtr<IInterface> obj;
                mHost->mNotificationsByKey->Get(CoreUtils::Convert((*keys)[i]), (IInterface**)&obj);
                r = (NotificationRecord*)INotificationRecord::Probe(obj);
            }
            else {
                AutoPtr<IInterface> obj;
                mHost->mNotificationList->Get(i, (IInterface**)&obj);
                r = (NotificationRecord*)INotificationRecord::Probe(obj);
            }
            if (r == NULL) continue;
            AutoPtr<IStatusBarNotification> sbn = r->mSbn;
            if (!mHost->IsVisibleToListener(sbn, info)) continue;
            AutoPtr<IStatusBarNotification> sbnToSend;
            if (trim != INotificationListenerService::TRIM_FULL) {
                AutoPtr<IStatusBarNotification> action;
                sbn->CloneLight((IStatusBarNotification**)&action);
                sbnToSend = action;
            }
            else {
                sbnToSend = sbn;
            }
            list->Add(sbnToSend);
        }
        return CParceledListSlice::New(IList::Probe(list), slice);
    }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::RequestHintsFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ Int32 hints)
{
    const Int64 identity = Binder::ClearCallingIdentity();
    // try {
    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        const Boolean disableEffects = (hints & INotificationListenerService::HINT_HOST_DISABLE_EFFECTS) != 0;
        if (disableEffects) {
            ISet::Probe(mHost->mListenersDisablingEffects)->Add((IProxyDeathRecipient*)info);
        }
        else {
            ISet::Probe(mHost->mListenersDisablingEffects)->Remove((IProxyDeathRecipient*)info);
        }
        mHost->UpdateListenerHintsLocked();
        mHost->UpdateEffectsSuppressorLocked();
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetHintsFromListener(
    /* [in] */ IINotificationListener* token,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized(mHost->mNotificationList) {
        *result = mHost->mListenerHints;
        return NOERROR;
    }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::RequestInterruptionFilterFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ Int32 interruptionFilter)
{
    const Int64 identity = Binder::ClearCallingIdentity();
    // try {
    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        mHost->mZenModeHelper->RequestFromListener(interruptionFilter);
        mHost->UpdateInterruptionFilterLocked();
    }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetInterruptionFilterFromListener(
    /* [in] */ IINotificationListener* token,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized(mHost->mNotificationLight) {
        *result = mHost->mInterruptionFilter;
        return NOERROR;
    }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetOnNotificationPostedTrimFromListener(
    /* [in] */ IINotificationListener* token,
    /* [in] */ Int32 trim)
{
    synchronized(mHost->mNotificationList) {
        AutoPtr<ManagedServices::ManagedServiceInfo> info;
        mHost->mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
        if (info == NULL) return NOERROR;
        mHost->mListeners->SetOnNotificationPostedTrimLocked(info, trim);
        return NOERROR;
    }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetZenModeConfig(
    /* [out] */ IZenModeConfig** config)
{
    VALIDATE_NOT_NULL(config);
    EnforceSystemOrSystemUI(String("INotificationManager.getZenModeConfig"));
    AutoPtr<IZenModeConfig> _config = mHost->mZenModeHelper->GetConfig();
    *config = _config;
    REFCOUNT_ADD(*config);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetZenModeConfig(
    /* [in] */ IZenModeConfig* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->CheckCallerIsSystem();
    *result = mHost->mZenModeHelper->SetConfig(config);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::NotifyConditions(
    /* [in] */ const String& pkg,
    /* [in] */ IIConditionProvider* provider,
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    AutoPtr<ManagedServices::ManagedServiceInfo> info = mHost->mConditionProviders->CheckServiceToken(provider);
    mHost->CheckCallerIsSystemOrSameApp(pkg);
    const Int64 identity = Binder::ClearCallingIdentity();
    // try {
    mHost->mConditionProviders->NotifyConditions(pkg, info, conditions);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::RequestZenModeConditions(
    /* [in] */ IIConditionListener* callback,
    /* [in] */ Int32 relevance)
{
    EnforceSystemOrSystemUI(String("INotificationManager.requestZenModeConditions"));
    mHost->mConditionProviders->RequestZenModeConditions(callback, relevance);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetZenModeCondition(
    /* [in] */ ICondition* condition)
{
    EnforceSystemOrSystemUI(String("INotificationManager.setZenModeCondition"));
    const Int64 identity = Binder::ClearCallingIdentity();
    // try {
    mHost->mConditionProviders->SetZenModeCondition(condition, String("binderCall"));
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode NotificationManagerService::BinderService::SetAutomaticZenModeConditions(
    /* [in] */ ArrayOf<IUri*>* conditionIds)
{
    EnforceSystemOrSystemUI(String("INotificationManager.setAutomaticZenModeConditions"));
    mHost->mConditionProviders->SetAutomaticZenModeConditions(conditionIds);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::GetAutomaticZenModeConditions(
    /* [out, callee] */ ArrayOf<ICondition*>** condition)
{
    VALIDATE_NOT_NULL(condition);
    EnforceSystemOrSystemUI(String("INotificationManager.getAutomaticZenModeConditions"));
    AutoPtr< ArrayOf<ICondition*> > con = mHost->mConditionProviders->GetAutomaticZenModeConditions();
    *condition = con;
    REFCOUNT_ADD(*condition);
    return NOERROR;;
}

void NotificationManagerService::BinderService::EnforceSystemOrSystemUI(
    /* [in] */ const String& message)
{
    if (IsCallerSystem()) return;
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    context->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE, message);
}

void NotificationManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    Int32 result;
    context->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DUMP, &result);
    if (result != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder builder;
        builder += "Permission Denial: can't dump NotificationManager from pid=";
        builder += Binder::GetCallingPid();
        builder += ", uid=";
        builder += Binder::GetCallingUid();

        pw->Println(builder.ToString());
        return;
    }

    mHost->DumpImpl(pw, DumpFilter::ParseFromArguments(args));
}

ECode NotificationManagerService::BinderService::GetEffectsSuppressor(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    EnforceSystemOrSystemUI(String("INotificationManager.getEffectsSuppressor"));
    *name = mHost->mEffectsSuppressor;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::MatchesCallFilter(
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    EnforceSystemOrSystemUI(String("INotificationManager.matchesCallFilter"));

    AutoPtr<INotificationSignalExtractor> actor =
            mHost->mRankingHelper->FindExtractor(EIID_IValidateNotificationPeople);

    *result = mHost->mZenModeHelper->MatchesCallFilter(
            UserHandle::GetCallingUserHandle(),
            extras,
            (ValidateNotificationPeople*)IValidateNotificationPeople::Probe(actor),
            MATCHES_CALL_FILTER_CONTACTS_TIMEOUT_MS,
            MATCHES_CALL_FILTER_TIMEOUT_AFFINITY);
    return NOERROR;
}

ECode NotificationManagerService::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "NotificationManagerService::BinderService: ";
    str->AppendFormat("%p", this);
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::NotificationListeners
//===============================================================================
NotificationManagerService::NotificationListeners::NotificationListeners()
{}

NotificationManagerService::NotificationListeners::~NotificationListeners()
{}

ECode NotificationManagerService::NotificationListeners::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (NotificationManagerService*) IObject::Probe(host);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    ManagedServices::constructor(context, mHost->mHandler,
            mHost->mNotificationList, mHost->mUserProfiles);

    CArraySet::New((IArraySet**)&mLightTrimListeners);
    return NOERROR;
}

AutoPtr<ManagedServices::Config> NotificationManagerService::NotificationListeners::GetConfig()
{
    AutoPtr<Config> c = new Config();
    c->mCaption = "notification listener";
    c->mServiceInterface = INotificationListenerService::SERVICE_INTERFACE;
    c->mSecureSettingName = ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS;
    c->mBindPermission = Elastos::Droid::Manifest::permission::BIND_NOTIFICATION_LISTENER_SERVICE;
    c->mSettingsAction = ISettings::ACTION_NOTIFICATION_LISTENER_SETTINGS;
    c->mClientLabel = R::string::notification_listener_binding_label;
    return c;
}

ECode NotificationManagerService::NotificationListeners::OnServiceAdded(
/* [in] */ ManagedServiceInfo* info)
{
    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    AutoPtr<INotificationRankingUpdate> update;
    synchronized(mHost->mNotificationList) {
        update = mHost->MakeRankingUpdateLocked(info);
    }
    // try {
    ECode ec = listener->OnListenerConnected(update);
    if (FAILED(ec)) {
        // we tried
    }
    // } catch (RemoteException e) {
    //     // we tried
    // }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::OnServiceRemovedLocked(
/* [in] */ ManagedServiceInfo* removed)
{
    Boolean res;
    ISet::Probe(mHost->mListenersDisablingEffects)->Remove((IProxyDeathRecipient*)removed, &res);
    if (res) {
        mHost->UpdateListenerHintsLocked();
    }
    ISet::Probe(mLightTrimListeners)->Remove((IProxyDeathRecipient*)removed);
    return NOERROR;
}

void NotificationManagerService::NotificationListeners::SetOnNotificationPostedTrimLocked(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ Int32 trim)
{
    if (trim == INotificationListenerService::TRIM_LIGHT) {
        ISet::Probe(mLightTrimListeners)->Add((IProxyDeathRecipient*)info);
    }
    else {
        ISet::Probe(mLightTrimListeners)->Remove((IProxyDeathRecipient*)info);
    }
}

Int32 NotificationManagerService::NotificationListeners::GetOnNotificationPostedTrim(
    /* [in] */ ManagedServiceInfo* info)
{
    Boolean res;
    ISet::Probe(mLightTrimListeners)->Contains((IProxyDeathRecipient*)info, &res);
    if (res) {
        return INotificationListenerService::TRIM_LIGHT;
    }
    return INotificationListenerService::TRIM_FULL;
}

ECode NotificationManagerService::NotificationListeners::NotifyPostedLocked(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ IStatusBarNotification* oldSbn)
{
    // Lazily initialized snapshots of the notification.
    AutoPtr<IStatusBarNotification> sbnClone;
    AutoPtr<IStatusBarNotification> sbnCloneLight;

    Int32 size = 0;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        Boolean sbnVisible = mHost->IsVisibleToListener(sbn, info);
        Boolean oldSbnVisible = oldSbn != NULL ? mHost->IsVisibleToListener(oldSbn, info) : FALSE;
        // This notification hasn't been and still isn't visible -> ignore.
        if (!oldSbnVisible && !sbnVisible) {
            continue;
        }
        AutoPtr<INotificationRankingUpdate> update = mHost->MakeRankingUpdateLocked(info);

        AutoPtr<NotificationListenersRunnable> runnable;
        Boolean res;
        // This notification became invisible -> remove the old one.
        if (oldSbnVisible && !sbnVisible) {
            AutoPtr<IStatusBarNotification> oldSbnLightClone;
            oldSbn->CloneLight((IStatusBarNotification**)&oldSbnLightClone);
            runnable = new NotificationListenersRunnable(0,
                    this, mHost, info, oldSbnLightClone, update, 0);
            mHost->mHandler->Post(runnable, &res);
            continue;
        }

        Int32 trim = mHost->mListeners->GetOnNotificationPostedTrim(info);

        if (trim == INotificationListenerService::TRIM_LIGHT && sbnCloneLight == NULL) {
            sbn->CloneLight((IStatusBarNotification**)&sbnCloneLight);
        }
        else if (trim == INotificationListenerService::TRIM_FULL && sbnClone == NULL) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe(sbn)->Clone((IInterface**)&obj);
            sbnClone = IStatusBarNotification::Probe(obj);
        }
        AutoPtr<IStatusBarNotification> sbnToPost =
                (trim == INotificationListenerService::TRIM_FULL) ? sbnClone : sbnCloneLight;

        runnable = NULL;
        runnable = new NotificationListenersRunnable(1, this, mHost, info, sbnToPost, update, 0);
        mHost->mHandler->Post(runnable, &res);
    }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyRemovedLocked(
    /* [in] */ IStatusBarNotification* sbn)
{
    // make a copy in case changes are made to the underlying Notification object
    // NOTE: this copy is lightweight: it doesn't include heavyweight parts of the
    // notification
    AutoPtr<IStatusBarNotification> sbnLight;
    sbn->CloneLight((IStatusBarNotification**)&sbnLight);

    Int32 size;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        if (!mHost->IsVisibleToListener(sbn, info)) {
            continue;
        }
        AutoPtr<INotificationRankingUpdate> update = mHost->MakeRankingUpdateLocked(info);
        AutoPtr<NotificationListenersRunnable> runnable = new NotificationListenersRunnable(
                2, this, mHost, info, sbnLight, update, 0);
        Boolean res;
        mHost->mHandler->Post(runnable, &res);
    }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyRankingUpdateLocked()
{
    Int32 size;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> serviceInfo = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        if (!serviceInfo->IsEnabledForCurrentProfiles()) {
            continue;
        }

        AutoPtr<INotificationRankingUpdate> update = mHost->MakeRankingUpdateLocked(serviceInfo);
        AutoPtr<NotificationListenersRunnable> runnable = new NotificationListenersRunnable(
                3, this, mHost, serviceInfo, NULL, update, 0);
        Boolean res;
        mHost->mHandler->Post(runnable, &res);
    }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyListenerHintsChangedLocked(
    /* [in] */ Int32 hints)
{
    Int32 size = 0;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> serviceInfo = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        if (!serviceInfo->IsEnabledForCurrentProfiles()) {
            continue;
        }

        AutoPtr<NotificationListenersRunnable> runnable = new NotificationListenersRunnable(
                4, this, mHost, serviceInfo, NULL, NULL, hints);
        Boolean res;
        mHost->mHandler->Post(runnable, &res);
    }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyInterruptionFilterChanged(
    /* [in] */ Int32 interruptionFilter)
{
    Int32 size = 0;
    mServices->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> serviceInfo = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);

        if (!serviceInfo->IsEnabledForCurrentProfiles()) {
            continue;
        }

        AutoPtr<NotificationListenersRunnable> runnable = new NotificationListenersRunnable(
                5, this, mHost, serviceInfo, NULL, NULL, interruptionFilter);
        Boolean res;
        mHost->mHandler->Post(runnable, &res);
    }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyPosted(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ INotificationRankingUpdate* rankingUpdate)
{
    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    AutoPtr<IIStatusBarNotificationHolder> sbnHolder;
    CStatusBarNotificationHolder::New(sbn, (IIStatusBarNotificationHolder**)&sbnHolder);
    // try {
    ECode ec = listener->OnNotificationPosted(sbnHolder, rankingUpdate);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to notify listener (posted): %p", listener.Get());
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "unable to notify listener (posted): " + listener, ex);
    // }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyRemoved(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ INotificationRankingUpdate* rankingUpdate)
{
    Int32 id;
    sbn->GetUserId(&id);
    if (!info->EnabledAndUserMatches(id)) {
        return NOERROR;
    }

    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    AutoPtr<IIStatusBarNotificationHolder> sbnHolder;
    CStatusBarNotificationHolder::New(sbn, (IIStatusBarNotificationHolder**)&sbnHolder);
    // try {
    ECode ec = listener->OnNotificationRemoved(sbnHolder, rankingUpdate);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to notify listener (removed): %p", listener.Get());
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "unable to notify listener (removed): " + listener, ex);
    // }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyRankingUpdate(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ INotificationRankingUpdate* rankingUpdate)
{
    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    // try {
    ECode ec = listener->OnNotificationRankingUpdate(rankingUpdate);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to notify listener (ranking update): %p", listener.Get());
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "unable to notify listener (ranking update): " + listener, ex);
    // }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyListenerHintsChanged(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ Int32 hints)
{
    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    // try {
    ECode ec = listener->OnListenerHintsChanged(hints);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to notify listener (listener hints): %p", listener.Get());
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "unable to notify listener (listener hints): " + listener, ex);
    // }
    return NOERROR;
}

ECode NotificationManagerService::NotificationListeners::NotifyInterruptionFilterChanged(
    /* [in] */ ManagedServiceInfo* info,
    /* [in] */ Int32 interruptionFilter)
{
    AutoPtr<IINotificationListener> listener = IINotificationListener::Probe(info->mService);
    // try {
    ECode ec = listener->OnInterruptionFilterChanged(interruptionFilter);
    if (FAILED(ec)) {
        Logger::E(TAG, "unable to notify listener (interruption filter): %p", listener.Get());
    }
    // } catch (RemoteException ex) {
    //     Log.e(TAG, "unable to notify listener (interruption filter): " + listener, ex);
    // }
    return NOERROR;
}

Boolean NotificationManagerService::NotificationListeners::IsListenerPackage(
    /* [in] */ const String& packageName)
{
    if (packageName.IsNull()) {
        return FALSE;
    }
    // TODO: clean up locking object later
    synchronized(mHost->mNotificationList) {
        Int32 size;
        mServices->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            AutoPtr<ManagedServiceInfo> serviceInfo = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
            String name;
            serviceInfo->mComponent->GetPackageName(&name);
            if (packageName.Equals(name)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

//===============================================================================
//                  NotificationManagerService::StatusBarNotificationHolder
//===============================================================================

CAR_INTERFACE_IMPL_2(NotificationManagerService::StatusBarNotificationHolder, Object, IIStatusBarNotificationHolder, IBinder);

NotificationManagerService::StatusBarNotificationHolder::StatusBarNotificationHolder()
{}

NotificationManagerService::StatusBarNotificationHolder::~StatusBarNotificationHolder()
{}

ECode NotificationManagerService::StatusBarNotificationHolder::constructor(
    /* [in] */ IStatusBarNotification* value)
{
    mValue = value;
    return NOERROR;
}

ECode NotificationManagerService::StatusBarNotificationHolder::Get(
    /* [out] */ IStatusBarNotification** ret)
{
    VALIDATE_NOT_NULL(ret);

    AutoPtr<IStatusBarNotification> value = mValue;
    mValue = NULL;
    *ret = value;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode NotificationManagerService::StatusBarNotificationHolder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "NotificationManagerService::StatusBarNotificationHolder: ";
    str->AppendFormat("%p", this);
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::MediaSessionManagerOnActiveSessionsChangedListener
//===============================================================================
CAR_INTERFACE_IMPL(NotificationManagerService::MediaSessionManagerOnActiveSessionsChangedListener, Object,
                    IMediaSessionManagerOnActiveSessionsChangedListener)

NotificationManagerService::MediaSessionManagerOnActiveSessionsChangedListener::MediaSessionManagerOnActiveSessionsChangedListener(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

ECode NotificationManagerService::MediaSessionManagerOnActiveSessionsChangedListener::OnActiveSessionsChanged(
    /* [in] */ IList* controllers)
{
    AutoPtr<IIterator> it;
    controllers->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMediaController> activeSession = IMediaController::Probe(p);
        AutoPtr<IPlaybackState> playbackState;
        activeSession->GetPlaybackState((IPlaybackState**)&playbackState);
        if (playbackState != NULL) {
            Int32 stat = 0;
            playbackState->GetState(&stat);
            if (stat == IPlaybackState::STATE_PLAYING) {
                mHost->mActiveMedia = TRUE;
                return NOERROR;
            }
        }
    }
    mHost->mActiveMedia = FALSE;
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::SettingsObserver
//===============================================================================

NotificationManagerService::SettingsObserver::SettingsObserver(
    /* [in] */ NotificationManagerService* host,
    /* [in] */ IHandler* handler)
    : mHost(host)
{
    ContentObserver::constructor(handler);
    AutoPtr<ISettingsSystem> systemSettings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&systemSettings);
    systemSettings->GetUriFor(ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
            (IUri**)&NOTIFICATION_LIGHT_PULSE_URI);
    AutoPtr<ISettingsSecure> systemSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&systemSecure);
    systemSecure->GetUriFor(ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS,
            (IUri**)&ENABLED_NOTIFICATION_LISTENERS_URI);
}

NotificationManagerService::SettingsObserver::~SettingsObserver()
{}

void NotificationManagerService::SettingsObserver::Observe()
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(
            NOTIFICATION_LIGHT_PULSE_URI, FALSE, this, IUserHandle::USER_ALL);
    resolver->RegisterContentObserver(
            ENABLED_NOTIFICATION_LISTENERS_URI, FALSE, this, IUserHandle::USER_ALL);
    AutoPtr<ISettingsSystem> systemSettings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&systemSettings);
    AutoPtr<IUri> uriColor;
    systemSettings->GetUriFor(
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_COLOR, (IUri**)&uriColor);
    resolver->RegisterContentObserver(uriColor,
            FALSE, this, IUserHandle::USER_ALL);

    AutoPtr<IUri> uriLedOn;
    systemSettings->GetUriFor(
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_LED_ON, (IUri**)&uriLedOn);
    resolver->RegisterContentObserver(uriLedOn,
            FALSE, this, IUserHandle::USER_ALL);

    AutoPtr<IUri> uriLedOff;
    systemSettings->GetUriFor(
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_LED_OFF, (IUri**)&uriLedOff);
    resolver->RegisterContentObserver(uriLedOff,
            FALSE, this, IUserHandle::USER_ALL);

    AutoPtr<IUri> uriCusEnb;
    systemSettings->GetUriFor(
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CUSTOM_ENABLE, (IUri**)&uriCusEnb);
    resolver->RegisterContentObserver(uriCusEnb,
            FALSE, this, IUserHandle::USER_ALL);

    AutoPtr<IUri> uriCusVal;
    systemSettings->GetUriFor(
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CUSTOM_VALUES, (IUri**)&uriCusVal);
    resolver->RegisterContentObserver(uriCusVal,
            FALSE, this, IUserHandle::USER_ALL);

    AutoPtr<ISettingsGlobal> setGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&setGlobal);
    AutoPtr<IUri> uriPlayBack;
    setGlobal->GetUriFor(
            ISettingsGlobal::ZEN_DISABLE_DUCKING_DURING_MEDIA_PLAYBACK, (IUri**)&uriPlayBack);
    resolver->RegisterContentObserver(uriPlayBack, FALSE,
            this, IUserHandle::USER_ALL);

    Update(NULL);
}

ECode NotificationManagerService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Update(uri);
    return NOERROR;
}

void NotificationManagerService::SettingsObserver::Update(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> systemSettings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&systemSettings);
    // LED enabled
    Int32 pulse = 0;
    systemSettings->GetInt32ForUser(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE, 0, IUserHandle::USER_CURRENT, &pulse);
    mHost->mNotificationPulseEnabled = pulse != 0;

    // LED default color
    systemSettings->GetInt32ForUser(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_COLOR,
            mHost->mDefaultNotificationColor, IUserHandle::USER_CURRENT, &(mHost->mDefaultNotificationColor));

    // LED default on MS
    systemSettings->GetInt32ForUser(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_LED_ON,
            mHost->mDefaultNotificationLedOn, IUserHandle::USER_CURRENT, &(mHost->mDefaultNotificationLedOn));

    // LED default off MS
    systemSettings->GetInt32ForUser(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_DEFAULT_LED_OFF,
            mHost->mDefaultNotificationLedOff, IUserHandle::USER_CURRENT, &(mHost->mDefaultNotificationLedOff));

    // LED custom notification colors
    mHost->mNotificationPulseCustomLedValues->Clear();
    Int32 enble = 0;
    systemSettings->GetInt32ForUser(resolver,
            ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CUSTOM_ENABLE, 0,
            IUserHandle::USER_CURRENT, &enble);
    if (enble != 0) {
        String val;
        systemSettings->GetStringForUser(resolver,
                ISettingsSystem::NOTIFICATION_LIGHT_PULSE_CUSTOM_VALUES,
                IUserHandle::USER_CURRENT, &val);
        mHost->ParseNotificationPulseCustomValuesString(val);
    }

    mHost->UpdateNotificationPulse();

    AutoPtr<ISettingsGlobal> setGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&setGlobal);
    Int32 playback = 0;
    setGlobal->GetInt32(resolver,
            ISettingsGlobal::ZEN_DISABLE_DUCKING_DURING_MEDIA_PLAYBACK, 0, &playback);
    mHost->mDisableDuckingWhileMedia = playback == 1;
    mHost->UpdateDisableDucking();
}

//===============================================================================
//                  NotificationManagerService::IteratorInner
//===============================================================================

CAR_INTERFACE_IMPL(NotificationManagerService::IteratorInner, Object, IIterator);

NotificationManagerService::IteratorInner::IteratorInner(
    /* [in] */ IIterator* iter,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
    : mIter(iter)
    , mPkg(pkg)
    , mUserId(userId)
{
    mNext = FindNext();
}

NotificationManagerService::IteratorInner::~IteratorInner()
{}

AutoPtr<IStatusBarNotification> NotificationManagerService::IteratorInner::FindNext()
{
    Boolean res = FALSE;
    while (mIter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        mIter->GetNext((IInterface**)&obj);
        AutoPtr<IStatusBarNotification> nr = IStatusBarNotification::Probe(obj);
        String name;
        Int32 id;
        if ((mPkg.IsNull() || (nr->GetPackageName(&name), name) == mPkg)
                && (mUserId == IUserHandle::USER_ALL || (nr->GetUserId(&id), id) == mUserId)) {
            return nr;
        }
    }
    return NULL;
}

ECode NotificationManagerService::IteratorInner::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNext == NULL;
    return NOERROR;
}

ECode NotificationManagerService::IteratorInner::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;

    AutoPtr<IStatusBarNotification> next = mNext;
    if (next == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
        // throw new NoSuchElementException();
    }
    mNext = FindNext();
    *object = next;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode NotificationManagerService::IteratorInner::Remove()
{
    return mIter->Remove();
}

//===============================================================================
//                  NotificationManagerService::Archive
//===============================================================================

NotificationManagerService::Archive::Archive(
    /* [in] */ Int32 size)
    : mBufferSize(size)
{
    CArrayDeque::New(mBufferSize, (IArrayDeque**)&mBuffer);
}

NotificationManagerService::Archive::~Archive()
{}

ECode NotificationManagerService::Archive::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    Int32 N;
    mBuffer->GetSize(&N);
    sb.Append("Archive (");
    sb.Append(N);
    sb.Append(" notification");
    sb.Append((N==1)?")":"s)");
    *str = sb.ToString();
    return NOERROR;
}

ECode NotificationManagerService::Archive::Record(
    /* [in] */ IStatusBarNotification* nr)
{
    Int32 size;
    mBuffer->GetSize(&size);
    if (size == mBufferSize) {
        AutoPtr<IInterface> obj;
        IDeque::Probe(mBuffer)->RemoveFirst((IInterface**)&obj);
    }

    // We don't want to store the heavy bits of the notification in the archive,
    // but other clients in the system process might be using the object, so we
    // store a (lightened) copy.
    AutoPtr<IStatusBarNotification> clone;
    nr->CloneLight((IStatusBarNotification**)&clone);
    IDeque::Probe(mBuffer)->AddLast(clone);
    return NOERROR;
}

ECode NotificationManagerService::Archive::Clear()
{
    mBuffer->Clear();
    return NOERROR;
}

ECode NotificationManagerService::Archive::DescendingIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    return IDeque::Probe(mBuffer)->GetDescendingIterator(iterator);
}

ECode NotificationManagerService::Archive::AscendingIterator(
    /* [out] */ IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    return mBuffer->GetIterator(iterator);
}

ECode NotificationManagerService::Archive::Filter(
    /* [in] */ IIterator* iter,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId,
    /* [out] */IIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    AutoPtr<IteratorInner> objIter = new IteratorInner(iter, pkg, userId);
    AutoPtr<IIterator> obj = (IIterator*)objIter.Get();
    *iterator = obj;
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode NotificationManagerService::Archive::GetArray(
    /* [in] */ Int32 count,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ation)
{
    VALIDATE_NOT_NULL(ation);
    if (count == 0) {
        count = mBufferSize;
    }

    Int32 size = 0;
    mBuffer->GetSize(&size);
    using Elastos::Core::Math;
    AutoPtr< ArrayOf<IStatusBarNotification*> > a = ArrayOf<IStatusBarNotification*>::Alloc(Math::Min(count, size));
    AutoPtr<IIterator> iter;
    DescendingIterator((IIterator**)&iter);

    Int32 i = 0;
    Boolean res = FALSE;
    while ((iter->HasNext(&res) ,res) && i < count) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IStatusBarNotification> notification = IStatusBarNotification::Probe(obj);
        (*a)[i++] = notification;
    }
    *ation = a;
    REFCOUNT_ADD(*ation);
    return NOERROR;
}

ECode NotificationManagerService::Archive::GetArray(
    /* [in] */ Int32 count,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IStatusBarNotification*>** ation)
{
    VALIDATE_NOT_NULL(ation);
    if (count == 0) {
        count = mBufferSize;
    }

    Int32 size = 0;
    mBuffer->GetSize(&size);
    using Elastos::Core::Math;
    AutoPtr< ArrayOf<IStatusBarNotification*> > a = ArrayOf<IStatusBarNotification*>::Alloc(Math::Min(count, size));
    AutoPtr<IIterator> iterOther;
    DescendingIterator((IIterator**)&iterOther);
    AutoPtr<IIterator> iter;
    Filter(iterOther, pkg, userId, (IIterator**)&iter);

    Int32 i = 0;
    Boolean res = FALSE;
    while ((iter->HasNext(&res) ,res) && i < count) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IStatusBarNotification> notification = IStatusBarNotification::Probe(obj);
        (*a)[i++] = notification;
    }
    *ation = a;
    REFCOUNT_ADD(*ation);
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::ToastRecord
//===============================================================================

NotificationManagerService::ToastRecord::ToastRecord(
    /* [in] */ Int32 pid,
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback,
    /* [in] */ Int32 duration)
    : mPid(pid)
    , mPkg(pkg)
    , mCallback(callback)
    , mDuration(duration)
{}

NotificationManagerService::ToastRecord::~ToastRecord()
{}

void NotificationManagerService::ToastRecord::Update(
    /* [in] */ Int32 duration)
{
    mDuration = duration;
}

void NotificationManagerService::ToastRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ DumpFilter* filter)
{
    if (filter != NULL && !filter->Matches(mPkg)) return;
    StringBuilder builder;
    builder += prefix;
    builder += StringUtils::ToHexString((Int32)this);
    pw->Println(builder.ToString());
}

ECode NotificationManagerService::ToastRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder builder;
    builder += "ToastRecord{";
    builder += StringUtils::ToHexString((Int32)this);
    builder += " pkg=";
    builder += mPkg;
    builder += " callback=";
    builder += mCallback;
    builder += " duration=";
    builder += mDuration;
    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::MyNotificationDelegate
//===============================================================================

CAR_INTERFACE_IMPL(NotificationManagerService::MyNotificationDelegate, Object, INotificationDelegate);

NotificationManagerService::MyNotificationDelegate::MyNotificationDelegate(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

NotificationManagerService::MyNotificationDelegate::~MyNotificationDelegate()
{}

ECode NotificationManagerService::MyNotificationDelegate::OnSetDisabled(
    /* [in] */ Int32 status)
{
    synchronized(mHost->mNotificationList) {
        mHost->mDisableNotificationEffects =
                (status & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0;
        if (!mHost->DisableNotificationEffects(NULL).IsNull()) {
            // cancel whatever's going on
            Int64 identity = Binder::ClearCallingIdentity();
            // try {
            AutoPtr<IIRingtonePlayer> player;
            mHost->mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&player);

            if (player != NULL) {
                player->StopAsync();
            }
            // } catch (RemoteException e) {
            // } finally {
            Binder::RestoreCallingIdentity(identity);
            // }

            identity = Binder::ClearCallingIdentity();
            // try {
            mHost->mVibrator->Cancel();
            // } finally {
            Binder::RestoreCallingIdentity(identity);
            // }
        }
    }
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnClearAll(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 userId)
{
    synchronized(mHost->mNotificationList) {
        mHost->CancelAllLocked(callingUid, callingPid, userId, REASON_DELEGATE_CANCEL_ALL, NULL,
                /*includeCurrentProfiles*/ TRUE);
    }
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnNotificationClick(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& key)
{
    synchronized(mHost->mNotificationList) {
        // TODO
        // EventLogTags.writeNotificationClicked(key);
        AutoPtr<IInterface> obj;
        mHost->mNotificationsByKey->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (r == NULL) {
            Logger::W(TAG, "No notification with key: %s", key.string());
            return NOERROR;
        }
        AutoPtr<IStatusBarNotification> sbn = r->mSbn;
        String pkg, tag;
        Int32 id;
        mHost->CancelNotification(callingUid, callingPid,
                (sbn->GetPackageName(&pkg), pkg),
                (sbn->GetTag(&tag), tag),
                (sbn->GetId(&id), id),
                INotification::FLAG_AUTO_CANCEL,
                INotification::FLAG_FOREGROUND_SERVICE, FALSE,
                r->GetUserId(),
                REASON_DELEGATE_CLICK, NULL);
    }
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnNotificationClear(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    mHost->CancelNotification(callingUid, callingPid, pkg, tag, id, 0,
            INotification::FLAG_ONGOING_EVENT | INotification::FLAG_FOREGROUND_SERVICE,
            TRUE, userId, REASON_DELEGATE_CANCEL, NULL);
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnPanelRevealed()
{
    // TODO
    // EventLogTags.writeNotificationPanelRevealed();
    synchronized(mHost->mNotificationList) {
        // sound
        mHost->mSoundNotification = NULL;

        Int64 identity = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIRingtonePlayer> player;
        mHost->mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&player);

        if (player != NULL) {
            player->StopAsync();
        }
        // } catch (RemoteException e) {
        // } finally {
        Binder::RestoreCallingIdentity(identity);
        // }

        // vibrate
        mHost->mVibrateNotification = NULL;
        identity = Binder::ClearCallingIdentity();
        // try {
        mHost->mVibrator->Cancel();
        // } finally {
        Binder::RestoreCallingIdentity(identity);
        // }

        // lights
        // clear only if lockscreen is not active
        if (mHost->mKeyguardManager != NULL) {
            Boolean bLock = FALSE;
            mHost->mKeyguardManager->IsKeyguardLocked(&bLock);
            if (!bLock) {
                mHost->mLights->Clear();
                mHost->mLedNotification = NULL;
                mHost->UpdateLightsLocked();
            }
        }
    }
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnPanelHidden()
{
    // TODO
    // EventLogTags.writeNotificationPanelHidden();
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnNotificationError(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& message,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    builder += "onNotification error pkg=";
    builder += pkg;
    builder += " tag=";
    builder += tag;
    builder += " id=";
    builder += id;
    builder += "; will crashApplication(uid=";
    builder += uid;
    builder += ", pid=";
    builder += initialPid;
    builder += ")";
    Slogger::D(TAG, builder.ToString().string());

    mHost->CancelNotification(callingUid, callingPid, pkg, tag, id, 0, 0, FALSE,
            userId, REASON_DELEGATE_ERROR, NULL);

    Int64 ident = Binder::ClearCallingIdentity();

    // try {
    AutoPtr<IIActivityManager> activityMgr = ActivityManagerNative::GetDefault();

    StringBuilder msg;
    msg += "Bad notification posted from package ";
    msg += pkg;
    msg += ": ";
    msg += message;

    activityMgr->CrashApplication(uid, initialPid, pkg, msg.ToString());
    // } catch (RemoteException e) {
    // }

    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnNotificationVisibilityChanged(
    /* [in] */ ArrayOf<String>* newlyVisibleKeys,
    /* [in] */ ArrayOf<String>* noLongerVisibleKeys)
{
    // Using ';' as separator since eventlogs uses ',' to separate
    // args.

    // TODO
    // EventLogTags.writeNotificationVisibilityChanged(
    //         TextUtils.join(";", newlyVisibleKeys),
    //         TextUtils.join(";", noLongerVisibleKeys));
    synchronized(mHost->mNotificationList) {
        for (Int32 i = 0; i < newlyVisibleKeys->GetLength(); i++) {
            String key = (*newlyVisibleKeys)[i];
            AutoPtr<IInterface> obj;
            mHost->mNotificationsByKey->Get(CoreUtils::Convert(key), (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
            if (r == NULL) continue;
            r->mStats->OnVisibilityChanged(TRUE);
        }
        // Note that we might receive this event after notifications
        // have already left the system, e.g. after dismissing from the
        // shade. Hence not finding notifications in
        // mNotificationsByKey is not an exceptional condition.
        for (Int32 i = 0; i < noLongerVisibleKeys->GetLength(); i++) {
            String key = (*noLongerVisibleKeys)[i];
            AutoPtr<IInterface> obj;
            mHost->mNotificationsByKey->Get(CoreUtils::Convert(key), (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
            if (r == NULL) continue;
            r->mStats->OnVisibilityChanged(FALSE);
        }
    }
    return NOERROR;
}

ECode NotificationManagerService::MyNotificationDelegate::OnNotificationExpansionChanged(
    /* [in] */ const String& key,
    /* [in] */ Boolean userAction,
    /* [in] */ Boolean expanded)
{
    // TODO
    // EventLogTags.writeNotificationExpansion(key, userAction ? 1 : 0, expanded ? 1 : 0);
    synchronized(mHost->mNotificationList) {
        AutoPtr<IInterface> obj;
        mHost->mNotificationsByKey->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (r != NULL) {
            r->mStats->OnExpansionChanged(userAction, expanded);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::MyBroadcastReceiver
//===============================================================================
NotificationManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{
}

NotificationManagerService::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode NotificationManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::D("NotificationManagerService::MyBroadcastReceiver", "OnReceive %s", action.string());

    Boolean queryRestart = FALSE;
    Boolean queryRemove = FALSE;
    Boolean packageChanged = FALSE;
    Boolean cancelNotifications = TRUE;

    if (action.Equals(IIntent::ACTION_PACKAGE_ADDED)
            || (queryRemove == action.Equals(IIntent::ACTION_PACKAGE_REMOVED))
            || action.Equals(IIntent::ACTION_PACKAGE_RESTARTED)
            || (packageChanged = action.Equals(IIntent::ACTION_PACKAGE_CHANGED))
            || (queryRestart = action.Equals(IIntent::ACTION_QUERY_PACKAGE_RESTART))
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE))
    {
        AutoPtr< ArrayOf<String> > pkgList;
        Boolean res;
        Boolean queryReplace = queryRemove &&
                (intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &res), res);
        if (DBG) {
            Slogger::I(TAG, "action=%s queryReplace=%d", action.string(), queryReplace);
        }

        if (action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
            intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        }
        else if (queryRestart) {
            intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgList);
        }
        else {
            AutoPtr<IUri> uri;
            intent->GetData((IUri**)&uri);
            if (uri == NULL) {
                return NOERROR;
            }

            String pkgName;
            uri->GetSchemeSpecificPart(&pkgName);
            if (pkgName.IsNull()) {
                return NOERROR;
            }

            if (packageChanged) {
                // We cancel notifications for packages which have just been disabled
                // try {
                AutoPtr<IContext> context;
                mHost->GetContext((IContext**)&context);
                AutoPtr<IPackageManager> pkgMgr;
                context->GetPackageManager((IPackageManager**)&pkgMgr);
                Int32 enabled;
                pkgMgr->GetApplicationEnabledSetting(pkgName, &enabled);

                if (enabled == IPackageManager::COMPONENT_ENABLED_STATE_ENABLED
                    || enabled == IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT) {
                    cancelNotifications = FALSE;
                }
                // } catch (IllegalArgumentException e) {
                //     // Package doesn't exist; probably racing with uninstall.
                //     // cancelNotifications is already true, so nothing to do here.
                //     if (DBG) {
                //         Slog.i(TAG, "Exception trying to look up app enabled setting", e);
                //     }
                // }
            }

            pkgList = ArrayOf<String>::Alloc(1);
            (*pkgList)[0] = pkgName;
        }

        if (pkgList != NULL && (pkgList->GetLength() > 0)) {
            for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                String pkgName = (*pkgList)[i];
                if (cancelNotifications) {
                    mHost->CancelAllNotificationsInt(MY_UID, MY_PID, pkgName, 0, 0, !queryRestart,
                            IUserHandle::USER_ALL, REASON_PACKAGE_CHANGED, NULL);
                }
            }
        }
        mHost->mListeners->OnPackagesChanged(queryReplace, pkgList);
        mHost->mConditionProviders->OnPackagesChanged(queryReplace, pkgList);
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        // Keep track of screen on/off state, but do not turn off the notification light
        // until user passes through the lock screen or views the notification.
        mHost->mScreenOn = TRUE;
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mHost->mScreenOn = FALSE;
        mHost->UpdateNotificationPulse();
    }
    else if (action.Equals(ITelephonyManager::ACTION_PHONE_STATE_CHANGED)) {
        String value;
        intent->GetStringExtra(ITelephonyManager::EXTRA_STATE, &value);
        mHost->mInCall = ITelephonyManager::EXTRA_STATE_OFFHOOK.Equals(value);
        mHost->UpdateNotificationPulse();
    }
    else if (action.Equals(IIntent::ACTION_USER_STOPPED)) {
        Int32 userHandle = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle);
        if (userHandle >= 0) {
            mHost->CancelAllNotificationsInt(MY_UID, MY_PID, String(NULL), 0, 0, TRUE,
                    userHandle, REASON_USER_STOPPED, NULL);
        }
    }
    else if (action.Equals(IIntent::ACTION_USER_PRESENT)) {
        // turn off LED when user passes through lock screen
        mHost->mNotificationLight->TurnOff();
        Slogger::I(TAG, "TODO: StatusBar");
        // mHost->mStatusBar->NotificationLightOff();
    }
    else if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        // reload per-user settings
        mHost->mSettingsObserver->Update(NULL);
        mHost->mUserProfiles->UpdateCache(context);
        // Refresh managed services
        mHost->mConditionProviders->OnUserSwitched();
        mHost->mListeners->OnUserSwitched();
    }
    else if (action.Equals(IIntent::ACTION_USER_ADDED)) {
        mHost->mUserProfiles->UpdateCache(context);
    }

    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::MyRunnable
//===============================================================================

NotificationManagerService::MyRunnable::MyRunnable(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

NotificationManagerService::MyRunnable::~MyRunnable()
{}

ECode NotificationManagerService::MyRunnable::Run()
{
    Slogger::I(TAG, "TODO: StatusBar");
    // return mHost->mStatusBar->BuzzBeepBlinked();
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::MyNotificationManagerInternal
//===============================================================================

CAR_INTERFACE_IMPL(NotificationManagerService::MyNotificationManagerInternal, Object, INotificationManagerInternal);

NotificationManagerService::MyNotificationManagerInternal::MyNotificationManagerInternal(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

NotificationManagerService::MyNotificationManagerInternal::~MyNotificationManagerInternal()
{}

ECode NotificationManagerService::MyNotificationManagerInternal::EnqueueNotification(
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ ArrayOf<Int32>* idReceived,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** out)
{
    return mHost->EnqueueNotificationInternal(pkg, opPkg, callingUid, callingPid,
            tag, id, notification, idReceived, userId, out);
}

ECode NotificationManagerService::MyNotificationManagerInternal::RemoveForegroundServiceFlagFromNotification(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 userId)
{
    mHost->CheckCallerIsSystem();
    synchronized(mHost->mNotificationList) {
        Int32 i = mHost->IndexOfNotificationLocked(pkg, String(NULL), notificationId, userId);
        if (i < 0) {
            Logger::D(TAG, "stripForegroundServiceFlag: Could not find notification with pkg=%s / id=%d / userId=%d",
                    pkg.string(), notificationId, userId);
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        mHost->mNotificationList->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
        AutoPtr<IStatusBarNotification> sbn = r->mSbn;
        // NoMan adds flags FLAG_NO_CLEAR and FLAG_ONGOING_EVENT when it sees
        // FLAG_FOREGROUND_SERVICE. Hence it's not enough to remove FLAG_FOREGROUND_SERVICE,
        // we have to revert to the flags we received initially *and* force remove
        // FLAG_FOREGROUND_SERVICE.
        AutoPtr<INotification> notification;
        sbn->GetNotification((INotification**)&notification);
        notification->SetFlags(r->mOriginalFlags & ~INotification::FLAG_FOREGROUND_SERVICE);
        mHost->mRankingHelper->Sort(mHost->mNotificationList);
        mHost->mListeners->NotifyPostedLocked(sbn, sbn /* oldSbn */);
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::WorkerHandler
//===============================================================================

NotificationManagerService::WorkerHandler::WorkerHandler(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

NotificationManagerService::WorkerHandler::~WorkerHandler()
{}

ECode NotificationManagerService::WorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case NotificationManagerService::MESSAGE_TIMEOUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            NotificationManagerService::ToastRecord* record = (NotificationManagerService::ToastRecord*)IObject::Probe(obj);
            mHost->HandleTimeout(record);
            break;
        }
        case MESSAGE_SAVE_POLICY_FILE:
            mHost->HandleSavePolicyFile();
            break;
        case MESSAGE_SEND_RANKING_UPDATE:
            mHost->HandleSendRankingUpdate();
            break;
        case MESSAGE_LISTENER_HINTS_CHANGED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleListenerHintsChanged(arg1);
            break;
        }
        case MESSAGE_LISTENER_NOTIFICATION_FILTER_CHANGED:{
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleListenerInterruptionFilterChanged(arg1);
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::WorkerHandler
//===============================================================================

NotificationManagerService::RankingWorkerHandler::RankingWorkerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ NotificationManagerService* host)
    : Handler(looper)
    , mHost(host)
{}

NotificationManagerService::RankingWorkerHandler::~RankingWorkerHandler()
{}

ECode NotificationManagerService::RankingWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_RECONSIDER_RANKING:
            mHost->HandleRankingReconsideration(msg);
            break;
        case MESSAGE_RANKING_CONFIG_CHANGE:
            mHost->HandleRankingConfigChange();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::NotificationListenersRunnable
//===============================================================================

NotificationManagerService::NotificationListenersRunnable::NotificationListenersRunnable(
    /* [in] */ Int32 id,
    /* [in] */ NotificationListeners* host,
    /* [in] */ NotificationManagerService* mainHost,
    /* [in] */ ManagedServices::ManagedServiceInfo* info,
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ INotificationRankingUpdate* update,
    /* [in] */ Int32 data)
    : mId(id)
    , mHost(host)
    , mMainHost(mainHost)
    , mInfo(info)
    , mSbn(sbn)
    , mUpdate(update)
    , mData(data)
{}

NotificationManagerService::NotificationListenersRunnable::~NotificationListenersRunnable()
{}

ECode NotificationManagerService::NotificationListenersRunnable::Run()
{
    switch (mId) {
        case 0:
            mHost->NotifyRemoved(mInfo, mSbn, mUpdate);
            break;
        case 1:
            mHost->NotifyPosted(mInfo, mSbn, mUpdate);
            break;
        case 2:
            mHost->NotifyRemoved(mInfo, mSbn, mUpdate);
            break;
        case 3:
            mHost->NotifyRankingUpdate(mInfo, mUpdate);
            break;
        case 4:
            mHost->NotifyListenerHintsChanged(mInfo, mData);
            break;
        case 5:
            mHost->NotifyInterruptionFilterChanged(mInfo, mData);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::EnqueueNotificationInternalRunnable
//===============================================================================

NotificationManagerService::EnqueueNotificationInternalRunnable::EnqueueNotificationInternalRunnable(
    /* [in] */ NotificationManagerService* host,
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean isSystemNotification)
    : mHost(host)
    , mPkg(pkg)
    , mOpPkg(opPkg)
    , mCallingUid(callingUid)
    , mCallingPid(callingPid)
    , mTag(tag)
    , mId(id)
    , mNotification(notification)
    , mUser(user)
    , mIsSystemNotification(isSystemNotification)
{}

NotificationManagerService::EnqueueNotificationInternalRunnable::~EnqueueNotificationInternalRunnable()
{}

ECode NotificationManagerService::EnqueueNotificationInternalRunnable::Run()
{
    synchronized(mHost->mNotificationList) {

        // === Scoring ===

        // 0. Sanitize inputs

        Int32 priority = 0;
        mNotification->GetPriority(&priority);
        mNotification->SetPriority(Clamp(priority, INotification::PRIORITY_MIN, INotification::PRIORITY_MAX));
        // Migrate notification flags to scores
        Int32 flags;
        mNotification->GetFlags(&flags);

        if (0 != (flags & INotification::FLAG_HIGH_PRIORITY)) {
            Int32 tmpPriority;
            mNotification->GetPriority(&tmpPriority);

            if (tmpPriority < INotification::PRIORITY_MAX) {
                mNotification->SetPriority(INotification::PRIORITY_MAX);
            }
        }
        else if (SCORE_ONGOING_HIGHER &&
                0 != (flags & INotification::FLAG_ONGOING_EVENT)) {
            Int32 tmpPriority;
            mNotification->GetPriority(&tmpPriority);

            if (tmpPriority < INotification::PRIORITY_HIGH) {
                mNotification->SetPriority(INotification::PRIORITY_HIGH);
            }
        }

        // 1. initial score: buckets of 10, around the app [-20..20]
        mNotification->GetPriority(&priority);
        Int32 score = priority * NOTIFICATION_PRIORITY_MULTIPLIER;

        // 2. extract ranking signals from the notification data
        AutoPtr<IStatusBarNotification> n;
        CStatusBarNotification::New(mPkg, mOpPkg, mId, mTag,
                mCallingUid, mCallingPid, score, mNotification, mUser,
                (IStatusBarNotification**)&n);

        AutoPtr<NotificationRecord> r = new NotificationRecord(n, score);
        String key;
        n->GetKey(&key);
        AutoPtr<IInterface> obj;
        mHost->mNotificationsByKey->Get(CoreUtils::Convert(key), (IInterface**)&obj);
        AutoPtr<NotificationRecord> old = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (old != NULL) {
            // Retain ranking information from previous record
            r->CopyRankingInformation(old);
        }
        mHost->mRankingHelper->ExtractSignals(r);

        // 3. Apply local rules

        // blocked apps
        if (ENABLE_BLOCKED_NOTIFICATIONS && !mHost->NoteNotificationOp(mPkg, mCallingUid)) {
            if (!mIsSystemNotification) {
                r->mScore = JUNK_SCORE;
                Logger::E(TAG, "Suppressing notification from package %s by user request.", mPkg.string());
            }
        }

        if (r->mScore < SCORE_DISPLAY_THRESHOLD) {
            // Notification will be blocked because the score is too low.
            return NOERROR;
        }

        if (mHost->IsNotificationSpam(mNotification, mPkg)) {
            mHost->mArchive->Record(r->mSbn);
            return NOERROR;
        }

        // Clear out group children of the old notification if the update causes the
        // group summary to go away. This happens when the old notification was a
        // summary and the new one isn't, or when the old notification was a summary
        // and its group key changed.
        Boolean res = FALSE;
        if (old != NULL && (old->GetNotification()->IsGroupSummary(&res), res) &&
                ((mNotification->IsGroupSummary(&res), !res) ||
                        !old->GetGroupKey().Equals(r->GetGroupKey()))) {
            mHost->CancelGroupChildrenLocked(old, mCallingUid, mCallingPid, String(NULL));
        }

        key = String(NULL);
        n->GetKey(&key);
        Int32 index = mHost->IndexOfNotificationLocked(key);
        if (index < 0) {
            mHost->mNotificationList->Add((INotificationRecord*)r);
            mHost->mUsageStats->RegisterPostedByApp(r);
        }
        else {
            AutoPtr<IInterface> obj;
            mHost->mNotificationList->Get(index, (IInterface**)&obj);
            old = (NotificationRecord*)INotificationRecord::Probe(obj);
            mHost->mNotificationList->Set(index, (INotificationRecord*)r);
            mHost->mUsageStats->RegisterUpdatedByApp(r, old);
            // Make sure we don't lose the foreground service state.

            mNotification->GetFlags(&flags);
            Int32 oldFlags;
            old->GetNotification()->GetFlags(&oldFlags);
            mNotification->SetFlags(flags |= oldFlags & INotification::FLAG_FOREGROUND_SERVICE);

            r->mIsUpdate = TRUE;
        }

        key = String(NULL);
        n->GetKey(&key);
        mHost->mNotificationsByKey->Put(CoreUtils::Convert(key), (INotificationRecord*)r);

        // Ensure if this is a foreground service that the proper additional
        // flags are set.
        mNotification->GetFlags(&flags);
        if ((flags & INotification::FLAG_FOREGROUND_SERVICE) != 0) {
            mNotification->SetFlags(flags |= INotification::FLAG_ONGOING_EVENT
                    | INotification::FLAG_NO_CLEAR);
        }

        mHost->ApplyZenModeLocked(r);
        mHost->mRankingHelper->Sort(mHost->mNotificationList);

        Int32 icon;
        mNotification->GetIcon(&icon);
        if (icon != 0) {
            AutoPtr<IStatusBarNotification> oldSbn = (old != NULL) ? old->mSbn : NULL;
            mHost->mListeners->NotifyPostedLocked(n, oldSbn);
        }
        else {
            Slogger::E(TAG, "Not posting notification with icon==0: %p", mNotification.Get());
            if (old != NULL && !old->mIsCanceled) {
                mHost->mListeners->NotifyRemovedLocked(n);
            }
            // ATTENTION: in a future release we will bail out here
            // so that we do not play sounds, show lights, etc. for invalid
            // notifications
            String str;
            n->GetPackageName(&str);
            Slogger::E(TAG, "WARNING: In a future release this will crash the app: %s", str.string());
        }

        mHost->BuzzBeepBlinkLocked(r);
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::CancelNotificationRunnable
//===============================================================================

NotificationManagerService::CancelNotificationRunnable::CancelNotificationRunnable(
    /* [in] */ NotificationManagerService* host,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 mustHaveFlags,
    /* [in] */ Int32 mustNotHaveFlags,
    /* [in] */ Boolean sendDelete,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ ManagedServices::ManagedServiceInfo* listener)
    : mHost(host)
    , mCallingUid(callingUid)
    , mCallingPid(callingPid)
    , mPkg(pkg)
    , mTag(tag)
    , mId(id)
    , mMustHaveFlags(mustHaveFlags)
    , mMustNotHaveFlags(mustNotHaveFlags)
    , mSendDelete(sendDelete)
    , mUserId(userId)
    , mReason(reason)
    , mListener(listener)
{}

NotificationManagerService::CancelNotificationRunnable::~CancelNotificationRunnable()
{}

ECode NotificationManagerService::CancelNotificationRunnable::Run()
{
    String listenerName;
    if (mListener == NULL){
        listenerName = String(NULL);
    }
    else {
        mListener->mComponent->ToShortString(&listenerName);
    }

    // TODO
    // EventLogTags::WriteNotificationCancel(mCallingUid, mCallingPid, mPkg, mId, mTag, mUserId,
    //         mMustHaveFlags, mMustNotHaveFlags, mReason, listenerName);

    synchronized(mHost->mNotificationList) {
        Int32 index = mHost->IndexOfNotificationLocked(mPkg, mTag, mId, mUserId);
        if (index >= 0) {
            AutoPtr<IInterface> obj;
            mHost->mNotificationList->Get(index, (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);

            // Ideally we'd do this in the caller of this method. However, that would
            // require the caller to also find the notification.
            if (mReason == REASON_DELEGATE_CLICK) {
                mHost->mUsageStats->RegisterClickedByUser(r);
            }

            Int32 flags;
            r->GetNotification()->GetFlags(&flags);
            if ((flags & mMustHaveFlags) != mMustHaveFlags) {
                return NOERROR;
            }
            if ((flags & mMustNotHaveFlags) != 0) {
                return NOERROR;
            }

            mHost->mNotificationList->Remove(index);

            mHost->CancelNotificationLocked(r, mSendDelete, mReason);
            mHost->CancelGroupChildrenLocked(r, mCallingUid, mCallingPid, listenerName);
            mHost->UpdateLightsLocked();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService::ZenModeHelperCallback
//===============================================================================

NotificationManagerService::ZenModeHelperCallback::ZenModeHelperCallback(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

NotificationManagerService::ZenModeHelperCallback::~ZenModeHelperCallback()
{}

void NotificationManagerService::ZenModeHelperCallback::OnConfigChanged()
{
    mHost->SavePolicyFile();
}

void NotificationManagerService::ZenModeHelperCallback::OnZenModeChanged()
{
    synchronized(mHost->mNotificationList) {
        mHost->UpdateInterruptionFilterLocked();
    }
}

//===============================================================================
//                  NotificationManagerService::InnerSub_PhoneStateListener
//===============================================================================
const String NotificationManagerService::InnerSub_PhoneStateListener::TAG("NotificationManagerService::InnerSub_PhoneStateListener");

NotificationManagerService::InnerSub_PhoneStateListener::InnerSub_PhoneStateListener(
    /* [in] */ NotificationManagerService* host)
    : mHost(host)
{}

ECode NotificationManagerService::InnerSub_PhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (mHost->mCallState == state) return NOERROR;
    if (DBG) Slogger::D(TAG, (String("Call state changed: ") + CallStateToString(state)).string());
    mHost->mCallState = state;
    return NOERROR;
}

//===============================================================================
//                  NotificationManagerService
//===============================================================================

NotificationManagerService::NotificationManagerService()
    : mDefaultNotificationColor(0)
    , mDefaultNotificationLedOn(0)
    , mDefaultNotificationLedOff(0)
    , mUseAttentionLight(FALSE)
    , mSystemReady(FALSE)
    , mDisableNotificationEffects(FALSE)
    , mCallState(0)
    , mListenerHints(0)
    , mInterruptionFilter(0)
    , mScreenOn(TRUE)
    , mInCall(FALSE)
    , mNotificationPulseEnabled(FALSE)
{
    mSpamCache = new LruCache<AutoPtr<IInteger32>, AutoPtr<FilterCacheInfo> >(100);
}

ECode NotificationManagerService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);

    AutoPtr<IExecutors> exe;
    CExecutors::AcquireSingleton((IExecutors**)&exe);
    exe->NewSingleThreadExecutor((IExecutorService**)&mSpamExecutor);

    CBinder::New((IBinder**)&mForegroundToken);
    CArrayList::New((IArrayList**)&mNotificationList);
    CArrayMap::New((IArrayMap**)&mNotificationsByKey);
    CArrayList::New((IArrayList**)&mToastQueue);
    CArrayList::New((IArrayList**)&mLights);

    CHandlerThread::New(String("ranker"),
            IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&mRankingThread);

    CArraySet::New((IArraySet**)&mListenersDisablingEffects);
    CHashSet::New((IHashSet**)&mBlockedPackages);
    mUserProfiles = new ManagedServices::UserProfiles();

    mNotificationDelegate = new MyNotificationDelegate(this);
    mIntentReceiver = new MyBroadcastReceiver(this);
    mBuzzBeepBlinked = new MyRunnable(this);

    CNotificationManagerBinderService::New(this, (IBinder**)&mService);
    mInternalService = new MyNotificationManagerInternal(this);

    mSessionListener = new MediaSessionManagerOnActiveSessionsChangedListener(this);
    return NOERROR;
}

void NotificationManagerService::LoadPolicyFile()
{
    synchronized(mPolicyFile) {
        mBlockedPackages->Clear();

        // try {
        AutoPtr<IFileInputStream> infile;
        ECode ec;
        do {
            ec = mPolicyFile->OpenRead((IFileInputStream**)&infile);
            if (FAILED(ec)) break;

            AutoPtr<IXmlPullParser> parser;
            ec = Xml::NewPullParser((IXmlPullParser**)&parser);
            if (FAILED(ec)) break;
            ec = parser->SetInput(IInputStream::Probe(infile), String(NULL));
            if (FAILED(ec)) break;

            Int32 type;
            String tag;
            Int32 version = DB_VERSION;

            while (ec = parser->Next(&type), type == IXmlPullParser::END_DOCUMENT) {
                if (FAILED(ec)) break;
                if (type != IXmlPullParser::START_TAG) {
                    continue;
                }

                ec = parser->GetName(&tag);
                if (FAILED(ec)) break;

                if (TAG_BODY.Equals(tag)) {
                    String value;
                    ec = parser->GetAttributeValue(String(NULL), ATTR_VERSION, &value);
                    if (FAILED(ec)) break;
                    version = StringUtils::ParseInt32(value);
                }
                else if (TAG_BLOCKED_PKGS.Equals(tag)) {
                    ec = parser->Next(&type);
                    if (FAILED(ec)) break;

                    while (type != IXmlPullParser::END_DOCUMENT) {
                        ec = parser->GetName(&tag);
                        if (FAILED(ec)) break;

                        if (TAG_PACKAGE.Equals(tag)) {
                            String value;
                            ec = parser->GetAttributeValue(String(NULL), ATTR_NAME, &value);
                            if (FAILED(ec)) break;
                            mBlockedPackages->Add(CoreUtils::Convert(value));
                        }
                        else if (TAG_BLOCKED_PKGS.Equals(tag) && type == IXmlPullParser::END_TAG) {
                            break;
                        }
                    }
                    if (FAILED(ec)) break;
                }
                ec = mZenModeHelper->ReadXml(parser);
                if (FAILED(ec)) break;
                ec = mRankingHelper->ReadXml(parser);
                if (FAILED(ec)) break;
            }
        } while(FALSE);
        if (FAILED(ec)) {
        // } catch (FileNotFoundException e) {
            if ((ECode) E_FILE_NOT_FOUND_EXCEPTION == ec) {
                // No data yet
            }
        // } catch (IOException e) {
            else if ((ECode) E_IO_EXCEPTION == ec) {
                Logger::W(TAG, "Unable to read notification policy %d", ec);
            }
        // } catch (NumberFormatException e) {
            else if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
                Logger::W(TAG, "Unable to parse notification policy %d", ec);
            }
        // } catch (XmlPullParserException e) {
            else if ((ECode) E_XML_PULL_PARSER_EXCEPTION == ec) {
                Logger::W(TAG, "Unable to parse notification policy %d", ec);
            }
        }
        // } finally {
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(infile));
        // }
    }
}

ECode NotificationManagerService::SavePolicyFile()
{
    mHandler->RemoveMessages(MESSAGE_SAVE_POLICY_FILE);
    Boolean res;
    mHandler->SendEmptyMessage(MESSAGE_SAVE_POLICY_FILE, &res);
    return NOERROR;
}

void NotificationManagerService::HandleSavePolicyFile()
{
    Slogger::D(TAG, "handleSavePolicyFile");
    synchronized(mPolicyFile) {
        AutoPtr<IFileOutputStream> stream;
        // try {
        ECode ec = mPolicyFile->StartWrite((IFileOutputStream**)&stream);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W(TAG, "Failed to save policy file");
            return;
        }
        // } catch (IOException e) {
        //     Slog.w(TAG, "Failed to save policy file", e);
        //     return;
        // }

        // try {
        AutoPtr<IXmlSerializer> out;
        assert (0 && "TODO");
        // ec = CFastXmlSerializer::New((IXmlSerializer**)&out);
        FAIL_GOTO(ec, _EXIT_)

        ec = out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
        FAIL_GOTO(ec, _EXIT_)

        ec = out->StartDocument(String(NULL), TRUE);
        FAIL_GOTO(ec, _EXIT_)

        ec = out->WriteStartTag(String(NULL), TAG_BODY);
        FAIL_GOTO(ec, _EXIT_)

        ec = out->WriteAttribute(String(NULL), ATTR_VERSION, StringUtils::ToString(DB_VERSION));
        FAIL_GOTO(ec, _EXIT_)

        mZenModeHelper->WriteXml(out);
        mRankingHelper->WriteXml(out);

        ec = out->WriteEndTag(String(NULL), TAG_BODY);
        FAIL_GOTO(ec, _EXIT_)

        ec = out->EndDocument();
        FAIL_GOTO(ec, _EXIT_)

        ec = mPolicyFile->FinishWrite(stream);
        FAIL_GOTO(ec, _EXIT_)
        // } catch (IOException e) {
        //     Slog.w(TAG, "Failed to save policy file, restoring backup", e);
        //     mPolicyFile.failWrite(stream);
        // }

        if (stream) {
            IOutputStream::Probe(stream)->Close();
        }
        return;

    _EXIT_:
        Slogger::W(TAG, "Failed to save policy file, restoring backup");
        mPolicyFile->FailWrite(stream);
        IOutputStream::Probe(stream)->Close();
    }
}

Boolean NotificationManagerService::NoteNotificationOp(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    Int32 result;
    mAppOps->NoteOpNoThrow(IAppOpsManager::OP_POST_NOTIFICATION, uid, pkg, &result);
    if (result != IAppOpsManager::MODE_ALLOWED) {
        Slogger::V(TAG, "notifications are disabled by AppOps for %s", pkg.string());
        return FALSE;
    }
    return TRUE;
}


AutoPtr< ArrayOf<Int64> > NotificationManagerService::GetLongArray(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resid,
    /* [in] */ Int32 maxlen,
    /* [in] */ ArrayOf<Int64>* def)
{
    AutoPtr< ArrayOf<Int32> > ar;
    res->GetInt32Array(resid, (ArrayOf<Int32>**)&ar);

    AutoPtr< ArrayOf<Int64> > result;
    if (ar == NULL) {
        if (def != NULL) result = def->Clone();
        return result;
    }

    Int32 len = ar->GetLength() > maxlen ? maxlen : ar->GetLength();
    result = ArrayOf<Int64>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        (*result)[i] = (*ar)[i];
    }

    return result;
}

ECode NotificationManagerService::OnStart()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    mAm = ActivityManagerNative::GetDefault();
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    mVibrator = IVibrator::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&obj);
    mKeyguardManager = IKeyguardManager::Probe(obj);

    mHandler = new WorkerHandler(this);
    mHandler->constructor();
    IThread::Probe(mRankingThread)->Start();
    AutoPtr< ArrayOf<String> > extractorNames;
    // try {
    ECode ec = resources->GetStringArray(
            R::array::config_notificationSignalExtractors,
            (ArrayOf<String>**)&extractorNames);
    if (ec == (ECode)E_RESOURCES_NOT_FOUND_EXCEPTION) {
        extractorNames = ArrayOf<String>::Alloc(0);
    }
    // } catch (Resources.NotFoundException e) {
    //     extractorNames = new String[0];
    // }

    AutoPtr<ILooper> looper;
    mRankingThread->GetLooper((ILooper**)&looper);
    AutoPtr<RankingWorkerHandler> workerHandler = new RankingWorkerHandler(looper, this);

    mRankingHelper = new RankingHelper(context, workerHandler, extractorNames);
    mZenModeHelper = new ZenModeHelper(context, mHandler);
    AutoPtr<ZenModeHelperCallback> callback = new ZenModeHelperCallback(this);
    mZenModeHelper->AddCallback(callback);

    AutoPtr<IFile> systemDir;
    CFile::New(Environment::GetDataDirectory(), String("system"), (IFile**)&systemDir);
    AutoPtr<IFile> other;
    CFile::New(systemDir, String("notification_policy.xml"), (IFile**)&other);
    CAtomicFile::New(other, (IAtomicFile**)&mPolicyFile);
    mUsageStats = new NotificationUsageStats(context);

    ImportOldBlockDb();

    AutoPtr<IManagedServices> managedServices;
    CNotificationListeners::New(TO_IINTERFACE(this), (IManagedServices**)&managedServices);
    mListeners = (NotificationListeners*) managedServices.Get();
    managedServices = NULL;
    CConditionProviders::New(context, mHandler, TO_IINTERFACE(mUserProfiles), TO_IINTERFACE(mZenModeHelper), (IManagedServices**)&managedServices);
    mConditionProviders = (ConditionProviders*) managedServices.Get();
    obj = GetLocalService(EIID_IStatusBarManagerInternal);
    mStatusBar = IStatusBarManagerInternal::Probe(obj);
    Slogger::I(TAG, "TODO: StatusBar");
    // mStatusBar->SetNotificationDelegate(mNotificationDelegate);

    obj = GetLocalService(EIID_ILightsManager);
    AutoPtr<LightsManager> lights = (LightsManager*)ILightsManager::Probe(obj);
    mNotificationLight = lights->GetLight(LightsManager::LIGHT_ID_NOTIFICATIONS);
    mAttentionLight = lights->GetLight(LightsManager::LIGHT_ID_ATTENTION);

    resources->GetColor(R::color::config_defaultNotificationColor, &mDefaultNotificationColor);
    resources->GetInteger(R::integer::config_defaultNotificationLedOn, &mDefaultNotificationLedOn);
    resources->GetInteger(R::integer::config_defaultNotificationLedOff, &mDefaultNotificationLedOff);

    CHashMap::New((IHashMap**)&mNotificationPulseCustomLedValues);

    CHashMap::New((IMap**)&mPackageNameMappings);
    AutoPtr<ArrayOf<String> > defaultMapping;
    resources->GetStringArray(
            R::array::notification_light_package_mapping, (ArrayOf<String>**)&defaultMapping);
    for (Int32 i = 0; i < defaultMapping->GetLength(); i++) {
        String mapping = (*defaultMapping)[i];
        AutoPtr<ArrayOf<String> > map;
        StringUtils::Split(mapping, "\\|", (ArrayOf<String>**)&map);
        AutoPtr<ICharSequence> p0;
        CString::New((*map)[0], (ICharSequence**)&p0);
        AutoPtr<ICharSequence> p1;
        CString::New((*map)[1], (ICharSequence**)&p1);
        mPackageNameMappings->Put(p0, p1);
    }

    mDefaultVibrationPattern = GetLongArray(resources,
        R::array::config_defaultNotificationVibePattern, VIBRATE_PATTERN_MAXLEN, DEFAULT_VIBRATE_PATTERN);
    mFallbackVibrationPattern = GetLongArray(resources,
        R::array::config_notificationFallbackVibePattern, VIBRATE_PATTERN_MAXLEN, DEFAULT_VIBRATE_PATTERN);

    resources->GetBoolean(R::bool_::config_useAttentionLight, &mUseAttentionLight);

    // Don't start allowing notifications until the setup wizard has run once.
    // After that, including subsequent boots, init with notifications turned on.
    // This works on the first boot because the setup wizard will toggle this
    // flag at least once and we'll go back to 0 after that.
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> globalSettings;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&globalSettings);

    Int32 value;
    globalSettings->GetInt32(resolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);

    if (value == 0) {
        mDisableNotificationEffects = TRUE;
    }

    mZenModeHelper->UpdateZenMode();

    mUserProfiles->UpdateCache(context);
    ListenForCallState();

    // register for various Intents
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(ITelephonyManager::ACTION_PHONE_STATE_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);
    filter->AddAction(IIntent::ACTION_USER_STOPPED);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    filter->AddAction(IIntent::ACTION_USER_ADDED);

    AutoPtr<IIntent> stickyIntent;
    context->RegisterReceiver(mIntentReceiver, filter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> pkgFilter;
    CIntentFilter::New((IIntentFilter**)&pkgFilter);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    pkgFilter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
    pkgFilter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART);
    pkgFilter->AddDataScheme(String("package"));

    stickyIntent = NULL;
    context->RegisterReceiver(mIntentReceiver,
            pkgFilter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE,
            (IIntentFilter**)&sdFilter);

    stickyIntent = NULL;
    context->RegisterReceiver(mIntentReceiver, sdFilter, (IIntent**)&stickyIntent);

    mSettingsObserver = new SettingsObserver(this, mHandler);
    mSettingsObserver->Observe();

    resources->GetInteger(R::integer::config_notificationServiceArchiveSize, &value);
    mArchive = new Archive(value);

    PublishBinderService(IContext::NOTIFICATION_SERVICE, mService);
    PublishLocalService(EIID_INotificationManagerInternal,
            (INotificationManagerInternal*)mInternalService.Get());

    return NOERROR;
}

void NotificationManagerService::ImportOldBlockDb()
{
    LoadPolicyFile();

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIterator> iterator;
    ISet::Probe(mBlockedPackages)->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        String pkg;
        ICharSequence::Probe(obj)->ToString(&pkg);
        AutoPtr<IPackageInfo> info;
        // try {
        pm->GetPackageInfo(pkg, 0, (IPackageInfo**)&info);
        AutoPtr<IApplicationInfo> applicationInfo;
        info->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 uid;
        applicationInfo->GetUid(&uid);
        SetNotificationsEnabledForPackageImpl(pkg, uid, FALSE);
        // } catch (NameNotFoundException e) {
        //     // forget you
        // }
    }
    mBlockedPackages->Clear();
}

ECode NotificationManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        // no beeping until we're basically done booting
        mSystemReady = TRUE;

        // Grab our optional AudioService
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        mAudioManager = IAudioManager::Probe(obj);
        mZenModeHelper->SetAudioManager(mAudioManager);

        UpdateDisableDucking();
    }
    else if (phase == ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START) {
        // This observer will force an update when observe is called, causing us to
        // bind to listener services.
        mSettingsObserver->Observe();
        mListeners->OnBootPhaseAppsCanStart();
        mConditionProviders->OnBootPhaseAppsCanStart();
    }
    return NOERROR;
}

void NotificationManagerService::SetNotificationsEnabledForPackageImpl(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean enabled)
{
    StringBuilder builder;
    builder += enabled ? "en":"dis";
    builder += "abling notifications for ";
    builder += pkg;

    Slogger::V(TAG, builder.ToString());

    mAppOps->SetMode(IAppOpsManager::OP_POST_NOTIFICATION, uid, pkg,
            enabled ? IAppOpsManager::MODE_ALLOWED : IAppOpsManager::MODE_IGNORED);

    // Now, cancel any outstanding notifications that are part of a just-disabled app
    if (ENABLE_BLOCKED_NOTIFICATIONS && !enabled) {
        CancelAllNotificationsInt(MY_UID, MY_PID, pkg, 0, 0, TRUE, UserHandle::GetUserId(uid),
                REASON_PACKAGE_BANNED, NULL);
    }
}

void NotificationManagerService::UpdateListenerHintsLocked()
{
    Boolean res;
    ISet::Probe(mListenersDisablingEffects)->IsEmpty(&res);
    const Int32 hints = res ? 0 : INotificationListenerService::HINT_HOST_DISABLE_EFFECTS;
    if (hints == mListenerHints) return;
    mListenerHints = hints;
    ScheduleListenerHintsChanged(hints);
}

void NotificationManagerService::UpdateEffectsSuppressorLocked()
{
    Boolean res;
    ISet::Probe(mListenersDisablingEffects)->IsEmpty(&res);
    AutoPtr<IComponentName> suppressor;
    if (!res) {
        AutoPtr<IInterface> obj;
        mListenersDisablingEffects->GetValueAt(0, (IInterface**)&obj);
        AutoPtr<ManagedServices::ManagedServiceInfo> info = (ManagedServices::ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
        suppressor = info->mComponent;
    }
    if (Objects::Equals(suppressor, mEffectsSuppressor)) return;
    mEffectsSuppressor = suppressor;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    CIntent::New(INotificationManager::ACTION_EFFECTS_SUPPRESSOR_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    context->SendBroadcast(intent);
}

void NotificationManagerService::UpdateInterruptionFilterLocked()
{
    Int32 interruptionFilter;
    interruptionFilter = mZenModeHelper->GetZenModeListenerInterruptionFilter();
    if (interruptionFilter == mInterruptionFilter) return;
    mInterruptionFilter = interruptionFilter;
    ScheduleInterruptionFilterChanged(interruptionFilter);
}

AutoPtr< ArrayOf<String> > NotificationManagerService::GetActiveNotificationKeys(
    /* [in] */ IINotificationListener* token)
{
    AutoPtr<ManagedServices::ManagedServiceInfo> info;
    mListeners->CheckServiceTokenLocked(token, (ManagedServices::ManagedServiceInfo**)&info);
    AutoPtr<IArrayList> keys;
    CArrayList::New((IArrayList**)&keys);
    if (info->IsEnabledForCurrentProfiles()) {
        synchronized(mNotificationList) {
            Int32 N;
            mNotificationList->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mNotificationList->Get(i, (IInterface**)&obj);
                AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
                AutoPtr<IStatusBarNotification> sbn = record->mSbn;
                Int32 id;
                sbn->GetUserId(&id);
                if (info->EnabledAndUserMatches(id)) {
                    String key;
                    sbn->GetKey(&key);
                    keys->Add(CoreUtils::Convert(key));
                }
            }
        }
    }
    Int32 size;
    keys->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > result;
    keys->ToArray(args, (ArrayOf<IInterface*>**)&result);

    AutoPtr< ArrayOf<String> > resultArray = ArrayOf<String>::Alloc(result->GetLength());
    for (Int32 i = 0; i < result->GetLength(); i++) {
        String str;
        ICharSequence::Probe((*result)[i])->ToString(&str);
        (*resultArray)[i] = str;
    }

    return resultArray;
}

String NotificationManagerService::DisableNotificationEffects(
    /* [in] */ NotificationRecord* record)
{
    if (mDisableNotificationEffects) {
        return String("booleanState");
    }
    if ((mListenerHints & INotificationListenerService::HINT_HOST_DISABLE_EFFECTS) != 0) {
        return String("listenerHints");
    }
    if (mCallState != ITelephonyManager::CALL_STATE_IDLE && !mZenModeHelper->IsCall(record)) {
        return String("callState");
    }
    return String(NULL);
}

void NotificationManagerService::DumpImpl(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DumpFilter* filter)
{
    pw->Print(String("Current Notification Manager state"));
    if (filter != NULL) {
        pw->Print(String(" (filtered to "));
        pw->Print((IObject*)filter);
        pw->Print(String(")"));
    }
    pw->PrintCharln(':');
    Int32 N;
    const Boolean zenOnly = filter != NULL && filter->mZen;

    if (!zenOnly) {
        synchronized(mToastQueue) {
            mToastQueue->GetSize(&N);
            if (N > 0) {
                pw->Println(String("  Toast Queue:"));
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<IInterface> obj;
                    mToastQueue->Get(i, (IInterface**)&obj);
                    AutoPtr<ToastRecord> record = (ToastRecord*)IObject::Probe(obj);
                    record->Dump(pw, String("    "), filter);
                }
                pw->Println(String("  "));
            }
        }
    }

    synchronized(mNotificationList) {
        if (!zenOnly) {
            mNotificationList->GetSize(&N);
            if (N > 0) {
                pw->Println(String("  Notification List:"));
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<IInterface> obj;
                    mNotificationList->Get(i, (IInterface**)&obj);
                    AutoPtr<NotificationRecord> nr = (NotificationRecord*)INotificationRecord::Probe(obj);
                    if (filter != NULL && !filter->Matches(nr->mSbn)) continue;
                    AutoPtr<IContext> context;
                    GetContext((IContext**)&context);
                    nr->Dump(pw, String("    "), context);
                }
                pw->Println(String("  "));
            }

            if (filter == NULL) {
                mLights->GetSize(&N);
                if (N > 0) {
                    pw->Println(String("  Lights List:"));
                    for (Int32 i = 0; i < N; i++) {
                        AutoPtr<IInterface> obj;
                        mLights->Get(i, (IInterface**)&obj);
                        String str;
                        ICharSequence::Probe(obj)->ToString(&str);
                        pw->Println(String("    ") + str);
                    }
                    pw->Println(String("  "));
                }
                pw->Println(String("  mUseAttentionLight=") + StringUtils::BooleanToString(mUseAttentionLight));
                pw->Println(String("  mNotificationPulseEnabled=") + StringUtils::BooleanToString(mNotificationPulseEnabled));
                StringBuilder builder;
                builder += "  mSoundNotification=";
                builder += (INotificationRecord*)mSoundNotification.Get();
                pw->Println(builder.ToString());

                builder.Reset();
                builder += "  mVibrateNotification=";
                builder += (INotificationRecord*)mVibrateNotification.Get();
                pw->Println(builder.ToString());
                pw->Println(String("  mDisableNotificationEffects=") + StringUtils::BooleanToString(mDisableNotificationEffects));
                pw->Println(String("  mCallState=") + CallStateToString(mCallState));
                pw->Println(String("  mSystemReady=") + StringUtils::BooleanToString(mSystemReady));
            }
            String str;
            mArchive->ToString(&str);
            pw->Println(String("  mArchive=") + str);

            AutoPtr<IIterator> iter;
            mArchive->DescendingIterator((IIterator**)&iter);
            Int32 i = 0;
            Boolean res;
            while (iter->HasNext(&res), res) {
                AutoPtr<IInterface> obj;
                iter->GetNext((IInterface**)&obj);
                AutoPtr<IStatusBarNotification> sbn = IStatusBarNotification::Probe(obj);
                if (filter != NULL && !filter->Matches(sbn)) continue;
                StringBuilder builder;
                builder += "    ";
                builder += sbn;
                pw->Println(builder.ToString());
                if (++i >= 5) {
                    if (iter->HasNext(&res), res) {
                        pw->Println(String("    ..."));
                    }
                    break;
                }
            }
        }

        if (!zenOnly) {
            pw->Println(String("\n  Usage Stats:"));
            mUsageStats->Dump(pw, String("    "), filter);
        }

        if (filter == NULL || zenOnly) {
            pw->Println(String("\n  Zen Mode:"));
            mZenModeHelper->Dump(pw, String("    "));

            pw->Println(String("\n  Zen Log:"));
            ZenLog::Dump(pw, String("    "));
        }

        if (!zenOnly) {
            pw->Println(String("\n  Ranking Config:"));
            mRankingHelper->Dump(pw, String("    "), filter);

            pw->Println(String("\n  Notification listeners:"));
            mListeners->Dump(pw, filter);
            pw->Print(String("    mListenerHints: "));
            pw->Println(mListenerHints);
            pw->Print(String("    mListenersDisablingEffects: ("));
            ISet::Probe(mListenersDisablingEffects)->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mListenersDisablingEffects->GetValueAt(i, (IInterface**)&obj);
                AutoPtr<ManagedServices::ManagedServiceInfo> listener = (ManagedServices::ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
                if (i > 0) pw->PrintCharln(',');
                pw->Print(listener->mComponent);
            }
            pw->PrintCharln(')');
        }

        pw->Println(String("\n  Condition providers:"));
        mConditionProviders->Dump(pw, filter);
    }
}

ECode NotificationManagerService::EnqueueNotificationInternal(
    /* [in] */ const String& pkg,
    /* [in] */ const String& opPkg,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ ArrayOf<Int32>* idIn,
    /* [in] */ Int32 incomingUserId,
    /* [out] */ ArrayOf<Int32>** idOut)
{
    VALIDATE_NOT_NULL(idOut)

    if (NULL == idIn) {
        *idOut = ArrayOf<Int32>::Alloc(1);
    }
    else {
        *idOut = idIn->Clone();
    }

    if (DBG) {
        AutoPtr<ICharSequence> text;
        notification->GetTickerText((ICharSequence**)&text);
        String str;
        if(text != NULL){
            text->ToString(&str);
        }

        Logger::V(TAG, "enqueueNotificationInternal: pkg=[%s] id=%d notification=[%s]",
                pkg.string(), id, str.string());
    }

    CheckCallerIsSystemOrSameApp(pkg);
    Boolean isSystemNotification = IsUidSystem(callingUid)|| pkg.Equals("android");
    Boolean isNotificationFromListener = mListeners->IsListenerPackage(pkg);

    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);
    Int32 userId;
    activityMgrHelper->HandleIncomingUser(callingPid, callingUid, incomingUserId, TRUE, FALSE,
            String("enqueueNotification"), pkg, &userId);

    AutoPtr<IUserHandle> user;
    CUserHandle::New(userId, (IUserHandle**)&user);

    // Limit the number of notifications that any given package except the android
    // package or a registered listener can enqueue.  Prevents DOS attacks and deals with leaks.
    if (!isSystemNotification && !isNotificationFromListener) {
        synchronized(mNotificationList) {
            Int32 count = 0;
            Int32 N;
            mNotificationList->GetSize(&N);
            for (Int32 i = 0; i < N; ++i) {
                AutoPtr<IInterface> obj;
                mNotificationList->Get(i, (IInterface**)&obj);
                AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
                String packageName;
                r->mSbn->GetPackageName(&packageName);
                Int32 _userid;
                r->mSbn->GetUserId(&_userid);
                if (packageName.Equals(pkg) && _userid == userId) {
                    count++;
                    if (count >= MAX_PACKAGE_NOTIFICATIONS) {
                        Logger::E(TAG, "Package has already posted %d notifications.  Not showing more.  package=%s",
                                count, pkg.string());
                        return NOERROR;
                    }
                }
            }
        }
    }

    // This conditional is a dirty hack to limit the logging done on
    //     behalf of the download manager without affecting other apps.
    if (!pkg.Equals("com.android.providers.downloads")
            || Logger::IsLoggable("DownloadManager", ILogHelper::VERBOSE)) {
        // TODO
        // EventLogTags.writeNotificationEnqueue(callingUid, callingPid,
        //         pkg, id, tag, userId, notification.toString());
    }

    if (pkg.IsNull() || notification == NULL) {
        Logger::E(TAG, "null not allowed: pkg=%s, id=%d, notification=%p", pkg.string(), id, notification);
        //throw new IllegalArgumentException("null not allowed: pkg=" + pkg + " id=" + id + " notification=" + notification);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 icon;
    notification->GetIcon(&icon);
    Boolean res;
    if (icon != 0) {
        if (notification->IsValid(&res), !res) {
            Logger::E(TAG, "Invalid notification: pkg=%s, id=%d, notification=[%p]", pkg.string(), id, notification);
            //throw new IllegalArgumentException("Invalid notification: pkg=" + pkg + " id=" + id + " notification=" + notification);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    AutoPtr<EnqueueNotificationInternalRunnable> runnable = new EnqueueNotificationInternalRunnable(
            this, pkg, opPkg, callingUid, callingPid, tag, id, notification, user, isSystemNotification);
    mHandler->Post(runnable, &res);

    (**idOut)[0] = id;
    return NOERROR;
}

ECode NotificationManagerService::BuzzBeepBlinkLocked(
    /* [in] */ NotificationRecord* record)
{
    Boolean buzzBeepBlinked = FALSE;
    AutoPtr<INotification> notification;
    record->mSbn->GetNotification((INotification**)&notification);

    // Should this notification make noise, vibe, or use the LED?
    const Boolean canInterrupt = (record->mScore >= SCORE_INTERRUPTION_THRESHOLD) &&
            !record->IsIntercepted();
    if (DBG || record->IsIntercepted()) {
        String packageName;
        record->mSbn->GetPackageName(&packageName);
        Slogger::V(TAG, "pkg=%s canInterrupt=%d intercept=%d", packageName.string(), canInterrupt, record->IsIntercepted());
    }

    Int32 currentUser;
    const Int64 token = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IActivityManagerHelper> activityMgrHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&activityMgrHelper);
    activityMgrHelper->GetCurrentUser(&currentUser);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }

    AutoPtr<IInterface> serv;
    mContext->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&serv);
    AutoPtr<IProfileManager> profileManager = IProfileManager::Probe(serv);

    String pakName;
    mContext->GetPackageName(&pakName);
    AutoPtr<IProfileGroup> group;
    profileManager->GetActiveProfileGroup(pakName, (IProfileGroup**)&group);
    if (group != NULL) {
        group->ApplyOverridesToNotification(notification);
    }

    // If we're not supposed to beep, vibrate, etc. then don't.
    String disableEffects = DisableNotificationEffects(record);
    if (!disableEffects.IsNull()) {
        ZenLog::TraceDisableEffects(record, disableEffects);
    }
    Int32 flags;
    notification->GetFlags(&flags);
    if (disableEffects.IsNull()
            && (!(record->mIsUpdate
                && (flags & INotification::FLAG_ONLY_ALERT_ONCE) != 0 ))
            && (record->GetUserId() == IUserHandle::USER_ALL ||
                record->GetUserId() == currentUser ||
                mUserProfiles->IsCurrentProfile(record->GetUserId()))
            && canInterrupt
            && mSystemReady
            && mAudioManager != NULL) {
        if (DBG) {
            Slogger::V(TAG, "Interrupting!");
        }

        String packageName;
        record->mSbn->GetPackageName(&packageName);
        SendAccessibilityEvent(notification, CoreUtils::Convert(packageName));

        // sound

        // should we use the default notification sound? (indicated either by
        // DEFAULT_SOUND or because notification.sound is pointing at
        // Settings.System.NOTIFICATION_SOUND)

        Int32 defaults = 0;
        notification->GetDefaults(&defaults);

        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        AutoPtr<IUri> notificationUri;
        settingsSystem->GetDEFAULT_NOTIFICATION_URI((IUri**)&notificationUri);
        AutoPtr<IUri> sound;
        notification->GetSound((IUri**)&sound);
        Boolean res;
        IObject::Probe(notificationUri)->Equals(sound, &res);

        Boolean useDefaultSound = (defaults & INotification::DEFAULT_SOUND) != 0 || res;

        AutoPtr<IUri> soundUri;
        Boolean hasValidSound = FALSE;

        if (useDefaultSound) {
            soundUri = notificationUri;

            // check to see if the default notification sound is silent
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            String str;
            settingsSystem->GetString(resolver, ISettingsSystem::NOTIFICATION_SOUND, &str);
            hasValidSound = !str.IsNull();
        }
        else if (sound != NULL){
            soundUri = sound;
            hasValidSound = (soundUri != NULL);
        }

        if (hasValidSound && (!mDisableDuckingWhileMedia || !mActiveMedia)) {
            notification->GetFlags(&flags);
            Boolean looping = (flags & INotification::FLAG_INSISTENT) != 0;
            AutoPtr<IAudioAttributes> attr;
            notification->GetAudioAttributes((IAudioAttributes**)&attr);
            AutoPtr<IAudioAttributes> audioAttributes;
            if (attr != NULL) {
                audioAttributes = attr;
            }
            else {
                AutoPtr<INotificationHelper> helper;
                CNotificationHelper::AcquireSingleton((INotificationHelper**)&helper);
                helper->GetAUDIO_ATTRIBUTES_DEFAULT((IAudioAttributes**)&audioAttributes);
            }
            mSoundNotification = record;
            // do not play notifications if stream volume is 0 (typically because
            // ringer mode is silent) or if there is a user of exclusive audio focus
            AutoPtr<IAudioAttributesHelper> helper;
            CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&helper);
            Int32 result = 0;
            helper->ToLegacyStreamType(audioAttributes, &result);
            Int32 data = 0;
            mAudioManager->GetStreamVolume(result, &data);
            Boolean res = FALSE;
            mAudioManager->IsAudioFocusExclusive(&res);
            if ((data != 0) && !res) {
                const Int64 identity = Binder::ClearCallingIdentity();
                // try {
                AutoPtr<IIRingtonePlayer> player;
                mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&player);
                if (player != NULL) {
                    if (DBG) {
                        Slogger::V(TAG, "Playing sound %p with attributes %p", soundUri.Get(), audioAttributes.Get());
                    }
                    AutoPtr<IUserHandle> user;
                    record->mSbn->GetUser((IUserHandle**)&user);
                    player->PlayAsync(soundUri, user, looping, audioAttributes);
                    buzzBeepBlinked = TRUE;
                }
                // } catch (RemoteException e) {
                // } finally {
                Binder::RestoreCallingIdentity(identity);
                // }
            }
        }

        // vibrate
        // Does the notification want to specify its own vibration?
        AutoPtr< ArrayOf<Int64> > vibrate;
        notification->GetVibrate((ArrayOf<Int64>**)&vibrate);
        Boolean hasCustomVibrate = vibrate != NULL;

        // new in 4.2: if there was supposed to be a sound and we're in vibrate
        // mode, and no other vibration is specified, we fall back to vibration
        Int32 mode = 0;
        mAudioManager->GetRingerMode(&mode);

        Boolean convertSoundToVibration =
                !hasCustomVibrate
                && hasValidSound
                && (mode == IAudioManager::RINGER_MODE_VIBRATE);

        // The DEFAULT_VIBRATE flag trumps any custom vibration AND the fallback.
        notification->GetDefaults(&defaults);
        Boolean useDefaultVibrate =
                (defaults & INotification::DEFAULT_VIBRATE) != 0;

        if ((useDefaultVibrate || convertSoundToVibration || hasCustomVibrate)
                && !(mode == IAudioManager::RINGER_MODE_SILENT)) {
            mVibrateNotification = record;

            if (useDefaultVibrate || convertSoundToVibration) {
                // Escalate privileges so we can use the vibrator even if the
                // notifying app does not have the VIBRATE permission.
                Int64 identity = Binder::ClearCallingIdentity();
                // try {
                Int32 uid = 0;
                record->mSbn->GetUid(&uid);
                String opPkg;
                record->mSbn->GetOpPkg(&opPkg);
                mVibrator->Vibrate(uid, opPkg,
                        useDefaultVibrate ? mDefaultVibrationPattern : mFallbackVibrationPattern,
                        ((flags & INotification::FLAG_INSISTENT) != 0) ? 0: -1,
                        AudioAttributesForNotification(notification));
                buzzBeepBlinked = TRUE;

                Binder::RestoreCallingIdentity(identity);
            }
            else if (vibrate->GetLength() > 1) {
                // If you want your own vibration pattern, you need the VIBRATE
                // permission
                Int32 uid = 0;
                record->mSbn->GetUid(&uid);
                String opPkg;
                record->mSbn->GetOpPkg(&opPkg);
                mVibrator->Vibrate(uid, opPkg, vibrate,
                        ((flags & INotification::FLAG_INSISTENT) != 0) ? 0: -1,
                        AudioAttributesForNotification(notification));
                buzzBeepBlinked = TRUE;
            }
        }
    }

    // light
    // release the light
    String key = record->GetKey();
    Boolean wasShowLights;
    mLights->Remove(CoreUtils::Convert(key), &wasShowLights);
    if (mLedNotification != NULL && key.Equals(mLedNotification->GetKey())) {
        mLedNotification = NULL;
    }
    Boolean canInterruptWithLight = canInterrupt || IsLedNotificationForcedOn(record);
    notification->GetFlags(&flags);
    if ((flags & INotification::FLAG_SHOW_LIGHTS) != 0 && canInterruptWithLight) {
        mLights->Add(CoreUtils::Convert(key));
        UpdateLightsLocked();
        if (mUseAttentionLight) {
            mAttentionLight->Pulse();
        }
        buzzBeepBlinked = TRUE;
    }
    else if (wasShowLights) {
        UpdateLightsLocked();
    }
    if (buzzBeepBlinked) {
        Boolean res;
        mHandler->Post(mBuzzBeepBlinked, &res);
    }

    return NOERROR;
}

AutoPtr<IAudioAttributes> NotificationManagerService::AudioAttributesForNotification(
    /* [in] */ INotification* n)
{
    AutoPtr<IAudioAttributes> attr;
    n->GetAudioAttributes((IAudioAttributes**)&attr);
    Int32 type = 0;
    n->GetAudioStreamType(&type);

    AutoPtr<IAudioSystem> audioSystem;
    CAudioSystem::AcquireSingleton((IAudioSystem**)&audioSystem);
    Int32 streamType;
    assert(0 && "TODO");
    // audioSystem->GetNumStreamTypes(&streamType);

    AutoPtr<INotificationHelper> helper;
    CNotificationHelper::AcquireSingleton((INotificationHelper**)&helper);
    AutoPtr<IAudioAttributes> result;
    helper->GetAUDIO_ATTRIBUTES_DEFAULT((IAudioAttributes**)&result);

    if (attr != NULL) {
        return attr;
    }
    else if (type >= 0 && type < streamType) {
        // the stream type is valid, use it
        AutoPtr<IAudioAttributesBuilder> builder;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
        builder->SetInternalLegacyStreamType(type);
        AutoPtr<IAudioAttributes> result;
        builder->Build((IAudioAttributes**)&result);
        return result;
    }
    else if (type == IAudioSystem::STREAM_DEFAULT) {
        return result;
    }
    else {
        String str("");
        str.AppendFormat("Invalid stream type: %d", type);
        Logger::W(TAG, str.string());
        return result;
    }
}

ECode NotificationManagerService::ShowNextToastLocked()
{
    AutoPtr<IInterface> obj;
    mToastQueue->Get(0, (IInterface**)&obj);
    AutoPtr<ToastRecord> record = (ToastRecord*)IObject::Probe(obj);
    while (record != NULL) {
        if (DBG) Slogger::D(TAG, "Show pkg= %s callback=%p",
                record->mPkg.string(), record->mCallback.Get());
        // try {
        ECode ec = record->mCallback->Show();
        if (SUCCEEDED(ec)) {
            ScheduleTimeoutLocked(record);
            return NOERROR;
        }
        // } catch (RemoteException e) {
        Slogger::W(TAG, "Object died trying to show notification %p in package %s",
                record->mCallback.Get(), record->mPkg.string());
        // remove it from the list and let the process die
        Int32 index;
        mToastQueue->IndexOf((IObject*)record, &index);
        if (index >= 0) {
            mToastQueue->Remove(index);
        }
        KeepProcessAliveLocked(record->mPid);
        Int32 size;
        if ((mToastQueue->GetSize(&size), size) > 0) {
            obj = NULL;
            mToastQueue->Get(0, (IInterface**)&obj);
            record = (ToastRecord*)IObject::Probe(obj);
        }
        else {
            record = NULL;
        }
        // }
    }
    return NOERROR;
}

ECode NotificationManagerService::CancelToastLocked(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> obj;
    mToastQueue->Get(index, (IInterface**)&obj);
    AutoPtr<ToastRecord> record = (ToastRecord*)IObject::Probe(obj);
    // try {
    ECode ec = record->mCallback->Hide();
    // }
    // catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::W(TAG, "Object died trying to hide notification %p in package %s"
                , record->mCallback.Get(), record->mPkg.string());
        // don't worry about this, we're about to remove it from
        // the list anyway
    }
    // }
    mToastQueue->Remove(index);
    KeepProcessAliveLocked(record->mPid);
    Int32 size;
    if ((mToastQueue->GetSize(&size), size) > 0) {
        // Show the next one. If the callback fails, this will remove
        // it from the list, so don't assume that the list hasn't changed
        // after this point.
        ShowNextToastLocked();
    }
    return NOERROR;
}

void NotificationManagerService::ScheduleTimeoutLocked(
    /* [in] */ ToastRecord* r)
{
    assert(r);
    mHandler->RemoveCallbacksAndMessages((IObject*)r);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    helper->Obtain(mHandler, MESSAGE_TIMEOUT, (IObject*)r, (IMessage**)&m);
    Int64 delay = r->mDuration == IToast::LENGTH_LONG ? LONG_DELAY : SHORT_DELAY;
    Boolean result;
    mHandler->SendMessageDelayed(m, delay, &result);
}

void NotificationManagerService::HandleTimeout(
    /* [in] */ ToastRecord* record)
{
    if (DBG) Slogger::D(TAG, "Timeout pkg=%s, callback=%p",
            record->mPkg.string(), record->mCallback.Get());

    synchronized(mToastQueue) {
        Int32 index = IndexOfToastLocked(record->mPkg, record->mCallback);
        if (index >= 0) {
            CancelToastLocked(index);
        }
    }
}

Int32 NotificationManagerService::IndexOfToastLocked(
    /* [in] */ const String& pkg,
    /* [in] */ IITransientNotification* callback)
{
    AutoPtr<IBinder> cbak = IBinder::Probe(callback);

    AutoPtr<IArrayList> list = mToastQueue;
    Int32 len = 0;
    list->GetSize(&len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ToastRecord> r = (ToastRecord*)IObject::Probe(obj);
        AutoPtr<IBinder> tmpCbak = IBinder::Probe(r->mCallback);
        if (r->mPkg.Equals(pkg) && tmpCbak == cbak) {
            return i;
        }
    }

    return -1;
}

void NotificationManagerService::KeepProcessAliveLocked(
    /* [in] */ Int32 pid)
{
    Int32 toastCount = 0; // toasts from this pid
    AutoPtr<IArrayList> list = mToastQueue;
    Int32 N = 0;
    list->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ToastRecord> r = (ToastRecord*)IObject::Probe(obj);
        if (r->mPid == pid) {
            toastCount++;
        }
    }
    // try {
    mAm->SetProcessForeground(mForegroundToken, pid, toastCount > 0);
    // } catch (RemoteException e) {
    //     // Shouldn't happen.
    // }
}

void NotificationManagerService::HandleRankingReconsideration(
    /* [in] */ IMessage* message)
{
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    IRankingReconsideration* rr = IRankingReconsideration::Probe(obj);
    if (rr == NULL) return;
    AutoPtr<RankingReconsideration> recon = (RankingReconsideration*)rr;
    recon->Run();
    Boolean changed;
    synchronized(mNotificationList) {
        AutoPtr<IInterface> obj;
        mNotificationsByKey->Get(CoreUtils::Convert(recon->GetKey()), (IInterface**)&obj);
        AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (record == NULL) {
            return;
        }
        Int32 indexBefore = FindNotificationRecordIndexLocked(record);
        Boolean interceptBefore = record->IsIntercepted();
        Int32 visibilityBefore = record->GetPackageVisibilityOverride();
        recon->ApplyChangesLocked(record);
        ApplyZenModeLocked(record);
        mRankingHelper->Sort(mNotificationList);
        Int32 indexAfter = FindNotificationRecordIndexLocked(record);
        Boolean interceptAfter = record->IsIntercepted();
        Int32 visibilityAfter = record->GetPackageVisibilityOverride();
        changed = indexBefore != indexAfter || interceptBefore != interceptAfter
                || visibilityBefore != visibilityAfter;
        if (interceptBefore && !interceptAfter) {
            BuzzBeepBlinkLocked(record);
        }
    }
    if (changed) {
        ScheduleSendRankingUpdate();
    }
}

void NotificationManagerService::HandleRankingConfigChange()
{
    synchronized(mNotificationList) {
        Int32 N = 0;
        mNotificationList->GetSize(&N);
        AutoPtr<IArrayList> orderBefore;
        CArrayList::New(N, (IArrayList**)&orderBefore);
        AutoPtr< ArrayOf<Int32> > visibilities = ArrayOf<Int32>::Alloc(N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mNotificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
            orderBefore->Add(CoreUtils::Convert(r->GetKey()));
            (*visibilities)[i] = r->GetPackageVisibilityOverride();
            mRankingHelper->ExtractSignals(r);
        }

        for (Int32 i = 0; i < N; i++) {
            mRankingHelper->Sort(mNotificationList);
            AutoPtr<IInterface> obj;
            mNotificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
            obj = NULL;
            orderBefore->Get(i, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            if (!str.Equals(r->GetKey())
                    || (*visibilities)[i] != r->GetPackageVisibilityOverride()) {
                ScheduleSendRankingUpdate();
                return;
            }
        }
    }
}

void NotificationManagerService::ApplyZenModeLocked(
    /* [in] */ NotificationRecord* record)
{
    record->SetIntercepted(mZenModeHelper->ShouldIntercept(record));
}

Int32 NotificationManagerService::FindNotificationRecordIndexLocked(
    /* [in] */ NotificationRecord* target)
{
    return mRankingHelper->IndexOf(mNotificationList, target);
}

void NotificationManagerService::ScheduleSendRankingUpdate()
{
    mHandler->RemoveMessages(MESSAGE_SEND_RANKING_UPDATE);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    helper->Obtain(mHandler, MESSAGE_SEND_RANKING_UPDATE, (IMessage**)&m);
    Boolean res;
    mHandler->SendMessage(m, &res);
}

void NotificationManagerService::HandleSendRankingUpdate()
{
    synchronized(mNotificationList) {
        mListeners->NotifyRankingUpdateLocked();
    }
}

void NotificationManagerService::ScheduleListenerHintsChanged(
    /* [in] */ Int32 state)
{
    mHandler->RemoveMessages(MESSAGE_LISTENER_HINTS_CHANGED);
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MESSAGE_LISTENER_HINTS_CHANGED, state, 0, (IMessage**)&m);
    m->SendToTarget();
}

void NotificationManagerService::ScheduleInterruptionFilterChanged(
    /* [in] */ Int32 listenerInterruptionFilter)
{
    mHandler->RemoveMessages(MESSAGE_LISTENER_NOTIFICATION_FILTER_CHANGED);
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(
            MESSAGE_LISTENER_NOTIFICATION_FILTER_CHANGED,
            listenerInterruptionFilter,
            0, (IMessage**)&m);
    m->SendToTarget();
}

void NotificationManagerService::HandleListenerHintsChanged(
    /* [in] */ Int32 hints)
{
    synchronized(mNotificationList) {
        mListeners->NotifyListenerHintsChangedLocked(hints);
    }
}

void NotificationManagerService::HandleListenerInterruptionFilterChanged(
    /* [in] */ Int32 interruptionFilter)
{
    synchronized(mNotificationList) {
        mListeners->NotifyInterruptionFilterChanged(interruptionFilter);
    }
}

// Notifications
// ============================================================================

Int32 NotificationManagerService::Clamp(
    /* [in] */ Int32 x,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    return (x < low) ? low : ((x > high) ? high : x);
}

void NotificationManagerService::UpdateDisableDucking()
{
    if (!mSystemReady) {
        return;
    }
    AutoPtr<IInterface> p;
    mContext->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&p);
    AutoPtr<IMediaSessionManager> mediaSessionManager = IMediaSessionManager::Probe(p);
    mediaSessionManager->RemoveOnActiveSessionsChangedListener(mSessionListener);
    if (mDisableDuckingWhileMedia) {
        mediaSessionManager->AddOnActiveSessionsChangedListener(mSessionListener, NULL);
    }
}

Int32 NotificationManagerService::GetNotificationHash(
    /* [in] */ INotification* notification,
    /* [in] */ const String& packageName)
{
    AutoPtr<ICharSequence> message;// = SpamFilter::GetNotificationContent(notification);
    String str;
    message->ToString(&str);
    str += ":";
    str += packageName;
    Int32 hc = str.GetHashCode();
    return hc;
}

Boolean NotificationManagerService::IsNotificationSpam(
    /* [in] */ INotification* notification,
    /* [in] */ const String& basePkg)
{
    Int32 notificationHash = GetNotificationHash(notification, basePkg);
    AutoPtr<IInteger32> iNotificationHash;
    CInteger32::New(notificationHash, (IInteger32**)&iNotificationHash);
    Boolean isSpam = FALSE;
    if (mSpamCache->Get(iNotificationHash) != NULL) {
        isSpam = TRUE;
    }
    else {
        String msg;// = SpamFilter::GetNotificationContent(notification);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ICursor> c;
        assert(0 && "TODO");
        // resolver->Query(FILTER_MSG_URI, NULL, IS_FILTERED_QUERY,
        //         new String[]{SpamFilter::GetNormalizedContent(msg), basePkg}, NULL, (ICursor**)&c);
        if (c != NULL) {
            Boolean bMF = FALSE;
            c->MoveToFirst(&bMF);
            if (bMF) {
                AutoPtr<FilterCacheInfo> info = new FilterCacheInfo();
                info->mPackageName = basePkg;
                Int32 ci = 0;
                assert(0 && "TODO");
                // c->GetColumnIndex(INotificationTable::ID, &ci);
                Int32 notifId = 0;
                c->GetInt32(ci, &notifId);
                info->mNotificationId = notifId;
                mSpamCache->Put(iNotificationHash, info);
                isSpam = TRUE;
            }
            ICloseable::Probe(c)->Close();
        }
    }
    if (isSpam) {
        AutoPtr<FilterCacheInfo> info = mSpamCache->Get(iNotificationHash);
        Int32 notifId = info->mNotificationId;
        AutoPtr<SpamExecutorRunnable> r = new SpamExecutorRunnable(notifId, this);
        AutoPtr<IFuture> fut;
        mSpamExecutor->Submit(r, (IFuture**)&fut);
    }
    return isSpam;
}

void NotificationManagerService::SendAccessibilityEvent(
    /* [in] */ INotification* notification,
    /* [in] */ ICharSequence* packageName)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAccessibilityManagerHelper> amHelper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&amHelper);
    AutoPtr<IAccessibilityManager> manager;
    amHelper->GetInstance(context, (IAccessibilityManager**)&manager);

    Boolean isEnabled = FALSE;
    manager->IsEnabled(&isEnabled);
    if (isEnabled == FALSE) {
        return;
    }

    AutoPtr<IAccessibilityEventHelper> eventHelper;
    CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&eventHelper);

    AutoPtr<IAccessibilityEvent> event;
    eventHelper->Obtain(IAccessibilityEvent::TYPE_NOTIFICATION_STATE_CHANGED, (IAccessibilityEvent**)&event);

    AutoPtr<ICharSequence> clsSeq;
    CString::New(String("CNotification"), (ICharSequence**)&clsSeq);
    event->SetPackageName(packageName);
    IAccessibilityRecord::Probe(event)->SetClassName(clsSeq);
    IAccessibilityRecord::Probe(event)->SetParcelableData(IParcelable::Probe(notification));

    AutoPtr<ICharSequence> tickerText;
    notification->GetTickerText((ICharSequence**)&tickerText);

    if (!TextUtils::IsEmpty(tickerText)) {
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Add(tickerText);
    }

    manager->SendAccessibilityEvent(event);
}

void NotificationManagerService::CancelNotificationLocked(
    /* [in] */ NotificationRecord* record,
    /* [in] */ Boolean sendDelete,
    /* [in] */ Int32 reason)
{
    // tell the app
    AutoPtr<INotification> notification = record->GetNotification();
    if (sendDelete) {
        AutoPtr<IPendingIntent> intent;
        notification->GetDeleteIntent((IPendingIntent**)&intent);

        if (intent != NULL) {
            ECode ec = intent->Send();

            if (FAILED(ec)) {
                // do nothing - there's no relevant way to recover, and
                //     no reason to let this propagate
                String str;
                record->mSbn->GetPackageName(&str);
                Logger::W(TAG, "canceled PendingIntent for %s", str.string());
            }
        }
    }

    // status bar
    Int32 icon = 0;
    notification->GetIcon(&icon);

    if (icon != 0) {
        record->mIsCanceled = TRUE;
        mListeners->NotifyRemovedLocked(record->mSbn);
    }

    // sound
    if (mSoundNotification.Get() == record) {
        mSoundNotification = NULL;

        Int64 identity = Binder::ClearCallingIdentity();
        AutoPtr<IIRingtonePlayer> player;
        mAudioManager->GetRingtonePlayer((IIRingtonePlayer**)&player);
        if (player != NULL) {
            player->StopAsync();
        }
        Binder::RestoreCallingIdentity(identity);
    }

    // vibrate
    if (mVibrateNotification.Get() == record) {
        mVibrateNotification = NULL;

        Int64 identity = Binder::ClearCallingIdentity();
        mVibrator->Cancel();
        Binder::RestoreCallingIdentity(identity);
    }

    // light
    mLights->Remove(CoreUtils::Convert(record->GetKey()));
    if (mLedNotification.Get() == record) {
        mLedNotification = NULL;
    }

    // Record usage stats
    switch (reason) {
        case REASON_DELEGATE_CANCEL:
        case REASON_DELEGATE_CANCEL_ALL:
        case REASON_LISTENER_CANCEL:
        case REASON_LISTENER_CANCEL_ALL:
            mUsageStats->RegisterDismissedByUser(record);
            break;
        case REASON_NOMAN_CANCEL:
        case REASON_NOMAN_CANCEL_ALL:
            mUsageStats->RegisterRemovedByApp(record);
            break;
        case REASON_DELEGATE_CLICK:
            mUsageStats->RegisterCancelDueToClick(record);
            break;
        default:
            mUsageStats->RegisterCancelUnknown(record);
            break;
    }

    String key;
    record->mSbn->GetKey(&key);
    mNotificationsByKey->Remove(CoreUtils::Convert(key));

    // Save it for users of getHistoricalNotifications()
    mArchive->Record(record->mSbn);
}

void NotificationManagerService::CancelNotification(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 mustHaveFlags,
    /* [in] */ Int32 mustNotHaveFlags,
    /* [in] */ Boolean sendDelete,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ ManagedServices::ManagedServiceInfo* listener)
{
    // In enqueueNotificationInternal notifications are added by scheduling the
    // work on the worker handler. Hence, we also schedule the cancel on this
    // handler to avoid a scenario where an add notification call followed by a
    // remove notification call ends up in not removing the notification.
    AutoPtr<CancelNotificationRunnable> runnable = new CancelNotificationRunnable(
            this, callingUid, callingPid, pkg, tag, id, mustHaveFlags,
            mustNotHaveFlags, sendDelete, userId, reason, listener);
    Boolean res;
    mHandler->Post(runnable, &res);
}

Boolean NotificationManagerService::NotificationMatchesUserId(
    /* [in] */ NotificationRecord* record,
    /* [in] */ Int32 userId)
{
    return
            // looking for USER_ALL notifications? match everything
            userId == IUserHandle::USER_ALL
            // a notification sent to USER_ALL matches any query
            || record->GetUserId() == IUserHandle::USER_ALL
            // an exact user match
            || record->GetUserId() == userId;
}

Boolean NotificationManagerService::NotificationMatchesCurrentProfiles(
    /* [in] */ NotificationRecord* record,
    /* [in] */ Int32 userId)
{
    return NotificationMatchesUserId(record, userId)
            || mUserProfiles->IsCurrentProfile(record->GetUserId());
}

Boolean NotificationManagerService::CancelAllNotificationsInt(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 mustHaveFlags,
    /* [in] */ Int32 mustNotHaveFlags,
    /* [in] */ Boolean doit,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ ManagedServices::ManagedServiceInfo* listener)
{
    String listenerName;
    if (listener == NULL) {
        listenerName = String(NULL);
    }
    else {
        listener->mComponent->ToShortString(&listenerName);
    }

    // TODO
    // EventLogTags::WriteNotificationCancelAll(callingUid, callingPid,
    //         pkg, userId, mustHaveFlags, mustNotHaveFlags, reason,
    //         listenerName);

    synchronized(mNotificationList) {
        Int32 N = 0;
        mNotificationList->GetSize(&N);
        AutoPtr<IArrayList> canceledNotifications;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mNotificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);

            if (!NotificationMatchesUserId(r, userId)) {
                continue;
            }

            // Don't remove notifications to all, if there's no package name specified
            if (r->GetUserId() == IUserHandle::USER_ALL && pkg.IsNull()) {
                continue;
            }

            if ((r->GetFlags() & mustHaveFlags) != mustHaveFlags) {
                continue;
            }

            if ((r->GetFlags() & mustNotHaveFlags) != 0) {
                continue;
            }

            String packageName;
            r->mSbn->GetPackageName(&packageName);
            if (!pkg.IsNull() && !packageName.Equals(pkg)) {
                continue;
            }

            if (canceledNotifications == NULL) {
                CArrayList::New((IArrayList**)&canceledNotifications);
            }
            canceledNotifications->Add((IObject*)r);

            if (!doit) {
                return TRUE;
            }

            mNotificationList->Remove(i);
            CancelNotificationLocked(r, FALSE, reason);
        }

        if (doit && canceledNotifications != NULL) {
            Int32 M = 0;
            canceledNotifications->GetSize(&M);
            for (Int32 i = 0; i < M; i++) {
                AutoPtr<IInterface> obj;
                canceledNotifications->Get(i, (IInterface**)&obj);
                AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
                CancelGroupChildrenLocked(record, callingUid, callingPid, listenerName);
            }
        }

        if (canceledNotifications) {
            UpdateLightsLocked();
        }

        return canceledNotifications != NULL;
    }
    return FALSE;
}

void NotificationManagerService::CancelAllLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 reason,
    /* [in] */ ManagedServices::ManagedServiceInfo* listener,
    /* [in] */ Boolean includeCurrentProfiles)
{
    String listenerName;
    if (listener == NULL) {
        listenerName = String(NULL);
    }
    else {
        listener->mComponent->ToShortString(&listenerName);
    }

    // TODO
    // EventLogTags::WriteNotificationCancelAll(callingUid, callingPid,
    //         null, userId, 0, 0, reason, listenerName);

    AutoPtr<IArrayList> canceledNotifications;
    Int32 N = 0;
    mNotificationList->GetSize(&N);
    for (Int32 i = N-1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mNotificationList->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (includeCurrentProfiles) {
            if (!NotificationMatchesCurrentProfiles(r, userId)) {
                continue;
            }
        }
        else {
            if (!NotificationMatchesUserId(r, userId)) {
                continue;
            }
        }

        if ((r->GetFlags() & (INotification::FLAG_ONGOING_EVENT
                | INotification::FLAG_NO_CLEAR)) == 0) {
            mNotificationList->Remove(i);
            CancelNotificationLocked(r, TRUE, reason);
            // Make a note so we can cancel children later.
            if (canceledNotifications == NULL) {
                CArrayList::New((IArrayList**)&canceledNotifications);
            }
            canceledNotifications->Add((INotificationRecord*)r);
        }
    }
    Int32 M = 0;
    if (canceledNotifications != NULL) {
        canceledNotifications->GetSize(&M);
    }
    for (Int32 i = 0; i < M; i++) {
        AutoPtr<IInterface> obj;
        canceledNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);
        CancelGroupChildrenLocked(r, callingUid, callingPid,
                listenerName);
    }
    UpdateLightsLocked();
}

void NotificationManagerService::CancelGroupChildrenLocked(
    /* [in] */ NotificationRecord* r,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 callingPid,
    /* [in] */ const String& listenerName)
{
    AutoPtr<INotification> n = r->GetNotification();
    Boolean res = FALSE;
    if (n->IsGroupSummary(&res), !res) {
        return;
    }

    String pkg;
    r->mSbn->GetPackageName(&pkg);
    // Int32 userId = r->GetUserId();

    if (pkg.IsNull()) {
        if (DBG) {
            Logger::E(TAG, "No package for group summary: %s", r->GetKey().string());
        }
        return;
    }

    Int32 N = 0;
    mNotificationList->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mNotificationList->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> childR = (NotificationRecord*)INotificationRecord::Probe(obj);
        AutoPtr<IStatusBarNotification> childSbn = childR->mSbn;
        if ((childR->GetNotification()->IsGroupChild(&res), res) &&
                childR->GetGroupKey().Equals(r->GetGroupKey())) {
            //TODO
            // EventLogTags.WriteNotificationCancel(callingUid, callingPid,
            //         pkg, childSbn.getId(), childSbn.getTag(), userId, 0, 0,
            //         REASON_GROUP_SUMMARY_CANCELED, listenerName);
            mNotificationList->Remove(i);
            CancelNotificationLocked(childR, FALSE, REASON_GROUP_SUMMARY_CANCELED);
        }
    }
}

Boolean NotificationManagerService::IsLedNotificationForcedOn(
    /* [in] */ INotificationRecord* r)
{
    if (r != NULL) {
        AutoPtr<NotificationRecord> _r = (NotificationRecord*)r;
        AutoPtr<INotification> n;
        _r->mSbn->GetNotification((INotification**)&n);
        AutoPtr<IBundle> ext;
        n->GetExtras((IBundle**)&ext);
        if (ext != NULL) {
            Boolean res = FALSE;
            ext->GetBoolean(INotification::EXTRA_FORCE_SHOW_LIGHTS, FALSE, &res);
            return res;
        }
    }
    return FALSE;
}

void NotificationManagerService::UpdateLightsLocked()
{
    // handle notification lights
    if (mLedNotification == NULL) {
        // use most recent light with highest score
        Int32 size = 0;
        mLights->GetSize(&size);
        for (Int32 i = size; i > 0; i--) {
            AutoPtr<IInterface> p;
            mLights->Get(i - 1, (IInterface**)&p);
            AutoPtr<IInterface> val;
            mNotificationsByKey->Get(p, (IInterface**)&val);
            AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(val);
            if (mLedNotification == NULL) {
                mLedNotification = r;
            }
            Int32 rScore = 0, lScore = 0;
            r->mSbn->GetScore(&rScore);
            mLedNotification->mSbn->GetScore(&lScore);
            if (rScore > lScore) {
                mLedNotification = r;
            }
        }
    }

    // Don't flash while we are in a call or screen is on
    // (unless Notification has EXTRA_FORCE_SHOW_LGHTS)
    Boolean enableLed;
    if (mLedNotification == NULL) {
        enableLed = FALSE;
    }
    else if (IsLedNotificationForcedOn(mLedNotification)) {
        enableLed = TRUE;
    }
    else if (mInCall || mScreenOn) {
        enableLed = FALSE;
    }
    else {
        enableLed = TRUE;
    }

    if (!enableLed) {
        mNotificationLight->TurnOff();
        Slogger::I(TAG, "TODO: StatusBar");
        // mStatusBar->NotificationLightOff();
    }
    else {
        AutoPtr<INotification> ledno;
        mLedNotification->mSbn->GetNotification((INotification**)&ledno);

        AutoPtr<NotificationLedValues> ledValues = GetLedValuesForNotification(mLedNotification);
        Int32 ledARGB;
        Int32 ledOnMS;
        Int32 ledOffMS;

        Int32 noDefalt = 0;
        if (ledValues != NULL) {
            ledARGB = ledValues->mColor != 0 ? ledValues->mColor : mDefaultNotificationColor;
            ledOnMS = ledValues->mOnMS >= 0 ? ledValues->mOnMS : mDefaultNotificationLedOn;
            ledOffMS = ledValues->mOffMS >= 0 ? ledValues->mOffMS : mDefaultNotificationLedOff;
        }
        else if (((ledno->GetDefaults(&noDefalt), noDefalt) & INotification::DEFAULT_LIGHTS) != 0) {
            ledARGB = mDefaultNotificationColor;
            ledOnMS = mDefaultNotificationLedOn;
            ledOffMS = mDefaultNotificationLedOff;
        }
        else {
            ledno->GetLedARGB(&ledARGB);
            ledno->GetLedOnMS(&ledOnMS);
            ledno->GetLedOffMS(&ledOffMS);
        }

        if (mNotificationPulseEnabled) {
            // pulse repeatedly
            mNotificationLight->SetFlashing(ledARGB, Light::LIGHT_FLASH_TIMED, ledOnMS, ledOffMS);
        }

        // let SystemUI make an independent decision
        Slogger::I(TAG, "TODO: StatusBar");
        // mStatusBar->NotificationLightPulse(ledARGB, ledOnMS, ledOffMS);
    }
}

void NotificationManagerService::ParseNotificationPulseCustomValuesString(
    /* [in] */ const String& customLedValuesString)
{
    if (TextUtils::IsEmpty(customLedValuesString)) {
        return;
    }

    AutoPtr<ArrayOf<String> > clvs;
    StringUtils::Split(customLedValuesString, "\\|", (ArrayOf<String>**)&clvs);
    for (Int32 i = 0; i < clvs->GetLength(); i++) {
        String packageValuesString = (*clvs)[i];
        AutoPtr<ArrayOf<String> > packageValues;
        StringUtils::Split(packageValuesString, "=", (ArrayOf<String>**)&packageValues);
        if (packageValues->GetLength() != 2) {
            Logger::E(TAG, "Error parsing custom led values for unknown package");
            continue;
        }
        String packageName = (*packageValues)[0];
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split((*packageValues)[1], ";", (ArrayOf<String>**)&values);
        if (values->GetLength() != 3) {
            Logger::E(TAG, "Error parsing custom led values '%s' for %s",
                    (const char*)((*packageValues)[1]), (const char*)packageName);
            continue;
        }
        AutoPtr<NotificationLedValues> ledValues = new NotificationLedValues();
        // try {
        ledValues->mColor = StringUtils::ParseInt32((*values)[0]);
        ledValues->mOnMS = StringUtils::ParseInt32((*values)[1]);
        ledValues->mOffMS = StringUtils::ParseInt32((*values)[2]);
        // } catch (NumberFormatException e) {
        //     Log.e(TAG, "Error parsing custom led values '"
        //             + packageValues[1] + "' for " + packageName);
        //     continue;
        // }
        AutoPtr<ICharSequence> pPakName;
        CString::New(packageName, (ICharSequence**)&pPakName);
        mNotificationPulseCustomLedValues->Put(pPakName, (IObject*)ledValues.Get());
    }
}

AutoPtr<NotificationManagerService::NotificationLedValues> NotificationManagerService::GetLedValuesForNotification(
    /* [in] */ INotificationRecord* ledNotification)
{
    AutoPtr<NotificationRecord> _ledNotification = (NotificationRecord*)ledNotification;
    String packageName;
    _ledNotification->mSbn->GetPackageName(&packageName);
    String str = MapPackage(packageName);
    AutoPtr<ICharSequence> pStr;
    CString::New(str, (ICharSequence**)&pStr);
    AutoPtr<IInterface> p;
    mNotificationPulseCustomLedValues->Get(pStr, (IInterface**)&p);
    return (NotificationLedValues*)(IObject*)p.Get();
}

String NotificationManagerService::MapPackage(
    /* [in] */ const String& pkg)
{
    AutoPtr<ICharSequence> pPkg;
    CString::New(pkg, (ICharSequence**)&pPkg);
    Boolean bContain = FALSE;
    mPackageNameMappings->ContainsKey(pPkg, &bContain);
    if (!bContain) {
        return pkg;
    }
    AutoPtr<IInterface> p;
    mPackageNameMappings->Get(pPkg, (IInterface**)&p);
    AutoPtr<ICharSequence> pRes = ICharSequence::Probe(p);
    String str;
    pRes->ToString(&str);
    return str;
}

Int32 NotificationManagerService::IndexOfNotificationLocked(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    AutoPtr<IArrayList> list = mNotificationList;
    Int32 len = 0;
    list->GetSize(&len);

    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);

        Int32 _id = 0;
        r->mSbn->GetId(&_id);
        if (!NotificationMatchesUserId(r, userId) || _id != id) {
            continue;
        }

        String _tag;
        r->mSbn->GetTag(&_tag);
        if (tag.IsNull()) {
            if (!_tag.IsNull()) {
                continue;
            }
        }
        else {
            if (!tag.Equals(_tag)) {
                continue;
            }
        }

        String _pkg;
        r->mSbn->GetPackageName(&_pkg);
        if (pkg.Equals(_pkg)) {
            return i;
        }
    }

    return -1;
}

Int32 NotificationManagerService::IndexOfNotificationLocked(
    /* [in] */ const String& key)
{
    Int32 N = 0;
    mNotificationList->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mNotificationList->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (key.Equals(record->GetKey())) {
            return i;
        }
    }
    return -1;
}

void NotificationManagerService::UpdateNotificationPulse()
{
    synchronized(mNotificationList) {
        UpdateLightsLocked();
    }
}

Boolean NotificationManagerService::IsUidSystem(
    /* [in] */ Int32 uid)
{
    Int32 appid = UserHandle::GetAppId(uid);
    return (appid == IProcess::SYSTEM_UID || appid == IProcess::PHONE_UID || uid == 0);
}

Boolean NotificationManagerService::IsCallerSystem()
{
    return IsUidSystem(Binder::GetCallingUid());
}

ECode NotificationManagerService::CheckCallerIsSystem()
{
    if (IsCallerSystem()) {
        return NOERROR;
    }
    Slogger::E(TAG, "Disallowed call for uid %d", Binder::GetCallingUid());
    return E_SECURITY_EXCEPTION;
    // throw new SecurityException("Disallowed call for uid " + Binder::GetCallingUid());
}

ECode NotificationManagerService::CheckCallerIsSystemOrSameApp(
    /* [in] */ const String& pkg)
{
    if (IsCallerSystem()) {
        return NOERROR;
    }

    const Int32 uid = Binder::GetCallingUid();

    // try {
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    AutoPtr<IApplicationInfo> ai;
    ECode ec = pm->GetApplicationInfo(pkg, 0, UserHandle::GetCallingUserId(), (IApplicationInfo**)&ai);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Unknown package %s, 0x%08x", pkg.string(), ec);
        return E_SECURITY_EXCEPTION;
        // } catch (RemoteException re) {
        //     throw new SecurityException("Unknown package " + pkg + "\n" + re);
        // }
    }
    if (ai == NULL) {
        Slogger::E(TAG, "Unknown package %s", pkg.string());
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Unknown package " + pkg);
    }
    Int32 appUid;
    ai->GetUid(&appUid);
    if (!UserHandle::IsSameApp(appUid, uid)) {
        Slogger::E(TAG, "Calling uid %d gave package %s which is owned by uid %d",
                uid, pkg.string(), appUid);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Calling uid " + uid + " gave package"
        //         + pkg + " which is owned by uid " + ai.uid);
    }

    return NOERROR;
}

String NotificationManagerService::CallStateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case ITelephonyManager::CALL_STATE_IDLE:
            return String("CALL_STATE_IDLE");
        case ITelephonyManager::CALL_STATE_RINGING:
            return String("CALL_STATE_RINGING");
        case ITelephonyManager::CALL_STATE_OFFHOOK:
            return String("CALL_STATE_OFFHOOK");
        default:
            return String("CALL_STATE_UNKNOWN_") + StringUtils::ToString(state);
    }
}

void NotificationManagerService::ListenForCallState()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> manager;
    helper->From(context, (ITelephonyManager**)&manager);
    Slogger::I(TAG, "TODO: ITelephonyManager");
    // manager->Listen(new InnerSub_PhoneStateListener(this), IPhoneStateListener::LISTEN_CALL_STATE);
}

AutoPtr<INotificationRankingUpdate> NotificationManagerService::MakeRankingUpdateLocked(
    /* [in] */ ManagedServices::ManagedServiceInfo* info)
{
    Int32 speedBumpIndex = -1;
    Int32 N;
    mNotificationList->GetSize(&N);
    AutoPtr<IArrayList> keys;
    CArrayList::New(N, (IArrayList**)&keys);
    AutoPtr<IArrayList> interceptedKeys;
    CArrayList::New(N, (IArrayList**)&interceptedKeys);
    AutoPtr<IBundle> visibilityOverrides;
    CBundle::New((IBundle**)&visibilityOverrides);
    Int32 size;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mNotificationList->Get(i, (IInterface**)&obj);
        AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
        if (!IsVisibleToListener(record->mSbn, info)) {
            continue;
        }
        String key;
        record->mSbn->GetKey(&key);
        keys->Add(CoreUtils::Convert(key));
        if (record->IsIntercepted()) {
            interceptedKeys->Add(CoreUtils::Convert(key));
        }
        if (record->GetPackageVisibilityOverride()
                != INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE) {
            visibilityOverrides->PutInt32(key,
                    record->GetPackageVisibilityOverride());
        }
        // Find first min-prio notification for speedbump placement.
        AutoPtr<INotification> notification;
        record->mSbn->GetNotification((INotification**)&notification);
        Int32 priority;
        notification->GetPriority(&priority);
        if (speedBumpIndex == -1 &&
                // Intrusiveness trumps priority, hence ignore intrusives.
                !record->IsRecentlyIntrusive() &&
                // Currently, package priority is either PRIORITY_DEFAULT or PRIORITY_MAX, so
                // scanning for PRIORITY_MIN within the package bucket PRIORITY_DEFAULT
                // (or lower as a safeguard) is sufficient to find the speedbump index.
                // We'll have to revisit this when more package priority buckets are introduced.
                record->GetPackagePriority() <= INotification::PRIORITY_DEFAULT &&
                priority == INotification::PRIORITY_MIN) {
            keys->GetSize(&size);
            speedBumpIndex = size - 1;
        }
    }

    keys->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > inArgs = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > outArgs;
    keys->ToArray(inArgs, (ArrayOf<IInterface*>**)&outArgs);
    AutoPtr< ArrayOf<String> > keysAr = ArrayOf<String>::Alloc(outArgs->GetLength());
    for (Int32 i = 0; i < outArgs->GetLength(); i++) {
        ICharSequence::Probe((*outArgs)[i])->ToString(&(*keysAr)[i]);
    }

    interceptedKeys->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > outArray;
    interceptedKeys->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    AutoPtr< ArrayOf<String> > interceptedKeysAr = ArrayOf<String>::Alloc(outArray->GetLength());
    for (Int32 i = 0; i < outArray->GetLength(); i++) {
        ICharSequence::Probe((*outArray)[i])->ToString(&(*interceptedKeysAr)[i]);
    }

    AutoPtr<INotificationRankingUpdate> update;
    CNotificationRankingUpdate::New(keysAr, interceptedKeysAr, visibilityOverrides,
            speedBumpIndex, (INotificationRankingUpdate**)&update);
    return update;
}

Boolean NotificationManagerService::IsVisibleToListener(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ ManagedServices::ManagedServiceInfo* listener)
{
    Int32 uid = 0;
    sbn->GetUserId(&uid);
    if (!listener->EnabledAndUserMatches(uid)) {
        return FALSE;
    }
    // TODO: remove this for older listeners.
    return TRUE;
}

} // Notification
} // Server
} // Droid
} // Elastos
