
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content_Public {
namespace Browser {

/**
 * A class that is notified of events and state changes related to gesture processing from
 * the ContentViewCore.
 */
class GestureStateListener
    :public Object
{
public:
    /**
     * Called when the pinch gesture starts.
     */
    virtual CARAPI_(void) OnPinchStarted();

    /**
     * Called when the pinch gesture ends.
     */
    virtual CARAPI_(void) OnPinchEnded();

    /**
     * Called when a fling starts.
     */
    virtual CARAPI_(void) OnFlingStartGesture(
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy,
        /* [in] */ Int32 scrollOffsetY,
        /* [in] */ Int32 scrollExtentY);

    /**
     * Called when a fling is cancelled.
     */
    virtual CARAPI_(void) OnFlingCancelGesture();

    /**
     * Called when a fling has ended.
     */
    virtual CARAPI_(void) OnFlingEndGesture(
        /* [in] */ Int32 scrollOffsetY,
        /* [in] */ Int32 scrollExtentY);

    /**
     * Called when a fling event was not handled by the renderer.
     */
    virtual CARAPI_(void) OnUnhandledFlingStartEvent(
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    /**
     * Called to indicate that a scroll update gesture had been consumed by the page.
     * This callback is called whenever any layer is scrolled (like a frame or div). It is
     * not called when a JS touch handler consumes the event (preventDefault), it is not called
     * for JS-initiated scrolling.
     */
    virtual CARAPI_(void) OnScrollUpdateGestureConsumed();

    /*
     * Called when a scroll gesture has started.
     */
    virtual CARAPI_(void) OnScrollStarted(
        /* [in] */ Int32 scrollOffsetY,
        /* [in] */ Int32 scrollExtentY);

    /*
     * Called when a scroll gesture has stopped.
     */
    virtual CARAPI_(void) OnScrollEnded(
        /* [in] */ Int32 scrollOffsetY,
        /* [in] */ Int32 scrollExtentY);

    /*
     * Called when the scroll offsets or extents may have changed.
     */
    virtual CARAPI_(void) OnScrollOffsetOrExtentChanged(
        /* [in] */ Int32 scrollOffsetY,
        /* [in] */ Int32 scrollExtentY);

    /*
     * Called after a single-tap gesture event was dispatched to the renderer,
     * indicating whether or not the gesture was consumed.
     */
    virtual CARAPI_(void) OnSingleTap(
        /* [in] */ Boolean consumed,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);
};

} // namespace Browser
} // namespace Content_Public
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__
