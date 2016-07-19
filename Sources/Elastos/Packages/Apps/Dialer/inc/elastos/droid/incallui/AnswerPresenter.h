
#ifndef __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/Presenter.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Presenter for the Incoming call widget.
 */
class AnswerPresenter
    : public Presenter
    , public ICallUpdateListener
    , public ICallListListener
{
public:
    CAR_INTERFACE_DECL();

    AnswerPresenter();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnCallListChange(
        /* [in] */ ICallList* callList);

    // @Override
    CARAPI OnDisconnect(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallChanged(
        /* [in] */ ICall* call);

    CARAPI OnAnswer(
        /* [in] */ Int32 videoState,
        /* [in] */ IContext* context);

    /**
     * TODO: We are using reject and decline interchangeably. We should settle on
     * reject since it seems to be more prevalent.
     */
    CARAPI OnDecline();

    CARAPI OnText();

    CARAPI_(void) RejectCallWithMessage(
        /* [in] */ const String& message);

    CARAPI OnDismissDialog();

private:
    CARAPI_(void) ProcessIncomingCall(
        /* [in] */ ICall* call);

    CARAPI_(void) ProcessVideoUpgradeRequestCall(
        /* [in] */ ICall* call);

    CARAPI_(void) ConfigureAnswerTargetsForSms(
        /* [in] */ ICall* call,
        /* [in] */ IList* textMsgs);

private:
    static const String TAG;

    String mCallId;
    AutoPtr<Call> mCall;
    Boolean mHasTextMessages;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__
