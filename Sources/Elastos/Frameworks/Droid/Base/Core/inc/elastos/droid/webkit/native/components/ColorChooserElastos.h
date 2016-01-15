// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_COMPONENTS_COLORCHOOSERANDROID_H_
#define _ELASTOS_DROID_WEBKIT_COMPONENTS_COLORCHOOSERANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/ui/ColorPickerDialog.h"
#include "elastos/droid/webkit/native/ui/ColorSuggestion.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.components.web_contents_delegate_android;
// import android.content.Context;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.ui.ColorPickerDialog;
// import org.chromium.ui.ColorSuggestion;
// import org.chromium.ui.OnColorChangedListener;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Webkit::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Ui::ColorPickerDialog;
using Elastos::Droid::Webkit::Ui::ColorSuggestion;
using Elastos::Droid::Webkit::Ui::IOnColorChangedListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Components {

/**
  * ColorChooserAndroid communicates with the java ColorPickerDialog and the
  * native color_chooser_android.cc
  */
// @JNINamespace("web_contents_delegate_android")
class ColorChooserElastos
    : public Object
{
public:
    class InnerOnColorChangedListener
        : public Object
        , public IOnColorChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnColorChangedListener(
            /* [in] */ ColorChooserElastos* owner);

        // @Override
        CARAPI OnColorChanged(
            /* [in] */ Int32 color);

    private:
        ColorChooserElastos* mOwner;
    };

public:
    // @CalledByNative
    virtual CARAPI CloseColorChooser();

    // @CalledByNative
    static CARAPI_(AutoPtr<ColorChooserElastos>) CreateColorChooserElastos(
        /* [in] */ Int64 nativeColorChooserElastos,
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int32 initialColor,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    static CARAPI_(void*) ElaColorChooserElastosCallback_Init();

private:
    static CARAPI_(void) CloseColorChooser(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) CreateColorChooserElastos(
        /* [in] */ Int64 nativeColorChooserElastos,
        /* [in] */ IInterface* contentViewCore,
        /* [in] */ Int32 initialColor,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    ColorChooserElastos(
        /* [in] */ Int64 nativeColorChooserElastos,
        /* [in] */ IContext* context,
        /* [in] */ Int32 initialColor,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    CARAPI OpenColorChooser();

    // @CalledByNative ColorSuggestion
    static CARAPI_(AutoPtr< ArrayOf<IInterface*> >) CreateColorSuggestionArray(
        /* [in] */ Int32 size);

    /**
      * @param array ColorSuggestion array that should get a new suggestion added.
      * @param index Index in the array where to place a new suggestion.
      * @param color Color of the suggestion.
      * @param label Label of the suggestion.
      */
    // @CalledByNative
    static CARAPI_(void) AddToColorSuggestionArray(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 index,
        /* [in] */ Int32 color,
        /* [in] */ const String& label);

    // Implemented in color_chooser_android.cc
    CARAPI NativeOnColorChosen(
        /* [in] */ Int64 nativeColorChooserElastos,
        /* [in] */ Int32 color);

private:
    /*const*/ AutoPtr<ColorPickerDialog> mDialog;
    /*const*/ Int64 mNativeColorChooserElastos;
};

} // namespace Components
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_COMPONENTS_COLORCHOOSERANDROID_H_

