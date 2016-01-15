
#include "location/GeocoderProxy.h"

using Elastos::Droid::Location::EIID_IIGeocodeProvider;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GeocoderProxy::TAG("GeocoderProxy");
const String GeocoderProxy::SERVICE_ACTION("com.android.location.service.GeocodeProvider");

AutoPtr<GeocoderProxy> GeocoderProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ List<String>* initialPackageNames,
    /* [in] */ Int32 userId)
{
    AutoPtr<GeocoderProxy> proxy = new GeocoderProxy(context, initialPackageNames, userId);
    if (proxy->Bind()) {
        return proxy;
    } else {
        return NULL;
    }
}

GeocoderProxy::GeocoderProxy(
    /* [in] */ IContext* context,
    /* [in] */ List<String>* initialPackageNames,
    /* [in] */ Int32 userId)
{
    mContext = context;
    mServiceWatcher = new ServiceWatcher(mContext, TAG, SERVICE_ACTION, initialPackageNames, NULL, NULL, userId);
}

String GeocoderProxy::GetConnectedPackageName()
{
    return mServiceWatcher->GetBestPackageName();
}

String GeocoderProxy::GetFromLocation(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [in] */ IObjectContainer** addrs)
{
    AutoPtr<IIGeocodeProvider> provider = GetService();
    if (provider != NULL) {
        String temp;
        provider->GetFromLocation(latitude, longitude, maxResults, params, addrs, &temp);
        return temp;
    }

    return String("Service not Available");
}

String GeocoderProxy::GetFromLocationName(
    /* [in] */ const String& locationName,
    /* [in] */ Double lowerLeftLatitude,
    /* [in] */ Double lowerLeftLongitude,
    /* [in] */ Double upperRightLatitude,
    /* [in] */ Double upperRightLongitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [out] */ IObjectContainer** addrs)
{
    assert(addrs != NULL);
    *addrs = NULL;

    AutoPtr<IIGeocodeProvider> provider = GetService();
    if (provider != NULL) {

        String temp;
        provider->GetFromLocationName(locationName, lowerLeftLatitude,
                lowerLeftLongitude, upperRightLatitude, upperRightLongitude,
                maxResults, params, addrs, &temp);
        return temp;

    }

    return String("Service not Available");
}

Boolean GeocoderProxy::Bind()
{
    return mServiceWatcher->Start();
}

AutoPtr<IIGeocodeProvider> GeocoderProxy::GetService()
{
    AutoPtr<IBinder> binder = mServiceWatcher->GetBinder();
    AutoPtr<IIGeocodeProvider> rst = (IIGeocodeProvider*)(binder->Probe(EIID_IIGeocodeProvider));
    return rst;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
