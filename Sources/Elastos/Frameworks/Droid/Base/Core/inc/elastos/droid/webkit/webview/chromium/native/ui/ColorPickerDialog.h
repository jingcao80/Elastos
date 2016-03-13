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
    , public IOnColorChangedListener
{
public:
    class InnerOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialogInterface,
            /* [in] */ Int32 i);

    private:
        ColorPickerDialog* mOwner;
    };

    class InnerOnClickListener1
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener1(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialogInterface,
            /* [in] */ Int32 i);

    private:
        ColorPickerDialog* mOwner;
    };

    class InnerOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnCancelListener(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* arg0);

    private:
        ColorPickerDialog* mOwner;
    };

    class InnerButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerButtonOnClickListener(
            /* [in] */ ColorPickerDialog* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

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

