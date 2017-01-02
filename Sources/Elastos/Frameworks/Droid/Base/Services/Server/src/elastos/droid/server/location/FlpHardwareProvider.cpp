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

#include "elastos/droid/hardware/location/GeofenceHardwareImpl.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/location/FlpHardwareProvider.h"
#include "elastos/droid/server/location/CFusedGeofenceHardwareService.h"
#include "elastos/droid/server/location/CFusedLocationHardwareService.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>
#include "hardware/fused_location.h"
#include "hardware_legacy/power.h"
#include <cutils/log.h>

using Elastos::Droid::Hardware::Location::EIID_IIFusedLocationHardware;
using Elastos::Droid::Hardware::Location::IGeofenceHardware;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareImpl;
using Elastos::Droid::Hardware::Location::GeofenceHardwareImpl;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::CLocationRequestHelper;
using Elastos::Droid::Location::EIID_IIFusedGeofenceHardware;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Location::ILocationRequestHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Server::Location::CFusedGeofenceHardwareService;
using Elastos::Droid::Server::Location::CFusedLocationHardwareService;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
// FlpHardwareProvider::FusedLocationHardwareService
//====================================

CAR_INTERFACE_IMPL_2(FlpHardwareProvider::FusedLocationHardwareService, Object, IIFusedLocationHardware, IBinder)

ECode FlpHardwareProvider::FusedLocationHardwareService::constructor(
    /* [in] */ IFlpHardwareProvider* host)
{
    mHost = (FlpHardwareProvider*)host;
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::RegisterSink(
    /* [in] */ IIFusedLocationHardwareSink* eventSink)
{
    {    AutoLock syncLock(this);
        // only one sink is allowed at the moment
        if (mHost->mLocationSink != NULL) {
            Logger::E(TAG, "Replacing an existing IFusedLocationHardware sink");
        }

        mHost->mLocationSink = eventSink;
    }
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::UnregisterSink(
    /* [in] */ IIFusedLocationHardwareSink* eventSink)
{
    {    AutoLock syncLock(this);
        // don't throw if the sink is not registered, simply make it a no-op
        if ((mHost->mLocationSink).Get() == eventSink) {
            mHost->mLocationSink = NULL;
        }
    }
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::GetSupportedBatchSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mHost->NativeGetBatchSize();
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::StartBatching(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* options)
{
    mHost->NativeStartBatching(requestId, options);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::StopBatching(
    /* [in] */ Int32 requestId)
{
    mHost->NativeStopBatching(requestId);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::UpdateBatchingOptions(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* options)
{
    mHost->NativeUpdateBatchingOptions(requestId, options);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::RequestBatchOfLocations(
    /* [in] */ Int32 batchSizeRequested)
{
    mHost->NativeRequestBatchedLocation(batchSizeRequested);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::SupportsDiagnosticDataInjection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->NativeIsDiagnosticSupported();
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::InjectDiagnosticData(
    /* [in] */ const String& data)
{
    mHost->NativeInjectDiagnosticData(data);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::SupportsDeviceContextInjection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->NativeIsDeviceContextSupported();
    return NOERROR;
}

ECode FlpHardwareProvider::FusedLocationHardwareService::InjectDeviceContext(
    /* [in] */ Int32 deviceEnabledContext)
{
    mHost->NativeInjectDeviceContext(deviceEnabledContext);
    return NOERROR;
}

//====================================
// FlpHardwareProvider::FusedGeofenceHardwareService
//====================================

CAR_INTERFACE_IMPL_2(FlpHardwareProvider::FusedGeofenceHardwareService, Object, IIFusedGeofenceHardware, IBinder)

ECode FlpHardwareProvider::FusedGeofenceHardwareService::constructor(
    /* [in] */ IFlpHardwareProvider* host)
{
    mHost = (FlpHardwareProvider*)host;
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::IsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->NativeIsGeofencingSupported();
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::AddGeofences(
   /* [in] */ ArrayOf<IGeofenceHardwareRequestParcelable*>* geofenceRequestsArray)
{
    mHost->NativeAddGeofences(geofenceRequestsArray);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::RemoveGeofences(
    /* [in] */ ArrayOf<Int32>* geofenceIds)
{
    mHost->NativeRemoveGeofences(geofenceIds);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::PauseMonitoringGeofence(
    /* [in] */ Int32 geofenceId)
{
    mHost->NativePauseGeofence(geofenceId);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::ResumeMonitoringGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitorTransitions)
{
    mHost->NativeResumeGeofence(geofenceId, monitorTransitions);
    return NOERROR;
}

ECode FlpHardwareProvider::FusedGeofenceHardwareService::ModifyGeofenceOptions(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 lastTransition,
    /* [in] */ Int32 monitorTransitions,
    /* [in] */ Int32 notificationResponsiveness,
    /* [in] */ Int32 unknownTimer,
    /* [in] */ Int32 sourcesToUse)
{
    mHost->NativeModifyGeofenceOption(geofenceId, lastTransition, monitorTransitions,
        notificationResponsiveness, unknownTimer, sourcesToUse);
    return NOERROR;
}

//====================================
// FlpHardwareProvider::NetworkLocationListener
//====================================

CAR_INTERFACE_IMPL(FlpHardwareProvider::NetworkLocationListener, Object, ILocationListener)

FlpHardwareProvider::NetworkLocationListener::NetworkLocationListener(
    /* [in] */ FlpHardwareProvider* host)
    : mHost(host)
{}

ECode FlpHardwareProvider::NetworkLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    String provider;
    Boolean hasAccuracy;
    location->GetProvider(&provider);
    location->HasAccuracy(&hasAccuracy);
    if (!ILocationManager::NETWORK_PROVIDER.Equals(provider) || !hasAccuracy) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mHost->NativeInjectLocation(location);
    return NOERROR;
}

ECode FlpHardwareProvider::NetworkLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode FlpHardwareProvider::NetworkLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode FlpHardwareProvider::NetworkLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}


//====================================
// FlpHardwareProvider::StaticInitializer
//====================================
FlpHardwareProvider::StaticInitializer::StaticInitializer()
{
    NativeClassInit();
}

//====================================
// FlpHardwareProvider
//====================================

const String FlpHardwareProvider::TAG("FlpHardwareProvider");
// const Int32 FlpHardwareProvider::FLP_RESULT_SUCCESS = 0;
// const Int32 FlpHardwareProvider::FLP_RESULT_ERROR = -1;
// const Int32 FlpHardwareProvider::FLP_RESULT_INSUFFICIENT_MEMORY = -2;
// const Int32 FlpHardwareProvider::FLP_RESULT_TOO_MANY_GEOFENCES = -3;
// const Int32 FlpHardwareProvider::FLP_RESULT_ID_EXISTS = -4;
// const Int32 FlpHardwareProvider::FLP_RESULT_ID_UNKNOWN = -5;
// const Int32 FlpHardwareProvider::FLP_RESULT_INVALID_GEOFENCE_TRANSITION = -6;
// const Int32 FlpHardwareProvider::FLP_GEOFENCE_MONITOR_STATUS_UNAVAILABLE = 1<<0;
// const Int32 FlpHardwareProvider::FLP_GEOFENCE_MONITOR_STATUS_AVAILABLE = 1<<1;
const FlpHardwareProvider::StaticInitializer FlpHardwareProvider::sInitializer;

AutoPtr<FlpHardwareProvider> FlpHardwareProvider::sSingletonInstance;

CAR_INTERFACE_IMPL(FlpHardwareProvider, Object, IFlpHardwareProvider)

AutoPtr<IFlpHardwareProvider> FlpHardwareProvider::GetInstance(
    /* [in] */ IContext* context)
{
    if (sSingletonInstance == NULL) {
        sSingletonInstance = new FlpHardwareProvider(context);
        sSingletonInstance->NativeInit();
    }

    return (IFlpHardwareProvider*)sSingletonInstance.Get();
}

FlpHardwareProvider::FlpHardwareProvider(
    /* [in] */ IContext* context)
{
    mContext = context;

    // register for listening for passive provider data
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&obj);
    AutoPtr<ILocationManager> manager = ILocationManager::Probe(obj);
    const Int64 minTime = 0;
    const Float minDistance = 0;
    const Boolean oneShot = FALSE;
    AutoPtr<ILocationRequestHelper> lrh;
    CLocationRequestHelper::AcquireSingleton((ILocationRequestHelper**)&lrh);
    AutoPtr<ILocationRequest> request;
    lrh->CreateFromDeprecatedProvider(ILocationManager::PASSIVE_PROVIDER,
        minTime, minDistance, oneShot, (ILocationRequest**)&request);
    // Don't keep track of this request since it's done on behalf of other clients
    // (which are kept track of separately).
    request->SetHideFromAppOps(TRUE);
    AutoPtr<NetworkLocationListener> nll = new NetworkLocationListener(this);
    manager->RequestLocationUpdates(request, (ILocationListener*)nll.Get(),
        Looper::GetMyLooper());
    CFusedLocationHardwareService::New(this, (IIFusedLocationHardware**)&mLocationHardware);
    CFusedGeofenceHardwareService::New(this, (IIFusedGeofenceHardware**)&mGeofenceHardwareService);
}

Boolean FlpHardwareProvider::IsSupported()
{
    return NativeIsSupported();
}

void FlpHardwareProvider::OnLocationReport(
    /* [in] */ ArrayOf<ILocation*>* locations)
{
    for (Int32 i = 0; i < locations->GetLength(); i++) {
        AutoPtr<ILocation> location = (*locations)[i];
        location->SetProvider(ILocationManager::FUSED_PROVIDER);
        // set the elapsed time-stamp just as GPS provider does
        location->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());
    }

    AutoPtr<IIFusedLocationHardwareSink> sink;
    {    AutoLock syncLock(this);
        sink = mLocationSink;
    }
    if (sink != NULL) {
        ECode ec = sink->OnLocationAvailable(locations);
        if (FAILED(ec)) {
            Logger::E(TAG, "RemoteException calling onLocationAvailable");
        }
    }
}

void FlpHardwareProvider::OnDataReport(
    /* [in] */ const String& data)
{
    AutoPtr<IIFusedLocationHardwareSink> sink;
    {    AutoLock syncLock(this);
        sink = mLocationSink;
    }
    if (mLocationSink != NULL) {
        ECode ec = sink->OnDiagnosticDataAvailable(data);
        if (FAILED(ec)) {
            Logger::E(TAG, "RemoteException calling onDiagnosticDataAvailable");
        }
    }
}

void FlpHardwareProvider::OnGeofenceTransition(
    /* [in] */ Int32 geofenceId,
    /* [in] */ ILocation* location,
    /* [in] */ Int32 transition,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 sourcesUsed)
{
    // the transition Id does not require translation because the values in fused_location.h
    // and GeofenceHardware are in sync
    GetGeofenceHardwareSink()->ReportGeofenceTransition(
        geofenceId,
        UpdateLocationInformation(location).Get(),
        transition,
        timestamp,
        IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE,
        sourcesUsed);
}

void FlpHardwareProvider::OnGeofenceMonitorStatus(
    /* [in] */ Int32 status,
    /* [in] */ Int32 source,
    /* [in] */ ILocation* location)
{
    // allow the location to be optional in this event
    AutoPtr<ILocation> updatedLocation;
    if (location != NULL) {
        updatedLocation = UpdateLocationInformation(location);
    }

    Int32 monitorStatus;
    switch (status) {
        case FLP_GEOFENCE_MONITOR_STATUS_UNAVAILABLE:
            monitorStatus = IGeofenceHardware::MONITOR_CURRENTLY_UNAVAILABLE;
            break;
        case FLP_GEOFENCE_MONITOR_STATUS_AVAILABLE:
            monitorStatus = IGeofenceHardware::MONITOR_CURRENTLY_AVAILABLE;
            break;
        default:
            Logger::E(TAG, "Invalid FlpHal Geofence monitor status: %d", status);
            monitorStatus = IGeofenceHardware::MONITOR_CURRENTLY_UNAVAILABLE;
            break;
    }

    GetGeofenceHardwareSink()->ReportGeofenceMonitorStatus(
        IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE,
        monitorStatus,
        updatedLocation,
        source);
}

void FlpHardwareProvider::OnGeofenceAdd(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 result)
{
    GetGeofenceHardwareSink()->ReportGeofenceAddStatus(
        geofenceId,
        TranslateToGeofenceHardwareStatus(result));
}

void FlpHardwareProvider::OnGeofenceRemove(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 result)
{
    GetGeofenceHardwareSink()->ReportGeofenceRemoveStatus(
        geofenceId,
        TranslateToGeofenceHardwareStatus(result));
}

void FlpHardwareProvider::OnGeofencePause(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 result)
{
    GetGeofenceHardwareSink()->ReportGeofencePauseStatus(
        geofenceId,
        TranslateToGeofenceHardwareStatus(result));
}

void FlpHardwareProvider::OnGeofenceResume(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 result)
{
    GetGeofenceHardwareSink()->ReportGeofenceResumeStatus(
        geofenceId,
        TranslateToGeofenceHardwareStatus(result));
}

ECode FlpHardwareProvider::GetLocationHardware(
    /* [out] */ IIFusedLocationHardware** lh)
{
    VALIDATE_NOT_NULL(lh)
    *lh = mLocationHardware;
    REFCOUNT_ADD(*lh)
    return NOERROR;
}

ECode FlpHardwareProvider::GetGeofenceHardware(
    /* [out] */ IIFusedGeofenceHardware** gh)
{
    VALIDATE_NOT_NULL(gh)
    *gh = mGeofenceHardwareService;
    REFCOUNT_ADD(*gh)
    return NOERROR;
}

AutoPtr<IGeofenceHardwareImpl> FlpHardwareProvider::GetGeofenceHardwareSink()
{
    if (mGeofenceHardwareSink == NULL) {
        mGeofenceHardwareSink = GeofenceHardwareImpl::GetInstance(mContext);
    }

    return mGeofenceHardwareSink;
}

Int32 FlpHardwareProvider::TranslateToGeofenceHardwareStatus(
    /* [in] */ Int32 flpHalResult)
{
    switch(flpHalResult) {
        case FLP_RESULT_SUCCESS:
            return IGeofenceHardware::GEOFENCE_SUCCESS;
        case FLP_RESULT_ERROR:
            return IGeofenceHardware::GEOFENCE_FAILURE;
        case FLP_RESULT_INSUFFICIENT_MEMORY:
            return IGeofenceHardware::GEOFENCE_ERROR_INSUFFICIENT_MEMORY;
        case FLP_RESULT_TOO_MANY_GEOFENCES:
            return IGeofenceHardware::GEOFENCE_ERROR_TOO_MANY_GEOFENCES;
        case FLP_RESULT_ID_EXISTS:
            return IGeofenceHardware::GEOFENCE_ERROR_ID_EXISTS;
        case FLP_RESULT_ID_UNKNOWN:
            return IGeofenceHardware::GEOFENCE_ERROR_ID_UNKNOWN;
        case FLP_RESULT_INVALID_GEOFENCE_TRANSITION:
            return IGeofenceHardware::GEOFENCE_ERROR_INVALID_TRANSITION;
        default:
            Logger::E(TAG, "Invalid FlpHal result code: %d", flpHalResult);
            return IGeofenceHardware::GEOFENCE_FAILURE;
    }
}

AutoPtr<ILocation> FlpHardwareProvider::UpdateLocationInformation(
    /* [in] */ ILocation* location)
{
    location->SetProvider(ILocationManager::FUSED_PROVIDER);
    // set the elapsed time-stamp just as GPS provider does
    location->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());
    return location;
}

#define WAKE_LOCK_NAME  "FLP"

static AutoPtr<FlpHardwareProvider> sCallbacksObj;
static AutoPtr<IThread> sCallbackThread;
static hw_device_t* sHardwareDevice = NULL;
static const FlpLocationInterface* sFlpInterface = NULL;
static const FlpDiagnosticInterface* sFlpDiagnosticInterface = NULL;
static const FlpGeofencingInterface* sFlpGeofencingInterface = NULL;
static const FlpDeviceContextInterface* sFlpDeviceContextInterface = NULL;

static inline ECode ThrowOnError(
    /* [in] */ int resultCode,
    /* [in] */ const char* methodName)
{
    if (resultCode == FLP_RESULT_SUCCESS) {
        return NOERROR;
    }

    ALOGE("Error %d in '%s'", resultCode, methodName);
    // TODO: this layer needs to be refactored to return error codes to Java
    // raising a FatalError is harsh, and because FLP Hardware Provider is loaded inside the system
    // service, it can cause the device to reboot, or remain in a reboot loop
    // a simple exception is still dumped to logcat, but it is handled more gracefully
    assert(0);
    return E_RUNTIME_EXCEPTION;
}


static bool IsValidCallbackThread()
{
    // JNIEnv* env = AndroidRuntime::getJNIEnv();
    AutoPtr<IThread> curr = Thread::GetCurrentThread();
    if (sCallbackThread == NULL || sCallbackThread != curr) {
        ALOGE("CallbackThread check fail: curr=%p, expected=%p", curr.Get(), sCallbackThread.Get());
        return false;
    }

    return true;
}

static int SetThreadEvent(
    /* [in] */ ThreadEvent event)
{
    // JavaVM* javaVm = AndroidRuntime::getJavaVM();

    switch(event) {
        case ASSOCIATE_JVM:
        {
            if (sCallbackThread != NULL) {
                ALOGE("Attempted to associate callback in '%s'. Callback already associated.", __FUNCTION__);
                return FLP_RESULT_ERROR;
            }

            // JavaVMAttachArgs args = {
            //     JNI_VERSION_1_6,
            //     "FLP Service Callback Thread",
            //     /* group */ NULL
            // };

            // jint attachResult = javaVm->AttachCurrentThread(&sCallbackThread, &args);
            // if (attachResult != 0) {
            //     ALOGE("Callback thread attachment error: %d", attachResult);
            //     return FLP_RESULT_ERROR;
            // }
            ECode ec = Thread::Attach(String(""), (IThread**)&sCallbackThread);
            if (FAILED(ec)) {
                ALOGE("Callback thread attachment error: 0x%x", ec);
                return FLP_RESULT_ERROR;
            }

            ALOGV("Callback thread attached: %p", sCallbackThread.Get());
            break;
        }
        case DISASSOCIATE_JVM:
        {
            if (!IsValidCallbackThread()) {
                ALOGE("Attempted to dissasociate an unnownk callback thread : '%s'.", __FUNCTION__);
                return FLP_RESULT_ERROR;
            }

            // if (javaVm->DetachCurrentThread() != 0) {
            //     return FLP_RESULT_ERROR;
            // }
            if (FAILED(sCallbackThread->Detach())) {
                return FLP_RESULT_ERROR;
            }

            sCallbackThread = NULL;
            break;
        }
        default:
            ALOGE("Invalid ThreadEvent request %d", event);
            return FLP_RESULT_ERROR;
      }

    return FLP_RESULT_SUCCESS;
}

void FlpHardwareProvider::NativeClassInit()
{
    sFlpInterface = NULL;

    // open the hardware module
    const hw_module_t* module = NULL;
    int err = hw_get_module(FUSED_LOCATION_HARDWARE_MODULE_ID, &module);
    if (err != 0) {
        ALOGE("Error hw_get_module '%s': %d", FUSED_LOCATION_HARDWARE_MODULE_ID, err);
        return;
    }

    err = module->methods->open(
        module,
        FUSED_LOCATION_HARDWARE_MODULE_ID,
        &sHardwareDevice);
    if (err != 0) {
        ALOGE("Error opening device '%s': %d", FUSED_LOCATION_HARDWARE_MODULE_ID, err);
        return;
    }

    // acquire the interfaces pointers
    flp_device_t* flp_device = reinterpret_cast<flp_device_t*>(sHardwareDevice);
    sFlpInterface = flp_device->get_flp_interface(flp_device);

    if (sFlpInterface != NULL) {
        sFlpDiagnosticInterface = reinterpret_cast<const FlpDiagnosticInterface*>(
            sFlpInterface->get_extension(FLP_DIAGNOSTIC_INTERFACE));

        sFlpGeofencingInterface = reinterpret_cast<const FlpGeofencingInterface*>(
            sFlpInterface->get_extension(FLP_GEOFENCING_INTERFACE));

        sFlpDeviceContextInterface = reinterpret_cast<const FlpDeviceContextInterface*>(
            sFlpInterface->get_extension(FLP_DEVICE_CONTEXT_INTERFACE));
    }

}

Boolean FlpHardwareProvider::NativeIsSupported()
{
    if (sFlpInterface == NULL) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Helper function to unwrap a java object back into a FlpLocation structure.
 */
static void TranslateFromObject(
    /* [in] */ ILocation* locationObject,
    /* [in] */ FlpLocation& location)
{
    location.size = sizeof(FlpLocation);
    location.flags = 0;

    locationObject->GetLatitude(&location.latitude);
    locationObject->GetLatitude(&location.latitude);
    locationObject->GetLongitude(&location.longitude);
    locationObject->GetTime(&location.timestamp);
    location.flags |= FLP_LOCATION_HAS_LAT_LONG;

    Boolean hasAltitude;
    locationObject->HasAltitude(&hasAltitude);
    if (hasAltitude) {
        locationObject->GetAltitude(&location.altitude);
        location.flags |= FLP_LOCATION_HAS_ALTITUDE;
    }

    Boolean hasSpeed;
    locationObject->HasSpeed(&hasSpeed);
    if (hasSpeed) {
        locationObject->GetSpeed(&location.speed);
        location.flags |= FLP_LOCATION_HAS_SPEED;
    }

    Boolean hasBearing;
    locationObject->HasBearing(&hasBearing);
    if (hasBearing) {
        locationObject->GetBearing(&location.bearing);
        location.flags |= FLP_LOCATION_HAS_BEARING;
    }

    Boolean hasAccuracy;
    locationObject->HasAccuracy(&hasAccuracy);
    if (hasAccuracy) {
        locationObject->GetAccuracy(&location.accuracy);
        location.flags |= FLP_LOCATION_HAS_ACCURACY;
    }

    // TODO: wire sources_used if Location class exposes them
}

/*
 * Helper function to unwrap FlpBatchOptions from the Java Runtime calls.
 */
static void TranslateFromObject(
    /* [in] */ IFusedBatchOptions* batchOptionsObject,
    /* [in] */ FlpBatchOptions& batchOptions)
{
    batchOptionsObject->GetMaxPowerAllocationInMW(&batchOptions.max_power_allocation_mW);
    batchOptionsObject->GetPeriodInNS(&batchOptions.period_ns);
    batchOptionsObject->GetSourcesToUse((Int32*)&batchOptions.sources_to_use);
    batchOptionsObject->GetFlags((Int32*)&batchOptions.flags);
}

/*
 * Helper function to unwrap Geofence structures from the Java Runtime calls.
 */
static void TranslateGeofenceFromGeofenceHardwareRequestParcelable(
    /* [in] */ IGeofenceHardwareRequestParcelable* geofenceRequestObject,
    /* [in] */ Geofence& geofence)
{
    geofenceRequestObject->GetId(&geofence.geofence_id);

    // this works because GeofenceHardwareRequest.java and fused_location.h have
    // the same notion of geofence types
    Int32 itype;
    geofenceRequestObject->GetType(&itype);
    GeofenceType type = (GeofenceType)itype;
    if(type != TYPE_CIRCLE) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }
    geofence.data->type = type;
    GeofenceCircle& circle = geofence.data->geofence.circle;
    geofenceRequestObject->GetLatitude(&circle.latitude);
    geofenceRequestObject->GetLongitude(&circle.longitude);
    geofenceRequestObject->GetRadius(&circle.radius_m);

    GeofenceOptions* options = geofence.options;
    geofenceRequestObject->GetMonitorTransitions(&options->monitor_transitions);
    geofenceRequestObject->GetUnknownTimer(&options->unknown_timer_ms);
    geofenceRequestObject->GetNotificationResponsiveness(&options->notification_responsivenes_ms);
    geofenceRequestObject->GetLastTransition(&options->last_transition);
    geofenceRequestObject->GetSourceTechnologies((Int32*)&options->sources_to_use);
}

/*
 * Helper function to transform FlpLocation into a java object.
 */
static AutoPtr<ILocation> TranslateToObject(
    /* [in] */ const FlpLocation* location)
{
    AutoPtr<ILocation> locationObject;
    CLocation::New(String(NULL), (ILocation**)&locationObject);

    Int32 flags = location->flags;

    // set the valid information in the object
    if (flags & FLP_LOCATION_HAS_LAT_LONG) {
        locationObject->SetLatitude(location->latitude);
        locationObject->SetLongitude(location->longitude);
        locationObject->SetTime(location->timestamp);
    }

    if (flags & FLP_LOCATION_HAS_ALTITUDE) {
        locationObject->SetAltitude(location->altitude);
    }

    if (flags & FLP_LOCATION_HAS_SPEED) {
        locationObject->SetSpeed(location->speed);
    }

    if (flags & FLP_LOCATION_HAS_BEARING) {
        locationObject->SetBearing(location->bearing);
    }

    if (flags & FLP_LOCATION_HAS_ACCURACY) {
        locationObject->SetAccuracy(location->accuracy);
    }

    // TODO: wire FlpLocation::sources_used when needed
    return locationObject;
}

/*
 * Helper function to serialize FlpLocation structures.
 */
static AutoPtr<ArrayOf<ILocation*> > TranslateToObjectArray(
    /* [in] */ int32_t locationsCount,
    /* [in] */ FlpLocation** locations)
{
    AutoPtr<ArrayOf<ILocation*> > array = ArrayOf<ILocation*>::Alloc(locationsCount);
    for (int i = 0; i < locationsCount; ++i) {
        array->Set(i, TranslateToObject(locations[i]));
    }

    return array;
}

static void LocationCallback(
    /* [in] */ int32_t locationsCount,
    /* [in] */ FlpLocation** locations)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    if (locationsCount == 0 || locations == NULL) {
        ALOGE("Invalid LocationCallback. Count: %d, Locations: %p", locationsCount, locations);
        return;
    }

    AutoPtr<ArrayOf<ILocation*> > locationsArray = TranslateToObjectArray(locationsCount, locations);
    sCallbacksObj->OnLocationReport(locationsArray);
}

static void AcquireWakelock()
{
    acquire_wake_lock(PARTIAL_WAKE_LOCK, WAKE_LOCK_NAME);
}

static void ReleaseWakelock()
{
    release_wake_lock(WAKE_LOCK_NAME);
}

FlpCallbacks sFlpCallbacks = {
  sizeof(FlpCallbacks),
  LocationCallback,
  AcquireWakelock,
  ReleaseWakelock,
  SetThreadEvent
};

static void ReportData(
    /* [in] */ char* data,
    /* [in] */ int length)
{
    if (length == 0 || data == NULL) {
        ALOGE("Invalid ReportData callback. Length: %d, Data: %p", length, data);
        return;
    }

    sCallbacksObj->OnDataReport(String(data, length));
}

FlpDiagnosticCallbacks sFlpDiagnosticCallbacks = {
  sizeof(FlpDiagnosticCallbacks),
  SetThreadEvent,
  ReportData
};

static void GeofenceTransitionCallback(
    /* [in] */ int32_t geofenceId,
    /* [in] */ FlpLocation* location,
    /* [in] */ int32_t transition,
    /* [in] */ FlpUtcTime timestamp,
    /* [in] */ uint32_t sourcesUsed)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    if (location == NULL) {
        ALOGE("GeofenceTransition received with invalid location: %p", location);
        return;
    }

    AutoPtr<ILocation> locationObject = TranslateToObject(location);

    sCallbacksObj->OnGeofenceTransition(geofenceId, locationObject, transition, timestamp, sourcesUsed);
}

static void GeofenceMonitorStatusCallback(
    /* [in] */ int32_t status,
    /* [in] */ uint32_t source,
    /* [in] */ FlpLocation* lastLocation)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    AutoPtr<ILocation> locationObject;
    if (lastLocation != NULL) {
        locationObject = TranslateToObject(lastLocation);
    }
    sCallbacksObj->OnGeofenceMonitorStatus(status, source, locationObject);
}

static void GeofenceAddCallback(
    /* [in] */ int32_t geofenceId,
    /* [in] */ int32_t result)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    sCallbacksObj->OnGeofenceAdd(geofenceId, result);
}

static void GeofenceRemoveCallback(
    /* [in] */ int32_t geofenceId,
    /* [in] */ int32_t result)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    sCallbacksObj->OnGeofenceRemove(geofenceId, result);
}

static void GeofencePauseCallback(
    /* [in] */ int32_t geofenceId,
    /* [in] */ int32_t result)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    sCallbacksObj->OnGeofencePause(geofenceId, result);
}

static void GeofenceResumeCallback(
    /* [in] */ int32_t geofenceId,
    /* [in] */ int32_t result)
{
    if (!IsValidCallbackThread()) {
        return;
    }

    sCallbacksObj->OnGeofenceResume(geofenceId, result);
}

FlpGeofenceCallbacks sFlpGeofenceCallbacks = {
  sizeof(FlpGeofenceCallbacks),
  GeofenceTransitionCallback,
  GeofenceMonitorStatusCallback,
  GeofenceAddCallback,
  GeofenceRemoveCallback,
  GeofencePauseCallback,
  GeofenceResumeCallback,
  SetThreadEvent
};

void FlpHardwareProvider::NativeInit()
{
    if (sCallbacksObj == NULL) {
        sCallbacksObj = this;
    }

    // initialize the Flp interfaces
    if (sFlpInterface == NULL || sFlpInterface->init(&sFlpCallbacks) != 0) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (sFlpDiagnosticInterface != NULL) {
        sFlpDiagnosticInterface->init(&sFlpDiagnosticCallbacks);
    }

    if (sFlpGeofencingInterface != NULL) {
        sFlpGeofencingInterface->init(&sFlpGeofenceCallbacks);
    }

    // TODO: inject any device context if when needed
}

Int32 FlpHardwareProvider::NativeGetBatchSize()
{
    if (sFlpInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
        return 0;
    }

    return sFlpInterface->get_batch_size();
}

void FlpHardwareProvider::NativeStartBatching(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* options)
{
    if (sFlpInterface == NULL || options == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    FlpBatchOptions nativeOptions;
    TranslateFromObject(options, nativeOptions);
    int result = sFlpInterface->start_batching(requestId, &nativeOptions);
    ThrowOnError(result, __FUNCTION__);
}

void FlpHardwareProvider::NativeUpdateBatchingOptions(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* optionsObject)
{
    if (sFlpInterface == NULL || optionsObject == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    FlpBatchOptions options;
    TranslateFromObject(optionsObject, options);
    int result = sFlpInterface->update_batching_options(requestId, &options);
    ThrowOnError(result, __FUNCTION__);
}

void FlpHardwareProvider::NativeStopBatching(
    /* [in] */ Int32 id)
{
    if (sFlpInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->stop_batching(id);
}

void FlpHardwareProvider::NativeRequestBatchedLocation(
    /* [in] */ Int32 lastNLocations)
{
    if (sFlpInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->get_batched_location(lastNLocations);
}

void FlpHardwareProvider::NativeInjectLocation(
    /* [in] */ ILocation* locationObject)
{
    if (locationObject == NULL) {
        Logger::E(TAG, "Invalid location for injection: %p", locationObject);
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (sFlpInterface == NULL) {
        // there is no listener, bail
        return;
    }

    FlpLocation location;
    TranslateFromObject(locationObject, location);
    int result = sFlpInterface->inject_location(&location);
    if (result != FLP_RESULT_SUCCESS) {
        // do not throw but log, this operation should be fire and forget
        ALOGE("Error %d in '%s'", result, __FUNCTION__);
    }
}

void FlpHardwareProvider::NativeCleanup()
{
    if (sFlpInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->cleanup();

    if (sCallbacksObj != NULL) {
        sCallbacksObj = NULL;
    }

    sFlpInterface = NULL;
    sFlpDiagnosticInterface = NULL;
    sFlpDeviceContextInterface = NULL;
    sFlpGeofencingInterface = NULL;

    if (sHardwareDevice != NULL) {
        sHardwareDevice->close(sHardwareDevice);
        sHardwareDevice = NULL;
    }
}

Boolean FlpHardwareProvider::NativeIsDiagnosticSupported()
{
    return sFlpDiagnosticInterface != NULL;
}

void FlpHardwareProvider::NativeInjectDiagnosticData(
    /* [in] */ const String& data)
{
    if (data == NULL) {
        Logger::E("Invalid diagnostic data for injection: %s", data.string());
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (sFlpDiagnosticInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    int length = data.GetLength();
    int result = sFlpDiagnosticInterface->inject_data((char*)data.string(), length);
    ThrowOnError(result, __FUNCTION__);
}

Boolean FlpHardwareProvider::NativeIsDeviceContextSupported()
{
    return sFlpDeviceContextInterface != NULL;
}

void FlpHardwareProvider::NativeInjectDeviceContext(
    /* [in] */ Int32 deviceEnabledContext)
{
    if (sFlpDeviceContextInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    int result = sFlpDeviceContextInterface->inject_device_context(deviceEnabledContext);
    ThrowOnError(result, __FUNCTION__);
}

Boolean FlpHardwareProvider::NativeIsGeofencingSupported()
{
    return sFlpGeofencingInterface != NULL;
}

void FlpHardwareProvider::NativeAddGeofences(
        /* [in] */ ArrayOf<IGeofenceHardwareRequestParcelable*>* geofenceRequestsArray)
{
    if (geofenceRequestsArray == NULL) {
        Logger::E(TAG, "Invalid Geofences to add: %p", geofenceRequestsArray);
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (sFlpGeofencingInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    Int32 geofenceRequestsCount = geofenceRequestsArray->GetLength();
    if (geofenceRequestsCount == 0) {
        return;
    }

    Geofence* geofences = new Geofence[geofenceRequestsCount];
    if (geofences == NULL) {
        ThrowOnError(FLP_RESULT_INSUFFICIENT_MEMORY, __FUNCTION__);
    }

    for (int i = 0; i < geofenceRequestsCount; ++i) {
        geofences[i].data = new GeofenceData();
        geofences[i].options = new GeofenceOptions();
        TranslateGeofenceFromGeofenceHardwareRequestParcelable((*geofenceRequestsArray)[i], geofences[i]);
    }

    sFlpGeofencingInterface->add_geofences(geofenceRequestsCount, &geofences);
    if (geofences != NULL) {
        for(int i = 0; i < geofenceRequestsCount; ++i) {
            delete geofences[i].data;
            delete geofences[i].options;
        }
        delete[] geofences;
    }
}

void FlpHardwareProvider::NativePauseGeofence(
    /* [in] */ Int32 geofenceId)
{
    if (sFlpGeofencingInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->pause_geofence(geofenceId);
}

void FlpHardwareProvider::NativeResumeGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitorTransitions)
{
    if (sFlpGeofencingInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->resume_geofence(geofenceId, monitorTransitions);
}

void FlpHardwareProvider::NativeModifyGeofenceOption(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 lastTransition,
    /* [in] */ Int32 monitorTransitions,
    /* [in] */ Int32 notificationResponsiveness,
    /* [in] */ Int32 unknownTimer,
    /* [in] */ Int32 sourcesToUse)
{
    if (sFlpGeofencingInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    GeofenceOptions options = {
        lastTransition,
        monitorTransitions,
        notificationResponsiveness,
        unknownTimer,
        (uint32_t)sourcesToUse};

    sFlpGeofencingInterface->modify_geofence_option(geofenceId, &options);
}

void FlpHardwareProvider::NativeRemoveGeofences(
    /* [in] */ ArrayOf<Int32>* geofenceIdsArray)
{
    if (sFlpGeofencingInterface == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (geofenceIdsArray == NULL) {
        ThrowOnError(FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->remove_geofences(geofenceIdsArray->GetLength(), geofenceIdsArray->GetPayload());
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos