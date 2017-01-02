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

// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERDIALOG_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERDIALOG_H_

#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerSimple.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerAdvanced.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.ui;
// import android.app.AlertDialog;
// import android.app.Dialog;
// import android.content.Context;
// import android.content.DialogInterface;
// import android.view.LayoutInflater;
// import android.view.View;
// import android.widget.Button;
// import android.widget.TextView;

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::ColorPickerAdvanced;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::ColorPickerSimple;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::IOnColorChangedListener;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * UI for the color chooser that shows on the Android platform as a result of
  * &lt;input type=color &gt; form element.
  */
class ColorPickerDialog
    : public AlertDialog
{
public:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnCancelListener
        , public IOnColorChangedListener
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialogInterface,
            /* [in] */ Int32 i);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* arg0);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnColorChanged(
            /* [in] */ Int32 color);
    private:
        ColorPickerDialog* mOwner;
    };

    class NegativeButtonListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NegativeButtonListener(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialogInterface,
            /* [in] */ Int32 i);

    private:
        ColorPickerDialog* mOwner;
    };


public:
    CAR_INTERFACE_DECL()

    /**
      * @param context The context the dialog is to run in.
      * @param listener The object to notify when the color is set.
      * @param color The initial color to set.
      * @param suggestions The list of suggestions.
      */
    ColorPickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ IOnColorChangedListener* listener,
        /* [in] */ Int32 color,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    /**
      * Listens to the ColorPicker for when the user has changed the selected color, and
      * updates the current color (the color shown in the title) accordingly.
      *
      * @param color The new color chosen by the user.
      */
    // @Override
    CARAPI OnColorChanged(
        /* [in] */ Int32 color);

private:
    /**
      * Hides the simple view (the default) and shows the advanced one instead, hiding the
      * "More" button at the same time.
      */
    CARAPI ShowAdvancedView();

    /**
      * Tries to notify any listeners that the color has been set.
      */
    CARAPI TryNotifyColorSet(
        /* [in] */ Int32 color);

    /**
      * Updates the internal cache of the currently selected color, updating the colorful little
      * box in the title at the same time.
      */
    CARAPI UpdateCurrentColor(
        /* [in] */ Int32 color);

private:
    AutoPtr<ColorPickerAdvanced> mAdvancedColorPicker;
    AutoPtr<ColorPickerSimple> mSimpleColorPicker;
    AutoPtr<IButton> mMoreButton;
    // The view up in the corner that shows the user the color they've currently selected.
    AutoPtr<IView> mCurrentColorView;
    AutoPtr<IOnColorChangedListener> mListener;
    Int32 mInitialColor;
    Int32 mCurrentColor;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORPICKERDIALOG_H_

