
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSimPukView.h"
#include <elastos/droid/widget/ImageButton.h>

using Elastos::Droid::Widget::ImageButton;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays a PIN pad for entering a PUK (Pin Unlock Kode) provided by a carrier.
 */
CarClass(CKeyguardSimPukView)
    , public KeyguardPinBasedInputView
{
private:
    class StateMachine
        : public Object
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::StateMachine")

        CARAPI Next();

        CARAPI Reset();

    public:
        const Int32 ENTER_PUK = 0;
        const Int32 ENTER_PIN = 1;
        const Int32 CONFIRM_PIN = 2;
        const Int32 DONE = 3;

    private:
        Int32 state = ENTER_PUK;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI ResetState();



private:
    CARAPI_(String) GetPukPasswordErrorMessage(
        /* [in] */ Int32 attemptsRemaining);

public:
    static const String TAG = "KeyguardSimPukView";

private:
    static const String LOG_TAG = "KeyguardSimPukView";
    static const Boolean DEBUG = KeyguardConstants.DEBUG;

    AutoPtr<IProgressDialog> mSimUnlockProgressDialog;
    AutoPtr<ICheckSimPuk> mCheckSimPukThread;
    String mPukText;
    String mPinText;
    AutoPtr<IStateMachine> mStateMachine = new StateMachine();
    AutoPtr<IAlertDialog> mRemainingAttemptsDialog;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__
