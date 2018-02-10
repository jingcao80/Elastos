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
            /* [in] */ HANDLE host);

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

