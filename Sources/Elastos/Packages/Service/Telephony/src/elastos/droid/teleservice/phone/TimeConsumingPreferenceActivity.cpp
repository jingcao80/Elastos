
#include "elastos/droid/teleservice/phone/TimeConsumingPreferenceActivity.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.View.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::TeleService::R;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(TimeConsumingPreferenceActivity::DismissOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode TimeConsumingPreferenceActivity::DismissOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return dialog->Dismiss();
}

CAR_INTERFACE_IMPL(TimeConsumingPreferenceActivity::DismissAndFinishOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode TimeConsumingPreferenceActivity::DismissAndFinishOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    dialog->Dismiss();
    return mHost->Finish();
}

const String TimeConsumingPreferenceActivity::TAG("TimeConsumingPreferenceActivity");

const Int32 TimeConsumingPreferenceActivity::BUSY_READING_DIALOG = 100;
const Int32 TimeConsumingPreferenceActivity::BUSY_SAVING_DIALOG = 200;

const Int32 TimeConsumingPreferenceActivity::EXCEPTION_ERROR = 300;
const Int32 TimeConsumingPreferenceActivity::RESPONSE_ERROR = 400;
const Int32 TimeConsumingPreferenceActivity::RADIO_OFF_ERROR = 500;
const Int32 TimeConsumingPreferenceActivity::FDN_CHECK_FAILURE = 600;

CAR_INTERFACE_IMPL_2(TimeConsumingPreferenceActivity, PreferenceActivity,
        ITimeConsumingPreferenceListener, IDialogInterfaceOnCancelListener)

TimeConsumingPreferenceActivity::TimeConsumingPreferenceActivity()
    : mIsForeground(FALSE)
    , DBG(IPhoneGlobals::DBG_LEVEL >= 2)
{
    mDismiss = new DismissOnClickListener();
    mDismissAndFinish = new DismissAndFinishOnClickListener(this);

    CArrayList::New((IArrayList**)&mBusyList);
}

ECode TimeConsumingPreferenceActivity::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** outdialog)
{
    VALIDATE_NOT_NULL(outdialog)

    if (id == BUSY_READING_DIALOG || id == BUSY_SAVING_DIALOG) {
        AutoPtr<IProgressDialog> dialog;
        CProgressDialog::New(this, (IProgressDialog**)&dialog);
        AutoPtr<ICharSequence> title;
        GetText(R::string::updating_title, (ICharSequence**)&title);
        IDialog::Probe(dialog)->SetTitle(title);
        dialog->SetIndeterminate(TRUE);

        switch(id) {
            case BUSY_READING_DIALOG:
            {
                IDialog::Probe(dialog)->SetCancelable(true);
                IDialog::Probe(dialog)->SetOnCancelListener(this);
                AutoPtr<ICharSequence> title;
                GetText(R::string::reading_settings, (ICharSequence**)&title);
                IAlertDialog::Probe(dialog)->SetMessage(title);
                *outdialog = IDialog::Probe(dialog);
                REFCOUNT_ADD(*outdialog)
                return NOERROR;
            }
            case BUSY_SAVING_DIALOG:
            {
                IDialog::Probe(dialog)->SetCancelable(FALSE);
                AutoPtr<ICharSequence> title;
                GetText(R::string::updating_settings, (ICharSequence**)&title);
                IAlertDialog::Probe(dialog)->SetMessage(title);
                *outdialog = IDialog::Probe(dialog);
                REFCOUNT_ADD(*outdialog)
                return NOERROR;
            }
        }
        *outdialog = NULL;
        return NOERROR;
    }

    if (id == RESPONSE_ERROR || id == RADIO_OFF_ERROR || id == EXCEPTION_ERROR
            || id == FDN_CHECK_FAILURE) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);

        Int32 msgId;
        Int32 titleId = R::string::error_updating_title;

        switch (id) {
            case RESPONSE_ERROR:
                msgId = R::string::response_error;
                builder->SetPositiveButton(R::string::close_dialog, mDismiss);
                break;
            case RADIO_OFF_ERROR:
                msgId = R::string::radio_off_error;
                // The error is not recoverable on dialog exit.
                builder->SetPositiveButton(R::string::close_dialog, mDismissAndFinish);
                break;
            case FDN_CHECK_FAILURE:
                msgId = R::string::fdn_check_failure;
                builder->SetPositiveButton(R::string::close_dialog, mDismiss);
                break;
            case EXCEPTION_ERROR:
            default:
                msgId = R::string::exception_error;
                // The error is not recoverable on dialog exit.
                builder->SetPositiveButton(R::string::close_dialog, mDismissAndFinish);
                break;
        }

        AutoPtr<ICharSequence> text;
        GetText(titleId, (ICharSequence**)&text);
        builder->SetTitle(text);
        AutoPtr<ICharSequence> text2;
        GetText(msgId, (ICharSequence**)&text2);
        builder->SetMessage(text2);
        builder->SetCancelable(FALSE);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);

        // make the dialog more obvious by blurring the background.
        AutoPtr<IWindow> window;
        IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_BLUR_BEHIND);

        *outdialog = IDialog::Probe(dialog);
        REFCOUNT_ADD(*outdialog)
        return NOERROR;
    }
    *outdialog = NULL;
    return NOERROR;
}

ECode TimeConsumingPreferenceActivity::OnResume()
{
    PreferenceActivity::OnResume();
    mIsForeground = TRUE;
    return NOERROR;
}

ECode TimeConsumingPreferenceActivity::OnPause()
{
    PreferenceActivity::OnPause();
    mIsForeground = FALSE;
    return NOERROR;
}

ECode TimeConsumingPreferenceActivity::OnStarted(
    /* [in] */ IPreference* preference,
    /* [in] */ Boolean reading)
{
    if (DBG) DumpState();
    if (DBG) {
        StringBuilder sb;
        sb += "onStarted, preference=";
        String key;
        preference->GetKey(&key);
        sb += key;
        sb += ", reading=";
        sb += reading;
        Logger::D(TAG, sb.ToString());
    }

    String key;
    preference->GetKey(&key);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(key);
    mBusyList->Add(TO_IINTERFACE(cchar));

    if (mIsForeground) {
        if (reading) {
            ShowDialog(BUSY_READING_DIALOG);
        }
        else {
            ShowDialog(BUSY_SAVING_DIALOG);
        }
    }
    return NOERROR;
}

ECode TimeConsumingPreferenceActivity::OnFinished(
    /* [in] */ IPreference* preference,
    /* [in] */ Boolean reading)
{
    if (DBG) DumpState();
    if (DBG) {
        StringBuilder sb;
        sb += "onFinished, preference=";
        String key;
        preference->GetKey(&key);
        sb += key;
        sb += ", reading=";
        sb += reading;
        Logger::D(TAG, sb.ToString());
    }

    String key;
    preference->GetKey(&key);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(key);
    mBusyList->Remove(TO_IINTERFACE(cchar));

    Boolean res;
    if (mBusyList->IsEmpty(&res), res) {
        if (reading) {
            DismissDialogSafely(BUSY_READING_DIALOG);
        }
        else {
            DismissDialogSafely(BUSY_SAVING_DIALOG);
        }
    }
    return preference->SetEnabled(TRUE);
}

ECode TimeConsumingPreferenceActivity::OnError(
    /* [in] */ IPreference* preference,
    /* [in] */ Int32 error)
{
    if (DBG) DumpState();
    if (DBG) {
        StringBuilder sb;
        sb += "onError, preference=";
        String key;
        preference->GetKey(&key);
        sb += key;
        sb += ", error=";
        sb += error;
        Logger::D(TAG, sb.ToString());
    }

    if (mIsForeground) {
        ShowDialog(error);
    }
    return preference->SetEnabled(FALSE);
}

ECode TimeConsumingPreferenceActivity::OnException(
    /* [in] */ IPreference* preference,
    /* [in] */ ICommandException* exception)
{
    assert(0);
    // Int32 error;
    // if ((exception->GetCommandError(&error), error) == ICommandExceptionError::FDN_CHECK_FAILURE) {
    //     OnError(preference, FDN_CHECK_FAILURE);
    // }
    // else {
    //     preference->SetEnabled(FALSE);
    //     OnError(preference, EXCEPTION_ERROR);
    // }
    return NOERROR;
}

ECode TimeConsumingPreferenceActivity::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    if (DBG) DumpState();
    return Finish();
}

void TimeConsumingPreferenceActivity::DismissDialogSafely(
    /* [in] */ Int32 id)
{
    // try {
    DismissDialog(id);
    // } catch (IllegalArgumentException e) {
    //     // This is expected in the case where we were in the background
    //     // at the time we would normally have shown the dialog, so we didn't
    //     // show it.
    // }
}

ECode TimeConsumingPreferenceActivity::DumpState()
{
    Logger::D(TAG, "dumpState begin");

    Int32 size;
    mBusyList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mBusyList->Get(i, (IInterface**)&obj);
        AutoPtr<ICharSequence> value = ICharSequence::Probe(obj);
        String key;
        value->ToString(&key);

        Logger::D(TAG, "mBusyList: key= %s", key.string());
    }

    Logger::D(TAG, "dumpState end");
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos