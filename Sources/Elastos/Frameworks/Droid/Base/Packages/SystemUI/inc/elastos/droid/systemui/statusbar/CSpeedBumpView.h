
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSPEEDBUMPVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSPEEDBUMPVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CSpeedBumpView.h"
#include "elastos/droid/systemui/statusbar/ExpandableView.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * The view representing the separation between important and less important notifications
 */
CarClass(CSpeedBumpView)
    , public ExpandableView
    , public ISpeedBumpView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSpeedBumpView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI IsTransparent(
        /* [out] */ Boolean* result);

    CARAPI PerformVisibilityAnimation(
        /* [in] */ Boolean nowVisible,
        /* [in] */ Int64 delay);

    /**
     * Animate the divider to a new visibility.
     *
     * @param nowVisible should it now be visible
     * @param delay the delay after the animation should start
     * @param onFinishedRunnable A runnable which should be run when the animation is
     *        finished.
     */
    CARAPI AnimateDivider(
        /* [in] */ Boolean nowVisible,
        /* [in] */ Int64 delay,
        /* [in] */ IRunnable* onFinishedRunnable);

    CARAPI SetInvisible();

    // @Override
    CARAPI PerformRemoveAnimation(
        /* [in] */ Int64 duration,
        /* [in] */ Float translationDirection,
        /* [in] */ IRunnable* onFinishedRunnable);

    // @Override
    CARAPI PerformAddAnimation(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Int32) GetInitialHeight();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    Int32 mSpeedBumpHeight;
    AutoPtr<IAlphaOptimizedView> mLine;
    Boolean mIsVisible;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSPEEDBUMPVIEW_H__
