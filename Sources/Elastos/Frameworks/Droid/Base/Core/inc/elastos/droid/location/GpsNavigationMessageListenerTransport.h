
#ifndef __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__
#define __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
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
 * A handler class to manage transport listeners for {@link GpsNavigationMessageEvent.Listener}.
 *
 * @hide
 */
class GpsNavigationMessageListenerTransport
    : public LocalListenerHelper
    , public IGpsNavigationMessageListenerTransport
{
public:
    class ListenerTransport
        : public Object
        , public IIGpsNavigationMessageListener
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
                /* [in] */ IGpsNavigationMessageEvent* event);

            CARAPI Execute(
                /* [in] */ IInterface* listener);

        private:
            AutoPtr<IGpsNavigationMessageEvent> mEvent;
        };
    public:
        CAR_INTERFACE_DECL()

        ListenerTransport();

        CARAPI constructor(
            /* [in] */ IGpsNavigationMessageListenerTransport* host);

        // @Override
        CARAPI OnGpsNavigationMessageReceived(
            /* [in] */ IGpsNavigationMessageEvent* event);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        GpsNavigationMessageListenerTransport* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GpsNavigationMessageListenerTransport();

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

    AutoPtr<IIGpsNavigationMessageListener> mListenerTransport;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGELISTENERTRANSPORT_H__
