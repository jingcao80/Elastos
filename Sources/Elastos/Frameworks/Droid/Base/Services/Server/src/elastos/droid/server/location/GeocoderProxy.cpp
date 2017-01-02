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

#include "elastos/droid/server/location/GeocoderProxy.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IIGeocodeProvider;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GeocoderProxy::TAG("GeocoderProxy");
const String GeocoderProxy::SERVICE_ACTION("com.android.location.service.GeocodeProvider");

AutoPtr<GeocoderProxy> GeocoderProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId,
    /* [in] */ IHandler* handler)
{
    AutoPtr<GeocoderProxy> proxy = new GeocoderProxy(
        context,
        overlaySwitchResId,
        defaultServicePackageNameResId,
        initialPackageNameResId,
        handler);
    if (proxy->Bind()) {
        return proxy;
    }
    else {
        return NULL;
    }
}

GeocoderProxy::GeocoderProxy(
    /* [in] */ IContext* context,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId,
    /* [in] */ IHandler* handler)
{
    mContext = context;

    mServiceWatcher = new ServiceWatcher(mContext, TAG, SERVICE_ACTION, overlaySwitchResId,
        defaultServicePackageNameResId, initialPackageNameResId, NULL, handler);
}

Boolean GeocoderProxy::Bind()
{
    return mServiceWatcher->Start();
}

AutoPtr<IIGeocodeProvider> GeocoderProxy::GetService()
{
    return IIGeocodeProvider::Probe(mServiceWatcher->GetBinder());
}

ECode GeocoderProxy::GetConnectedPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mServiceWatcher->GetBestPackageName();
    return NOERROR;
}

ECode GeocoderProxy::GetFromLocation(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [out] */ IList** addrs,///*List<Address>*/
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(addrs)
    VALIDATE_NOT_NULL(name)
    AutoPtr<IIGeocodeProvider> provider = GetService();
    if (provider != NULL) {
        ECode ec = provider->GetFromLocation(latitude, longitude, maxResults, params, addrs, name);
        if (FAILED(ec)) {
            Logger::W(TAG, "%08x", ec);
            return E_REMOTE_EXCEPTION;
        }
    }
    *name = "Service not Available";
    return NOERROR;
}

ECode GeocoderProxy::GetFromLocationName(
    /* [in] */ const String& locationName,
    /* [in] */ Double lowerLeftLatitude,
    /* [in] */ Double lowerLeftLongitude,
    /* [in] */ Double upperRightLatitude,
    /* [in] */ Double upperRightLongitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [out] */ IList** addrs,///*List<Address>*/
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(addrs)
    VALIDATE_NOT_NULL(name)
    AutoPtr<IIGeocodeProvider> provider = GetService();
    if (provider != NULL) {
        ECode ec = provider->GetFromLocationName(locationName, lowerLeftLatitude,
            lowerLeftLongitude, upperRightLatitude, upperRightLongitude,
            maxResults, params, addrs, name);
        if (FAILED(ec)) {
            Logger::W(TAG, "%08x", ec);
            return E_REMOTE_EXCEPTION;
        }
    }
    *name = "Service not Available";
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
