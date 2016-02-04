
#include "elastos/droid/hardware/location/GeofenceHardwareImpl.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/server/location/FlpHardwareProvider.h"
#include "elastos/droid/server/location/CFusedGeofenceHardwareService.h"
#include "elastos/droid/server/location/CFusedLocationHardwareService.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Location::EIID_IIFusedLocationHardware;
using Elastos::Droid::Hardware::Location::IGeofenceHardware;
using Elastos::Droid::Hardware::Location::IGeofenceHardwareImpl;
using Elastos::Droid::Hardware::Location::GeofenceHardwareImpl;
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
using Elastos::Core::AutoLock;
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
    synchronized(this) {
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
    synchronized(this) {
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
const Int32 FlpHardwareProvider::FLP_RESULT_SUCCESS = 0;
const Int32 FlpHardwareProvider::FLP_RESULT_ERROR = -1;
const Int32 FlpHardwareProvider::FLP_RESULT_INSUFFICIENT_MEMORY = -2;
const Int32 FlpHardwareProvider::FLP_RESULT_TOO_MANY_GEOFENCES = -3;
const Int32 FlpHardwareProvider::FLP_RESULT_ID_EXISTS = -4;
const Int32 FlpHardwareProvider::FLP_RESULT_ID_UNKNOWN = -5;
const Int32 FlpHardwareProvider::FLP_RESULT_INVALID_GEOFENCE_TRANSITION = -6;
const Int32 FlpHardwareProvider::FLP_GEOFENCE_MONITOR_STATUS_UNAVAILABLE = 1<<0;
const Int32 FlpHardwareProvider::FLP_GEOFENCE_MONITOR_STATUS_AVAILABLE = 1<<1;
const FlpHardwareProvider::StaticInitializer FlpHardwareProvider::sInitializer;

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
    synchronized(this) {
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
    synchronized(this) {
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
    if(location != NULL) {
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

Boolean FlpHardwareProvider::NativeClassInit()
{
    // TODO:
/*
    sFlpInterface = NULL;

    // get references to the Java provider methods
    sOnLocationReport = env->GetMethodID(
        clazz,
        "onLocationReport",
        "([Landroid/location/Location;)V");
    sOnDataReport = env->GetMethodID(
        clazz,
        "onDataReport",
        "(Ljava/lang/String;)V"
        );
    sOnGeofenceTransition = env->GetMethodID(
        clazz,
        "onGeofenceTransition",
        "(ILandroid/location/Location;IJI)V"
        );
    sOnGeofenceMonitorStatus = env->GetMethodID(
        clazz,
        "onGeofenceMonitorStatus",
        "(IILandroid/location/Location;)V"
        );
    sOnGeofenceAdd = env->GetMethodID(clazz, "onGeofenceAdd", "(II)V");
    sOnGeofenceRemove = env->GetMethodID(clazz, "onGeofenceRemove", "(II)V");
    sOnGeofencePause = env->GetMethodID(clazz, "onGeofencePause", "(II)V");
    sOnGeofenceResume = env->GetMethodID(clazz, "onGeofenceResume", "(II)V");

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
*/
    return TRUE;
}

Boolean FlpHardwareProvider::NativeIsSupported()
{
    #if 0
    if (sFlpInterface == NULL) {
        return FALSE;
    }
    #endif
    return TRUE;
}

void FlpHardwareProvider::NativeInit()
{
    assert(0);
    #if 0
    // if(sCallbacksObj == NULL) {
    //     sCallbacksObj = env->NewGlobalRef(obj);
    // }

    // initialize the Flp interfaces
    if(sFlpInterface == NULL || sFlpInterface->init(&sFlpCallbacks) != 0) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    if(sFlpDiagnosticInterface != NULL) {
        sFlpDiagnosticInterface->init(&sFlpDiagnosticCallbacks);
    }

    if(sFlpGeofencingInterface != NULL) {
        sFlpGeofencingInterface->init(&sFlpGeofenceCallbacks);
    }

    // TODO: inject any device context if when needed
    #endif
}

Int32 FlpHardwareProvider::NativeGetBatchSize()
{
    #if 0
    if(sFlpInterface == NULL) {
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
        assert(0);
        return 0;
    }

    return sFlpInterface->get_batch_size();
    #endif
    return 0;
}

void FlpHardwareProvider::NativeStartBatching(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* options)
{
    #if 0
    if(sFlpInterface == NULL || options == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    int result = sFlpInterface->start_batching(requestId, &options);
    assert(0);
    // ThrowOnError(env, result, __FUNCTION__);
    #endif
}

void FlpHardwareProvider::NativeUpdateBatchingOptions(
    /* [in] */ Int32 requestId,
    /* [in] */ IFusedBatchOptions* optionsObject)
{
    #if 0
    if(sFlpInterface == NULL || optionsObject == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    int result = sFlpInterface->update_batching_options(id, &optionsObject);
    assert(0);
    // ThrowOnError(env, result, __FUNCTION__);
    #endif
}

void FlpHardwareProvider::NativeStopBatching(
    /* [in] */ Int32 id)
{
    #if 0
    if(sFlpInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->stop_batching(id);
    #endif
}

void FlpHardwareProvider::NativeRequestBatchedLocation(
    /* [in] */ Int32 lastNLocations)
{
    #if 0
    if(sFlpInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->get_batched_location(lastNLocations);
    #endif
}

void FlpHardwareProvider::NativeInjectLocation(
    /* [in] */ ILocation* location)
{
    #if 0
    if(location == NULL) {
        Logger::E(TAG, "Invalid location for injection: %p", location);
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    if(sFlpInterface == NULL) {
        // there is no listener, bail
        return;
    }

    int result = sFlpInterface->inject_location(&location);
    if (result != FLP_RESULT_SUCCESS) {
        // do not throw but log, this operation should be fire and forget
        ALOGE("Error %d in '%s'", result, __FUNCTION__);
    }
    #endif
}

void FlpHardwareProvider::NativeCleanup()
{
    #if 0
    if(sFlpInterface == NULL) {
        ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpInterface->cleanup();

    assert(0);
    // if(sCallbacksObj != NULL) {
    //     env->DeleteGlobalRef(sCallbacksObj);
    //     sCallbacksObj = NULL;
    // }

    sFlpInterface = NULL;
    sFlpDiagnosticInterface = NULL;
    sFlpDeviceContextInterface = NULL;
    sFlpGeofencingInterface = NULL;

    assert(0);
    // if(sHardwareDevice != NULL) {
    //     sHardwareDevice->close(sHardwareDevice);
    //     sHardwareDevice = NULL;
    // }
    #endif
}

Boolean FlpHardwareProvider::NativeIsDiagnosticSupported()
{
    #if 0
    return sFlpDiagnosticInterface != NULL;
    #endif
    return FALSE;
}

void FlpHardwareProvider::NativeInjectDiagnosticData(
    /* [in] */ const String& data)
{
    #if 0
    if(data == NULL) {
        Logger::E("Invalid diagnostic data for injection: %s", data.string());
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    if(sFlpDiagnosticInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    int length = data.GetLength();
    int result = sFlpDiagnosticInterface->inject_data((char*) data, length);
    assert(0);
    // ThrowOnError(env, result, __FUNCTION__);
    #endif
}

Boolean FlpHardwareProvider::NativeIsDeviceContextSupported()
{
    #if 0
    return sFlpDeviceContextInterface != NULL;
    #endif
    return FALSE;
}

void FlpHardwareProvider::NativeInjectDeviceContext(
    /* [in] */ Int32 deviceEnabledContext)
{
    #if 0
    if(sFlpDeviceContextInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    int result = sFlpDeviceContextInterface->inject_device_context(deviceEnabledContext);
    assert(0);
    // ThrowOnError(env, result, __FUNCTION__);
    #endif
}

Boolean FlpHardwareProvider::NativeIsGeofencingSupported()
{
    #if 0
    return sFlpGeofencingInterface != NULL;
    #endif
    return FALSE;
}

void FlpHardwareProvider::NativeAddGeofences(
        /* [in] */ ArrayOf<IGeofenceHardwareRequestParcelable*>* geofenceRequestsArray)
{
    #if 0
    if(geofenceRequestsArray == NULL) {
        Logger::E(TAG, "Invalid Geofences to add: %p", geofenceRequestsArray);
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    if (sFlpGeofencingInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    Int32 geofenceRequestsCount = geofenceRequestsArray->GetLength();
    if(geofenceRequestsCount == 0) {
        return;
    }

    Geofence* geofences = new Geofence[geofenceRequestsCount];
    if (geofences == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_INSUFFICIENT_MEMORY, __FUNCTION__);
    }

    for (int i = 0; i < geofenceRequestsCount; ++i) {
        geofences[i].data = new GeofenceData();
        geofences[i].options = new GeofenceOptions();
        assert(0);
        // jobject geofenceObject = env->GetObjectArrayElement(geofenceRequestsArray, i);

        // TranslateGeofenceFromGeofenceHardwareRequestParcelable(env, geofenceObject, geofences[i]);
        // env->DeleteLocalRef(geofenceObject);
    }

    sFlpGeofencingInterface->add_geofences(geofenceRequestsCount, &geofences);
    if (geofences != NULL) {
        for(int i = 0; i < geofenceRequestsCount; ++i) {
            delete geofences[i].data;
            delete geofences[i].options;
        }
        delete[] geofences;
    }
    #endif
}

void FlpHardwareProvider::NativePauseGeofence(
    /* [in] */ Int32 geofenceId)
{
    #if 0
    if(sFlpGeofencingInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->pause_geofence(geofenceId);
    #endif
}

void FlpHardwareProvider::NativeResumeGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitorTransitions)
{
    #if 0
    if(sFlpGeofencingInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->resume_geofence(geofenceId, monitorTransitions);
    #endif
}

void FlpHardwareProvider::NativeModifyGeofenceOption(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 lastTransition,
    /* [in] */ Int32 monitorTransitions,
    /* [in] */ Int32 notificationResponsiveness,
    /* [in] */ Int32 unknownTimer,
    /* [in] */ Int32 sourcesToUse)
{
    #if 0
    if(sFlpGeofencingInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    GeofenceOptions options = {
        lastTransition,
        monitorTransitions,
        notificationResponsiveness,
        unknownTimer,
        (uint32_t)sourcesToUse};

    sFlpGeofencingInterface->modify_geofence_option(geofenceId, &options);
    #endif
}

void FlpHardwareProvider::NativeRemoveGeofences(
    /* [in] */ ArrayOf<Int32>* geofenceIdsArray)
{
    #if 0
    if(sFlpGeofencingInterface == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    Int32 geofenceIdsCount = geofenceIdsArray->GetLength();
    AutoPtr<ArrayOf<Int32> > geofenceIds = ArrayOf<Int32>::Alloc(geofenceIdsCount);
    for (Int32 i = 0; i < geofenceIds->GetLength(); i++) {
        (*geofenceIds)[i] = (*geofenceIdsArray)[i];
    }
    if(geofenceIds == NULL) {
        assert(0);
        // ThrowOnError(env, FLP_RESULT_ERROR, __FUNCTION__);
    }

    sFlpGeofencingInterface->remove_geofences(geofenceIdsCount, geofenceIds);
    // env->ReleaseIntArrayElements(geofenceIdsArray, geofenceIds, 0 /*mode*/);
    #endif
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos