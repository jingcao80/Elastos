
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCERPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCERPROXY_H__

#include "elastos/droid/server/location/GeoFencerBase.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Location::IIGeoFencer;
using Elastos::Droid::Location::IIGeoFenceListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A class for proxying IGeoFenceProvider implementations.
 *
 * {@hide}
 */
class GeoFencerProxy : public GeoFencerBase
{
public:
    class GeoFenceListener
        : public Object
        , public IIGeoFenceListener
        , public IBinder
    {
    public:
        CARAPI constructor(
            /* [in] */ Handle64 host);

        CAR_INTERFACE_DECL()

        CARAPI GeoFenceExpired(
            /* [in] */ IPendingIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        GeoFencerProxy* mHost;
    };

private:
    class ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ GeoFencerProxy* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        GeoFencerProxy* mHost;
    };

public:
    static AutoPtr<GeoFencerProxy> GetGeoFencerProxy(
        /* [in] */ IContext* context,
        /* [in] */ const String& serviceName);

    CARAPI_(void) RemoveCaller(
        /* [in] */ Int32 uid);

protected:
    CARAPI_(Boolean) Start(
        /* [in] */ IGeoFenceParams* geofence);

    CARAPI_(Boolean) Stop(
        /* [in] */ IPendingIntent* intent);

private:
    GeoFencerProxy(
        /* [in] */ IContext* context,
        /* [in] */ const String& serviceName);

    CARAPI_(Boolean) EnsureGeoFencer();

    CARAPI_(void) Logv(
        /* [in] */ const String& s);

private:
    static const String TAG;
    static const Boolean LOGV_ENABLED;

    AutoPtr<IContext> mContext;
    AutoPtr<IIntent> mIntent;
    AutoPtr<IIGeoFencer> mGeoFencer;
    AutoPtr<ServiceConnection> mServiceConnection;
    AutoPtr<IBinder> mListener;//IIGeoFenceListener

    static AutoPtr<GeoFencerProxy> mGeoFencerProxy;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCERPROXY_H__

