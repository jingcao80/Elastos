
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/hardware/location/GeofenceHardwareImpl.h"
#include "elastos/droid/hardware/location/GeofenceHardware.h"
#include "elastos/droid/hardware/location/GeofenceHardwareMonitorEvent.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Hardware::Location::IGeofenceHardware;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

const Int32 GeofenceHardwareImpl::GEOFENCE_TRANSITION_CALLBACK = 1;
const Int32 GeofenceHardwareImpl::ADD_GEOFENCE_CALLBACK = 2;
const Int32 GeofenceHardwareImpl::REMOVE_GEOFENCE_CALLBACK = 3;
const Int32 GeofenceHardwareImpl::PAUSE_GEOFENCE_CALLBACK = 4;
const Int32 GeofenceHardwareImpl::RESUME_GEOFENCE_CALLBACK = 5;
const Int32 GeofenceHardwareImpl::GEOFENCE_CALLBACK_BINDER_DIED = 6;

const Int32 GeofenceHardwareImpl::GEOFENCE_STATUS = 1;
const Int32 GeofenceHardwareImpl::CALLBACK_ADD = 2;
const Int32 GeofenceHardwareImpl::CALLBACK_REMOVE = 3;
const Int32 GeofenceHardwareImpl::MONITOR_CALLBACK_BINDER_DIED = 4;

const Int32 GeofenceHardwareImpl::REAPER_GEOFENCE_ADDED = 1;
const Int32 GeofenceHardwareImpl::REAPER_MONITOR_CALLBACK_ADDED = 2;
const Int32 GeofenceHardwareImpl::REAPER_REMOVED = 3;

GeofenceHardwareImpl::GeofenceTransition::GeofenceTransition(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 transition,
    /* [in] */ Int64 timestamp,
    /* [in] */ ILocation* location,
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 sourcesUsed)
    : mGeofenceId(geofenceId)
    , mTransition(transition)
    , mTimestamp(timestamp)
    , mLocation(location)
    , mMonitoringType(monitoringType)
    , mSourcesUsed(sourcesUsed)
{
}

CAR_INTERFACE_IMPL(GeofenceHardwareImpl::Reaper, Object, IProxyDeathRecipient)

GeofenceHardwareImpl::Reaper::Reaper(
    /* [in] */ IIGeofenceHardwareCallback* c,
    /* [in] */ Int32 monitoringType,
    /* [in] */ GeofenceHardwareImpl* host)
    : mCallback(c)
    , mMonitoringType(monitoringType)
    , mHost(host)
{
}

GeofenceHardwareImpl::Reaper::Reaper(
    /* [in] */ IIGeofenceHardwareMonitorCallback* c,
    /* [in] */ Int32 monitoringType,
    /* [in] */ GeofenceHardwareImpl* host)
    : mMonitorCallback(c)
    , mMonitoringType(monitoringType)
    , mHost(host)
{
}

ECode GeofenceHardwareImpl::Reaper::ProxyDied()
{
    AutoPtr<IMessage> m;
    Boolean result;
    if (mCallback != NULL) {
        mHost->mGeofenceHandler->ObtainMessage(GeofenceHardwareImpl::GEOFENCE_CALLBACK_BINDER_DIED,
                mCallback, (IMessage**)&m);
        m->SetArg1(mMonitoringType);
        mHost->mGeofenceHandler->SendMessage(m, &result);
    }
    else if (mMonitorCallback != NULL) {
        mHost->mCallbacksHandler->ObtainMessage(GeofenceHardwareImpl::MONITOR_CALLBACK_BINDER_DIED,
                mMonitorCallback, (IMessage**)&m);
        m->SetArg1(mMonitoringType);
        mHost->mCallbacksHandler->SendMessage(m, &result);
    }
    AutoPtr<IMessage> reaperMessage;
    mHost->mReaperHandler->ObtainMessage(GeofenceHardwareImpl::REAPER_REMOVED, TO_IINTERFACE(this),
            (IMessage**)&reaperMessage);
    mHost->mReaperHandler->SendMessage(reaperMessage, &result);
    return NOERROR;
}

ECode GeofenceHardwareImpl::Reaper::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    Int32 result = 17;
    if (mCallback != NULL) {
        Int32 callbackHashCode;
        (IObject::Probe(mCallback))->GetHashCode(&callbackHashCode);
        result = 31 * result + callbackHashCode;
    }
    else {
        result = 31 * result + 0;
    }

    if (mMonitorCallback != NULL) {
        Int32 monitorCallbackHashCode;
        (IObject::Probe(mMonitorCallback))->GetHashCode(&monitorCallbackHashCode);
        result = 31 * result + monitorCallbackHashCode;
    }
    else {
        result = 31 * result + 0;
    }

    result = 31 * result + mMonitoringType;
    *code = result;
    return NOERROR;
}

//@Override
ECode GeofenceHardwareImpl::Reaper::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *result = TRUE;
        return NOERROR;
    }

    Reaper* rhs = (Reaper*)IProxyDeathRecipient::Probe(obj);
    *result = rhs->mCallback == mCallback && rhs->mMonitorCallback == mMonitorCallback &&
            rhs->mMonitoringType == mMonitoringType;
    return NOERROR;
}

GeofenceHardwareImpl::GeofenceHandler::GeofenceHandler(
    /* [in] */ GeofenceHardwareImpl* host)
    : mHost(host)
{
}

ECode GeofenceHardwareImpl::GeofenceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 geofenceId;
    //Int32 status;
    AutoPtr<IIGeofenceHardwareCallback> _callback;
    Object& lockObj = mHost->mGeofencesLock;

    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case GeofenceHardwareImpl::ADD_GEOFENCE_CALLBACK:
            msg->GetArg1(&geofenceId);
            {    AutoLock syncLock(lockObj);
                _callback = (mHost->mGeofences)[geofenceId];
            }

            if (_callback != NULL) {
                //try {
                Int32 arg2;
                msg->GetArg2(&arg2);
                ECode ec = _callback->OnGeofenceAdd(geofenceId, arg2);
                //} catch (RemoteException e) {
                if (FAILED(ec)) {
                    Logger::I(TAG, "Remote Exception:"/* + e*/);
                }
                //}
            }
            mHost->ReleaseWakeLock();
            break;
        case GeofenceHardwareImpl::REMOVE_GEOFENCE_CALLBACK:
            msg->GetArg1(&geofenceId);
            {    AutoLock syncLock(lockObj);
                _callback = (mHost->mGeofences)[geofenceId];
            }

            if (_callback != NULL) {
                //try {
                Int32 arg2;
                msg->GetArg2(&arg2);
                ECode ec = _callback->OnGeofenceRemove(geofenceId, arg2);
                //} catch (RemoteException e) {}
                if (FAILED(ec)) {
                    {    AutoLock syncLock(lockObj);
                        (mHost->mGeofences).Erase(geofenceId);
                    }
                }
            }
            mHost->ReleaseWakeLock();
            break;

        case GeofenceHardwareImpl::PAUSE_GEOFENCE_CALLBACK:
            msg->GetArg1(&geofenceId);
            {    AutoLock syncLock(lockObj);
                _callback = (mHost->mGeofences)[geofenceId];
            }

            if (_callback != NULL) {
                //try {
                Int32 arg2;
                msg->GetArg2(&arg2);
                _callback->OnGeofencePause(geofenceId, arg2);
                //} catch (RemoteException e) {}
            }
            mHost->ReleaseWakeLock();
            break;

        case GeofenceHardwareImpl::RESUME_GEOFENCE_CALLBACK:
            msg->GetArg1(&geofenceId);
            {    AutoLock syncLock(lockObj);
                _callback = (mHost->mGeofences)[geofenceId];
            }

            if (_callback != NULL) {
                //try {
                Int32 arg2;
                msg->GetArg2(&arg2);
                _callback->OnGeofenceResume(geofenceId, arg2);
                //} catch (RemoteException e) {}
            }
            mHost->ReleaseWakeLock();
            break;

        case GeofenceHardwareImpl::GEOFENCE_TRANSITION_CALLBACK:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<GeofenceTransition> geofenceTransition;
            geofenceTransition = (GeofenceTransition*)IObject::Probe(obj);
            {    AutoLock syncLock(lockObj);
                _callback = (mHost->mGeofences)[geofenceTransition->mGeofenceId];

                // need to keep access to mGeofences synchronized at all times
                if (DEBUG) {
                    StringBuilder sb;
                    sb += "GeofenceTransistionCallback: GPS : GeofenceId: ";
                    sb += geofenceTransition->mGeofenceId;
                    sb += " Transition: ";
                    sb += geofenceTransition->mTransition;
                    sb += " Location: ";
                    sb += geofenceTransition->mLocation;
                    sb += ":";
                    assert(0 && "HashMap ToString");
                    //sb += mGeofences;
                    Logger::D(TAG, sb.ToString());
                }
            }

            if (_callback != NULL) {
                //try {
                _callback->OnGeofenceTransition(
                        geofenceTransition->mGeofenceId, geofenceTransition->mTransition,
                        geofenceTransition->mLocation, geofenceTransition->mTimestamp,
                        geofenceTransition->mMonitoringType);
                //} catch (RemoteException e) {}
            }
            mHost->ReleaseWakeLock();
            break;
        }
        case GeofenceHardwareImpl::GEOFENCE_CALLBACK_BINDER_DIED:
        {
            // Find all geofences associated with this callback and remove them.
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            _callback = IIGeofenceHardwareCallback::Probe(obj);
            if (DEBUG) {
                StringBuilder sb;
                sb += "Geofence callback reaped:";
                sb += Object::ToString(_callback);
                Logger::D(TAG, sb.ToString());
            }
            Int32 monitoringType;
            msg->GetArg1(&monitoringType);
            {    AutoLock syncLock(lockObj);
                for (UInt32 i = 0; i < (mHost->mGeofences).GetSize(); i++) {
                    Boolean result;
                    IObject::Probe(mHost->mGeofences[i])->Equals(_callback, &result);
                    if (result) {
                        mHost->RemoveGeofence(i, monitoringType, &result);
                        (mHost->mGeofences).Erase(i);
                    }
               }
           }
        }
    }

    return NOERROR;
}

GeofenceHardwareImpl::CallbacksHandler::CallbacksHandler(
    /* [in] */ GeofenceHardwareImpl* host)
    : mHost(host)
{
}

ECode GeofenceHardwareImpl::CallbacksHandler::HandleMessage(
     /* [in] */ IMessage* msg)
{
    Int32 monitoringType;
    AutoPtr<IArrayList> callbackList;
    AutoPtr<IIGeofenceHardwareMonitorCallback> _callback;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IGeofenceHardwareMonitorEvent> event;
    switch (what) {
        case GeofenceHardwareImpl::GEOFENCE_STATUS:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IGeofenceHardwareMonitorEvent> event = IGeofenceHardwareMonitorEvent::Probe(obj);
            Int32 type;
            event->GetMonitoringType(&type);
            callbackList = (*(mHost->mCallbacks))[type];
            if (callbackList != NULL) {
                if (DEBUG) {
                    StringBuilder sb;
                    sb += "MonitoringSystemChangeCallback: ";
                    String str;
                    IObject::Probe(event)->ToString(&str);
                    sb += str;
                    Logger::D(TAG, sb.ToString());
                }

                Int32 size;
                callbackList->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> _obj;
                    callbackList->Get(i, (IInterface**)&_obj);
                    AutoPtr<IIGeofenceHardwareMonitorCallback> c = IIGeofenceHardwareMonitorCallback::Probe(_obj);
                    //try {
                    ECode ec = c->OnMonitoringSystemChange(event);
                    //} catch (RemoteException e) {
                    if (FAILED(ec)) {
                        Logger::D(TAG, "Error reporting onMonitoringSystemChange."/*, e*/);
                    }
                    //}
                }
            }
            mHost->ReleaseWakeLock();
            break;
        }
        case GeofenceHardwareImpl::CALLBACK_ADD:
        {
            msg->GetArg1(&monitoringType);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            _callback = IIGeofenceHardwareMonitorCallback::Probe(obj);
            callbackList = (*(mHost->mCallbacks))[monitoringType];
            if (callbackList == NULL) {
                CArrayList::New((IArrayList**)&callbackList);
                //mCallbacks[monitoringType] = callbackList;
                mHost->mCallbacks->Set(monitoringType, callbackList);
            }
            Boolean result;
            callbackList->Contains(TO_IINTERFACE(_callback), &result);
            if (!result) {
                callbackList->Add(TO_IINTERFACE(_callback));
            }
            break;
        }
        case GeofenceHardwareImpl::CALLBACK_REMOVE:
        {
            msg->GetArg1(&monitoringType);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            _callback = IIGeofenceHardwareMonitorCallback::Probe(obj);
            callbackList = (*(mHost->mCallbacks))[monitoringType];
            if (callbackList != NULL) {
                callbackList->Remove(TO_IINTERFACE(_callback));
            }
            break;
        }
        case GeofenceHardwareImpl::MONITOR_CALLBACK_BINDER_DIED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            _callback = IIGeofenceHardwareMonitorCallback::Probe(obj);
            if (DEBUG) {
                StringBuilder sb;
                sb += "Monitor callback reaped:";
                String str;
                IObject::Probe(_callback)->ToString(&str);
                sb += str;
                Logger::D(TAG, sb.ToString());
            }

            Int32 arg1;
            msg->GetArg1(&arg1);
            if (callbackList != NULL) {
                Boolean result;
                callbackList->Contains(TO_IINTERFACE(_callback), &result);
                if (result) {
                    callbackList->Remove(TO_IINTERFACE(_callback));
                }
            }
        }
    }

    return NOERROR;
}

GeofenceHardwareImpl::ReaperHandler::ReaperHandler(
    /* [in] */ GeofenceHardwareImpl* host)
    : mHost(host)
{
}

