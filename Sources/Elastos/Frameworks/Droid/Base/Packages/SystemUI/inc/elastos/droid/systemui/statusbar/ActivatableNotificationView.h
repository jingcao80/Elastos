
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ACTIVATABLENOTIFICATIONVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ACTIVATABLENOTIFICATIONVIEW_H__

#include "elastos/droid/systemui/statusbar/ExpandableOutlineView.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Graphics::IPorterDuffColorFilter;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Base class for both {@link ExpandableNotificationRow} and {@link NotificationOverflowContainer}
 * to implement dimming/activating on Keyguard for the double-tap gesture
 */
class ActivatableNotificationView
    : public ExpandableOutlineView
    , public IActivatableNotificationView
{
private:
    class HostRunnable: public Runnable
    {
    public:
        HostRunnable(
            /* [in] */ ActivatableNotificationView* host);

        // @Override
        CARAPI Run();

    private:
        ActivatableNotificationView* mHost;
    };

    class InvisibleAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        InvisibleAnimatorListenerAdapter(
            /* [in] */ ActivatableNotificationView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        ActivatableNotificationView* mHost;
    };

    class VisibleAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        VisibleAnimatorListenerAdapter(
            /* [in] */ ActivatableNotificationView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        ActivatableNotificationView* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener(
            /* [in] */ ActivatableNotificationView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        ActivatableNotificationView* mHost;
    };

    class AnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter3(
            /* [in] */ ActivatableNotificationView* host,
            /* [in] */ IRunnable* runnable);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        ActivatableNotificationView* mHost;
        AutoPtr<IRunnable> mRunnable;
        Boolean mWasCancelled;
    };

public:
    CAR_INTERFACE_DECL()

    ActivatableNotificationView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Cancels the hotspot and makes the notification inactive.
     */
    CARAPI MakeInactive(
        /* [in] */ Boolean animate);

    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed,
        /* [in] */ Boolean fade);

    CARAPI SetDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean fade);

    CARAPI SetShowingLegacyBackground(
        /* [in] */ Boolean showing);

    // @Override
    CARAPI SetBelowSpeedBump(
        /* [in] */ Boolean below);

    /**
     * Sets the tint color of the background
     */
    CARAPI SetTintColor(
        /* [in] */ Int32 color);

    // @Override
    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight,
        /* [in] */ Boolean notifyListeners);

    // @Override
    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    // @Override
    CARAPI PerformRemoveAnimation(
        /* [in] */ Int64 duration,
        /* [in] */ Float translationDirection,
        /* [in] */ IRunnable* onFinishedRunnable);

    // @Override
    CARAPI PerformAddAnimation(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

    CARAPI SetOnActivatedListener(
        /* [in] */ IActivatableNotificationViewOnActivatedListener* onActivatedListener);

    CARAPI Reset();

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI DrawableStateChanged();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Boolean) HandleTouchEventDimmed(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) MakeActive();

    CARAPI_(void) StartActivateAnimation(
        /* [in] */ Boolean reverse);

    CARAPI_(Boolean) IsWithinTouchSlop(
        /* [in] */ IMotionEvent* event);

    static CARAPI_(AutoPtr<IPaint>) CreateDarkPaint();

    CARAPI_(void) UpdateBackgroundTint();

    CARAPI_(void) FadeBackground();

    CARAPI_(void) UpdateBackground();

    CARAPI_(void) CancelFadeAnimations();

    CARAPI_(void) StartAppearAnimation(
        /* [in] */ Boolean isAppearing,
        /* [in] */ Float translationDirection,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration,
        /* [in] */ IRunnable* onFinishedRunnable);

    CARAPI_(void) UpdateAppearRect();

    CARAPI_(void) UpdateAppearAnimationAlpha();

    CARAPI_(Int32) GetBackgroundColor();

    CARAPI_(Int32) GetRippleColor();

    /**
     * When we draw the appear animation, we render the view in a bitmap and render this bitmap
     * as a shader of a rect. This call creates the Bitmap and switches the drawing mode,
     * such that the normal drawing of the views does not happen anymore.
     *
     * @param enable Should it be enabled.
     */
    CARAPI_(void) EnableAppearDrawing(
        /* [in] */ Boolean enable);

    CARAPI_(void) DrawAppearRect(
        /* [in] */ ICanvas* canvas);

private:
    static const Int64 DOUBLETAP_TIMEOUT_MS;
    static const Int32 BACKGROUND_ANIMATION_LENGTH_MS;
    static const Int32 ACTIVATE_ANIMATION_LENGTH;

    /**
     * The amount of width, which is kept in the end when performing a disappear animation (also
     * the amount from which the horizontal appearing begins)
     */
    static const Float HORIZONTAL_COLLAPSED_REST_PARTIAL;

    /**
     * At which point from [0,1] does the horizontal collapse animation end (or start when
     * expanding)? 1.0 meaning that it ends immediately and 0.0 that it is continuously animated.
     */
    static const Float HORIZONTAL_ANIMATION_END;

    /**
     * At which point from [0,1] does the alpha animation end (or start when
     * expanding)? 1.0 meaning that it ends immediately and 0.0 that it is continuously animated.
     */
    static const Float ALPHA_ANIMATION_END;

    /**
     * At which point from [0,1] does the horizontal collapse animation start (or start when
     * expanding)? 1.0 meaning that it starts immediately and 0.0 that it is animated at all.
     */
    static const Float HORIZONTAL_ANIMATION_START;

    /**
     * At which point from [0,1] does the vertical collapse animation start (or end when
     * expanding) 1.0 meaning that it starts immediately and 0.0 that it is animated at all.
     */
    static const Float VERTICAL_ANIMATION_START;

    static AutoPtr<IInterpolator> ACTIVATE_INVERSE_INTERPOLATOR;
    static AutoPtr<IInterpolator> ACTIVATE_INVERSE_ALPHA_INTERPOLATOR;

    AutoPtr<IRunnable> mTapTimeoutRunnable;
    Int32 mTintedRippleColor;
    Int32 mLowPriorityRippleColor;
    Int32 mNormalRippleColor;

    Boolean mDimmed;
    Boolean mDark;
    AutoPtr<IPaint> mDarkPaint;

    Int32 mBgTint;
    Int32 mRoundedRectCornerRadius;

    /**
     * Flag to indicate that the notification has been touched once and the second touch will
     * click it.
     */
    Boolean mActivated;

    Float mDownX;
    Float mDownY;
    Float mTouchSlop;

    AutoPtr<IActivatableNotificationViewOnActivatedListener> mOnActivatedListener;

    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mSlowOutFastInInterpolator;
    AutoPtr<IInterpolator> mSlowOutLinearInInterpolator;
    AutoPtr<IInterpolator> mLinearInterpolator;
    AutoPtr<IInterpolator> mCurrentAppearInterpolator;
    AutoPtr<IInterpolator> mCurrentAlphaInterpolator;

    AutoPtr<INotificationBackgroundView> mBackgroundNormal;
    AutoPtr<INotificationBackgroundView> mBackgroundDimmed;
    AutoPtr<IObjectAnimator> mBackgroundAnimator;
    AutoPtr<IRectF> mAppearAnimationRect;
    AutoPtr<IPorterDuffColorFilter> mAppearAnimationFilter;
    Float mAnimationTranslationY;
    Boolean mDrawingAppearAnimation;
    AutoPtr<IPaint> mAppearPaint;
    AutoPtr<IValueAnimator> mAppearAnimator;
    Float mAppearAnimationFraction;
    Float mAppearAnimationTranslation;
    Boolean mShowingLegacyBackground;
    Int32 mLegacyColor;
    Int32 mNormalColor;
    Int32 mLowPriorityColor;
    Boolean mIsBelowSpeedBump;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ACTIVATABLENOTIFICATIONVIEW_H__
