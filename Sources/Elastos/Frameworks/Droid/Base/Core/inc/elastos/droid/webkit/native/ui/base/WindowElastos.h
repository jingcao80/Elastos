    // Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_BASE_WINDOWANDROID_H_
#define _ELASTOS_DROID_WEBKIT_UI_BASE_WINDOWANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/ui/VSyncMonitor.h"

// package org.chromium.ui.base;
// import android.annotation.SuppressLint;
// import android.app.Activity;
// import android.app.PendingIntent;
// import android.content.ContentResolver;
// import android.content.Context;
// import android.content.Intent;
// import android.os.Bundle;
// import android.util.Log;
// import android.util.SparseArray;
// import android.widget.Toast;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.ui.VSyncMonitor;
// import java.lang.ref.WeakReference;
// import java.util.HashMap;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Webkit::Ui::VSyncMonitor;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

/**
  * The window base class that has the minimum functionality.
  */
// @JNINamespace("ui")
class WindowElastos
    : public Object
{
public:
    class InnerVSyncMonitorListener
        : public VSyncMonitor::Listener
    {
    public:
        InnerVSyncMonitorListener(
            /* [in] */ WindowElastos* owner);

        // @Override
        CARAPI OnVSync(
            /* [in] */ VSyncMonitor* monitor,
            /* [in] */ Int64 vsyncTimeMicros);

    private:
        WindowElastos* mOwner;
    };

    /**
      * An interface that intent callback objects have to implement.
      */
    class IntentCallback
        : public Object
    {
    public:
        /**
          * Handles the data returned by the requested intent.
          * @param window A window reference.
          * @param resultCode Result code of the requested intent.
          * @param contentResolver An instance of ContentResolver class for accessing returned data.
          * @param data The data returned by the intent.
          */
        virtual CARAPI OnIntentCompleted(
            /* [in] */ WindowElastos* window,
            /* [in] */ Int32 resultCode,
            /* [in] */ IContentResolver* contentResolver,
            /* [in] */ IIntent* data) = 0;
    };

public:
    /**
      * @param context The application context.
      */
    // @SuppressLint("UseSparseArrays")
    WindowElastos(
        /* [in] */ IContext* context);

    /**
      * Shows an intent and returns the results to the callback object.
      * @param intent   The PendingIntent that needs to be shown.
      * @param callback The object that will receive the results for the intent.
      * @param errorId  The ID of error string to be show if activity is paused before intent
      *                 results.
      * @return Whether the intent was shown.
      */
    virtual CARAPI_(Boolean) ShowIntent(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    /**
      * Shows an intent and returns the results to the callback object.
      * @param intent   The intent that needs to be shown.
      * @param callback The object that will receive the results for the intent.
      * @param errorId  The ID of error string to be show if activity is paused before intent
      *                 results.
      * @return Whether the intent was shown.
      */
    virtual CARAPI_(Boolean) ShowIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    /**
      * Shows an intent that could be canceled and returns the results to the callback object.
      * @param  intent   The PendingIntent that needs to be shown.
      * @param  callback The object that will receive the results for the intent.
      * @param  errorId  The ID of error string to be show if activity is paused before intent
      *                  results.
      * @return A non-negative request code that could be used for finishActivity, or
      *         START_INTENT_FAILURE if failed.
      */
    virtual CARAPI_(Int32) ShowCancelableIntent(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    /**
      * Shows an intent that could be canceled and returns the results to the callback object.
      * @param  intent   The intent that needs to be showed.
      * @param  callback The object that will receive the results for the intent.
      * @param  errorId  The ID of error string to be show if activity is paused before intent
      *                  results.
      * @return A non-negative request code that could be used for finishActivity, or
      *         START_INTENT_FAILURE if failed.
      */
    virtual CARAPI_(Int32) ShowCancelableIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    /**
      * Force finish another activity that you had previously started with showCancelableIntent.
      * @param requestCode The request code returned from showCancelableIntent.
      */
    virtual CARAPI CancelIntent(
        /* [in] */ Int32 requestCode);

    /**
      * Removes a callback from the list of pending intents, so that nothing happens if/when the
      * result for that intent is received.
      * @param callback The object that should have received the results
      * @return True if the callback was removed, false if it was not found.
     */
    virtual CARAPI_(Boolean) RemoveIntentCallback(
        /* [in] */ IntentCallback* callback);

    /**
      * Displays an error message with a provided error message string.
      * @param error The error message string to be displayed.
      */
    virtual CARAPI ShowError(
        /* [in] */ const String& error);

    /**
      * Displays an error message from the given resource id.
      * @param resId The error message string's resource id.
      */
    virtual CARAPI ShowError(
        /* [in] */ Int32 resId);

    /**
      * Broadcasts the given intent to all interested BroadcastReceivers.
      */
    virtual CARAPI SendBroadcast(
        /* [in] */ IIntent* intent);

    /**
      * @return A reference to owning Activity.  The returned WeakReference will never be null, but
      *         the contained Activity can be null (either if it has been garbage collected or if
      *         this is in the context of a WebView that was not created using an Activity).
      */
    virtual CARAPI_(AutoPtr<IWeakReference>) GetActivity();

    /**
      * @return The application context for this activity.
      */
    virtual CARAPI_(AutoPtr<IContext>) GetApplicationContext();

    /**
      * Saves the error messages that should be shown if any pending intents would return
      * after the application has been put onPause.
      * @param bundle The bundle to save the information in onPause
      */
    virtual CARAPI SaveInstanceState(
        /* [in] */ IBundle* bundle);

    /**
      * Restores the error messages that should be shown if any pending intents would return
      * after the application has been put onPause.
      * @param bundle The bundle to restore the information from onResume
      */
    virtual CARAPI RestoreInstanceState(
        /* [in] */ IBundle* bundle);

    /**
      * Responds to the intent result if the intent was created by the native window.
      * @param requestCode Request code of the requested intent.
      * @param resultCode Result code of the requested intent.
      * @param data The data returned by the intent.
      * @return Boolean value of whether the intent was started by the native window.
      */
    virtual CARAPI_(Boolean) OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /**
      * Tests that an activity is available to handle the passed in intent.
      * @param  intent The intent to check.
      * @return True if an activity is available to process this intent when started, meaning that
      *         Context.startActivity will not throw ActivityNotFoundException.
      */
    virtual CARAPI_(Boolean) CanResolveActivity(
        /* [in] */ IIntent* intent);

    /**
      * Destroys the c++ WindowElastos object if one has been created.
      */
    virtual CARAPI Destroy();

    /**
      * Returns a pointer to the c++ AndroidWindow object and calls the initializer if
      * the object has not been previously initialized.
      * @return A pointer to the c++ AndroidWindow.
      */
    virtual CARAPI_(Int64) GetNativePointer();

    static CARAPI_(void*) ElaWindowElastosCallback_Init();

protected:
    /**
      * Displays an error message for a nonexistent callback.
      * @param error The error message string to be displayed.
      */
    virtual CARAPI ShowCallbackNonExistentError(
        /* [in] */ const String& error);

private:
    static CARAPI_(void) RequestVSyncUpdate(
        /* [in] */ IInterface* obj);

    // @CalledByNative
    CARAPI RequestVSyncUpdate();

    CARAPI_(Int64) NativeInit(
        /* [in] */ Int64 vsyncPeriod);

    CARAPI NativeOnVSync(
        /* [in] */ Int64 nativeWindowElastos,
        /* [in] */ Int64 vsyncTimeMicros);

    CARAPI NativeDestroy(
        /* [in] */ Int64 nativeWindowElastos);

public:
    // A string used as a key to store intent errors in a bundle
    static const String WINDOW_CALLBACK_ERRORS;
    // Error code returned when an Intent fails to start an Activity.
    static const Int32 START_INTENT_FAILURE = -1;

protected:
    AutoPtr<IContext> mApplicationContext;
    AutoPtr<ISparseArray> mOutstandingIntents;
    // Ideally, this would be a SparseArray<String>, but there's no easy way to store a
    // SparseArray<String> in a bundle during saveInstanceState(). So we use a HashMap and suppress
    // the Android lint warning "UseSparseArrays".
    AutoPtr<IHashMap> mIntentErrors;

private:
    static const String TAG;
    // Native pointer to the c++ WindowElastos object.
    Int64 mNativeWindowElastos;
    AutoPtr<VSyncMonitor> mVSyncMonitor;
    AutoPtr<VSyncMonitor::Listener> mVSyncListener;
};

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_BASE_WINDOWANDROID_H_

