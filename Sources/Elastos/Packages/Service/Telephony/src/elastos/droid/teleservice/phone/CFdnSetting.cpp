//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/teleservice/phone/CFdnSetting.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/CCallFeaturesSetting.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.CoreLibrary.Core.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL_2(CFdnSetting::InnerListener, Object, \
    IEditPinPreferenceOnPinEnteredListener, \
    IDialogInterfaceOnCancelListener)

CFdnSetting::InnerListener::InnerListener(
    /* [in] */ CFdnSetting* host)
    : mHost(host)
{
}

ECode CFdnSetting::InnerListener::OnPinEntered(
    /* [in] */ IPhoneEditPinPreference* preference,
    /* [in] */ Boolean positiveResult)
{
    return mHost->OnPinEntered(preference, positiveResult);
}

ECode CFdnSetting::InnerListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->OnCancel(dialog);
}

CFdnSetting::MyHandler::MyHandler(
    /* [in] */ CFdnSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CFdnSetting::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        // when we are enabling FDN, either we are unsuccessful and display
        // a toast, or just update the UI.
        case EVENT_PIN2_ENTRY_COMPLETE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL && ICommandException::Probe(ar->mException) != NULL) {
                Int32 attemptsRemaining;
                msg->GetArg1(&attemptsRemaining);
                // see if PUK2 is requested and alert the user accordingly.
                assert(0);
                // CommandException.Error e =
                //         ((CommandException) ar.exception).getCommandError();
                // switch (e) {
                //     case SIM_PUK2:
                //         // make sure we set the PUK2 state so that we can skip
                //         // some redundant behaviour.
                //         mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::fdn_enable_puk2_requested,
                //                 attemptsRemaining);
                //         ResetPinChangeStateForPUK2();
                //         break;
                //     case PASSWORD_INCORRECT:
                //         mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::pin2_invalid,
                //                 attemptsRemaining);
                //         break;
                //     default:
                //         mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::fdn_failed,
                //                 attemptsRemaining);
                //         break;
                // }
            }
            mHost->UpdateEnableFDN();
            break;
        }
        // when changing the pin we need to pay attention to whether or not
        // the error requests a PUK (usually after too many incorrect tries)
        // Set the state accordingly.
        case EVENT_PIN2_CHANGE_COMPLETE:
        {
            if (DBG) mHost->Log(String("Handle EVENT_PIN2_CHANGE_COMPLETE"));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Int32 attemptsRemaining;
                msg->GetArg1(&attemptsRemaining);
                StringBuilder sb;
                sb += "Handle EVENT_PIN2_CHANGE_COMPLETE attemptsRemaining=";
                sb += attemptsRemaining;
                mHost->Log(sb.ToString());
                assert(0);
                // CommandException ce = (CommandException) ar.exception;
                // if (ce.getCommandError() == CommandException.Error.SIM_PUK2) {
                //     // throw an alert dialog on the screen, displaying the
                //     // request for a PUK2.  set the cancel listener to
                //     // FdnSetting.onCancel().
                //     AutoPtr<IAlertDialogBuilder> builder;
                //     CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
                //     builder->SetMessage(R.string.puk2_requested);
                //     builder->SetCancelable(TRUE);
                //     builder->SetOnCancelListener(this);
                //     AutoPtr<IAlertDialog> a;
                //     builder->Create((IAlertDialog**)&a);
                //     AutoPtr<IWindow> window;
                //     a->GetWindow((IWindow**)&window);
                //     window->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
                //     a->Show();
                // }
                // else {
                //     // set the correct error message depending upon the state.
                //     // Reset the state depending upon or knowledge of the PUK state.
                //     if (!mIsPuk2Locked) {
                //         mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::badPin2, attemptsRemaining);
                //         mHost->ResetPinChangeState();
                //     }
                //     else {
                //         mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::badPuk2, attemptsRemaining);
                //         mHost->ResetPinChangeStateForPUK2();
                //     }
                // }
            }
            else {
                if (mHost->mPinChangeState == PIN_CHANGE_PUK) {
                    mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::pin2_unblocked);
                }
                else {
                    mHost->DisplayMessage(Elastos::Droid::TeleService::R::string::pin2_changed);
                }

                // reset to normal behaviour on successful change.
                mHost->ResetPinChangeState();
            }
            break;
        }
    }
    return NOERROR;
}

