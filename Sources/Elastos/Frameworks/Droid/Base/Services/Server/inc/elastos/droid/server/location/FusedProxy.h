
#ifndef __ELASTOS_DROID_SERVER_LOCATION_FUSEPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_FUSEPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/server/location/FusedLocationHardwareSecure.h"
#include "elastos/droid/server/ServiceWatcher.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardware;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::Location::FusedLocationHardwareSecure;
using Elastos::Droid::Server::ServiceWatcher;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy that helps bind GCore FusedProvider implementations to the Fused Hardware instances.
 *
 * @hide
 */
class FusedProxy
    : public Object
{
private:
    class NewServiceWork
        : public Runnable
    {
    public:
        NewServiceWork(
            /* [in] */ FusedProxy* host);

        // @Override
        CARAPI Run();

    private:
        FusedProxy* mHost;
    };

public:
    /**
     * Creates an instance of the proxy and binds it to the appropriate FusedProvider.
     *
     * @param context           The context needed for construction.
     * @param handler           The handler needed for construction.
     * @param locationHardware  The instance of the Fused location hardware assigned to the proxy.
     *
     * @return An instance of the proxy if it could be bound, null otherwise.
     */
    static CARAPI_(AutoPtr<FusedProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IIFusedLocationHardware* locationHardware,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId);

    /**
     * Helper function to bind the FusedLocationHardware to the appropriate FusedProvider instance.
     *
     * @param locationHardware  The FusedLocationHardware instance to use for the binding operation.
     */
    CARAPI BindProvider(
        /* [in] */ IIFusedLocationHardware* locationHardware);

private:
    /**
     * Constructor of the class.
     * This is private as the class follows a factory pattern for construction.
     *
     * @param context           The context needed for construction.
     * @param handler           The handler needed for construction.
     * @param locationHardware  The instance of the Fused location hardware assigned to the proxy.
     */
    FusedProxy(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IIFusedLocationHardware* locationHardware,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId);

private:
    const String TAG;
    AutoPtr<ServiceWatcher> mServiceWatcher;
    AutoPtr<FusedLocationHardwareSecure> mLocationHardware;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_FUSEPROXY_H__

