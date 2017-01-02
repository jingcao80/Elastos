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

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_VALIDATIONMESSAGEBUBBLE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_VALIDATIONMESSAGEBUBBLE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/RenderCoordinates.h"

// package org.chromium.components.web_contents_delegate_android;
// import android.graphics.Point;
// import android.graphics.RectF;
// import android.text.TextUtils;
// import android.view.Gravity;
// import android.view.View;
// import android.view.ViewGroup;
// import android.widget.PopupWindow;
// import android.widget.RelativeLayout;
// import android.widget.TextView;
// import org.chromium.base.ApiCompatibilityUtils;
// import org.chromium.base.CalledByNative;
// import org.chromium.content.R;
// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.content.browser.RenderCoordinates;

using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::RenderCoordinates;
using Elastos::Droid::Widget::IPopupWindow;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

/**
  * This class is an implementation of validation message bubble UI.
  */
class ValidationMessageBubble
    : public Object
{
public:
    static CARAPI_(void*) ElaValidationMessageBubbleCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateAndShow(
        /* [in] */ IInterface* contentViewCore,
        /* [in] */ Int32 anchorX,
        /* [in] */ Int32 anchorY,
        /* [in] */ Int32 anchorWidth,
        /* [in] */ Int32 anchorHeight,
        /* [in] */ const String& mainText,
        /* [in] */ const String& subText);

    static CARAPI_(void) Close(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SetPositionRelativeToAnchor(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* contentViewCore,
        /* [in] */ Int32 anchorX,
        /* [in] */ Int32 anchorY,
        /* [in] */ Int32 anchorWidth,
        /* [in] */ Int32 anchorHeight);

    ValidationMessageBubble(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ IRectF* anchor,
        /* [in] */ const String& mainText,
        /* [in] */ const String& subText);

    /**
      * Creates a popup window to show the specified messages, and show it on
      * the specified anchor rectangle.
      *
      * @param contentViewCore The ContentViewCore object to provide various
      *                        information.
      * @param anchorX Anchor position in the CSS unit.
      * @param anchorY Anchor position in the CSS unit.
      * @param anchorWidth Anchor size in the CSS unit.
      * @param anchorHeight Anchor size in the CSS unit.
      * @param mainText The main message. It will shown at the top of the popup
      *                 window, and its font size is larger.
      * @param subText The sub message. It will shown below the main message, and
      *                its font size is smaller.
      */
    // @CalledByNative
    static CARAPI_(AutoPtr<ValidationMessageBubble>) CreateAndShow(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int32 anchorX,
        /* [in] */ Int32 anchorY,
        /* [in] */ Int32 anchorWidth,
        /* [in] */ Int32 anchorHeight,
        /* [in] */ const String& mainText,
        /* [in] */ const String& subText);

    // @CalledByNative
    CARAPI Close();

    /**
      * Moves the popup window on the specified anchor rectangle.
      *
      * @param contentViewCore The ContentViewCore object to provide various
      *                        information.
      * @param anchorX Anchor position in the CSS unit.
      * @param anchorY Anchor position in the CSS unit.
      * @param anchorWidth Anchor size in the CSS unit.
      * @param anchorHeight Anchor size in the CSS unit.
      */
    // @CalledByNative
    CARAPI SetPositionRelativeToAnchor(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int32 anchorX,
        /* [in] */ Int32 anchorY,
        /* [in] */ Int32 anchorWidth,
        /* [in] */ Int32 anchorHeight);

    static CARAPI_(AutoPtr<IRectF>) MakePixRectInScreen(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int32 anchorX,
        /* [in] */ Int32 anchorY,
        /* [in] */ Int32 anchorWidth,
        /* [in] */ Int32 anchorHeight);

    static CARAPI_(Float) GetWebViewOffsetYPixInScreen(
        /* [in] */ ContentViewCore* contentViewCore);

    static CARAPI UpdateTextViews(
        /* [in] */ IViewGroup* root,
        /* [in] */ const String& mainText,
        /* [in] */ const String& subText);

    CARAPI Measure(
        /* [in] */ RenderCoordinates* coordinates);

    CARAPI_(Float) GetAnchorOffset();

    /**
      * This adjusts the position if the popup protrudes the web view.
      */
    CARAPI_(AutoPtr<IPoint>) AdjustWindowPosition(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

private:
    AutoPtr<IPopupWindow> mPopup;
};

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_COMPONENTS_VALIDATIONMESSAGEBUBBLE_H_


