// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_COLORPICKERMOREBUTTON_H_
#define _ELASTOS_DROID_WEBKIT_UI_COLORPICKERMOREBUTTON_H_

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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_COLORPICKERMOREBUTTON_H_

