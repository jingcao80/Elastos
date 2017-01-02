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

#include "elastos/droid/dialer/SpecialCharSequenceMgr.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Telephony::ITelephonyManager;

namespace Elastos {
namespace Droid {
namespace Dialer {


//================================================================
// SpecialCharSequenceMgr::SimContactQueryCookie
//================================================================
CAR_INTERFACE_IMPL(SpecialCharSequenceMgr::SimContactQueryCookie, Object, IDialogInterfaceOnCancelListener);

SpecialCharSequenceMgr::SimContactQueryCookie::SimContactQueryCookie(
    /* [in] */ Int32 number,
    // TODO:
    // /* [in] */ QueryHandler* handler,
    /* [in] */ Int32 token)
    : mContactNum(number)
    // TODO:
    // , mHandler(handler)
    , mToken(token)
{}

AutoPtr<IEditText> SpecialCharSequenceMgr::SimContactQueryCookie::GetTextField()
{
    synchronized(this) {
        return mTextField;
    }
}

void SpecialCharSequenceMgr::SimContactQueryCookie::SetTextField(
    /* [in] */ IEditText* text)
{
    synchronized(this) {
        mTextField = text;
    }
}

CARAPI SpecialCharSequenceMgr::SimContactQueryCookie::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    synchronized(this) {
        // close the progress dialog
        if (mProgressDialog != NULL) {
            mProgressDialog->Dismiss();
        }

        // setting the textfield to null ensures that the UI does NOT get
        // updated.
        mTextField = NULL;

        // Cancel the operation if possible.
        // TODO:
        // mHandler->CancelOperation(mToken);
        return NOERROR;
    }
}

//================================================================
// SpecialCharSequenceMgr::QueryHandler
//================================================================
// TODO:
// ECode SpecialCharSequenceMgr::QueryHandler::constructor(
//     /* [in] */ IContentResolver* cr)
// {
//     return NoNullCursorAsyncQueryHandler::constructor(cr);
// }

// ECode SpecialCharSequenceMgr::QueryHandler::OnNotNullableQueryComplete(
//     /* [in] */ Int32 token,
//     /* [in] */ IInterface* cookie,
//     /* [in] */ ICursor* cursor)
// {
//     // try {
//     sPreviousAdnQueryHandler = NULL;
//     if (mCanceled) {
//         return;
//     }

//     SimContactQueryCookie* sc = (SimContactQueryCookie*) cookie;

//     // close the progress dialog.
//     sc->mProgressDialog->Dismiss();

//     // get the EditText to update or see if the request was cancelled.
//     AutoPtr<IEditText> text = sc->GetTextField();

//     // if the textview is valid, and the cursor is valid and postionable
//     // on the Nth number, then we update the text field and display a
//     // toast indicating the caller name.
//     Boolean succeeded;
//     if ((c != NULL) && (text != NULL) &&
//             (c->MoveToPosition(sc->mContactNum, &succeeded), succeeded)) {
//         Int32 columnIndex;
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ADN_NAME_COLUMN_NAME, &columnIndex), exit);
//         String name;
//         c->GetString(columnIndex, &name);
//         FAIL_GOTO(c->GetColumnIndexOrThrow(ADN_PHONE_NUMBER_COLUMN_NAME, &columnIndex));
//         String number;
//         c->GetString(columnIndex, &number);

//         // fill the text in.
//         AutoPtr<ICharSequence> cs;
//         ITextView::Probe(text)->GetText((ICharSequence**)&cs);
//         CoreUtils::Unbox(cs).Replace(0, 0, number);

//         // display the name as a toast
//         AutoPtr<IContext> context;
//         sc->mProgressDialog->GetContext((IContext**)&context);
//         context->GetString(R::string::menu_callNumber, name, &name);

//         AutoPtr<IToastHelper> toastHelper;
//         CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
//         AutoPtr<IToast> toast;
//         toastHelper->MakeText(context,
//                 name,
//                 IToast::LENGTH_SHORT, (IToast**)&toast);
//         toast->Show();
//     }

// exit:
//     assert(0 && "TODO");
//     // } finally {
//     //     MoreCloseables.closeQuietly(c);
//     // }
//     return NOERROR;
// }

// void SpecialCharSequenceMgr::QueryHandler::Cancel()
// {
//     mCanceled = TRUE;
//     // Ask AsyncQueryHandler to cancel the whole request. This will fails when the
//     // query already started.
//     CancelOperation(ADN_QUERY_TOKEN);
// }

//================================================================
// SpecialCharSequenceMgr
//================================================================
const String SpecialCharSequenceMgr::TAG("SpecialCharSequenceMgr");

const String SpecialCharSequenceMgr::SECRET_CODE_ACTION("android.provider.Telephony.SECRET_CODE");
const String SpecialCharSequenceMgr::MMI_IMEI_DISPLAY("*#06#");
const String SpecialCharSequenceMgr::MMI_REGULATORY_INFO_DISPLAY("*#07#");

// TODO:
// AutoPtr<QueryHandler> SpecialCharSequenceMgr::sPreviousAdnQueryHandler;

const String SpecialCharSequenceMgr::ADN_PHONE_NUMBER_COLUMN_NAME("number");
const String SpecialCharSequenceMgr::ADN_NAME_COLUMN_NAME("name");
const Int32 SpecialCharSequenceMgr::ADN_QUERY_TOKEN = -1;

SpecialCharSequenceMgr::SpecialCharSequenceMgr()
{}

Boolean SpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ IEditText* textField)
{
    return HandleChars(context, input, FALSE, textField);
}

Boolean SpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    return handleChars(context, input, FALSE, NULL);
}

Boolean SpecialCharSequenceMgr::HandleChars(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ Boolean useSystemWindow,
    /* [in] */ IEditText* textField)
{
    //get rid of the separators so that the string gets parsed correctly
    String dialString = PhoneNumberUtils.stripSeparators(input);

    if (HandleIMEIDisplay(context, dialString, useSystemWindow)
            || HandleRegulatoryInfoDisplay(context, dialString)
            || HandlePinEntry(context, dialString)
            || HandleAdnEntry(context, dialString, textField)
            || HandleSecretCode(context, dialString)) {
        return TRUE;
    }

    return FALSE;
}

void SpecialCharSequenceMgr::Cleanup()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMyLooper(&looper);
    AutoPtr<ILooper> mainLooper;
    helper->GetMainLooper(&mainLooper);
    if (looper != mainLooper) {
        Logger::Wtf(TAG, "cleanup() is called outside the main thread");
        return;
    }

    assert(0 && "TODO");
    // if (sPreviousAdnQueryHandler != NULL) {
    //     sPreviousAdnQueryHandler->Cancel();
    //     sPreviousAdnQueryHandler = NULL;
    // }
}

