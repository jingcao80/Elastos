#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWSCROLLOFFSETMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWSCROLLOFFSETMANAGER_H__

#include <Elastos.Droid.Widget.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/android_webview/OverScrollGlow.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Widget::IOverScroller;
using Elastos::Droid::Webkit::AndroidWebview::OverScrollGlow;

// import com.google.common.annotations.VisibleForTesting;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Takes care of syncing the scroll offset between the Android View system and the
 * InProcessViewRenderer.
 *
 * Unless otherwise values (sizes, scroll offsets) are in physical pixels.
 */
//@VisibleForTesting
class AwScrollOffsetManager
:public Object
{
public:
    /**
     * The interface that all users of AwScrollOffsetManager should implement.
     *
     * The unit of all the values in this delegate are physical pixels.
     */
    class Delegate
    :public Object
    {
    public:
        // Call View#overScrollBy on the containerView.
        virtual CARAPI_(void) OverScrollContainerViewBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Boolean isTouchEvent) = 0;

        // Call View#scrollTo on the containerView.
        virtual CARAPI_(void) ScrollContainerViewTo(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y) = 0;

        // Store the scroll offset in the native side. This should really be a simple store
        // operation, the native side shouldn't synchronously alter the scroll offset from within
        // this call.
        virtual CARAPI_(void) ScrollNativeTo(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y) = 0;

        virtual CARAPI_(Int32) GetContainerViewScrollX() = 0;

        virtual CARAPI_(Int32) GetContainerViewScrollY() = 0;

        virtual CARAPI_(void) Invalidate() = 0;
    };

public:
    AwScrollOffsetManager(
        /* [in] */ Delegate* delegate,
        /* [in] */ IOverScroller* overScroller);

    //----- Scroll range and extent calculation methods -------------------------------------------

    CARAPI_(Int32) ComputeHorizontalScrollRange();

    CARAPI_(Int32) ComputeMaximumHorizontalScrollOffset();

    CARAPI_(Int32) ComputeHorizontalScrollOffset();

    CARAPI_(Int32) ComputeVerticalScrollRange();

    CARAPI_(Int32) ComputeMaximumVerticalScrollOffset();

    CARAPI_(Int32) ComputeVerticalScrollOffset();

    CARAPI_(Int32) ComputeVerticalScrollExtent();

    //---------------------------------------------------------------------------------------------
    /**
     * Called when the scroll range changes. This needs to be the size of the on-screen content.
     */
    CARAPI_(void) SetMaxScrollOffset(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Called when the physical size of the view changes.
     */
    CARAPI_(void) SetContainerViewSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) SyncScrollOffsetFromOnDraw();

    CARAPI_(void) SetProcessingTouchEvent(
        /* [in] */ Boolean processingTouchEvent);

    // Called by the native side to scroll the container view.
    CARAPI_(void) ScrollContainerViewTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(Boolean) IsFlingActive();

    // Called by the native side to over-scroll the container view.
    CARAPI_(void) OverScrollBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    // Called by the View system as a response to the mDelegate.overScrollContainerViewBy call.
    CARAPI_(void) OnContainerViewOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    // Called by the View system when the scroll offset had changed. This might not get called if
    // the embedder overrides WebView#onScrollChanged without calling super.onScrollChanged. If
    // this method does get called it is called both as a response to the embedder scrolling the
    // view as well as a response to mDelegate.scrollContainerViewTo.
    CARAPI_(void) OnContainerViewScrollChanged(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // Called whenever some other touch interaction requires the fling gesture to be canceled.
    CARAPI_(void) OnFlingCancelGesture();

    // Called when a fling gesture is not handled by the renderer.
    // We explicitly ask the renderer not to handle fling gestures targeted at the root
    // scroll layer.
    CARAPI_(void) OnUnhandledFlingStartEvent(
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY);

    // Starts the fling animation. Called both as a response to a fling gesture and as via the
    // public WebView#flingScroll(int, int) API.
    CARAPI_(void) FlingScroll(
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY);

    // Called immediately before the draw to update the scroll offset.
    CARAPI_(void) ComputeScrollAndAbsorbGlow(
        /* [in] */ OverScrollGlow* overScrollGlow);

    /**
     * See {@link android.webkit.WebView#pageUp(boolean)}
     */
    CARAPI_(Boolean) PageUp(
        /* [in] */ Boolean top);

    /**
     * See {@link android.webkit.WebView#pageDown(boolean)}
     */
    CARAPI_(Boolean) PageDown(
        /* [in] */ Boolean bottom);

    /**
     * See {@link android.webkit.WebView#requestChildRectangleOnScreen(View, Rect, boolean)}
     */
    CARAPI_(Boolean) RequestChildRectangleOnScreen(
        /* [in] */ Int32 childOffsetX,
        /* [in] */ Int32 childOffsetY,
        /* [in] */ IRect* rect,
        /* [in] */ Boolean immediate);

private:
    CARAPI_(void) ScrollBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    CARAPI_(Int32) ClampHorizontalScroll(
        /* [in] */ Int32 scrollX);

    CARAPI_(Int32) ClampVerticalScroll(
        /* [in] */ Int32 scrollY);

    CARAPI_(void) ScrollNativeTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    static CARAPI_(Int32) ComputeDurationInMilliSec(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    CARAPI_(Boolean) AnimateScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

private:
    // Values taken from WebViewClassic.

    // The amount of content to overlap between two screens when using pageUp/pageDown methiods.
    static const Int32 PAGE_SCROLL_OVERLAP = 24;
    // Standard animated scroll speed.
    static const Int32 STD_SCROLL_ANIMATION_SPEED_PIX_PER_SEC = 480;
    // Time for the longest scroll animation.
    static const Int32 MAX_SCROLL_ANIMATION_DURATION_MILLISEC = 750;

    AutoPtr<Delegate> mDelegate;

    // Scroll offset as seen by the native side.
    Int32 mNativeScrollX;
    Int32 mNativeScrollY;

    // How many pixels can we scroll in a given direction.
    Int32 mMaxHorizontalScrollOffset;
    Int32 mMaxVerticalScrollOffset;

    // Size of the container view.
    Int32 mContainerViewWidth;
    Int32 mContainerViewHeight;

    // Whether we're in the middle of processing a touch event.
    Boolean mProcessingTouchEvent;

    // Don't skip computeScrollAndAbsorbGlow just because isFling is called in between.
    Boolean mWasFlinging;

    // Whether (and to what value) to update the native side scroll offset after we've finished
    // processing a touch event.
    Boolean mApplyDeferredNativeScroll;
    Int32 mDeferredNativeScrollX;
    Int32 mDeferredNativeScrollY;

    AutoPtr<IOverScroller> mScroller;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWSCROLLOFFSETMANAGER_H__
