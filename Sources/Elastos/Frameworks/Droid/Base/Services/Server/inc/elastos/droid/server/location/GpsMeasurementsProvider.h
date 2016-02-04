
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSMEASUREMENTSPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSMEASUREMENTSPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/server/location/RemoteListenerHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Location::IGpsMeasurementsEvent;
using Elastos::Droid::Server::Location::IListenerOperation;
using Elastos::Droid::Server::Location::RemoteListenerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * An base implementation for GPS measurements provider.
 * It abstracts out the responsibility of handling listeners, while still allowing technology
 * specific implementations to be built.
 *
 * @hide
 */
class GpsMeasurementsProvider
    : public RemoteListenerHelper
{
private:
    class MyOperation
        : public Object
        , public IListenerOperation
    {
    public:
        CAR_INTERFACE_DECL()

        MyOperation(
            /* [in] */ IGpsMeasurementsEvent* event);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        AutoPtr<IGpsMeasurementsEvent> mEvent;

    };

public:
    GpsMeasurementsProvider();

    CARAPI OnMeasurementsAvailable(
        /* [in] */ IGpsMeasurementsEvent* event);

};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSMEASUREMENTSPROVIDER_H__