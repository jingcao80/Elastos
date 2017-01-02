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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_GPSNAVIGATIONMESSAGEPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GPSNAVIGATIONMESSAGEPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/server/location/RemoteListenerHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Location::IGpsNavigationMessageEvent;
using Elastos::Droid::Server::Location::IListenerOperation;
using Elastos::Droid::Server::Location::RemoteListenerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * An base implementation for GPS navigation messages provider.
 * It abstracts out the responsibility of handling listeners, while still allowing technology
 * specific implementations to be built.
 *
 * @hide
 */
class GpsNavigationMessageProvider
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
            /* [in] */ IGpsNavigationMessageEvent* event);

        //@Overide
        CARAPI Execute(
            /* [in] */ IInterface* listener);

    private:
        AutoPtr<IGpsNavigationMessageEvent> mEvent;

    };

public:
    GpsNavigationMessageProvider();

    CARAPI OnNavigationMessageAvailable(
        /* [in] */ IGpsNavigationMessageEvent* event);

};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GPSNAVIGATIONMESSAGEPROVIDER_H__