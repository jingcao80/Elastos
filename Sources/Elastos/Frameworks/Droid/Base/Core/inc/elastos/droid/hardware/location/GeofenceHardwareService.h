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

#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARERESERVICE_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARERESERVICE_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/app/Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareImpl;
using Elastos::Droid::Location::IIFusedGeofenceHardware;
using Elastos::Droid::Location::IIGpsGeofenceHardware;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class GeofenceHardwareService
    : public Elastos::Droid::App::Service
    , public IGeofenceHardwareService
{
private:
    class MyBinder
        : public Object
        , public IIGeofenceHardware
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyBinder(
            /* [in] */ GeofenceHardwareService* host);

        CARAPI SetGpsGeofenceHardware(
            /* [in] */ IIGpsGeofenceHardware* service);

        CARAPI SetFusedGeofenceHardware(
            /* [in] */ IIFusedGeofenceHardware* service);

        CARAPI GetMonitoringTypes(
            /* [out, callee] */ ArrayOf<Int32>** types);

        CARAPI GetStatusOfMonitoringType(
            /* [in] */ Int32 monitoringType,
            /* [out] */ Int32* type);

        CARAPI AddCircularFence(
            /* [in] */ Int32 monitoringType,
            /* [in] */ IGeofenceHardwareRequestParcelable* request,
            /* [in] */ IIGeofenceHardwareCallback* _callback,
            /* [out] */ Boolean* result);

        CARAPI RemoveGeofence(
            /* [in] */ Int32 id,
            /* [in] */ Int32 monitoringType,
            /* [out] */ Boolean* result);

        CARAPI PauseGeofence(
            /* [in] */ Int32 id,
            /* [in] */ Int32 monitoringType,
            /* [out] */ Boolean* result);

        CARAPI ResumeGeofence(
            /* [in] */ Int32 id,
            /* [in] */ Int32 monitoringType,
            /* [in] */ Int32 monitorTransitions,
            /* [out] */ Boolean* result);

        CARAPI RegisterForMonitorStateChangeCallback(
            /* [in] */ Int32 monitoringType,
            /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
            /* [out] */ Boolean* result);

        CARAPI UnregisterForMonitorStateChangeCallback(
            /* [in] */ Int32 monitoringType,
            /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
            /* [out] */ Boolean* result);

        CARAPI ToString(
             /* [out]*/ String* str);

    private:
        GeofenceHardwareService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GeofenceHardwareService();

    virtual ~GeofenceHardwareService();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    //@Override
    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    //@Override
    CARAPI OnDestroy();

private:
    using Service::CheckPermission;

    CARAPI CheckPermission(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 monitoringType);

private:
    AutoPtr<IGeofenceHardwareImpl> mGeofenceHardwareImpl;
    AutoPtr<IContext> mContext;

    AutoPtr<IBinder> mBinder;
};

} //Location
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_GEOFENCEHARDWARERESERVICE_H__
