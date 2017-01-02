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

// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TRACINGCONTROLLERANDROID_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TRACINGCONTROLLERANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/IntentFilter.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Widget::IToast;
using Elastos::IO::IFile;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Controller for Chrome's tracing feature.
  *
  * We don't have any UI per se. Just call startTracing() to start and
  * stopTracing() to stop. We'll report progress to the user with Toasts.
  *
  * If the host application registers this class's BroadcastReceiver, you can
  * also start and stop the tracer with a broadcast intent, as follows:
  * <ul>
  * <li>To start tracing: am broadcast -a org.chromium.content_shell_apk.GPU_PROFILER_START
  * <li>Add "-e file /foo/bar/xyzzy" to log trace data to a specific file.
  * <li>To stop tracing: am broadcast -a org.chromium.content_shell_apk.GPU_PROFILER_STOP
  * </ul>
  * Note that the name of these intents change depending on which application
  * is being traced, but the general form is [app package name].GPU_PROFILER_{START,STOP}.
  */
// @JNINamespace("content")
class TracingControllerElastos
    : public Object
{
public:
    class TracingBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TracingControllerElastos::TracingBroadcastReceiver")

        TracingBroadcastReceiver(
          /* [in] */ TracingControllerElastos* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TracingControllerElastos* mOwner;
    };

private:
    class TracingIntentFilter
        : public IntentFilter
    {
    public:
        TracingIntentFilter(
            /* [in] */ IContext* context);
    };

public:
    TracingControllerElastos(
        /* [in] */ IContext* context);

    /**
      * Get a BroadcastReceiver that can handle profiler intents.
      */
    virtual CARAPI_(AutoPtr<IBroadcastReceiver>) GetBroadcastReceiver();

    /**
      * Get an IntentFilter for profiler intents.
      */
    virtual CARAPI_(AutoPtr<IIntentFilter>) GetIntentFilter();

    /**
      * Register a BroadcastReceiver in the given context.
      */
    virtual CARAPI RegisterReceiver(
        /* [in] */ IContext* context);

    /**
      * Unregister the GPU BroadcastReceiver in the given context.
      * @param context
      */
    virtual CARAPI UnregisterReceiver(
        /* [in] */ IContext* context);

    /**
      * Returns true if we're currently profiling.
      */
    virtual CARAPI_(Boolean) IsTracing();

    /**
      * Returns the path of the current output file. Null if isTracing() false.
      */
    virtual CARAPI_(String) GetOutputPath();

    /**
      * Start profiling to a new file in the Downloads directory.
      *
      * Calls #startTracing(String, boolean, String, boolean) with a new timestamped filename.
      * @see #startTracing(String, boolean, String, boolean)
      */
    virtual CARAPI_(Boolean) StartTracing(
        /* [in] */ Boolean showToasts,
        /* [in] */ const String& categories,
        /* [in] */ Boolean recordContinuously);

    /**
      * Start profiling to the specified file. Returns true on success.
      *
      * Only one TracingControllerAndroid can be running at the same time. If another profiler
      * is running when this method is called, it will be cancelled. If this
      * profiler is already running, this method does nothing and returns false.
      *
      * @param filename The name of the file to output the profile data to.
      * @param showToasts Whether or not we want to show toasts during this profiling session.
      * When we are timing the profile run we might not want to incur extra draw overhead of showing
      * notifications about the profiling system.
      * @param categories Which categories to trace. See TracingControllerAndroid::BeginTracing()
      * (in content/public/browser/trace_controller.h) for the format.
      * @param recordContinuously Record until the user ends the trace. The trace buffer is fixed
      * size and we use it as a ring buffer during recording.
      */
    virtual CARAPI_(Boolean) StartTracing(
        /* [in] */ const String& filename,
        /* [in] */ Boolean showToasts,
        /* [in] */ const String& categories,
        /* [in] */ Boolean recordContinuously);

    /**
      * Stop profiling. This won't take effect until Chrome has flushed its file.
      */
    virtual CARAPI StopTracing();

    /**
      * Get known category groups.
      */
    virtual CARAPI GetCategoryGroups();

    static CARAPI_(void*) ElaTracingControllerElastosCallback_Init();

protected:
    /**
      * Called by native code when the profiler's output file is closed.
      */
    // @CalledByNative
    virtual CARAPI OnTracingStopped();

    // @Override
    CARAPI Finalize();

private:

    static CARAPI_(void) OnTracingStopped(
        /* [in] */ IInterface* obj);

    /**
      * Generates a unique filename to be used for tracing in the Downloads directory.
      */
    // @CalledByNative
    static CARAPI_(String) GenerateTracingFilePath();

    CARAPI InitializeNativeControllerIfNeeded();

    CARAPI LogAndToastError(
        /* [in] */ const String& str);

    // The |str| string needs to match the ones that adb_chrome_profiler looks for.
    CARAPI LogForProfiler(
        /* [in] */ const String& str);

    CARAPI ShowToast(
        /* [in] */ const String& str);

    CARAPI_(Handle64) NativeInit();

    CARAPI NativeDestroy(
        /* [in] */ Handle64 nativeTracingControllerElastos);

    CARAPI_(Boolean) NativeStartTracing(
        /* [in] */ Handle64 nativeTracingControllerElastos,
        /* [in] */ const String& categories,
        /* [in] */ Boolean recordContinuously);

    CARAPI NativeStopTracing(
        /* [in] */ Handle64 nativeTracingControllerElastos,
        /* [in] */ const String& filename);

    CARAPI_(Boolean) NativeGetKnownCategoryGroupsAsync(
        /* [in] */ Handle64 nativeTracingControllerElastos);

    CARAPI_(String) NativeGetDefaultCategories();

private:
    static const String TAG;
    static const String ACTION_START;
    static const String ACTION_STOP;
    static const String ACTION_LIST_CATEGORIES;
    static const String FILE_EXTRA;
    static const String CATEGORIES_EXTRA;
    static const String RECORD_CONTINUOUSLY_EXTRA;
    static const String DEFAULT_CHROME_CATEGORIES_PLACE_HOLDER;
    // These strings must match the ones expected by adb_profile_chrome.
    static const String PROFILER_STARTED_FMT;
    static const String PROFILER_FINISHED_FMT;
    AutoPtr<IContext> mContext;
    AutoPtr<TracingBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<TracingIntentFilter> mIntentFilter;
    Boolean mIsTracing;
    // We might not want to always show toasts when we start the profiler, especially if
    // showing the toast impacts performance.  This gives us the chance to disable them.
    Boolean mShowToasts;
    String mFilename;
    Handle64 mNativeTracingControllerElastos;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TRACINGCONTROLLERANDROID_H_
