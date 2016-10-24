
#include "CPinnedHeaderListView.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

const Int32 CPinnedHeaderListView::MAX_ALPHA = 255;
const Int32 CPinnedHeaderListView::TOP = 0;
const Int32 CPinnedHeaderListView::BOTTOM = 1;
const Int32 CPinnedHeaderListView::FADING = 2;

const Int32 CPinnedHeaderListView::DEFAULT_ANIMATION_DURATION = 20;

const Int32 CPinnedHeaderListView::DEFAULT_SMOOTH_SCROLL_DURATION = 100;

CAR_INTERFACE_IMPL_3(CPinnedHeaderListView, CAutoScrollListView, IPinnedHeaderListView,\
        IAbsListViewOnScrollListener, IAdapterViewOnItemSelectedListener)

CAR_OBJECT_IMPL(CPinnedHeaderListView)

CPinnedHeaderListView::CPinnedHeaderListView()
    : mScrollToSectionOnHeaderTouch(FALSE)
    , mHeaderTouched(FALSE)
    , mAnimationDuration(DEFAULT_ANIMATION_DURATION)
{
    CRectF::New((IRectF**)&mBounds);
}

CPinnedHeaderListView::~CPinnedHeaderListView()
{}

ECode CPinnedHeaderListView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, Elastos::R::attr::listViewStyle);
}

ECode CPinnedHeaderListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, Elastos::R::attr::listViewStyle);
}

ECode CPinnedHeaderListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    CAutoScrollListView::constructor(context, attrs, defStyle);
    CAutoScrollListView::SetOnScrollListener(this);
    CAutoScrollListView::SetOnItemSelectedListener(this);

    return NOERROR;
}

ECode CPinnedHeaderListView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    CAutoScrollListView::OnLayout(changed, l, t, r, b);
    GetPaddingStart(&mHeaderPaddingStart);
    Int32 paddingEnd;
    GetPaddingEnd(&paddingEnd);
    mHeaderWidth = r - l - mHeaderPaddingStart - paddingEnd;
    return NOERROR;
}

ECode CPinnedHeaderListView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    mAdapter = IPinnedHeaderAdapter::Probe(adapter);
    CAutoScrollListView::SetAdapter(adapter);
    return NOERROR;
}

ECode CPinnedHeaderListView::SetOnScrollListener(
    /* [in] */ IAbsListViewOnScrollListener* onScrollListener)
{
    mOnScrollListener = onScrollListener;
    CAutoScrollListView::SetOnScrollListener(this);
    return NOERROR;
}

ECode CPinnedHeaderListView::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    mOnItemSelectedListener = listener;
    CAutoScrollListView::SetOnItemSelectedListener(this);
    return NOERROR;
}

ECode CPinnedHeaderListView::SetScrollToSectionOnHeaderTouch(
    /* [in] */ Boolean value)
{
    mScrollToSectionOnHeaderTouch = value;
    return NOERROR;
}

ECode CPinnedHeaderListView::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    if (mAdapter != null) {
        Int32 count;
        mAdapter->GetPinnedHeaderCount(&count);
        if (count != mSize) {
            mSize = count;
            if (mHeaders == NULL) {
                mHeaders = ArrayOf<PinnedHeader*>::Alloc(mSize);
            }
            else if (mHeaders->GetLength() < mSize) {
                AutoPtr<ArrayOf<PinnedHeader*> > headers = mHeaders;
                mHeaders = ArrayOf<PinnedHeader*>::Alloc(headers, mSize);
            }
        }

        for (Int32 i = 0; i < mSize; i++) {
            if (mHeaders[i] == NULL) {
                mHeaders->Set(i, new PinnedHeader());
            }
            mAdapter->GetPinnedHeaderView(i, mHeaders[i]->mView, this, &mHeaders[i]->mView);
        }

        AutoPtr<ISystem> sysObj;
        CSystem::AcquireSingleton((ISystem**)&sysObj);
        Int64 value;
        sysObj->GetCurrentTimeMillis(&value);
        mAnimationTargetTime = value + mAnimationDuration;
        mAdapter->ConfigurePinnedHeaders(this);
        InvalidateIfAnimating();
    }
    if (mOnScrollListener != NULL) {
        mOnScrollListener->OnScroll(this, firstVisibleItem, visibleItemCount, totalItemCount);
    }

    return NOERROR;
}

Float CPinnedHeaderListView::GetTopFadingEdgeStrength()
{
    // Disable vertical fading at the top when the pinned header is present
    return mSize > 0 ? 0 : CAutoScrollListView::GetTopFadingEdgeStrength();
}

ECode CPinnedHeaderListView::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    mScrollState = scrollState;
    if (mOnScrollListener != NULL) {
        mOnScrollListener->OnScrollStateChanged(this, scrollState);
    }
    return NOERROR;
}

ECode CPinnedHeaderListView::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Int32 height;
    GetHeight(&height);

    Int32 windowTop = 0;
    Int32 windowBottom = height;

    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<PinnedHeader*> header = mHeaders[i];
        if (header->mVisible) {
            if (header->mState == TOP) {
                windowTop = header->mY + header->mHeight;
            }
            else if (header->mState == BOTTOM) {
                windowBottom = header->mY;
                break;
            }
        }
    }

    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    if (selectedView != NULL) {
        Int32 top, bottom;
        if (selectedView->GetTop(&top), top < windowTop) {
            SetSelectionFromTop(position, windowTop);
        }
        else if (selectedView->GetBottom(&bottom), bottom > windowBottom) {
            Int32 height;
            selectedView->GetHeight(&height);
            SetSelectionFromTop(position, windowBottom - height);
        }
    }

    if (mOnItemSelectedListener != NULL) {
        mOnItemSelectedListener->OnItemSelected(parent, view, position, id);
    }
    return NOERROR;
}

ECode CPinnedHeaderListView::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    if (mOnItemSelectedListener != NULL) {
        mOnItemSelectedListener->OnNothingSelected(parent);
    }
    return NOERROR;
}

ECode CPinnedHeaderListView::GetPinnedHeaderHeight(
    /* [in] */ Int32 viewIndex,
    /* [out] */ Int32* height)
{
    VALUE_NOT_NULL(height);
    EnsurePinnedHeaderLayout(viewIndex);
    mHeaders[viewIndex]->mView->GetHeight(&height);
    return NOERROR;
}

ECode CPinnedHeaderListView::SetHeaderPinnedAtTop(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Int32 y,
    /* [in] */ Boolean animate)
{
    EnsurePinnedHeaderLayout(viewIndex);
    AutoPtr<PinnedHeader*> header = mHeaders[viewIndex];
    header->mVisible = TRUE;
    header->mY = y;
    header->mState = TOP;

    // TODO perhaps we should animate at the top as well
    header->mAnimating = FALSE;
    return NOERROR;
}

ECode CPinnedHeaderListView::SetHeaderPinnedAtBottom(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Int32 y,
    /* [in] */ Boolean animate)
{
    EnsurePinnedHeaderLayout(viewIndex);
    AutoPtr<PinnedHeader*> header = mHeaders[viewIndex];
    header->mState = BOTTOM;
    if (header->mAnimating) {
        header->mTargetTime = mAnimationTargetTime;
        header->mSourceY = header->mY;
        header->mTargetY = y;
    }
    else if (animate && (header->mY != y || !header->mVisible)) {
        if (header->mVisible) {
            header->mSourceY = header->mY;
        }
        else {
            header->mVisible = TRUE;
            header->mSourceY = y + header->mHeight;
        }
        header->mAnimating = TRUE;
        header->mTargetVisible = TRUE;
        header->mTargetTime = mAnimationTargetTime;
        header->mTargetY = y;
    }
    else {
        header->mVisible = TRUE;
        header->mY = y;
    }
    return NOERROR;
}

ECode CPinnedHeaderListView::SetFadingHeader(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Int32 position,
    /* [in] */ Boolean fade)
{
    EnsurePinnedHeaderLayout(viewIndex);

    Int32 fvP;
    GetFirstVisiblePosition(&fvP);
    AutoPtr<IView> child;
    GetChildAt(position - fvP, (IView**)&child);
    if (child == NULL){
        return NOERROR;
    }

    AutoPtr<PinnedHeader*> header = mHeaders[viewIndex];
    header->mVisible = TRUE;
    header->mState = FADING;
    header->mAlpha = MAX_ALPHA;
    header->mAnimating = FALSE;

    Int32 top;
    GetTotalTopPinnedHeaderHeight(&top);
    header->mY = top;
    if (fade) {
        Int32 bottom;
        child->GetBottom(&bottom)
        bottom -= top;
        Int32 headerHeight = header->mHeight;
        if (bottom < headerHeight) {
            Int32 portion = bottom - headerHeight;
            header->mAlpha = MAX_ALPHA * (headerHeight + portion) / headerHeight;
            header->mY = top + portion;
        }
    }
    return NOERROR;
}

ECode CPinnedHeaderListView::SetHeaderInvisible(
    /* [in] */ Int32 viewIndex,
    /* [in] */ Boolean animate)
{
    AutoPtr<PinnedHeader*> header = mHeaders[viewIndex];
    if (header->mVisible && (animate || header->mAnimating)
            && header->mState == BOTTOM) {
        header->mSourceY = header->mY;
        if (!header->mAnimating) {
            header->mVisible = TRUE;
            Int32 bottom;
            GetBottom(&bottom);
            header->mTargetY = bottom + header->mHeight;
        }
        header->mAnimating = TRUE;
        header->mTargetTime = mAnimationTargetTime;
        header->mTargetVisible = FALSE;
    }
    else {
        header->mVisible = FALSE;
    }
    return NOERROR;
}

void CPinnedHeaderListView::EnsurePinnedHeaderLayout(
    /* [in] */ Int32 viewIndex)
{
    AutoPtr<IView> view = mHeaders[viewIndex]->mView;
    Boolean result = FALSE;
    if (view->isLayoutRequested(&result), result) {
        AutoPtr<IViewGroupLayoutParams> layoutParams;
        view->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
        Int32 widthSpec;
        Int32 heightSpec;

        Int32 width;
        if (layoutParams != NULL && layoutParams->GetWidth(&width), width > 0) {
            widthSpec = View::MeasureSpec
                    ::MakeMeasureSpec(width, View::MeasureSpec::EXACTLY);
        }
        else {
            widthSpec = View::MeasureSpec
                    ::makeMeasureSpec(mHeaderWidth, View::MeasureSpec::EXACTLY);
        }

        Int32 height;
        if (layoutParams != NULL && layoutParams->GetHeight(&height), height > 0) {
            heightSpec = View::MeasureSpec
                    ::MakeMeasureSpec(layoutParams.height, View::MeasureSpec::EXACTLY);
        }
        else {
            heightSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
        }
        view->Measure(widthSpec, heightSpec);
        view->GetMeasuredHeight(&height);
        mHeaders[viewIndex]->mHeight = height;
        view->GetMeasuredWidth(&width);
        view->Layout(0, 0, width, height);
    }
}

ECode CPinnedHeaderListView::GetTotalTopPinnedHeaderHeight(
    /* [out] */ Int32* height)
{
    VALUE_NOT_NULL(height);

    for (Int32 i = mSize; --i >= 0;) {
        AutoPtr<PinnedHeader*> header = mHeaders[i];
        if (header->mVisible && header->mState == TOP) {
            *height = header->mY + header->mHeight;
            return NOERROR;
        }
    }
    *height = 0;
    return NOERROR;
}

ECode CPinnedHeaderListView::GetPositionAt(
    /* [in] */ Int32 y,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(height);

    do {
        Int32 left;
        GetPaddingLeft(&left);
        Int32 position;
        PointToPosition(left + 1, y, &position);
        if (position != -1) {
            *result = position;
            return NOERROR;
        }
        // If position == -1, we must have hit a separator. Let's examine
        // a nearby pixel
        y--;
    } while (y > 0);

    *result = 0;
    return NOERROR;
}

ECode CPinnedHeaderListView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALUE_NOT_NULL(res);

    mHeaderTouched = FALSE;
    Boolean result;
    if (CAutoScrollListView::OnInterceptTouchEvent(ev, &result), result) {
        *res = TRUE;
        return NOERROR;
    }

    if (mScrollState == SCROLL_STATE_IDLE) {
        Int32 y;
        ev->GetY(&y);
        Int32 x;
        ev->GetX(&x);
        for (Int32 i = mSize; --i >= 0;) {
            AutoPtr<PinnedHeader*> header = mHeaders[i];
            // For RTL layouts, this also takes into account that the scrollbar is on the left
            // side.
            Int32 padding;
            GetPaddingLeft(&padding);
            Int32 width;
            header->mView.GetWidth(&width);
            if (header->mVisible && header->mY <= y && header.y + header->mHeight > y
                    && x >= padding && padding + width >= x) {
                mHeaderTouched = TRUE;
                Int32 action;
                ev->GetAction(&action);
                if (mScrollToSectionOnHeaderTouch &&
                        action == MotionEvent.ACTION_DOWN) {
                    *res = SmoothScrollToPartition(i);
                    return NOERROR;
                }
                else {
                    *res = true;
                    return NOERROR;
                }
            }
        }
    }

    *res = false;
    return NOERROR;
}

ECode CPinnedHeaderListView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALUE_NOT_NULL(res);

    if (mHeaderTouched) {
        Int32 action;
        ev->GetAction(&action);
        if (action == MotionEvent.ACTION_UP) {
            mHeaderTouched = FALSE;
        }
        *res = TRUE;
        return NOERROR;
    }
    return CAutoScrollListView::OnTouchEvent(ev, res);
}

Boolean CPinnedHeaderListView::SmoothScrollToPartition(
    /* [in] */ Int32 partition)
{
    if (mAdapter == NULL) {
        return FALSE;
    }
    Int32 position;
    mAdapter->GetScrollPositionForHeader(partition, &position);
    if (position == -1) {
        return FALSE;
    }

    Int32 offset = 0;
    for (Int32 i = 0; i < partition; i++) {
        AutoPtr<PinnedHeader*> header = mHeaders[i];
        if (header->mVisible) {
            offset += header->mHeight;
        }
    }
    Int32 count;
    GetHeaderViewsCount(&count);
    SmoothScrollToPositionFromTop(position + count, offset,
            DEFAULT_SMOOTH_SCROLL_DURATION);
    return TRUE;
}

