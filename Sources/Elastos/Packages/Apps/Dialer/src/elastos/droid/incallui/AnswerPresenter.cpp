
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/incallui/AnswerPresenter.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/CAnswerFragment.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const String AnswerPresenter::TAG("AnswerPresenter");

CAR_INTERFACE_IMPL_2(AnswerPresenter, Presenter, ICallUpdateListener, ICallListListener);

AnswerPresenter::AnswerPresenter()
    : mHasTextMessages(FALSE)
{}

ECode AnswerPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiReady(ui));

    AutoPtr<CallList> calls = CallList::GetInstance();
    AutoPtr<Call> call = calls->GetIncomingCall();
    if (call != NULL) {
        ProcessIncomingCall(call);
    }
    call = calls->GetVideoUpgradeRequestCall();
    if (call != NULL) {
        ProcessVideoUpgradeRequestCall(call);
    }

    // Listen for incoming calls.
    calls->AddListener(this);
    return NOERROR;
}

ECode AnswerPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiUnready(ui));

    CallList::GetInstance()->RemoveListener(this);

    // This is necessary because the activity can be destroyed while an incoming call exists.
    // This happens when back button is pressed while incoming call is still being shown.
    if (!mCallId.IsNull()) {
        CallList::GetInstance()->RemoveCallUpdateListener(mCallId, this);
    }
    return NOERROR;
}

ECode AnswerPresenter::OnCallListChange(
    /* [in] */ ICallList* callList)
{
    // no-op
    return NOERROR;
}

ECode AnswerPresenter::OnDisconnect(
    /* [in] */ ICall* call)
{
    // no-op
    return NOERROR;
}

ECode AnswerPresenter::OnIncomingCall(
    /* [in] */ ICall* call)
{
    // TODO: Ui is being destroyed when the fragment detaches.  Need clean up step to stop
    // getting updates here.
    Logger::D(TAG, "onIncomingCall: %s", TO_CSTR(this));
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    if (ui != NULL) {
        if (!((Call*)call)->GetId().Equals(mCallId)) {
            // A new call is coming in.
            ProcessIncomingCall(call);
        }
    }
    return NOERROR;
}

void AnswerPresenter::ProcessIncomingCall(
    /* [in] */ ICall* call)
{
    mCall = (Call*)call;
    mCallId = mCall->GetId();

    // Listen for call updates for the current call.
    CallList::GetInstance()->AddCallUpdateListener(mCallId, this);

    Logger::D(TAG, "Showing incoming for call id: %s %s", mCallId.string(), TO_CSTR(this));
    AutoPtr<IList> textMsgs = CallList::GetInstance()->GetTextResponses(mCallId);
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    IAnswerUi::Probe(ui)->ShowAnswerUi(TRUE);
    ConfigureAnswerTargetsForSms(call, textMsgs);
}

void AnswerPresenter::ProcessVideoUpgradeRequestCall(
    /* [in] */ ICall* call)
{
    mCall = (Call*)call;
    mCallId = mCall->GetId();

    // Listen for call updates for the current call.
    CallList::GetInstance()->AddCallUpdateListener(mCallId, this);
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    IAnswerUi::Probe(ui)->ShowAnswerUi(TRUE);

    IAnswerUi::Probe(ui)->ShowTargets(CAnswerFragment::TARGET_SET_FOR_VIDEO_UPGRADE_REQUEST);
}

ECode AnswerPresenter::OnCallChanged(
    /* [in] */ ICall* _call)
{
    Call* call = (Call*)_call;
    Logger::D("AnswerPresenter", "onCallStateChange() %s %s", TO_CSTR(call), TO_CSTR(this));
    if (call->GetState() != Call::State::INCOMING) {
        // Stop listening for updates.
        CallList::GetInstance()->RemoveCallUpdateListener(mCallId, this);

        AutoPtr<IUi> ui;
        GetUi((IUi**)&ui);
        IAnswerUi::Probe(ui)->ShowAnswerUi(FALSE);

        // mCallId will hold the state of the call. We don't clear the mCall variable here as
        // it may be useful for sending text messages after phone disconnects.
        mCallId = NULL;
        mHasTextMessages = FALSE;
    }
    else if (!mHasTextMessages) {
        AutoPtr<IList> textMsgs = CallList::GetInstance()->GetTextResponses(call->GetId());
        if (textMsgs != NULL) {
            ConfigureAnswerTargetsForSms(call, textMsgs);
        }
    }
    return NOERROR;
}

ECode AnswerPresenter::OnAnswer(
    /* [in] */ Int32 videoState,
    /* [in] */ IContext* context)
{
    if (mCallId.IsNull()) {
        return NOERROR;
    }

    Logger::D("AnswerPresenter", "onAnswer %s", mCallId.string());
    if (mCall->GetSessionModificationState()
            == Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST) {
        InCallPresenter::GetInstance()->AcceptUpgradeRequest(context);
    }
    else {
        TelecomAdapter::GetInstance()->AnswerCall(mCall->GetId(), videoState);
    }
    return NOERROR;
}

ECode AnswerPresenter::OnDecline()
{
    Logger::D("AnswerPresenter", "onDecline %s", mCallId.string());
    TelecomAdapter::GetInstance()->RejectCall(mCall->GetId(), FALSE, String(NULL));
    return NOERROR;
}

ECode AnswerPresenter::OnText()
{
    AutoPtr<IUi> ui;
    GetUi((IUi**)&ui);
    if (ui != NULL) {
        IAnswerUi::Probe(ui)->ShowMessageDialog();
    }
    return NOERROR;
}

void AnswerPresenter::RejectCallWithMessage(
    /* [in] */ const String& message)
{
    Logger::D("AnswerPresenter", "sendTextToDefaultActivity()...");
    TelecomAdapter::GetInstance()->RejectCall(mCall->GetId(), TRUE, message);

    OnDismissDialog();
}

ECode AnswerPresenter::OnDismissDialog()
{
    InCallPresenter::GetInstance()->OnDismissDialog();
    return NOERROR;
}

void AnswerPresenter::ConfigureAnswerTargetsForSms(
    /* [in] */ ICall* call,
    /* [in] */ IList* textMsgs)
{
    AutoPtr<IUi> _ui;
    GetUi((IUi**)&_ui);
    IAnswerUi* ui = IAnswerUi::Probe(_ui);
    AutoPtr<IContext> context;
    ui->GetContext((IContext**)&context);

    mHasTextMessages = textMsgs != NULL;
    Boolean withSms = ((Call*)call)->Can(IPhoneCapabilities::RESPOND_VIA_TEXT) && mHasTextMessages;
    if (((Call*)call)->IsVideoCall(context)) {
        if (withSms) {
            ui->ShowTargets(CAnswerFragment::TARGET_SET_FOR_VIDEO_WITH_SMS);
            ui->ConfigureMessageDialog(textMsgs);
        }
        else {
            ui->ShowTargets(CAnswerFragment::TARGET_SET_FOR_VIDEO_WITHOUT_SMS);
        }
    }
    else {
        if (withSms) {
            ui->ShowTargets(CAnswerFragment::TARGET_SET_FOR_AUDIO_WITH_SMS);
            ui->ConfigureMessageDialog(textMsgs);
        }
        else {
            ui->ShowTargets(CAnswerFragment::TARGET_SET_FOR_AUDIO_WITHOUT_SMS);
        }
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
