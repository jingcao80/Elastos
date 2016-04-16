// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_CLIPBOARD_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_CLIPBOARD_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.content.ClipData;
// import android.content.ClipboardManager;
// import android.content.Context;
// import android.widget.Toast;
// import org.chromium.base.ApiCompatibilityUtils;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.ui.R;

using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * Simple proxy that provides C++ code with an access pathway to the Android
  * clipboard.
  */
// @JNINamespace("ui")
class Clipboard
    : public Object
{
public:
    /**
      * Use the factory constructor instead.
      *
      * @param context for accessing the clipboard
      */
    Clipboard(
        /* [in] */ IContext* context);

    /**
      * Emulates the behavior of the now-deprecated
      * {@link android.text.ClipboardManager#setText(CharSequence)}, setting the
      * clipboard's current primary clip to a plain-text clip that consists of
      * the specified string.
      *
      * @param label will become the label of the clipboard's primary clip
      * @param text  will become the content of the clipboard's primary clip
      */
    virtual CARAPI SetText(
        /* [in] */ const String& label,
        /* [in] */ const String& text);

    /**
      * Emulates the behavior of the now-deprecated
      * {@link android.text.ClipboardManager#setText(CharSequence)}, setting the
      * clipboard's current primary clip to a plain-text clip that consists of
      * the specified string.
      *
      * @param text will become the content of the clipboard's primary clip
      */
    // @CalledByNative
    virtual CARAPI SetText(
        /* [in] */ const String& text);

    /**
      * Writes HTML to the clipboard, together with a plain-text representation
      * of that very data. This API is only available in Android JellyBean+ and
      * will be a no-operation in older versions.
      *
      * @param html  The HTML content to be pasted to the clipboard.
      * @param label The Plain-text label for the HTML content.
      * @param text  Plain-text representation of the HTML content.
      */
    virtual CARAPI SetHTMLText(
        /* [in] */ const String& html,
        /* [in] */ const String& label,
        /* [in] */ const String& text);

    /**
      * Writes HTML to the clipboard, together with a plain-text representation
      * of that very data. This API is only available in Android JellyBean+ and
      * will be a no-operation in older versions.
      *
      * @param html The HTML content to be pasted to the clipboard.
      * @param text Plain-text representation of the HTML content.
      */
    // @CalledByNative
    virtual CARAPI SetHTMLText(
        /* [in] */ const String& html,
        /* [in] */ const String& text);

//callback function declaration
    static CARAPI_(void*) ElaClipboardCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ IInterface* context);

    static CARAPI_(String) GetCoercedText(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetHTMLText(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SetText(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& text);

    static CARAPI_(void) SetHTMLText(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& html,
        /* [in] */ const String& text);

private:
    /**
      * Returns a new Clipboard object bound to the specified context.
      *
      * @param context for accessing the clipboard
      * @return the new object
      */
    // @CalledByNative
    static CARAPI_(AutoPtr<Clipboard>) Create(
        /* [in] */ IContext* context);

    /**
      * Emulates the behavior of the now-deprecated
      * {@link android.text.ClipboardManager#getText()} by invoking
      * {@link android.content.ClipData.Item#coerceToText(Context)} on the first
      * item in the clipboard (if any) and returning the result as a string.
      * <p>
      * This is quite different than simply calling {@link Object#toString()} on
      * the clip; consumers of this API should familiarize themselves with the
      * process described in
      * {@link android.content.ClipData.Item#coerceToText(Context)} before using
      * this method.
      *
      * @return a string representation of the first item on the clipboard, if
      *         the clipboard currently has an item and coercion of the item into
      *         a string is possible; otherwise, <code>null</code>
      */
    // @SuppressWarnings("javadoc")
    // @CalledByNative
    CARAPI_(String) GetCoercedText();

    /**
      * Gets the HTML text of top item on the primary clip on the Android clipboard.
      *
      * @return a Java string with the html text if any, or null if there is no html
      *         text or no entries on the primary clip.
      */
    // @CalledByNative
    CARAPI_(String) GetHTMLText();

    // @CalledByNative
    static CARAPI_(Boolean) IsHTMLClipboardSupported();

    CARAPI SetPrimaryClipNoException(
        /* [in] */ IClipData* clip);

private:
    // Necessary for coercing clipboard contents to text if they require
    // access to network resources, etceteras (e.g., URI in clipboard)
    AutoPtr<IContext> mContext;
    AutoPtr<IClipboardManager> mClipboardManager;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_CLIPBOARD_H_

