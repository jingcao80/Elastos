
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwLayoutSizer.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Default constructor. Note: both setDelegate and setDIPScale must be called before the class
 * is ready for use.
 */
AwLayoutSizer::AwLayoutSizer()
    : mWidthMeasurementIsFixed(FALSE)
    , mHeightMeasurementIsFixed(FALSE)
    , mContentHeightCss(0)
    , mContentWidthCss(0)
    , mPageScaleFactor(0.0f)
    , mFreezeLayoutRequests(FALSE)
    , mFrozenLayoutRequestPending(FALSE)
    , mDIPScale(0.0)
    , mHeightMeasurementLimited(FALSE)
    , mHeightMeasurementLimit(0)
{
}

void AwLayoutSizer::SetDelegate(
    /* [in] */ Delegate* delegate)
{
    mDelegate = delegate;
}

void AwLayoutSizer::SetDIPScale(
    /* [in] */ Double dipScale)
{
    mDIPScale = dipScale;
}

/**
 * Postpone requesting layouts till unfreezeLayoutRequests is called.
 */
void AwLayoutSizer::FreezeLayoutRequests()
{
    mFreezeLayoutRequests = TRUE;
    mFrozenLayoutRequestPending = FALSE;
}

/**
 * Stop postponing layout requests and request layout if such a request would have been made
 * had the freezeLayoutRequests method not been called before.
 */
void AwLayoutSizer::UnfreezeLayoutRequests()
{
    mFreezeLayoutRequests = FALSE;
    if (mFrozenLayoutRequestPending) {
        mFrozenLayoutRequestPending = FALSE;
        mDelegate->RequestLayout();
    }
}

/**
 * Update the contents size.
 * This should be called whenever the content size changes (due to DOM manipulation or page
 * load, for example).
 * The width and height should be in CSS pixels.
 */
void AwLayoutSizer::OnContentSizeChanged(
    /* [in] */ Int32 widthCss,
    /* [in] */ Int32 heightCss)
{
    DoUpdate(widthCss, heightCss, mPageScaleFactor);
}

/**
 * Update the contents page scale.
 * This should be called whenever the content page scale factor changes (due to pinch zoom, for
 * example).
 */
void AwLayoutSizer::OnPageScaleChanged(
    /* [in] */ Float pageScaleFactor)
{
    DoUpdate(mContentWidthCss, mContentHeightCss, pageScaleFactor);
}

void AwLayoutSizer::DoUpdate(
    /* [in] */ Int32 widthCss,
    /* [in] */ Int32 heightCss,
    /* [in] */ Float pageScaleFactor)
{
    // We want to request layout only if the size or scale change, however if any of the
    // measurements are 'fixed', then changing the underlying size won't have any effect, so we
    // ignore changes to dimensions that are 'fixed'.
    Int32 heightPix = (Int32) (heightCss * mPageScaleFactor * mDIPScale);
    Boolean pageScaleChanged = mPageScaleFactor != pageScaleFactor;
    Boolean contentHeightChangeMeaningful = !mHeightMeasurementIsFixed &&
        (!mHeightMeasurementLimited || heightPix < mHeightMeasurementLimit);
    Boolean pageScaleChangeMeaningful =
        !mWidthMeasurementIsFixed || contentHeightChangeMeaningful;
    Boolean layoutNeeded = (mContentWidthCss != widthCss && !mWidthMeasurementIsFixed) ||
        (mContentHeightCss != heightCss && contentHeightChangeMeaningful) ||
        (pageScaleChanged && pageScaleChangeMeaningful);

    mContentWidthCss = widthCss;
    mContentHeightCss = heightCss;
    mPageScaleFactor = pageScaleFactor;

    if (layoutNeeded) {
        if (mFreezeLayoutRequests) {
            mFrozenLayoutRequestPending = TRUE;
        }
        else {
            mDelegate->RequestLayout();
        }
    }
}

/**
 * Calculate the size of the view.
 * This is designed to be used to implement the android.view.View#onMeasure() method.
 */
ECode AwLayoutSizer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 heightMode = Elastos::Droid::View::View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSize = Elastos::Droid::View::View::MeasureSpec::GetSize(heightMeasureSpec);
    Int32 widthMode = Elastos::Droid::View::View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSize = Elastos::Droid::View::View::MeasureSpec::GetSize(widthMeasureSpec);

    Int32 contentHeightPix = (Int32) (mContentHeightCss * mPageScaleFactor * mDIPScale);
    Int32 contentWidthPix = (Int32) (mContentWidthCss * mPageScaleFactor * mDIPScale);

    Int32 measuredHeight = contentHeightPix;
    Int32 measuredWidth = contentWidthPix;

    // Always use the given size unless unspecified. This matches WebViewClassic behavior.
    mWidthMeasurementIsFixed = (widthMode != (Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED));
    mHeightMeasurementIsFixed = (heightMode == (Elastos::Droid::View::View::MeasureSpec::EXACTLY));
    mHeightMeasurementLimited =
        (heightMode == (Elastos::Droid::View::View::MeasureSpec::AT_MOST) && (contentHeightPix > heightSize));
    mHeightMeasurementLimit = heightSize;

    if (mHeightMeasurementIsFixed || mHeightMeasurementLimited) {
        measuredHeight = heightSize;
    }

    if (mWidthMeasurementIsFixed) {
        measuredWidth = widthSize;
    }

    if (measuredHeight < contentHeightPix) {
        measuredHeight |= IView::MEASURED_STATE_TOO_SMALL;
    }

    if (measuredWidth < contentWidthPix) {
        measuredWidth |= IView::MEASURED_STATE_TOO_SMALL;
    }

    mDelegate->SetMeasuredDimension(measuredWidth, measuredHeight);
    return NOERROR;
}

/**
 * Notify the AwLayoutSizer that the size of the view has changed.
 * This should be called by the Android view system after onMeasure if the view's size has
 * changed.
 */
ECode AwLayoutSizer::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    UpdateLayoutSettings();
    return NOERROR;
}

/**
 * Notify the AwLayoutSizer that the layout pass requested via Delegate.requestLayout has
 * completed.
 * This should be called after onSizeChanged regardless of whether the size has changed or not.
 */
void AwLayoutSizer::OnLayoutChange()
{
    UpdateLayoutSettings();
}

// This needs to be called every time either the physical size of the view is changed or layout
// params are updated.
void AwLayoutSizer::UpdateLayoutSettings()
{
    mDelegate->SetForceZeroLayoutHeight(mDelegate->IsLayoutParamsHeightWrapContent());
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
