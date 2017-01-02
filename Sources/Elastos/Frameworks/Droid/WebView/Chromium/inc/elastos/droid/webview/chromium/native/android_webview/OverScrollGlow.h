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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_OVERSCROLLGLOW_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_OVERSCROLLGLOW_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEdgeEffect;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This class manages the edge glow effect when a WebView is flung or pulled beyond the edges.
 */
class OverScrollGlow
:public Object
{
public:
    OverScrollGlow(
        /* [in] */ IContext* context,
        /* [in] */ IView* host);

    CARAPI_(void) SetShouldPull(
        /* [in] */ Boolean shouldPull);

    /**
     * Pull leftover touch scroll distance into one of the edge glows as appropriate.
     *
     * @param x Current X scroll offset
     * @param y Current Y scroll offset
     * @param oldX Old X scroll offset
     * @param oldY Old Y scroll offset
     * @param maxX Maximum range for horizontal scrolling
     * @param maxY Maximum range for vertical scrolling
     */
    CARAPI_(void) PullGlow(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 oldX,
        /* [in] */ Int32 oldY,
        /* [in] */ Int32 maxX,
        /* [in] */ Int32 maxY);

    /**
     * Absorb leftover fling velocity into one of the edge glows as appropriate.
     *
     * @param x Current X scroll offset
     * @param y Current Y scroll offset
     * @param oldX Old X scroll offset
     * @param oldY Old Y scroll offset
     * @param rangeX Maximum range for horizontal scrolling
     * @param rangeY Maximum range for vertical scrolling
     * @param currentFlingVelocity Current fling velocity
     */
    CARAPI_(void) AbsorbGlow(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 oldX,
        /* [in] */ Int32 oldY,
        /* [in] */ Int32 rangeX,
        /* [in] */ Int32 rangeY,
        /* [in] */ Float currentFlingVelocity);

    /**
     * Set touch delta values indicating the current amount of overscroll.
     *
     * @param deltaX
     * @param deltaY
     */
    CARAPI_(void) SetOverScrollDeltas(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    /**
     * Draw the glow effect along the sides of the widget.
     *
     * @param canvas Canvas to draw into, transformed into view coordinates.
     * @param maxScrollX maximum horizontal scroll offset
     * @param maxScrollY maximum vertical scroll offset
     * @return true if glow effects are still animating and the view should invalidate again.
     */
    CARAPI_(Boolean) DrawEdgeGlows(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 maxScrollX,
        /* [in] */ Int32 maxScrollY);

    /**
     * @return True if any glow is still animating
     */
    CARAPI_(Boolean) IsAnimating();

    /**
     * Release all glows from any touch pulls in progress.
     */
    CARAPI_(void) ReleaseAll();

private:
    AutoPtr<IView> mHostView;

    AutoPtr<IEdgeEffect> mEdgeGlowTop;
    AutoPtr<IEdgeEffect> mEdgeGlowBottom;
    AutoPtr<IEdgeEffect> mEdgeGlowLeft;
    AutoPtr<IEdgeEffect> mEdgeGlowRight;

    Int32 mOverScrollDeltaX;
    Int32 mOverScrollDeltaY;

    Boolean mShouldPull;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_OVERSCROLLGLOW_H__