ECode GeofenceHardwareImpl::ReaperHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<Reaper> r;
    AutoPtr<IIGeofenceHardwareCallback> _callback;
    AutoPtr<IIGeofenceHardwareMonitorCallback> monitorCallback;
    Int32 monitoringType;
    Boolean result;

    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case GeofenceHardwareImpl::REAPER_GEOFENCE_ADDED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            _callback = IIGeofenceHardwareCallback::Probe(obj);
            msg->GetArg1(&monitoringType);
            r = new Reaper(_callback, monitoringType, mHost);
            mHost->mReapers->Contains(TO_IINTERFACE(r), &result);
            if (!result) {
                mHost->mReapers->Add(TO_IINTERFACE(r));
                AutoPtr<IProxy> proxy = (IProxy*)_callback->Probe(EIID_IProxy);
                //try {
                proxy->LinkToDeath(r, 0);
                //} catch (RemoteException e) {}
            }
            break;
        }
        case GeofenceHardwareImpl::REAPER_MONITOR_CALLBACK_ADDED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            monitorCallback = IIGeofenceHardwareMonitorCallback::Probe(obj);
            msg->GetArg1(&monitoringType);

            r = new Reaper(monitorCallback, monitoringType, mHost);
            mHost->mReapers->Contains(TO_IINTERFACE(r), &result);
            if (!result) {
                mHost->mReapers->Add(TO_IINTERFACE(r));
                AutoPtr<IProxy> proxy = (IProxy*)monitorCallback->Probe(EIID_IProxy);
                //try {
                proxy->LinkToDeath(r, 0);
                //} catch (RemoteException e) {}
            }
            break;
        }
        case GeofenceHardwareImpl::REAPER_REMOVED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            r = (Reaper*)IObject::Probe(obj);
            mHost->mReapers->Remove(TO_IINTERFACE(r));
        }
    }

    return NOERROR;
}

const String GeofenceHardwareImpl::TAG("GeofenceHardwareImpl");
const Boolean GeofenceHardwareImpl::DEBUG = FALSE;// TODO: = Log.isLoggable(TAG, Log.DEBUG);

Object GeofenceHardwareImpl::sLock;

AutoPtr<IGeofenceHardwareImpl> GeofenceHardwareImpl::sInstance;

const Int32 GeofenceHardwareImpl::LOCATION_INVALID = 0;
const Int32 GeofenceHardwareImpl::LOCATION_HAS_LAT_LONG = 1;
const Int32 GeofenceHardwareImpl::LOCATION_HAS_ALTITUDE = 2;
const Int32 GeofenceHardwareImpl::LOCATION_HAS_SPEED = 4;
const Int32 GeofenceHardwareImpl::LOCATION_HAS_BEARING = 8;
const Int32 GeofenceHardwareImpl::LOCATION_HAS_ACCURACY = 16;

// Resolution level constants used for permission checks.
// These constants must be in increasing order of finer resolution.
const Int32 GeofenceHardwareImpl::RESOLUTION_LEVEL_NONE = 1;
const Int32 GeofenceHardwareImpl::RESOLUTION_LEVEL_COARSE = 2;
const Int32 GeofenceHardwareImpl::RESOLUTION_LEVEL_FINE = 3;

CAR_INTERFACE_IMPL(GeofenceHardwareImpl, Object, IGeofenceHardwareImpl)

GeofenceHardwareImpl::GeofenceHardwareImpl()
    : mFusedService(NULL)
    , mGpsService(NULL)
{
    mCallbacks = ArrayOf<IArrayList*>::Alloc(GeofenceHardware::NUM_MONITORS);
    CArrayList::New((IArrayList**)&mReapers);
    mSupportedMonitorTypes = ArrayOf<Int32>::Alloc(GeofenceHardware::NUM_MONITORS);
}

GeofenceHardwareImpl::~GeofenceHardwareImpl()
{
}

AutoPtr<IGeofenceHardwareImpl> GeofenceHardwareImpl::GetInstance(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(sLock);
        if (sInstance == NULL) {
            sInstance = new GeofenceHardwareImpl(context);
        }
    }
    return sInstance;
}

GeofenceHardwareImpl::GeofenceHardwareImpl(
    /* [in] */ IContext* context)
    : mContext(context)
{
    // Init everything to unsupported.
    SetMonitorAvailability(IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE,
            IGeofenceHardware::MONITOR_UNSUPPORTED);
    SetMonitorAvailability(
            IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE,
            IGeofenceHardware::MONITOR_UNSUPPORTED);
}

ECode GeofenceHardwareImpl::AcquireWakeLock()
{
    if (mWakeLock == NULL) {
        AutoPtr<IInterface> service;
        FAIL_RETURN(mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service))
        AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
        powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG,
                (IPowerManagerWakeLock**&)mWakeLock);
    }
    return mWakeLock->AcquireLock();
}

ECode GeofenceHardwareImpl::ReleaseWakeLock()
{
    Boolean isHeld;
    mWakeLock->IsHeld(&isHeld);
    if (isHeld) {
        mWakeLock->ReleaseLock();
    }
    return NOERROR;
}

void GeofenceHardwareImpl::UpdateGpsHardwareAvailability()
{
    //Check which monitors are available.
    Boolean gpsSupported;
    //try {
    ECode ec = mGpsService->IsHardwareGeofenceSupported(&gpsSupported);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "Remote Exception calling LocationManagerService");
        gpsSupported = FALSE;
    }
    //}

    if (gpsSupported) {
        // Its assumed currently available at startup.
        // native layer will update later.
        SetMonitorAvailability(IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE,
                IGeofenceHardware::MONITOR_CURRENTLY_AVAILABLE);
    }
    return;
}

void GeofenceHardwareImpl::UpdateFusedHardwareAvailability()
{
    Boolean fusedSupported;
    //try {
    ECode ec = NOERROR;
    if (mFusedService != NULL) {
        ec = mFusedService->IsSupported(&fusedSupported);
    }
    else {
        fusedSupported = FALSE;
    }
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "RemoteException calling LocationManagerService");
        fusedSupported = FALSE;
    }
    //}

    if (fusedSupported) {
        SetMonitorAvailability(
                IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE,
                IGeofenceHardware::MONITOR_CURRENTLY_AVAILABLE);
    }
    return;
}

ECode GeofenceHardwareImpl::SetGpsHardwareGeofence(
    /* [in] */ IIGpsGeofenceHardware* service)
{
    if (mGpsService == NULL) {
        mGpsService = service;
        UpdateGpsHardwareAvailability();
    }
    else if (service == NULL) {
        mGpsService = NULL;
        Logger::W(TAG, "GPS Geofence Hardware service seems to have crashed");
    }
    else {
        Logger::E(TAG, "Error: GpsService being set again.");
    }
    return NOERROR;
}

ECode GeofenceHardwareImpl::SetFusedGeofenceHardware(
    /* [in] */ IIFusedGeofenceHardware* service)
{
    if (mFusedService == NULL) {
        mFusedService = service;
        UpdateFusedHardwareAvailability();
    }
    else if (service == NULL) {
        mFusedService = NULL;
        Logger::W(TAG, "Fused Geofence Hardware service seems to have crashed");
    }
    else {
        Logger::E(TAG, "Error: FusedService being set again");
    }
    return NOERROR;
}

ECode GeofenceHardwareImpl::GetMonitoringTypes(
    /* [out, callee] */ ArrayOf<Int32>** types)
{
    VALIDATE_NOT_NULL(types);

    Boolean gpsSupported = FALSE;
    Boolean fusedSupported = FALSE;
    {    AutoLock syncLock(mSupportedMonitorTypesLock);
        gpsSupported = (*mSupportedMonitorTypes)[IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE]
                != IGeofenceHardware::MONITOR_UNSUPPORTED;
        fusedSupported = (*mSupportedMonitorTypes)[IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE]
                != IGeofenceHardware::MONITOR_UNSUPPORTED;
    }

    if (gpsSupported) {
        if (fusedSupported) {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
            (*array)[0] = IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE;
            (*array)[1] = IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE;
            *types = array;
            REFCOUNT_ADD(*types);
            return NOERROR;
        }
        else {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
            (*array)[0] = IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE;
            *types = array;
            REFCOUNT_ADD(*types);
            return NOERROR;
        }
    }
    else if (fusedSupported) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
        (*array)[0] = IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE;
        *types = array;
        REFCOUNT_ADD(*types);
        return NOERROR;
    }
    else {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(0);
        *types = array;
        REFCOUNT_ADD(*types);
        return NOERROR;
    }
    return NOERROR;
}

ECode GeofenceHardwareImpl::GetStatusOfMonitoringType(
    /* [in] */ Int32 monitoringType,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    {    AutoLock syncLock(mSupportedMonitorTypesLock);
        if (monitoringType >= mSupportedMonitorTypes->GetLength() || monitoringType < 0) {
            //throw new IllegalArgumentException("Unknown monitoring type");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *type = (*mSupportedMonitorTypes)[monitoringType];
    }
    return NOERROR;
}

ECode GeofenceHardwareImpl::AddCircularFence(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IGeofenceHardwareRequestParcelable* request,
    /* [in] */ IIGeofenceHardwareCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 geofenceId;
    request->GetId(&geofenceId);

    // This API is not thread safe. Operations on the same geofence need to be serialized
    // by upper layers
    if (DEBUG) {
        StringBuilder sb;
        sb += "addCircularFence: monitoringType=";
        sb += monitoringType;
        sb += ", ";
        sb += TO_STR(request);
        Logger::D(TAG, sb.ToString());
    }
    Boolean _result;

    // The callback must be added before addCircularHardwareGeofence is called otherwise the
    // callback might not be called after the geofence is added in the geofence hardware.
    // This also means that the callback must be removed if the addCircularHardwareGeofence
    // operations is not called or fails.
    {    AutoLock syncLock(mGeofencesLock);
        mGeofences[geofenceId] = _callback;
    }

    AutoPtr<ArrayOf<IGeofenceHardwareRequestParcelable*> > req;
    switch (monitoringType) {
        case IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE:
        {
            if (mGpsService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            _result = FALSE;
            Int32 id;
            request->GetId(&id);
            Double latitude;
            request->GetLatitude(&latitude);
            Double longitude;
            request->GetLongitude(&longitude);
            Double radius;
            request->GetRadius(&radius);
            Int32 transition;
            request->GetLastTransition(&transition);
            Int32 transitions;
            request->GetMonitorTransitions(&transitions);
            Int32 responsiveness;
            request->GetNotificationResponsiveness(&responsiveness);
            Int32 timer;
            request->GetUnknownTimer(&timer);
            ECode ec = mGpsService->AddCircularHardwareGeofence(id, latitude, longitude, radius, transition,
                    transitions, responsiveness, timer, &_result);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "AddGeofence: Remote Exception calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        case IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE:
        {
            if (mFusedService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            req = ArrayOf<IGeofenceHardwareRequestParcelable*>::Alloc(1);
            req->Set(0, request);
            ECode ec = mFusedService->AddGeofences(req);
            _result = TRUE;
            //} catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "AddGeofence: RemoteException calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        default:
            _result = FALSE;
    }
    if (_result) {
        AutoPtr<IMessage> m;
        mReaperHandler->ObtainMessage(REAPER_GEOFENCE_ADDED, _callback, (IMessage**)&m);
        m->SetArg1(monitoringType);
        Boolean success;
        mReaperHandler->SendMessage(m, &success);
    }
    else {
        {    AutoLock syncLock(mGeofencesLock);
            mGeofences.Erase(geofenceId);
        }
    }

    if (DEBUG) {
        StringBuilder sb;
        sb += "addCircularFence: Result is: ";
        sb += _result;
        Logger::D(TAG, sb.ToString());
    }
    *result = _result;
    return NOERROR;
}

ECode GeofenceHardwareImpl::RemoveGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // This API is not thread safe. Operations on the same geofence need to be serialized
    // by upper layers
    if (DEBUG) {
        StringBuilder sb;
        sb += "Remove Geofence: GeofenceId: ";
        sb += geofenceId;
        Logger::D(TAG, sb.ToString());
    }

    Boolean _result = FALSE;

    {    AutoLock syncLock(mGeofencesLock);
        if (mGeofences[geofenceId] == NULL) {
            //throw new IllegalArgumentException("Geofence " + geofenceId + " not registered.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    switch (monitoringType) {
        case IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE:
        {
            if (mGpsService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            ECode ec = mGpsService->RemoveHardwareGeofence(geofenceId, &_result);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "RemoveGeofence: Remote Exception calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        case IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE:
        {
            if (mFusedService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            AutoPtr<ArrayOf<Int32> > arrays = ArrayOf<Int32>::Alloc(1);
            (*arrays)[0] = geofenceId;
            ECode ec = mFusedService->RemoveGeofences(arrays);
            _result = TRUE;
            //} catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "RemoveGeofence: RemoteException calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        default:
            _result = FALSE;
    }
    if (DEBUG) {
        StringBuilder sb;
        sb += "removeGeofence: Result is: ";
        sb += _result;
        Logger::D(TAG, sb.ToString());
    }
    *result = _result;
    return NOERROR;
}

ECode GeofenceHardwareImpl::PauseGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // This API is not thread safe. Operations on the same geofence need to be serialized
    // by upper layers
    if (DEBUG) {
        StringBuilder sb;
        sb += "Pause Geofence: GeofenceId: ";
        sb += geofenceId;
        Logger::D(TAG, sb.ToString());
    }
    Boolean _result;
    {    AutoLock syncLock(mGeofencesLock);
        if (mGeofences[geofenceId] == NULL) {
            //throw new IllegalArgumentException("Geofence " + geofenceId + " not registered.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    switch (monitoringType) {
        case IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE:
        {
            if (mGpsService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            ECode ec = mGpsService->PauseHardwareGeofence(geofenceId, &_result);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "PauseGeofence: Remote Exception calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        case IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE:
        {
            if (mFusedService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            ECode ec = mFusedService->PauseMonitoringGeofence(geofenceId);
            _result = TRUE;
            //} catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "PauseGeofence: RemoteException calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        default:
            _result = FALSE;
    }
    if (DEBUG) {
        StringBuilder sb;
        sb += "pauseGeofence: Result is: ";
        sb += _result;
        Logger::D(TAG, sb.ToString());
    }
    *result = _result;
    return NOERROR;
}

ECode GeofenceHardwareImpl::ResumeGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 monitorTransition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // This API is not thread safe. Operations on the same geofence need to be serialized
    // by upper layers
    if (DEBUG) {
        StringBuilder sb;
        sb += "Resume Geofence: GeofenceId: ";
        sb += geofenceId;
        Logger::D(TAG, sb.ToString());
    }
    Boolean _result;
    {    AutoLock syncLock(mGeofencesLock);
        if (mGeofences[geofenceId] == NULL) {
            // /throw new IllegalArgumentException("Geofence " + geofenceId + " not registered.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    switch (monitoringType) {
        case IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE:
        {
            if (mGpsService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            ECode ec = mGpsService->ResumeHardwareGeofence(geofenceId, monitorTransition, &_result);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "ResumeGeofence: Remote Exception calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        case IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE:
        {
            if (mFusedService == NULL) {
                *result = FALSE;
                return NOERROR;
            }
            //try {
            ECode ec = mFusedService->ResumeMonitoringGeofence(geofenceId, monitorTransition);
            _result = TRUE;
            //} catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "ResumeGeofence: RemoteException calling LocationManagerService");
                _result = FALSE;
            }
            //}
            break;
        }
        default:
            _result = FALSE;
    }
    if (DEBUG) {
        StringBuilder sb;
        sb += "resumeGeofence: Result is: ";
        sb += _result;
        Logger::D(TAG, sb.ToString());
    }
    *result = _result;
    return NOERROR;
}

ECode GeofenceHardwareImpl::RegisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMessage> reaperMessage;
    Boolean _result;
    mReaperHandler->ObtainMessage(REAPER_MONITOR_CALLBACK_ADDED,
            _callback, (IMessage**)&reaperMessage);
    reaperMessage->SetArg1(monitoringType);
    mReaperHandler->SendMessage(reaperMessage, &_result);

    AutoPtr<IMessage> m;
    mCallbacksHandler->ObtainMessage(CALLBACK_ADD, _callback, (IMessage**)&m);
    m->SetArg1(monitoringType);
    mCallbacksHandler->SendMessage(m, &_result);
    *result = TRUE;
    return NOERROR;
}

ECode GeofenceHardwareImpl::UnregisterForMonitorStateChangeCallback(
    /* [in] */ Int32 monitoringType,
    /* [in] */ IIGeofenceHardwareMonitorCallback* _callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMessage> m;
    mCallbacksHandler->ObtainMessage(CALLBACK_REMOVE, _callback, (IMessage**)&m);
    m->SetArg1(monitoringType);
    Boolean _result;
    mCallbacksHandler->SendMessage(m, &_result);
    *result = TRUE;
    return NOERROR;
}

ECode GeofenceHardwareImpl::ReportGeofenceTransition(
    /* [in] */ Int32 geofenceId,
    /* [in] */ ILocation* location,
    /* [in] */ Int32 transition,
    /* [in] */ Int64 transitionTimestamp,
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 sourcesUsed)
{
    if (location == NULL) {
        StringBuilder sb;
        sb += "Invalid Geofence Transition: location=";
        sb += location;
        Logger::E(TAG, sb.ToString());
        return NOERROR;
    }
    if (DEBUG) {
        StringBuilder sb;
        sb += "GeofenceTransition| ";
        sb += location;
        sb += ", transition:";
        sb += transition;
        sb += ", transitionTimestamp:";
        sb += transitionTimestamp;
        sb += ", monitoringType:";
        sb += monitoringType;
        sb += ", sourcesUsed:";
        sb += sourcesUsed;
        Logger::D(TAG, sb.ToString());
    }

    AutoPtr<GeofenceTransition> geofenceTransition = new GeofenceTransition(
            geofenceId,
            transition,
            transitionTimestamp,
            location,
            monitoringType,
            sourcesUsed);
    AcquireWakeLock();

    AutoPtr<IMessage> message;
    mGeofenceHandler->ObtainMessage(
            GEOFENCE_TRANSITION_CALLBACK,
            TO_IINTERFACE(geofenceTransition), (IMessage**)&message);
    message->SendToTarget();
    return NOERROR;
}

ECode GeofenceHardwareImpl::ReportGeofenceMonitorStatus(
    /* [in] */ Int32 monitoringType,
    /* [in] */ Int32 monitoringStatus,
    /* [in] */ ILocation* location,
    /* [in] */ Int32 source)
{
    SetMonitorAvailability(monitoringType, monitoringStatus);
    AcquireWakeLock();
    AutoPtr<GeofenceHardwareMonitorEvent> event = new GeofenceHardwareMonitorEvent();
    FAIL_RETURN(event->constructor( monitoringType, monitoringStatus, source, location))
    AutoPtr<IMessage> message;
    mCallbacksHandler->ObtainMessage(GEOFENCE_STATUS, TO_IINTERFACE(event), (IMessage**)&message);
    message->SendToTarget();
    return NOERROR;
}

ECode GeofenceHardwareImpl::ReportGeofenceOperationStatus(
    /* [in] */ Int32 operation,
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 operationStatus)
{
    AcquireWakeLock();
    AutoPtr<IMessage> message;
    mGeofenceHandler->ObtainMessage(operation, (IMessage**)&message);
    message->SetArg1(geofenceId);
    message->SetArg2(operationStatus);
    message->SendToTarget();
    return NOERROR;
}

ECode GeofenceHardwareImpl::ReportGeofenceAddStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (DEBUG) {
        StringBuilder sb;
        sb += "AddCallback| id:";
        sb += geofenceId;
        sb += ", status:";
        sb += status;
        Logger::D(TAG, sb.ToString());
    }
    return ReportGeofenceOperationStatus(ADD_GEOFENCE_CALLBACK, geofenceId, status);
}

ECode GeofenceHardwareImpl::ReportGeofenceRemoveStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (DEBUG) {
        StringBuilder sb;
        sb += "RemoveCallback| id:";
        sb += geofenceId;
        sb += ", status:";
        sb += status;
        Logger::D(TAG, sb.ToString());
    }
    return ReportGeofenceOperationStatus(REMOVE_GEOFENCE_CALLBACK, geofenceId, status);
}

ECode GeofenceHardwareImpl::ReportGeofencePauseStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (DEBUG) {
        StringBuilder sb;
        sb += "PauseCallbac| id:";
        sb += geofenceId;
        sb += ", status:";
        sb += status;
        Logger::D(TAG, sb.ToString());
    }
    return ReportGeofenceOperationStatus(PAUSE_GEOFENCE_CALLBACK, geofenceId, status);
}

ECode GeofenceHardwareImpl::ReportGeofenceResumeStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (DEBUG) {
        StringBuilder sb;
        sb += "ResumeCallback| id:";
        sb += geofenceId;
        sb += ", status:";
        sb += status;
        Logger::D(TAG, sb.ToString());
    }
    return ReportGeofenceOperationStatus(RESUME_GEOFENCE_CALLBACK, geofenceId, status);
}

void GeofenceHardwareImpl::SetMonitorAvailability(
    /* [in] */ Int32 monitor,
    /* [in] */ Int32 val)
{
    {    AutoLock syncLock(mSupportedMonitorTypesLock);
        (*mSupportedMonitorTypes)[monitor] = val;
    }
    return;
}


ECode GeofenceHardwareImpl::GetMonitoringResolutionLevel(
    /* [in] */ Int32 monitoringType,
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    switch (monitoringType) {
        case IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE:
            *level = RESOLUTION_LEVEL_FINE;
            return NOERROR;
        case IGeofenceHardware::MONITORING_TYPE_FUSED_HARDWARE:
            *level = RESOLUTION_LEVEL_FINE;
            return NOERROR;
    }
    *level = RESOLUTION_LEVEL_NONE;
    return NOERROR;
}

ECode GeofenceHardwareImpl::GetAllowedResolutionLevel(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    Int32 result;
    FAIL_RETURN(mContext->CheckPermission(Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION,
            pid, uid, &result))
    if (result == IPackageManager::PERMISSION_GRANTED) {
        *level = RESOLUTION_LEVEL_FINE;
        return NOERROR;
    }
    else {
        FAIL_RETURN(mContext->CheckPermission(Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION,
            pid, uid, &result))
        if (result == IPackageManager::PERMISSION_GRANTED) {
            *level = RESOLUTION_LEVEL_COARSE;
            return NOERROR;
        }
        else {
            *level = RESOLUTION_LEVEL_NONE;
            return NOERROR;
        }
    }
    return NOERROR;
}

} //Location
} //Hardware
} //Droid
} //Elastos