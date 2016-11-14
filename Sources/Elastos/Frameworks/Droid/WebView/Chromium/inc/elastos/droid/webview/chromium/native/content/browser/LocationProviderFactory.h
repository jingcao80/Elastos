// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERFACTORY_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERFACTORY_H_

#include "Elastos.Droid.Location.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::IBundle;
//using Elastos::Droid::Util::ILog;
//using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Factory to create a LocationProvider to allow us to inject
  * a mock for tests.
  */
class LocationProviderFactory
    : public Object
{
public:
    /**
      * LocationProviderFactory.get() returns an instance of this interface.
      */
    class LocationProvider
        : public Object
    {
    public:
        virtual CARAPI Start(
            /* [in] */ Boolean gpsEnabled) = 0;

        virtual CARAPI Stop() = 0;

        virtual CARAPI_(Boolean) IsRunning() = 0;
    };

private:
    /**
      * This is the core of android location provider. It is a separate class for clarity
      * so that it can manage all processing completely in the UI thread. The container class
      * ensures that the start/stop calls into this class are done in the UI thread.
      */
    class LocationProviderImpl
        : public LocationProviderFactory::LocationProvider
        , public ILocationListener
    {
    private:
        class InnerRunnable
            : public Runnable
        {
        public:
            InnerRunnable(
                /* [in] */ LocationProviderImpl* owner,
                /* [in] */ ILocation* location);

            // @Override
            CARAPI Run();

        private:
            LocationProviderImpl* mOwner;
            ILocation* mLocation;
        };

    public:
        LocationProviderImpl(
            /* [in] */ IContext* context);

        CAR_INTERFACE_DECL()

        /**
          * Start listening for location updates.
          * @param gpsEnabled Whether or not we're interested in high accuracy GPS.
          */
        // @Override
        CARAPI Start(
            /* [in] */ Boolean gpsEnabled);

        /**
          * Stop listening for location updates.
          */
        // @Override
        CARAPI Stop();

        /**
          * Returns true if we are currently listening for location updates, false if not.
          */
        // @Override
        CARAPI_(Boolean) IsRunning();

        // @Override
        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        // @Override
        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);

        // @Override
        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        // @Override
        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);

    private:
        CARAPI UpdateNewLocation(
            /* [in] */ ILocation* location);

        CARAPI EnsureLocationManagerCreated();

        /**
          * Registers this object with the location service.
          */
        CARAPI RegisterForLocationUpdates(
            /* [in] */ Boolean isGpsEnabled);

        /**
          * Unregisters this object from the location service.
          */
        CARAPI UnregisterFromLocationUpdates();

        CARAPI_(Boolean) UsePassiveOneShotLocation();

        /*
          * Checks if the passive location provider is the only provider available
          * in the system.
          */
        CARAPI_(Boolean) IsOnlyPassiveLocationProviderEnabled();

    private:
        // Log tag
        static const String TAG;
        AutoPtr<IContext> mContext;
        AutoPtr<ILocationManager> mLocationManager;
        Boolean mIsRunning;
    };

public:
    // @VisibleForTesting
    static CARAPI SetLocationProviderImpl(
        /* [in] */  LocationProvider* provider);

    static CARAPI_(AutoPtr<LocationProvider>) Get(
        /* [in] */ IContext* context);

private:
    LocationProviderFactory();

private:
    static AutoPtr<LocationProviderFactory::LocationProvider> sProviderImpl;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOCATIONPROVIDERFACTORY_H_
