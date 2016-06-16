
#include "elastos/droid/phone/SpecialCharSequenceMgr.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const String SpecialCharSequenceMgr::TAG = PhoneGlobals::LOG_TAG;
const Boolean SpecialCharSequenceMgr::DBG = FALSE;

const String SpecialCharSequenceMgr::MMI_IMEI_DISPLAY("*#06#");
const String SpecialCharSequenceMgr::MMI_REGULATORY_INFO_DISPLAY("*#07#");

Boolean SpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    return HandleChars(context, input, NULL);
}

Boolean SpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ IActivity* pukInputActivity)
{
    //get rid of the separators so that the string gets parsed correctly
    String dialString = PhoneNumberUtils::StripSeparators(input);

    if (HandleIMEIDisplay(context, dialString)
        || HandleRegulatoryInfoDisplay(context, dialString)
        || HandlePinEntry(context, dialString, pukInputActivity)
        || HandleAdnEntry(context, dialString)
        || HandleSecretCode(context, dialString)) {
        return TRUE;
    }

    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleCharsForLockedDevice(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ IActivity* pukInputActivity)
{
    // Get rid of the separators so that the string gets parsed correctly
    String dialString = PhoneNumberUtils::StripSeparators(input);

    // The only sequences available on a locked device are the "**04"
    // or "**05" sequences that allow you to enter PIN or PUK-related
    // codes.  (e.g. for the case where you're currently locked out of
    // your phone, and need to change the PIN!  The only way to do
    // that is via the Emergency Dialer.)

    if (HandlePinEntry(context, dialString, pukInputActivity)) {
        return TRUE;
    }

    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleSecretCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    // Secret codes are in the form *#*#<code>#*#*
    Int32 len = input.GetLength();
    if (len > 8 && input.StartsWith(String("*#*#")) && input.EndsWith(String("#*#*"))) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        StringBuilder sb;
        sb += "android_secret_code://";
        sb += input.substring(4, len - 4);
        AutoPtr<IUri> uri;
        helper->Parse(sb.ToString(), (IUri**)&uri);

        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyIntents::SECRET_CODE_ACTION,
                uri, (IIntent**)&intent);
        context->SendBroadcast(intent);
        return TRUE;
    }

    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleAdnEntry(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    /* ADN entries are of the form "N(N)(N)#" */

    // if the phone is keyguard-restricted, then just ignore this
    // input.  We want to make sure that sim card contacts are NOT
    // exposed unless the phone is unlocked, and this code can be
    // accessed from the emergency dialer.
    AutoPtr<IPhoneGlobals> instance = PhoneGlobals::GetInstance();
    AutoPtr<IKeyguardManager> manager;
    instance->GetKeyguardManager((IKeyguardManager**)&manager);
    Boolean res;
    if (manager->InKeyguardRestrictedInputMode(&res), res) {
        return FALSE;
    }

    Int32 len = input.GetLength();
    if ((len > 1) && (len < 5) && (input.EndsWith(String("#")))) {
        //try {
        String subs = input.Substring(0, len - 1);
        Int32 index = Integer.parseInt(subs);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_PICK, (IIntent**)&intent);

        intent->SetClassName(String("com.android.phone"),
                String("com.android.phone.SimContacts"));
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->PutExtra(String("index"), index);
        instance->StartActivity(intent);

        return TRUE;
        //} catch (NumberFormatException ex) {}
    }
    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandlePinEntry(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ IActivity* pukInputActivity)
{
    // TODO: The string constants here should be removed in favor
    // of some call to a static the MmiCode class that determines
    // if a dialstring is an MMI code.
    if ((input.startsWith("**04") || input.startsWith("**05"))
            && input.endsWith("#")) {
        AutoPtr<IPhoneGlobals> app = PhoneGlobals::GetInstance();
        Boolean isMMIHandled = app.phone->HandlePinMmi(input);

        // if the PUK code is recognized then indicate to the
        // phone app that an attempt to unPUK the device was
        // made with this activity.  The PUK code may still
        // fail though, but we won't know until the MMI code
        // returns a result.
        if (isMMIHandled && input.StartsWith("**05")) {
            app->SetPukEntryActivity(pukInputActivity);
        }
        return isMMIHandled;
    }
    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleIMEIDisplay(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    if (input.Equals(MMI_IMEI_DISPLAY)) {
        ShowDeviceIdPanel(context);
        return TRUE;
    }

    return FALSE;
}

void SpecialCharSequenceMgr::ShowDeviceIdPanel(
    /* [in] */ IContext* context)
{
    if (DBG) Log(String("showDeviceIdPanel()..."));

    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    Int32 labelId = TelephonyCapabilities::GetDeviceIdLabel(phone);
    String deviceId;
    phone->GetDeviceId(&deviceId);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New((IAlertDialogBuilder**)&builder);
    builder->SetTitle(labelId);
    builder->SetMessage(deviceId);
    builder->SetPositiveButton(R.string.ok, NULL);
    builder->SetCancelable(FALSE);

    AutoPtr<IAlertDialog> alert;
    builder->Create((IAlertDialog**)&alert);

    AutoPtr<IWindow> window;
    alert->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE);
    alert->Show();
}

Boolean SpecialCharSequenceMgr::HandleRegulatoryInfoDisplay(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    if (input.Equals(MMI_REGULATORY_INFO_DISPLAY)) {
        Log(String("handleRegulatoryInfoDisplay() sending intent to settings app"));
        AutoPtr<IIntent> showRegInfoIntent;
        CIntent::New(ISettings::ACTION_SHOW_REGULATORY_INFO, (IIntent**)&showRegInfoIntent);
        //try {
        ECode ec = context->StartActivity(showRegInfoIntent);
        //} catch (ActivityNotFoundException e) {
        if (ec == (ECode)ActivityNotFoundException) {
            Logger::E(TAG, "startActivity() failed: %d", ec);
        }
        //}
        return TRUE;
    }
    return FALSE;
}

void SpecialCharSequenceMgr::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, "[SpecialCharSequenceMgr] %s", msg.string());
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos