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
