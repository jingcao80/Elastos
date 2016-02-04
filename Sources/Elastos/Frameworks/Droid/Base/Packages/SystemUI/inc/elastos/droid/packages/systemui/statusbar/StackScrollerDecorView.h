
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTACKSCROLLERDECORVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTACKSCROLLERDECORVIEW_H__

#include "elastos/droid/packages/systemui/statusbar/ExpandableView.h"


namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

/**
 * A common base class for all views in the notification stack scroller which don't have a
 * background.
 */
class StackScrollerDecorView
    : public ExpandableView
    , public IStackScrollerDecorView
{
private:
    class ViewRunnable : public Runnable
    {
    public:
        ViewRunnable(
            /* [in] */ StackScrollerDecorView* host,
            /* [in] */ IRunnable* onFinishedRunnable);

        // @Override
        CARAPI Run();

    private:
        StackScrollerDecorView* mHost;
        AutoPtr<IRunnable> mOnFinishedRunnable;
    };

public:
    CAR_INTERFACE_DECL();

    StackScrollerDecorView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI IsTransparent(
        /* [out] */ Boolean* result);

    CARAPI PerformVisibilityAnimation(
        /* [in] */ Boolean nowVisible);

    CARAPI PerformVisibilityAnimation(
        /* [in] */ Boolean nowVisible,
        /* [in] */ IRunnable* onFinishedRunnable);

    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

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

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

    CARAPI CancelAnimation();

    CARAPI WillBeGone(
        /* [out] */ Boolean* beGone);

    CARAPI SetWillBeGone(
        /* [in] */ Boolean willBeGone);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(AutoPtr<IView>) FindContentView() = 0;

private:
    /**
     * Animate the text to a new visibility.
     *
     * @param nowVisible should it now be visible
     * @param onFinishedRunnable A runnable which should be run when the animation is
     *        finished.
     */
    CARAPI_(void) AnimateText(
        /* [in] */ Boolean nowVisible,
        /* [in] */ IRunnable* onFinishedRunnable);

protected:
    AutoPtr<IView> mContent;

private:
    Boolean mIsVisible;
    Boolean mAnimating;
    Boolean mWillBeGone;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTACKSCROLLERDECORVIEW_H__
