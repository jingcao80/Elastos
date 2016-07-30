
#ifndef __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__

#include "elastos/droid/incallui/Presenter.h"
#include "elastos/droid/incallui/Call.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

class DialpadPresenter
    : public Presenter
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Processes the specified digit as a DTMF key, by playing the
     * appropriate DTMF tone, and appending the digit to the EditText
     * field that displays the DTMF digits sent so far.
     *
     */
    CARAPI_(void) ProcessDtmf(
      /* [in] */ Char32 c);

    /**
     * Stops the local tone based on the phone type.
     */
    CARAPI_(void) StopDtmf();

private:
    AutoPtr<Call> mCall;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__
