// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCED_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCED_H_

#include <Elastos.Droid.Webkit.Native.h>
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerAdvancedComponent.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.Color;
// import android.util.AttributeSet;
// import android.view.LayoutInflater;
// import android.view.View;
// import android.widget.LinearLayout;
// import android.widget.SeekBar;
// import android.widget.SeekBar.OnSeekBarChangeListener;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::ColorPickerAdvancedComponent;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::IOnColorChangedListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Represents a more advanced way for the user to choose a color, based on selecting each of
  * the Hue, Saturation and Value attributes.s
  */
class ColorPickerAdvanced
    : public LinearLayout
    , public ISeekBarOnSeekBarChangeListener
{
public:
    CAR_INTERFACE_DECL()

    ColorPickerAdvanced(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ColorPickerAdvanced(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    ColorPickerAdvanced(
        /* [in] */ IContext* context);

    /**
      * Creates a new GradientDetails object from the parameters provided, initializes it,
      * and adds it to this advanced view.
      *
      * @param textResourceId The text to display for the label.
      * @param seekBarMax The maximum value of the seek bar for the gradient.
      * @param seekBarListener Object listening to when the user changes the seek bar.
      *
      * @return A new GradientDetails object initialized with the given parameters.
      */
    virtual CARAPI_(AutoPtr<ColorPickerAdvancedComponent>) CreateAndAddNewGradient(
        /* [in] */ Int32 textResourceId,
        /* [in] */ Int32 seekBarMax,
        /* [in] */ ISeekBarOnSeekBarChangeListener* seekBarListener);

    /**
      * Sets the listener for when the user changes the color.
      *
      * @param onColorChangedListener The object listening for the change in color.
      */
    virtual CARAPI SetListener(
        /* [in] */ IOnColorChangedListener* onColorChangedListener);

    /**
      * @return The color the user has currently chosen.
      */
    virtual CARAPI_(Int32) GetColor();

    /**
      * Sets the color that the user has currently chosen.
      *
      * @param color The currently chosen color.
      */
    virtual CARAPI SetColor(
        /* [in] */ Int32 color);

    /**
      * Callback for when a slider is updated on the advanced view.
      *
      * @param seekBar The color slider that was updated.
      * @param progress The new value of the color slider.
      * @param fromUser Whether it was the user the changed the value, or whether
      *            we were setting it up.
      */
    // @Override
    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

    // @Override
    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    // @Override
    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

private:
    CARAPI_(AutoPtr< ArrayOf<Float> >) MiddleInitMcurrenthsvvalues();

    /**
      * Initializes all the views and variables in the advanced view.
      */
    CARAPI Init();

    /**
      * Notifies the listener, if there is one, of a change in the selected color.
      */
    CARAPI NotifyColorChanged();

    /**
      * Updates only the hue gradient display with the hue value for the
      * currently selected color.
      */
    CARAPI UpdateHueGradient();

    /**
      * Updates only the saturation gradient display with the saturation value
      * for the currently selected color.
      */
    CARAPI UpdateSaturationGradient();

    /**
      * Updates only the Value gradient display with the Value amount for
      * the currently selected color.
      */
    CARAPI UpdateValueGradient();

    /**
      * Updates all the gradient displays to show the currently selected color.
      */
    CARAPI RefreshGradientComponents();

public:
    AutoPtr<ColorPickerAdvancedComponent> mHueDetails;
    AutoPtr<ColorPickerAdvancedComponent> mSaturationDetails;
    AutoPtr<ColorPickerAdvancedComponent> mValueDetails;

private:
    static const Int32 HUE_SEEK_BAR_MAX = 360;
    static const Int32 HUE_COLOR_COUNT = 7;
    static const Int32 SATURATION_SEEK_BAR_MAX = 100;
    static const Int32 SATURATION_COLOR_COUNT = 2;
    static const Int32 VALUE_SEEK_BAR_MAX = 100;
    static const Int32 VALUE_COLOR_COUNT = 2;
    AutoPtr<IOnColorChangedListener> mOnColorChangedListener;
    Int32 mCurrentColor;
    AutoPtr< ArrayOf<Float> > mCurrentHsvValues;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERADVANCED_H_

