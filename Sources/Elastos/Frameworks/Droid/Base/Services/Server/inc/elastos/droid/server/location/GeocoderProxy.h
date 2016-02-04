
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/ServiceWatcher.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::IIGeocodeProvider;
using Elastos::Droid::Location::IGeocoderParams;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Server::ServiceWatcher;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy for IGeocodeProvider implementations.
 */
class GeocoderProxy
    : public Object
{
public:
    static CARAPI_(AutoPtr<GeocoderProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId,
        /* [in] */ IHandler* handler);

    CARAPI GetConnectedPackageName(
        /* [out] */ String* name);

    CARAPI GetFromLocation(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IList** addrs,///*List<Address>*/
        /* [out] */ String* name);

    CARAPI GetFromLocationName(
        /* [in] */ const String& locationName,
        /* [in] */ Double lowerLeftLatitude,
        /* [in] */ Double lowerLeftLongitude,
        /* [in] */ Double upperRightLatitude,
        /* [in] */ Double upperRightLongitude,
        /* [in] */ Int32 maxResults,
        /* [in] */ IGeocoderParams* params,
        /* [out] */ IList** addrs,///*List<Address>*/
        /* [out] */ String* name);

private:
    GeocoderProxy(
        /* [in] */ IContext* context,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId,
        /* [in] */ IHandler* handler);

    CARAPI_(Boolean) Bind();

    CARAPI_(AutoPtr<IIGeocodeProvider>) GetService();

private:
    const static String TAG;
    const static String SERVICE_ACTION;
    AutoPtr<IContext> mContext;
    AutoPtr<ServiceWatcher> mServiceWatcher;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOCODERPROXY_H__

