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
