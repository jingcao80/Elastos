
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/ConferenceManagerPresenter.h"
#include "elastos/droid/incallui/InCallPresenter.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

const Int32 ConferenceManagerPresenter::MAX_CALLERS_IN_CONFERENCE = 5;

ECode ConferenceManagerPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiReady(ui));

    // register for call state changes last
    InCallPresenter::GetInstance()->AddListener(this);
    return NOERROR;
}

ECode ConferenceManagerPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiUnready(ui));

    InCallPresenter::GetInstance()->RemoveListener(this);
    return NOERROR;
}

ECode ConferenceManagerPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    AutoPtr<IUi> _ui;
    GetUi((IUi**)&_ui);
    IConferenceManagerUi* ui = IConferenceManagerUi::Probe(_ui);
    Boolean visible;
    if (ui->IsFragmentVisible(&visible), visible) {
        Logger::V("ConferenceManagerPresenter", "onStateChange %d", newState);
        if (newState == InCallState_INCALL) {
            AutoPtr<Call> call = ((CallList*)callList)->GetActiveOrBackgroundCall();
            if (call != NULL && call->IsConferenceCall()) {
                Logger::V("ConferenceManagerPresenter", "Number of existing calls is " +
                        String.valueOf(call.getChildCallIds().size()));
                update(callList);
            }
            else {
                getUi().setVisible(false);
            }
        }
        else {
            getUi().setVisible(false);
        }
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
