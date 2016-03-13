// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCEDCOMPONENT_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCEDCOMPONENT_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.drawable.GradientDrawable;
// import android.graphics.drawable.GradientDrawable.Orientation;
// import android.os.Build;
// import android.view.View;
// import android.widget.SeekBar;
// import android.widget.SeekBar.OnSeekBarChangeListener;
// import android.widget.TextView;
// import org.chromium.base.ApiCompatibilityUtils;

using Elastos::Droid::Graphics::Drawable::IGradientDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Encapsulates a single gradient view of the HSV color display, including its label, gradient
  * view and seek bar.
  *
  * Mirrors a "color_picker_advanced_component" layout.
  */
class ColorPickerAdvancedComponent
    : public Object
{
public:
    /**
      * Initializes the views.
      *
      * @param rootView View that contains all the content, such as the label, gradient view, etc.
      * @param textResourceId The resource ID of the text to show on the label.
      * @param seekBarMax The range of the seek bar.
      * @param seekBarListener The listener for when the seek bar value changes.
      */
    ColorPickerAdvancedComponent(
        /* [in] */ IView* rootView,
        /* [in] */ const Int32& textResourceId,
        /* [in] */ const Int32& seekBarMax,
        /* [in] */ ISeekBarOnSeekBarChangeListener* seekBarListener);

    /**
      * @return The value represented by this component, maintained by the seek bar progress.
      */
    virtual CARAPI_(Float) GetValue();

    /**
      * Sets the value of the component (by setting the seek bar value).
      *
      * @param newValue The value to give the component.
      */
    virtual CARAPI SetValue(
        /* [in] */ Float newValue);

    /**
      * Sets the colors for the gradient view to interpolate through.
      *
      * @param newColors The set of colors representing the interpolation points for the gradient.
      */
    virtual CARAPI SetGradientColors(
        /* [in] */ ArrayOf<Int32>* newColors);

private:
    // The view that displays the gradient.
    /*const*/ AutoPtr<IView> mGradientView;
    // The seek bar that allows the user to change the value of this component.
    /*const*/ AutoPtr<ISeekBar> mSeekBar;
    // The set of colors to interpolate the gradient through.
    AutoPtr< ArrayOf<Int32> > mGradientColors;
    // The Drawable that represents the gradient.
    AutoPtr<IGradientDrawable> mGradientDrawable;
    // The text label for the component.
    /*const*/ AutoPtr<ITextView> mText;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCEDCOMPONENT_H_

