
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardWidgetFrame.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardWidgetFrame)
    , public FrameLayout
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardWidgetFrame::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardWidgetFrame* host)
           : mHost(host)
        {}

        //@Override
        CARAPI OnBootCompleted();

    private:
        CKeyguardWidgetFrame* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardWidgetFrame();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetIsHoveringOverDeleteDropTarget(
        /* [in] */ Boolean isHovering);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    //@Override
    CARAPI CancelLongPress();

    /**
     * Because this view has fading outlines, it is essential that we enable hardware
     * layers on the content (child) so that updating the alpha of the outlines doesn't
     * result in the content layer being recreated.
     */
    CARAPI EnableHardwareLayersForContent();

    /**
     * Because this view has fading outlines, it is essential that we enable hardware
     * layers on the content (child) so that updating the alpha of the outlines doesn't
     * result in the content layer being recreated.
     */
    CARAPI DisableHardwareLayersForContent();

    CARAPI GetContent(
        /* [out] */ IView** view);

    CARAPI GetContentAppWidgetId(
        /* [out] */ Int32* id);

    CARAPI GetBackgroundAlpha(
        /* [out] */ Float* res);

    CARAPI SetBackgroundAlphaMultiplier(
        /* [in] */ Float multiplier);

    CARAPI GetBackgroundAlphaMultiplier(
        /* [out] */ Float* res);

    CARAPI SetBackgroundAlpha(
        /* [in] */ Float alpha);

    CARAPI GetContentAlpha(
        /* [out] */ Float* res);

    CARAPI SetContentAlpha(
        /* [in] */ Float alpha);

    CARAPI SetMaxChallengeTop(
        /* [in] */ Int32 top);

    CARAPI IsSmall(
        /* [out] */ Boolean* result);

    CARAPI AdjustFrame(
        /* [in] */ Int32 challengeTop);

    CARAPI ShrinkWidget(
        /* [in] */ Boolean alsoShrinkFrame);

    CARAPI GetSmallFrameHeight(
        /* [out] */ Int32* height);

    CARAPI SetWidgetLockedSmall(
        /* [in] */ Boolean locked);

    CARAPI ResetSize();

    CARAPI SetFrameHeight(
        /* [in] */ Int32 height);

    CARAPI HideFrame(
        /* [in] */ IInterface* caller);

    CARAPI ShowFrame(
        /* [in] */ IInterface* caller);

    CARAPI FadeFrame(
        /* [in] */ IInterface* caller,
        /* [in] */ Boolean takeControl,
        /* [in] */ Float alpha,
        /* [in] */ Int32 duration);

    CARAPI SetOverScrollAmount(
        /* [in] */ Float r,
        /* [in] */ Boolean left);

    CARAPI OnActive(
        /* [in] */ Boolean isActive);

    CARAPI OnUserInteraction(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnBouncerShowing(
        /* [in] */ Boolean showing);

    CARAPI SetWorkerHandler(
        /* [in] */ IHandler* workerHandler);

    CARAPI GetWorkerHandler(
        /* [out] */ IHandler** handler);

protected:
    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnAttachedToWindow();

    CARAPI DrawBg(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) DrawGradientOverlay(
        /* [in] */ ICanvas* c);

    CARAPI_(void) DrawHoveringOverDeleteOverlay(
        /* [in] */ ICanvas* c);

    /**
     * Depending on whether the security is up, the widget size needs to change
     *
     * @param height The height of the widget, -1 for full height
     */
    CARAPI_(void) SetWidgetHeight(
        /* [in] */ Int32 height);

    CARAPI_(void) UpdateGradient();

    CARAPI_(void) PerformAppWidgetSizeCallbacksIfNecessary();

private:
    static AutoPtr<IPorterDuffXfermode> sAddBlendMode;

    static const Float OUTLINE_ALPHA_MULTIPLIER;
    static const Int32 HOVER_OVER_DELETE_DROP_TARGET_OVERLAY_COLOR;

    // Temporarily disable this for the time being until we know why the gfx is messing up
    static const Boolean ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY;

    Int32 mGradientColor;
    AutoPtr<ILinearGradient> mForegroundGradient;
    AutoPtr<ILinearGradient> mLeftToRightGradient;
    AutoPtr<ILinearGradient> mRightToLeftGradient;
    AutoPtr<IPaint> mGradientPaint;
    Boolean mLeftToRight;

    Float mOverScrollAmount;
    AutoPtr<IRect> mForegroundRect;
    Int32 mForegroundAlpha;
    AutoPtr<ICheckLongPressHelper> mLongPressHelper;
    AutoPtr<IAnimator> mFrameFade;
    Boolean mIsSmall;
    AutoPtr<IHandler> mWorkerHandler;

    Float mBackgroundAlpha;
    Float mContentAlpha;
    Float mBackgroundAlphaMultiplier;
    AutoPtr<IDrawable> mBackgroundDrawable;
    AutoPtr<IRect> mBackgroundRect;

    // These variables are all needed in order to size things properly before we're actually
    // measured.
    Int32 mSmallWidgetHeight;
    Int32 mSmallFrameHeight;
    Boolean mWidgetLockedSmall;
    Int32 mMaxChallengeTop;
    Int32 mFrameStrokeAdjustment;
    Boolean mPerformAppWidgetSizeUpdateOnBootComplete;

    // This will hold the width value before we've actually been measured
    Int32 mFrameHeight;

    Boolean mIsHoveringOverDeleteDropTarget;

    // Multiple callers may try and adjust the alpha of the frame. When a caller shows
    // the outlines, we give that caller control, and nobody else can fade them out.
    // This prevents animation conflicts.
    Object mBgAlphaController;

    AutoPtr<KeyguardUpdateMonitorCallback> mUpdateMonitorCallbacks;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__
