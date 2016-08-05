
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETCAROUSEL_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETCAROUSEL_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardWidgetCarousel.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetPager.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardWidgetCarousel)
    , public KeyguardWidgetPager
{
private:
    class MyInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("CKeyguardWidgetCarousel::MyInterpolator")

        CAR_INTERFACE_DECL()

        MyInterpolator();

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<IInterpolator> mInternal;
        Float mFactor;
    };

    class MyInterpolator2
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        TO_STRING_IMPL("CKeyguardWidgetCarousel::MyInterpolator2")

        CAR_INTERFACE_DECL()

        MyInterpolator2();

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<IInterpolator> mInternal;
        Float mFactor;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardWidgetCarousel();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI GetAlphaForPage(
        /* [in] */ Int32 screenCenter,
        /* [in] */ Int32 index,
        /* [in] */ Boolean showSidePages,
        /* [out] */ Float* page);

    CARAPI GetOutlineAlphaForPage(
        /* [in] */ Int32 screenCenter,
        /* [in] */ Int32 index,
        /* [in] */ Boolean showSidePages,
        /* [out] */ Float* page);

    CARAPI ShowInitialPageHints();

    CARAPI AnimatePagesToNeutral();

    CARAPI AnimatePagesToCarousel();

protected:
    CARAPI GetMaxScrollProgress(
        /* [out] */ Float* result);

    //@Override
    CARAPI ScreenScrolled(
        /* [in] */ Int32 screenCenter);

    CARAPI ReorderStarting();

    CARAPI ZoomIn(
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnEndReordering();

private:
    CARAPI_(void) UpdatePageAlphaValues(
        /* [in] */ Int32 screenCenter);

    CARAPI_(void) GetTransformForPage(
        /* [in] */ Int32 screenCenter,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Float>* transform);

protected:
    AutoPtr<IAnimatorSet> mChildrenTransformsAnimator;

private:
    Float mAdjacentPagesAngle;
    static Float MAX_SCROLL_PROGRESS;
    static Float CAMERA_DISTANCE;
    AutoPtr<ArrayOf<Float> > mTmpTransform;

    AutoPtr<IInterpolator> mFastFadeInterpolator;

    AutoPtr<IInterpolator> mSlowFadeInterpolator;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETCAROUSEL_H__
