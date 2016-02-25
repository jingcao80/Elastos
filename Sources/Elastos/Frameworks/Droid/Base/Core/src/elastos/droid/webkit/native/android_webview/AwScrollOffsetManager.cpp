
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/native/android_webview/AwScrollOffsetManager.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

const Int32 AwScrollOffsetManager::PAGE_SCROLL_OVERLAP;
const Int32 AwScrollOffsetManager::STD_SCROLL_ANIMATION_SPEED_PIX_PER_SEC;
const Int32 AwScrollOffsetManager::MAX_SCROLL_ANIMATION_DURATION_MILLISEC;

AwScrollOffsetManager::AwScrollOffsetManager(
    /* [in] */ Delegate* delegate,
    /* [in] */ IOverScroller* overScroller)
    : mDelegate(delegate)
    , mNativeScrollX(0)
    , mNativeScrollY(0)
    , mMaxHorizontalScrollOffset(0)
    , mMaxVerticalScrollOffset(0)
    , mContainerViewWidth(0)
    , mContainerViewHeight(0)
    , mProcessingTouchEvent(FALSE)
    , mWasFlinging(FALSE)
    , mApplyDeferredNativeScroll(FALSE)
    , mDeferredNativeScrollX(0)
    , mDeferredNativeScrollY(0)
    , mScroller(overScroller)
{
}

//----- Scroll range and extent calculation methods -------------------------------------------

Int32 AwScrollOffsetManager::ComputeHorizontalScrollRange()
{
    return mContainerViewWidth + mMaxHorizontalScrollOffset;
}

Int32 AwScrollOffsetManager::ComputeMaximumHorizontalScrollOffset()
{
    return mMaxHorizontalScrollOffset;
}

Int32 AwScrollOffsetManager::ComputeHorizontalScrollOffset()
{
    return mDelegate->GetContainerViewScrollX();
}

Int32 AwScrollOffsetManager::ComputeVerticalScrollRange()
{
    return mContainerViewHeight + mMaxVerticalScrollOffset;
}

Int32 AwScrollOffsetManager::ComputeMaximumVerticalScrollOffset()
{
    return mMaxVerticalScrollOffset;
}

Int32 AwScrollOffsetManager::ComputeVerticalScrollOffset()
{
    return mDelegate->GetContainerViewScrollY();
}

Int32 AwScrollOffsetManager::ComputeVerticalScrollExtent()
{
    return mContainerViewHeight;
}

//---------------------------------------------------------------------------------------------
/**
 * Called when the scroll range changes. This needs to be the size of the on-screen content.
 */
void AwScrollOffsetManager::SetMaxScrollOffset(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mMaxHorizontalScrollOffset = width;
    mMaxVerticalScrollOffset = height;
}

/**
 * Called when the physical size of the view changes.
 */
void AwScrollOffsetManager::SetContainerViewSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mContainerViewWidth = width;
    mContainerViewHeight = height;
}

void AwScrollOffsetManager::SyncScrollOffsetFromOnDraw()
{
    // Unfortunately apps override onScrollChanged without calling super which is why we need
    // to sync the scroll offset on every onDraw.
    OnContainerViewScrollChanged(mDelegate->GetContainerViewScrollX(),
            mDelegate->GetContainerViewScrollY());
}

void AwScrollOffsetManager::SetProcessingTouchEvent(
    /* [in] */ Boolean processingTouchEvent)
{
    assert(mProcessingTouchEvent != processingTouchEvent);
    mProcessingTouchEvent = processingTouchEvent;

    if (!mProcessingTouchEvent && mApplyDeferredNativeScroll) {
        mApplyDeferredNativeScroll = FALSE;
        ScrollNativeTo(mDeferredNativeScrollX, mDeferredNativeScrollY);
    }
}