void CPinnedHeaderListView::InvalidateIfAnimating()
{
    mAnimating = FALSE;
    for (Int32 i = 0; i < mSize; i++) {
        if (mHeaders[i]->mAnimating) {
            mAnimating = TRUE;
            Invalidate();
            return;
        }
    }
}

ECode CPinnedHeaderListView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Int64 currentTime = 0;
    if (mAnimating) {
        AutoPtr<ISystem> sysObj;
        CSystem::AcquireSingleton((ISystem**)&sysObj);
        sysObj->GetCurrentTimeMillis(&currentTime);
    }


    Int32 top = 0;
    Int32 right = 0;
    Int32 bottom;
    GetBottom(&bottom);
    Boolean hasVisibleHeaders = FALSE;
    for (Int32 i = 0; i < mSize; i++) {
        AutoPtr<PinnedHeader*> header = mHeaders[i];
        if (header->mVisible) {
            hasVisibleHeaders = TRUE;
            if (header->mState == BOTTOM && header->mY < bottom) {
                bottom = header->mY;
            }
            else if (header->mState == TOP || header->mState == FADING) {
                Int32 newTop = header->mY + header->mHeight;
                if (newTop > top) {
                    top = newTop;
                }
            }
        }
    }

    if (hasVisibleHeaders) {
        canvas->Save();
    }

    CAutoScrollListView::DispatchDraw(canvas);

    if (hasVisibleHeaders) {
        canvas->Restore();

        // If the first item is visible and if it has a positive top that is greater than the
        // first header's assigned y-value, use that for the first header's y value. This way,
        // the header inherits any padding applied to the list view.
        Int32 position;
        if (mSize > 0 && (GetFirstVisiblePosition(&position), position == 0)) {
            AutoPtr<IView> firstChild;
            GetChildAt(0, &firstChild);
            AutoPtr<PinnedHeader*> firstHeader = mHeaders[0];

            if (firstHeader != NULL) {
                Int32 firstHeaderTop = 0;
                if (firstChild != NULL) {
                    firstChild->GetTop(&firstHeaderTop);
                }
                firstHeader->mY = Math::Max(firstHeader->mY, firstHeaderTop);
            }
        }

        // First draw top headers, then the bottom ones to handle the Z axis correctly
        for (Int32 i = mSize; --i >= 0;) {
            AutoPtr<PinnedHeader*> header = mHeaders[i];
            if (header->mVisible && (header->mState == TOP || header->mState == FADING)) {
                DrawHeader(canvas, header, currentTime);
            }
        }

        for (Int32 i = 0; i < mSize; i++) {
            AutoPtr<PinnedHeader*> header = mHeaders[i];
            if (header->mVisible && header->mState == BOTTOM) {
                DrawHeader(canvas, header, currentTime);
            }
        }
    }

    InvalidateIfAnimating();
    return NOERROR;
}

void CPinnedHeaderListView::DrawHeader(
    /* [in] */ ICanvas* canvas,
    /* [in] */ PinnedHeader* header,
    /* [in] */ Int64 currentTime)
{
    if (header->mAnimating) {
        Int32 timeLeft = (Int32)(header->mTargetTime - currentTime);
        if (timeLeft <= 0) {
            header->mY = header->mTargetY;
            header->mVisible = header->mTargetVisible;
            header->mAnimating = FALSE;
        }
        else {
            header->mY = header->mTargetY + (header>mSourceY - header->mTargetY) * timeLeft
                    / mAnimationDuration;
        }
    }
    if (header->mVisible) {
        AutoPtr<IView> view = header->mView;
        Int32 saveCount;
        canvas->Save(&saveCount);
        Int32 translateX;
        if (ViewUtil::IsViewLayoutRtl(this)) {
            Int32 width, viewWidth;
            GetWidth(&width);
            view->GetWidth(&viewWidth);
            translateX = width - mHeaderPaddingStart - viewWidth;
        }
        else {
            translateX = mHeaderPaddingStart;
        }

        canvas->Translate(translateX, header->mY);
        if (header->mState == FADING) {
            Int32 width, height;
            view->GetWidth(&width);
            view->GetHeight(&height);
            mBounds->Set(0, 0, width, height);
            canvas->SaveLayerAlpha(mBounds, header->mAlpha, ICanvas::ALL_SAVE_FLAG);
        }
        view->Draw(canvas);
        canvas->RestoreToCount(saveCount);
    }
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
