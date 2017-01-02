//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_GESTURESTATELISTENER_H__
