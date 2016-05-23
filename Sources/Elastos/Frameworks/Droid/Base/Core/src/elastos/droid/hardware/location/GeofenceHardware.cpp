
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/hardware/location/GeofenceHardware.h"
#include "elastos/droid/hardware/location/GeofenceHardwareRequest.h"
#include "elastos/droid/hardware/location/GeofenceHardwareRequestParcelable.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Build;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_2(GeofenceHardware::GeofenceHardwareMonitorCallbackWrapper,
        Object, IIGeofenceHardwareMonitorCallback, IBinder)

GeofenceHardware::GeofenceHardwareMonitorCallbackWrapper::GeofenceHardwareMonitorCallbackWrapper(
/* [in] */ IGeofenceHardwareMonitorCallback* c)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(c);
    wrs->GetWeakReference((IWeakReference**)&mCallback);
}

ECode GeofenceHardware::GeofenceHardwareMonitorCallbackWrapper::OnMonitoringSystemChange(
    /* [in] */ IGeofenceHardwareMonitorEvent* event)
{
    AutoPtr<IGeofenceHardwareMonitorCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareMonitorCallback, (IInterface**)&c);

    if (c == NULL) {
        return NOERROR;
    }

    // report the legacy event first, so older clients are not broken
    Int32 type;
    event->GetMonitoringType(&type);
    Int32 status;
    event->GetMonitoringStatus(&status);
    AutoPtr<ILocation> location;
    event->GetLocation((ILocation**)&location);

    c->OnMonitoringSystemChange(type,
            status == IGeofenceHardware::MONITOR_CURRENTLY_AVAILABLE, location);

    // and only call the updated callback on on L and above, this complies with the
    // documentation of GeofenceHardwareMonitorCallback
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::LOLLIPOP) {
        c->OnMonitoringSystemChange(event);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareMonitorCallbackWrapper::ToString(
    /* [out] */ String* info)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(GeofenceHardware::GeofenceHardwareCallbackWrapper,
        Object, IIGeofenceHardwareCallback, IBinder)

GeofenceHardware::GeofenceHardwareCallbackWrapper::GeofenceHardwareCallbackWrapper(
    /* [in] */ IGeofenceHardwareCallback* c,
    /* [in] */ GeofenceHardware* host)
    : mHost(host)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(c);
    wrs->GetWeakReference((IWeakReference**)&mCallback);
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::OnGeofenceTransition(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 transition,
    /* [in] */ ILocation* location,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 monitoringType)
{
    AutoPtr<IGeofenceHardwareCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareCallback, (IInterface**)&c);

    if (c != NULL) {
        c->OnGeofenceTransition(geofenceId, transition, location, timestamp,
                monitoringType);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::OnGeofenceAdd(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    AutoPtr<IGeofenceHardwareCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareCallback, (IInterface**)&c);

    if (c != NULL) {
        c->OnGeofenceAdd(geofenceId, status);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::OnGeofenceRemove(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    AutoPtr<IGeofenceHardwareCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareCallback, (IInterface**)&c);

    if (c != NULL) {
        c->OnGeofenceRemove(geofenceId, status);
        mHost->RemoveCallback(c);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::OnGeofencePause(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    AutoPtr<IGeofenceHardwareCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareCallback, (IInterface**)&c);

    if (c != NULL) {
        c->OnGeofencePause(geofenceId, status);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::OnGeofenceResume(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    AutoPtr<IGeofenceHardwareCallback> c;
    mCallback->Resolve(EIID_IGeofenceHardwareCallback, (IInterface**)&c);

    if (c != NULL) {
        c->OnGeofenceResume(geofenceId, status);
    }
    return NOERROR;
}

ECode GeofenceHardware::GeofenceHardwareCallbackWrapper::ToString(
    /* [out] */ String* info)
{
    return NOERROR;
}

const Int32 GeofenceHardware::NUM_MONITORS = 2;

CAR_INTERFACE_IMPL(GeofenceHardware, Object, IGeofenceHardware)

GeofenceHardware::GeofenceHardware(
        /* [in] */ IIGeofenceHardware* service)
{
}

GeofenceHardware::~GeofenceHardware()
{
}

ECode GeofenceHardware::GetMonitoringTypes(
    /* [out, callee] */ ArrayOf<Int32>** types)
{
    VALIDATE_NOT_NULL(types);

    //try {
    ECode ec = mService->GetMonitoringTypes(types);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *types = ArrayOf<Int32>::Alloc(0);
        REFCOUNT_ADD(*types);
    }
    //}

    return NOERROR;
}

ECode GeofenceHardware::GetStatusOfMonitoringType(
    /* [in] */ Int32 monitoringType,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    //try {
    ECode ec = mService->GetStatusOfMonitoringType(monitoringType, type);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *type = MONITOR_UNSUPPORTED;
    }
    //}
    return NOERROR;
}

ECode GeofenceHardware::AddGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [in] */ IGeofenceHardwareRequest* geofenceRequest,
    /* [in] */ IGeofenceHardwareCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
    ECode ec = NOERROR;
    Int32 type;
    geofenceRequest->GetType(&type);
    if (type == GeofenceHardwareRequest::GEOFENCE_TYPE_CIRCLE) {
        AutoPtr<GeofenceHardwareRequestParcelable> parce = new GeofenceHardwareRequestParcelable();
        parce->constructor(geofenceId, (GeofenceHardwareRequest*)geofenceRequest);
        AutoPtr<GeofenceHardwareCallbackWrapper> wrapper;
        FAIL_RETURN(GetCallbackWrapper(_callback, (GeofenceHardwareCallbackWrapper**)&wrapper))
        ec = mService->AddCircularFence(monitoringType, (IGeofenceHardwareRequestParcelable*)parce.Get(), wrapper, result);
        if (SUCCEEDED(ec)) {
            return NOERROR;
        }
    }
    else {
        //throw new IllegalArgumentException("Geofence Request type not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //} catch (RemoteException e) {
    //}
    *result = FALSE;
    return ec;
}

ECode GeofenceHardware::RemoveGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
    ECode ec = mService->RemoveGeofence(geofenceId, monitoringType, result);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *result = FALSE;
    }
    //}
    return NOERROR;
}

ECode GeofenceHardware::PauseGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
    ECode ec = mService->PauseGeofence(geofenceId, monitoringType, result);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *result = FALSE;
    }
    //}
    return NOERROR;
}

ECode GeofenceHardware::ResumeGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 monitorTransition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
    ECode ec = mService->ResumeGeofence(geofenceId, monitoringType, monitorTransition, result);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *result = FALSE;
    }
    //}
    return NOERROR;
}

ECode GeofenceHardware::RegisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    //try {
    AutoPtr<GeofenceHardwareMonitorCallbackWrapper> wrapper;
    ECode ec = GetMonitorCallbackWrapper(_callback, (GeofenceHardwareMonitorCallbackWrapper**)&wrapper);
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    ec = mService->RegisterForMonitorStateChangeCallback(monitoringType, wrapper, result);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    //}
    return NOERROR;
}

ECode GeofenceHardware::UnregisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean _result = FALSE;
    //try {
    AutoPtr<GeofenceHardwareMonitorCallbackWrapper> wrapper;
    ECode ec = GetMonitorCallbackWrapper(_callback, (GeofenceHardwareMonitorCallbackWrapper**)&wrapper);
    if (FAILED(ec)) {
        *result = _result;
        return ec;
    }
    ec = mService->UnregisterForMonitorStateChangeCallback(monitoringType, wrapper, &_result);
    if (FAILED(ec)) {
        *result = _result;
        return ec;
    }
    if (_result) {
        RemoveMonitorCallback(_callback);
    }
    //} catch (RemoteException e) {
    //}
    *result = _result;
    return ec;
}

void GeofenceHardware::RemoveCallback(
    /* [in] */ IGeofenceHardwareCallback* _callback)
{
    {    AutoLock syncLock(mCallbacksLock);
        mCallbacks.Erase(_callback);
    }
}

ECode GeofenceHardware::GetCallbackWrapper(
    /* [in] */ IGeofenceHardwareCallback* _callback,
    /* [out] */ GeofenceHardwareCallbackWrapper** wrapper)
{
    VALIDATE_NOT_NULL(wrapper);

    {    AutoLock syncLock(mCallbacksLock);
        AutoPtr<GeofenceHardwareCallbackWrapper> _wrapper = mCallbacks[_callback];
        if (_wrapper == NULL) {
            _wrapper = new GeofenceHardwareCallbackWrapper(_callback, this);
            mCallbacks[_callback] = _wrapper;
        }
        *wrapper = _wrapper;
        REFCOUNT_ADD(*wrapper);
        return NOERROR;
    }
    return NOERROR;
}

void GeofenceHardware::RemoveMonitorCallback(
    /* [in] */ IGeofenceHardwareMonitorCallback* _callback)
{
    {    AutoLock syncLock(mMonitorCallbacksLock);
        mMonitorCallbacks.Erase(_callback);
    }
}

ECode GeofenceHardware::GetMonitorCallbackWrapper(
    /* [in] */ IGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ GeofenceHardwareMonitorCallbackWrapper** wrapper)
{
    VALIDATE_NOT_NULL(wrapper);

    {    AutoLock syncLock(mMonitorCallbacksLock);
        AutoPtr<GeofenceHardwareMonitorCallbackWrapper> _wrapper = mMonitorCallbacks[_callback];
        if (_wrapper == NULL) {
            _wrapper = new GeofenceHardwareMonitorCallbackWrapper(_callback);
            mMonitorCallbacks[_callback] = _wrapper;
        }
        *wrapper = _wrapper;
        REFCOUNT_ADD(*wrapper);
        return NOERROR;
    }
    return NOERROR;
}

} //Location
} //Hardware
} //Droid
} //Elastos