Boolean SpecialCharSequenceMgr::HandleSecretCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    // Secret codes are in the form *#*#<code>#*#*
    Int32 len = input.GetLength();
    if (len > 8 && input.StartWith("*#*#") && input.EndWith("#*#*")) {
        AutoPtr<IIntent> intent;
        CIntent::New(SECRET_CODE_ACTION,
                Uri::Parse("android_secret_code://" + input.Substring(4, len - 4)),
                (IIntent**)&intent);
        context->SendBroadcast(intent);
        return TRUE;
    }

    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleAdnEntry(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ IEditText* textField)
{
    /* ADN entries are of the form "N(N)(N)#" */

    AutoPtr<IInterface> telephonyManager;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&telephonyManager);
    Int32 type;
    if (telephonyManager == NULL
            || ITelephonyManager::Probe(telephonyManager)->GetPhoneType(
                    &type), type != ITelephonyManager::PHONE_TYPE_GSM) {
        return FALSE;
    }

    // if the phone is keyguard-restricted, then just ignore this
    // input.  We want to make sure that sim card contacts are NOT
    // exposed unless the phone is unlocked, and this code can be
    // accessed from the emergency dialer.
    AutoPtr<IInterface> keyguardManager;
    context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&keyguardManager);
    Boolean result;
    if (IKeyguardManager::Probe(keyguardManager)->InKeyguardRestrictedInputMode(&result), result) {
        return FALSE;
    }

    Int32 len = input.GetLength();
    if ((len > 1) && (len < 5) && (input.EndsWith("#"))) {
        // try {
        // get the ordinal number of the sim contact
        Int32 index = StringUtils::ParseInt32(input.Substring(0, len-1));

        // The original code that navigated to a SIM Contacts list view did not
        // highlight the requested contact correctly, a requirement for PTCRB
        // certification.  This behaviour is consistent with the UI paradigm
        // for touch-enabled lists, so it does not make sense to try to work
        // around it.  Instead we fill in the the requested phone number into
        // the dialer text field.

        // create the async query handler
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        assert(0 && "TODO");
        // AutoPtr<QueryHandler> handler = new QueryHandler(resolver);

        // create the cookie object
        // AutoPtr<SimContactQueryCookie> sc = new SimContactQueryCookie(
        //         index - 1, handler, ADN_QUERY_TOKEN);

        // // setup the cookie fields
        // sc->mContactNum = index - 1;
        // sc->SetTextField(textField);

        // // create the progress dialog
        // CProgressDialog::New(context, (IProgressDialog**)&sc->mProgressDialog);
        // sc->mProgressDialog->SetTitle(R::string::simContacts_title);
        // String text;
        // context->GetText(R:;string::simContacts_emptyLoading, &text);
        // sc->mProgressDialog->SetMessage(text);
        // sc->mProgressDialog->SetIndeterminate(TRUE);
        // sc->mProgressDialog->SetCancelable(TRUE);
        // sc->mProgressDialog->SetOnCancelListener(sc);
        // AutoPtr<IWindow> window;
        // sc->mProgressDialog->GetWindow((IWindow**)&window);
        // window->AddFlags(IWindowManagerLayoutParams::FLAG_BLUR_BEHIND);

        // // display the progress dialog
        // sc->mProgressDialog->Show();

        // run the query.
        // handler->StartQuery(ADN_QUERY_TOKEN, sc, Uri::Parse("content://icc/adn"),
        //         new String[]{ADN_PHONE_NUMBER_COLUMN_NAME}, NULL, NULL, NULL);

        // if (sPreviousAdnQueryHandler != NULL) {
        //     // It is harmless to call cancel() even after the handler's gone.
        //     sPreviousAdnQueryHandler->Cancel();
        // }
        // sPreviousAdnQueryHandler = handler;
        return TRUE;
        // } catch (NumberFormatException ex) {
        //     // Ignore
        // }
    }
    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandlePinEntry(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    if ((input.StartsWith("**04") || input.StartsWith("**05")) && input.EndsWith("#")) {
        AutoPtr<IInterface> telecomManager;
        context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&telecomManager);
        Boolean result;
        ITelecomManager::Probe(telecomManager)->HandleMmi(input, &result);
        return result;
    }
    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleIMEIDisplay(
    /* [in] */ IContext* context,
    /* [in] */ const String& input,
    /* [in] */ Boolean useSystemWindow)
{
    AutoPtr<IInterface> telephonyManager;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&telephonyManager);
    ITelephonyManager* telephonyManager = ITelephonyManager::Probe(telephonyManager);
    if (telephonyManager != NULL && input.Equals(MMI_IMEI_DISPLAY)) {
        Int32 phoneType;
        telephonyManager-?GetPhoneType(&phoneType);
        if (phoneType == ITelephonyManager::PHONE_TYPE_GSM) {
            ShowIMEIPanel(context, useSystemWindow, telephonyManager);
            return TRUE;
        }
        else if (phoneType == ITelephonyManager::PHONE_TYPE_CDMA) {
            ShowMEIDPanel(context, useSystemWindow, telephonyManager);
            return TRUE;
        }
    }

    return FALSE;
}

Boolean SpecialCharSequenceMgr::HandleRegulatoryInfoDisplay(
    /* [in] */ IContext* context,
    /* [in] */ const String& input)
{
    if (input.Equals(MMI_REGULATORY_INFO_DISPLAY)) {
        Logger::D(TAG, "handleRegulatoryInfoDisplay() sending intent to settings app");
        AutoPtr<IIntent> showRegInfoIntent;
        CIntent::New(ISettings::ACTION_SHOW_REGULATORY_INFO, (IIntent**)&showRegInfoIntent);
        // try {
        ECode ec = context->StartActivity(showRegInfoIntent);
        if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "startActivity() failed: %x", ec);
        }
        // } catch (ActivityNotFoundException e) {
        //     Log.e(TAG, "startActivity() failed: " + e);
        // }
        return TRUE;
    }
    return FALSE;
}

void SpecialCharSequenceMgr::ShowIMEIPanel(
    /* [in] */ IContext* context,
    /* [in] */ Boolean useSystemWindow,
    /* [in] */ ITelephonyManager* telephonyManager)
{
    String imeiStr;
    telephonyManager->getDeviceId(&imeiStr);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::imei);
    builder->SetMessage(imeiStr);
    builder->SetPositiveButton(Elastos::R::string::ok, NULL);
    builder->SetCancelable(FALSE);
    AutoPtr<IAlertDialog> dialog;
    builder->Show((IAlertDialog**)&dialog);
}

void SpecialCharSequenceMgr::ShowMEIDPanel(
    /* [in] */ IContext* context,
    /* [in] */ Boolean useSystemWindow,
    /* [in] */ ITelephonyManager* telephonyManager)
{
    String imeiStr;
    telephonyManager->getDeviceId(&imeiStr);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::meid);
    builder->SetMessage(imeiStr);
    builder->SetPositiveButton(Elastos::R::string::ok, NULL);
    builder->SetCancelable(FALSE);
    AutoPtr<IAlertDialog> dialog;
    builder->Show((IAlertDialog**)&dialog);
}

} // Dialer
} // Droid
} // Elastos
