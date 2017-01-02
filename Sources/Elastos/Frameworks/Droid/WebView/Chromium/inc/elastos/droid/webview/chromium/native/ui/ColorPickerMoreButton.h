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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERMOREBUTTON_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERMOREBUTTON_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/Button.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.Canvas;
// import android.graphics.Color;
// import android.graphics.Paint;
// import android.util.AttributeSet;
// import android.widget.Button;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::Button;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Simple class that draws a white border around a button, purely for a UI change.
  */
class ColorPickerMoreButton
    : public Button
{
public:
    ColorPickerMoreButton(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ColorPickerMoreButton(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    /**
      * Sets up the paint to use for drawing the border.
      */
    virtual CARAPI Init();

protected:
    /**
      * Draws the border around the edge of the button.
      *
      * @param canvas The canvas to draw on.
      */
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    // A cache for the paint used to draw the border, so it doesn't have to be created in
    // every onDraw() call.
    AutoPtr<IPaint> mBorderPaint;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERMOREBUTTON_H_

