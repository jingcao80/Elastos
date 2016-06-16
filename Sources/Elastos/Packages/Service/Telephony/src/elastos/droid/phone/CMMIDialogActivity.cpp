
#include "elastos/droid/phone/CMMIDialogActivity.h"

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
            mHost->OnMMIComplete((MmiCode) ((AsyncResult) msg.obj).result);
            break;
        case IPhoneGlobals::MMI_CANCEL:
            mHost->OnMMICancel();
            break;
    }
    return NOERROR;
}

const String CMMIDialogActivity::TAG("CMMIDialogActivity");// = MMIDialogActivity.class.getSimpleName();

CAR_INTERFACE_IMPL(CMMIDialogActivity, Activity, IMMIDialogActivity)

CAR_OBJECT_IMPL(CMMIDialogActivity)

CMMIDialogActivity::CMMIDialogActivity()
{
    AutoPtr<IPhoneGlobals> phoneGlobals = PhoneGlobals::GetInstance();
    phoneGlobals->GetCallManager((ICallManager**)&mCM);
    PhoneGlobals::GetPhone((IPhone**)&mPhone);
}

ECode CMMIDialogActivity::constructor()
{
    return Activity::constructor();
}

ECode CMMIDialogActivity::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    mHandler = new MyHandler();
    mCM->RegisterForMmiComplete(mHandler, IPhoneGlobals::MMI_COMPLETE, NULL);

    PhoneConstantsState state;
    mCM->GetState(&state);
    if (state == PhoneConstantsState_OFFHOOK) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, R.string.incall_status_dialed_mmi, IToast::LENGTH_SHORT, (IToast**)&toast);
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

        AutoPtr<IMessage> message;
        Message::Obtain(mHandler, IPhoneGlobals::MMI_CANCEL, (IMessage**)&message);
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
            Logger.::D(TAG, "Got MMI_COMPLETE, finishing dialog activity...");
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
        mMMIDialog->Dismiss();
    }
    if (mHandler != NULL) {
        mCM->UnregisterForMmiComplete(mHandler);
    }
    Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos