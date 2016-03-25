
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/hardware/location/GeofenceHardwareService.h"
#include "elastos/droid/hardware/location/GeofenceHardwareImpl.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Os::Binder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL( GeofenceHardwareService::MyBinder, Object, IIGeofenceHardware)

GeofenceHardwareService::MyBinder::MyBinder(
    /* [in] */ GeofenceHardwareService* host)
    : mHost(host)
{
}

ECode GeofenceHardwareService::MyBinder::SetGpsGeofenceHardware(
    /* [in] */ IIGpsGeofenceHardware* service)
{
    return mHost->mGeofenceHardwareImpl->SetGpsHardwareGeofence(service);
}

ECode GeofenceHardwareService::MyBinder::SetFusedGeofenceHardware(
    /* [in] */ IIFusedGeofenceHardware* service)
{
    return mHost->mGeofenceHardwareImpl->SetFusedGeofenceHardware(service);
}

ECode GeofenceHardwareService::MyBinder::GetMonitoringTypes(
    /* [out, callee] */ ArrayOf<Int32>** types)
{
    VALIDATE_NOT_NULL(types);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    return mHost->mGeofenceHardwareImpl->GetMonitoringTypes(types);
}

ECode GeofenceHardwareService::MyBinder::GetStatusOfMonitoringType(
    /* [in] */ Int32 monitoringType,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    return mHost->mGeofenceHardwareImpl->GetStatusOfMonitoringType(monitoringType, type);
}

ECode GeofenceHardwareService::MyBinder::AddCircularFence(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IGeofenceHardwareRequestParcelable* request,
    /* [in] */ IIGeofenceHardwareCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->AddCircularFence(monitoringType, request, _callback, result);
}

ECode GeofenceHardwareService::MyBinder::RemoveGeofence(
    /* [in] */ Int32 id,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->RemoveGeofence(id, monitoringType, result);
}

ECode GeofenceHardwareService::MyBinder::PauseGeofence(
    /* [in] */ Int32 id,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->PauseGeofence(id, monitoringType, result);
}

ECode GeofenceHardwareService::MyBinder::ResumeGeofence(
    /* [in] */ Int32 id,
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 monitorTransitions,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->ResumeGeofence(id, monitoringType, monitorTransitions, result);
}

ECode GeofenceHardwareService::MyBinder::RegisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->RegisterForMonitorStateChangeCallback(monitoringType,
            _callback, result);
}

ECode GeofenceHardwareService::MyBinder::UnregisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mHost->mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            String("Location Hardware permission not granted to access hardware geofence")))

    mHost->CheckPermission(Binder::GetCallingPid(), Binder::GetCallingUid(), monitoringType);
    return mHost->mGeofenceHardwareImpl->UnregisterForMonitorStateChangeCallback(monitoringType,
            _callback, result);
}

ECode GeofenceHardwareService::MyBinder::ToString(
    /* [out]*/ String* str)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(GeofenceHardwareService, Service, IGeofenceHardwareService)

GeofenceHardwareService::GeofenceHardwareService()
    : mGeofenceHardwareImpl(NULL)
    , mContext(NULL)
{
    mBinder = new MyBinder(this);
}

GeofenceHardwareService::~GeofenceHardwareService()
{
}

ECode GeofenceHardwareService::constructor()
{
    return Service::constructor();
}

ECode GeofenceHardwareService::OnCreate()
{
    mContext = this;
    mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    return NOERROR;
}

ECode GeofenceHardwareService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = mBinder;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode GeofenceHardwareService::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    *succeeded = FALSE;
    return NOERROR;
}

ECode GeofenceHardwareService::OnDestroy()
{
    mGeofenceHardwareImpl = NULL;
    return NOERROR;
}

ECode GeofenceHardwareService::CheckPermission(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 monitoringType)
{
    Int32 AllowedLevel, MonitoringLevel;
    mGeofenceHardwareImpl->GetAllowedResolutionLevel(pid, uid, &AllowedLevel);
    mGeofenceHardwareImpl->GetMonitoringResolutionLevel(monitoringType, &MonitoringLevel);

    if (AllowedLevel < MonitoringLevel) {
        // throw new SecurityException("Insufficient permissions to access hardware geofence for"
        //         + " type: " + monitoringType);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

} //Location
} //Hardware
} //Droid
} //Elastos