
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/GestureStateListener.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content_Public {
namespace Browser {

/**
 * Called when the pinch gesture starts.
 */
void GestureStateListener::OnPinchStarted()
{
}

/**
 * Called when the pinch gesture ends.
 */
void GestureStateListener::OnPinchEnded()
{
}

/**
 * Called when a fling starts.
 */
void GestureStateListener::OnFlingStartGesture(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy,
    /* [in] */ Int32 scrollOffsetY,
    /* [in] */ Int32 scrollExtentY)
{
}

/**
 * Called when a fling is cancelled.
 */
void GestureStateListener::OnFlingCancelGesture()
{
}

/**
 * Called when a fling has ended.
 */
void GestureStateListener::OnFlingEndGesture(
    /* [in] */ Int32 scrollOffsetY,
    /* [in] */ Int32 scrollExtentY)
{
}

/**
 * Called when a fling event was not handled by the renderer.
 */
void GestureStateListener::OnUnhandledFlingStartEvent(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
}

/**
 * Called to indicate that a scroll update gesture had been consumed by the page.
 * This callback is called whenever any layer is scrolled (like a frame or div). It is
 * not called when a JS touch handler consumes the event (preventDefault), it is not called
 * for JS-initiated scrolling.
 */
void GestureStateListener::OnScrollUpdateGestureConsumed()
{
}

/*
 * Called when a scroll gesture has started.
 */
void GestureStateListener::OnScrollStarted(
    /* [in] */ Int32 scrollOffsetY,
    /* [in] */ Int32 scrollExtentY)
{
}

/*
 * Called when a scroll gesture has stopped.
 */
void GestureStateListener::OnScrollEnded(
    /* [in] */ Int32 scrollOffsetY,
    /* [in] */ Int32 scrollExtentY)
{
}

/*
 * Called when the scroll offsets or extents may have changed.
 */
void GestureStateListener::OnScrollOffsetOrExtentChanged(
    /* [in] */ Int32 scrollOffsetY,
    /* [in] */ Int32 scrollExtentY)
{
}

/*
 * Called after a single-tap gesture event was dispatched to the renderer,
 * indicating whether or not the gesture was consumed.
 */
void GestureStateListener::OnSingleTap(
    /* [in] */ Boolean consumed,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
}

} // namespace Browser
} // namespace Content_Public
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
