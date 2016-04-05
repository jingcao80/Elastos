
#include "elastos/droid/launcher2/AppWidgetResizeFrame.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/core/Math.h>
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::AppWidget::AppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

AppWidgetResizeFrame::MyRunnable::MyRunnable(
    /* [in] */ AppWidgetResizeFrame* host)
    : mHost(host)
{
}

ECode AppWidgetResizeFrame::MyRunnable::Run()
{
    return mHost->SnapToWidget(TRUE);
}

CAR_INTERFACE_IMPL(AppWidgetResizeFrame::MyAnimatorUpdateListener,
        Object, IAnimatorUpdateListener);

AppWidgetResizeFrame::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ AppWidgetResizeFrame* host)
    : mHost(host)
{
}

ECode AppWidgetResizeFrame::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    return mHost->RequestLayout();
}

static AutoPtr<IRect> InitTmpRect()
{
    AutoPtr<IRect> tmp;
    CRect::New((IRect**)&tmp);
    return tmp;
}

AutoPtr<IRect> mTmpRect = InitTmpRect();

CAR_INTERFACE_IMPL(AppWidgetResizeFrame, FrameLayout,
        IAppWidgetResizeFrame);

AppWidgetResizeFrame::AppWidgetResizeFrame()
    : SNAP_DURATION(150)
    , BACKGROUND_PADDING(24)
    , DIMMED_HANDLE_ALPHA(0.0f)
    , RESIZE_THRESHOLD(0.66f)

    , mLeftBorderActive(FALSE)
    , mRightBorderActive(FALSE)
    , mTopBorderActive(FALSE)
    , mBottomBorderActive(FALSE)

    , mWidgetPaddingLeft(0)
    , mWidgetPaddingRight(0)
    , mWidgetPaddingTop(0)
    , mWidgetPaddingBottom(0)

    , mBaselineWidth(0)
    , mBaselineHeight(0)
    , mBaselineX(0)
    , mBaselineY(0)
    , mResizeMode(0)

    , mRunningHInc(0)
    , mRunningVInc(0)
    , mMinHSpan(0)
    , mMinVSpan(0)
    , mDeltaX(0)
    , mDeltaY(0)
    , mDeltaXAddOn(0)
    , mDeltaYAddOn(0)

    , mBackgroundPadding(0)
    , mTouchTargetWidth(0)

    , mTopTouchRegionAdjustment(0)
    , mBottomTouchRegionAdjustment(0)
{
    mDirectionVector = ArrayOf<Int32>::Alloc(2);
    mLastDirectionVector = ArrayOf<Int32>::Alloc(2);
}

