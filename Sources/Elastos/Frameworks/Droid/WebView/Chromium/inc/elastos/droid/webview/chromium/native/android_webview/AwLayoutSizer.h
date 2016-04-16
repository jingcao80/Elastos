#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWLAYOUTSIZER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWLAYOUTSIZER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// import android.view.View.MeasureSpec;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Helper methods used to manage the layout of the View that contains AwContents.
 */
class AwLayoutSizer
    :public Object
{
public:
    /**
     * Delegate interface through which the AwLayoutSizer communicates with the view it's sizing.
     */
    class Delegate
    :public Object
    {
    public:
        virtual CARAPI_(void) RequestLayout() = 0;

        virtual CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight) = 0;

        virtual CARAPI_(Boolean) IsLayoutParamsHeightWrapContent() = 0;

        virtual CARAPI_(void) SetForceZeroLayoutHeight(
            /* [in] */ Boolean forceZeroHeight) = 0;
    };

public:
    /**
     * Default constructor. Note: both setDelegate and setDIPScale must be called before the class
     * is ready for use.
     */
    AwLayoutSizer();

    CARAPI_(void) SetDelegate(
        /* [in] */ Delegate* delegate);

    CARAPI_(void) SetDIPScale(
        /* [in] */ Double dipScale);

    /**
     * Postpone requesting layouts till unfreezeLayoutRequests is called.
     */
    CARAPI_(void) FreezeLayoutRequests();

    /**
     * Stop postponing layout requests and request layout if such a request would have been made
     * had the freezeLayoutRequests method not been called before.
     */
    CARAPI_(void) UnfreezeLayoutRequests();

    /**
     * Update the contents size.
     * This should be called whenever the content size changes (due to DOM manipulation or page
     * load, for example).
     * The width and height should be in CSS pixels.
     */
    CARAPI_(void) OnContentSizeChanged(
        /* [in] */ Int32 widthCss,
        /* [in] */ Int32 heightCss);

    /**
     * Update the contents page scale.
     * This should be called whenever the content page scale factor changes (due to pinch zoom, for
     * example).
     */
    CARAPI_(void) OnPageScaleChanged(
        /* [in] */ Float pageScaleFactor);

    /**
     * Calculate the size of the view.
     * This is designed to be used to implement the android.view.View#onMeasure() method.
     */
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * Notify the AwLayoutSizer that the size of the view has changed.
     * This should be called by the Android view system after onMeasure if the view's size has
     * changed.
     */
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    /**
     * Notify the AwLayoutSizer that the layout pass requested via Delegate.requestLayout has
     * completed.
     * This should be called after onSizeChanged regardless of whether the size has changed or not.
     */
    CARAPI_(void) OnLayoutChange();

private:
    CARAPI_(void) DoUpdate(
        /* [in] */ Int32 widthCss,
        /* [in] */ Int32 heightCss,
        /* [in] */ Float pageScaleFactor);

    // This needs to be called every time either the physical size of the view is changed or layout
    // params are updated.
    CARAPI_(void) UpdateLayoutSettings();

private:
    // These are used to prevent a re-layout if the content size changes within a dimension that is
    // fixed by the view system.
    Boolean mWidthMeasurementIsFixed;
    Boolean mHeightMeasurementIsFixed;

    // Size of the rendered content, as reported by native.
    Int32 mContentHeightCss;
    Int32 mContentWidthCss;

    // Page scale factor. This is set to zero initially so that we don't attempt to do a layout if
    // we get the content size change notification first and a page scale change second.
    Float mPageScaleFactor;

    // Whether to postpone layout requests.
    Boolean mFreezeLayoutRequests;
    // Did we try to request a layout since the last time mPostponeLayoutRequests was set to true.
    Boolean mFrozenLayoutRequestPending;

    Double mDIPScale;

    // Was our height larger than the AT_MOST constraint the last time onMeasure was called?
    Boolean mHeightMeasurementLimited;
    // If mHeightMeasurementLimited is true then this contains the height limit.
    Int32 mHeightMeasurementLimit;

    // Callback object for interacting with the View.
    AutoPtr<Delegate> mDelegate;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWLAYOUTSIZER_H__
