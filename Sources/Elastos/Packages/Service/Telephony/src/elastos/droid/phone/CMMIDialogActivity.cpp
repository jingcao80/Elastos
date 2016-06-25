
#include "elastos/droid/phone/CMMIDialogActivity.h"
#include "elastos/droid/phone/PhoneGlobals.h"
#include "elastos/droid/phone/PhoneUtils.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.CoreLibrary.Utility.h"
#include "R.h"

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Internal::Telephony::IMmiCodeState;
using Elastos::Droid::Internal::Telephony::IMmiCodeState_PENDING;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

CMMIDialogActivity::MyHandler::MyHandler(
    /* [in] */ CMMIDialogActivity* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CMMIDialogActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IPhoneGlobals::MMI_COMPLETE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IObject::Probe(obj);
            AutoPtr<IMmiCode> code = IMmiCode::Probe(result->mResult);
            mHost->OnMMIComplete(code);
            break;
        }
        case IPhoneGlobals::MMI_CANCEL:
            mHost->OnMMICancel();
            break;
    }
    return NOERROR;
}

const String CMMIDialogActivity::TAG("CMMIDialogActivity");// = MMIDialogActivity.class.getSimpleName();

CAR_OBJECT_IMPL(CMMIDialogActivity)

CMMIDialogActivity::CMMIDialogActivity()
{
    AutoPtr<PhoneGlobals> phoneGlobals;
    PhoneGlobals::GetInstance((PhoneGlobals**)&phoneGlobals);
    phoneGlobals->GetCallManager((ICallManager**)&mCM);
    mPhone = PhoneGlobals::GetPhone();
}

ECode CMMIDialogActivity::constructor()
{
    return Activity::constructor();
}

ECode CMMIDialogActivity::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    mHandler = new MyHandler(this);
    mCM->RegisterForMmiComplete(mHandler, IPhoneGlobals::MMI_COMPLETE, NULL);

    PhoneConstantsState state;
    mCM->GetState(&state);
    if (state == PhoneConstantsState_OFFHOOK) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, Elastos::Droid::Server::Telephony::R::string::incall_status_dialed_mmi, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    ShowMMIDialog();
    return NOERROR;
}

void CMMIDialogActivity::ShowMMIDialog()
{
    AutoPtr<IList> codes;
    mPhone->GetPendingMmiCodes((IList**)&codes);
    Int32 size;
    codes->GetSize(&size);
    if (size > 0) {
        AutoPtr<IInterface> obj;
        codes->Get(0, (IInterface**)&obj);
        AutoPtr<IMmiCode> mmiCode = IMmiCode::Probe(obj);

        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> message;
        helper->Obtain(mHandler, IPhoneGlobals::MMI_CANCEL, (IMessage**)&message);
        mMMIDialog = PhoneUtils::DisplayMMIInitiate(this, mmiCode, message, mMMIDialog);
    }
    else {
        Finish();
    }
}

void CMMIDialogActivity::OnMMIComplete(
    /* [in] */ IMmiCode* mmiCode)
{
    // Check the code to see if the request is ready to
    // finish, this includes any MMI state that is not
    // PENDING.

    // if phone is a CDMA phone display feature code completed message
    Int32 phoneType;
    mPhone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        PhoneUtils::DisplayMMIComplete(mPhone, this, mmiCode, NULL, NULL);
    }
    else if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
        IMmiCodeState state;
        mmiCode->GetState(&state);
        if (state != IMmiCodeState_PENDING) {
            Logger::D(TAG, "Got MMI_COMPLETE, finishing dialog activity...");
            DismissDialogsAndFinish();
        }
    }
}

void CMMIDialogActivity::OnMMICancel()
{
    Logger::V(TAG, "onMMICancel()...");

    // First of all, cancel the outstanding MMI code (if possible.)
    PhoneUtils::CancelMmiCode(mPhone);

    // Regardless of whether the current MMI code was cancelable, the
    // PhoneApp will get an MMI_COMPLETE event very soon, which will
    // take us to the MMI Complete dialog (see
    // PhoneUtils.displayMMIComplete().)
    //
    // But until that event comes in, we *don't* want to stay here on
    // the in-call screen, since we'll be visible in a
    // partially-constructed state as soon as the "MMI Started" dialog
    // gets dismissed. So let's forcibly bail out right now.
    Logger::D(TAG, "onMMICancel: finishing InCallScreen...");
    DismissDialogsAndFinish();
}

void CMMIDialogActivity::DismissDialogsAndFinish()
{
    if (mMMIDialog != NULL) {
        IDialogInterface::Probe(mMMIDialog)->Dismiss();
    }
    if (mHandler != NULL) {
        mCM->UnregisterForMmiComplete(mHandler);
    }
    Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos