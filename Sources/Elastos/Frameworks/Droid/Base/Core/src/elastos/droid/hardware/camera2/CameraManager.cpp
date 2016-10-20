
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/CameraInfo.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/hardware/camera2/CameraManager.h"
#include "elastos/droid/hardware/camera2/CCameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceUserShim.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImpl.h"
#include "elastos/droid/hardware/camera2/utils/BinderHolder.h"
#include "elastos/droid/hardware/camera2/utils/CameraServiceBinderDecorator.h"
#include "elastos/droid/text/TextUtils.h"
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
#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#include <camera/Camera.h>
#include <camera/CameraMetadata.h>

using Elastos::Droid::Hardware::CameraInfo;
using Elastos::Droid::Hardware::HardwareCamera;
using Elastos::Droid::Hardware::Camera2::CCameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::Legacy::CameraDeviceUserShim;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyMetadataMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::ICameraDeviceUserShim;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
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
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CameraManager::CameraServiceDeathRecipient::CameraServiceDeathRecipient(
    /* [in] */ CameraManager* host)
    : mHost(host)
{
}

void CameraManager::CameraServiceDeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    AutoLock lock(mHost->mLock);

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
        String cameraId = TO_STR(obj);
        mHost->mServiceListener->OnStatusChangedLocked(
                CameraManager::CameraServiceListener::STATUS_PRESENT, cameraId);
    }
}

CAR_INTERFACE_IMPL_2(CameraManager::CameraServiceWrapper, Object, IBinder, IICameraService)

CameraManager::CameraServiceWrapper::~CameraServiceWrapper()
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> binder = sm->getService(android::String16(CAMERA_SERVICE_BINDER_NAME));
    if (binder != NULL) {
        binder->unlinkToDeath(mDeathRecipient);
    }
}

ECode CameraManager::CameraServiceWrapper::Init(
    /* [in] */ android::sp<android::ICameraService>& service,
    /* [in] */ CameraManager* host)
{
    mDeathRecipient = new CameraServiceDeathRecipient(host);
    mCameraService = service;
    mCameraService->asBinder()->linkToDeath(mDeathRecipient, NULL, 0);
    return NOERROR;
}

ECode CameraManager::CameraServiceWrapper::GetNumberOfCameras(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    *num = 0;

    if (mCameraService.get() != NULL) {
        *num = mCameraService->getNumberOfCameras();
        return NOERROR;
    }

    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::GetCameraInfo(
    /* [in] */ Int32 cameraId,
    /* [in] */ ICameraInfo* info,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = android::BAD_VALUE;

    if (mCameraService.get() != NULL) {
        android::CameraInfo cameraInfo;
        *result = mCameraService->getCameraInfo(cameraId, &cameraInfo);
        if (*result == android::OK) {
            AutoPtr<IHardwareCameraInfo> hci = new HardwareCamera::CameraInfo();
            hci->SetFacing(cameraInfo.facing);
            hci->SetOrientation(cameraInfo.orientation);
            info->SetInfo(hci);
            return NOERROR;
        }
    }

    Logger::E("CameraManager", "Failed to GetCameraInfo for cameraId: %d", cameraId);
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::Connect(
    /* [in] */ IICameraClient* client,
    /* [in] */ Int32 cameraId,
    /* [in] */ const String& clientPackageName,
    /* [in] */ Int32 clientUid,
    // Container for an ICamera object
    /* [in] */ IBinderHolder* device,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::ConnectPro(
    /* [in] */ IIProCameraCallbacks* _callbacks,
    /* [in] */ Int32 cameraId,
    /* [in] */ const String& clientPackageName,
    /* [in] */ Int32 clientUid,
    // Container for an IProCameraUser object
    /* [in] */ IBinderHolder* device,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::ConnectDevice(
    /* [in] */ IICameraDeviceCallbacks* _callbacks,
    /* [in] */ Int32 cameraId,
    /* [in] */ const String& clientPackageName,
    /* [in] */ Int32 clientUid,
    // Container for an ICameraDeviceUser object
    /* [in] */ IBinderHolder* device,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::AddListener(
    /* [in] */ IICameraServiceListener* listener,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::RemoveListener(
    /* [in] */ IICameraServiceListener* listener,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::GetCameraCharacteristics(
    /* [in] */ Int32 cameraId,
    /* [in] */ ICameraMetadataNative* info,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = android::BAD_VALUE;

    if (mCameraService.get() != NULL) {
        CameraMetadataNative* wrapper = (CameraMetadataNative*)info;
        android::CameraMetadata* data = wrapper->GetNative();
        *result = mCameraService->getCameraCharacteristics(cameraId, data);
        if (*result == android::OK) {
            return NOERROR;
        }
    }

    Logger::E("CameraManager", "Failed to GetCameraCharacteristics for cameraId: %d", cameraId);
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::GetCameraVendorTagDescriptor(
    /* [in] */ IBinderHolder* desc,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

// Writes the camera1 parameters into a single-element array.
ECode CameraManager::CameraServiceWrapper::GetLegacyParameters(
    /* [in] */ Int32 cameraId,
    /* [out, callee] */ ArrayOf<String>** parameters,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(parameters)
    VALIDATE_NOT_NULL(result)
    *result = android::BAD_VALUE;
    *parameters = NULL;

    if (mCameraService.get() != NULL) {
        android::String16 str16;
        *result = mCameraService->getLegacyParameters(cameraId, &str16);
        if (*result == android::OK) {
            android::String8 str8(str16);
            String str(str8.string());
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            array->Set(0, str);
            *parameters = array;
            REFCOUNT_ADD(*parameters)
            Logger::I(TAG, " >> GetLegacyParameters for Camera %d: \n%s", cameraId, str.string());
            return NOERROR;
        }
    }

    Logger::E("CameraManager", "Failed to GetLegacyParameters for cameraId: %d", cameraId);
    return E_CAMERA_RUNTIME_EXCEPTION;
}

// Determines if a particular API version is supported; see ICameraService.h for version defines
ECode CameraManager::CameraServiceWrapper::SupportsCameraApi(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 apiVersion,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = android::BAD_VALUE;

    if (mCameraService.get() != NULL) {
        *result = mCameraService->supportsCameraApi(cameraId, apiVersion);
        Logger::I(TAG, " >> camera %d SupportsCameraApi %d, result: %d", cameraId, apiVersion, *result);
        return NOERROR;
    }

    Logger::E("CameraManager", "Failed to SupportsCameraApi %d for cameraId: %d", apiVersion, cameraId);
    return E_CAMERA_RUNTIME_EXCEPTION;
}

ECode CameraManager::CameraServiceWrapper::ConnectLegacy(
    /* [in] */ IICameraClient* client,
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion,
    /* [in] */ const String& clientPackageName,
    /* [in] */ Int32 clientUid,
    // Container for an ICamera object
    /* [in] */ IBinderHolder* device,
    /* [out] */ Int32* result)
{
    Logger::I("CameraManager", " TODO: CameraServiceWrapper: %d", __LINE__); assert(0 && "TODO");
    return E_CAMERA_RUNTIME_EXCEPTION;
}

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

CAR_INTERFACE_IMPL_2(CameraManager::CameraServiceListener, Object, IICameraServiceListener, IBinder)

const Int32 CameraManager::CameraServiceListener::STATUS_NOT_PRESENT = 0;
const Int32 CameraManager::CameraServiceListener::STATUS_PRESENT = 1;
const Int32 CameraManager::CameraServiceListener::STATUS_ENUMERATING = 2;
const Int32 CameraManager::CameraServiceListener::STATUS_NOT_AVAILABLE = 0x80000000;

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

    *binder = this;
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
        String id = TO_STR(keyObj);

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
    AutoLock syncLock(mHost->mLock);
    return OnStatusChangedLocked(status, StringUtils::ToString(cameraId));
}

ECode CameraManager::CameraServiceListener::OnStatusChangedLocked(
    /* [in] */ Int32 status,
    /* [in] */ const String& id)
{
    if (mHost->DEBUG) {
        Logger::V(TAG, "Camera id %s has status changed to 0x%x", id.string(), status);
    }

    if (!ValidStatus(status)) {
        Logger::E(TAG, "Ignoring invalid device %s status 0x%x", id.string(), status);
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
                Logger::V(TAG, "Device status changed to 0x%x, which is what it already was",
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
                Logger::V(TAG,
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
const Boolean CameraManager::DEBUG = TRUE;

const String CameraManager::CAMERA_SERVICE_BINDER_NAME("media.camera");
const Int32 CameraManager::USE_CALLING_UID = -1;

//@SuppressWarnings("unused")
const Int32 CameraManager::API_VERSION_1 = 1;
const Int32 CameraManager::API_VERSION_2 = 2;

CAR_INTERFACE_IMPL(CameraManager, Object, ICameraManager)

CameraManager::CameraManager()
{
}

ECode CameraManager::constructor(
    /* [in] */ IContext* context)
{
    CArrayMap::New((IArrayMap**)&mCallbackMap);
    mServiceListener = new CameraServiceListener(this);

    {
        AutoLock syncLock(mLock);
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

    {
        AutoLock syncLock(mLock);
        // ID list creation handles various known failures in device enumeration, so only
        // exceptions it'll throw are unexpected, and should be propagated upward.
        AutoPtr<IArrayList> list;
        GetOrCreateDeviceIdListLocked((IArrayList**)&list);

        AutoPtr<ArrayOf<String> > outArray;
        if (list != NULL) {
            Int32 size;
            list->GetSize(&size);
            outArray = ArrayOf<String>::Alloc(size);

            for (Int32 i = 0; i < size; i ++) {
                AutoPtr<IInterface> obj;
                list->Get(i, (IInterface**)&obj);
                outArray->Set(i, Object::ToString(obj));
            }
        }
        else {
            outArray = ArrayOf<String>::Alloc(0);
        }

        *outarr = outArray;
        REFCOUNT_ADD(*outarr);
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
            Logger::E(TAG, "No handler given, and current thread has no looper!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CHandler::New(looper, (IHandler**)&handler);
    }

    {
        AutoLock syncLock(mLock);
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
    {
        AutoLock syncLock(mLock);
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

    {
        AutoLock syncLock(mLock);
        AutoPtr<IArrayList> list;
        GetOrCreateDeviceIdListLocked((IArrayList**)&list);
        Boolean result;
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(cameraId);
        list->Contains(TO_IINTERFACE(cchar), &result);
        if (!result) {
            Logger::E(TAG, "Camera id %s does not match any currently connected camera device", cameraId.string());
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
            Logger::E(TAG, "Camera service is currently unavailable");
            return E_CAMERA_ACCESS_EXCEPTION;
        }

        ECode ec = SupportsCamera2ApiLocked(cameraId, &result);
        if (FAILED(ec)) {
            Logger::E(TAG, "Camera service is currently unavailable");
            return ec;
        }

        if (DEBUG) {
            Logger::I(TAG, "camera %s SupportsCamera2Api %d", cameraId.string(), result);
        }

        if (!result) {
            // Legacy backwards compatibility path; build static info from the camera
            // parameters
            AutoPtr<ArrayOf<String> > outParameters;

            Int32 tmp;
            cameraService->GetLegacyParameters(id, (ArrayOf<String>**)&outParameters, &tmp);
            String parameters = (*outParameters)[0];

            AutoPtr<ICameraInfo> info = new CameraInfo();
            cameraService->GetCameraInfo(id, /*out*/info, &tmp);

            // Logger::I(TAG, " >> parameters: [%s], CameraInfo: %s", parameters.string(), TO_CSTR(info));
            LegacyMetadataMapper::CreateCharacteristics(parameters, info, (ICameraCharacteristics**)&characteristics);
        }
        else {
            // Normal path: Get the camera characteristics directly from the camera service
            AutoPtr<ICameraMetadataNative> info;
            CCameraMetadataNative::New((ICameraMetadataNative**)&info);

            Int32 tmp;
            cameraService->GetCameraCharacteristics(id, info, &tmp);
            CCameraCharacteristics::New(info, (ICameraCharacteristics**)&characteristics);
        }
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

    Logger::I(TAG, " >> OpenCameraDeviceUserAsync: %s, callback: %s", cameraId.string(), TO_CSTR(ccallback));
    Logger::I(TAG, "%s, line: %d", __FUNCTION__, __LINE__);
    AutoPtr<ICameraCharacteristics> characteristics;
    GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&characteristics);
    AutoPtr<ICameraDevice> device;
    //try {

    Logger::I(TAG, "%s, line: %d", __FUNCTION__, __LINE__);
    {
        AutoLock syncLock(mLock);

        AutoPtr<IICameraDeviceUser> cameraUser;
        AutoPtr<ICameraDeviceImpl> deviceImpl;
        CCameraDeviceImpl::New(cameraId, ccallback, handler, characteristics, (ICameraDeviceImpl**)&deviceImpl);

        AutoPtr<ICameraDeviceImplCameraDeviceCallbacks> _callbacks;
        deviceImpl->GetCallbacks((ICameraDeviceImplCameraDeviceCallbacks**)&_callbacks);
        AutoPtr<IICameraDeviceCallbacks> callbacks = IICameraDeviceCallbacks::Probe(_callbacks);
        Int32 id = StringUtils::ParseInt32(cameraId);

        //try {
        Boolean result;
        SupportsCamera2ApiLocked(cameraId, &result);

        if (result) {
            Logger::I(TAG, "Use cameraservice's cameradeviceclient implementation for HAL3.2+ devices.");
            // Use cameraservice's cameradeviceclient implementation for HAL3.2+ devices
            AutoPtr<IICameraService> cameraService;
            GetCameraServiceLocked((IICameraService**)&cameraService);
            if (cameraService == NULL) {
                Logger::E(TAG, "Camera service is currently unavailable");
                return E_CAMERA_ACCESS_EXCEPTION;
            }
            String name;
            mContext->GetPackageName(&name);
            Int32 tmp;
            AutoPtr<IBinderHolder> holder = new BinderHolder();
            cameraService->ConnectDevice(callbacks, id, name, USE_CALLING_UID, holder, &tmp);
            AutoPtr<IBinder> binder;
            holder->GetBinder((IBinder**)&binder);
            cameraUser = IICameraDeviceUser::Probe(binder);
        }
        else {
            // Use legacy camera implementation for HAL1 devices
            Logger::I(TAG, "Using legacy camera HAL.");
            AutoPtr<ICameraDeviceUserShim> shim;
            CameraDeviceUserShim::ConnectBinderShim(callbacks, id, (ICameraDeviceUserShim**)&shim);
            cameraUser = IICameraDeviceUser::Probe(shim);
        }
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
    Logger::I(TAG, "%s, line: %d", __FUNCTION__, __LINE__);
        FAIL_RETURN(deviceImpl->SetRemoteDevice(cameraUser))
    Logger::I(TAG, "%s, line: %d", __FUNCTION__, __LINE__);
        device = ICameraDevice::Probe(deviceImpl);
    }

    // } catch (NumberFormatException e) {
    //     throw new IllegalArgumentException("Expected cameraId to be numeric, but it was: "
    //             + cameraId);
    // } catch (CameraRuntimeException e) {
    //     throw e.asChecked();
    // }
    *_device = device;
    REFCOUNT_ADD(*_device);

    Logger::I(TAG, " << OpenCameraDeviceUserAsync: %s, device: %s", cameraId.string(), TO_CSTR(device));
    return NOERROR;
}

ECode CameraManager::OpenCamera(
    /* [in] */ const String& cameraId,
    /* [in] */ ICameraDeviceStateCallback* ccallback,
    /* [in] */ IHandler* handler)
{
    if (cameraId.IsNull()) {
        Logger::E(TAG, "cameraId was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (ccallback == NULL) {
        Logger::E(TAG, "callback was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (handler == NULL) {
        AutoPtr<ILooper> looper = Looper::GetMyLooper();
        if (looper != NULL) {
            CHandler::New((IHandler**)&handler);
        }
        else {
            Logger::E(TAG, "Looper doesn't exist in the calling thread");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    mCameraDevice = NULL;
    return OpenCameraDeviceUserAsync(cameraId, ccallback, handler, (ICameraDevice**)&mCameraDevice);
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
            Logger::E(TAG, "Camera service is currently unavailable");
            *list = deviceIdList;
            REFCOUNT_ADD(*list);
            return E_CAMERA_ACCESS_EXCEPTION;
        }

        ECode ec = cameraService->GetNumberOfCameras(&numCameras);
        if (FAILED(ec)) {
            *list = deviceIdList;
            REFCOUNT_ADD(*list);
            return ec;
        }

        AutoPtr<ICameraMetadataNative> info;
        CCameraMetadataNative::New((ICameraMetadataNative**)&info);
        for (Int32 i = 0; i < numCameras; ++i) {
            // Non-removable cameras use integers starting at 0 for their
            // identifiers
            Int32 tmp;
            ec = cameraService->GetCameraCharacteristics(i, info, &tmp);
            if (FAILED(ec)) {
                Logger::E(TAG, "Failed to GetCameraCharacteristics for camera %d", i);
                return ec;
            }

            Boolean isDeviceSupported = FALSE;
            Boolean res;
            if (info->IsEmpty(&res), !res) {
                isDeviceSupported = TRUE;
            }
            else {
                Logger::E(TAG, "Expected to get non-empty characteristics for cameraId %d", i);
            }

            // //} catch(IllegalArgumentException  e) {
            // if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            //     // Got a BAD_VALUE from service, meaning that this
            //     // device is not supported.
            // }
            // //} catch(CameraRuntimeException e) {
            // if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            //     // DISCONNECTED means that the HAL reported an low-level error getting the
            //     // device info; skip listing the device.  Other errors,
            //     // propagate exception onward
            //     assert(0);
            //     // if (e.getReason() != CameraAccessException.CAMERA_DISCONNECTED) {
            //     //     throw e.asChecked();
            //     // }
            // }
            // //} catch(RemoteException e) {
            // if (ec == (ECode)E_REMOTE_EXCEPTION) {
            //     // Camera service died - no devices to list
            //     deviceIdList->Clear();
            //     *list = deviceIdList;
            //     REFCOUNT_ADD(*list);
            //     return NOERROR;
            // }
            // //}

            if (isDeviceSupported) {
                String str = StringUtils::ToString(i);
                AutoPtr<ICharSequence> ccahr = CoreUtils::Convert(str);
                deviceIdList->Add(TO_IINTERFACE(ccahr));
            }
            else {
                Logger::W(TAG, "Error querying camera device %d for listing.", i);
            }
        }
        mDeviceIdList = deviceIdList;
    }

    *list = mDeviceIdList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CameraManager::HandleRecoverableSetupErrors(
    /* [in] */ Int32 problem,
    /* [in] */ const String& msg)
{
    switch (problem) {
        case ICameraAccessException::CAMERA_DISCONNECTED:
            Logger::W(TAG, "CAMERA_DISCONNECTED:%s", msg.string());
            break;
        default:
            Logger::W(TAG, "E_ILLEGAL_STATE_EXCEPTION: %s", msg.string());
            return E_ILLEGAL_STATE_EXCEPTION;
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
    GetCameraServiceLocked((IICameraService**)&cameraService);

    // If no camera service, no support
    if (cameraService == NULL) {
        Logger::E(TAG, "Camera service is currently unavailable");
        return E_CAMERA_ACCESS_EXCEPTION;
    }

    Int32 res = android::BAD_VALUE;
    cameraService->SupportsCameraApi(id, apiVersion, &res);
    *result = res == android::OK;
    return NOERROR;
}

ECode CameraManager::ConnectCameraServiceLocked()
{
    mCameraService = NULL;

    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> binder = sm->getService(android::String16(CAMERA_SERVICE_BINDER_NAME));

    if (binder == NULL) {
        // Camera service is now down, leave mCameraService as null
        Logger::W(TAG, "Camera service is now down");
        return NOERROR;
    }

    android::sp<android::ICameraService> service = android::ICameraService::asInterface(binder);
    if (service == NULL) {
        Logger::E(TAG, "Failed to get Camera service interface.");
        return E_CAMERA_RUNTIME_EXCEPTION;
    }

    AutoPtr<CameraServiceWrapper> wrapper = new CameraServiceWrapper();
    wrapper->Init(service, this);
    mCameraService = wrapper.Get();


    // /**
    //  * Wrap the camera service in a decorator which automatically translates return codes
    //  * into exceptions.
    //  */
    // AutoPtr<IInterface> obj;
    // CameraServiceBinderDecorator::NewInstance(TO_IINTERFACE(cameraServiceRaw), (IInterface**)&obj);
    // AutoPtr<IICameraService> cameraService = IICameraService::Probe(obj);

    // assert(0);
    // // try {
    // //     CameraServiceBinderDecorator.throwOnError(
    // //             CameraMetadataNative.nativeSetupGlobalVendorTagDescriptor());
    // // } catch (CameraRuntimeException e) {
    // //     handleRecoverableSetupErrors(e, "Failed to set up vendor tags");
    // // }

    // //try {
    // Int32 result;
    // ec = cameraService->AddListener(IICameraServiceListener::Probe(mServiceListener), &result);
    // mCameraService = cameraService;
    // //} catch(CameraRuntimeException e) {
    // if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
    //     // Unexpected failure
    //     // throw new IllegalStateException("Failed to register a camera service listener",
    //     //         e.asChecked());
    //     Logger::E(TAG, "Failed to register a camera service listener");
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }
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
        Logger::I(TAG, "getCameraServiceLocked: Reconnecting to camera service");
        FAIL_RETURN(ConnectCameraServiceLocked())
        if (mCameraService == NULL) {
            Logger::E(TAG, "Camera service is unavailable");
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