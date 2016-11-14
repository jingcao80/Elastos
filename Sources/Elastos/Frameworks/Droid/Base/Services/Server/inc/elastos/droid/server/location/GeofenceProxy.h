
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/server/ServiceWatcher.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Hardware::Location::IIGeofenceHardware;
using Elastos::Droid::Location::IIGpsGeofenceHardware;
using Elastos::Droid::Location::IIFusedGeofenceHardware;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::ServiceWatcher;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * @hide
 */
class GeofenceProxy
    : public Object
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ GeofenceProxy* host);

        // @Override
        CARAPI Run();

    private:
        GeofenceProxy* mHost;
    };

    class MyServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        MyServiceConnection(
            /* [in] */ GeofenceProxy* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        GeofenceProxy* mHost;
    };

    // This needs to be reworked, when more services get added,
    // Might need a state machine or add a framework utility class,
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("GeofenceProxy::MyHandler")

        MyHandler(
            /* [in] */ GeofenceProxy* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceProxy* mHost;
    };

public:
    static CARAPI_(AutoPtr<GeofenceProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId,
        /* [in] */ IHandler* handler,
        /* [in] */ IIGpsGeofenceHardware* gpsGeofence,
        /* [in] */ IIFusedGeofenceHardware* fusedGeofenceHardware);

private:
    GeofenceProxy(
        /* [in] */ IContext* context,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId,
        /* [in] */ IHandler* handler,
        /* [in] */ IIGpsGeofenceHardware* gpsGeofence,
        /* [in] */ IIFusedGeofenceHardware* fusedGeofenceHardware);

    CARAPI_(Boolean) BindGeofenceProvider();

    CARAPI_(void) BindHardwareGeofence();

    CARAPI_(void) SetGeofenceHardwareInProviderLocked();

    CARAPI_(void) SetGpsGeofenceLocked();

    CARAPI_(void) SetFusedGeofenceLocked();

private:
    const static String TAG;
    const static String SERVICE_ACTION;
    AutoPtr<ServiceWatcher> mServiceWatcher;
    AutoPtr<IContext> mContext;
    AutoPtr<IIGpsGeofenceHardware> mGpsGeofenceHardware;
    AutoPtr<IIFusedGeofenceHardware> mFusedGeofenceHardware;

    // Access to mGeofenceHardware needs to be synchronized by mLock.
    AutoPtr<IIGeofenceHardware> mGeofenceHardware;

    const static Int32 GEOFENCE_PROVIDER_CONNECTED = 1;
    const static Int32 GEOFENCE_HARDWARE_CONNECTED = 2;
    const static Int32 GEOFENCE_HARDWARE_DISCONNECTED = 3;
    const static Int32 GEOFENCE_GPS_HARDWARE_CONNECTED = 4;
    const static Int32 GEOFENCE_GPS_HARDWARE_DISCONNECTED = 5;

    AutoPtr<Runnable> mRunnable;
    AutoPtr<IServiceConnection> mServiceConnection;
    AutoPtr<Handler> mHandler;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCEPROXY_H__

