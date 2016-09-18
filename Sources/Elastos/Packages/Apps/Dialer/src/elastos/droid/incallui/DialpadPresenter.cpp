
#include "elastos/droid/incallui/DialpadPresenter.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::InCallUI::EIID_IInCallStateListener;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL(DialpadPresenter, Presenter, IInCallStateListener)

ECode DialpadPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiReady(ui));
    // register for call state changes last
    InCallPresenter::GetInstance()->AddListener(this);
    mCall = CallList::GetInstance()->GetOutgoingOrActive();
    return NOERROR;
}

ECode DialpadPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiUnready(ui));
    InCallPresenter::GetInstance()->RemoveListener(this);
    return NOERROR;
}

ECode DialpadPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    mCall = ((CallList*)callList)->GetOutgoingOrActive();
    Logger::D("DialpadPresenter", "DialpadPresenter mCall = %s", TO_CSTR(mCall));
    return NOERROR;
}

void DialpadPresenter::ProcessDtmf(
  /* [in] */ Char32 c)
{
    Logger::D("DialpadPresenter", "Processing dtmf key %c", c);
    // if it is a valid key, then update the display and send the dtmf tone.
    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    Boolean is12Key;
    if ((utils->Is12Key(c, &is12Key), is12Key) && mCall != NULL) {
        Logger::D("DialpadPresenter", "updating display and sending dtmf tone for '%c'", c);

        // Append this key to the "digits" widget.
        AutoPtr<IUi> ui;
        GetUi((IUi**)&ui);
        IDialpadUi::Probe(ui)->AppendDigitsToField(c);
        // Plays the tone through Telecomm.
        TelecomAdapter::GetInstance()->PlayDtmfTone(mCall->GetId(), c);
    }
    else {
        Logger::D("DialpadPresenter", "ignoring dtmf request for '%c'", c);
    }
}

void DialpadPresenter::StopDtmf()
{
    if (mCall != NULL) {
        Logger::D("DialpadPresenter", "stopping remote tone");
        TelecomAdapter::GetInstance()->StopDtmfTone(mCall->GetId());
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
