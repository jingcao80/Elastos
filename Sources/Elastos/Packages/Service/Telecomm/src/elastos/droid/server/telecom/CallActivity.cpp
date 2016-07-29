
#include "R.h"
#include "elastos/droid/server/telecom/CallActivity.h"
#include "elastos/droid/server/telecom/CallReceiver.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telecom.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/droid/R.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Telecom::CVideoProfileVideoStateHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Telecom::IVideoProfileVideoStateHelper;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(CallActivity, Activity, ICallActivity)

ECode CallActivity::OnCreate(
    /* [in] */ IBundle* bundle)
{
    Activity::OnCreate(bundle);
    // TODO: Figure out if there is something to restore from bundle.
    // See OutgoingCallBroadcaster in services/Telephony for more.
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    ProcessIntent(intent);
    // This activity does not have associated UI, so close.
    Finish();
    Log::D("CallActivity", "onCreate: end");
    return NOERROR;
}

ECode CallActivity::ProcessIntent(
    /* [in] */ IIntent* intent)
{
    // Ensure call intents are not processed on devices that are not capable of calling.
    Boolean isVoiceCapable;
    IsVoiceCapable(&isVoiceCapable);
    if (!isVoiceCapable) {
        return NOERROR;
    }
    VerifyCallAction(intent);
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CALL.Equals(action) ||
            IIntent::ACTION_CALL_PRIVILEGED.Equals(action) ||
            IIntent::ACTION_CALL_EMERGENCY.Equals(action)) {
        ProcessOutgoingCallIntent(intent);
    } else if (ITelecomManager::ACTION_INCOMING_CALL.Equals(action)) {
        ProcessIncomingCallIntent(intent);
    }
    return NOERROR;
}

ECode CallActivity::VerifyCallAction(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    String className;
    component->GetClassName(&className);
    if (className.Equals("CallActivity")) {
        // If we were launched directly from the CallActivity, not one of its more privileged
        // aliases, then make sure that only the non-privileged actions are allowed.
        String action;
        intent->GetAction(&action);
        if (!IIntent::ACTION_CALL.Equals(action)) {
            Log::W("CallActivity", "Attempt to deliver non-CALL action; forcing to CALL");
            intent->SetAction(IIntent::ACTION_CALL);
        }
    }
    return NOERROR;
}

ECode CallActivity::ProcessOutgoingCallIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUri> handle;
    intent->GetData((IUri**)&handle);
    String scheme;
    handle->GetScheme(&scheme);
    String uriString;
    handle->GetSchemeSpecificPart(&uriString);
    if (!IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme)) {
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        Boolean isUriNumber;
        phoneNumberUtilsHelper->IsUriNumber(uriString, &isUriNumber);
        Uri::FromParts(isUriNumber ?
                IPhoneAccount::SCHEME_SIP : IPhoneAccount::SCHEME_TEL, uriString, String(NULL), (IUri**)&handle);
    }
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    Boolean hasUserRestriction;
    userManager->HasUserRestriction(IUserManager::DISALLOW_OUTGOING_CALLS, &hasUserRestriction);
    Boolean shouldProcessAsEmergency;
    TelephonyUtil::ShouldProcessAsEmergency(this, handle, &shouldProcessAsEmergency);
    if (hasUserRestriction
            && !shouldProcessAsEmergency) {
        // Only emergency calls are allowed for users with the DISALLOW_OUTGOING_CALLS
        // restriction.
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        String str;
        res->GetString(R::string::outgoing_call_not_allowed, &str);

        AutoPtr<IToastHelper> tHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&tHelper);
        AutoPtr<IToast> toast;
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        tHelper->MakeText(this, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        Log::D("CallActivity", "Rejecting non-emergency phone call due to DISALLOW_OUTGOING_CALLS "
                "restriction");
        return NOERROR;
    }
    Int32 videoState;
    intent->GetInt32Extra(
            ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE,
            IVideoProfileVideoState::AUDIO_ONLY, &videoState);
    Log::D("CallActivity", "processOutgoingCallIntent videoState = %d", videoState);
    Boolean isTtyModeEnabled;
    IsTtyModeEnabled(&isTtyModeEnabled);

    AutoPtr<IVideoProfileVideoStateHelper> vsHelper;
    CVideoProfileVideoStateHelper::AcquireSingleton((IVideoProfileVideoStateHelper**)&vsHelper);
    Boolean vFlag = FALSE;
    vsHelper->IsVideo(videoState, &vFlag);
    if (vFlag && isTtyModeEnabled) {
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        AutoPtr<IToastHelper> tHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&tHelper);
        String v;
        res->GetString(R::string::video_call_not_allowed_if_tty_enabled, &v);
        AutoPtr<ICharSequence> cs;
        CString::New(v, (ICharSequence**)&cs);
        AutoPtr<IToast> toast;
        tHelper->MakeText(this, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        Log::D("CallActivity", "Rejecting video calls as tty is enabled");
        return NOERROR;
    }
    obj = NULL;
    GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(obj);
    Int32 launchedFromUid;
    String launchedFromPackage;
    // try {
    ECode ec;
    do {
        AutoPtr<IBinder> binder;
        ec = GetActivityToken((IBinder**)&binder);
        if (FAILED(ec)) break;
        ec = ActivityManagerNative::GetDefault()
                ->GetLaunchedFromUid(binder, &launchedFromUid);
        if (FAILED(ec)) break;
        ec = ActivityManagerNative::GetDefault()
                ->GetLaunchedFromPackage(binder, &launchedFromPackage);
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            launchedFromUid = -1;
            launchedFromPackage = NULL;
        }
        else {
            return ec;
        }
    }
    // }
    Int32 noteOpNoThrow = 0;
    appOps->NoteOpNoThrow(IAppOpsManager::OP_CALL_PHONE, launchedFromUid,
            launchedFromPackage, &noteOpNoThrow);
    if (noteOpNoThrow != IAppOpsManager::MODE_ALLOWED) {
        Log::W("CallActivity", "Rejecting call from uid %d package %s", launchedFromUid,
                launchedFromPackage.string());
        return NOERROR;
    }
    Boolean isDefaultDialer;
    IsDefaultDialer(&isDefaultDialer);
    intent->PutBooleanExtra(CallReceiver::KEY_IS_DEFAULT_DIALER, isDefaultDialer);
    if (UserHandle::GetMyUserId() == IUserHandle::USER_OWNER) {
        AutoPtr<IContext> context;
        GetApplicationContext((IContext**)&context);
        CallReceiver::ProcessOutgoingCallIntent(context, intent);
    } else {
        Boolean bNoUse;
        SendBroadcastToReceiver(intent, FALSE /* isIncoming */, &bNoUse);
    }
    return NOERROR;
}

ECode CallActivity::IsTtyModeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    Int32 i32;
    Elastos::Droid::Provider::Settings::Secure::GetInt32(
            contentResolver,
            Droid::Provider::ISettingsSecure::PREFERRED_TTY_MODE,
            ITelecomManager::TTY_MODE_OFF, &i32);
    *result = i32 != ITelecomManager::TTY_MODE_OFF;
    return NOERROR;
}

ECode CallActivity::ProcessIncomingCallIntent(
    /* [in] */ IIntent* intent)
{
    if (UserHandle::GetMyUserId() == IUserHandle::USER_OWNER) {
        CallReceiver::ProcessIncomingCallIntent(intent);
    } else {
        Boolean bNoUse;
        SendBroadcastToReceiver(intent, TRUE /* isIncoming */, &bNoUse);
    }
    return NOERROR;
}

ECode CallActivity::IsDefaultDialer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String packageName;
    GetCallingPackage(&packageName);
    if (TextUtils::IsEmpty(packageName)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelecomManager> telecomManager = ITelecomManager::Probe(obj);
    AutoPtr<IComponentName> defaultPhoneApp;
    telecomManager->GetDefaultPhoneApp((IComponentName**)&defaultPhoneApp);
    String appPackageName;
    defaultPhoneApp->GetPackageName(&appPackageName);
    *result = (defaultPhoneApp != NULL
            && TextUtils::Equals(packageName, appPackageName));
    return NOERROR;
}

ECode CallActivity::IsVoiceCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetBoolean(
            Elastos::Droid::R::bool_::config_voice_capable, result);
}

ECode CallActivity::SendBroadcastToReceiver(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean incoming,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    intent->PutExtra(CallReceiver::KEY_IS_INCOMING_CALL, incoming);
    intent->SetFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    intent->SetClass(this, ECLSID_CCallReceiver);
    Log::D("CallActivity", "Sending broadcast as user to CallReceiver- isIncoming: %d", incoming);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> owner;
    helper->GetOWNER((IUserHandle**)&owner);
    SendBroadcastAsUser(intent, owner);
    *result = TRUE;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
