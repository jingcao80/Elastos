
#include "elastos/droid/phone/NotificationMgr.h"

namespace Elastos {
namespace Droid {
namespace Phone {

NotificationMgr::StatusBarHelper::StatusBarHelper()
    : mIsNotificationEnabled(TRUE)
    , mIsExpandedViewEnabled(TRUE)
    , mIsSystemBarNavigationEnabled(TRUE)
{
}

ECode NotificationMgr::StatusBarHelper::EnableNotificationAlerts(
    /* [in] */ Boolean enable)
{
    if (mIsNotificationEnabled != enable) {
        mIsNotificationEnabled = enable;
        UpdateStatusBar();
    }
    return NOERROR;
}

ECode NotificationMgr::StatusBarHelper::EnableExpandedView(
    /* [in] */ Boolean enable)
{
    if (mIsExpandedViewEnabled != enable) {
        mIsExpandedViewEnabled = enable;
        UpdateStatusBar();
    }
    return NOERROR;
}

ECode NotificationMgr::StatusBarHelper::EnableSystemBarNavigation(
    /* [in] */ Boolean enable)
{
    if (mIsSystemBarNavigationEnabled != enable) {
        mIsSystemBarNavigationEnabled = enable;
        UpdateStatusBar();
    }
    return NOERROR;
}

void NotificationMgr::StatusBarHelper::UpdateStatusBar()
{
    Int32 state = IStatusBarManager::DISABLE_NONE;

    if (!mIsExpandedViewEnabled) {
        state |= IStatusBarManager::DISABLE_EXPAND;
    }
    if (!mIsNotificationEnabled) {
        state |= IStatusBarManager::DISABLE_NOTIFICATION_ALERTS;
    }
    if (!mIsSystemBarNavigationEnabled) {
        // Disable *all* possible navigation via the system bar.
        state |= IStatusBarManager::DISABLE_HOME;
        state |= IStatusBarManager::DISABLE_RECENT;
        state |= IStatusBarManager::DISABLE_BACK;
        state |= IStatusBarManager::DISABLE_SEARCH;
    }

    if (DBG) {
        StringBuilder sb;
        sb += "updateStatusBar: state = 0x";
        sb += StringUtils::ToHexString(state);
        Log(sb.ToString());
    }
    mStatusBarManager->Disable(state);
}

const String NotificationMgr::LOG_TAG("NotificationMgr");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 tmp;
    helper->GetInt32(String("ro.debuggable"), 0, &tmp);

    return (PhoneGlobals.DBG_LEVEL >= 1) && (tmp == 1);
}

const Boolean NotificationMgr::DBG = initDBG();
// Do not check in with VDBG = true, since that may write PII to the system log.
const Boolean NotificationMgr::VDBG = FALSE;

// notification types
const Int32 NotificationMgr::MMI_NOTIFICATION = 1;
const Int32 NotificationMgr::NETWORK_SELECTION_NOTIFICATION = 2;
const Int32 NotificationMgr::VOICEMAIL_NOTIFICATION = 3;
const Int32 NotificationMgr::CALL_FORWARD_NOTIFICATION = 4;
const Int32 NotificationMgr::DATA_DISCONNECTED_ROAMING_NOTIFICATION = 5;
const Int32 NotificationMgr::SELECTED_OPERATOR_FAIL_NOTIFICATION = 6;

/** The singleton NotificationMgr instance. */
AutoPtr<NotificationMgr> NotificationMgr::sInstance;

const Int32 NotificationMgr::MAX_VM_NUMBER_RETRIES = 5;
const Int32 NotificationMgr::VM_NUMBER_RETRY_DELAY_MILLIS = 10000;

static AutoPtr<ArrayOf<String> > initPHONES_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);

    array->Set(0, IPhoneLookup::NUMBER);
    array->Set(1, IPhoneLookup::DISPLAY_NAME);
    array->Set(2, IPhoneLookup::_ID);
    return array;
}

const AutoPtr<ArrayOf<String> > PHONES_PROJECTION = initPHONES_PROJECTION();

NotificationMgr::NotificationMgr(
    /* [in] */ IPhoneGlobals* app)
    : mSelectedUnavailableNotify(FALSE)
    , mVmNumberRetriesRemaining(MAX_VM_NUMBER_RETRIES)
    , mApp(app)
    , mContext(app)
{
    AutoPtr<IInterface> obj;
    app->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    mNotificationManager = INotificationManager::Probe(obj);

    AutoPtr<IInterface> obj2;
    app->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj2);
    mStatusBarManager = IStatusBarManager::Probe(obj2);

    AutoPtr<IInterface> obj3;
    app->GetSystemService(Context.USER_SERVICE, (IInterface**)&obj3);
    mUserManager = IUserManager::Probe(obj3);

    mPhone = app->mPhone;  // TODO: better style to use mCM.getDefaultPhone() everywhere instead
    statusBarHelper = new StatusBarHelper();
}

AutoPtr<NotificationMgr> NotificationMgr::Init(
    /* [in] */ IPhoneGlobals* app)
{
    {
        assert(0 && "synchronized (NotificationMgr.class)");
        AutoLock syncLock(this);
        if (sInstance == NULL) {
            sInstance = new NotificationMgr(app);
        }
        else {
            StringBuilder sb;
            sb += "init() called multiple times!  sInstance = ";
            sb += TO_CSTR(sInstance);
            Logger::Wtf(LOG_TAG, sb.ToString());
        }
        return sInstance;
    }
}

ECode NotificationMgr::UpdateMwi(
    /* [in] */ Boolean visible)
{
    if (DBG) {
        StringBuilder sb;
        sb += "updateMwi(): ";
        sb += visible;
        log(sb.ToString());
    }

    if (visible) {
        Int32 resId = android.R.drawable.stat_notify_voicemail;

        // This Notification can get a lot fancier once we have more
        // information about the current voicemail messages.
        // (For example, the current voicemail system can't tell
        // us the caller-id or timestamp of a message, or tell us the
        // message count.)

        // But for now, the UI is ultra-simple: if the MWI indication
        // is supposed to be visible, just show a single generic
        // notification.

        String notificationTitle;
        mContext->GetString(R.string.notification_voicemail_title, &notificationTitle);
        String vmNumber;
        mPhone->GetVoiceMailNumber(&vmNumber);
        if (DBG) {
            StringBuilder sb;
            sb += "- got vm number: '";
            sb += vmNumber;
            sb += "'")
            Log(sb.ToString());
        }

        // Watch out: vmNumber may be null, for two possible reasons:
        //
        //   (1) This phone really has no voicemail number
        //
        //   (2) This phone *does* have a voicemail number, but
        //       the SIM isn't ready yet.
        //
        // Case (2) *does* happen in practice if you have voicemail
        // messages when the device first boots: we get an MWI
        // notification as soon as we register on the network, but the
        // SIM hasn't finished loading yet.
        //
        // So handle case (2) by retrying the lookup after a short
        // delay.

        Boolean res;
        if ((vmNumber == NULL) && (mPhone->GetIccRecordsLoaded(&res), !res)) {
            if (DBG) Log(String("- Null vm number: SIM records not loaded (yet)..."));

            // TODO: rather than retrying after an arbitrary delay, it
            // would be cleaner to instead just wait for a
            // SIM_RECORDS_LOADED notification.
            // (Unfortunately right now there's no convenient way to
            // get that notification in phone app code.  We'd first
            // want to add a call like registerForSimRecordsLoaded()
            // to Phone.java and GSMPhone.java, and *then* we could
            // listen for that in the CallNotifier class.)

            // Limit the number of retries (in case the SIM is broken
            // or missing and can *never* load successfully.)
            if (mVmNumberRetriesRemaining-- > 0) {
                if (DBG) {
                    StringBuilder sb;
                    sb += "  - Retrying in ";
                    sb += VM_NUMBER_RETRY_DELAY_MILLIS;
                    sb += " msec...";
                    Log(sb.ToString());
                }
                mApp->mNotifier->SendMwiChangedDelayed(VM_NUMBER_RETRY_DELAY_MILLIS);
                return;
            }
            else {
                StringBuilder sb;
                sb += "NotificationMgr.updateMwi: getVoiceMailNumber() failed after ";
                sb += MAX_VM_NUMBER_RETRIES;
                sb += " retries; giving up.";
                Logger::W(LOG_TAG, sb.ToString());
                // ...and continue with vmNumber==null, just as if the
                // phone had no VM number set up in the first place.
            }
        }

        if (TelephonyCapabilities::SupportsVoiceMessageCount(mPhone)) {
            Int32 vmCount;
            mPhone->GetVoiceMessageCount(&vmCount);
            String titleFormat;
            mContext->GetString(R.string.notification_voicemail_title_count, &titleFormat);
            notificationTitle = String.format(titleFormat, vmCount);
        }

        String notificationText;
        if (TextUtils::IsEmpty(vmNumber)) {
            mContext->GetString(
                    R.string.notification_voicemail_no_vm_number, &notificationText);
        }
        else {
            String tmp;
            mContext->GetString(R.string.notification_voicemail_text_format, &tmp);
            notificationText = String.format(tmp, PhoneNumberUtils.formatNumber(vmNumber));
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_VOICEMAIL, String(""), String(NULL), (IUri**)&uri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_CALL, uri, (IIntent**)&intent);

        AutoPtr<IPendingIntent> pendingIntent;
        PendingIntent::GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&pendingIntent);

        AutoPtr<ISharedPreferences> prefs;
        PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&prefs);
        AutoPtr<IUri> ringtoneUri;
        String uriString;
        prefs->GetString(ICallFeaturesSetting::BUTTON_VOICEMAIL_NOTIFICATION_RINGTONE_KEY, NULL, &uriString);
        if (!TextUtils::IsEmpty(uriString)) {
            helper->Parse(uriString, (IUri**)&ringtoneUri);
        }
        else {
            ringtoneUri = Settings.System.DEFAULT_NOTIFICATION_URI;
        }

        AutoPtr<INotificationBuilder> builder;
        CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
        builder->SetSmallIcon(resId)
        builder->SetWhen(System::GetCurrentTimeMillis());
        builder->SetContentTitle(notificationTitle)
        builder->SetContentText(notificationText)
        builder->SetContentIntent(pendingIntent)
        builder->SetSound(ringtoneUri)
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<IColor> color;
        resources->GetColor(R.color.dialer_theme_color, (IColor**)&color);
        builder->SetColor(color)
        builder->SetOngoing(TRUE);

        CallFeaturesSetting::MigrateVoicemailVibrationSettingsIfNeeded(prefs);
        Boolean vibrate;
        prefs->GetBoolean(ICallFeaturesSetting::BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, FALSE, &vibrate);
        if (vibrate) {
            builder->SetDefaults(INotification::DEFAULT_VIBRATE);
        }

        AutoPtr<INotification> notification;
        builder->Build((INotification**)&notification);
        AutoPtr<IList> users;
        mUserManager->GetUsers(TRUE, (IList**)&users);
        Int32 size;
        for (Int32 i = 0; i < (users->GetSize(&size), size); i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            user->GetUserHandle((IUserHandle**)&userHandle);

            Boolean res;
            if (mUserManager->HasUserRestriction(
                    IUserManager::DISALLOW_OUTGOING_CALLS, userHandle, &res), !res) {
                if (user->IsManagedProfile(&res), !res) {
                    mNotificationManager->NotifyAsUser(
                            NULL /* tag */, VOICEMAIL_NOTIFICATION, notification, userHandle);
                }
            }
        }
    }
    else {
        mNotificationManager->CancelAsUser(
                NULL /* tag */, VOICEMAIL_NOTIFICATION, IUserHandle::ALL);
    }
    return NOERROR;
}

ECode NotificationMgr::UpdateCfi(
    /* [in] */ Boolean visible)
{
    if (DBG) {
        StringBuilder sb;
        sb += "updateCfi(): ";
        sb += visible;
        Log(sb.ToString());
    }
    if (visible) {
        // If Unconditional Call Forwarding (forward all calls) for VOICE
        // is enabled, just show a notification.  We'll default to expanded
        // view for now, so the there is less confusion about the icon.  If
        // it is deemed too weird to have CF indications as expanded views,
        // then we'll flip the flag back.

        // TODO: We may want to take a look to see if the notification can
        // display the target to forward calls to.  This will require some
        // effort though, since there are multiple layers of messages that
        // will need to propagate that information.

        AutoPtr<INotificationBuilder> builder;
        CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
        builder->SetSmallIcon(R.drawable.stat_sys_phone_call_forward)
        String title;
        mContext->GetString(R.string.labelCF, &title);
        builder->SetContentTitle(title);
        String text;
        mContext->GetString(R.string.sum_cfu_enabled_indicator, &text);
        builder->SetContentText(&text);
        builder->SetShowWhen(FALSE);
        builder->SetOngoing(TRUE);

        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->SetClassName(String("com.android.phone"), String("com.android.phone.CallFeaturesSetting"));
        AutoPtr<IPendingIntent> contentIntent;
        PendingIntent::GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&contentIntent);

        AutoPtr<IList> users;
        mUserManager->GetUsers(TRUE, (IList**)&users);
        Int32 size;
        for (Int32 i = 0; i < (users->GetSize(&size), size); i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            info->GetUserHandle((IUserHandle**)&userHandle);

            Boolean res;
            builder->SetContentIntent((userHandle->IsOwner(&res), res) ? contentIntent : NULL);
            AutoPtr<INotification> notification;
            builder->Build((INotification**)&notification);
            mNotificationManager->NotifyAsUser(
                    NULL /* tag */, CALL_FORWARD_NOTIFICATION, notification, userHandle);
        }
    }
    else {
        mNotificationManager->CancelAsUser(
                NULL /* tag */, CALL_FORWARD_NOTIFICATION, IUserHandle::ALL);
    }
    return NOERROR;
}

ECode NotificationMgr::ShowDataDisconnectedRoaming()
{
    if (DBG) Log(String("showDataDisconnectedRoaming()..."));

    // "Mobile network settings" screen / dialog
    AutoPtr<IIntent> intent;
    CIntent::New(mContext, com.android.phone.MobileNetworkSettings.class, (IIntent**)&intent);
    AutoPtr<IPendingIntent> contentIntent;
    PendingIntent::GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&contentIntent);

    AutoPtr<ICharSequence> contentText;
    mContext->GetText(R.string.roaming_reenable_message, (ICharSequence**)&contentText);

    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
    builder->SetSmallIcon(android.R.drawable.stat_sys_warning);
    String title;
    mContext->GetText(R.string.roaming, &title);
    builder->SetContentTitle(title);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IColor> color;
    resources->GetColor(R.color.dialer_theme_color, (IColor**)&color);
    builder->SetColor(color);
    builder->SetContentText(contentText);

    AutoPtr<IList> users;
    mUserManager->GetUsers(TRUE, (IList**)&users);
    Int32 size;
    for (Int32 i = 0; i < (users->GetSize(&size), size); i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> userHandle;
        info->GetUserHandle((IUserHandle**)&userHandle);

        Boolean res;
        builder->SetContentIntent((userHandle->IsOwner(&res), res) ? contentIntent : NULL);
        AutoPtr<INotificationBigTextStyle> style;
        CNotificationBigTextStyle::New(builder, (INotificationBigTextStyle**)&style);
        style->BigText(contentText);
        AutoPtr<INotification> notif;
        INotificationStyle::Probe(style)->Build((INotification**)&notif);

        mNotificationManager->NotifyAsUser(
                NULL /* tag */, DATA_DISCONNECTED_ROAMING_NOTIFICATION, notif, userHandle);
    }
    return NOERROR;
}

ECode NotificationMgr::HideDataDisconnectedRoaming()
{
    if (DBG) Log(String("hideDataDisconnectedRoaming()..."));
    return mNotificationManager->Cancel(DATA_DISCONNECTED_ROAMING_NOTIFICATION);
}

void NotificationMgr::ShowNetworkSelection(
    /* [in] */ const String& _operator)
{
    if (DBG) {
        StringBuilder sb;
        sb += "showNetworkSelection(";
        sb += _operator;
        sb += ")...";
        Log(sb.ToString());
    }

    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
    builder->SetSmallIcon(android.R.drawable.stat_sys_warning)
    String title;
    mContext->GetString(R.string.notification_network_selection_title, &title);
    builder->SetContentTitle(title);
    String text;
    mContext->GetString(R.string.notification_network_selection_text, _operator, &text);
    builder->SetContentText(text);
    builder->SetShowWhen(FALSE)
    builder->SetOngoing(TRUE);

    // create the target network operators settings intent
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
            IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    // Use NetworkSetting to handle the selection intent
    AutoPtr<IComponentName> name;
    CComponentName::New(String("com.android.phone"), String("com.android.phone.NetworkSetting"),
            (IComponentName**)&name);
    intent->SetComponent(name);
    AutoPtr<IPendingIntent> contentIntent;
    PendingIntent::GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&contentIntent);

    AutoPtr<IList> users;
    mUserManager->GetUsers(TRUE, (IList**)&users);
    Int32 size;
    for (Int32 i = 0; i < (users->GetSize(&size), size); i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> userHandle;
        info->GetUserHandle((IUserHandle**)&userHandle);

        Boolean res;
        builder->SetContentIntent((userHandle->IsOwner(&res), res) ? contentIntent : NULL);
        AutoPtr<INotification> notification;
        builder->Build((INotification**)&notification);
        mNotificationManager->NotifyAsUser(
                NULL /* tag */,
                SELECTED_OPERATOR_FAIL_NOTIFICATION,
                notification,
                userHandle);
    }
}

void NotificationMgr::CancelNetworkSelection()
{
    if (DBG) Log(String("cancelNetworkSelection()..."));
    mNotificationManager->CancelAsUser(
            BULL /* tag */, SELECTED_OPERATOR_FAIL_NOTIFICATION, IUserHandle::ALL);
}

ECode NotificationMgr::UpdateNetworkSelection(
    /* [in] */ Int32 serviceState)
{
    Boolean res;
    if (TelephonyCapabilities::SupportsNetworkSelection(mPhone, &res), res) {
        // get the shared preference of network_selection.
        // empty is auto mode, otherwise it is the operator alpha name
        // in case there is no operator name, check the operator numeric
        AutoPtr<ISharedPreferences> sp;
        PreferenceManager::GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
        String networkSelection;
        sp->GetString(IPhoneBase::NETWORK_SELECTION_NAME_KEY, String(""), &networkSelection);
        if (TextUtils::IsEmpty(networkSelection)) {
            sp->GetString(PhoneBase.NETWORK_SELECTION_KEY, String(""), &networkSelection);
        }

        if (DBG) {
            StringBuilder sb;
            sb += "updateNetworkSelection()...";
            sb += "state = ";
            sb += serviceState;
            sb += " new network ";
            sb += networkSelection;
            Log(sb.ToString());
        }

        if (serviceState == IServiceState::STATE_OUT_OF_SERVICE
                && !TextUtils::IsEmpty(networkSelection)) {
            if (!mSelectedUnavailableNotify) {
                ShowNetworkSelection(networkSelection);
                mSelectedUnavailableNotify = TRUE;
            }
        }
        else {
            if (mSelectedUnavailableNotify) {
                CancelNetworkSelection();
                mSelectedUnavailableNotify = FALSE;
            }
        }
    }
    return NOERROR;
}

ECode NotificationMgr::PostTransientNotification(
    /* [in] */ Int32 notifyId,
    /* [in] */ ICharSequence* msg)
{
    if (mToast != NULL) {
        mToast->Cancel();
    }

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(mContext, msg, IToast::LENGTH_LONG, (IToast**)&mToast);
    return mToast->Show();
}

void NotificationMgr::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOG_TAG, msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos