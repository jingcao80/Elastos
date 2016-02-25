
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONHINT_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONHINT_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CBalloonHint.h"
#include "elastos/droid/os/HandlerRunnable.h"
#include "elastos/droid/widget/PopupWindow.h"

using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::PopupWindow;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * Subclass of PopupWindow used as the feedback when user presses on a soft key
 * or a candidate.
 */
CarClass(CBalloonHint)
    , public PopupWindow
    , public IBalloonHint
{
private:
    class BalloonTimer
        : public HandlerRunnable
    {
    public:
        BalloonTimer(
            /* [in] */ CBalloonHint* host);

        void StartTimer(
            /* [in] */ Int64 tm,
            /* [in] */ Int32 action,
            /* [in] */ ArrayOf<Int32>* positionInParent,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        Boolean IsPending();

        Boolean RemoveTimer();

        Int32 GetAction();

        CARAPI Run();

    public:
        static const Int32 ACTION_SHOW = 1;
        static const Int32 ACTION_HIDE = 2;
        static const Int32 ACTION_UPDATE = 3;

    private:
        /**
         * The pending action.
         */
        Int32 mAction;

        Int32 mPositionInParent[2];
        Int32 mWidth;
        Int32 mHeight;

        Boolean mTimerPending;
        CBalloonHint* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CBalloonHint();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* parent,
        /* [in] */ Int32 measureSpecMode);

    CARAPI GetContext(
        /* [out] */ IContext** ctx);

    CARAPI GetPadding(
        /* [out] */ IRect** rect);

    CARAPI SetBalloonBackground(
        /* [in] */ IDrawable* drawable);

    /**
     * Set configurations to show text label in this balloon.
     *
     * @param label The text label to show in the balloon.
     * @param textSize The text size used to show label.
     * @param textBold Used to indicate whether the label should be bold.
     * @param textColor The text color used to show label.
     * @param width The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     * @param height The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     */
    CARAPI SetBalloonConfig(
        /* [in] */ const String& label,
        /* [in] */ Float textSize,
        /* [in] */ Boolean textBold,
        /* [in] */ Int32 textColor,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Set configurations to show text label in this balloon.
     *
     * @param icon The icon used to shown in this balloon.
     * @param width The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     * @param height The desired width of the balloon. The real width is
     *        determined by the desired width and balloon's measuring
     *        specification.
     */
    CARAPI SetBalloonConfig(
        /* [in] */ IDrawable* icon,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI NeedForceDismiss(
        /* [out] */ Boolean* result);

    CARAPI GetPaddingLeft(
        /* [out] */ Int32* value);

    CARAPI GetPaddingTop(
        /* [out] */ Int32* value);

    CARAPI GetPaddingRight(
        /* [out] */ Int32* value);

    CARAPI GetPaddingBottom(
        /* [out] */ Int32* value);

    CARAPI DelayedShow(
        /* [in] */ Int64 delay,
        /* [in] */ ArrayOf<Int32>* locationInParent);

    CARAPI DelayedUpdate(
        /* [in] */ Int64 delay,
        /* [in] */ ArrayOf<Int32>* locationInParent,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI DelayedDismiss(
        /* [in] */ Int64 delay);

    CARAPI RemoveTimer();

    CARAPI SetAllowScrollingAnchorParent(
        /* [in] */ Boolean enabled);

    CARAPI GetBackground(
        /* [out] */ IDrawable** background);

    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* background);

    CARAPI GetAnimationStyle(
        /* [out] */ Int32* style);

    CARAPI SetIgnoreCheekPress();

    CARAPI SetAnimationStyle(
        /* [in] */ Int32 animationStyle);

    CARAPI GetContentView(
        /* [out] */ IView** contentView);

    CARAPI SetContentView(
        /* [in] */ IView* contentView);

    CARAPI SetTouchInterceptor(
        /* [in] */ IViewOnTouchListener* l);

    CARAPI IsFocusable(
        /* [out] */ Boolean* isFocusable);

    CARAPI  SetFocusable(
         /* [in] */ Boolean focusable);

    CARAPI GetInputMethodMode(
        /* [out] */ Int32* inputMethodMode);

    CARAPI SetInputMethodMode(
        /* [in] */ Int32 mode);

    CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    CARAPI GetSoftInputMode(
        /* [out] */ Int32* softInputMode);

    CARAPI IsTouchable(
        /* [out] */ Boolean* touchable);

    CARAPI SetTouchable(
        /* [in] */ Boolean touchable);

    CARAPI IsOutsideTouchable(
        /* [out] */ Boolean* touchable);

    CARAPI SetOutsideTouchable(
        /* [in] */ Boolean touchable);

    CARAPI IsClippingEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetClippingEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetClipToScreenEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsSplitTouchEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetSplitTouchEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsLayoutInScreenEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetLayoutInScreenEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetLayoutInsetDecor(
        /* [in] */ Boolean enabled);

    CARAPI SetWindowLayoutType(
        /* [in] */ Int32 layoutType);

    CARAPI GetWindowLayoutType(
        /* [out] */ Int32* layoutType);

    CARAPI SetTouchModal(
        /* [in] */ Boolean touchModal);

    CARAPI SetWindowLayoutMode(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI SetHeight(
        /* [in] */ Int32 height);

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI SetWidth(
        /* [in] */ Int32 width);

    CARAPI IsShowing(
        /* [out] */ Boolean* isShowing);

    CARAPI ShowAtLocation(
        /* [in] */ IView* parent,
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI ShowAtLocation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI ShowAsDropDown(
        /* [in] */ IView* anchor);

    CARAPI ShowAsDropDown(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff);

    CARAPI IsAboveAnchor(
        /* [out] */ Boolean* isAboveAnchor);

    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [out] */ Int32* maxAvailableHeight);

    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 yOffset,
        /* [out] */ Int32* maxAvailableHeight);

    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 yOffset,
        /* [in] */ Boolean ignoreBottomDecorations,
        /* [out] */ Int32* maxAvailableHeight);

    CARAPI Dismiss();

    CARAPI SetOnDismissListener(
        /* [in] */ IPopupWindowOnDismissListener* l);

    CARAPI Update();

    CARAPI Update(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI Update(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI Update(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean force);

    CARAPI Update(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI Update(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);


private:
    void SetBalloonSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

public:
    /**
     * Delayed time to show the balloon hint.
     */
    static const Int32 TIME_DELAY_SHOW;

    /**
     * Delayed time to dismiss the balloon hint.
     */
    static const Int32 TIME_DELAY_DISMISS;

private:
    /**
     * The padding information of the balloon. Because PopupWindow's background
     * can not be changed unless it is dismissed and shown again, we set the
     * real background drawable to the content view, and make the PopupWindow's
     * background transparent. So actually this padding information is for the
     * content view.
     */
    AutoPtr<IRect> mPaddingRect;

    /**
     * The context used to create this balloon hint object.
     */
    AutoPtr<IContext> mContext;

    /**
     * Parent used to show the balloon window.
     */
    AutoPtr<IView> mParent;

    /**
     * The content view of the balloon.
     */
    AutoPtr<IBalloonView> mBalloonView;

    /**
     * The measuring specification used to determine its size. Key-press
     * balloons and candidates balloons have different measuring specifications.
     */
    Int32 mMeasureSpecMode;

    /**
     * Used to indicate whether the balloon needs to be dismissed forcibly.
     */
    Boolean mForceDismiss;

    /**
     * Timer used to show/dismiss the balloon window with some time delay.
     */
    AutoPtr<BalloonTimer> mBalloonTimer;

    Int32 mParentLocationInWindow[2];
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CBALLOONHINT_H__
