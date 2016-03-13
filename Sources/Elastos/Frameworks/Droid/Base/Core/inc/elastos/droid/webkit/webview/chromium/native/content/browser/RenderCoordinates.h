// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_RENDERCOORDINATES_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_RENDERCOORDINATES_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Cached copy of all positions and scales (CSS-to-DIP-to-physical pixels)
  * reported from the renderer.
  * Provides wrappers and a utility class to help with coordinate transforms on the client side.
  * Provides the internally-visible set of update methods (called from ContentViewCore).
  *
  * Unless stated otherwise, all coordinates are in CSS (document) coordinate space.
  */
class RenderCoordinates : public Object
{
public:
    /**
      * Handles conversion of a point from window-relative-local-dip or screen-pix
      * to document-absolute-CSS space and vice versa.
      */
    class NormalizedPoint : public Object
    {
        friend class RenderCoordinates;
    public:
        /**
          * @return Absolute CSS (document) X coordinate of the point.
          */
        virtual CARAPI_(Float) GetXAbsoluteCss();

        /**
          * @return Absolute CSS (document) Y coordinate of the point.
          */
        virtual CARAPI_(Float) GetYAbsoluteCss();

        /**
          * @return Local device-scale-unadjusted X coordinate of the point.
          */
        virtual CARAPI_(Float) GetXLocalDip();

        /**
          * @return Local device-scale-unadjusted Y coordinate of the point.
          */
        virtual CARAPI_(Float) GetYLocalDip();

        /**
          * @return Physical (screen) X coordinate of the point.
          */
        virtual CARAPI_(Float) GetXPix();

        /**
          * @return Physical (screen) Y coordinate of the point.
          */
        virtual CARAPI_(Float) GetYPix();

        /**
          * Sets the point to the given absolute CSS (document) coordinates.
          */
        virtual CARAPI SetAbsoluteCss(
            /* [in] */ Float xCss,
            /* [in] */ Float yCss);

        /**
          * Sets the point to the given local device-scale-unadjusted coordinates.
          */
        virtual CARAPI SetLocalDip(
            /* [in] */ Float xDip,
            /* [in] */ Float yDip);

        /**
          * Sets the point to the given physical (screen) coordinates.
          */
        virtual CARAPI SetScreen(
            /* [in] */ Float xPix,
            /* [in] */ Float yPix);

    private:
        NormalizedPoint(
          /* [in] */ RenderCoordinates* owner);

    private:
        RenderCoordinates* mOwner;
        Float mXAbsoluteCss;
        Float mYAbsoluteCss;
    };

public:
    // Internally-visible set of update methods (used by ContentViewCore).
    virtual CARAPI Reset();

    virtual CARAPI UpdateContentSizeCss(
        /* [in] */ Float contentWidthCss,
        /* [in] */ Float contentHeightCss);

    virtual CARAPI SetDeviceScaleFactor(
        /* [in] */ Float deviceScaleFactor);

    virtual CARAPI UpdateFrameInfo(
        /* [in] */ Float scrollXCss,
        /* [in] */ Float scrollYCss,
        /* [in] */ Float contentWidthCss,
        /* [in] */ Float contentHeightCss,
        /* [in] */ Float viewportWidthCss,
        /* [in] */ Float viewportHeightCss,
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor,
        /* [in] */ Float contentOffsetYPix);

    /**
      * @return A helper to convert a point between between absolute CSS and local DIP spaces.
      */
    virtual CARAPI_(AutoPtr<NormalizedPoint>) CreateNormalizedPoint();

    /**
      * @return Horizontal scroll offset in CSS pixels.
      */
    virtual CARAPI_(Float) GetScrollX();

    /**
      * @return Vertical scroll offset in CSS pixels.
      */
    virtual CARAPI_(Float) GetScrollY();

    /**
      * @return Horizontal scroll offset in physical pixels.
      */
    virtual CARAPI_(Float) GetScrollXPix();

    /**
      * @return Vertical scroll offset in physical pixels.
      */
    virtual CARAPI_(Float) GetScrollYPix();

    /**
      * @return Horizontal scroll offset in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetScrollXPixInt();

    /**
      * @return Vertical scroll offset in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetScrollYPixInt();

    /**
      * @return Width of the content in CSS pixels.
      */
    virtual CARAPI_(Float) GetContentWidthCss();

    /**
      * @return Height of the content in CSS pixels.
      */
    virtual CARAPI_(Float) GetContentHeightCss();

    /**
      * @return Approximate width of the content in physical pixels.
      */
    virtual CARAPI_(Float) GetContentWidthPix();

    /**
      * @return Approximate height of the content in physical pixels.
      */
    virtual CARAPI_(Float) GetContentHeightPix();

    /**
      * @return Approximate width of the content in physical pixels (integer).
      */
    virtual CARAPI_(Int32) GetContentWidthPixInt();

    /**
      * @return Approximate height of the content in physical pixels (integer).
      */
    virtual CARAPI_(Int32) GetContentHeightPixInt();

    /**
      * @return Render-reported width of the viewport in CSS pixels.
      */
    virtual CARAPI_(Float) GetLastFrameViewportWidthCss();

    /**
      * @return Render-reported height of the viewport in CSS pixels.
      */
    virtual CARAPI_(Float) GetLastFrameViewportHeightCss();

    /**
      * @return Render-reported width of the viewport in physical pixels (approximate).
      */
    virtual CARAPI_(Float) GetLastFrameViewportWidthPix();

    /**
      * @return Render-reported height of the viewport in physical pixels (approximate).
      */
    virtual CARAPI_(Float) GetLastFrameViewportHeightPix();

    /**
      * @return Render-reported width of the viewport in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetLastFrameViewportWidthPixInt();

    /**
      * @return Render-reported height of the viewport in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetLastFrameViewportHeightPixInt();

    /**
      * @return The Physical on-screen Y offset amount below the top controls.
      */
    virtual CARAPI_(Float) GetContentOffsetYPix();

    /**
      * @return Current page scale factor (maps CSS pixels to DIP pixels).
      */
    virtual CARAPI_(Float) GetPageScaleFactor();

    /**
      * @return Minimum page scale factor to be used with the content.
      */
    virtual CARAPI_(Float) GetMinPageScaleFactor();

    /**
      * @return Maximum page scale factor to be used with the content.
      */
    virtual CARAPI_(Float) GetMaxPageScaleFactor();

    /**
      * @return Current device scale factor (maps DIP pixels to physical pixels).
      */
    virtual CARAPI_(Float) GetDeviceScaleFactor();

    /**
      * @return Maximum possible horizontal scroll in physical pixels.
      */
    virtual CARAPI_(Float) GetMaxHorizontalScrollPix();

    /**
      * @return Maximum possible vertical scroll in physical pixels.
      */
    virtual CARAPI_(Float) GetMaxVerticalScrollPix();

    /**
      * @return Maximum possible horizontal scroll in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetMaxHorizontalScrollPixInt();

    /**
      * @return Maximum possible vertical scroll in physical pixels (approx, integer).
      */
    virtual CARAPI_(Int32) GetMaxVerticalScrollPixInt();

    /**
      * @return Physical on-screen coordinate converted to local DIP.
      */
    virtual CARAPI_(Float) FromPixToDip(
        /* [in] */ Float pix);

    /**
      * @return Local DIP converted to physical coordinates.
      */
    virtual CARAPI_(Float) FromDipToPix(
        /* [in] */ Float dip);

    /**
      * @return Physical coordinate converted to local CSS.
      */
    virtual CARAPI_(Float) FromPixToLocalCss(
        /* [in] */ Float pix);

    /**
      * @return Local CSS converted to physical coordinates.
      */
    virtual CARAPI_(Float) FromLocalCssToPix(
        /* [in] */ Float css);

private:
    // Scroll offset from the native in CSS.
    Float mScrollXCss;
    Float mScrollYCss;
    // Content size from native in CSS.
    Float mContentWidthCss;
    Float mContentHeightCss;
    // Last-frame render-reported viewport size in CSS.
    Float mLastFrameViewportWidthCss;
    Float mLastFrameViewportHeightCss;
    // Cached page scale factor from native.
    Float mPageScaleFactor;
    Float mMinPageScaleFactor;
    Float mMaxPageScaleFactor;
    // Cached device density.
    Float mDeviceScaleFactor;
    Float mContentOffsetYPix;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_RENDERCOORDINATES_H_