ECode AppWidgetResizeFrame::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILauncherAppWidgetHostView* widgetView,
    /* [in] */ ICellLayout* cellLayout,
    /* [in] */ IDragLayer* dragLayer)
{
    FrameLayout::constructor(context);

    mLauncher = ILauncher::Probe(context);
    mCellLayout = cellLayout;
    mWidgetView = widgetView;
    AutoPtr<IAppWidgetProviderInfo> info1;
    IAppWidgetHostView::Probe(widgetView)->GetAppWidgetInfo(
            (IAppWidgetProviderInfo**)&info1);
    info1->GetResizeMode(&mResizeMode);
    mDragLayer = dragLayer;
    AutoPtr<IView> view;
    IView::Probe(dragLayer)->FindViewById(
            Elastos::Droid::Launcher2::R::id::workspace, (IView**)&view);
    mWorkspace = IWorkspace::Probe(view);

    AutoPtr<IAppWidgetProviderInfo> info2;
    IAppWidgetHostView::Probe(widgetView)->GetAppWidgetInfo(
            (IAppWidgetProviderInfo**)&info2);
    AutoPtr<ArrayOf<Int32> > result;
    assert(0 && "need class Launcher");
    //Launcher::GetMinSpanForWidget(mLauncher, info2, (ArrayOf<Int32>**)&result);
    mMinHSpan = (*result)[0];
    mMinVSpan = (*result)[1];

    SetBackgroundResource(Elastos::Droid::Launcher2::R
            ::drawable::widget_resize_frame_holo);
    SetPadding(0, 0, 0, 0);

    CImageView::New(context, (IImageView**)&mLeftHandle);
    mLeftHandle->SetImageResource(Elastos::Droid::Launcher2::R
            ::drawable::widget_resize_handle_left);
    AutoPtr<IFrameLayoutLayoutParams> lp;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            IGravity::START | IGravity::CENTER_VERTICAL,
            (IFrameLayoutLayoutParams**)&lp);
    AddView(IView::Probe(mLeftHandle), IViewGroupLayoutParams::Probe(lp));

    CImageView::New(context, (IImageView**)&mRightHandle);
    mRightHandle->SetImageResource(Elastos::Droid::Launcher2::R
            ::drawable::widget_resize_handle_right);
    lp = NULL;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            IGravity::END | IGravity::CENTER_VERTICAL, (IFrameLayoutLayoutParams**)&lp);
    AddView(IView::Probe(mRightHandle), IViewGroupLayoutParams::Probe(lp));

    CImageView::New(context, (IImageView**)&mTopHandle);
    mTopHandle->SetImageResource(Elastos::Droid::Launcher2::R
            ::drawable::widget_resize_handle_top);
    lp = NULL;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            IGravity::CENTER_HORIZONTAL | IGravity::TOP,
            (IFrameLayoutLayoutParams**)&lp);
    AddView(IView::Probe(mTopHandle), IViewGroupLayoutParams::Probe(lp));

    CImageView::New(context, (IImageView**)&mBottomHandle);
    mBottomHandle->SetImageResource(Elastos::Droid::Launcher2::R
            ::drawable::widget_resize_handle_bottom);
    lp = NULL;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            IGravity::CENTER_HORIZONTAL | IGravity::BOTTOM,
            (IFrameLayoutLayoutParams**)&lp);
    AddView(IView::Probe(mBottomHandle), IViewGroupLayoutParams::Probe(lp));

    AutoPtr<IAppWidgetProviderInfo> info3;
    IAppWidgetHostView::Probe(widgetView)->GetAppWidgetInfo(
            (IAppWidgetProviderInfo**)&info3);
    AutoPtr<IComponentName> name;
    info3->GetProvider((IComponentName**)&name);;
    AutoPtr<IRect> p = AppWidgetHostView::GetDefaultPaddingForWidget(
            context, name, NULL);
    p->GetLeft(&mWidgetPaddingLeft);
    p->GetTop(&mWidgetPaddingTop);
    p->GetRight(&mWidgetPaddingRight);
    p->GetBottom(&mWidgetPaddingBottom);

    if (mResizeMode == IAppWidgetProviderInfo::RESIZE_HORIZONTAL) {
        IView::Probe(mTopHandle)->SetVisibility(GONE);
        IView::Probe(mBottomHandle)->SetVisibility(GONE);
    }
    else if (mResizeMode == IAppWidgetProviderInfo::RESIZE_VERTICAL) {
        IView::Probe(mLeftHandle)->SetVisibility(GONE);
        IView::Probe(mRightHandle)->SetVisibility(GONE);
    }

    AutoPtr<IResources> resources;
    IContext::Probe(mLauncher)->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mBackgroundPadding = (Int32)Elastos::Core::Math::Ceil(density * BACKGROUND_PADDING);
    mTouchTargetWidth = 2 * mBackgroundPadding;

    // When we create the resize frame, we first mark all cells as unoccupied. The appropriate
    // cells (same if not resized, or different) will be marked as occupied when the resize
    // frame is dismissed.
    assert(0 && "need class mCellLayout");
    //return mCellLayout->MarkCellsAsUnoccupiedForView(mWidgetView);
    return NOERROR;
}

ECode AppWidgetResizeFrame::BeginResizeIfPointInRegion(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean horizontalActive = (mResizeMode &
            IAppWidgetProviderInfo::RESIZE_HORIZONTAL) != 0;
    Boolean verticalActive = (mResizeMode &
            IAppWidgetProviderInfo::RESIZE_VERTICAL) != 0;

    mLeftBorderActive = (x < mTouchTargetWidth) && horizontalActive;
    Int32 width;
    GetWidth(&width);
    mRightBorderActive = (x > width - mTouchTargetWidth) && horizontalActive;
    mTopBorderActive = (y < mTouchTargetWidth + mTopTouchRegionAdjustment)
            && verticalActive;
    Int32 height;
    GetHeight(&height);
    mBottomBorderActive = (y > height - mTouchTargetWidth + mBottomTouchRegionAdjustment)
            && verticalActive;

    Boolean anyBordersActive = mLeftBorderActive || mRightBorderActive
            || mTopBorderActive || mBottomBorderActive;

    GetMeasuredWidth(&mBaselineWidth);
    GetMeasuredHeight(&mBaselineHeight);
    GetLeft(&mBaselineX);
    GetTop(&mBaselineY);

    if (anyBordersActive) {
        mLeftHandle->SetAlpha(mLeftBorderActive ? 1.0f : DIMMED_HANDLE_ALPHA);
        mRightHandle->SetAlpha(mRightBorderActive ? 1.0f :DIMMED_HANDLE_ALPHA);
        mTopHandle->SetAlpha(mTopBorderActive ? 1.0f : DIMMED_HANDLE_ALPHA);
        mBottomHandle->SetAlpha(mBottomBorderActive ? 1.0f : DIMMED_HANDLE_ALPHA);
    }
    *result = anyBordersActive;
    return NOERROR;
}

ECode AppWidgetResizeFrame::UpdateDeltas(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    if (mLeftBorderActive) {
        mDeltaX = Elastos::Core::Math::Max(-mBaselineX, deltaX);
        mDeltaX = Elastos::Core::Math::Min(mBaselineWidth - 2 * mTouchTargetWidth, mDeltaX);
    }
    else if (mRightBorderActive) {
        Int32 width;
        IView::Probe(mDragLayer)->GetWidth(&width);
        mDeltaX = Elastos::Core::Math::Min(width - (mBaselineX + mBaselineWidth), deltaX);
        mDeltaX = Elastos::Core::Math::Max(-mBaselineWidth + 2 * mTouchTargetWidth, mDeltaX);
    }

    if (mTopBorderActive) {
        mDeltaY = Elastos::Core::Math::Max(-mBaselineY, deltaY);
        mDeltaY = Elastos::Core::Math::Min(mBaselineHeight - 2 * mTouchTargetWidth, mDeltaY);
    }
    else if (mBottomBorderActive) {
        Int32 height;
        IView::Probe(mDragLayer)->GetHeight(&height);
        mDeltaY = Elastos::Core::Math::Min(height - (mBaselineY + mBaselineHeight), deltaY);
        mDeltaY = Elastos::Core::Math::Max(-mBaselineHeight + 2 * mTouchTargetWidth, mDeltaY);
    }
    return NOERROR;
}

ECode AppWidgetResizeFrame::VisualizeResizeForDelta(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    return VisualizeResizeForDelta(deltaX, deltaY, FALSE);
}

ECode AppWidgetResizeFrame::VisualizeResizeForDelta(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Boolean onDismiss)
{
    UpdateDeltas(deltaX, deltaY);
    AutoPtr<IViewGroupLayoutParams> res;
    GetLayoutParams((IViewGroupLayoutParams**)&res);
    assert(0 && "need class CDragLayer::LayoutParams");
    // AutoPtr<CDragLayer::LayoutParams> lp =
    //         (CDragLayer::LayoutParams*)IDragLayerLayoutParams::Probe(res);

    // if (mLeftBorderActive) {
    //     lp->SetX(mBaselineX + mDeltaX);
    //     lp->SetWidth(mBaselineWidth - mDeltaX);
    // }
    // else if (mRightBorderActive) {
    //     lp->SetWidth(mBaselineWidth + mDeltaX);
    // }

    // if (mTopBorderActive) {
    //     lp->SetY(mBaselineY + mDeltaY);
    //     lp->SetHeight(mBaselineHeight - mDeltaY);
    // }
    // else if (mBottomBorderActive) {
    //     lp->SetHeight(mBaselineHeight + mDeltaY);
    // }

    ResizeWidgetIfNeeded(onDismiss);
    return RequestLayout();
}

ECode AppWidgetResizeFrame::ResizeWidgetIfNeeded(
    /* [in] */ Boolean onDismiss)
{
    Int32 width;
    assert(0 && "need class celllayout");
    //mCellLayout->GetCellWidth(&width);
    Int32 wgap;
    assert(0 && "need class celllayout");
    //mCellLayout->GetWidthGap(&wgap);
    Int32 xThreshold = width + wgap;
    Int32 height;
    assert(0 && "need class celllayout");
    //mCellLayout->GetCellHeight(&height);
    Int32 hgap;
    assert(0 && "need class celllayout");
    //mCellLayout->GetHeightGap(&hgap);
    Int32 yThreshold = height + hgap;

    Int32 deltaX = mDeltaX + mDeltaXAddOn;
    Int32 deltaY = mDeltaY + mDeltaYAddOn;

    Float hSpanIncF = 1.0f * deltaX / xThreshold - mRunningHInc;
    Float vSpanIncF = 1.0f * deltaY / yThreshold - mRunningVInc;

    Int32 hSpanInc = 0;
    Int32 vSpanInc = 0;
    Int32 cellXInc = 0;
    Int32 cellYInc = 0;

    Int32 countX;
    assert(0 && "need class celllayout");
    //mCellLayout->GetCountX(&countX);
    Int32 countY;
    assert(0 && "need class celllayout");
    //mCellLayout->GetCountY(&countY);

    if (Elastos::Core::Math::Abs(hSpanIncF) > RESIZE_THRESHOLD) {
        hSpanInc = Elastos::Core::Math::Round(hSpanIncF);
    }
    if (Elastos::Core::Math::Abs(vSpanIncF) > RESIZE_THRESHOLD) {
        vSpanInc = Elastos::Core::Math::Round(vSpanIncF);
    }

    if (!onDismiss && (hSpanInc == 0 && vSpanInc == 0)) return NOERROR;

    AutoPtr<IViewGroupLayoutParams> res;
    IView::Probe(mWidgetView)->GetLayoutParams((IViewGroupLayoutParams**)&res);
    assert(0 && "need class CellLayout::LayoutParams");
    // AutoPtr<ICellLayoutLayoutParams> lp = ICellLayoutLayoutParams::Probe(res);
    // CellLayout::LayoutParams* _lp = (CellLayout::LayoutParams*)lp;

    // Int32 spanX = _lp->mCellHSpan;
    // Int32 spanY = _lp->mCellVSpan;
    // Int32 cellX = _lp->mUseTmpCoords ? _lp->mTmpCellX : _lp->mCellX;
    // Int32 cellY = _lp->mUseTmpCoords ? _lp->mTmpCellY : _lp->mCellY;

    // Int32 hSpanDelta = 0;
    // Int32 vSpanDelta = 0;

    // // For each border, we bound the resizing based on the minimum width, and the maximum
    // // expandability.
    // if (mLeftBorderActive) {
    //     cellXInc = Elastos::Core::Math::Max(-cellX, hSpanInc);
    //     cellXInc = Elastos::Core::Math::Min(_lp->mCellHSpan - mMinHSpan, cellXInc);
    //     hSpanInc *= -1;
    //     hSpanInc = Elastos::Core::Math::Min(cellX, hSpanInc);
    //     hSpanInc = Elastos::Core::Math::Max(-(_lp->mCellHSpan - mMinHSpan), hSpanInc);
    //     hSpanDelta = -hSpanInc;

    // }
    // else if (mRightBorderActive) {
    //     hSpanInc = Elastos::Core::Math::Min(countX - (cellX + spanX), hSpanInc);
    //     hSpanInc = Elastos::Core::Math::Max(-(_lp->mCellHSpan - mMinHSpan), hSpanInc);
    //     hSpanDelta = hSpanInc;
    // }

    // if (mTopBorderActive) {
    //     cellYInc = Elastos::Core::Math::Max(-cellY, vSpanInc);
    //     cellYInc = Elastos::Core::Math::Min(_lp->mCellVSpan - mMinVSpan, cellYInc);
    //     vSpanInc *= -1;
    //     vSpanInc = Elastos::Core::Math::Min(cellY, vSpanInc);
    //     vSpanInc = Elastos::Core::Math::Max(-(_lp->mCellVSpan - mMinVSpan), vSpanInc);
    //     vSpanDelta = -vSpanInc;
    // }
    // else if (mBottomBorderActive) {
    //     vSpanInc = Elastos::Core::Math::Min(countY - (cellY + spanY), vSpanInc);
    //     vSpanInc = Elastos::Core::Math::Max(-(_lp->mCellVSpan - mMinVSpan), vSpanInc);
    //     vSpanDelta = vSpanInc;
    // }

    // (*mDirectionVector)[0] = 0;
    // (*mDirectionVector)[1] = 0;
    // // Update the widget's dimensions and position according to the deltas computed above
    // if (mLeftBorderActive || mRightBorderActive) {
    //     spanX += hSpanInc;
    //     cellX += cellXInc;
    //     if (hSpanDelta != 0) {
    //         (*mDirectionVector)[0] = mLeftBorderActive ? -1 : 1;
    //     }
    // }

    // if (mTopBorderActive || mBottomBorderActive) {
    //     spanY += vSpanInc;
    //     cellY += cellYInc;
    //     if (vSpanDelta != 0) {
    //         (*mDirectionVector)[1] = mTopBorderActive ? -1 : 1;
    //     }
    // }

    // if (!onDismiss && vSpanDelta == 0 && hSpanDelta == 0) return NOERROR;

    // // We always want the final commit to match the feedback, so we make sure to use the
    // // last used direction vector when committing the resize / reorder.
    // if (onDismiss) {
    //     (*mDirectionVector)[0] = (*mLastDirectionVector)[0];
    //     (*mDirectionVector)[1] = (*mLastDirectionVector)[1];
    // }
    // else {
    //     (*mLastDirectionVector)[0] = (*mDirectionVector)[0];
    //     (*mLastDirectionVector)[1] = (*mDirectionVector)[1];
    // }

    Boolean tmp;
    assert(0 && "need class celllayout");
    // mCellLayout->CreateAreaForResize(cellX, cellY, spanX, spanY, mWidgetView,
    //         mDirectionVector, onDismiss, &tmp)
    if (tmp) {
        assert(0 && "need class CellLayout::LayoutParams");
        // _lp->mTmpCellX = cellX;
        // _lp->mTmpCellY = cellY;
        // _lp->mCellHSpan = spanX;
        // _lp->mCellVSpan = spanY;
        // mRunningVInc += vSpanDelta;
        // mRunningHInc += hSpanDelta;
        // if (!onDismiss) {
        //     UpdateWidgetSizeRanges(mWidgetView, mLauncher, spanX, spanY);
        // }
    }
    return IView::Probe(mWidgetView)->RequestLayout();
}

ECode AppWidgetResizeFrame::UpdateWidgetSizeRanges(
    /* [in] */ IAppWidgetHostView* widgetView,
    /* [in] */ ILauncher* launcher,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY)
{
    GetWidgetSizeRanges(launcher, spanX, spanY, mTmpRect);
    Int32 left;
    mTmpRect->GetLeft(&left);
    Int32 top;
    mTmpRect->GetTop(&top);
    Int32 right;
    mTmpRect->GetRight(&right);
    Int32 bottom;
    mTmpRect->GetBottom(&bottom);
    return widgetView->UpdateAppWidgetSize(NULL, left, top,
            right, bottom);
}

AutoPtr<IRect> AppWidgetResizeFrame::GetWidgetSizeRanges(
    /* [in] */ ILauncher* launcher,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IRect* rect)
{
    if (rect == NULL) {
        CRect::New((IRect**)&rect);
    }
    AutoPtr<IRect> landMetrics;
    assert(0 && "need class Workspace");
    // Workspace::GetCellLayoutMetrics(launcher, ICellLayout::LANDSCAPE,
    //         (IRect**)&landMetrics);
    AutoPtr<IRect> portMetrics;
    assert(0 && "need class Workspace");
    // Workspace::GetCellLayoutMetrics(launcher, ICellLayout::PORTRAIT,
    //         (IRect**)&portMetrics);
    AutoPtr<IResources> resources;
    IContext::Probe(launcher)->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);

    // Compute landscape size
    Int32 cellWidth;
    landMetrics->GetLeft(&cellWidth);
    Int32 cellHeight;
    landMetrics->GetTop(&cellHeight);
    Int32 widthGap;
    landMetrics->GetRight(&widthGap);
    Int32 heightGap;
    landMetrics->GetBottom(&heightGap);
    Int32 landWidth = (Int32)((spanX * cellWidth + (spanX - 1) * widthGap) / density);
    Int32 landHeight = (Int32)((spanY * cellHeight + (spanY - 1) * heightGap) / density);

    // Compute portrait size
    portMetrics->GetLeft(&cellWidth);
    portMetrics->GetTop(&cellHeight);
    portMetrics->GetRight(&widthGap);
    portMetrics->GetBottom(&heightGap);
    Int32 portWidth = (Int32)((spanX * cellWidth + (spanX - 1) * widthGap) / density);
    Int32 portHeight = (Int32)((spanY * cellHeight + (spanY - 1) * heightGap) / density);
    rect->Set(portWidth, landHeight, landWidth, portHeight);
    return rect;
}

ECode AppWidgetResizeFrame::CommitResize()
{
    ResizeWidgetIfNeeded(TRUE);
    return RequestLayout();
}

ECode AppWidgetResizeFrame::OnTouchUp()
{
    Int32 width;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetCellWidth(&width);
    Int32 wgap;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetWidthGap(&wgap);
    Int32 xThreshold = width + wgap;
    Int32 height;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetCellHeight(&height);
    Int32 hgap;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetHeightGap(&hgap);
    Int32 yThreshold = height + hgap;

    mDeltaXAddOn = mRunningHInc * xThreshold;
    mDeltaYAddOn = mRunningVInc * yThreshold;
    mDeltaX = 0;
    mDeltaY = 0;

    AutoPtr<IRunnable> run = new MyRunnable(this);
    Boolean tmp;
    return Post(run, &tmp);
}

ECode AppWidgetResizeFrame::SnapToWidget(
    /* [in] */ Boolean animate)
{
    AutoPtr<IViewGroupLayoutParams> res;
    GetLayoutParams((IViewGroupLayoutParams**)&res);
    AutoPtr<IDragLayerLayoutParams> lp = IDragLayerLayoutParams::Probe(res);

    Int32 left;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetLeft(&left);
    Int32 pleft;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetPaddingLeft(&pleft);
    Int32 pleft2;
    IView::Probe(mDragLayer)->GetPaddingLeft(&pleft2);
    Int32 x;
    IView::Probe(mWorkspace)->GetScrollX(&x);
    Int32 xOffset = left + pleft
            + pleft2 - x;

    Int32 top;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetTop(&top);
    Int32 ptop;
    assert(0 && "need class CellLayout");
    //mCellLayout->GetPaddingTop(&ptop);
    Int32 ptop2;
    IView::Probe(mDragLayer)->GetPaddingTop(&ptop2);
    Int32 y;
    IView::Probe(mWorkspace)->GetScrollY(&y);
    Int32 yOffset = top + ptop
            + ptop2 - y;

    Int32 width2;
    IView::Probe(mWidgetView)->GetWidth(&width2);
    Int32 newWidth = width2 + 2 * mBackgroundPadding - mWidgetPaddingLeft -
            mWidgetPaddingRight;
    Int32 height2;
    IView::Probe(mWidgetView)->GetHeight(&height2);
    Int32 newHeight = height2 + 2 * mBackgroundPadding - mWidgetPaddingTop -
            mWidgetPaddingBottom;

    Int32 left2;
    IView::Probe(mWidgetView)->GetLeft(&left2);
    Int32 newX = left2 - mBackgroundPadding + xOffset + mWidgetPaddingLeft;
    Int32 top2;
    IView::Probe(mWidgetView)->GetTop(&top2);
    Int32 newY = top2 - mBackgroundPadding + yOffset + mWidgetPaddingTop;

    // We need to make sure the frame's touchable regions lie fully within the bounds of the
    // DragLayer. We allow the actual handles to be clipped, but we shift the touch regions
    // down accordingly to provide a proper touch target.
    if (newY < 0) {
        // In this case we shift the touch region down to start at the top of the DragLayer
        mTopTouchRegionAdjustment = -newY;
    }
    else {
        mTopTouchRegionAdjustment = 0;
    }
    Int32 height;
    IView::Probe(mDragLayer)->GetHeight(&height);
    if (newY + newHeight > height) {
        // In this case we shift the touch region up to end at the bottom of the DragLayer
        mBottomTouchRegionAdjustment = -(newY + newHeight - height);
    }
    else {
        mBottomTouchRegionAdjustment = 0;
    }

    if (!animate) {
        lp->SetWidth(newWidth);
        lp->SetHeight(newHeight);
        lp->SetX(newX);
        lp->SetY(newY);
        mLeftHandle->SetAlpha(1.0f);
        mRightHandle->SetAlpha(1.0f);
        mTopHandle->SetAlpha(1.0f);
        mBottomHandle->SetAlpha(1.0f);
        RequestLayout();
    }
    else {
        AutoPtr<IPropertyValuesHolderHelper> helper;
        CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);

        AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(2);
        Int32 _width;
        lp->GetWidth(&_width);
        (*values)[0] = _width;
        (*values)[1] = newWidth;
        AutoPtr<IPropertyValuesHolder> width;
        helper->OfInt32(String("width"), values, (IPropertyValuesHolder**)&width);

        AutoPtr<ArrayOf<Int32> > values2 = ArrayOf<Int32>::Alloc(2);
        Int32 _height;
        lp->GetWidth(&_height);
        (*values2)[0] = _height;
        (*values2)[1] = newHeight;
        AutoPtr<IPropertyValuesHolder> height;
        helper->OfInt32(String("height"), values2, (IPropertyValuesHolder**)&height);

        AutoPtr<ArrayOf<Int32> > values3 = ArrayOf<Int32>::Alloc(2);
        Int32 _x;
        lp->GetX(&_x);
        (*values3)[0] = _x;
        (*values3)[1] = newX;
        AutoPtr<IPropertyValuesHolder> x;
        helper->OfInt32(String("x"), values3, (IPropertyValuesHolder**)&x);

        AutoPtr<ArrayOf<Int32> > values4 = ArrayOf<Int32>::Alloc(2);
        Int32 _y;
        lp->GetX(&_y);
        (*values4)[0] = _y;
        (*values4)[1] = newY;
        AutoPtr<IPropertyValuesHolder> y;
        helper->OfInt32(String("y"), values4, (IPropertyValuesHolder**)&y);

        AutoPtr<ArrayOf<IPropertyValuesHolder*> > arrays =
                ArrayOf<IPropertyValuesHolder*>::Alloc(4);
        (*arrays)[0] = width;
        (*arrays)[1] = height;
        (*arrays)[2] = x;
        (*arrays)[3] = y;
        AutoPtr<IObjectAnimator> oa =
                LauncherAnimUtils::OfPropertyValuesHolder(lp,
                IView::Probe(this), arrays);

        AutoPtr<ArrayOf<Float> > values5 = ArrayOf<Float>::Alloc(1);
        (*values5)[0] = 1.0f;
        AutoPtr<IObjectAnimator> leftOa = LauncherAnimUtils::OfFloat(
                IView::Probe(mLeftHandle), String("alpha"), values5);
        AutoPtr<IObjectAnimator> rightOa = LauncherAnimUtils::OfFloat(
                IView::Probe(mRightHandle), String("alpha"), values5);
        AutoPtr<IObjectAnimator> topOa = LauncherAnimUtils::OfFloat(
                IView::Probe(mTopHandle), String("alpha"), values5);
        AutoPtr<IObjectAnimator> bottomOa = LauncherAnimUtils::OfFloat(
                IView::Probe(mBottomHandle), String("alpha"), values5);

        AutoPtr<IAnimatorUpdateListener> listener = new MyAnimatorUpdateListener(this);
        IValueAnimator::Probe(oa)->AddUpdateListener(listener);

        AutoPtr<IAnimatorSet> set = LauncherAnimUtils::CreateAnimatorSet();
        if (mResizeMode == IAppWidgetProviderInfo::RESIZE_VERTICAL) {
            AutoPtr<ArrayOf<IAnimator*> > array = ArrayOf<IAnimator*>::Alloc(3);
            array->Set(0, IAnimator::Probe(oa));
            array->Set(1, IAnimator::Probe(topOa));
            array->Set(2, IAnimator::Probe(bottomOa));
            set->PlayTogether(array);
        }
        else if (mResizeMode == IAppWidgetProviderInfo::RESIZE_HORIZONTAL) {
            AutoPtr<ArrayOf<IAnimator*> > array = ArrayOf<IAnimator*>::Alloc(3);
            array->Set(0, IAnimator::Probe(oa));
            array->Set(1, IAnimator::Probe(leftOa));
            array->Set(2, IAnimator::Probe(rightOa));
            set->PlayTogether(array);
        }
        else {
            AutoPtr<ArrayOf<IAnimator*> > array = ArrayOf<IAnimator*>::Alloc(5);
            array->Set(0, IAnimator::Probe(oa));
            array->Set(1, IAnimator::Probe(leftOa));
            array->Set(2, IAnimator::Probe(rightOa));
            array->Set(3, IAnimator::Probe(topOa));
            array->Set(4, IAnimator::Probe(bottomOa));
            set->PlayTogether(array);
        }

        IAnimator::Probe(set)->SetDuration(SNAP_DURATION);
        IAnimator::Probe(set)->Start();
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos