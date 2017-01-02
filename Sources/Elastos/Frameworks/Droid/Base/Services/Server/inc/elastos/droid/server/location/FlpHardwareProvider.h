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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_FLPHARDWAREPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_FLPHARDWAREPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
// #include "hardware/fused_location.h"
// #include "hardware_legacy/power.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareImpl;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareRequestParcelable;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardware;
using Elastos::Droid::Hardware::Location::IIFusedLocationHardwareSink;
using Elastos::Droid::Location::IFusedBatchOptions;
using Elastos::Droid::Location::IIFusedGeofenceHardware;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class is an interop layer for JVM types and the JNI code that interacts
 * with the FLP HAL implementation.
 *
 * {@hide}
 */
class FlpHardwareProvider
    : public Object
    , public IFlpHardwareProvider
{
public:
    class FusedLocationHardwareService
        : public Object
        , public IIFusedLocationHardware
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IFlpHardwareProvider* host);

        // @Override
        CARAPI RegisterSink(
            /* [in] */ IIFusedLocationHardwareSink* eventSink);

        // @Override
        CARAPI UnregisterSink(
            /* [in] */ IIFusedLocationHardwareSink* eventSink);

        // @Override
        CARAPI GetSupportedBatchSize(
            /* [out] */ Int32* size);

        // @Override
        CARAPI StartBatching(
            /* [in] */ Int32 requestId,
            /* [in] */ IFusedBatchOptions* options);

        // @Override
        CARAPI StopBatching(
            /* [in] */ Int32 requestId);

        // @Override
        CARAPI UpdateBatchingOptions(
            /* [in] */ Int32 requestId,
            /* [in] */ IFusedBatchOptions* options);

        // @Override
        CARAPI RequestBatchOfLocations(
            /* [in] */ Int32 batchSizeRequested);

        // @Override
        CARAPI SupportsDiagnosticDataInjection(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI InjectDiagnosticData(
            /* [in] */ const String& data);

        // @Override
        CARAPI SupportsDeviceContextInjection(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI InjectDeviceContext(
            /* [in] */ Int32 deviceEnabledContext);

        //TODO delete
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return Object::ToString(str);
        }

    private:
        FlpHardwareProvider* mHost;
    };

    class FusedGeofenceHardwareService
        : public Object
        , public IIFusedGeofenceHardware
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IFlpHardwareProvider* host);

        // @Override
        CARAPI IsSupported(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI AddGeofences(
           /* [in] */ ArrayOf<IGeofenceHardwareRequestParcelable*>* geofenceRequestsArray);

        // @Override
        CARAPI RemoveGeofences(
            /* [in] */ ArrayOf<Int32>* geofenceIds);

        // @Override
        CARAPI PauseMonitoringGeofence(
            /* [in] */ Int32 geofenceId);

        // @Override
        CARAPI ResumeMonitoringGeofence(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 monitorTransitions);

        // @Override
        CARAPI ModifyGeofenceOptions(
            /* [in] */ Int32 geofenceId,
            /* [in] */ Int32 lastTransition,
            /* [in] */ Int32 monitorTransitions,
            /* [in] */ Int32 notificationResponsiveness,
            /* [in] */ Int32 unknownTimer,
            /* [in] */ Int32 sourcesToUse);

        //TODO delete
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return Object::ToString(str);
        }

    private:
        FlpHardwareProvider* mHost;
    };

private:
    /**
     * Internal classes and functions used by the provider.
     */
    class NetworkLocationListener
        : public Object
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkLocationListener(
            /* [in] */ FlpHardwareProvider* host);

        // @Override
        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        // @Override
        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);

        // @Override
        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        // @Override
        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);

    private:
        FlpHardwareProvider* mHost;
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IFlpHardwareProvider>) GetInstance(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsSupported();

    CARAPI GetLocationHardware(
        /* [out] */ IIFusedLocationHardware** lh);

    CARAPI GetGeofenceHardware(
        /* [out] */ IIFusedGeofenceHardware** gh);

public:
    /**
     * Private callback functions used by FLP HAL.
     */
    // FlpCallbacks members
    CARAPI_(void) OnLocationReport(
        /* [in] */ ArrayOf<ILocation*>* locations);

    // FlpDiagnosticCallbacks members
    CARAPI_(void) OnDataReport(
        /* [in] */ const String& data);

    // FlpGeofenceCallbacks members
    CARAPI_(void) OnGeofenceTransition(
        /* [in] */ Int32 geofenceId,
        /* [in] */ ILocation* location,
        /* [in] */ Int32 transition,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 sourcesUsed);

    CARAPI_(void) OnGeofenceMonitorStatus(
        /* [in] */ Int32 status,
        /* [in] */ Int32 source,
        /* [in] */ ILocation* location);

    CARAPI_(void) OnGeofenceAdd(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 result);

    CARAPI_(void) OnGeofenceRemove(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 result);

    CARAPI_(void) OnGeofencePause(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 result);

    CARAPI_(void) OnGeofenceResume(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 result);

private:
    FlpHardwareProvider(
        /* [in] */ IContext* context);

    // Core members
    static CARAPI_(void) NativeClassInit();
    static CARAPI_(Boolean) NativeIsSupported();

    // FlpLocationInterface members
    CARAPI_(void) NativeInit();
    CARAPI_(Int32) NativeGetBatchSize();
    CARAPI_(void) NativeStartBatching(
        /* [in] */ Int32 requestId,
        /* [in] */ IFusedBatchOptions* options);
    CARAPI_(void) NativeUpdateBatchingOptions(
        /* [in] */ Int32 requestId,
        /* [in] */ IFusedBatchOptions* optionsObject);
    CARAPI_(void) NativeStopBatching(
        /* [in] */ Int32 id);
    CARAPI_(void) NativeRequestBatchedLocation(
        /* [in] */ Int32 lastNLocations);
    CARAPI_(void) NativeInjectLocation(
        /* [in] */ ILocation* location);
    // TODO [Fix] sort out the lifetime of the instance
    CARAPI_(void) NativeCleanup();

    // FlpDiagnosticsInterface members
    CARAPI_(Boolean) NativeIsDiagnosticSupported();
    CARAPI_(void) NativeInjectDiagnosticData(
        /* [in] */ const String& data);

    // FlpDeviceContextInterface members
    CARAPI_(Boolean) NativeIsDeviceContextSupported();
    CARAPI_(void) NativeInjectDeviceContext(
        /* [in] */ Int32 deviceEnabledContext);

    // FlpGeofencingInterface members
    CARAPI_(Boolean) NativeIsGeofencingSupported();
    CARAPI_(void) NativeAddGeofences(
            /* [in] */ ArrayOf<IGeofenceHardwareRequestParcelable*>* geofenceRequestsArray);
    CARAPI_(void) NativePauseGeofence(
        /* [in] */ Int32 geofenceId);
    CARAPI_(void) NativeResumeGeofence(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 monitorTransitions);
    CARAPI_(void) NativeModifyGeofenceOption(
        /* [in] */ Int32 geofenceId,
        /* [in] */ Int32 lastTransition,
        /* [in] */ Int32 monitorTransitions,
        /* [in] */ Int32 notificationResponsiveness,
        /* [in] */ Int32 unknownTimer,
        /* [in] */ Int32 sourcesToUse);
    CARAPI_(void) NativeRemoveGeofences(
        /* [in] */ ArrayOf<Int32>* geofenceIdsArray);

    CARAPI_(AutoPtr<IGeofenceHardwareImpl>) GetGeofenceHardwareSink();

    static CARAPI_(Int32) TranslateToGeofenceHardwareStatus(
        /* [in] */ Int32 flpHalResult);

    CARAPI_(AutoPtr<ILocation>) UpdateLocationInformation(
        /* [in] */ ILocation* location);

private:
    AutoPtr<IGeofenceHardwareImpl> mGeofenceHardwareSink;
    AutoPtr<IIFusedLocationHardwareSink> mLocationSink;

    static AutoPtr<FlpHardwareProvider> sSingletonInstance;

    const static String TAG;
    AutoPtr<IContext> mContext;
    // private final Object mLocationSinkLock = new Object();

    // FlpHal result codes, they must be equal to the ones in fused_location.h
    // const static Int32 FLP_RESULT_SUCCESS;
    // const static Int32 FLP_RESULT_ERROR;
    // const static Int32 FLP_RESULT_INSUFFICIENT_MEMORY;
    // const static Int32 FLP_RESULT_TOO_MANY_GEOFENCES;
    // const static Int32 FLP_RESULT_ID_EXISTS;
    // const static Int32 FLP_RESULT_ID_UNKNOWN;
    // const static Int32 FLP_RESULT_INVALID_GEOFENCE_TRANSITION;

    // FlpHal monitor status codes, they must be equal to the ones in fused_location.h
    // const static Int32 FLP_GEOFENCE_MONITOR_STATUS_UNAVAILABLE;
    // const static Int32 FLP_GEOFENCE_MONITOR_STATUS_AVAILABLE;

    AutoPtr<IIFusedLocationHardware> mLocationHardware;
    AutoPtr<IIFusedGeofenceHardware> mGeofenceHardwareService;

    /**
     * Private native methods accessing FLP HAL.
     */
    static const StaticInitializer sInitializer;

    // static jobject sCallbacksObj;
    // static JNIEnv *sCallbackEnv;
    // static hw_device_t* sHardwareDevice;

    // static jmethodID sOnLocationReport;
    // static jmethodID sOnDataReport;
    // static jmethodID sOnGeofenceTransition;
    // static jmethodID sOnGeofenceMonitorStatus;
    // static jmethodID sOnGeofenceAdd;
    // static jmethodID sOnGeofenceRemove;
    // static jmethodID sOnGeofencePause;
    // static jmethodID sOnGeofenceResume;

    // static const FlpLocationInterface* sFlpInterface;
    // static const FlpDiagnosticInterface* sFlpDiagnosticInterface;
    // static const FlpGeofencingInterface* sFlpGeofencingInterface;
    // static const FlpDeviceContextInterface* sFlpDeviceContextInterface;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_FLPHARDWAREPROVIDER_H__

