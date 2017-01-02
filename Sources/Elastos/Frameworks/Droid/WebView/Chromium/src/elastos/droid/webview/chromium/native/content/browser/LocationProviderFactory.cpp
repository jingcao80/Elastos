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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LocationProviderFactory.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LocationProviderAdapter.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Location::CCriteria;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//     LocationProviderFactory::LocationProviderImpl::InnerRunnable
//=====================================================================
LocationProviderFactory::LocationProviderImpl::InnerRunnable::InnerRunnable(
    /* [in] */ LocationProviderFactory::LocationProviderImpl* owner,
    /* [in] */ ILocation* location)
    : mOwner(owner)
    , mLocation(location)
{
}

ECode LocationProviderFactory::LocationProviderImpl::InnerRunnable::Run()
{
    mOwner->UpdateNewLocation(mLocation);
    return NOERROR;
}

//=====================================================================
//            LocationProviderFactory::LocationProviderImpl
//=====================================================================
CAR_INTERFACE_IMPL(LocationProviderFactory::LocationProviderImpl, LocationProviderFactory::LocationProvider, ILocationListener);
const String LocationProviderFactory::LocationProviderImpl::TAG("LocationProvider");

LocationProviderFactory::LocationProviderImpl::LocationProviderImpl(
    /* [in] */ IContext* context)
    : mContext(context)
{
}

ECode LocationProviderFactory::LocationProviderImpl::Start(
    /* [in] */ Boolean gpsEnabled)
{
    UnregisterFromLocationUpdates();
    RegisterForLocationUpdates(gpsEnabled);
    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::Stop()
{
    UnregisterFromLocationUpdates();
    return NOERROR;
}

Boolean LocationProviderFactory::LocationProviderImpl::IsRunning()
{
    return mIsRunning;
}

ECode LocationProviderFactory::LocationProviderImpl::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    // Callbacks from the system location sevice are queued to this thread, so it's
    // possible that we receive callbacks after unregistering. At this point, the
    // native object will no longer exist.
    if (mIsRunning) {
        UpdateNewLocation(location);
    }

    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::UpdateNewLocation(
    /* [in] */ ILocation* location)
{
    Double latitude;
    location->GetLatitude(&latitude);
    Double longitude;
    location->GetLongitude(&longitude);
    Int64 timestamp;
    location->GetTime(&timestamp);
    Boolean hasAltitude;
    location->HasAltitude(&hasAltitude);
    Double altitude;
    location->GetAltitude(&altitude);
    Boolean hasAccuracy;
    location->HasAccuracy(&hasAccuracy);
    Float accuracy;
    location->GetAccuracy(&accuracy);
    Boolean hasHeading;
    location->HasBearing(&hasHeading);
    Float heading;
    location->GetBearing(&heading);
    Boolean hasSpeed;
    location->HasSpeed(&hasSpeed);
    Float speed;
    location->GetSpeed(&speed);
    LocationProviderAdapter::NewLocationAvailable(
           latitude, longitude,
           timestamp / 1000.0,
           hasAltitude, altitude,
           hasAccuracy, accuracy,
           hasHeading, heading,
           hasSpeed, speed);

    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::EnsureLocationManagerCreated()
{
    if (mLocationManager != NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(
            IContext::LOCATION_SERVICE,
            (IInterface**)&obj);
    mLocationManager = ILocationManager::Probe(obj);
    if (mLocationManager == NULL) {
        Slogger::E(TAG, "Could not get location manager.");
    }

    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::RegisterForLocationUpdates(
    /* [in] */ Boolean isGpsEnabled)
{
    EnsureLocationManagerCreated();
    if (UsePassiveOneShotLocation()) {
        return NOERROR;
    }

    assert(!mIsRunning);
    mIsRunning = TRUE;

    // We're running on the main thread. The C++ side is responsible to
    // bounce notifications to the Geolocation thread as they arrive in the mainLooper.
    // try {
        AutoPtr<ICriteria> criteria;
        CCriteria::New((ICriteria**)&criteria);
        mLocationManager->RequestLocationUpdates(0, 0, criteria, this,
               ThreadUtils::GetUiThreadLooper());
        if (isGpsEnabled) {
            criteria->SetAccuracy(ICriteria::Criteria_ACCURACY_FINE);
            mLocationManager->RequestLocationUpdates(0, 0, criteria, this,
                    ThreadUtils::GetUiThreadLooper());
        }
    // } catch (SecurityException e) {
    //     Log.e(TAG, "Caught security exception registering for location updates from " +
    //         "system. This should only happen in DumpRenderTree.");
    // } catch (IllegalArgumentException e) {
    //     Log.e(TAG, "Caught IllegalArgumentException registering for location updates.");
    // }

    return NOERROR;
}

ECode LocationProviderFactory::LocationProviderImpl::UnregisterFromLocationUpdates()
{
    if (mIsRunning) {
        mIsRunning = FALSE;
        mLocationManager->RemoveUpdates(this);
    }

    return NOERROR;
}

Boolean LocationProviderFactory::LocationProviderImpl::UsePassiveOneShotLocation()
{
    if (!IsOnlyPassiveLocationProviderEnabled()) {
        return FALSE;
    }

    // Do not request a location update if the only available location provider is
    // the passive one. Make use of the last known location and call
    // onLocationChanged directly.
    AutoPtr<ILocation> location;
    mLocationManager->GetLastKnownLocation(
             ILocationManager::PASSIVE_PROVIDER,
             (ILocation**)&location);

    if (location != NULL) {
        AutoPtr<IRunnable> runnable = new InnerRunnable(this, location);
        ThreadUtils::RunOnUiThread(runnable);
    }

    return TRUE;
}

Boolean LocationProviderFactory::LocationProviderImpl::IsOnlyPassiveLocationProviderEnabled()
{
    AutoPtr<IList> providers;
    mLocationManager->GetProviders(TRUE, (IList**)&providers);
    Int32 size;
    providers->GetSize(&size);
    AutoPtr<IInterface> strObj;
    providers->Get(0, (IInterface**)&strObj);
    ICharSequence* strC = ICharSequence::Probe(strObj);
    String str;
    strC->ToString(&str);
    return providers != NULL && size == 1
            && str.Equals(ILocationManager::PASSIVE_PROVIDER);
}

//=====================================================================
//                       LocationProviderFactory
//=====================================================================

AutoPtr<LocationProviderFactory::LocationProvider> LocationProviderFactory::sProviderImpl;

LocationProviderFactory::LocationProviderFactory()
{
}

ECode LocationProviderFactory::SetLocationProviderImpl(
    /* [in] */  LocationProviderFactory::LocationProvider* provider)
{
    assert(sProviderImpl == NULL);
    sProviderImpl = provider;

    return NOERROR;
}

AutoPtr<LocationProviderFactory::LocationProvider> LocationProviderFactory::Get(
    /* [in] */ IContext* context)
{
    if (sProviderImpl == NULL) {
        sProviderImpl = new LocationProviderImpl(context);
    }

    return sProviderImpl;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
