// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERADAPTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LocationProviderFactory.h"

// package org.chromium.content.browser;
// import android.content.Context;
// import com.google.common.annotations.VisibleForTesting;
// import org.chromium.base.CalledByNative;

using Elastos::Core::Runnable;
using Elastos::Droid::Content::IContext;
//using Elastos::Utility::Concurrent::IFutureTask;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Implements the Java side of LocationProviderAndroid.
  * Delegates all real functionality to the implementation
  * returned from LocationProviderFactory.
  * See detailed documentation on
  * content/browser/geolocation/android_location_api_adapter.h.
  * Based on android.webkit.GeolocationService.java
  */
// @VisibleForTesting
class LocationProviderAdapter
    : public Object
{
public:
    class StartRunnable
        : public Runnable
    {
    public:
        StartRunnable(
            /* [in] */ LocationProviderAdapter* owner,
            /* [in] */ const Boolean& gpsEnabled);

        // @Override
        CARAPI Run();

    private:
        LocationProviderAdapter* mOwner;
        const Boolean& mGpsEnabled;
    };

    class StopRunnable
        : public Runnable
    {
    public:
        StopRunnable(
            /* [in] */ LocationProviderAdapter* owner);

        // @Override
        CARAPI Run();

    private:
        LocationProviderAdapter* mOwner;
    };

public:
    // @CalledByNative
    static CARAPI_(AutoPtr<LocationProviderAdapter>) Create(
        /* [in] */ IContext* context);

    /**
      * Start listening for location updates until we're told to quit. May be
      * called in any thread.
      * @param gpsEnabled Whether or not we're interested in high accuracy GPS.
      */
    // @CalledByNative
    virtual CARAPI_(Boolean) Start(
        /* [in] */ const Boolean& gpsEnabled);

    /**
      * Stop listening for location updates. May be called in any thread.
      */
    // @CalledByNative
    virtual CARAPI Stop();

    /**
      * Returns true if we are currently listening for location updates, false if not.
      * Must be called only in the UI thread.
      */
    virtual CARAPI_(Boolean) IsRunning();

    static CARAPI NewLocationAvailable(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double timestamp,
        /* [in] */ Boolean hasAltitude,
        /* [in] */ Double altitude,
        /* [in] */ Boolean hasAccuracy,
        /* [in] */ Double accuracy,
        /* [in] */ Boolean hasHeading,
        /* [in] */ Double heading,
        /* [in] */ Boolean hasSpeed,
        /* [in] */ Double speed);

    static CARAPI NewErrorAvailable(
        /* [in] */ const String& message);

    static CARAPI_(void*) ElaLocationProviderAdapterCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ IInterface* context);

    static CARAPI_(Boolean) Start(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean gpsEnabled);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    LocationProviderAdapter(
        /* [in] */ IContext* context);

    // Native functions
    static CARAPI NativeNewLocationAvailable(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Double timeStamp,
        /* [in] */ Boolean hasAltitude,
        /* [in] */ Double altitude,
        /* [in] */ Boolean hasAccuracy,
        /* [in] */ Double accuracy,
        /* [in] */ Boolean hasHeading,
        /* [in] */ Double heading,
        /* [in] */ Boolean hasSpeed,
        /* [in] */ Double speed);

    static CARAPI NativeNewErrorAvailable(
        /* [in] */ const String& message);

private:
    // Delegate handling the real work in the main thread.
    AutoPtr<LocationProviderFactory::LocationProvider> mImpl;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERADAPTER_H_
