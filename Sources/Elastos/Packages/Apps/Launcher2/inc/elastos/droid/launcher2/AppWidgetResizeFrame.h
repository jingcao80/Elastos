#ifndef  __ELASTOS_DROID_LAUNCHER2_APPWIDGETRESIZEFRAME_H__
#define  __ELASTOS_DROID_LAUNCHER2_APPWIDGETRESIZEFRAME_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class AppWidgetResizeFrame
    : public FrameLayout
    , public IAppWidgetResizeFrame
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ AppWidgetResizeFrame* host);

        CARAPI Run();

    private:
        AppWidgetResizeFrame* mHost;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener(
            /* [in] */ AppWidgetResizeFrame* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);
    private:
        AppWidgetResizeFrame* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    AppWidgetResizeFrame();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILauncherAppWidgetHostView* widgetView,
        /* [in] */ ICellLayout* cellLayout,
        /* [in] */ IDragLayer* dragLayer);

    CARAPI BeginResizeIfPointInRegion(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    /**
     *  Here we bound the deltas such that the frame cannot be stretched beyond the extents
     *  of the CellLayout, and such that the frame's borders can't cross.
     */
    CARAPI UpdateDeltas(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    CARAPI VisualizeResizeForDelta(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    static CARAPI UpdateWidgetSizeRanges(
        /* [in] */ IAppWidgetHostView* widgetView,
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY);

    static CARAPI_(AutoPtr<IRect>) GetWidgetSizeRanges(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IRect* rect);

    /**
     * This is the final step of the resize. Here we save the new widget size and position
     * to LauncherModel and animate the resize frame.
     */
    CARAPI CommitResize();

    CARAPI OnTouchUp();

    CARAPI SnapToWidget(
        /* [in] */ Boolean animate);

private:
    /**
     *  Based on the deltas, we resize the frame, and, if needed, we resize the widget.
     */
    CARAPI VisualizeResizeForDelta(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY,
        /* [in] */ Boolean onDismiss);

    /**
     *  Based on the current deltas, we determine if and how to resize the widget.
     */
    CARAPI ResizeWidgetIfNeeded(
        /* [in] */ Boolean onDismiss);

    static CARAPI_(AutoPtr<IRect>) InitTmpRect();

public:
    AutoPtr<ArrayOf<Int32> > mDirectionVector;
    AutoPtr<ArrayOf<Int32> > mLastDirectionVector;

    const Int32 SNAP_DURATION;
    const Int32 BACKGROUND_PADDING;
    const Float DIMMED_HANDLE_ALPHA;
    const Float RESIZE_THRESHOLD;

private:
    AutoPtr<ILauncherAppWidgetHostView> mWidgetView;
    AutoPtr<ICellLayout> mCellLayout;
    AutoPtr<IDragLayer> mDragLayer;
    AutoPtr<IWorkspace> mWorkspace;
    AutoPtr<IImageView> mLeftHandle;
    AutoPtr<IImageView> mRightHandle;
    AutoPtr<IImageView> mTopHandle;
    AutoPtr<IImageView> mBottomHandle;

    Boolean mLeftBorderActive;
    Boolean mRightBorderActive;
    Boolean mTopBorderActive;
    Boolean mBottomBorderActive;

    Int32 mWidgetPaddingLeft;
    Int32 mWidgetPaddingRight;
    Int32 mWidgetPaddingTop;
    Int32 mWidgetPaddingBottom;

    Int32 mBaselineWidth;
    Int32 mBaselineHeight;
    Int32 mBaselineX;
    Int32 mBaselineY;
    Int32 mResizeMode;

    Int32 mRunningHInc;
    Int32 mRunningVInc;
    Int32 mMinHSpan;
    Int32 mMinVSpan;
    Int32 mDeltaX;
    Int32 mDeltaY;
    Int32 mDeltaXAddOn;
    Int32 mDeltaYAddOn;

    Int32 mBackgroundPadding;
    Int32 mTouchTargetWidth;

    Int32 mTopTouchRegionAdjustment;
    Int32 mBottomTouchRegionAdjustment;

    static AutoPtr<IRect> sTmpRect;

    ILauncher* mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_APPWIDGETRESIZEFRAME_H__