// Called by the native side to scroll the container view.
void AwScrollOffsetManager::ScrollContainerViewTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mNativeScrollX = x;
    mNativeScrollY = y;

    Int32 scrollX = mDelegate->GetContainerViewScrollX();
    Int32 scrollY = mDelegate->GetContainerViewScrollY();
    Int32 deltaX = x - scrollX;
    Int32 deltaY = y - scrollY;
    Int32 scrollRangeX = ComputeMaximumHorizontalScrollOffset();
    Int32 scrollRangeY = ComputeMaximumVerticalScrollOffset();

    // We use overScrollContainerViewBy to be compatible with WebViewClassic which used this
    // method for handling both over-scroll as well as in-bounds scroll.
    mDelegate->OverScrollContainerViewBy(deltaX, deltaY, scrollX, scrollY,
            scrollRangeX, scrollRangeY, mProcessingTouchEvent);
}

Boolean AwScrollOffsetManager::IsFlingActive()
{
    Boolean flinging = FALSE;
    mScroller->ComputeScrollOffset(&flinging);
    mWasFlinging |= flinging;
    return flinging;
}

// Called by the native side to over-scroll the container view.
void AwScrollOffsetManager::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    // TODO(mkosiba): Once http://crbug.com/260663 and http://crbug.com/261239 are fixed it
    // should be possible to uncomment the following asserts:
    if (deltaX < 0) assert (mDelegate->GetContainerViewScrollX() == 0);
    if (deltaX > 0) assert (mDelegate->GetContainerViewScrollX() ==
             ComputeMaximumHorizontalScrollOffset());
    ScrollBy(deltaX, deltaY);
}

void AwScrollOffsetManager::ScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    if (deltaX == 0 && deltaY == 0) return;

    Int32 scrollX = mDelegate->GetContainerViewScrollX();
    Int32 scrollY = mDelegate->GetContainerViewScrollY();
    Int32 scrollRangeX = ComputeMaximumHorizontalScrollOffset();
    Int32 scrollRangeY = ComputeMaximumVerticalScrollOffset();

    // The android.view.View.overScrollBy method is used for both scrolling and over-scrolling
    // which is why we use it here.
    mDelegate->OverScrollContainerViewBy(deltaX, deltaY, scrollX, scrollY,
            scrollRangeX, scrollRangeY, mProcessingTouchEvent);
}

Int32 AwScrollOffsetManager::ClampHorizontalScroll(
    /* [in] */ Int32 scrollX)
{
    scrollX = Elastos::Core::Math::Max(0, scrollX);
    scrollX = Elastos::Core::Math::Min(ComputeMaximumHorizontalScrollOffset(), scrollX);
    return scrollX;
}

Int32 AwScrollOffsetManager::ClampVerticalScroll(
    /* [in] */ Int32 scrollY)
{
    scrollY = Elastos::Core::Math::Max(0, scrollY);
    scrollY = Elastos::Core::Math::Min(ComputeMaximumVerticalScrollOffset(), scrollY);
    return scrollY;
}

// Called by the View system as a response to the mDelegate.overScrollContainerViewBy call.
void AwScrollOffsetManager::OnContainerViewOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    // Clamp the scroll offset at (0, max).
    scrollX = ClampHorizontalScroll(scrollX);
    scrollY = ClampVerticalScroll(scrollY);

    mDelegate->ScrollContainerViewTo(scrollX, scrollY);

    // This is only necessary if the containerView scroll offset ends up being different
    // than the one set from native in which case we want the value stored on the native side
    // to reflect the value stored in the containerView (and not the other way around).
    ScrollNativeTo(mDelegate->GetContainerViewScrollX(), mDelegate->GetContainerViewScrollY());
}

// Called by the View system when the scroll offset had changed. This might not get called if
// the embedder overrides WebView#onScrollChanged without calling super.onScrollChanged. If
// this method does get called it is called both as a response to the embedder scrolling the
// view as well as a response to mDelegate.scrollContainerViewTo.
void AwScrollOffsetManager::OnContainerViewScrollChanged(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    ScrollNativeTo(x, y);
}

void AwScrollOffsetManager::ScrollNativeTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    x = ClampHorizontalScroll(x);
    y = ClampVerticalScroll(y);

    // We shouldn't do the store to native while processing a touch event since that confuses
    // the gesture processing logic.
    if (mProcessingTouchEvent) {
        mDeferredNativeScrollX = x;
        mDeferredNativeScrollY = y;
        mApplyDeferredNativeScroll = TRUE;
        return;
    }

    if (x == mNativeScrollX && y == mNativeScrollY)
        return;

    // The scrollNativeTo call should be a simple store, so it's OK to assume it always
    // succeeds.
    mNativeScrollX = x;
    mNativeScrollY = y;

    mDelegate->ScrollNativeTo(x, y);
}

// Called whenever some other touch interaction requires the fling gesture to be canceled.
void AwScrollOffsetManager::OnFlingCancelGesture()
{
    // TODO(mkosiba): Support speeding up a fling by flinging again.
    // http://crbug.com/265841
    mScroller->ForceFinished(TRUE);
}

// Called when a fling gesture is not handled by the renderer.
// We explicitly ask the renderer not to handle fling gestures targeted at the root
// scroll layer.
void AwScrollOffsetManager::OnUnhandledFlingStartEvent(
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY)
{
    FlingScroll(-velocityX, -velocityY);
}

// Starts the fling animation. Called both as a response to a fling gesture and as via the
// public WebView#flingScroll(int, int) API.
void AwScrollOffsetManager::FlingScroll(
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY)
{
    Int32 scrollX = mDelegate->GetContainerViewScrollX();
    Int32 scrollY = mDelegate->GetContainerViewScrollY();
    Int32 scrollRangeX = ComputeMaximumHorizontalScrollOffset();
    Int32 scrollRangeY = ComputeMaximumVerticalScrollOffset();

    mScroller->Fling (scrollX, scrollY, velocityX, velocityY, 0, scrollRangeX, 0, scrollRangeY);
    mDelegate->Invalidate();
}

// Called immediately before the draw to update the scroll offset.
void AwScrollOffsetManager::ComputeScrollAndAbsorbGlow(
    /* [in] */ OverScrollGlow* overScrollGlow)
{
    Boolean bOffset = FALSE;
    mScroller->ComputeScrollOffset(&bOffset);
    if (!bOffset && !mWasFlinging) {
        return;
    }
    mWasFlinging = FALSE;

    Int32 oldX = mDelegate->GetContainerViewScrollX();
    Int32 oldY = mDelegate->GetContainerViewScrollY();
    Int32 x = 0;
    mScroller->GetCurrX(&x);
    Int32 y = 0;
    mScroller->GetCurrY(&y);

    Int32 scrollRangeX = ComputeMaximumHorizontalScrollOffset();
    Int32 scrollRangeY = ComputeMaximumVerticalScrollOffset();

    if (overScrollGlow != NULL) {
        Float velocity;
        mScroller->GetCurrVelocity(&velocity);
        overScrollGlow->AbsorbGlow(x, y, oldX, oldY, scrollRangeX, scrollRangeY, velocity);
    }

    // The mScroller is configured not to go outside of the scrollable range, so this call
    // should never result in attempting to scroll outside of the scrollable region.
    ScrollBy(x - oldX, y - oldY);

    mDelegate->Invalidate();
}

Int32 AwScrollOffsetManager::ComputeDurationInMilliSec(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    Int32 distance = Elastos::Core::Math::Max(Elastos::Core::Math::Abs(dx), Elastos::Core::Math::Abs(dy));
    Int32 duration = distance * 1000 / STD_SCROLL_ANIMATION_SPEED_PIX_PER_SEC;
    return Elastos::Core::Math::Min(duration, MAX_SCROLL_ANIMATION_DURATION_MILLISEC);
}

Boolean AwScrollOffsetManager::AnimateScrollTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 scrollX = mDelegate->GetContainerViewScrollX();
    Int32 scrollY = mDelegate->GetContainerViewScrollY();

    x = ClampHorizontalScroll(x);
    y = ClampVerticalScroll(y);

    Int32 dx = x - scrollX;
    Int32 dy = y - scrollY;

    if (dx == 0 && dy == 0)
        return FALSE;

    mScroller->StartScroll(scrollX, scrollY, dx, dy, ComputeDurationInMilliSec(dx, dy));
    mDelegate->Invalidate();

    return TRUE;
}

/**
 * See {@link android.webkit.WebView#pageUp(boolean)}
 */
Boolean AwScrollOffsetManager::PageUp(
    /* [in] */ Boolean top)
{
    const Int32 scrollX = mDelegate->GetContainerViewScrollX();
    const Int32 scrollY = mDelegate->GetContainerViewScrollY();

    if (top) {
        // go to the top of the document
        return AnimateScrollTo(scrollX, 0);
    }
    Int32 dy = -mContainerViewHeight / 2;
    if (mContainerViewHeight > 2 * PAGE_SCROLL_OVERLAP) {
        dy = -mContainerViewHeight + PAGE_SCROLL_OVERLAP;
    }
    // animateScrollTo clamps the argument to the scrollable range so using (scrollY + dy) is
    // fine.
    return AnimateScrollTo(scrollX, scrollY + dy);
}

/**
 * See {@link android.webkit.WebView#pageDown(boolean)}
 */
Boolean AwScrollOffsetManager::PageDown(
    /* [in] */ Boolean bottom)
{
    const Int32 scrollX = mDelegate->GetContainerViewScrollX();
    const Int32 scrollY = mDelegate->GetContainerViewScrollY();

    if (bottom) {
        return AnimateScrollTo(scrollX, ComputeVerticalScrollRange());
    }
    Int32 dy = mContainerViewHeight / 2;
    if (mContainerViewHeight > 2 * PAGE_SCROLL_OVERLAP) {
        dy = mContainerViewHeight - PAGE_SCROLL_OVERLAP;
    }
    // animateScrollTo clamps the argument to the scrollable range so using (scrollY + dy) is
    // fine.
    return AnimateScrollTo(scrollX, scrollY + dy);
}

/**
 * See {@link android.webkit.WebView#requestChildRectangleOnScreen(View, Rect, boolean)}
 */
Boolean AwScrollOffsetManager::RequestChildRectangleOnScreen(
    /* [in] */ Int32 childOffsetX,
    /* [in] */ Int32 childOffsetY,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
{
    // TODO(mkosiba): WebViewClassic immediately returns false if a zoom animation is
    // in progress. We currently can't tell if one is happening.. should we instead cancel any
    // scroll animation when the size/pageScaleFactor changes?

    // TODO(mkosiba): Take scrollbar width into account in the screenRight/screenBotton
    // calculations. http://crbug.com/269032

    const Int32 scrollX = mDelegate->GetContainerViewScrollX();
    const Int32 scrollY = mDelegate->GetContainerViewScrollY();

    rect->Offset(childOffsetX, childOffsetY);

    Int32 screenTop = scrollY;
    Int32 screenBottom = scrollY + mContainerViewHeight;
    Int32 scrollYDelta = 0;

    Int32 top, bottom, width, left, right;
    rect->GetTop(&top);
    rect->GetBottom(&bottom);
    rect->GetWidth(&width);
    rect->GetLeft(&left);
    rect->GetRight(&right);

    if (bottom > screenBottom) {
        Int32 oneThirdOfScreenHeight = mContainerViewHeight / 3;
        if (width > 2 * oneThirdOfScreenHeight) {
            // If the rectangle is too tall to fit in the bottom two thirds
            // of the screen, place it at the top.
            scrollYDelta = top - screenTop;
        }
        else {
            // If the rectangle will still fit on screen, we want its
            // top to be in the top third of the screen.
            scrollYDelta = top - (screenTop + oneThirdOfScreenHeight);
        }
    }
    else if (top < screenTop) {
        scrollYDelta = top - screenTop;
    }

    Int32 screenLeft = scrollX;
    Int32 screenRight = scrollX + mContainerViewWidth;
    Int32 scrollXDelta = 0;

    if (right > screenRight && left > screenLeft) {
        if (width > mContainerViewWidth) {
            scrollXDelta += (left - screenLeft);
        }
        else {
            scrollXDelta += (right - screenRight);
        }
    }
    else if (left < screenLeft) {
        scrollXDelta -= (screenLeft - left);
    }

    if (scrollYDelta == 0 && scrollXDelta == 0) {
        return FALSE;
    }

    if (immediate) {
        ScrollBy(scrollXDelta, scrollYDelta);
        return TRUE;
    }
    else {
        return AnimateScrollTo(scrollX + scrollXDelta, scrollY + scrollYDelta);
    }
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
