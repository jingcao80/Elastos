
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardPINView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays a PIN pad for unlocking.
 */
CarClass(CKeyguardPINView)
    , public KeyguardPinBasedInputView
{
private:
    class MyRunnable
        : Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardPINView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardPINView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardPINView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardPINView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI GetWrongPasswordStringId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    CARAPI ResetState();

    //@Override
    CARAPI GetPasswordTextViewId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) EnableClipping(
        /* [in] */ Boolean enable);

private:
    AutoPtr<AppearAnimationUtils> mAppearAnimationUtils;
    AutoPtr<IViewGroup> mKeyguardBouncerFrame;
    AutoPtr<IViewGroup> mRow0;
    AutoPtr<IViewGroup> mRow1;
    AutoPtr<IViewGroup> mRow2;
    AutoPtr<IViewGroup> mRow3;
    AutoPtr<IView> mDivider;
    Int32 mDisappearYTranslation;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPINVIEW_H__
