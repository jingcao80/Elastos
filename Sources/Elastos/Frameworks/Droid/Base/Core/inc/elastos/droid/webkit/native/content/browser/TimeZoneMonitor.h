// wuweizuo automatic build .h file from .java file.
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_TIMEZONEMONITOR_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_TIMEZONEMONITOR_H_

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.content.browser;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.util.Log;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;

namespace Elastos {
namespace Droid {
namespace Webkit {
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

        CAR_INTERFACE_DECL();

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
        /* [in] */ Int64 nativePtr);

    /**
      * Stop listening for intents.
      */
    // @CalledByNative
    virtual CARAPI Stop();

    static CARAPI_(void*) ElaTimeZoneMonitorCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) GetInstance(
        /* [in] */ IInterface* context,
        /* [in] */ Int64 nativePtr);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    /**
      * Start listening for intents.
      * @param nativePtr The native content::TimeZoneMonitorAndroid to notify of time zone changes.
      */
    TimeZoneMonitor(
        /* [in] */ IContext* context,
        /* [in] */ Int64 nativePtr);

    /**
      * Native JNI call to content::TimeZoneMonitorAndroid::TimeZoneChanged.
      * See content/browser/time_zone_monitor_android.cc.
      */
    CARAPI NativeTimeZoneChangedFromJava(
        /* [in] */ Int64 nativeTimeZoneMonitorAndroid);

private:
    static const String TAG;
    AutoPtr<IContext> mAppContext;
    AutoPtr<IIntentFilter> mFilter;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    Int64 mNativePtr;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_TIMEZONEMONITOR_H_