const String CFdnSetting::TAG("CFdnSetting");// = PhoneGlobals.TAG;
const Boolean CFdnSetting::DBG = FALSE;

const String CFdnSetting::BUTTON_FDN_ENABLE_KEY("button_fdn_enable_key");
const String CFdnSetting::BUTTON_CHANGE_PIN2_KEY("button_change_pin2_key");

const String CFdnSetting::SKIP_OLD_PIN_KEY("skip_old_pin_key");
const String CFdnSetting::PIN_CHANGE_STATE_KEY("pin_change_state_key");
const String CFdnSetting::OLD_PIN_KEY("old_pin_key");
const String CFdnSetting::NEW_PIN_KEY("new_pin_key");
const String CFdnSetting::DIALOG_MESSAGE_KEY("dialog_message_key");
const String CFdnSetting::DIALOG_PIN_ENTRY_KEY("dialog_pin_entry_key");

const Int32 CFdnSetting::MIN_PIN_LENGTH = 4;
const Int32 CFdnSetting::MAX_PIN_LENGTH = 8;

CAR_OBJECT_IMPL(CFdnSetting)

CFdnSetting::CFdnSetting()
    : mOldPin(NULL)
    , mNewPin(NULL)
    , mPuk2(NULL)
    , mPinChangeState(0)
    , mIsPuk2Locked(FALSE)
{
    mFDNHandler = new MyHandler(this);
}

ECode CFdnSetting::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CFdnSetting::OnPinEntered(
    /* [in] */ IPhoneEditPinPreference* preference,
    /* [in] */ Boolean positiveResult)
{
    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEnableFDN)) {
        ToggleFDNEnable(positiveResult);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonChangePin2)){
        UpdatePINChangeState(positiveResult);
    }
    return NOERROR;
}

void CFdnSetting::ToggleFDNEnable(
    /* [in] */ Boolean positiveResult)
{
    if (!positiveResult) {
        return;
    }

    // validate the pin first, before submitting it to the RIL for FDN enable.
    String password;
    IEditTextPreference::Probe(mButtonEnableFDN)->GetText(&password);
    if (ValidatePin(password, FALSE)) {
        // get the relevant data for the icc call
        AutoPtr<IIccCard> card;
        mPhone->GetIccCard((IIccCard**)&card);
        Boolean isEnabled;
        card->GetIccFdnEnabled(&isEnabled);

        AutoPtr<IMessage> onComplete;
        mFDNHandler->ObtainMessage(EVENT_PIN2_ENTRY_COMPLETE, (IMessage**)&onComplete);

        // make fdn request
        card->SetIccFdnEnabled(!isEnabled, password, onComplete);
    }
    else {
        // throw up error if the pin is invalid.
        DisplayMessage(Elastos::Droid::TeleService::R::string::invalidPin2);
    }

    IEditTextPreference::Probe(mButtonEnableFDN)->SetText(String(""));
}

void CFdnSetting::UpdatePINChangeState(
    /* [in] */ Boolean positiveResult)
{
    if (DBG) {
        StringBuilder sb;
        sb += "updatePINChangeState positive=";
        sb += positiveResult;
        sb += " mPinChangeState=";
        sb += mPinChangeState;
        sb += " mSkipOldPin=";
        sb += mIsPuk2Locked;
        Log(sb.ToString());
    }

    if (!positiveResult) {
        // reset the state on cancel, either to expect PUK2 or PIN2
        if (!mIsPuk2Locked) {
            ResetPinChangeState();
        }
        else {
            ResetPinChangeStateForPUK2();
        }
        return;
    }

    // Progress through the dialog states, generally in this order:
    //   1. Enter old pin
    //   2. Enter new pin
    //   3. Re-Enter new pin
    // While handling any error conditions that may show up in between.
    // Also handle the PUK2 entry, if it is requested.
    //
    // In general, if any invalid entries are made, the dialog re-
    // appears with text to indicate what the issue is.
    switch (mPinChangeState) {
        case PIN_CHANGE_OLD:
        {
            IEditTextPreference::Probe(mButtonChangePin2)->GetText(&mOldPin);
            IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
            // if the pin is not valid, display a message and reset the state.
            if (ValidatePin(mOldPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_NEW;
                DisplayPinChangeDialog();
            }
            else {
                DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_NEW:
        {
            IEditTextPreference::Probe(mButtonChangePin2)->GetText(&mNewPin);
            IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
            // if the new pin is not valid, display a message and reset the state.
            if (ValidatePin(mNewPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_REENTER;
                DisplayPinChangeDialog();
            } else {
                DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_REENTER:
        {
            String text;
            IEditTextPreference::Probe(mButtonChangePin2)->GetText(&text);
            // if the re-entered pin is not valid, display a message and reset the state.
            if (!mNewPin.Equals(text)) {
                mPinChangeState = PIN_CHANGE_NEW;
                IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
                DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::mismatchPin2, TRUE);
            }
            else {
                // If the PIN is valid, then we submit the change PIN request.
                IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
                AutoPtr<IMessage> onComplete;
                mFDNHandler->ObtainMessage(
                        EVENT_PIN2_CHANGE_COMPLETE, (IMessage**)&onComplete);
                AutoPtr<IIccCard> card;
                mPhone->GetIccCard((IIccCard**)&card);
                card->ChangeIccFdnPassword(mOldPin, mNewPin, onComplete);
            }
            break;
        }
        case PIN_CHANGE_PUK:
        {
                // Doh! too many incorrect requests, PUK requested.
                IEditTextPreference::Probe(mButtonChangePin2)->GetText(&mPuk2);
                IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
                // if the puk is not valid, display
                // a message and reset the state.
                if (ValidatePin(mPuk2, TRUE)) {
                    mPinChangeState = PIN_CHANGE_NEW_PIN_FOR_PUK;
                    DisplayPinChangeDialog();
                }
                else {
                    DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::invalidPuk2, TRUE);
                }
            break;
        }
        case PIN_CHANGE_NEW_PIN_FOR_PUK:
        {
            IEditTextPreference::Probe(mButtonChangePin2)->GetText(&mNewPin);
            IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
            // if the new pin is not valid, display
            // a message and reset the state.
            if (ValidatePin(mNewPin, FALSE)) {
                mPinChangeState = PIN_CHANGE_REENTER_PIN_FOR_PUK;
                DisplayPinChangeDialog();
            }
            else {
                DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::invalidPin2, TRUE);
            }
            break;
        }
        case PIN_CHANGE_REENTER_PIN_FOR_PUK:
        {
            String text;
            IEditTextPreference::Probe(mButtonChangePin2)->GetText(&text);
            // if the re-entered pin is not valid, display
            // a message and reset the state.
            if (!mNewPin.Equals(text)) {
                mPinChangeState = PIN_CHANGE_NEW_PIN_FOR_PUK;
                IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
                DisplayPinChangeDialog(Elastos::Droid::TeleService::R::string::mismatchPin2, TRUE);
            }
            else {
                // Both puk2 and new pin2 are ready to submit
                IEditTextPreference::Probe(mButtonChangePin2)->SetText(String(""));
                AutoPtr<IMessage> onComplete;
                mFDNHandler->ObtainMessage(
                        EVENT_PIN2_CHANGE_COMPLETE, (IMessage**)&onComplete);
                AutoPtr<IIccCard> card;
                mPhone->GetIccCard((IIccCard**)&card);
                card->SupplyPuk2(mPuk2, mNewPin, onComplete);
            }
            break;
        }
    }
}

ECode CFdnSetting::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    // set the state of the preference and then display the dialog.
    ResetPinChangeStateForPUK2();
    DisplayPinChangeDialog(0, TRUE);
    return NOERROR;
}

void CFdnSetting::DisplayMessage(
    /* [in] */ Int32 strId,
    /* [in] */ Int32 attemptsRemaining)
{
    String s;
    GetString(strId, &s);
    if ((strId == Elastos::Droid::TeleService::R::string::badPin2) ||
            (strId == Elastos::Droid::TeleService::R::string::badPuk2) ||
            (strId == Elastos::Droid::TeleService::R::string::pin2_invalid)) {
        if (attemptsRemaining >= 0) {
            StringBuilder sb;
            String str;
            GetString(strId, &str);
            sb += str;
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
            AutoPtr<IInteger32> value = CoreUtils::Convert(attemptsRemaining);
            array->Set(0, value);
            GetString(Elastos::Droid::TeleService::R::string::pin2_attempts, array, &str);
            sb += str;
            s = sb.ToString();
        }
        else {
            GetString(strId, &s);
        }
    }
    StringBuilder sb;
    sb += "displayMessage: attemptsRemaining=";
    sb += attemptsRemaining;
    sb += " s=";
    sb += s;
    Log(sb.ToString());

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(s);
    helper->MakeText(this, cchar, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
}

void CFdnSetting::DisplayMessage(
    /* [in] */ Int32 strId)
{
    DisplayMessage(strId, -1);
}

void CFdnSetting::DisplayPinChangeDialog()
{
    DisplayPinChangeDialog(0, TRUE);
}

void CFdnSetting::DisplayPinChangeDialog(
    /* [in] */ Int32 strId,
    /* [in] */ Boolean shouldDisplay)
{
    Int32 msgId;
    switch (mPinChangeState) {
        case PIN_CHANGE_OLD:
            msgId = Elastos::Droid::TeleService::R::string::oldPin2Label;
            break;
        case PIN_CHANGE_NEW:
        case PIN_CHANGE_NEW_PIN_FOR_PUK:
            msgId = Elastos::Droid::TeleService::R::string::newPin2Label;
            break;
        case PIN_CHANGE_REENTER:
        case PIN_CHANGE_REENTER_PIN_FOR_PUK:
            msgId = Elastos::Droid::TeleService::R::string::confirmPin2Label;
            break;
        case PIN_CHANGE_PUK:
        default:
            msgId = Elastos::Droid::TeleService::R::string::label_puk2_code;
            break;
    }

    // append the note / additional message, if needed.
    if (strId != 0) {
        StringBuilder sb;
        AutoPtr<ICharSequence> cchar;
        GetText(msgId, (ICharSequence**)&cchar);
        String str;
        cchar->ToString(&str);
        sb += str;
        sb += "\n";
        AutoPtr<ICharSequence> cchar2;
        GetText(strId, (ICharSequence**)&cchar2);
        cchar->ToString(&str);
        sb += str;
        AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(sb.ToString());
        IDialogPreference::Probe(mButtonChangePin2)->SetDialogMessage(cchar3);
    }
    else {
        IDialogPreference::Probe(mButtonChangePin2)->SetDialogMessage(msgId);
    }

    // only display if requested.
    if (shouldDisplay) {
        mButtonChangePin2->ShowPinDialog();
    }
}

void CFdnSetting::ResetPinChangeState()
{
    if (DBG) Log(String("resetPinChangeState"));
    mPinChangeState = PIN_CHANGE_OLD;
    DisplayPinChangeDialog(0, FALSE);
    mOldPin = String("");
    mNewPin = String("");
    mIsPuk2Locked = FALSE;
}

void CFdnSetting::ResetPinChangeStateForPUK2()
{
    if (DBG) Log(String("resetPinChangeStateForPUK2"));
    mPinChangeState = PIN_CHANGE_PUK;
    DisplayPinChangeDialog(0, FALSE);
    mOldPin = String("");
    mNewPin = String("");
    mPuk2 = String("");
    mIsPuk2Locked = TRUE;
}

Boolean CFdnSetting::ValidatePin(
    /* [in] */ const String& pin,
    /* [in] */ Boolean isPuk)
{
    // for pin, we have 4-8 numbers, or puk, we use only 8.
    Int32 pinMinimum = isPuk ? MAX_PIN_LENGTH : MIN_PIN_LENGTH;

    // check validity
    if (pin.IsNull() || pin.GetLength() < pinMinimum || pin.GetLength() > MAX_PIN_LENGTH) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void CFdnSetting::UpdateEnableFDN()
{
    AutoPtr<IIccCard> card;
    mPhone->GetIccCard((IIccCard**)&card);
    Boolean enable;
    if (card->GetIccFdnEnabled(&enable), enable) {
        IPreference::Probe(mButtonEnableFDN)->SetTitle(Elastos::Droid::TeleService::R::string::enable_fdn_ok);
        IPreference::Probe(mButtonEnableFDN)->SetSummary(Elastos::Droid::TeleService::R::string::fdn_enabled);
        IDialogPreference::Probe(mButtonEnableFDN)->SetDialogTitle(Elastos::Droid::TeleService::R::string::disable_fdn);
    }
    else {
        IPreference::Probe(mButtonEnableFDN)->SetTitle(Elastos::Droid::TeleService::R::string::disable_fdn_ok);
        IPreference::Probe(mButtonEnableFDN)->SetSummary(Elastos::Droid::TeleService::R::string::fdn_disabled);
        IDialogPreference::Probe(mButtonEnableFDN)->SetDialogTitle(Elastos::Droid::TeleService::R::string::enable_fdn);
    }
}

ECode CFdnSetting::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);

    AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::fdn_setting);

    mPhone = PhoneGlobals::GetPhone();

    //get UI object references
    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(BUTTON_FDN_ENABLE_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar, (IPreference**)&preference);
    mButtonEnableFDN = IPhoneEditPinPreference::Probe(preference);

    AutoPtr<IPreference> preference2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_CHANGE_PIN2_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar2, (IPreference**)&preference2);
    mButtonChangePin2 = IPhoneEditPinPreference::Probe(preference2);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    //assign click listener and update state
    mButtonEnableFDN->SetOnPinEnteredListener(listener);
    UpdateEnableFDN();

    mButtonChangePin2->SetOnPinEnteredListener(listener);

    // Only reset the pin change dialog if we're not in the middle of changing it.
    if (icicle == NULL) {
        ResetPinChangeState();
    }
    else {
        icicle->GetBoolean(SKIP_OLD_PIN_KEY, &mIsPuk2Locked);
        icicle->GetInt32(PIN_CHANGE_STATE_KEY, &mPinChangeState);
        icicle->GetString(OLD_PIN_KEY, &mOldPin);
        icicle->GetString(NEW_PIN_KEY, &mNewPin);

        String str;
        icicle->GetString(DIALOG_MESSAGE_KEY, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        IDialogPreference::Probe(mButtonChangePin2)->SetDialogMessage(cchar);

        String str2;
        icicle->GetString(DIALOG_PIN_ENTRY_KEY, &str2);
        IEditTextPreference::Probe(mButtonChangePin2)->SetText(str2);
    }

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CFdnSetting::OnResume()
{
    PreferenceActivity::OnResume();
    mPhone = PhoneGlobals::GetPhone();
    UpdateEnableFDN();
    return NOERROR;
}

ECode CFdnSetting::OnSaveInstanceState(
    /* [in] */ IBundle* _out)
{
    PreferenceActivity::OnSaveInstanceState(_out);
    _out->PutBoolean(SKIP_OLD_PIN_KEY, mIsPuk2Locked);
    _out->PutInt32(PIN_CHANGE_STATE_KEY, mPinChangeState);
    _out->PutString(OLD_PIN_KEY, mOldPin);
    _out->PutString(NEW_PIN_KEY, mNewPin);

    AutoPtr<ICharSequence> dialogMessage;
    IDialogPreference::Probe(mButtonChangePin2)->GetDialogMessage((ICharSequence**)&dialogMessage);
    String str;
    dialogMessage->ToString(&str);
    _out->PutString(DIALOG_MESSAGE_KEY, str);

    String str2;
    IEditTextPreference::Probe(mButtonChangePin2)->GetText(&str2);
    return _out->PutString(DIALOG_PIN_ENTRY_KEY, str2);
}

ECode CFdnSetting::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        CCallFeaturesSetting::GoUpToTopLevelSetting(this);
        *result = TRUE;
        return NOERROR;
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

void CFdnSetting::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "FdnSetting: ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos