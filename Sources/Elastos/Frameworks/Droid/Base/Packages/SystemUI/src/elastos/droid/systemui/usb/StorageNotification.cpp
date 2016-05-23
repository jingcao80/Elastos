#include "elastos/droid/systemui/usb/StorageNotification.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
// using Elastos::Droid::Internal::App::EIID_IExternalMediaFormatActivity;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Storage::EIID_IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
// using Elastos::Droid::SystemUI::Usb::EIID_IUsbStorageActivity;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

//-------------------------------------------------------------------------------------
// StorageNotification::R1
//-------------------------------------------------------------------------------------

StorageNotification::R1::R1(
    /* [in] */ Boolean connected,
    /* [in] */ StorageNotification* host)
    : mConnected(connected)
    , mHost(host)
{}

ECode StorageNotification::R1::Run()
{
    mHost->OnUsbMassStorageConnectionChangedAsync(mConnected);
    return NOERROR;
}


//-------------------------------------------------------------------------------------
// StorageNotification::R2
//-------------------------------------------------------------------------------------

StorageNotification::R2::R2(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState,
    /* [in] */ StorageNotification* host)
    : mPath(path)
    , mOldState(oldState)
    , mNewState(newState)
    , mHost(host)
{}

ECode StorageNotification::R2::Run()
{
    mHost->OnStorageStateChangedAsync(mPath, mOldState, mNewState);
    return NOERROR;
}

//-------------------------------------------------------------------------------------
// StorageNotification::StorageNotificationEventListener
//-------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL(StorageNotification::StorageNotificationEventListener, Object, IStorageEventListener)

StorageNotification::StorageNotificationEventListener::StorageNotificationEventListener(
    /* [in] */ StorageNotification* host)
    : mHost(host)
{}

ECode StorageNotification::StorageNotificationEventListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    AutoPtr<R1> r1= new R1(connected, mHost);
    Boolean result;
    return mHost->mAsyncEventHandler->Post(r1, &result);
}
ECode StorageNotification::StorageNotificationEventListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    AutoPtr<R2> r2= new R2(path, oldState, newState, mHost);
    Boolean result;
    return mHost->mAsyncEventHandler->Post(r2, &result);
}

//-------------------------------------------------------------------------------------
// StorageNotification
//-------------------------------------------------------------------------------------

const String StorageNotification::TAG("StorageNotification");
const Boolean StorageNotification::DEBUG = FALSE;
const Boolean StorageNotification::POP_UMS_ACTIVITY_ON_CONNECT = TRUE;

CAR_INTERFACE_IMPL(StorageNotification, SystemUI, IStorageNotification)

StorageNotification::StorageNotification()
    : mUmsAvailable(FALSE)
{}

ECode StorageNotification::constructor()
{
    return NOERROR;
}

ECode StorageNotification::Start()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&obj);
    mStorageManager = IStorageManager::Probe(obj);
    Boolean connected;
    mStorageManager->IsUsbMassStorageConnected(&connected);
    if (DEBUG) {
        AutoPtr<IEnvironment> en;
        CEnvironment::AcquireSingleton((IEnvironment**)&en);
        String state;
        en->GetExternalStorageState(&state);
        Logger::D(TAG, "Startup with UMS connection %s (media state %s)",
            mUmsAvailable ? "TRUE" : "FALSE", state.string());
    }

    AutoPtr<IHandlerThread> thr;
    CHandlerThread::New(String("SystemUI StorageNotification"), (IHandlerThread**)&thr);
    IThread::Probe(thr)->Start();
    AutoPtr<ILooper> looper;
    thr->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mAsyncEventHandler);

    AutoPtr<StorageNotificationEventListener> listener = new StorageNotificationEventListener(this);
    listener->OnUsbMassStorageConnectionChanged(connected);
    mStorageManager->RegisterListener(listener);
    return NOERROR;
}

void StorageNotification::OnUsbMassStorageConnectionChangedAsync(
    /* [in] */ Boolean connected)
{
    mUmsAvailable = connected;
    /*
     * Even though we may have a UMS host connected, we the SD card
     * may not be in a state for export.
     */
    AutoPtr<IEnvironment> en;
    CEnvironment::AcquireSingleton((IEnvironment**)&en);
    String st;
    en->GetExternalStorageState(&st);

    if (DEBUG) Logger::I(TAG, "UMS connection changed to %s (media state %s)",
        connected ? "TRUE" : "FALSE", st.string());

    if (connected && (st.Equals(
            IEnvironment::MEDIA_REMOVED) || st.Equals(IEnvironment::MEDIA_CHECKING))) {
        /*
         * No card or card being checked = don't display
         */
        connected = FALSE;
    }
    UpdateUsbMassStorageNotification(connected);
}

void StorageNotification::OnStorageStateChangedAsync(
        /* [in] */ const String& path,
        /* [in] */ const String& oldState,
        /* [in] */ const String& newState)
{
    if (DEBUG) Logger::I(TAG, "Media {%s} state changed from {%s} -> {%s}",
        path.string(), oldState.string(), newState.string());
    if (newState.Equals(IEnvironment::MEDIA_SHARED)) {
        /*
         * Storage is now shared. Modify the UMS notification
         * for stopping UMS.
         */
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        // intent->SetClass(mContext, EIID_IUsbStorageActivity);
        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pi;
        pih->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&pi);
        SetUsbStorageNotification(
                Elastos::Droid::R::string::usb_storage_stop_notification_title,
                Elastos::Droid::R::string::usb_storage_stop_notification_message,
                R::drawable::stat_sys_warning, FALSE, TRUE, pi);
    }
    else if (newState.Equals(IEnvironment::MEDIA_CHECKING)) {
        /*
         * Storage is now checking. Update media notification and disable
         * UMS notification.
         */
        SetMediaStorageNotification(
            Elastos::Droid::R::string::ext_media_checking_notification_title,
            Elastos::Droid::R::string::ext_media_checking_notification_message,
            Elastos::Droid::R::drawable::stat_notify_sdcard_prepare, TRUE, FALSE, NULL);
        UpdateUsbMassStorageNotification(FALSE);
    }
    else if (newState.Equals(IEnvironment::MEDIA_MOUNTED)) {
        /*
         * Storage is now mounted. Dismiss any media notifications,
         * and enable UMS notification if connected.
         */
        SetMediaStorageNotification(0, 0, 0, FALSE, FALSE, NULL);
        UpdateUsbMassStorageNotification(mUmsAvailable);
    }
    else if (newState.Equals(IEnvironment::MEDIA_UNMOUNTED)) {
        /*
         * Storage is now unmounted. We may have been unmounted
         * because the user is enabling/disabling UMS, in which case we don't
         * want to display the 'safe to unmount' notification.
         */
         Boolean isUsbMassStorageEnabled;
         mStorageManager->IsUsbMassStorageEnabled(&isUsbMassStorageEnabled);
        if (!isUsbMassStorageEnabled) {
            if (oldState.Equals(IEnvironment::MEDIA_SHARED)) {
                /*
                 * The unmount was due to UMS being enabled. Dismiss any
                 * media notifications, and enable UMS notification if connected
                 */
                SetMediaStorageNotification(0, 0, 0, FALSE, FALSE, NULL);
                UpdateUsbMassStorageNotification(mUmsAvailable);
            }
            else {
                /*
                 * Show safe to unmount media notification, and enable UMS
                 * notification if connected.
                 */
                AutoPtr<IEnvironment> en;
                CEnvironment::AcquireSingleton((IEnvironment**)&en);
                Boolean isExternalStorageRemovable;
                en->IsExternalStorageRemovable(&isExternalStorageRemovable);
                if (isExternalStorageRemovable) {
                    SetMediaStorageNotification(
                        Elastos::Droid::R::string::ext_media_safe_unmount_notification_title,
                        Elastos::Droid::R::string::ext_media_safe_unmount_notification_message,
                        Elastos::Droid::R::drawable::stat_notify_sdcard, TRUE, TRUE, NULL);
                }
                else {
                    // This device does not have removable storage, so
                    // don't tell the user they can remove it.
                    SetMediaStorageNotification(0, 0, 0, FALSE, FALSE, NULL);
                }
                UpdateUsbMassStorageNotification(mUmsAvailable);
            }
        }
        else {
            /*
             * The unmount was due to UMS being enabled. Dismiss any
             * media notifications, and disable the UMS notification
             */
            SetMediaStorageNotification(0, 0, 0, FALSE, FALSE, NULL);
            UpdateUsbMassStorageNotification(FALSE);
        }
    }
    else if (newState.Equals(IEnvironment::MEDIA_NOFS)) {
        /*
         * Storage has no filesystem. Show blank media notification,
         * and enable UMS notification if connected.
         */
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        // intent->SetClass(mContext, EIID_IExternalMediaFormatActivity);
        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pi;
        pih->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&pi);

        SetMediaStorageNotification(
            Elastos::Droid::R::string::ext_media_nofs_notification_title,
            Elastos::Droid::R::string::ext_media_nofs_notification_message,
            Elastos::Droid::R::drawable::stat_notify_sdcard_usb, TRUE, FALSE, pi);
        UpdateUsbMassStorageNotification(mUmsAvailable);
    }
    else if (newState.Equals(IEnvironment::MEDIA_UNMOUNTABLE)) {
        /*
         * Storage is corrupt. Show corrupt media notification,
         * and enable UMS notification if connected.
         */
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        // intent->SetClass(mContext, EIID_IExternalMediaFormatActivity);
        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pi;
        pih->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&pi);

        SetMediaStorageNotification(
            Elastos::Droid::R::string::ext_media_unmountable_notification_title,
            Elastos::Droid::R::string::ext_media_unmountable_notification_message,
            Elastos::Droid::R::drawable::stat_notify_sdcard_usb, TRUE, FALSE, pi);
        UpdateUsbMassStorageNotification(mUmsAvailable);
    }
    else if (newState.Equals(IEnvironment::MEDIA_REMOVED)) {
        /*
         * Storage has been removed. Show nomedia media notification,
         * and disable UMS notification regardless of connection state.
         */
        SetMediaStorageNotification(
            Elastos::Droid::R::string::ext_media_nomedia_notification_title,
            Elastos::Droid::R::string::ext_media_nomedia_notification_message,
            Elastos::Droid::R::drawable::stat_notify_sdcard_usb, TRUE, FALSE, NULL);
        UpdateUsbMassStorageNotification(FALSE);
    }
    else if (newState.Equals(IEnvironment::MEDIA_BAD_REMOVAL)) {
        /*
         * Storage has been removed unsafely. Show bad removal media notification,
         * and disable UMS notification regardless of connection state.
         */
        SetMediaStorageNotification(
            Elastos::Droid::R::string::ext_media_badremoval_notification_title,
            Elastos::Droid::R::string::ext_media_badremoval_notification_message,
            R::drawable::stat_sys_warning, TRUE, TRUE, NULL);
        UpdateUsbMassStorageNotification(FALSE);
    }
    else {
        Logger::W(TAG, "Ignoring unknown state {%s}", newState.string());
    }
}

ECode StorageNotification::StorageNotification::UpdateUsbMassStorageNotification(
    /* [in] */ Boolean available)
{
    if (available) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        // intent->SetClass(mContext, EIID_IUsbStorageActivity);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pi;
        pih->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&pi);

        SetUsbStorageNotification(
            Elastos::Droid::R::string::usb_storage_notification_title,
            Elastos::Droid::R::string::usb_storage_notification_message,
            Elastos::Droid::R::drawable::stat_sys_data_usb, FALSE, TRUE, pi);
    }
    else {
        SetUsbStorageNotification(0, 0, 0, FALSE, FALSE, NULL);
    }
    return NOERROR;
}

void StorageNotification::SetUsbStorageNotification(
    /* [in] */ Int32 titleId,
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 icon,
    /* [in] */ Boolean sound,
    /* [in] */ Boolean visible,
    /* [in] */ IPendingIntent* pi)
{
    {    AutoLock syncLock(this);
        if (!visible && mUsbStorageNotification == NULL) {
            return;
        }

        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(obj);

        if (notificationManager == NULL) {
            return;
        }

        if (visible) {
            AutoPtr<IResourcesHelper> rh;
            CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rh);
            AutoPtr<IResources> r;
            rh->GetSystem((IResources**)&r);
            AutoPtr<ICharSequence> title, message;
            r->GetText(titleId, (ICharSequence**)&title);
            r->GetText(messageId, (ICharSequence**)&message);

            if (mUsbStorageNotification == NULL) {
                CNotification::New((INotification**)&mUsbStorageNotification);
                mUsbStorageNotification->SetIcon(icon);
                mUsbStorageNotification->SetWhen(0);
            }

            if (sound) {
                Int32 defaults;
                mUsbStorageNotification->GetDefaults(&defaults);
                mUsbStorageNotification->SetDefaults(defaults |= INotification::DEFAULT_SOUND);
            }
            else {
                Int32 defaults;
                mUsbStorageNotification->GetDefaults(&defaults);
                mUsbStorageNotification->SetDefaults(defaults &= ~INotification::DEFAULT_SOUND);
            }

            mUsbStorageNotification->SetFlags(INotification::FLAG_ONGOING_EVENT);

            mUsbStorageNotification->SetTickerText(title);
            if (pi == NULL) {
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                AutoPtr<IPendingIntentHelper> pih;
                CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
                AutoPtr<IUserHandleHelper> uhh;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
                AutoPtr<IUserHandle> uh;
                uhh->GetCURRENT((IUserHandle**)&uh);
                AutoPtr<IPendingIntent> pi;
                pih->GetBroadcastAsUser(mContext, 0, intent, 0, uh, (IPendingIntent**)&pi);
            }
            AutoPtr<IResources> r2;
            mContext->GetResources((IResources**)&r2);
            Int32 color;
            r2->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &color);
            mUsbStorageNotification->SetColor(color);
            mUsbStorageNotification->SetLatestEventInfo(mContext, title, message, pi);
            mUsbStorageNotification->SetVisibility(INotification::VISIBILITY_PUBLIC);
            mUsbStorageNotification->SetCategory(INotification::CATEGORY_SYSTEM);

            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            AutoPtr<ISettingsGlobal> sg;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
            Int32 v;
            sg->GetInt32(resolver, ISettingsGlobal::ADB_ENABLED, 0, &v);
            const Boolean adbOn = 1 == v;

            if (POP_UMS_ACTIVITY_ON_CONNECT && !adbOn) {
                // Pop up a full-screen alert to coach the user through enabling UMS. The average
                // user has attached the device to USB either to charge the phone (in which case
                // this is harmless) or transfer files, and in the latter case this alert saves
                // several steps (as well as subtly indicates that you shouldn't mix UMS with other
                // activities on the device).
                //
                // If ADB is enabled, however, we suppress this dialog (under the assumption that a
                // developer (a) knows how to enable UMS, and (b) is probably using USB to install
                // builds or use adb commands.
                mUsbStorageNotification->SetFullScreenIntent(pi);
            }
        }

        Int32 icon;
        mUsbStorageNotification->GetIcon(&icon);
        const Int32 notificationId = icon;
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        AutoPtr<IUserHandle> uh;
        uhh->GetALL((IUserHandle**)&uh);
        if (visible) {
            notificationManager->NotifyAsUser(String(NULL), notificationId, mUsbStorageNotification, uh);
        }
        else {
            notificationManager->CancelAsUser(String(NULL), notificationId, uh);
        }
    }
}

Boolean StorageNotification::GetMediaStorageNotificationDismissable()
{
    {    AutoLock syncLock(this);
        Int32 flags;
        mMediaStorageNotification->GetFlags(&flags);

        if ((mMediaStorageNotification != NULL) &&
            ((flags & INotification::FLAG_AUTO_CANCEL) == INotification::FLAG_AUTO_CANCEL))
            return TRUE;
    }
    return FALSE;
}

void StorageNotification::SetMediaStorageNotification(
    /* [in] */ Int32 titleId,
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 icon,
    /* [in] */ Boolean visible,
    /* [in] */ Boolean dismissable,
    /* [in] */ IPendingIntent* pi)
{
    {    AutoLock syncLock(this);
        if (!visible && mMediaStorageNotification == NULL) {
            return;
        }

        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(obj);

        if (notificationManager == NULL) {
            return;
        }

        if (mMediaStorageNotification != NULL && visible) {
            /*
             * Dismiss the previous notification - we're about to
             * re-use it.
             */
            Int32 icon;
            mMediaStorageNotification->GetIcon(&icon);
            const Int32 notificationId = icon;
            notificationManager->Cancel(notificationId);
        }

        if (visible) {
            AutoPtr<IResourcesHelper> rh;
            CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rh);
            AutoPtr<IResources> r;
            rh->GetSystem((IResources**)&r);
            AutoPtr<ICharSequence> title, message;
            r->GetText(titleId, (ICharSequence**)&title);
            r->GetText(messageId, (ICharSequence**)&message);

            if (mMediaStorageNotification == NULL) {
                CNotification::New((INotification**)&mMediaStorageNotification);
                mMediaStorageNotification->SetWhen(0);
            }

            Int32 defaults;
            mMediaStorageNotification->GetDefaults(&defaults);
            mMediaStorageNotification->SetDefaults(defaults |= INotification::DEFAULT_SOUND);

            if (dismissable) {
                mMediaStorageNotification->SetFlags(INotification::FLAG_AUTO_CANCEL);
            }
            else {
                mMediaStorageNotification->SetFlags(INotification::FLAG_ONGOING_EVENT);
            }

            mMediaStorageNotification->SetTickerText(title);

            if (pi == NULL) {
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                AutoPtr<IPendingIntentHelper> pih;
                CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
                AutoPtr<IUserHandleHelper> uhh;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
                AutoPtr<IUserHandle> uh;
                uhh->GetCURRENT((IUserHandle**)&uh);
                AutoPtr<IPendingIntent> pi;
                pih->GetBroadcastAsUser(mContext, 0, intent, 0, uh, (IPendingIntent**)&pi);
            }

            mMediaStorageNotification->SetIcon(icon);
            AutoPtr<IResources> r3;
            mContext->GetResources((IResources**)&r3);
            Int32 color;
            r3->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &color);
            mMediaStorageNotification->SetColor(color);
            mMediaStorageNotification->SetLatestEventInfo(mContext, title, message, pi);
            mMediaStorageNotification->SetVisibility(INotification::VISIBILITY_PUBLIC);
            mMediaStorageNotification->SetCategory(INotification::CATEGORY_SYSTEM);
        }

        Int32 icon;
        mMediaStorageNotification->GetIcon(&icon);
        const Int32 notificationId = icon;
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        AutoPtr<IUserHandle> uh;
        uhh->GetALL((IUserHandle**)&uh);
        if (visible) {
            notificationManager->NotifyAsUser(String(NULL), notificationId, mMediaStorageNotification, uh);
        }
        else {
            notificationManager->CancelAsUser(String(NULL), notificationId, uh);
        }
    }
}

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos