
#ifndef __ELASTOS_DROID_LOCATION_GPSMEASUREMENTLISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_GPSMEASUREMENTLISTENERTRANSPORT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/location/LocalListenerHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ILocalListenerHelperListenerOperation;
using Elastos::Droid::Location::LocalListenerHelper;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A handler class to manage transport listeners for {@link GpsMeasurementsEvent.Listener}.
 *
 * @hide
 */
class GpsMeasurementListenerTransport
    : public LocalListenerHelper
    , public IGpsMeasurementListenerTransport
{
public:
    class ListenerTransport
        : public Object
        , public IIGpsMeasurementsListener
        , public IBinder
    {
    public:
        class Operation
            : public Object
            , public ILocalListenerHelperListenerOperation
        {
        public:
            CAR_INTERFACE_DECL()

            Operation(
                /* [in] */ IGpsMeasurementsEvent* event);

            CARAPI Execute(
                /* [in] */ IInterface* listener);

        private:
            AutoPtr<IGpsMeasurementsEvent> mEvent;
        };

    public:
        CAR_INTERFACE_DECL()

        ListenerTransport();

        CARAPI constructor(
            /* [in] */ IGpsMeasurementListenerTransport* host);

        // @Override
        CARAPI OnGpsMeasurementsReceived(
            /* [in] */ IGpsMeasurementsEvent* event);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    private:
        GpsMeasurementListenerTransport* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GpsMeasurementListenerTransport();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IILocationManager* locationManager);

protected:
    // @Override
    CARAPI RegisterWithServer(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI UnregisterFromServer();

private:
    IContext* mContext;
    AutoPtr<IILocationManager> mLocationManager;

    AutoPtr<IIGpsMeasurementsListener> mListenerTransport;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSMEASUREMENTLISTENERTRANSPORT_H__
