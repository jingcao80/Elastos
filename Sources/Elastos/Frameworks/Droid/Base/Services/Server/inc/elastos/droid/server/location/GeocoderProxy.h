
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include "ServiceWatcher.h"

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::IGeocoderParams;
using Elastos::Droid::Location::IIGeocodeProvider;
using Elastos::Droid::Location::IAddress;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy for IGeocodeProvider implementations.
 */
class GeocoderProxy : public ElRefBase
{
private:

    static const String TAG;// = "GeocoderProxy";

public:

    static CARAPI_(AutoPtr<GeocoderProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ List<String>* initialPackageNames,
        /* [in] */ Int32 userId);

    GeocoderProxy(
        /* [in] */ IContext* context,
        /* [in] */ List<String>* initialPackageNames,
        /* [in] */ Int32 userId);

    CARAPI_(String) GetConnectedPackageName();

    CARAPI_(String) GetFromLocation(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [in] */ IObjectContainer** addrs);

    CARAPI_(String) GetFromLocationName(
        /* [in] */ const String& locationName,
        /* [in] */ Double lowerLeftLatitude,
        /* [in] */ Double lowerLeftLongitude,
        /* [in] */ Double upperRightLatitude,
        /* [in] */ Double upperRightLongitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IObjectContainer** addrs);

private:
    CARAPI_(Boolean) Bind ();

    CARAPI_(AutoPtr<IIGeocodeProvider>) GetService();

private:
    static const String SERVICE_ACTION;// = "com.android.location.service.GeocodeProvider";

    AutoPtr<IContext> mContext;
    AutoPtr<ServiceWatcher> mServiceWatcher;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__
