
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/CameraInfo.h"
#include "elastos/droid/hardware/camera2/CameraManager.h"
#include "elastos/droid/hardware/camera2/CCameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceUserShim.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImpl.h"
#include "elastos/droid/hardware/camera2/utils/BinderHolder.h"
#include "elastos/droid/hardware/camera2/utils/CameraServiceBinderDecorator.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::CCameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::Legacy::CameraDeviceUserShim;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyMetadataMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::ICameraDeviceUserShim;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraDeviceImpl;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImpl;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Droid::Hardware::Camera2::Utils::IBinderHolder;
using Elastos::Droid::Hardware::Camera2::Utils::BinderHolder;
using Elastos::Droid::Hardware::Camera2::Utils::ICameraBinderDecorator;
using Elastos::Droid::Hardware::Camera2::Utils::CameraServiceBinderDecorator;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CameraManager::AvailabilityCallback, Object, ICameraManagerAvailabilityCallback)

ECode CameraManager::AvailabilityCallback::OnCameraAvailable(
    /* [in] */ const String& cameraId)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraManager::AvailabilityCallback::OnCameraUnavailable(
    /* [in] */ const String& cameraId)
{
    // default empty implementation
    return NOERROR;
}

CameraManager::MyRunnableAvailable::MyRunnableAvailable(
    /* [in] */ ICameraManagerAvailabilityCallback* _callback,
    /* [in] */ const String& id)
    : mCallback(_callback)
    , mId(id)
{
}

ECode CameraManager::MyRunnableAvailable::MyRunnableAvailable::Run()
{
    return mCallback->OnCameraAvailable(mId);
}

CameraManager::MyRunnableUnavailable::MyRunnableUnavailable(
    /* [in] */ ICameraManagerAvailabilityCallback* _callback,
    /* [in] */ const String& id)
    : mCallback(_callback)
    , mId(id)
{
}

ECode CameraManager::MyRunnableUnavailable::MyRunnableUnavailable::Run()
{
    return mCallback->OnCameraUnavailable(mId);
}

CameraManager::CameraServiceDeathListener::CameraServiceDeathListener(
    /* [in] */ CameraManager* host)
    : mHost(host)
{
}

CAR_INTERFACE_IMPL(CameraManager::CameraServiceDeathListener, Object, IProxyDeathRecipient)

ECode CameraManager::CameraServiceDeathListener::ProxyDied()
{
    Object& lock = mHost->mLock;
    synchronized(lock) {
        mHost->mCameraService = NULL;
        // Tell listeners that the cameras are _available_, because any existing clients
        // will have gotten disconnected. This is optimistic under the assumption that the
        // service will be back shortly.
        //
        // Without this, a camera service crash while a camera is open will never signal to
        // listeners that previously in-use cameras are now available.
        Int32 size;
        mHost->mDeviceIdList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mDeviceIdList->Get(i, (IInterface**)&obj);
            AutoPtr<ICharSequence> charObj = ICharSequence::Probe(obj);
            String cameraId;
            charObj->ToString(&cameraId);

            mHost->mServiceListener->OnStatusChangedLocked(
                    CameraManager::CameraServiceListener::STATUS_PRESENT, cameraId);
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CameraManager::CameraServiceListener, Object, IICameraServiceListener, IBinder)

const Int32 CameraManager::CameraServiceListener::STATUS_NOT_PRESENT = 0;
const Int32 CameraManager::CameraServiceListener::STATUS_PRESENT = 1;
const Int32 CameraManager::CameraServiceListener::STATUS_ENUMERATING = 2;
const Int32 CameraManager::CameraServiceListener::STATUS_NOT_AVAILABLE = 0x80000000;

const String CameraManager::CameraServiceListener::TAG("CameraServiceListener");

CameraManager::CameraServiceListener::CameraServiceListener(
    /* [in] */ CameraManager* host)
    : mHost(host)
{
    CArrayMap::New((IArrayMap**)&mDeviceStatus);
}

ECode CameraManager::CameraServiceListener::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = IBinder::Probe(this);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

Boolean CameraManager::CameraServiceListener::IsAvailable(
    /* [in] */ Int32 status)
{
    switch (status) {
        case STATUS_PRESENT:
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

Boolean CameraManager::CameraServiceListener::ValidStatus(
    /* [in] */ Int32 status)
{
    switch (status) {
        case STATUS_NOT_PRESENT:
        case STATUS_PRESENT:
        case STATUS_ENUMERATING:
        case STATUS_NOT_AVAILABLE:
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

ECode CameraManager::CameraServiceListener::PostSingleUpdate(
    /* [in] */ ICameraManagerAvailabilityCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& id,
    /* [in] */ Int32 status)
{
    if (IsAvailable(status)) {
        AutoPtr<MyRunnableAvailable> myRun = new MyRunnableAvailable(_callback, id);
        Boolean result;
        handler->Post(IRunnable::Probe(myRun), &result);
    }
    else {
        AutoPtr<MyRunnableUnavailable> myRun = new MyRunnableUnavailable(_callback, id);
        Boolean result;
        handler->Post(IRunnable::Probe(myRun), &result);
    }
    return NOERROR;
}

ECode CameraManager::CameraServiceListener::UpdateCallbackLocked(
    /* [in] */ ICameraManagerAvailabilityCallback* _callback,
    /* [in] */ IHandler* handler)
{
    Int32 size;
    mDeviceStatus->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> keyObj;
        mDeviceStatus->GetKeyAt(i, (IInterface**)&keyObj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(keyObj);
        String id;
        cchar->ToString(&id);

        AutoPtr<IInterface> valueObj;
        mDeviceStatus->GetValueAt(i, (IInterface**)&valueObj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(valueObj);
        Int32 status;
        intObj->GetValue(&status);
        PostSingleUpdate(_callback, handler, id, status);
    }
    return NOERROR;
}

ECode CameraManager::CameraServiceListener::OnStatusChanged(
    /* [in] */ Int32 status,
    /* [in] */ Int32 cameraId)
{
    Object& lock =  mHost->mLock;
    synchronized(lock) {
        return OnStatusChangedLocked(status, StringUtils::ToString(cameraId));
    }
    return NOERROR;
}

ECode CameraManager::CameraServiceListener::OnStatusChangedLocked(
    /* [in] */ Int32 status,
    /* [in] */ const String& id)
{
    if (mHost->DEBUG) {
        Slogger::V(TAG, "Camera id %s has status changed to 0x%x", id.string(), status);
    }

    if (!ValidStatus(status)) {
        Slogger::E(mHost->TAG, "Ignoring invalid device %s status 0x%x", id.string(), status);
        return NOERROR;
    }

    AutoPtr<ICharSequence> idChar = CoreUtils::Convert(id);
    AutoPtr<ICharSequence> statusChar = CoreUtils::Convert(StringUtils::ToString(status));
    AutoPtr<IInterface> oldStatus;
    mDeviceStatus->Put(TO_IINTERFACE(idChar), TO_IINTERFACE(statusChar), (IInterface**)&oldStatus);

    if (oldStatus != NULL){
        AutoPtr<IInteger32> obj = IInteger32::Probe(oldStatus);
        Int32 value;
        obj->GetValue(&value);
        if(value == status) {
            if (mHost->DEBUG) {
                Slogger::V(mHost->TAG, "Device status changed to 0x%x, which is what it already was",
                    status);
            }
            return NOERROR;
        }
    }

    // TODO: consider abstracting out this state minimization + transition
    // into a separate
    // more easily testable class
    // i.e. (new State()).addState(STATE_AVAILABLE)
    //                   .addState(STATE_NOT_AVAILABLE)
    //                   .addTransition(STATUS_PRESENT, STATE_AVAILABLE),
    //                   .addTransition(STATUS_NOT_PRESENT, STATE_NOT_AVAILABLE)
    //                   .addTransition(STATUS_ENUMERATING, STATE_NOT_AVAILABLE);
    //                   .addTransition(STATUS_NOT_AVAILABLE, STATE_NOT_AVAILABLE);

    // Translate all the statuses to either 'available' or 'not available'
    //  available -> available         => no new update
    //  not available -> not available => no new update
    if (oldStatus != NULL) {
        AutoPtr<IInteger32> obj = IInteger32::Probe(oldStatus);
        Int32 value;
        obj->GetValue(&value);
        if (IsAvailable(status) == IsAvailable(value)) {
            if (mHost->DEBUG) {
                Slogger::V(mHost->TAG,
                        "Device status was previously available (%d), "
                        " and is now again available (%d)"
                        "so no new client visible update will be sent",
                        IsAvailable(status), IsAvailable(status));
            }
            return NOERROR;
        }
    }

    Int32 callbackCount;
    mHost->mCallbackMap->GetSize(&callbackCount);
    for (Int32 i = 0; i < callbackCount; i++) {
        AutoPtr<IInterface> valueObj;
        mHost->mCallbackMap->GetValueAt(i, (IInterface**)&valueObj);
        AutoPtr<IHandler> handler = IHandler::Probe(valueObj);

        AutoPtr<IInterface> keyObj;
        mHost->mCallbackMap->GetKeyAt(i, (IInterface**)&keyObj);
        AutoPtr<ICameraManagerAvailabilityCallback> _callback =
                ICameraManagerAvailabilityCallback::Probe(keyObj);

        PostSingleUpdate(_callback, handler, id, status);
    }
    return NOERROR;
}

ECode CameraManager::CameraServiceListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

const String CameraManager::TAG("CameraManager");

const String CameraManager::CAMERA_SERVICE_BINDER_NAME("media.camera");
const Int32 CameraManager::USE_CALLING_UID = -1;

//@SuppressWarnings("unused")
const Int32 CameraManager::API_VERSION_1 = 1;
const Int32 CameraManager::API_VERSION_2 = 2;

CAR_INTERFACE_IMPL(CameraManager, Object, ICameraManager)

CameraManager::CameraManager()
    : DEBUG(FALSE) //Log.isLoggable(TAG, Log.DEBUG);
{
    CArrayMap::New((IArrayMap**)&mCallbackMap);
    mServiceListener = new CameraServiceListener(this);
}

ECode CameraManager::constructor()
{
    return NOERROR;
}

ECode CameraManager::constructor(
    /* [in] */ IContext* context)
{
    synchronized(mLock) {
        mContext = context;

        ConnectCameraServiceLocked();
    }
    return NOERROR;
}

ECode CameraManager::GetCameraIdList(
    /* [out, callee] */ ArrayOf<String>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    synchronized (mLock) {
        // ID list creation handles various known failures in device enumeration, so only
        // exceptions it'll throw are unexpected, and should be propagated upward.
        AutoPtr<IArrayList> list;
        GetOrCreateDeviceIdListLocked((IArrayList**)&list);
        AutoPtr<ArrayOf<IInterface*> > array;
        list->ToArray((ArrayOf<IInterface*>**)&array);
        AutoPtr<ArrayOf<String> > outArray = ArrayOf<String>::Alloc(array->GetLength());
        for (Int32 i = 0; i < array->GetLength(); i ++) {
            AutoPtr<ICharSequence> cchar = ICharSequence::Probe((*array)[i]);
            String str;
            cchar->ToString(&str);
            outArray->Set(i, str);
        }
        *outarr = outArray;
        REFCOUNT_ADD(*outarr );
        return NOERROR;
    }
    return NOERROR;
}

ECode CameraManager::RegisterAvailabilityCallback(
    /* [in] */ ICameraManagerAvailabilityCallback* ccallback,
    /* [in] */ IHandler* handler)
{
    if (handler == NULL) {
        AutoPtr<ILooper> looper = Looper::GetMyLooper();
        if (looper == NULL) {
            // throw new IllegalArgumentException(
            //         "No handler given, and current thread has no looper!");
            Slogger::E(TAG, "No handler given, and current thread has no looper!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CHandler::New(looper, (IHandler**)&handler);
    }

    synchronized (mLock) {
        AutoPtr<IInterface> obj;
        mCallbackMap->Put(TO_IINTERFACE(ccallback), TO_IINTERFACE(handler), (IInterface**)&obj);
        AutoPtr<IHandler> oldHandler = IHandler::Probe(obj);
        // For new callbacks, provide initial availability information
        if (oldHandler == NULL) {
            mServiceListener->UpdateCallbackLocked(ccallback, handler);
        }
    }
    return NOERROR;
}

ECode CameraManager::UnregisterAvailabilityCallback(
    /* [in] */ ICameraManagerAvailabilityCallback* ccallback)
{
    synchronized (mLock) {
        mCallbackMap->Remove(TO_IINTERFACE(ccallback));
    }
    return NOERROR;
}

ECode CameraManager::GetCameraCharacteristics(
    /* [in] */ const String& cameraId,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc);
    *outcc = NULL;

    AutoPtr<ICameraCharacteristics> characteristics;

    synchronized (mLock) {
        AutoPtr<IArrayList> list;
        GetOrCreateDeviceIdListLocked((IArrayList**)&list);
        Boolean result;
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(cameraId);
        list->Contains(TO_IINTERFACE(cchar), &result);
        if (!result) {
            // throw new IllegalArgumentException(String.format("Camera id %s does not match any" +
            //         " currently connected camera device", cameraId));
            Slogger::E(TAG, "Camera id %s does not match any"
                    " currently connected camera device", cameraId.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int32 id = StringUtils::ParseInt32(cameraId);

        /*
         * Get the camera characteristics from the camera service directly if it supports it,
         * otherwise get them from the legacy shim instead.
         */

        AutoPtr<IICameraService> cameraService;
        GetCameraServiceLocked((IICameraService**)&cameraService);
        if (cameraService == NULL) {
            // throw new CameraAccessException(CameraAccessException.CAMERA_DISCONNECTED,
            //         "Camera service is currently unavailable");
            Slogger::E(TAG, "Camera service is currently unavailable");
            return E_CAMERA_ACCESS_EXCEPTION;
        }
        //try {
        ECode ec = SupportsCamera2ApiLocked(cameraId, &result);
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            return ec;
        }
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // Camera service died - act as if the camera was disconnected
            // throw new CameraAccessException(CameraAccessException.CAMERA_DISCONNECTED,
            //         "Camera service is currently unavailable", e);
            Slogger::E(TAG, "Camera service is currently unavailable");
            return E_CAMERA_ACCESS_EXCEPTION;
        }
        if (!result) {
            // Legacy backwards compatibility path; build static info from the camera
            // parameters
            AutoPtr<ArrayOf<String> > outParameters = ArrayOf<String>::Alloc(1);

            Int32 tmp;
            cameraService->GetLegacyParameters(id, /*out*/outParameters, &tmp);
            String parameters = (*outParameters)[0];

            AutoPtr<ICameraInfo> info = new CameraInfo();
            cameraService->GetCameraInfo(id, /*out*/info, &tmp);

            LegacyMetadataMapper::CreateCharacteristics(parameters, info,
                    (ICameraCharacteristics**)&characteristics);
        }
        else {
            // Normal path: Get the camera characteristics directly from the camera service
            AutoPtr<ICameraMetadataNative> info;
            CCameraMetadataNative::New((ICameraMetadataNative**)&info);

            Int32 tmp;
            cameraService->GetCameraCharacteristics(id, info, &tmp);

            CCameraCharacteristics::New(info, (ICameraCharacteristics**)&characteristics);
        }
        //} catch (CameraRuntimeException e) {
        // if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
        //     return ec;
        // }
        // //} catch (RemoteException e) {
        // if (ec == (ECode)E_REMOTE_EXCEPTION) {
        //     // Camera service died - act as if the camera was disconnected
        //     // throw new CameraAccessException(CameraAccessException.CAMERA_DISCONNECTED,
        //     //         "Camera service is currently unavailable", e);
        //     Slogger::E(TAG, "Camera service is currently unavailable");
        //     return E_CAMERA_ACCESS_EXCEPTION;
        // }
        //}
    }
    *outcc = characteristics;
    REFCOUNT_ADD(*outcc);
    return NOERROR;
}

ECode CameraManager::OpenCameraDeviceUserAsync(
    /* [in] */ const String& cameraId,
    /* [in] */ ICameraDeviceStateCallback* ccallback,
    /* [in] */ IHandler* handler,
    /* [out] */ ICameraDevice** _device)
{
    VALIDATE_NOT_NULL(_device);
    *_device = NULL;

    AutoPtr<ICameraCharacteristics> characteristics;
    GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&characteristics);
    AutoPtr<ICameraDevice> device;
    //try {

    synchronized (mLock) {

        AutoPtr<IICameraDeviceUser> cameraUser;
        AutoPtr<ICameraDeviceImpl> deviceImpl;
        CCameraDeviceImpl::New(cameraId, ccallback, handler, characteristics, (ICameraDeviceImpl**)&deviceImpl);

        AutoPtr<IBinderHolder> holder = new BinderHolder();

        AutoPtr<ICameraDeviceImplCameraDeviceCallbacks> _callbacks;
        deviceImpl->GetCallbacks((ICameraDeviceImplCameraDeviceCallbacks**)&_callbacks);
        AutoPtr<IICameraDeviceCallbacks> callbacks = IICameraDeviceCallbacks::Probe(_callbacks);
        Int32 id = StringUtils::ParseInt32(cameraId);

        //try {
        Boolean result;
        SupportsCamera2ApiLocked(cameraId, &result);
        if (result) {
            // Use cameraservice's cameradeviceclient implementation for HAL3.2+ devices
            AutoPtr<IICameraService> cameraService;
            GetCameraServiceLocked((IICameraService**)&cameraService);
            if (cameraService == NULL) {
                // throw new CameraRuntimeException(
                //     CameraAccessException.CAMERA_DISCONNECTED,
                //     "Camera service is currently unavailable");
                Slogger::E(TAG, "Camera service is currently unavailable");
                return E_CAMERA_RUNTIME_EXCEPTION;
            }
            String name;
            mContext->GetPackageName(&name);
            Int32 tmp;
            cameraService->ConnectDevice(callbacks, id,
                    name, USE_CALLING_UID, holder, &tmp);
            AutoPtr<IBinder> binder;
            holder->GetBinder((IBinder**)&binder);
            cameraUser = IICameraDeviceUser::Probe(binder);
        }
        else {
            // Use legacy camera implementation for HAL1 devices
            Slogger::I(TAG, "Using legacy camera HAL.");
            AutoPtr<ICameraDeviceUserShim> shim;
            CameraDeviceUserShim::ConnectBinderShim(callbacks, id,
                    (ICameraDeviceUserShim**)&shim);
            cameraUser = IICameraDeviceUser::Probe(shim);
        }
        assert(0);
        // } catch (CameraRuntimeException e) {
        //     if (e.getReason() == CameraAccessException.CAMERA_DEPRECATED_HAL) {
        //         throw new AssertionError("Should've gone down the shim path");
        //     }
        //     else if (e.getReason() == CameraAccessException.CAMERA_IN_USE ||
        //             e.getReason() == CameraAccessException.MAX_CAMERAS_IN_USE ||
        //             e.getReason() == CameraAccessException.CAMERA_DISABLED ||
        //             e.getReason() == CameraAccessException.CAMERA_DISCONNECTED ||
        //             e.getReason() == CameraAccessException.CAMERA_ERROR) {
        //         // Received one of the known connection errors
        //         // The remote camera device cannot be connected to, so
        //         // set the local camera to the startup error state
        //         deviceImpl.setRemoteFailure(e);

        //         if (e.getReason() == CameraAccessException.CAMERA_DISABLED ||
        //                 e.getReason() == CameraAccessException.CAMERA_DISCONNECTED) {
        //             // Per API docs, these failures call onError and throw
        //             throw e.asChecked();
        //         }
        //     }
        //     else {
        //         // Unexpected failure - rethrow
        //         throw e;
        //     }
        // } catch (RemoteException e) {
        //     // Camera service died - act as if it's a CAMERA_DISCONNECTED case
        //     CameraRuntimeException ce = new CameraRuntimeException(
        //         CameraAccessException.CAMERA_DISCONNECTED,
        //         "Camera service is currently unavailable", e);
        //     deviceImpl.setRemoteFailure(ce);
        //     throw ce.asChecked();
        // }

        // TODO: factor out callback to be non-nested, then move setter to constructor
        // For now, calling setRemoteDevice will fire initial
        // onOpened/onUnconfigured callbacks.
        deviceImpl->SetRemoteDevice(cameraUser);
        device = ICameraDevice::Probe(deviceImpl);
    }

    assert(0);
    // } catch (NumberFormatException e) {
    //     throw new IllegalArgumentException("Expected cameraId to be numeric, but it was: "
    //             + cameraId);
    // } catch (CameraRuntimeException e) {
    //     throw e.asChecked();
    // }
    *_device = device;
    REFCOUNT_ADD(*_device);
    return NOERROR;
}

ECode CameraManager::OpenCamera(
    /* [in] */ const String& cameraId,
    /* [in] */ ICameraDeviceStateCallback* ccallback,
    /* [in] */ IHandler* handler)
{
    if (cameraId.IsNull()) {
        //throw new IllegalArgumentException("cameraId was null");
        Slogger::E(TAG, "cameraId was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (ccallback == NULL) {
        //throw new IllegalArgumentException("callback was null");
        Slogger::E(TAG, "callback was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (handler == NULL) {
        AutoPtr<ILooper> looper = Looper::GetMyLooper();
        if (looper != NULL) {
            CHandler::New((IHandler**)&handler);
        }
        else {
            // throw new IllegalArgumentException(
            //         "Looper doesn't exist in the calling thread");
            Slogger::E(TAG, "Looper doesn't exist in the calling thread");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    AutoPtr<ICameraDevice> device;
    return OpenCameraDeviceUserAsync(cameraId, ccallback, handler, (ICameraDevice**)&device);
}

ECode CameraManager::GetOrCreateDeviceIdListLocked(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    if (mDeviceIdList == NULL) {
        Int32 numCameras = 0;
        AutoPtr<IICameraService> cameraService;
        GetCameraServiceLocked((IICameraService**)&cameraService);
        AutoPtr<IArrayList> deviceIdList;
        CArrayList::New((IArrayList**)&deviceIdList);

        // If no camera service, then no devices
        if (cameraService == NULL) {
            *list = deviceIdList;
            REFCOUNT_ADD(*list);
            return NOERROR;
        }

        //try {
        ECode ec = cameraService->GetNumberOfCameras(&numCameras);
        //} catch(CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            return ec;
        }
        //} catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // camera service just died - if no camera service, then no devices
            *list = deviceIdList;
            REFCOUNT_ADD(*list);
            return NOERROR;
        }
        //}

        AutoPtr<ICameraMetadataNative> info;
        CCameraMetadataNative::New((ICameraMetadataNative**)&info);
        for (Int32 i = 0; i < numCameras; ++i) {
            // Non-removable cameras use integers starting at 0 for their
            // identifiers
            Boolean isDeviceSupported = FALSE;
            //try {
            Int32 tmp;
            ECode ec = cameraService->GetCameraCharacteristics(i, info, &tmp);
            Boolean res;
            info->IsEmpty(&res);
            if (!res) {
                isDeviceSupported = TRUE;
            }
            else {
                //throw new AssertionError("Expected to get non-empty characteristics");
                Slogger::E(TAG, "Expected to get non-empty characteristics");
                return E_ASSERTION_ERROR;
            }
            //} catch(IllegalArgumentException  e) {
            if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                // Got a BAD_VALUE from service, meaning that this
                // device is not supported.
            }
            //} catch(CameraRuntimeException e) {
            if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
                // DISCONNECTED means that the HAL reported an low-level error getting the
                // device info; skip listing the device.  Other errors,
                // propagate exception onward
                assert(0);
                // if (e.getReason() != CameraAccessException.CAMERA_DISCONNECTED) {
                //     throw e.asChecked();
                // }
            }
            //} catch(RemoteException e) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                // Camera service died - no devices to list
                deviceIdList->Clear();
                *list = deviceIdList;
                REFCOUNT_ADD(*list);
                return NOERROR;
            }
            //}

            if (isDeviceSupported) {
                String str = StringUtils::ToString(i);
                AutoPtr<ICharSequence> ccahr = CoreUtils::Convert(str);
                deviceIdList->Add(TO_IINTERFACE(ccahr));
            }
            else {
                Slogger::W(TAG, "Error querying camera device %d for listing.", i);
            }

        }
        mDeviceIdList = deviceIdList;
    }
    *list = mDeviceIdList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CameraManager::HandleRecoverableSetupErrors(
    /* [in] */ Int32 e,
    /* [in] */ const String& msg)
{
    Int32 problem = e;
    switch (problem) {
        case ICameraAccessException::CAMERA_DISCONNECTED:
            // String errorMsg = CameraAccessException.getDefaultMessage(problem);
            // Log.w(TAG, msg + ": " + errorMsg);
            break;
        default:
            //throw new IllegalStateException(msg, e.asChecked());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CameraManager::SupportsCamera2ApiLocked(
    /* [in] */ const String& cameraId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return SupportsCameraApiLocked(cameraId, API_VERSION_2, result);
}

ECode CameraManager::SupportsCameraApiLocked(
    /* [in] */ const String& cameraId,
    /* [in] */ Int32 apiVersion,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 id = StringUtils::ParseInt32(cameraId);

    /*
     * Possible return values:
     * - NO_ERROR => CameraX API is supported
     * - CAMERA_DEPRECATED_HAL => CameraX API is *not* supported (thrown as an exception)
     * - Remote exception => If the camera service died
     *
     * Anything else is an unexpected error we don't want to recover from.
     */
    //try {
    AutoPtr<IICameraService> cameraService;
    ECode ec = GetCameraServiceLocked((IICameraService**)&cameraService);
    if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
        assert(0);
        // if (e.getReason() != CameraAccessException.CAMERA_DEPRECATED_HAL) {
        //     throw e;
        // }
        // API level is not supported
    }
    // If no camera service, no support
    if (cameraService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 res;
    cameraService->SupportsCameraApi(id, apiVersion, &res);

    if (res != ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR) {
        //throw new AssertionError("Unexpected value " + res);
        Slogger::E(TAG, "Unexpected value %d", res);
        return E_ASSERTION_ERROR;
    }
    *result = TRUE;
    return NOERROR;
    //} catch (CameraRuntimeException e) {
    // if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
    //     // if (e.getReason() != CameraAccessException.CAMERA_DEPRECATED_HAL) {
    //     //     throw e;
    //     // }
    //     // API level is not supported
    // }
    //} catch (RemoteException e) {
        // Camera service is now down, no support for any API level
    //}
}

ECode CameraManager::ConnectCameraServiceLocked()
{
    mCameraService = NULL;
    AutoPtr<IInterface> service = ServiceManager::GetService(CAMERA_SERVICE_BINDER_NAME);
    AutoPtr<IBinder> cameraServiceBinder = IBinder::Probe(service);

    if (cameraServiceBinder == NULL) {
        // Camera service is now down, leave mCameraService as null
        return NOERROR;
    }
    //try {
    AutoPtr<CameraServiceDeathListener> listener = new CameraServiceDeathListener(this);
    AutoPtr<IProxy> proxy = (IProxy*)cameraServiceBinder->Probe(EIID_IProxy);
    ECode ec = proxy->LinkToDeath(listener, /*flags*/ 0);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // Camera service is now down, leave mCameraService as null
        return NOERROR;
    }
    //}

    AutoPtr<IICameraService> cameraServiceRaw =IICameraService::Probe(cameraServiceBinder);

    /**
     * Wrap the camera service in a decorator which automatically translates return codes
     * into exceptions.
     */
    AutoPtr<IInterface> obj;
    CameraServiceBinderDecorator::NewInstance(TO_IINTERFACE(cameraServiceRaw), (IInterface**)&obj);
    AutoPtr<IICameraService> cameraService = IICameraService::Probe(obj);

    assert(0);
    // try {
    //     CameraServiceBinderDecorator.throwOnError(
    //             CameraMetadataNative.nativeSetupGlobalVendorTagDescriptor());
    // } catch (CameraRuntimeException e) {
    //     handleRecoverableSetupErrors(e, "Failed to set up vendor tags");
    // }

    //try {
    Int32 result;
    ec = cameraService->AddListener(IICameraServiceListener::Probe(mServiceListener), &result);
    mCameraService = cameraService;
    //} catch(CameraRuntimeException e) {
    if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
        // Unexpected failure
        // throw new IllegalStateException("Failed to register a camera service listener",
        //         e.asChecked());
        Slogger::E(TAG, "Failed to register a camera service listener");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //} catch (RemoteException e) {
        // Camera service is now down, leave mCameraService as null
    //}
    return NOERROR;
}

ECode CameraManager::GetCameraServiceLocked(
    /* [out] */ IICameraService** service)
{
    VALIDATE_NOT_NULL(service);
    *service = NULL;

    if (mCameraService == NULL) {
        Slogger::I(TAG, "getCameraServiceLocked: Reconnecting to camera service");
        FAIL_RETURN(ConnectCameraServiceLocked())
        if (mCameraService == NULL) {
            Slogger::E(TAG, "Camera service is unavailable");
        }
    }
    *service = mCameraService;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos