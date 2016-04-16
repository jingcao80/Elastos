// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_UIUTILS_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_UIUTILS_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.Bitmap;
// import android.graphics.Canvas;
// import android.graphics.Rect;
// import android.util.Log;
// import android.view.SurfaceView;
// import android.view.View;
// import android.view.ViewGroup;
// import android.view.inputmethod.InputMethodManager;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Utility functions for common Android UI tasks.
  * This class is not supposed to be instantiated.
  */
class UiUtils
    : public Object
{
public:
    /**
      * A delegate that can be implemented to override whether or not keyboard detection will be
      * used.
      */
    class KeyboardShowingDelegate
        : public Object
    {
    public:
        /**
          * Will be called to determine whether or not to detect if the keyboard is visible.
          * @param context A {@link Context} instance.
          * @param view    A {@link View}.
          * @return        Whether or not the keyboard check should be disabled.
          */
        virtual CARAPI_(Boolean) DisableKeyboardCheck(
            /* [in] */ IContext* context,
            /* [in] */ IView* view) = 0;
    };

public:
    /**
      * Allows setting a delegate to override the default software keyboard visibility detection.
      * @param delegate A {@link KeyboardShowingDelegate} instance.
      */
    static CARAPI SetKeyboardShowingDelegate(
        /* [in] */ KeyboardShowingDelegate* delegate);

    /**
      * Shows the software keyboard if necessary.
      * @param view The currently focused {@link View}, which would receive soft keyboard input.
      */
    static CARAPI ShowKeyboard(
        /* [in] */ IView* view);

    /**
      * Hides the keyboard.
      * @param view The {@link View} that is currently accepting input.
      * @return Whether the keyboard was visible before.
      */
    static CARAPI_(Boolean) HideKeyboard(
        /* [in] */ IView* view);

    /**
      * Detects whether or not the keyboard is showing.  This is a best guess as there is no
      * standardized/foolproof way to do this.
      * @param context A {@link Context} instance.
      * @param view    A {@link View}.
      * @return        Whether or not the software keyboard is visible and taking up screen space.
      */
    static CARAPI_(Boolean) IsKeyboardShowing(
        /* [in] */ IContext* context,
        /* [in] */ IView* view);

    /**
      * Inserts a {@link View} into a {@link ViewGroup} after directly before a given {@View}.
      * @param container The {@link View} to add newView to.
      * @param newView The new {@link View} to add.
      * @param existingView The {@link View} to insert the newView before.
      * @return The index where newView was inserted, or -1 if it was not inserted.
      */
    static CARAPI_(Int32) InsertBefore(
        /* [in] */ IViewGroup* container,
        /* [in] */ IView* newView,
        /* [in] */ IView* existingView);

    /**
      * Inserts a {@link View} into a {@link ViewGroup} after directly after a given {@View}.
      * @param container The {@link View} to add newView to.
      * @param newView The new {@link View} to add.
      * @param existingView The {@link View} to insert the newView after.
      * @return The index where newView was inserted, or -1 if it was not inserted.
      */
    static CARAPI_(Int32) InsertAfter(
        /* [in] */ IViewGroup* container,
        /* [in] */ IView* newView,
        /* [in] */ IView* existingView);

    /**
      * Generates a scaled screenshot of the given view.  The maximum size of the screenshot is
      * determined by maximumDimension.
      *
      * @param currentView      The view to generate a screenshot of.
      * @param maximumDimension The maximum width or height of the generated screenshot.  The bitmap
      *                         will be scaled to ensure the maximum width or height is equal to or
      *                         less than this.  Any value <= 0, will result in no scaling.
      * @param bitmapConfig     Bitmap config for the generated screenshot (ARGB_8888 or RGB_565).
      * @return The screen bitmap of the view or null if a problem was encountered.
      */
    static CARAPI_(AutoPtr<IBitmap>) GenerateScaledScreenshot(
        /* [in] */ IView* currentView,
        /* [in] */ Int32 maximumDimension,
        /* [in] */ BitmapConfig bitmapConfig);

private:
    /**
      * Guards this class from being instantiated.
      */
    UiUtils();

    static CARAPI_(Int32) InsertView(
        /* [in] */ IViewGroup* container,
        /* [in] */ IView* newView,
        /* [in] */ IView* existingView,
        /* [in] */ Boolean after);

    static CARAPI PrepareViewHierarchyForScreenshot(
        /* [in] */ IView* view,
        /* [in] */ Boolean takingScreenshot);

private:
    static const String TAG;
    /** The minimum size of the bottom margin below the app to detect a keyboard. */
    static const Float KEYBOARD_DETECT_BOTTOM_THRESHOLD_DP;
    /** A delegate that allows disabling keyboard visibility detection. */
    static AutoPtr<UiUtils::KeyboardShowingDelegate> sKeyboardShowingDelegate;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_UIUTILS_H_

