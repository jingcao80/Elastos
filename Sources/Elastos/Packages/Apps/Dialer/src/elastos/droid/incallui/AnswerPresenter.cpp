
#include "elastos/droid/incallui/AnswerPresenter.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/CAnswerFragment.h"
#include <elastos/utility/logging/Logger.h>

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
    /* [in] */ ICall* call)
{
    Log.d(this, "onCallStateChange() " + call + " " + this);
    if (call.getState() != Call.State.INCOMING) {
        // Stop listening for updates.
        CallList.getInstance().removeCallUpdateListener(mCallId, this);

        getUi().showAnswerUi(false);

        // mCallId will hold the state of the call. We don't clear the mCall variable here as
        // it may be useful for sending text messages after phone disconnects.
        mCallId = null;
        mHasTextMessages = false;
    } else if (!mHasTextMessages) {
        final List<String> textMsgs = CallList.getInstance().getTextResponses(call.getId());
        if (textMsgs != null) {
            configureAnswerTargetsForSms(call, textMsgs);
        }
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
