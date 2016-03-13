
#include "elastos/droid/internal/location/CGpsNiNotification.h"
#include "elastos/droid/internal/location/CGpsNiResponse.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/R.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/StringBuilder.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::EIID_INotificationManager;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Location {

CAR_OBJECT_IMPL(CGpsNiNotification)

CAR_INTERFACE_IMPL(CGpsNetInitiatedHandler::GpsNiNotification, Object, IGpsNiNotification)

CGpsNetInitiatedHandler::GpsNiNotification::GpsNiNotification()
    : mNotificationId(0)
    , mNiType(0)
    , mNeedNotify(FALSE)
    , mNeedVerify(FALSE)
    , mPrivacyOverride(FALSE)
    , mTimeout(0)
    , mDefaultResponse(0)
    , mRequestorId(NULL)
    , mText(NULL)
    , mRequestorIdEncoding(0)
    , mTextEncoding(0)
{}

ECode CGpsNetInitiatedHandler::GpsNiNotification::constructor()
{
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetNotificationId(
    /* [out] */ Int32* notificationId)
{
    VALIDATE_NOT_NULL(notificationId);
    *notificationId = mNotificationId;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetNotificationId(
    /* [in] */ Int32 notificationId)
{
    mNotificationId = notificationId;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetNiType(
    /* [out] */ Int32* niType)
{
    VALIDATE_NOT_NULL(niType);
    *niType = mNiType;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetNiType(
    /* [in] */ Int32 niType)
{
    mNiType = niType;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetNeedNotify(
    /* [out] */ Boolean* needNotify)
{
    VALIDATE_NOT_NULL(needNotify);
    *needNotify = mNeedNotify;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetNeedNotify(
    /* [in] */ Boolean needNotify)
{
    mNeedNotify = needNotify;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetNeedVerify(
    /* [out] */ Boolean* needVerify)
{
    VALIDATE_NOT_NULL(needVerify);
    *needVerify = mNeedVerify;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetNeedVerify(
    /* [in] */ Boolean needVerify)
{
    mNeedVerify = needVerify;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetPrivacyOverride(
    /* [out] */ Boolean* privacyOverride)
{
    VALIDATE_NOT_NULL(privacyOverride);
    *privacyOverride = mPrivacyOverride;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetPrivacyOverride(
    /* [in] */ Boolean privacyOverride)
{
    mPrivacyOverride = privacyOverride;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetTimeOut(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);
    *timeout = mTimeout;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetTimeOut(
    /* [in] */ Int32 timeout)
{
    mTimeout = timeout;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetDefaultResponse(
    /* [out] */ Int32* defaultResponse)
{
    VALIDATE_NOT_NULL(defaultResponse);
    *defaultResponse = mDefaultResponse;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetDefaultResponse(
    /* [in] */ Int32 defaultResponse)
{
    mDefaultResponse = defaultResponse;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetRequestorId(
    /* [out] */ String* requestorId)
{
    VALIDATE_NOT_NULL(requestorId);
    *requestorId = mRequestorId;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetRequestorId(
    /* [in] */ const String& requestorId)
{
    mRequestorId = requestorId;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetText(
    /* [in] */ const String& text)
{
    mText = text;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetRequestorIdEncoding(
    /* [out] */ Int32* requestorIdEncoding)
{
    VALIDATE_NOT_NULL(requestorIdEncoding);
    *requestorIdEncoding = mRequestorIdEncoding;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetRequestorIdEncoding(
    /* [in] */ Int32 requestorIdEncoding)
{
    mRequestorIdEncoding = requestorIdEncoding;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetTextEncoding(
    /* [out] */ Int32* textEncoding)
{
    VALIDATE_NOT_NULL(textEncoding);
    *textEncoding = mTextEncoding;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetTextEncoding(
    /* [in] */ Int32 textEncoding)
{
    mTextEncoding = textEncoding;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras;
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiNotification::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

CAR_OBJECT_IMPL(CGpsNiResponse)

CAR_INTERFACE_IMPL(CGpsNetInitiatedHandler::GpsNiResponse, Object, IGpsNiResponse)

CGpsNetInitiatedHandler::GpsNiResponse::GpsNiResponse()
    : mUserResponse(0)
{}

ECode CGpsNetInitiatedHandler::GpsNiResponse::constructor()
{
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiResponse::GetUserResponse(
    /* [out] */ Int32* userResponse)
{
    VALIDATE_NOT_NULL(userResponse);
    *userResponse = mUserResponse;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiResponse::SetUserResponse(
    /* [in] */ Int32 userResponse)
{
    mUserResponse = userResponse;
    return NOERROR;
}

/* Optional extra data to pass with the user response */
ECode CGpsNetInitiatedHandler::GpsNiResponse::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras;
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GpsNiResponse::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

CGpsNetInitiatedHandler::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CGpsNetInitiatedHandler* host)
    : mHost(host)
{}

ECode CGpsNetInitiatedHandler::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_NEW_OUTGOING_CALL)) {
        String phoneNumber;
        intent->GetStringExtra(IIntent::EXTRA_PHONE_NUMBER, &phoneNumber);
        /*
           Emergency Mode is when during emergency call or in emergency call back mode.
           For checking if it is during emergency call:
               mIsInEmergency records if the phone is in emergency call or not. It will
               be set to true when the phone is having emergency call, and then will
               be set to false by mPhoneStateListener when the emergency call ends.
           For checking if it is in emergency call back mode:
               Emergency call back mode will be checked by reading system properties
               when necessary: SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE)
        */
        Boolean isEmergencyNumber;
        PhoneNumberUtils::IsEmergencyNumber(phoneNumber, &isEmergencyNumber);
        mHost->SetInEmergency(isEmergencyNumber);
        if (DEBUG) {
            Boolean res;
            mHost->GetInEmergency(&res);
            Logger::V(TAG, "ACTION_NEW_OUTGOING_CALL - %d", res);
        }
    }
    else if (action.Equals(ILocationManager::MODE_CHANGED_ACTION)) {
        mHost->UpdateLocationMode();
        if (DEBUG) {
            Boolean res;
            mHost->GetLocationEnabled(&res);
            Logger::D(TAG, "location enabled :%d", res);
        }
    }
    return NOERROR;
}

CGpsNetInitiatedHandler::MyPhoneStateListener::MyPhoneStateListener(
    /* [in] */ CGpsNetInitiatedHandler* host)
    : mHost(host)
{}

ECode CGpsNetInitiatedHandler::MyPhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (DEBUG) Logger::D(CGpsNetInitiatedHandler::TAG, "onCallStateChanged(): state is %d", state);
    // listening for emergency call ends
    if (state == ITelephonyManager::CALL_STATE_IDLE) {
        mHost->SetInEmergency(FALSE);
    }
    return NOERROR;
}

const String CGpsNetInitiatedHandler::TAG("CGpsNetInitiatedHandler");
const String CGpsNetInitiatedHandler::sLanguage(
        /* 3GPP TS 23.038 V9.1.1 section 6.2.1 - GSM 7 bit Default Alphabet
          01.....23.....4.....5.....6.....7.....8.....9.....A.B.....C.....D.E.....F.....0.....1 */
         "@\u00a3$\u00a5\u00e8\u00e9\u00f9\u00ec\u00f2\u00c7\n\u00d8\u00f8\r\u00c5\u00e5\u0394_"
             // 2.....3.....4.....5.....6.....7.....8.....9.....A.....B.....C.....D.....E.....
             "\u03a6\u0393\u039b\u03a9\u03a0\u03a8\u03a3\u0398\u039e\uffff\u00c6\u00e6\u00df"
             // F.....012.34.....56789ABCDEF0123456789ABCDEF0.....123456789ABCDEF0123456789A
             "\u00c9 !\"#\u00a4%&'()*+,-./0123456789:;<=>?\u00a1ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             // B.....C.....D.....E.....F.....0.....123456789ABCDEF0123456789AB.....C.....D.....
             "\u00c4\u00d6\u00d1\u00dc\u00a7\u00bfabcdefghijklmnopqrstuvwxyz\u00e4\u00f6\u00f1"
             // E.....F.....
             "\u00fc\u00e0");

const String CGpsNetInitiatedHandler::sShift(
        /* 6.2.1.1 GSM 7 bit Default Alphabet Extension Table
                 0123456789A.....BCDEF0123456789ABCDEF0123456789ABCDEF.0123456789ABCDEF0123456789ABCDEF */
                "          \u000c         ^                   {}     \\            [~] |               "
                    // 0123456789ABCDEF012345.....6789ABCDEF0123456789ABCDEF
                    "                     \u20ac                          ");


const Boolean CGpsNetInitiatedHandler::DEBUG = TRUE;
const Boolean CGpsNetInitiatedHandler::VERBOSE = FALSE;
const Byte CGpsNetInitiatedHandler::GSM_EXTENDED_ESCAPE = 0x1B;

// Set to true if string from HAL is encoded as Hex, e.g., "3F0039"
Boolean CGpsNetInitiatedHandler::mIsHexInput = TRUE;

CAR_OBJECT_IMPL(CGpsNetInitiatedHandler);

CAR_INTERFACE_IMPL(CGpsNetInitiatedHandler, Object, IGpsNetInitiatedHandler);

CGpsNetInitiatedHandler::CGpsNetInitiatedHandler()
    : mPlaySounds(FALSE)
    , mPopupImmediately(TRUE)
    , mIsSuplEsEnabled(FALSE)
    , mIsInEmergency(FALSE)
    , mIsLocationEnabled(FALSE)
{
    mBroadcastReciever = new MyBroadcastReceiver(this);
}

ECode CGpsNetInitiatedHandler::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetInitiatedListener* netInitiatedListener,
    /* [in] */ Boolean isSuplEsEnabled)
{
    if (netInitiatedListener == NULL) {
        Logger::E(TAG, "netInitiatedListener is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        mNetInitiatedListener = netInitiatedListener;
    }

    SetSuplEsEnabled(isSuplEsEnabled);
    mContext = context;
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&service);
    mLocationManager = ILocationManager::Probe(service);
    UpdateLocationMode();
    service = NULL;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    mTelephonyManager = ITelephonyManager::Probe(service);

    mPhoneStateListener = new MyPhoneStateListener(this);
    mTelephonyManager->Listen(mPhoneStateListener, PhoneStateListener::LISTEN_CALL_STATE);

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_NEW_OUTGOING_CALL);
    intentFilter->AddAction(ILocationManager::MODE_CHANGED_ACTION);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReciever, intentFilter, (IIntent**)&intent);
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::SetSuplEsEnabled(
    /* [in] */ Boolean isEnabled)
{
    mIsSuplEsEnabled = isEnabled;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GetSuplEsEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mIsSuplEsEnabled;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::UpdateLocationMode()
{
    Boolean enabled;
    mLocationManager->IsProviderEnabled(
        ILocationManager::GPS_PROVIDER, &enabled);
    mIsLocationEnabled = enabled;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GetLocationEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mIsLocationEnabled;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::SetInEmergency(
    /* [in] */ Boolean isInEmergency)
{
    mIsInEmergency = isInEmergency;
    return NOERROR;
}

ECode CGpsNetInitiatedHandler::GetInEmergency(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    String value;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, &value);
    Boolean isInEmergencyCallback = StringUtils::ParseBoolean(value);
    *res = mIsInEmergency || isInEmergencyCallback;
    return NOERROR;
}

// Handles NI events from HAL
ECode CGpsNetInitiatedHandler::HandleNiNotification(
    /* [in] */ IGpsNiNotification* _notif)
{
    GpsNiNotification* notif = (GpsNiNotification*)_notif;
    if (DEBUG) {
        Boolean seEnabled, lEnabled;
        GetSuplEsEnabled(&seEnabled);
        GetLocationEnabled(&lEnabled);
        Logger::D(TAG, "in handleNiNotification () : notificationId: %d"
            " requestorId: %s text: %s mIsSuplEsEnabled %d mIsLocationEnabled%d",
            notif->mNotificationId, notif->mRequestorId.string(), notif->mText.string(),
            seEnabled, lEnabled);
    }

    Boolean res;
    GetSuplEsEnabled(&res);
    if (res) {
        HandleNiInEs(notif);
    }
    else {
        HandleNi(notif);
    }

    //////////////////////////////////////////////////////////////////////////
    //   A note about timeout
    //   According to the protocol, in the need_notify and need_verify case,
    //   a default response should be sent when time out.
    //
    //   In some GPS hardware, the GPS driver (under HAL) can handle the timeout case
    //   and this class CGpsNetInitiatedHandler does not need to do anything.
    //
    //   However, the UI should at least close the dialog when timeout. Further,
    //   for more general handling, timeout response should be added to the Handler here.

    return NOERROR;
}

// handle NI form HAL when SUPL_ES is disabled.
void CGpsNetInitiatedHandler::HandleNi(
    /* [in] */ GpsNiNotification* notif)
{
    Boolean inEmergency;
    GetInEmergency(&inEmergency);
    if (DEBUG) {
        Logger::D(TAG, "in handleNi () : needNotify: %d needVerify: %d"
            " privacyOverride: %d mPopupImmediately: %d mInEmergency: %d",
            notif->mNeedNotify, notif->mNeedVerify, notif->mPrivacyOverride,
            mPopupImmediately, inEmergency);
    }

    Boolean locationEnabled;
    GetLocationEnabled(&locationEnabled);
    if (!locationEnabled && !inEmergency) {
        // Location is currently disabled, ignore all NI requests.
        Boolean res;
        if (FAILED(mNetInitiatedListener->SendNiResponse(
            notif->mNotificationId, GPS_NI_RESPONSE_IGNORE, &res))) {
            Logger::E(TAG, "RemoteException in sendNiResponse");
        }
    }
    if (notif->mNeedNotify) {
    // If NI does not need verify or the dialog is not requested
    // to pop up immediately, the dialog box will not pop up.
        if (notif->mNeedVerify && mPopupImmediately) {
            // Popup the dialog box now
            OpenNiDialog(notif);
        }
        else {
            // Show the notification
            SetNiNotification(notif);
        }
    }
    // ACCEPT cases: 1. Notify, no verify; 2. no notify, no verify;
    // 3. privacy override.
    if (!notif->mNeedVerify || notif->mPrivacyOverride) {
        Boolean res;
        if (FAILED(mNetInitiatedListener->SendNiResponse(
            notif->mNotificationId, GPS_NI_RESPONSE_ACCEPT, &res))) {
            Logger::E(TAG, "RemoteException in sendNiResponse");
        }
    }
}

// handle NI from HAL when the SUPL_ES is enabled
void CGpsNetInitiatedHandler::HandleNiInEs(
    /* [in] */ GpsNiNotification* notif)
{
    if (DEBUG) {
        Logger::D(TAG, "in handleNiInEs () : niType: %d notificationId: %d",
            notif->mNiType, notif->mNotificationId);
    }

    // UE is in emergency mode when in emergency call mode or in emergency call back mode
    /*
       1. When SUPL ES bit is off and UE is not in emergency mode:
              Call handleNi() to do legacy behaviour.
       2. When SUPL ES bit is on and UE is in emergency mode:
              Call handleNi() to do acceptance behaviour.
       3. When SUPL ES bit is off but UE is in emergency mode:
              Ignore the emergency SUPL INIT.
       4. When SUPL ES bit is on but UE is not in emergency mode:
              Ignore the emergency SUPL INIT.
    */
    Boolean isNiTypeES = (notif->mNiType == GPS_NI_TYPE_EMERGENCY_SUPL);
    Boolean inEmergency;
    GetInEmergency(&inEmergency);
    if (isNiTypeES != inEmergency) {
        Boolean res;
        if (FAILED(mNetInitiatedListener->SendNiResponse(
            notif->mNotificationId, GPS_NI_RESPONSE_IGNORE, &res))) {
            Logger::E(TAG, "RemoteException in sendNiResponse");
        }
    }
    else {
        HandleNi(notif);
    }
}

// Sets the NI notification.
/*synchronized*/
void CGpsNetInitiatedHandler::SetNiNotification(
    /* [in] */ IGpsNiNotification* notif)
{
    AutoLock lock(this);

    assert(notif != NULL);

    AutoPtr<IInterface> svTemp;
    AutoPtr<INotificationManager> notificationManager;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = (INotificationManager*)svTemp->Probe(EIID_INotificationManager);
    if (notificationManager == NULL) {
        return;
    }

    String title = GetNotifTitle(notif, mContext);
    String message = GetNotifMessage(notif, mContext);

//    if (DEBUG) Log.d(TAG, "setNiNotification, notifyId: " + notif->mNotificationId +
//            ", title: " + title +
//            ", message: " + message);

    // Construct Notification
    if (mNiNotification == NULL) {
        CNotification::New((INotification**)&mNiNotification);
        mNiNotification->SetIcon(R::drawable::stat_sys_gps_on);
        mNiNotification->SetWhen(0);
    }

    Int32 defaults;
    if (mPlaySounds) {
        mNiNotification->GetDefaults(&defaults);
        defaults |= INotification::DEFAULT_SOUND;
        mNiNotification->SetDefaults(defaults);
    }
    else {
        mNiNotification->GetDefaults(&defaults);
        defaults &= ~INotification::DEFAULT_SOUND;
        mNiNotification->SetDefaults(defaults);
    }

    mNiNotification->SetFlags(INotification::FLAG_ONGOING_EVENT | INotification::FLAG_AUTO_CANCEL);
    String tickerTemp = GetNotifTicker(notif, mContext);
    AutoPtr<ICharSequence> ticker;
    CString::New(tickerTemp, (ICharSequence**)&ticker);
    mNiNotification->SetTickerText(ticker);

    // if not to popup dialog immediately, pending intent will open the dialog
    AutoPtr<IIntent> intent;
    if (!mPopupImmediately) {
      intent = GetDlgIntent(notif);
    }
    else {
      CIntent::New((IIntent**)&intent);
    }

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pi;
    helper->GetBroadcast(mContext, 0, intent, 0, (IPendingIntent**)&pi);
    AutoPtr<ICharSequence> titleTemp, messageTemp;
    CString::New(title, (ICharSequence**)&titleTemp);
    CString::New(message, (ICharSequence**)&messageTemp);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(R::color::system_notification_accent_color, &color);
    mNiNotification->SetColor(color);
    mNiNotification->SetLatestEventInfo(mContext, titleTemp, messageTemp, pi);

    AutoPtr<IUserHandle> ALL;
    AutoPtr<IUserHandleHelper> userHandleHelper;
    userHandleHelper->GetALL((IUserHandle**)&ALL);
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
    Int32 notificationId;
    notif->GetNotificationId(&notificationId);
    notificationManager->NotifyAsUser(String(""), notificationId, mNiNotification,
            ALL);
}

// Opens the notification dialog and waits for user input
void CGpsNetInitiatedHandler::OpenNiDialog(
    /* [in] */ IGpsNiNotification* notif)
{
  assert(notif != NULL);

  AutoPtr<IIntent> intent = GetDlgIntent(notif);

//    if (DEBUG) Log.d(TAG, "openNiDialog, notifyId: " + notif.notificationId +
//            ", requestorId: " + notif.requestorId +
//            ", text: " + notif.text);

    mContext->StartActivity(intent);
}

// Construct the intent for bringing up the dialog activity, which shows the
// notification and takes user input
AutoPtr<IIntent> CGpsNetInitiatedHandler::GetDlgIntent(
    /* [in] */ IGpsNiNotification* notif)
{
    assert(notif != NULL);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    String title = GetDialogTitle(notif, mContext);
    String message = GetDialogMessage(notif, mContext);

    // directly bring up the NI activity
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK);
    intent->SetClassName(mContext, String("Elastos.App.NetInitiatedActivity"));

    // put data in the intent
    Int32 notificationId, timeout, defaultResponse;
    notif->GetNotificationId(&notificationId);
    notif->GetTimeOut(&timeout);
    notif->GetDefaultResponse(&defaultResponse);
    intent->PutExtra(NI_INTENT_KEY_NOTIF_ID, notificationId);
    intent->PutExtra(NI_INTENT_KEY_TITLE, title);
    intent->PutExtra(NI_INTENT_KEY_MESSAGE, message);
    intent->PutExtra(NI_INTENT_KEY_TIMEOUT, timeout);
    intent->PutExtra(NI_INTENT_KEY_DEFAULT_RESPONSE, defaultResponse);

//    if (DEBUG) Log.d(TAG, "generateIntent, title: " + title + ", message: " + message +
//            ", timeout: " + notif.timeout);

    return intent;
}

// Converts a string (or Hex string) to a char array
AutoPtr<ArrayOf<Byte> > CGpsNetInitiatedHandler::StringToByteArray(
    /* [in] */ const String& original,
    /* [in] */ Boolean isHex)
{
    Int32 length = isHex ? original.GetByteLength() / 2 : original.GetByteLength();
    AutoPtr<ArrayOf<Byte> > output = ArrayOf<Byte>::Alloc(length);
    Int32 i;

    if (isHex) {
        for (i = 0; i < length; i++) {
            String substr = original.Substring(i*2, i*2+2);
            Int32 integer = StringUtils::ParseInt32(substr, 16);
            (*output)[i] = (Byte)integer;
        }
    }
    else {
        const char* p = original.string();
        for (i = 0; i < length; i++) {
            (*output)[i] = *(p + i);
        }
    }

    return output;
}

/*
 * In Android Codes, this function was defined in class GsmAlphabet
 * but which module this class located is telephony was useless in Elastos
 * So moved the function to the place where needed
 * */
String CGpsNetInitiatedHandler::GsmAlphabet_Gsm7BitPackedToString(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 lengthSeptets,
    /* [in] */ Int32 numPaddingBits)
{
    StringBuilder ret(lengthSeptets);
    Boolean prevCharWasEscape = FALSE;
    for (Int32 i = 0 ; i < lengthSeptets ; i++) {
        Int32 bitOffset = (7 * i) + numPaddingBits;

        Int32 byteOffset = bitOffset / 8;
        Int32 shift = bitOffset % 8;
        Int32 gsmVal;

        gsmVal = (0x7f & ((*pdu)[offset + byteOffset] >> shift));

        // if it crosses a byte boundary
        if (shift > 1) {
            // set msb bits to 0
            gsmVal &= 0x7f >> (shift - 1);

            gsmVal |= 0x7f & ((*pdu)[offset + byteOffset + 1] << (8 - shift));
        }

        if (prevCharWasEscape) {
            if (gsmVal == GSM_EXTENDED_ESCAPE) {
                ret += ' ';    // display ' ' for reserved double escape sequence
            }
            else {
                Char32 c = sShift.GetChar(gsmVal);
                if (c == ' ') {
                    ret += sLanguage.GetChar(gsmVal);
                }
                else {
                    ret += c;
                }
            }
            prevCharWasEscape = FALSE;
        }
        else if (gsmVal == GSM_EXTENDED_ESCAPE) {
            prevCharWasEscape = TRUE;
        }
        else {
            ret += sLanguage.GetChar(gsmVal);
        }
    }

    String strResult;
    ret.ToString(&strResult);
    return strResult;
}

/**
 * Unpacks an byte array containing 7-bit packed characters into a String.
 *
 * @param input a 7-bit packed char array
 * @return the unpacked String
 */
String CGpsNetInitiatedHandler::DecodeGSMPackedString(
    /* [in] */ ArrayOf<Byte>* input)
{
    Char16 PADDING_CHAR = 0x00;
    Int32 lengthBytes = input->GetLength();
    Int32 lengthSeptets = (lengthBytes * 8) / 7;
    String decoded("");

    /* Special case where the last 7 bits in the last byte could hold a valid
     * 7-bit character or a padding character. Drop the last 7-bit character
     * if it is a padding character.
     */
    if (lengthBytes % 7 == 0) {
        if (lengthBytes > 0) {
            if (((*input)[lengthBytes - 1] >> 1) == PADDING_CHAR) {
                lengthSeptets = lengthSeptets - 1;
            }
        }
    }

    decoded = GsmAlphabet_Gsm7BitPackedToString(input, 0, lengthSeptets, 0);

    // Return "" if decoding of GSM packed string fails
    if (decoded.IsNullOrEmpty()) {
//        Log.e(TAG, "Decoding of GSM packed string failed");
        decoded = "";
    }

    return decoded;
}

String CGpsNetInitiatedHandler::DecodeUTF8String(
    /* [in] */ ArrayOf<Byte>* input)
{
    String decoded((char const*)(input->GetPayload()));

    return decoded;
}

String CGpsNetInitiatedHandler::DecodeUCS2String(
    /* [in] */ ArrayOf<Byte>* input)
{
    String decoded("");
    PFL_EX("TODO: need UTF-16 String")
///*    try {
//        decoded = String(input, "UTF-16");
//    }
//    catch (UnsupportedEncodingException e)
//    {
//        throw new AssertionError();
//    }*/
//
    return decoded;
}

/** Decode NI string
 *
 * @param original   The text string to be decoded
 * @param isHex      Specifies whether the content of the string has been encoded as a Hex string. Encoding
 *                   a string as Hex can allow zeros inside the coded text.
 * @param coding     Specifies the coding scheme of the string, such as GSM, UTF8, UCS2, etc. This coding scheme
 *                      needs to match those used passed to HAL from the native GPS driver. Decoding is done according
 *                   to the <code> coding </code>, after a Hex string is decoded. Generally, if the
 *                   notification strings don't need further decoding, <code> coding </code> encoding can be
 *                   set to -1, and <code> isHex </code> can be false.
 * @return the decoded string
 */
String CGpsNetInitiatedHandler::DecodeString(
    /* [in] */ const String& original,
    /* [in] */ Boolean isHex,
    /* [in] */ Int32 coding)
{
    String decoded = original;
    AutoPtr<ArrayOf<Byte> > input = StringToByteArray(original, isHex);

    switch (coding) {
    case IGpsNetInitiatedHandler::GPS_ENC_NONE:
        decoded = original;
        break;

    case IGpsNetInitiatedHandler::GPS_ENC_SUPL_GSM_DEFAULT:
        decoded = DecodeGSMPackedString(input);
        break;

    case IGpsNetInitiatedHandler::GPS_ENC_SUPL_UTF8:
        decoded = DecodeUTF8String(input);
        break;

    case IGpsNetInitiatedHandler::GPS_ENC_SUPL_UCS2:
        decoded = DecodeUCS2String(input);
        break;

    case IGpsNetInitiatedHandler::GPS_ENC_UNKNOWN:
        decoded = original;
        break;

    default:
//        Log.e(TAG, "Unknown encoding " + coding + " for NI text " + original);
        break;
    }

    return decoded;
}

// change this to configure notification display
String CGpsNetInitiatedHandler::GetNotifTicker(
    /* [in] */ IGpsNiNotification* notif,
    /* [in] */ IContext* context)
{
    assert(notif != NULL);
    assert(context != NULL);

    String ticker;
    context->GetString(R::string::gpsNotifTicker, &ticker);
    StringBuilder sb;
    String requestorId, text;
    Int32 requestorIdEncoding, textEncoding;
    notif->GetRequestorId(&requestorId);
    notif->GetText(&text);
    notif->GetRequestorIdEncoding(&requestorIdEncoding);
    notif->GetTextEncoding(&textEncoding);
    sb += ticker;
    sb += DecodeString(requestorId, mIsHexInput, requestorIdEncoding);
    sb += DecodeString(text, mIsHexInput, textEncoding);
    sb.ToString(&ticker);
    return ticker;
}

// change this to configure notification display
String CGpsNetInitiatedHandler::GetNotifTitle(
    /* [in] */ IGpsNiNotification* notif,
    /* [in] */ IContext* context)
{
    assert(notif != NULL);
    assert(context != NULL);

    String title;
    context->GetString(R::string::gpsNotifTitle, &title);
    return title;
}

// change this to configure notification display
String CGpsNetInitiatedHandler::GetNotifMessage(
    /* [in] */ IGpsNiNotification* notif,
    /* [in] */ IContext* context)
{
    assert(notif != NULL);
    assert(context != NULL);

    String message;
    context->GetString(R::string::gpsNotifMessage, &message);
    StringBuilder sb;
    String text, requestorId;
    Int32 requestorIdEncoding, textEncoding;
    notif->GetText(&text);
    notif->GetRequestorId(&requestorId);
    notif->GetRequestorIdEncoding(&requestorIdEncoding);
    notif->GetTextEncoding(&textEncoding);
    sb += message;
    sb += DecodeString(requestorId, mIsHexInput, requestorIdEncoding);
    sb += DecodeString(text, mIsHexInput, textEncoding);
    sb.ToString(&message);
    return message;
}

// change this to configure dialog display (for verification)
String CGpsNetInitiatedHandler::GetDialogTitle(
    /* [in] */ IGpsNiNotification* notif,
    /* [in] */ IContext* context)
{
    return GetNotifTitle(notif, context);
}

// change this to configure dialog display (for verification)
String CGpsNetInitiatedHandler::GetDialogMessage(
    /* [in] */ IGpsNiNotification* notif,
    /* [in] */ IContext* context)
{
    assert(notif != NULL);
    assert(context != NULL);

    return GetNotifMessage(notif, context);
}

} // namespace Location
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
