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
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TIMEZONEMONITOR_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TIMEZONEMONITOR_H_

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Android implementation details for content::TimeZoneMonitorAndroid.
  */
// @JNINamespace("content")
class TimeZoneMonitor
    : public Object
{
public:
    class InnerBroadcastReceiver
        : public Object
        , public IBroadcastReceiver
    {
    public:
        InnerBroadcastReceiver(
            /* [in] */ TimeZoneMonitor* owner);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TimeZoneMonitor* mOwner;
    };

public:
    // @CalledByNative
    static CARAPI_(AutoPtr<TimeZoneMonitor>) GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ Handle64 nativePtr);

    /**
      * Stop listening for intents.
      */
    // @CalledByNative
    virtual CARAPI Stop();

    static CARAPI_(void*) ElaTimeZoneMonitorCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) GetInstance(
        /* [in] */ IInterface* context,
        /* [in] */ Handle64 nativePtr);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    /**
      * Start listening for intents.
      * @param nativePtr The native content::TimeZoneMonitorAndroid to notify of time zone changes.
      */
    TimeZoneMonitor(
        /* [in] */ IContext* context,
        /* [in] */ Handle64 nativePtr);

    /**
      * Native JNI call to content::TimeZoneMonitorAndroid::TimeZoneChanged.
      * See content/browser/time_zone_monitor_android.cc.
      */
    CARAPI NativeTimeZoneChangedFromJava(
        /* [in] */ Handle64 nativeTimeZoneMonitorAndroid);

private:
    static const String TAG;
    AutoPtr<IContext> mAppContext;
    AutoPtr<IIntentFilter> mFilter;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    Handle64 mNativePtr;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_TIMEZONEMONITOR_H_
