
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEVIEW_H__

#include "_SystemUI.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * An abstract view for expandable views.
 */
class ExpandableView
    : public FrameLayout
    , public IExpandableView
{
public:
    CAR_INTERFACE_DECL();

    ExpandableView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Sets the actual height of this notification. This is different than the laid out
     * {@link View#getHeight()}, as we want to avoid layouting during scrolling and expanding.
     *
     * @param actualHeight The height of this notification.
     * @param notifyListeners Whether the listener should be informed about the change.
     */
    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight,
        /* [in] */ Boolean notifyListeners);

    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight);

    /**
     * See {@link #setActualHeight}.
     *
     * @return The current actual height of this notification.
     */
    CARAPI GetActualHeight(
        /* [out] */ Int32* height);

    /**
     * @return The maximum height of this notification.
     */
    CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    /**
     * @return The minimum height of this notification.
     */
    CARAPI GetMinHeight(
        /* [out] */ Int32* height);

    /**
     * Sets the notification as dimmed. The default implementation does nothing.
     *
     * @param dimmed Whether the notification should be dimmed.
     * @param fade Whether an animation should be played to change the state.
     */
    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed,
        /* [in] */ Boolean fade);

    /**
     * Sets the notification as dark. The default implementation does nothing.
     *
     * @param dark Whether the notification should be dark.
     * @param fade Whether an animation should be played to change the state.
     */
    CARAPI SetDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean fade);

    /**
     * See {@link #setHideSensitive}. This is a variant which notifies this view in advance about
     * the upcoming state of hiding sensitive notifications. It gets called at the very beginning
     * of a stack scroller update such that the updated intrinsic height (which is dependent on
     * whether private or public layout is showing) gets taken into account into all layout
     * calculations.
     */
    CARAPI SetHideSensitiveForIntrinsicHeight(
        /* [in] */ Boolean hideSensitive);

    /**
     * Sets whether the notification should hide its private contents if it is sensitive.
     */
    CARAPI SetHideSensitive(
        /* [in] */ Boolean hideSensitive,
        /* [in] */ Boolean animated,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

    /**
     * @return The desired notification height.
     */
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    /**
     * Sets the amount this view should be clipped from the top. This is used when an expanded
     * notification is scrolling in the top or bottom stack.
     *
     * @param clipTopAmount The amount of pixels this view should be clipped from top.
     */
    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    CARAPI GetClipTopAmount(
        /* [out] */ Int32* amount);

    CARAPI SetOnHeightChangedListener(
        /* [in] */ IExpandableViewOnHeightChangedListener* listener);

    /**
     * @return Whether we can expand this views content.
     */
    CARAPI IsContentExpandable(
        /* [out] */ Boolean* result);

    CARAPI NotifyHeightChanged();

    CARAPI IsTransparent(
        /* [out] */ Boolean* result);

    /**
     * Perform a remove animation on this view.
     *
     * @param duration The duration of the remove animation.
     * @param translationDirection The direction value from [-1 ... 1] indicating in which the
     *                             animation should be performed. A value of -1 means that The
     *                             remove animation should be performed upwards,
     *                             such that the  child appears to be going away to the top. 1
     *                             Should mean the opposite.
     * @param onFinishedRunnable A runnable which should be run when the animation is finished.
     */
    virtual CARAPI PerformRemoveAnimation(
        /* [in] */ Int64 duration,
        /* [in] */ Float translationDirection,
        /* [in] */ IRunnable* onFinishedRunnable) = 0;

    virtual CARAPI PerformAddAnimation(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration) = 0;

    CARAPI SetBelowSpeedBump(
        /* [in] */ Boolean below);

    CARAPI OnHeightReset();

    /**
     * This method returns the drawing rect for the view which is different from the regular
     * drawing rect, since we layout all children in the {@link NotificationStackScrollLayout} at
     * position 0 and usually the translation is neglected. Since we are manually clipping this
     * view,we also need to subtract the clipTopAmount from the top. This is needed in order to
     * ensure that accessibility and focusing work correctly.
     *
     * @param outRect The (scrolled) drawing bounds of the view.
     */
    // @Override
    CARAPI GetDrawingRect(
        /* [in] */ IRect* outRect);

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual Int32 GetInitialHeight();

private:
    CARAPI_(Boolean) FilterMotionEvent(
        /* [in] */ IMotionEvent* event);

protected:
    Int32 mActualHeight;
    Int32 mClipTopAmount;

private:
    Int32 mMaxNotificationHeight;
    AutoPtr<IExpandableViewOnHeightChangedListener> mOnHeightChangedListener;
    Boolean mActualHeightInitialized;
    AutoPtr<IArrayList/*<View*/> mMatchParentViews;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_EXPANDABLEVIEW_H__
