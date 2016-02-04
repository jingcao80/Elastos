
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/HardwareCamera.h"
//#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/text/TextUtils.h"
//#include "elastos/droid/text/CSimpleStringSplitter.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>
#include <gui/BufferQueue.h>
//#include <gui/SurfaceTexture.h>
#include <gui/Surface.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include <utils/Errors.h>
#include <elastos/core/CoreUtils.h>

//using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::BitmapCompressFormat_JPEG;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Privacy::IPrivacySettings;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISimpleStringSplitter;
//using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {

String HardwareCamera::TAG("Camera");
const Int32 HardwareCamera::CAMERA_MSG_ERROR            = 0x001;
const Int32 HardwareCamera::CAMERA_MSG_SHUTTER          = 0x002;
const Int32 HardwareCamera::CAMERA_MSG_FOCUS            = 0x004;
const Int32 HardwareCamera::CAMERA_MSG_ZOOM             = 0x008;
const Int32 HardwareCamera::CAMERA_MSG_PREVIEW_FRAME    = 0x010;
const Int32 HardwareCamera::CAMERA_MSG_VIDEO_FRAME      = 0x020;
const Int32 HardwareCamera::CAMERA_MSG_POSTVIEW_FRAME   = 0x040;
const Int32 HardwareCamera::CAMERA_MSG_RAW_IMAGE        = 0x080;
const Int32 HardwareCamera::CAMERA_MSG_COMPRESSED_IMAGE = 0x100;
const Int32 HardwareCamera::CAMERA_MSG_RAW_IMAGE_NOTIFY = 0x200;
const Int32 HardwareCamera::CAMERA_MSG_PREVIEW_METADATA = 0x400;
const Int32 HardwareCamera::CAMERA_MSG_FOCUS_MOVE       = 0x800;
const Int32 HardwareCamera::CAMERA_HAL_API_VERSION_NORMAL_CONNECT = -2;
const Int32 HardwareCamera::CAMERA_HAL_API_VERSION_UNSPECIFIED = -1;
const Int32 HardwareCamera::CAMERA_FACE_DETECTION_HW = 0;
const Int32 HardwareCamera::CAMERA_FACE_DETECTION_SW = 1;

const Int32 HardwareCamera::HardwareCamera_NO_ERROR = 0;
const Int32 HardwareCamera::HardwareCamera_EACCESS = -13;
const Int32 HardwareCamera::HardwareCamera_ENODEV = -19;
const Int32 HardwareCamera::HardwareCamera_EBUSY = -16;
const Int32 HardwareCamera::HardwareCamera_EINVAL = -22;
const Int32 HardwareCamera::HardwareCamera_ENOSYS = -38;
const Int32 HardwareCamera::HardwareCamera_EUSERS = -87;
const Int32 HardwareCamera::HardwareCamera_EOPNOTSUPP = -95;

String HardwareCamera::Parameters::KEY_PREVIEW_SIZE = String("preview-size");
String HardwareCamera::Parameters::KEY_PREVIEW_FORMAT = String("preview-format");
String HardwareCamera::Parameters::KEY_PREVIEW_FRAME_RATE = String("preview-frame-rate");
String HardwareCamera::Parameters::KEY_PREVIEW_FPS_RANGE = String("preview-fps-range");
String HardwareCamera::Parameters::KEY_PICTURE_SIZE = String("picture-size");
String HardwareCamera::Parameters::KEY_PICTURE_FORMAT = String("picture-format");
String HardwareCamera::Parameters::KEY_JPEG_THUMBNAIL_SIZE = String("jpeg-thumbnail-size");
String HardwareCamera::Parameters::KEY_JPEG_THUMBNAIL_WIDTH = String("jpeg-thumbnail-width");
String HardwareCamera::Parameters::KEY_JPEG_THUMBNAIL_HEIGHT = String("jpeg-thumbnail-height");
String HardwareCamera::Parameters::KEY_JPEG_THUMBNAIL_QUALITY = String("jpeg-thumbnail-quality");
String HardwareCamera::Parameters::KEY_JPEG_QUALITY = String("jpeg-quality");
String HardwareCamera::Parameters::KEY_ROTATION = String("rotation");
String HardwareCamera::Parameters::KEY_GPS_LATITUDE = String("gps-latitude");
String HardwareCamera::Parameters::KEY_GPS_LONGITUDE = String("gps-longitude");
String HardwareCamera::Parameters::KEY_GPS_ALTITUDE = String("gps-altitude");
String HardwareCamera::Parameters::KEY_GPS_TIMESTAMP = String("gps-timestamp");
String HardwareCamera::Parameters::KEY_GPS_PROCESSING_METHOD = String("gps-processing-method");
String HardwareCamera::Parameters::KEY_WHITE_BALANCE = String("whitebalance");
String HardwareCamera::Parameters::KEY_EFFECT = String("effect");
String HardwareCamera::Parameters::KEY_ANTIBANDING = String("antibanding");
String HardwareCamera::Parameters::KEY_SCENE_MODE = String("scene-mode");
String HardwareCamera::Parameters::KEY_FLASH_MODE = String("flash-mode");
String HardwareCamera::Parameters::KEY_FOCUS_MODE = String("focus-mode");
String HardwareCamera::Parameters::KEY_FOCUS_AREAS = String("focus-areas");
String HardwareCamera::Parameters::KEY_MAX_NUM_FOCUS_AREAS = String("max-num-focus-areas");
String HardwareCamera::Parameters::KEY_FOCAL_LENGTH = String("focal-length");
String HardwareCamera::Parameters::KEY_HORIZONTAL_VIEW_ANGLE = String("horizontal-view-angle");
String HardwareCamera::Parameters::KEY_VERTICAL_VIEW_ANGLE = String("vertical-view-angle");
String HardwareCamera::Parameters::KEY_EXPOSURE_COMPENSATION = String("exposure-compensation");
String HardwareCamera::Parameters::KEY_MAX_EXPOSURE_COMPENSATION = String("max-exposure-compensation");
String HardwareCamera::Parameters::KEY_MIN_EXPOSURE_COMPENSATION = String("min-exposure-compensation");
String HardwareCamera::Parameters::KEY_EXPOSURE_COMPENSATION_STEP = String("exposure-compensation-step");
String HardwareCamera::Parameters::KEY_AUTO_EXPOSURE_LOCK = String("auto-exposure-lock");
String HardwareCamera::Parameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED = String("auto-exposure-lock-supported");
String HardwareCamera::Parameters::KEY_AUTO_WHITEBALANCE_LOCK = String("auto-whitebalance-lock");
String HardwareCamera::Parameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED = String("auto-whitebalance-lock-supported");
String HardwareCamera::Parameters::KEY_METERING_AREAS = String("metering-areas");
String HardwareCamera::Parameters::KEY_MAX_NUM_METERING_AREAS = String("max-num-metering-areas");
String HardwareCamera::Parameters::KEY_ZOOM = String("zoom");
String HardwareCamera::Parameters::KEY_MAX_ZOOM = String("max-zoom");
String HardwareCamera::Parameters::KEY_ZOOM_RATIOS = String("zoom-ratios");
String HardwareCamera::Parameters::KEY_ZOOM_SUPPORTED = String("zoom-supported");
String HardwareCamera::Parameters::KEY_SMOOTH_ZOOM_SUPPORTED = String("smooth-zoom-supported");
String HardwareCamera::Parameters::KEY_FOCUS_DISTANCES = String("focus-distances");
String HardwareCamera::Parameters::KEY_VIDEO_SIZE = String("video-size");
String HardwareCamera::Parameters::KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO = String("preferred-preview-size-for-video");
String HardwareCamera::Parameters::KEY_MAX_NUM_DETECTED_FACES_HW = String("max-num-detected-faces-hw");
String HardwareCamera::Parameters::KEY_MAX_NUM_DETECTED_FACES_SW = String("max-num-detected-faces-sw");
String HardwareCamera::Parameters::KEY_RECORDING_HINT = String("recording-hint");
String HardwareCamera::Parameters::KEY_VIDEO_SNAPSHOT_SUPPORTED = String("video-snapshot-supported");
String HardwareCamera::Parameters::KEY_VIDEO_STABILIZATION = String("video-stabilization");
String HardwareCamera::Parameters::KEY_VIDEO_STABILIZATION_SUPPORTED = String("video-stabilization-supported");
String HardwareCamera::Parameters::SUPPORTED_VALUES_SUFFIX = String("-values");
String HardwareCamera::Parameters::TRUESTR = String("TRUE");
String HardwareCamera::Parameters::FALSESTR = String("FALSE");
String HardwareCamera::Parameters::PIXEL_FORMAT_YUV422SP = String("yuv422sp");
String HardwareCamera::Parameters::PIXEL_FORMAT_YUV420SP = String("yuv420sp");
String HardwareCamera::Parameters::PIXEL_FORMAT_YUV422I = String("yuv422i-yuyv");
String HardwareCamera::Parameters::PIXEL_FORMAT_YUV420P = String("yuv420p");
String HardwareCamera::Parameters::PIXEL_FORMAT_RGB565 = String("rgb565");
String HardwareCamera::Parameters::PIXEL_FORMAT_JPEG = String("jpeg");
String HardwareCamera::Parameters::PIXEL_FORMAT_BAYER_RGGB = String("bayer-rggb");

static Object sLock;
android::sp<android::Camera> HardwareCamera::get_native_camera(
    /* [in] */ HardwareCamera* thiz,
    /* [in] */ JNICameraContext** pContext)
{
    android::sp<android::Camera> camera;
    synchronized(sLock) {
        JNICameraContext* context = reinterpret_cast<JNICameraContext*>(thiz->mNativeContext)/*(env->GetIntField(thiz, fields.context))*/;
        if (context != NULL) {
            camera = context->getCamera();
        }
        // ALOGV("get_native_camera: context=%p, camera=%p", context, camera.get());
        if (camera == 0) {
            assert(0);
            // jniThrowRuntimeException(env, "Method called after release()");
        }

        if (pContext != NULL) *pContext = context;
    }
    return camera;
}


HardwareCamera::EventHandler::EventHandler(
    /* [in] */ HardwareCamera* c,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mCamera(c)
{
}

ECode HardwareCamera::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg)

    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<IPreviewCallback> pCb;
    AutoPtr<IAutoFocusCallback> cb;

    switch(what) {
        case CAMERA_MSG_SHUTTER:
            if (mCamera->mShutterCallback != NULL) {
                mCamera->mShutterCallback->OnShutter();
            }
            return NOERROR;

        case CAMERA_MSG_RAW_IMAGE:
            if (mCamera->mRawImageCallback != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                Int32 length;
                IArrayOf::Probe(obj)->GetLength(&length);
                AutoPtr<ArrayOf<Byte> > values = ArrayOf<Byte>::Alloc(length);
                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<IByte> bObj;
                    IArrayOf::Probe(obj)->Get(i, (IInterface**)&bObj);
                    Byte value;
                    bObj->GetValue(&value);
                    values->Set(i, value);
                }
                mCamera->mRawImageCallback->OnPictureTaken(values, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_COMPRESSED_IMAGE:
            if (mCamera->mJpegCallback != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                Int32 length;
                IArrayOf::Probe(obj)->GetLength(&length);
                AutoPtr<ArrayOf<Byte> > values = ArrayOf<Byte>::Alloc(length);
                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<IByte> bObj;
                    IArrayOf::Probe(obj)->Get(i, (IInterface**)&bObj);
                    Byte value;
                    bObj->GetValue(&value);
                    values->Set(i, value);
                }
                mCamera->mJpegCallback->OnPictureTaken(values, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_PREVIEW_FRAME:
            pCb = mCamera->mPreviewCallback;
            if (pCb != NULL) {
                if (mCamera->mOneShot) {
                    // Clear the callback variable before the callback
                    // in case the app calls setPreviewCallback from
                    // the callback function
                    mCamera->mPreviewCallback = NULL;
                } else if (!mCamera->mWithBuffer) {
                    // We're faking the camera preview mode to prevent
                    // the app from being flooded with preview frames.
                    // Set to oneshot mode again.
                    mCamera->setHasPreviewCallback(TRUE, FALSE);
                }

                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                Int32 length;
                IArrayOf::Probe(obj)->GetLength(&length);
                AutoPtr<ArrayOf<Byte> > values = ArrayOf<Byte>::Alloc(length);
                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<IByte> bObj;
                    IArrayOf::Probe(obj)->Get(i, (IInterface**)&bObj);
                    Byte value;
                    bObj->GetValue(&value);
                    values->Set(i, value);
                }
                pCb->OnPreviewFrame(values, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_POSTVIEW_FRAME:
            if (mCamera->mPostviewCallback != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                Int32 length;
                IArrayOf::Probe(obj)->GetLength(&length);
                AutoPtr<ArrayOf<Byte> > values = ArrayOf<Byte>::Alloc(length);
                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<IByte> bObj;
                    IArrayOf::Probe(obj)->Get(i, (IInterface**)&bObj);
                    Byte value;
                    bObj->GetValue(&value);
                    values->Set(i, value);
                }
                mCamera->mPostviewCallback->OnPictureTaken(values, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_FOCUS:
            cb = NULL;
            {
                AutoLock lock(mCamera->mAutoFocusCallbackLock);
                cb = mCamera->mAutoFocusCallback;
            }
            if (cb != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                Boolean success = arg1 == 0 ? FALSE : TRUE;
                cb->OnAutoFocus(success, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_ZOOM:
            if (mCamera->mZoomListener != NULL) {
                Int32 arg1, arg2;
                msg->GetArg1(&arg1);
                msg->GetArg1(&arg2);
                mCamera->mZoomListener->OnZoomChange(arg1, arg2 != 0, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_PREVIEW_METADATA:
            if (mCamera->mFaceListener != NULL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                Int32 length;
                IArrayOf::Probe(obj)->GetLength(&length);
                AutoPtr<ArrayOf<ICameraFace*> > faces = ArrayOf<ICameraFace*>::Alloc(length);
                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<ICameraFace> faceObj;
                    IArrayOf::Probe(obj)->Get(i, (IInterface**)&faceObj);
                    faces->Set(i, faceObj);
                }
                assert(0 && "TODO: OnFaceDetection");
                //mCamera->mFaceListener->OnFaceDetection(faces, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_ERROR :
            //Log.e(TAG, "Error " + msg.arg1);
            if (mCamera->mErrorCallback != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mCamera->mErrorCallback->OnError(arg1, mCamera);
            }
            return NOERROR;

        case CAMERA_MSG_FOCUS_MOVE:
            if (mCamera->mAutoFocusMoveCallback != NULL) {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mCamera->mAutoFocusMoveCallback->OnAutoFocusMoving(arg1 == 0 ? FALSE : TRUE, mCamera);
            }
            return NOERROR;

        default:
            //Log.e(TAG, "Unknown message type " + msg.what);
            return NOERROR;
        }
}

bool HardwareCamera::JNICameraContext::isRawImageCallbackBufferAvailable() const
{
    return !mRawImageCallbackBuffers.isEmpty();
}

HardwareCamera::JNICameraContext::JNICameraContext(
    /* [in] */ HardwareCamera* weak_this,
    /* [in] */ /*jclass*/HardwareCamera* clazz,
    /* [in] */ const android::sp<android::Camera>& camera)
{
    mCameraJObjectWeak = weak_this;
    // mCameraJClass = clazz/*(jclass)env->NewGlobalRef(clazz)*/;
    mCamera = camera;

    // jclass faceClazz = env->FindClass("android/hardware/Camera$Face");
    // mFaceClass = (jclass) env->NewGlobalRef(faceClazz);

    // jclass rectClazz = env->FindClass("android/graphics/Rect");
    // mRectClass = (jclass) env->NewGlobalRef(rectClazz);

    mManualBufferMode = false;
    mManualCameraCallbackSet = false;
}

HardwareCamera::JNICameraContext::~JNICameraContext()
{
    release();
}

void HardwareCamera::JNICameraContext::release()
{
    // ALOGV("release");
    AutoLock _l(mLock);
    // JNIEnv *env = AndroidRuntime::getJNIEnv();

    // if (mCameraJObjectWeak != NULL) {
    //     env->DeleteGlobalRef(mCameraJObjectWeak);
    //     mCameraJObjectWeak = NULL;
    // }
    // if (mCameraJClass != NULL) {
    //     env->DeleteGlobalRef(mCameraJClass);
    //     mCameraJClass = NULL;
    // }
    // if (mFaceClass != NULL) {
    //     env->DeleteGlobalRef(mFaceClass);
    //     mFaceClass = NULL;
    // }
    // if (mRectClass != NULL) {
    //     env->DeleteGlobalRef(mRectClass);
    //     mRectClass = NULL;
    // }
    clearCallbackBuffers_l();
    mCamera.clear();
}

void HardwareCamera::JNICameraContext::notify(
    /* [in] */ int32_t msgType,
    /* [in] */ int32_t ext1,
    /* [in] */ int32_t ext2)
{
    // ALOGV("notify");

    // VM pointer will be NULL if object is released
    AutoLock _l(mLock);
    if (mCameraJObjectWeak == NULL) {
        // ALOGW("callback on dead camera object");
        return;
    }

    // JNIEnv *env = AndroidRuntime::getJNIEnv();

    /*
     * If the notification or msgType is CAMERA_MSG_RAW_IMAGE_NOTIFY, change it
     * to CAMERA_MSG_RAW_IMAGE since CAMERA_MSG_RAW_IMAGE_NOTIFY is not exposed
     * to the Java app.
     */
    if (msgType == CAMERA_MSG_RAW_IMAGE_NOTIFY) {
        msgType = CAMERA_MSG_RAW_IMAGE;
    }

    PostEventFromNative(mCameraJObjectWeak, msgType, ext1, ext2, NULL);

    // env->CallStaticVoidMethod(mCameraJClass, fields.post_event,
    //         mCameraJObjectWeak, msgType, ext1, ext2, NULL);
}

AutoPtr<ArrayOf<Byte> > HardwareCamera::JNICameraContext::getCallbackBuffer(
    /* [in] */ android::Vector<ArrayOf<Byte>*>* buffers,
    /* [in] */ size_t bufferSize)
{
    AutoPtr<ArrayOf<Byte> > obj;

    // Vector access should be protected by lock in postData()
    if (!buffers->isEmpty()) {
        // ALOGV("Using callback buffer from queue of length %d", buffers->size());
        AutoPtr<ArrayOf<Byte> > globalBuffer = buffers->itemAt(0);
        buffers->removeAt(0);

        obj = globalBuffer/*(jbyteArray)env->NewLocalRef(globalBuffer)*/;
        // env->DeleteGlobalRef(globalBuffer);

        if (obj != NULL) {
            Int32 bufferLength = obj->GetLength();
            if ((int)bufferLength < (int)bufferSize) {
                // ALOGE("Callback buffer was too small! Expected %d bytes, but got %d bytes!",
                //     bufferSize, bufferLength);
                // env->DeleteLocalRef(obj);
                return NULL;
            }
        }
    }

    return obj;
}

void HardwareCamera::JNICameraContext::copyAndPost(
    /* [in] */ const android::sp<android::IMemory>& dataPtr,
    /* [in] */ int msgType)
{
    AutoPtr<ArrayOf<Byte> > obj;

    // allocate Java byte array and copy data
    if (dataPtr != NULL) {
        ssize_t offset;
        size_t size;
        android::sp<android::IMemoryHeap> heap = dataPtr->getMemory(&offset, &size);
        // ALOGV("copyAndPost: off=%zd, size=%zu", offset, size);
        uint8_t *heapBase = (uint8_t*)heap->base();

        if (heapBase != NULL) {
            const Byte* data = reinterpret_cast<const Byte*>(heapBase + offset);

            if (msgType == CAMERA_MSG_RAW_IMAGE) {
                obj = getCallbackBuffer(&mRawImageCallbackBuffers, size);
            } else if (msgType == CAMERA_MSG_PREVIEW_FRAME && mManualBufferMode) {
                obj = getCallbackBuffer(&mCallbackBuffers, size);

                if (mCallbackBuffers.isEmpty()) {
                    // ALOGV("Out of buffers, clearing callback!");
                    mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_NOOP);
                    mManualCameraCallbackSet = false;

                    if (obj == NULL) {
                        return;
                    }
                }
            } else {
                // ALOGV("Allocating callback buffer");
                // obj = env->NewByteArray(size);
                obj = ArrayOf<Byte>::Alloc(size);
            }

            if (obj == NULL) {
                // ALOGE("Couldn't allocate byte array for JPEG data");
                // env->ExceptionClear();
            } else {
                //TODO
                // env->SetByteArrayRegion(obj, 0, size, data);
                obj->Copy(data, size);
            }
        } else {
            // ALOGE("image heap is NULL");
        }
    }

    // post image data to Java
    // env->CallStaticVoidMethod(mCameraJClass, fields.post_event,
    //         mCameraJObjectWeak, msgType, 0, 0, obj);
    PostEventFromNative(mCameraJObjectWeak, msgType, 0, 0, obj);
    // if (obj) {
    //     env->DeleteLocalRef(obj);
    // }
}

void HardwareCamera::JNICameraContext::postData(
    /* [in] */ int32_t msgType,
    /* [in] */ const android::sp<android::IMemory>& dataPtr,
    /* [in] */ camera_frame_metadata_t *metadata)
{
    // VM pointer will be NULL if object is released
    AutoLock _l(mLock);
    // JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (mCameraJObjectWeak == NULL) {
        // ALOGW("callback on dead camera object");
        return;
    }

    int32_t dataMsgType = msgType & ~CAMERA_MSG_PREVIEW_METADATA;

    // return data based on callback type
    switch (dataMsgType) {
        case CAMERA_MSG_VIDEO_FRAME:
            // should never happen
            break;

        // For backward-compatibility purpose, if there is no callback
        // buffer for raw image, the callback returns NULL.
        case CAMERA_MSG_RAW_IMAGE:
            // ALOGV("rawCallback");
            if (mRawImageCallbackBuffers.isEmpty()) {
                // env->CallStaticVoidMethod(mCameraJClass, fields.post_event,
                //         mCameraJObjectWeak, dataMsgType, 0, 0, NULL);
                PostEventFromNative(mCameraJObjectWeak, dataMsgType, 0, 0, NULL);
            } else {
                copyAndPost(dataPtr, dataMsgType);
            }
            break;

        // There is no data.
        case 0:
            break;

        default:
            // ALOGV("dataCallback(%d, %p)", dataMsgType, dataPtr.get());
            copyAndPost(dataPtr, dataMsgType);
            break;
    }

    // post frame metadata to Java
    if (metadata && (msgType & CAMERA_MSG_PREVIEW_METADATA)) {
        postMetadata(CAMERA_MSG_PREVIEW_METADATA, metadata);
    }
}

void HardwareCamera::JNICameraContext::postDataTimestamp(
    /* [in] */ nsecs_t timestamp,
    /* [in] */ int32_t msgType,
    /* [in] */ const android::sp<android::IMemory>& dataPtr)
{
    // TODO: plumb up to Java. For now, just drop the timestamp
    postData(msgType, dataPtr, NULL);
}

void HardwareCamera::JNICameraContext::postMetadata(
    /* [in] */ int32_t msgType,
    /* [in] */ camera_frame_metadata_t *metadata)
{
    // jobjectArray obj = NULL;
    // obj = (jobjectArray) env->NewObjectArray(metadata->number_of_faces,
    //                                          mFaceClass, NULL);
    AutoPtr<ArrayOf<Face*> > obj = ArrayOf<Face*>::Alloc(metadata->number_of_faces);
    if (obj == NULL) {
        // ALOGE("Couldn't allocate face metadata array");
        return;
    }

    for (int i = 0; i < metadata->number_of_faces; i++) {
        // jobject face = env->NewObject(mFaceClass, fields.face_constructor);
        AutoPtr<Face> face = new Face();

        // env->SetObjectArrayElement(obj, i, face);
        (*obj)[i] = face;

        // jobject rect = env->NewObject(mRectClass, fields.rect_constructor);
        AutoPtr<IRect> rect;
        CRect::New((IRect**)&rect);
        assert(rect != NULL);

        // env->SetIntField(rect, fields.rect_left, metadata->faces[i].rect[0]);
        // env->SetIntField(rect, fields.rect_top, metadata->faces[i].rect[1]);
        // env->SetIntField(rect, fields.rect_right, metadata->faces[i].rect[2]);
        // env->SetIntField(rect, fields.rect_bottom, metadata->faces[i].rect[3]);
        rect->SetLeft(metadata->faces[i].rect[0]);
        rect->SetTop(metadata->faces[i].rect[1]);
        rect->SetRight(metadata->faces[i].rect[2]);
        rect->SetBottom(metadata->faces[i].rect[3]);


        // env->SetObjectField(face, fields.face_rect, rect);
        face->mRect = rect;

        // env->SetIntField(face, fields.face_score, metadata->faces[i].score);
        face->mScore = metadata->faces[i].score;

        // env->DeleteLocalRef(face);
        // env->DeleteLocalRef(rect);
    }

    // env->CallStaticVoidMethod(mCameraJClass, fields.post_event,
    //         mCameraJObjectWeak, msgType, 0, 0, obj);
    PostEventFromNative(mCameraJObjectWeak, msgType, 0, 0, obj);
    // env->DeleteLocalRef(obj);
}

void HardwareCamera::JNICameraContext::setCallbackMode(
    /* [in] */ bool installed,
    /* [in] */ bool manualMode)
{
    AutoLock _l(mLock);
    mManualBufferMode = manualMode;
    mManualCameraCallbackSet = false;

    // In order to limit the over usage of binder threads, all non-manual buffer
    // callbacks use CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER mode now.
    //
    // Continuous callbacks will have the callback re-registered from handleMessage.
    // Manual buffer mode will operate as fast as possible, relying on the finite supply
    // of buffers for throttling.

    if (!installed) {
        mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_NOOP);
        clearCallbackBuffers_l(&mCallbackBuffers);
    } else if (mManualBufferMode) {
        if (!mCallbackBuffers.isEmpty()) {
            mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_CAMERA);
            mManualCameraCallbackSet = TRUE;
        }
    } else {
        mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_BARCODE_SCANNER);
        clearCallbackBuffers_l(&mCallbackBuffers);
    }
}

android::sp<android::Camera> HardwareCamera::JNICameraContext::getCamera()
{
    AutoLock _l(mLock);
    return mCamera;
}

void HardwareCamera::JNICameraContext::addCallbackBuffer(
    /* [in] */ ArrayOf<Byte>* cbb,
    /* [in] */ int msgType)
{
    // ALOGV("addCallbackBuffer: 0x%x", msgType);
    if (cbb != NULL) {
        AutoLock _l(mLock);
        switch (msgType) {
            case CAMERA_MSG_PREVIEW_FRAME: {
                // jbyteArray callbackBuffer = (jbyteArray)env->NewGlobalRef(cbb);
                cbb->AddRef();
                mCallbackBuffers.push(cbb);

                // ALOGV("Adding callback buffer to queue, %d total",
                //         mCallbackBuffers.size());

                // We want to make sure the camera knows we're ready for the
                // next frame. This may have come unset had we not had a
                // callbackbuffer ready for it last time.
                if (mManualBufferMode && !mManualCameraCallbackSet) {
                    mCamera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_CAMERA);
                    mManualCameraCallbackSet = TRUE;
                }
                break;
            }
            case CAMERA_MSG_RAW_IMAGE: {
                // jbyteArray callbackBuffer = (jbyteArray)env->NewGlobalRef(cbb);
                cbb->AddRef();
                mRawImageCallbackBuffers.push(cbb);
                break;
            }
            default: {
                assert(0);
                // jniThrowException(env,
                //         "java/lang/IllegalArgumentException",
                //         "Unsupported message type");
                return;
            }
        }
    } else {
       // ALOGE("Null byte array!");
    }
}

void HardwareCamera::JNICameraContext::clearCallbackBuffers_l()
{
    // TODO : call Release() of each element in container.
    clearCallbackBuffers_l(&mCallbackBuffers);
    clearCallbackBuffers_l(&mRawImageCallbackBuffers);
}

void HardwareCamera::JNICameraContext::clearCallbackBuffers_l(
    /* [in] */ android::Vector<ArrayOf<Byte>*>* buffers)
{
    // ALOGV("Clearing callback buffers, %d remained", buffers->size());
    while (!buffers->isEmpty()) {
        // env->DeleteGlobalRef(buffers->top());
        //TODO :need to release?
        //buffers->top()->Release();

        buffers->pop();
    }
}

CAR_INTERFACE_IMPL(HardwareCamera::CameraInfo, Object, IHardwareCameraInfo)

HardwareCamera::CameraInfo::CameraInfo()
    : mFacing(0)
    , mOrientation(0)
    , mCanDisableShutterSound(FALSE)
{
}

ECode HardwareCamera::CameraInfo::SetFacing(
    /* [in] */ Int32 facing)
{
    mFacing = facing;
    return NOERROR;
}

ECode HardwareCamera::CameraInfo::GetFacing(
    /* [out] */ Int32* facing)
{
    VALIDATE_NOT_NULL(facing);

    (*facing) = mFacing;
    return NOERROR;
}

ECode HardwareCamera::CameraInfo::SetOrientation(
    /* [in] */ Int32 orientation)
{
    mOrientation = orientation;
    return NOERROR;
}

ECode HardwareCamera::CameraInfo::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);

    *orientation = mOrientation;
    return NOERROR;
}

ECode HardwareCamera::CameraInfo::SetCanDisableShutterSound(
    /* [in] */ Boolean can)
{
    mCanDisableShutterSound = can;
    return NOERROR;
}

ECode HardwareCamera::CameraInfo::GetCanDisableShutterSound(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);

    *can = mCanDisableShutterSound;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HardwareCamera::Face, Object, ICameraFace);

HardwareCamera::Face::Face()
    : mScore(0)
    , mId(-1)
{
}

ECode HardwareCamera::Face::SetRect(
    /* [in] */ IRect* rect)
{
    mRect = rect;
    return NOERROR;
}

ECode HardwareCamera::Face::SetScore(
    /* [in] */ Int32 score)
{
    mScore = score;
    return NOERROR;
}

ECode HardwareCamera::Face::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode HardwareCamera::Face::SetLeftEye(
    /* [in] */ IPoint* leftEye)
{
    mLeftEye = leftEye;
    return NOERROR;
}

ECode HardwareCamera::Face::SetRightEye(
    /* [in] */ IPoint* rightEye)
{
    mRightEye = rightEye;
    return NOERROR;
}

ECode HardwareCamera::Face::SetMouth(
    /* [in] */ IPoint* mouth)
{
    mMouth = mouth;
    return NOERROR;
}

ECode HardwareCamera::Face::GetRect(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect)

    *rect = mRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode HardwareCamera::Face::GetScore(
    /* [out] */ Int32* score)
{
    VALIDATE_NOT_NULL(score)

    *score = mScore;
    return NOERROR;
}

ECode HardwareCamera::Face::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    *id = mId;
    return NOERROR;
}

ECode HardwareCamera::Face::GetLeftEye(
    /* [out] */ IPoint** leftEye)
{
    VALIDATE_NOT_NULL(leftEye)

    *leftEye = mLeftEye;
    REFCOUNT_ADD(*leftEye);
    return NOERROR;
}

ECode HardwareCamera::Face::GetRightEye(
    /* [out] */ IPoint** rightEye)
{
    VALIDATE_NOT_NULL(rightEye)

    *rightEye = mRightEye;
    REFCOUNT_ADD(*rightEye);
    return NOERROR;
}

ECode HardwareCamera::Face::GetMouth(
    /* [out] */ IPoint** mouth)
{
    VALIDATE_NOT_NULL(mouth)

    *mouth = mMouth;
    REFCOUNT_ADD(*mouth);
    return NOERROR;
}

CAR_INTERFACE_IMPL(HardwareCamera::Size, Object, ICameraSize);

HardwareCamera::Size::Size(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
    : mWidth(w)
    , mHeight(h)
{
}

ECode HardwareCamera::Size::Equals(
    /* [in] */ ICameraSize* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);

    *e = FALSE;
    if (obj == NULL) {
        return NOERROR;
    }

    Size* s = (Size*) obj;
    *e = mWidth == s->mWidth && mHeight == s->mHeight;
    return NOERROR;
}

ECode HardwareCamera::Size::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);

    *e = FALSE;
    return Equals(ICameraSize::Probe(obj), e);
}

ECode HardwareCamera::Size::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    *code = mWidth * 32713 + mHeight;
    return NOERROR;
}

ECode HardwareCamera::Size::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode HardwareCamera::Size::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HardwareCamera::Area, Object, ICameraArea);

HardwareCamera::Area::Area(
    /* [in] */ IRect* rect,
    /* [in] */ Int32 weight)
    : mRect(rect)
    , mWeight(weight)
{
}

ECode HardwareCamera::Area::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;
    if (ICameraArea::Probe(obj) == NULL) {
        return NOERROR;
    }

    Area* a = (Area*)ICameraArea::Probe(obj);
    if (mRect == NULL) {
        if (a->mRect != NULL) return NOERROR;
    } else {
        Boolean tmp = FALSE;
        mRect->Equals(a->mRect, &tmp);
        if (!tmp) return NOERROR;
    }

    *e = mWeight == a->mWeight;
    return NOERROR;
}

ECode HardwareCamera::Area::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = 0; // do not care.
    return NOERROR;
}

ECode HardwareCamera::Area::GetRect(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    *rect = mRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode HardwareCamera::Area::GetWeight(
    /* [out] */ Int32* weight)
{
    VALIDATE_NOT_NULL(weight);

    *weight = mWeight;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HardwareCamera::Parameters, Object, IParameters);

HardwareCamera::Parameters::Parameters(
    /* [in] */ IHardwareCamera* camera)
{
    CHashMap::New(64, (IHashMap**)&mMap);
    mOuter = camera;
}

HardwareCamera::Parameters::CopyFrom(
    /* [in] */ IParameters* other)
{
    if (other == NULL) {
        //throw new NullPointerException("other must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mMap->PutAll(IMap::Probe(((Parameters*)other)->mMap));
    return NOERROR;
}

AutoPtr<IHardwareCamera> HardwareCamera::Parameters::GetOuter()
{
    return mOuter;
}

HardwareCamera::Parameters::Same(
    /* [in] */ IParameters* other,
    /* [out] */ Boolean* result)
{
    if (this == other) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean _result;
    mMap->Equals(((Parameters*)other)->mMap, &_result);
    *result = (other != NULL && _result);
    return NOERROR;
}

ECode HardwareCamera::Parameters::Flatten(
    /* [out] */ String* content)
{
    VALIDATE_NOT_NULL(content);

    AutoPtr<ISet> entrySet;
    mMap->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IMapEntry* entry;
    AutoPtr<StringBuilder> flattened = new StringBuilder(128);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);
        entry->GetValue((IInterface**)&vo);

        flattened->Append(Object::ToString(ko));
        flattened->Append("=");
        flattened->Append(Object::ToString(vo));
        flattened->Append(";");
    }

    // chop off the extra semicolon at the end
    flattened->DeleteCharAt(flattened->GetLength() - 1);
    *content = flattened->ToString();
    return NOERROR;
}

ECode HardwareCamera::Parameters::Unflatten(
    /* [in] */ const String& flattened)
{
    mMap->Clear();

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    //CSimpleStringSplitter::New(';', (ISimpleStringSplitter**)&splitter);
    //splitter->SetString(flattened);

    // Boolean has = FALSE;
    // while (splitter->HasNext(&has), has) {
    //     AutoPtr<ICharSequence> cs;
    //     splitter->Next((IInterface**)&cs);
    //     String kv;
    //     cs->ToString(&kv);

    //     Int32 pos = kv.IndexOf('=');
    //     if (pos == -1) {
    //         continue;
    //     }
    //     String k = kv.Substring(0, pos);
    //     String v = kv.Substring(pos + 1);
    //     mMap[k] = v;
    // }

    return NOERROR;
}

ECode HardwareCamera::Parameters::Remove(
    /* [in] */ const String& key)
{
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    return mMap->Remove(ko);
}

ECode HardwareCamera::Parameters::Set(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (key.IndexOf('=') != -1 || key.IndexOf(';') != -1 || key.IndexOf((Char32)0) != -1) {
        // Log.e(TAG, "Key \"" + key + "\" contains invalid character (= or ; or \\0)");
        return NOERROR;
    }
    if (value.IndexOf('=') != -1 || value.IndexOf(';') != -1 || value.IndexOf((Char32)0) != -1) {
        // Log.e(TAG, "Value \"" + value + "\" contains invalid character (= or ; or \\0)");
        return NOERROR;
    }

    Put(key, value);
    return NOERROR;
}

ECode HardwareCamera::Parameters::Set(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    Put(key, StringUtils::ToString(value));
    return NOERROR;
}

void HardwareCamera::Parameters::Set(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<ICameraArea*>* areas)
{
    if (areas == NULL) {
        Set(key, String("(0,0,0,0,0)"));
    } else {
        AutoPtr<StringBuilder> buffer = new StringBuilder();
        for (Int32 i = 0; i < areas->GetLength(); i++) {
            AutoPtr<ICameraArea> _area = (*areas)[i];
            Area* area = (Area*)_area.Get();

            CRect* rect = (CRect*)area->mRect.Get();
            buffer->AppendChar('(');
            buffer->Append(rect->mLeft);
            buffer->AppendChar(',');
            buffer->Append(rect->mTop);
            buffer->AppendChar(',');
            buffer->Append(rect->mRight);
            buffer->AppendChar(',');
            buffer->Append(rect->mBottom);
            buffer->AppendChar(',');
            buffer->Append(area->mWeight);
            buffer->AppendChar(')');
            if (i != areas->GetLength() - 1) buffer->AppendChar(',');
        }

        Set(key, buffer->ToString());
    }
}

void HardwareCamera::Parameters::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    /*
     * Remove the key if it already exists.
     *
     * This way setting a new value for an already existing key will always move
     * that key to be ordered the latest in the map.
     */
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    mMap->Remove(ko);
    AutoPtr<ICharSequence> vo = CoreUtils::Convert(value);
    mMap->Put(TO_IINTERFACE(ko), TO_IINTERFACE(vo));
}

ECode HardwareCamera::Parameters::Get(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mMap->Get(ko, (IInterface**)&obj);
    *value = Object::ToString(obj);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetInt(
    /* [in] */ const String& key,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mMap->Get(ko, (IInterface**)&obj);
    String _value = Object::ToString(obj);
    *value = StringUtils::ParseInt32(_value);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetPreviewSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    String v = StringUtils::ToString(width) + String("x") + StringUtils::ToString(height);
    Set(KEY_PREVIEW_SIZE, v);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetPreviewSize(
    /* [out] */ ICameraSize** size)
{
    VALIDATE_NOT_NULL(size);

    String pair;
    Get(KEY_PREVIEW_SIZE, &pair);
    AutoPtr<ICameraSize> cs = StrToSize(pair);
    *size = cs;
    REFCOUNT_ADD(*size);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedPreviewSizes(
    /* [out, callee] */ ArrayOf<ICameraSize*>** sizes)
{
    VALIDATE_NOT_NULL(sizes);

    String str;
    Get(KEY_PREVIEW_SIZE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<ICameraSize*> > array = SplitSize(str);
    *sizes = array;
    REFCOUNT_ADD(*sizes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedVideoSizes(
    /* [out, callee] */ ArrayOf<ICameraSize*>** sizes)
{
    VALIDATE_NOT_NULL(sizes);

    String str;
    Get(KEY_VIDEO_SIZE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<ICameraSize*> > array = SplitSize(str);
    *sizes = array;
    REFCOUNT_ADD(*sizes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetPreferredPreviewSizeForVideo(
    /* [out] */ ICameraSize** size)
{
    VALIDATE_NOT_NULL(size);

    String pair;
    Get(KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO, &pair);
    AutoPtr<ICameraSize> temp = StrToSize(pair);
    *size = temp;
    REFCOUNT_ADD(*size);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetJpegThumbnailSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Set(KEY_JPEG_THUMBNAIL_WIDTH, width);
    Set(KEY_JPEG_THUMBNAIL_HEIGHT, height);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetJpegThumbnailSize(
    /* [out] */ ICameraSize** size)
{
    VALIDATE_NOT_NULL(size);

    Int32 w = 0, h = 0;
    GetInt(KEY_JPEG_THUMBNAIL_WIDTH, &w);
    GetInt(KEY_JPEG_THUMBNAIL_HEIGHT, &h);

    AutoPtr<ICameraSize> _size = new Size(w, h);
    *size = _size;
    REFCOUNT_ADD(*size);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedJpegThumbnailSizes(
    /* [out, callee] */ ArrayOf<ICameraSize*>** sizes)
{
    VALIDATE_NOT_NULL(sizes);

    String str;
    Get(KEY_JPEG_THUMBNAIL_SIZE + SUPPORTED_VALUES_SUFFIX, &str);
    assert(sizes != NULL);
    AutoPtr<ArrayOf<ICameraSize*> > array = SplitSize(str);
    *sizes = array;
    REFCOUNT_ADD(*sizes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetJpegThumbnailQuality(
    /* [in] */ Int32 quality)
{
    return Set(KEY_JPEG_THUMBNAIL_QUALITY, quality);
}

ECode HardwareCamera::Parameters::GetJpegThumbnailQuality(
    /* [out] */ Int32* quality)
{
    VALIDATE_NOT_NULL(quality);

    return GetInt(KEY_JPEG_THUMBNAIL_QUALITY, quality);
}

ECode HardwareCamera::Parameters::SetJpegQuality(
    /* [in] */ Int32 quality)
{
    return Set(KEY_JPEG_QUALITY, quality);
}

ECode HardwareCamera::Parameters::GetJpegQuality(
    /* [out] */ Int32* quality)
{
    VALIDATE_NOT_NULL(quality);

    return GetInt(KEY_JPEG_QUALITY, quality);
}

ECode HardwareCamera::Parameters::SetPreviewFrameRate(
    /* [in] */ Int32 fps)
{
    return Set(KEY_PREVIEW_FRAME_RATE, fps);
}

ECode HardwareCamera::Parameters::GetPreviewFrameRate(
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);

    return GetInt(KEY_PREVIEW_FRAME_RATE, rate);
}

ECode HardwareCamera::Parameters::GetSupportedPreviewFrameRates(
    /* [out, callee] */ ArrayOf<IInteger32*>** rates)
{
    VALIDATE_NOT_NULL(rates);

    String str;
    Get(KEY_PREVIEW_FRAME_RATE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<IInteger32*> > array =  SplitInt(str);
    *rates = array;
    REFCOUNT_ADD(*rates);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetPreviewFpsRange(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    return Set(KEY_PREVIEW_FPS_RANGE, StringUtils::ToString(min) + String(",") + StringUtils::ToString(max));
}

ECode HardwareCamera::Parameters::GetPreviewFpsRange(
    /* [out] */ ArrayOf<Int32>* range)
{
    VALIDATE_NOT_NULL(range);

    if (range == NULL || range->GetLength() != 2) {
        // throw new IllegalArgumentException(
        //         "range must be an array with two elements.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String str;
    Get(KEY_PREVIEW_FPS_RANGE, &str);
    SplitInt(str, range);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedPreviewFpsRange(
    /* [out] */ IList** lists)
{
    VALIDATE_NOT_NULL(lists);

    String str;
    Get(KEY_PREVIEW_FPS_RANGE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<IList> temp = SplitRange(str);
    *lists = temp;
    REFCOUNT_ADD(*lists);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetPreviewFormat(
    /* [in] */ Int32 pixel_format)
{
    String s;
    CameraFormatForPixelFormat(pixel_format, &s);
    if (s.IsNull()) {
        // throw new IllegalArgumentException(
        //         "Invalid pixel_format=" + pixel_format);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }

    return Set(KEY_PREVIEW_FORMAT, s);
}

ECode HardwareCamera::Parameters::GetPreviewFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);

    String f;
    Get(KEY_PREVIEW_FORMAT, &f);
    *format = PixelFormatForCameraFormat(f);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedPreviewFormats(
    /* [out, callee] */ ArrayOf<IInteger32*>** formats)
{
    VALIDATE_NOT_NULL(formats);
    *formats = NULL;

    String str;
    Get(KEY_PREVIEW_FORMAT + SUPPORTED_VALUES_SUFFIX, &str);

    AutoPtr<ArrayOf<String> > splitStrArray = Split(str);
    if (splitStrArray != NULL) {
        *formats = ArrayOf<IInteger32*>::Alloc(splitStrArray->GetLength());
        REFCOUNT_ADD(*formats);

        for (Int32 i = 0; i < splitStrArray->GetLength(); i++) {
            String s = (*splitStrArray)[i];

            Int32 f = PixelFormatForCameraFormat(s);
            if (f == IImageFormat::UNKNOWN) continue;

            AutoPtr<IInteger32> obj;
            CInteger32::New(f, (IInteger32**)&obj);
            (*formats)->Set(i, obj.Get());
        }
    }

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetPictureSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    String v = StringUtils::ToString(width) + String("x") + StringUtils::ToString(height);
    return Set(KEY_PICTURE_SIZE, v);
}

ECode HardwareCamera::Parameters::GetPictureSize(
    /* [out] */ ICameraSize** size)
{
    VALIDATE_NOT_NULL(size);

    String pair;
    Get(KEY_PICTURE_SIZE, &pair);
    AutoPtr<ICameraSize> temp = StrToSize(pair);
    *size = temp;
    REFCOUNT_ADD(*size);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedPictureSizes(
    /* [out, callee] */ ArrayOf<ICameraSize*>** sizes)
{
    VALIDATE_NOT_NULL(sizes);

    String str;
    Get(KEY_PICTURE_SIZE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<ICameraSize*> > array = SplitSize(str).Get();
    *sizes = array;
    REFCOUNT_ADD(*sizes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetPictureFormat(
    /* [in] */ Int32 pixel_format)
{
    String s;
    CameraFormatForPixelFormat(pixel_format, &s);
    if (s.IsNull()) {
        // throw new IllegalArgumentException(
        //         "Invalid pixel_format=" + pixel_format);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }

    return Set(KEY_PICTURE_FORMAT, s);
}

ECode HardwareCamera::Parameters::GetPictureFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);

    String f;
    Get(KEY_PICTURE_FORMAT, &f);
    *format = PixelFormatForCameraFormat(f);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSupportedPictureFormats(
    /* [out, callee] */ ArrayOf<IInteger32*>** formats)
{
    VALIDATE_NOT_NULL(formats);
    *formats = NULL;

    String str;
    Get(KEY_PICTURE_FORMAT + SUPPORTED_VALUES_SUFFIX, &str);

    AutoPtr<ArrayOf<String> > splitStrArray = Split(str);
    if (splitStrArray != NULL) {
        *formats = ArrayOf<IInteger32*>::Alloc(splitStrArray->GetLength());
        REFCOUNT_ADD(*formats);

        for (Int32 i = 0; i < splitStrArray->GetLength(); i++) {
            String s = (*splitStrArray)[i];

            Int32 f = PixelFormatForCameraFormat(s);
            if (f == IImageFormat::UNKNOWN) continue;

            AutoPtr<IInteger32> obj;
            CInteger32::New(f, (IInteger32**)&obj);
            (*formats)->Set(i, obj.Get());
        }
    }

    return NOERROR;
}

ECode HardwareCamera::Parameters::CameraFormatForPixelFormat(
    /* [in] */ Int32 pixel_format,
    /* [out] */ String* format)
{
    VALIDATE_NOT_NULL(format);
    *format = NULL;

    switch(pixel_format) {
        case IImageFormat::NV16:      *format = PIXEL_FORMAT_YUV422SP;
        case IImageFormat::NV21:      *format = PIXEL_FORMAT_YUV420SP;
        case IImageFormat::YUY2:      *format = PIXEL_FORMAT_YUV422I;
        case IImageFormat::YV12:      *format = PIXEL_FORMAT_YUV420P;
        case IImageFormat::RGB_565:   *format = PIXEL_FORMAT_RGB565;
        case IImageFormat::JPEG:      *format = PIXEL_FORMAT_JPEG;
        default:                    return NOERROR;
    }
    return NOERROR;
}

Int32 HardwareCamera::Parameters::PixelFormatForCameraFormat(
    /* [in] */ const String& format)
{
    if (format.IsNull())
        return IImageFormat::UNKNOWN;

    if (format.Equals(PIXEL_FORMAT_YUV422SP))
        return IImageFormat::NV16;

    if (format.Equals(PIXEL_FORMAT_YUV420SP))
        return IImageFormat::NV21;

    if (format.Equals(PIXEL_FORMAT_YUV422I))
        return IImageFormat::YUY2;

    if (format.Equals(PIXEL_FORMAT_YUV420P))
        return IImageFormat::YV12;

    if (format.Equals(PIXEL_FORMAT_RGB565))
        return IImageFormat::RGB_565;

    if (format.Equals(PIXEL_FORMAT_JPEG))
        return IImageFormat::JPEG;

    return IImageFormat::UNKNOWN;
}

ECode HardwareCamera::Parameters::SetRotation(
    /* [in] */ Int32 rotation)
{
    if (rotation == 0 || rotation == 90 || rotation == 180
            || rotation == 270) {
        Set(KEY_ROTATION, StringUtils::ToString(rotation));
    } else {
        // throw new IllegalArgumentException(
        //         "Invalid rotation=" + rotation);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetGpsLatitude(
    /* [in] */ Double latitude)
{
    return Set(KEY_GPS_LATITUDE, StringUtils::ToString(latitude));
}

ECode HardwareCamera::Parameters::SetGpsLongitude(
    /* [in] */ Double longitude)
{
    return Set(KEY_GPS_LONGITUDE, StringUtils::ToString(longitude));
}

ECode HardwareCamera::Parameters::SetGpsAltitude(
    /* [in] */ Double altitude)
{
    return Set(KEY_GPS_ALTITUDE, StringUtils::ToString(altitude));
}

ECode HardwareCamera::Parameters::SetGpsTimestamp(
    /* [in] */ Int64 timestamp)
{
    return Set(KEY_GPS_TIMESTAMP, StringUtils::ToString(timestamp));
}

ECode HardwareCamera::Parameters::SetGpsProcessingMethod(
    /* [in] */ const String& processing_method)
{
    return Set(KEY_GPS_PROCESSING_METHOD, processing_method);
}

ECode HardwareCamera::Parameters::RemoveGpsData()
{
    Remove(KEY_GPS_LATITUDE);
    Remove(KEY_GPS_LONGITUDE);
    Remove(KEY_GPS_ALTITUDE);
    Remove(KEY_GPS_TIMESTAMP);
    Remove(KEY_GPS_PROCESSING_METHOD);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetWhiteBalance(
    /* [out] */ String* balance)
{
    return Get(KEY_WHITE_BALANCE, balance);
}

ECode HardwareCamera::Parameters::SetWhiteBalance(
    /* [in] */ const String& value)
{
    String oldValue;
    Get(KEY_WHITE_BALANCE, &oldValue);
    if (Same(value, oldValue)) return NOERROR;
    Set(KEY_WHITE_BALANCE, value);
    return Set(KEY_AUTO_WHITEBALANCE_LOCK, FALSESTR);
}

ECode HardwareCamera::Parameters::GetSupportedWhiteBalance(
    /* [out, callee] */ ArrayOf<String>** balance)
{
    VALIDATE_NOT_NULL(balance);

    String str;
    Get(KEY_WHITE_BALANCE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *balance = array;
    REFCOUNT_ADD(*balance);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetColorEffect(
    /* [out] */ String* effect)
{
    return Get(KEY_EFFECT, effect);
}

ECode HardwareCamera::Parameters::SetColorEffect(
    /* [in] */ const String& value)
{
    return Set(KEY_EFFECT, value);
}

ECode HardwareCamera::Parameters::GetSupportedColorEffects(
    /* [out, callee] */ ArrayOf<String>** effects)
{
    VALIDATE_NOT_NULL(effects);

    String str;
    Get(KEY_EFFECT + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *effects = array;
    REFCOUNT_ADD(*effects);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetAntibanding(
    /* [out] */ String* banding)
{
    return Get(KEY_ANTIBANDING, banding);
}

ECode HardwareCamera::Parameters::SetAntibanding(
    /* [in] */ const String& antibanding)
{
    return Set(KEY_ANTIBANDING, antibanding);
}

ECode HardwareCamera::Parameters::GetSupportedAntibanding(
    /* [out, callee] */ ArrayOf<String>** values)
{
    VALIDATE_NOT_NULL(values);

    String str;
    Get(KEY_ANTIBANDING + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *values = array;
    REFCOUNT_ADD(*values);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetSceneMode(
    /* [out] */ String* mode)
{
    return Get(KEY_SCENE_MODE, mode);
}

ECode HardwareCamera::Parameters::SetSceneMode(
    /* [in] */ const String& value)
{
    return Set(KEY_SCENE_MODE, value);
}

ECode HardwareCamera::Parameters::GetSupportedSceneModes(
    /* [out, callee] */ ArrayOf<String>** modes)
{
    VALIDATE_NOT_NULL(modes);

    String str;
    Get(KEY_SCENE_MODE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *modes = array;
    REFCOUNT_ADD(*modes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetFlashMode(
    /* [out] */ String* mode)
{
    return Get(KEY_FLASH_MODE, mode);
}

ECode HardwareCamera::Parameters::SetFlashMode(
    /* [in] */ const String& value)
{
    return Set(KEY_FLASH_MODE, value);
}

ECode HardwareCamera::Parameters::GetSupportedFlashModes(
    /* [out, callee] */ ArrayOf<String>** modes)
{
    VALIDATE_NOT_NULL(modes);

    String str;
    Get(KEY_FLASH_MODE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *modes = array;
    REFCOUNT_ADD(*modes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetFocusMode(
    /* [out] */ String* mode)
{
    return Get(KEY_FOCUS_MODE, mode);
}

ECode HardwareCamera::Parameters::SetFocusMode(
    /* [in] */ const String& value)
{
    return Set(KEY_FOCUS_MODE, value);
}

ECode HardwareCamera::Parameters::GetSupportedFocusModes(
    /* [out, callee] */ ArrayOf<String>** modes)
{
    VALIDATE_NOT_NULL(modes);

    String str;
    Get(KEY_FOCUS_MODE + SUPPORTED_VALUES_SUFFIX, &str);
    AutoPtr<ArrayOf<String> > array = Split(str);
    *modes = array;
    REFCOUNT_ADD(*modes);

    return NOERROR;
}

ECode HardwareCamera::Parameters::GetFocalLength(
    /* [out] */ Float* len)
{
    VALIDATE_NOT_NULL(len);

    String f;
    Get(KEY_FOCAL_LENGTH, &f);
    *len = StringUtils::ParseFloat(f);
    // return Float.parseFloat(Get(KEY_FOCAL_LENGTH));
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetHorizontalViewAngle(
    /* [out] */ Float* angle)
{
    VALIDATE_NOT_NULL(angle);

    String f;
    Get(KEY_HORIZONTAL_VIEW_ANGLE, &f);
    *angle = StringUtils::ParseDouble(f);
    // return Float.parseFloat(get(KEY_HORIZONTAL_VIEW_ANGLE));
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetVerticalViewAngle(
    /* [out] */ Float* angle)
{
    VALIDATE_NOT_NULL(angle);

    String f;
    Get(KEY_VERTICAL_VIEW_ANGLE, &f);
    *angle = StringUtils::ParseFloat(f);

    // return Float.parseFloat(get(KEY_VERTICAL_VIEW_ANGLE));
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetExposureCompensation(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = GetInt(KEY_EXPOSURE_COMPENSATION, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetExposureCompensation(
    /* [out] */ Int32 value)
{
    return Set(KEY_EXPOSURE_COMPENSATION, value);
}

ECode HardwareCamera::Parameters::GetMaxExposureCompensation(
    /* [out] */ Int32* compensation)
{
    VALIDATE_NOT_NULL(compensation);

    *compensation = GetInt(KEY_MAX_EXPOSURE_COMPENSATION, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMinExposureCompensation(
    /* [out] */ Int32* compensation)
{
    VALIDATE_NOT_NULL(compensation);

    *compensation = GetInt(KEY_MIN_EXPOSURE_COMPENSATION, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetExposureCompensationStep(
    /* [out] */ Float* step)
{
    VALIDATE_NOT_NULL(step);

    *step = GetFloat(KEY_EXPOSURE_COMPENSATION_STEP, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetAutoExposureLock(
    /* [in] */ Boolean toggle)
{
    return Set(KEY_AUTO_EXPOSURE_LOCK, toggle ? TRUESTR : FALSESTR);
}

ECode HardwareCamera::Parameters::GetAutoExposureLock(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    String str;
    Get(KEY_AUTO_EXPOSURE_LOCK, &str);
    *state = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::IsAutoExposureLockSupported(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);

    String str;
    Get(KEY_AUTO_EXPOSURE_LOCK_SUPPORTED, &str);
    *state = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetAutoWhiteBalanceLock(
    /* [in] */ Boolean toggle)
{
    return Set(KEY_AUTO_WHITEBALANCE_LOCK, toggle ? TRUESTR : FALSESTR);
}

ECode HardwareCamera::Parameters::GetAutoWhiteBalanceLock(
    /* [out] */ Boolean* lock)
{
    VALIDATE_NOT_NULL(lock);

    String str;
    Get(KEY_AUTO_WHITEBALANCE_LOCK, &str);
    *lock = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::IsAutoWhiteBalanceLockSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    String str;
    Get(KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED, &str);
    *supported = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetZoom(
    /* [out] */ Int32* zoom)
{
    VALIDATE_NOT_NULL(zoom);

    *zoom = GetInt(KEY_ZOOM, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetZoom(
    /* [in] */ Int32 value)
{
    return Set(KEY_ZOOM, value);
}

ECode HardwareCamera::Parameters::IsZoomSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    String str;
    Get(KEY_ZOOM_SUPPORTED, &str);
    *supported = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMaxZoom(
    /* [out] */ Int32* zoom)
{
    VALIDATE_NOT_NULL(zoom);

    *zoom = GetInt(KEY_MAX_ZOOM, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetZoomRatios(
    /* [out, callee] */ ArrayOf<IInteger32*>** ratios)
{
    VALIDATE_NOT_NULL(ratios);

    String str;
    Get(KEY_ZOOM_RATIOS, &str);
    AutoPtr<ArrayOf<IInteger32*> > array = SplitInt(str);
    *ratios = array;
    REFCOUNT_ADD(*ratios);

    return NOERROR;
}

ECode HardwareCamera::Parameters::IsSmoothZoomSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    String str;
    Get(KEY_SMOOTH_ZOOM_SUPPORTED, &str);

    *supported = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetFocusDistances(
    /* [in] */ ArrayOf<Float>* output)
{
    if (output == NULL || output->GetLength() != 3) {
        // throw new IllegalArgumentException(
        //         "output must be a Float array with three elements.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String str;
    Get(KEY_FOCUS_DISTANCES, &str);
    SplitFloat(str, output);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMaxNumFocusAreas(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = GetInt(KEY_MAX_NUM_FOCUS_AREAS, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetFocusAreas(
    /* [out, callee] */ ArrayOf<ICameraArea*>** areas)
{
    VALIDATE_NOT_NULL(areas);

    String str;
    Get(KEY_FOCUS_AREAS, &str);
    AutoPtr<ArrayOf<ICameraArea*> > array = SplitArea(str);
    *areas = array;
    REFCOUNT_ADD(array);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetFocusAreas(
    /* [in] */ ArrayOf<ICameraArea*>* focusAreas)
{
    Set(KEY_FOCUS_AREAS, focusAreas);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMaxNumMeteringAreas(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = GetInt(KEY_MAX_NUM_METERING_AREAS, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMeteringAreas(
    /* [out, callee] */ ArrayOf<ICameraArea*>** areas)
{
    VALIDATE_NOT_NULL(areas);

    String str;
    Get(KEY_METERING_AREAS, &str);
    AutoPtr<ArrayOf<ICameraArea*> > array = SplitArea(str);
    *areas = array;
    REFCOUNT_ADD(*areas);

    return NOERROR;
}

ECode HardwareCamera::Parameters::SetMeteringAreas(
    /* [in] */ ArrayOf<ICameraArea*>* meteringAreas)
{
    Set(KEY_METERING_AREAS, meteringAreas);
    return NOERROR;
}

ECode HardwareCamera::Parameters::GetMaxNumDetectedFaces(
    /* [out] */ Int32* faces)
{
    VALIDATE_NOT_NULL(faces);

    *faces = GetInt(KEY_MAX_NUM_DETECTED_FACES_HW, 0);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetRecordingHint(
    /* [in] */ Boolean hint)
{
    return Set(KEY_RECORDING_HINT, hint ? TRUESTR : FALSESTR);
}

ECode HardwareCamera::Parameters::IsVideoSnapshotSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    String str;
    Get(KEY_VIDEO_SNAPSHOT_SUPPORTED, &str);
    *supported = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::SetVideoStabilization(
    /* [in] */ Boolean toggle)
{
    return Set(KEY_VIDEO_STABILIZATION, toggle ? TRUESTR : FALSESTR);
}

ECode HardwareCamera::Parameters::GetVideoStabilization(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    String str;
    Get(KEY_VIDEO_STABILIZATION, &str);
    *value = TRUESTR.Equals(str);
    return NOERROR;
}

ECode HardwareCamera::Parameters::IsVideoStabilizationSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    String str;
    Get(KEY_VIDEO_STABILIZATION_SUPPORTED, &str);
    *supported = TRUESTR.Equals(str);
    return NOERROR;
}

AutoPtr<ArrayOf<String> > HardwareCamera::Parameters::Split(
    /* [in] */ const String& str)
{
    if (str.IsNull()) return NULL;

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    // CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    // splitter->SetString(str);

    // Boolean has = FALSE;
    // Int32 count = 0;
    // while(splitter->HasNext(&has), has) {
    //     count ++;
    // }

    // if (count > 0) {
    //     AutoPtr<ArrayOf<String> > substrings = ArrayOf<String>::Alloc(count);
    //     has = FALSE;
    //     Int32 index = 0;
    //     while(splitter->HasNext(&has), has) {
    //         AutoPtr<ICharSequence> cs;
    //         splitter->Next((IInterface**)&cs);
    //         String s;
    //         cs->ToString(&s);

    //         (*substrings)[index++] = s;
    //         assert(index > count);
    //     }

    //     return substrings;
    // }

    return NULL;
}

AutoPtr<ArrayOf<IInteger32*> > HardwareCamera::Parameters::SplitInt(
    /* [in] */ const String& str)
{
    if (str.IsNull()) return NULL;

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    // CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    // splitter->SetString(str);

    // Boolean has = FALSE;
    // Int32 count = 0;
    // while(splitter->HasNext(&has), has) {
    //     count ++;
    // }

    // if (count > 0) {
    //     AutoPtr<ArrayOf<IInteger32*> > substrings = ArrayOf<IInteger32*>::Alloc(count);
    //     has = FALSE;
    //     Int32 index = 0;
    //     while(splitter->HasNext(&has), has) {
    //         AutoPtr<ICharSequence> cs;
    //         splitter->Next((IInterface**)&cs);
    //         String s;
    //         cs->ToString(&s);

    //         AutoPtr<IInteger32> obj;
    //         CInteger32::New(StringUtils::ParseInt32(s), (IInteger32**)&obj);
    //         substrings->Set(index++, obj);
    //         assert(index > count);
    //     }

    //     return substrings;
    // }

    return NULL;
}

void HardwareCamera::Parameters::SplitInt(
    /* [in] */ const String& str,
    /* [in] */ ArrayOf<Int32>* output)
{
    if (str.IsNull()) return;

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    // CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    // splitter->SetString(str);

    // Int32 index = 0;
    // Boolean has = FALSE;
    // while(splitter->HasNext(&has), has) {
    //     AutoPtr<ICharSequence> cs;
    //     splitter->Next((IInterface**)&cs);
    //     String s;
    //     cs->ToString(&s);

    //     (*output)[index++] = StringUtils::ParseInt32(s);
    // }
}

void HardwareCamera::Parameters::SplitFloat(
    /* [in] */ const String& str,
    /* [in] */ ArrayOf<Float>* output)
{
    if (str.IsNull()) return;

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    // CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    // splitter->SetString(str);

    // Boolean has = FALSE;
    // Int32 index = 0;
    // while(splitter->HasNext(&has), has) {
    //     AutoPtr<ICharSequence> cs;
    //     splitter->Next((IInterface**)&cs);
    //     String s;
    //     cs->ToString(&s);

    //     (*output)[index++] = StringUtils::ParseFloat(s);
    // }
}

Float HardwareCamera::Parameters::GetFloat(
    /* [in] */ const String& key,
    /* [in] */ Float defaultValue)
{
    // try {
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mMap->Get(ko, (IInterface**)&obj);
    return StringUtils::ParseFloat(Object::ToString(obj), defaultValue);
    // } catch (NumberFormatException ex) {
    //     return defaultValue;
    // }
}

Int32 HardwareCamera::Parameters::GetInt(
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue)
{
    // try {
    AutoPtr<ICharSequence> ko = CoreUtils::Convert(key);
    AutoPtr<IInterface> obj;
    mMap->Get(ko, (IInterface**)&obj);
    return StringUtils::ParseInt32(Object::ToString(obj), 10, defaultValue);
    // } catch (NumberFormatException ex) {
    //     return defaultValue;
    // }
}

AutoPtr<ArrayOf<ICameraSize*> > HardwareCamera::Parameters::SplitSize(
    /* [in] */ const String& str)
{
    if (str.IsNull()) return NULL;

    AutoPtr<ISimpleStringSplitter> splitter;
    assert(0 && "TODO: CSimpleStringSplitter");
    // CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    // splitter->SetString(str);

    // Boolean has = FALSE;
    // Int32 count = 0;
    // while(splitter->HasNext(&has), has) {
    //     count ++;
    // }

    // if (count > 0) {
    //     AutoPtr<ArrayOf<ICameraSize*> > sizeList = ArrayOf<ICameraSize*>::Alloc(count);

    //     has = FALSE;
    //     Int32 index = 0;
    //     while(splitter->HasNext(&has), has) {
    //         AutoPtr<ICharSequence> cs;
    //         splitter->Next((IInterface**)&cs);
    //         String s;
    //         cs->ToString(&s);

    //         AutoPtr<ICameraSize> size = StrToSize(s);
    //         if (size != NULL) {
    //             sizeList->Set(index++, size);
    //         }
    //     }
    // }

    return NULL;
}

AutoPtr<ICameraSize> HardwareCamera::Parameters::StrToSize(
    /* [in] */ const String& str)
{
    if (str.IsNull()) return NULL;

    Int32 pos = str.IndexOf('x');
    if (pos != -1) {
        String width = str.Substring(0, pos);
        String height = str.Substring(pos + 1);
        return new Size(
            StringUtils::ParseInt32(width), StringUtils::ParseInt32(height));
    }

    // Log.e(TAG, "Invalid size parameter string=" + str);
    return NULL;
}

AutoPtr<IList> HardwareCamera::Parameters::SplitRange(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty() || str.GetChar(0) != '('
            || str.GetChar(str.GetLength() - 1) != ')') {
        // Log.e(TAG, "Invalid range list string=" + str);
        return NULL;
    }

    AutoPtr<IList> rangeList;
    CArrayList::New((IList**)&rangeList);
    Int32 endIndex = 0, fromIndex = 1;
    do {
        AutoPtr<IArrayOf> range;
        CArrayOf::New(EIID_IInteger32, 2, (IArrayOf**)&range);
        endIndex = str.IndexOf("),(", fromIndex);
        if (endIndex == -1) endIndex = str.GetLength() - 1;
        AutoPtr<ArrayOf<Int32> > output = ArrayOf<Int32>::Alloc(2);
        SplitInt(str.Substring(fromIndex, endIndex), output);
        AutoPtr<IInteger32> value1;
        CInteger32::New((*output)[0], (IInteger32**)&value1);
        range->Set(0, value1);
        AutoPtr<IInteger32> value2;
        CInteger32::New((*output)[1], (IInteger32**)&value2);
        range->Set(1, value2);
        rangeList->Add(TO_IINTERFACE(range));
        fromIndex = endIndex + 3;
    } while (endIndex != (str.GetLength() - 1));

    Int32 count = 0;
    rangeList->GetSize(&count);
    if (count == 0) return NULL;
    return rangeList;

}

AutoPtr<ArrayOf<ICameraArea*> > HardwareCamera::Parameters::SplitArea(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty() || str.GetChar(0) != '('
            || str.GetChar(str.GetLength() - 1) != ')') {
        // Log.e(TAG, "Invalid area string=" + str);
        return NULL;
    }

    List<AutoPtr<Area> > result/* = new ArrayList<Area>()*/;
    Int32 endIndex = 0, fromIndex = 1;
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(5);

    do {
        endIndex = str.IndexOf("),(", fromIndex);
        if (endIndex == -1) endIndex = str.GetLength() - 1;
        SplitInt(str.Substring(fromIndex, endIndex), array);

        AutoPtr<IRect> rect;
        CRect::New((*array)[0], (*array)[1], (*array)[2], (*array)[3], (IRect**)&rect);
        AutoPtr<Area> area = new Area(rect, (*array)[4]);
        result.PushBack(area);
        fromIndex = endIndex + 3;
    } while (endIndex != (str.GetLength() - 1));

    if (result.IsEmpty()) return NULL;

    if (result.GetSize() == 1) {
        AutoPtr<Area> area = result[0];
        AutoPtr<IRect> _rect = area->mRect;
        CRect* rect = (CRect*)_rect.Get();
        if (rect->mLeft == 0 && rect->mTop == 0 && rect->mRight == 0
                && rect->mBottom == 0 && area->mWeight == 0) {
            return NULL;
        }
    }

    AutoPtr<ArrayOf<ICameraArea*> > retArray = ArrayOf<ICameraArea*>::Alloc(result.GetSize());
    List<AutoPtr<Area> >::Iterator ator = result.Begin();
    Int32 index = 0;
    for (; ator != result.End(); ++ator) {
        retArray->Set(index, (*ator).Get());
    }

    return retArray;
}

Boolean HardwareCamera::Parameters::Same(
    /* [in] */ const String& s1,
    /* [in] */ const String& s2)
{
    if (s1.IsNull() && s2.IsNull()) return TRUE;
    if (!s1.IsNull() && s1.Equals(s2)) return TRUE;
    return FALSE;
}

CAR_INTERFACE_IMPL(HardwareCamera, Object, IHardwareCamera);

HardwareCamera::HardwareCamera()
    : mNativeContext(0)
    , mOneShot(FALSE)
    , mWithBuffer(FALSE)
    , mFaceDetectionRunning(FALSE)
{
}

HardwareCamera::~HardwareCamera()
{
    ReleaseResources();
}

ECode HardwareCamera::ReleaseResources()
{
    native_release();
    mFaceDetectionRunning = FALSE;
    return NOERROR;
}

ECode HardwareCamera::constructor()
{
    return NOERROR;
}

ECode HardwareCamera::constructor(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion)
{
    Int32 err = CameraInitVersion(cameraId, halVersion);
    if (CheckInitErrors(err)) {
        switch(err) {
            case HardwareCamera_EACCESS:
                //throw new RuntimeException("Fail to connect to camera service");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_ENODEV:
                //throw new RuntimeException("Camera initialization failed");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_ENOSYS:
                //throw new RuntimeException("Camera initialization failed because some methods"
                //        + " are not implemented");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_EOPNOTSUPP:
                //throw new RuntimeException("Camera initialization failed because the hal"
                //        + " version is not supported by this device");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_EINVAL:
                //throw new RuntimeException("Camera initialization failed because the input"
                //        + " arugments are invalid");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_EBUSY:
                //throw new RuntimeException("Camera initialization failed because the camera"
                //        + " device was already opened");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_EUSERS:
                //throw new RuntimeException("Camera initialization failed because the max"
                //        + " number of camera devices were already opened");
                return E_RUNTIME_EXCEPTION;
            default:
                // Should never hit this.
                //throw new RuntimeException("Unknown camera error");
                return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode HardwareCamera::constructor(
    /* [in] */ Int32 cameraId)
{
    Int32 err = CameraInitNormal(cameraId);
    if (CheckInitErrors(err)) {
        switch(err) {
            case HardwareCamera_EACCESS:
                //throw new RuntimeException("Fail to connect to camera service");
                return E_RUNTIME_EXCEPTION;
            case HardwareCamera_ENODEV:
                //throw new RuntimeException("Camera initialization failed");
                return E_RUNTIME_EXCEPTION;
            default:
                // Should never hit this.
                //throw new RuntimeException("Unknown camera error");
                return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

Int32 HardwareCamera::CameraInitVersion(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion)
{
    mShutterCallback = NULL;
    mRawImageCallback = NULL;
    mJpegCallback = NULL;
    mPreviewCallback = NULL;
    mPostviewCallback = NULL;
    mUsingPreviewAllocation = FALSE;
    mZoomListener = NULL;

    AutoPtr<ILooper> looper;
    if ((looper = Looper::GetMyLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    } else if ((looper = Looper::GetMainLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    } else {
        mEventHandler = NULL;
    }

    assert(0 && "TODO: CActivityThread");
    String packageName;// = CActivityThread::GetCurrentPackageName();

    Int32 result;
    native_setup(this, cameraId, halVersion, packageName, &result);
    return result;
}

Int32 HardwareCamera::CameraInitNormal(
    /* [in] */ Int32 cameraId)
{
    return CameraInitVersion(cameraId, CAMERA_HAL_API_VERSION_NORMAL_CONNECT);
}

ECode HardwareCamera::OpenLegacy(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion,
    /* [out] */ IHardwareCamera** camera)
{
    if (halVersion < IHardwareCamera::CAMERA_HAL_API_VERSION_1_0) {
        //throw new IllegalArgumentException("Invalid HAL version " + halVersion);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHardwareCamera> _camera = new HardwareCamera();
    ((HardwareCamera*)_camera.Get())->constructor(cameraId, halVersion);
    *camera = _camera;
    REFCOUNT_ADD(*camera);
    return NOERROR;
}

ECode HardwareCamera::CameraInitUnspecified(
    /* [in] */ Int32 cameraId,
    /* [out] */  Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = CameraInitVersion(cameraId, CAMERA_HAL_API_VERSION_UNSPECIFIED);
    return NOERROR;
}

Boolean HardwareCamera::CheckInitErrors(
    /* [in] */ Int32 err)
{
    return err != HardwareCamera_NO_ERROR;
}

AutoPtr<IHardwareCamera> HardwareCamera::OpenUninitialized()
{
    AutoPtr<IHardwareCamera> camera = new HardwareCamera();
    return camera;
}

ECode HardwareCamera::GetNumberOfCameras(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    *num = android::Camera::getNumberOfCameras();
    return NOERROR;
}

ECode HardwareCamera::Open(
    /* [in] */ Int32 cameraId,
    /* [out] */ IHardwareCamera** camera)
{
    AutoPtr<IHardwareCamera> _camera = new HardwareCamera();
    ((HardwareCamera*)_camera.Get())->constructor(cameraId);
    *camera = _camera;
    REFCOUNT_ADD(*camera);
    return NOERROR;
}

ECode HardwareCamera::Open(
    /* [out] */ IHardwareCamera** camera)
{
    VALIDATE_NOT_NULL(camera);
    *camera = NULL;

    Int32 numberOfCameras = 0;
    GetNumberOfCameras(&numberOfCameras);
    AutoPtr<IHardwareCameraInfo> cameraInfo = new HardwareCamera::CameraInfo();
    for (Int32 i = 0; i < numberOfCameras; i++) {
        GetCameraInfo(i, cameraInfo);
        if (((HardwareCamera::CameraInfo*)cameraInfo.Get())->mFacing == IHardwareCameraInfo::CAMERA_FACING_BACK) {
            AutoPtr<IHardwareCamera> _camera = new HardwareCamera();
            ((HardwareCamera*)_camera.Get())->constructor(i);
            *camera = _camera;
            REFCOUNT_ADD(*camera);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode HardwareCamera::GetCameraInfo(
    /* [in] */ Int32 cameraId,
    /* [in] */ IHardwareCameraInfo* cameraInfo)
{
    FAIL_RETURN(_getCameraInfo(cameraId, cameraInfo))

    AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    AutoPtr<IIAudioService> audioService = IIAudioService::Probe(tmpObj);

    // try {
    Boolean state = FALSE;
    if (audioService->IsCameraSoundForced(&state), state) {
        // Only set this when sound is forced; otherwise let native code
        // decide.
        ((HardwareCamera::CameraInfo*)cameraInfo)->mCanDisableShutterSound = FALSE;
    }
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Audio service is unavailable for queries");
    //}
    return NOERROR;
}

ECode HardwareCamera::_getCameraInfo(
    /* [in] */ Int32 cameraId,
    /* [in] */ IHardwareCameraInfo* info_obj)
{
    android::CameraInfo cameraInfo;
    android::status_t rc = android::Camera::getCameraInfo(cameraId, &cameraInfo);
    if (rc != HardwareCamera_NO_ERROR) {
        // jniThrowRuntimeException(env, "Fail to get camera info");
        return E_RUNTIME_EXCEPTION;
    }

    // env->SetIntField(info_obj, fields.facing, cameraInfo.facing);
    // env->SetIntField(info_obj, fields.orientation, cameraInfo.orientation);
    ((HardwareCamera::CameraInfo*)info_obj)->mFacing = cameraInfo.facing;
    ((HardwareCamera::CameraInfo*)info_obj)->mOrientation = cameraInfo.orientation;

    assert(0 && "TODO: PROPERTY_VALUE_MAX");
    // char value[PROPERTY_VALUE_MAX];
    // property_get("ro.camera.sound.forced", value, "0");
    // Boolean canDisableShutterSound = (strncmp(value, "0", 2) == 0);
    // env->SetBooleanField(info_obj, fields.canDisableShutterSound,
    //         canDisableShutterSound);
    // ((HardwareCamera::CameraInfo*)info_obj)->mCanDisableShutterSound = canDisableShutterSound;
    return NOERROR;
}

ECode HardwareCamera::SetPreviewDisplay(
    /* [in] */ ISurfaceHolder* holder)
{
    if (holder != NULL) {
        AutoPtr<ISurface> surface;
        holder->GetSurface((ISurface**)&surface);
        FAIL_RETURN(SetPreviewSurface(surface.Get()))
    } else {
        FAIL_RETURN(SetPreviewSurface(NULL))
    }

    return NOERROR;
}

ECode HardwareCamera::SetPreviewSurface(
    /* [in] */ ISurface* _surface)
{
    Slogger::V(TAG, "setPreviewSurface");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    android::sp<android::IGraphicBufferProducer> gbp;
    android::sp<android::Surface> surface;
    assert(0 && "TODO: android_view_Surface_getSurface");
    //surface = android_view_Surface_getSurface(env, _surface);
    // if (surface != NULL) {
    //     gbp = surface->getIGraphicBufferProducer();
    // }

    if (camera->setPreviewTarget(gbp) != HardwareCamera_NO_ERROR) {
        // jniThrowException(env, "java/io/IOException", "setPreviewDisplay failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::StopPreview()
{
    _stopPreview();
    mFaceDetectionRunning = FALSE;

    mShutterCallback = NULL;
    mRawImageCallback = NULL;
    mPostviewCallback = NULL;
    mJpegCallback = NULL;

    synchronized(mAutoFocusCallbackLock) {
        mAutoFocusCallback = NULL;
    }

    mAutoFocusMoveCallback = NULL;
    return NOERROR;
}

ECode HardwareCamera::SetPreviewCallback(
    /* [in] */ IPreviewCallback* cb)
{
    mPreviewCallback = cb;
    mOneShot = FALSE;
    mWithBuffer = FALSE;
    if (cb != NULL) {
        mUsingPreviewAllocation = FALSE;
    }
    // Always use one-shot mode. We fake camera preview mode by
    // doing one-shot preview continuously.
    setHasPreviewCallback(cb != NULL, FALSE);
    return NOERROR;
}

ECode HardwareCamera::SetOneShotPreviewCallback(
    /* [in] */ IPreviewCallback* cb)
{
    mPreviewCallback = cb;
    mOneShot = TRUE;
    mWithBuffer = FALSE;
    if (cb != NULL) {
        mUsingPreviewAllocation = FALSE;
    }
    setHasPreviewCallback(cb != NULL, FALSE);
    return NOERROR;
}

ECode HardwareCamera::SetPreviewCallbackWithBuffer(
    /* [in] */ IPreviewCallback* cb)
{
    mPreviewCallback = cb;
    mOneShot = FALSE;
    mWithBuffer = TRUE;
    if (cb != NULL) {
        mUsingPreviewAllocation = FALSE;
    }
    setHasPreviewCallback(cb != NULL, TRUE);
    return NOERROR;
}

ECode HardwareCamera::AddCallbackBuffer(
    /* [in] */ ArrayOf<Byte>* callbackBuffer)
{
    _addCallbackBuffer(callbackBuffer, CAMERA_MSG_PREVIEW_FRAME);
    return NOERROR;
}

ECode HardwareCamera::AddRawImageCallbackBuffer(
    /* [in] */ ArrayOf<Byte>* callbackBuffer)
{
    return AddCallbackBuffer(callbackBuffer, CAMERA_MSG_RAW_IMAGE);
}

ECode HardwareCamera::AddCallbackBuffer(
    /* [in] */ ArrayOf<Byte>* callbackBuffer,
    /* [in] */ Int32 msgType)
{
    // CAMERA_MSG_VIDEO_FRAME may be allowed in the future.
    if (msgType != CAMERA_MSG_PREVIEW_FRAME &&
        msgType != CAMERA_MSG_RAW_IMAGE) {
        // throw new IllegalArgumentException(
                        // "Unsupported message type: " + msgType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    _addCallbackBuffer(callbackBuffer, msgType);
    return NOERROR;
}

void HardwareCamera::PostEventFromNative(
    /* [in] */ HardwareCamera* camera_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ const void* obj)
{
    HardwareCamera* c = camera_ref;
    if (c == NULL)
        return;

    if (c->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        assert(0 && "TODO : (IInterface*)obj");
        //c->mEventHandler->ObtainMessage(what, arg1, arg2, (IInterface*)obj, (IMessage**)&m);
        Boolean result;
        c->mEventHandler->SendMessage(m, &result);
    }
}

ECode HardwareCamera::AutoFocus(
    /* [in] */ IAutoFocusCallback* cb)
{
    synchronized(mAutoFocusCallbackLock) {
        mAutoFocusCallback = cb;
    }
    native_autoFocus();
    return NOERROR;
}

ECode HardwareCamera::CancelAutoFocus()
{
    synchronized(mAutoFocusCallbackLock) {
        mAutoFocusCallback = NULL;
    }
    native_cancelAutoFocus();
    // CAMERA_MSG_FOCUS should be removed here because the following
    // scenario can happen:
    // - An application uses the same thread for autoFocus, cancelAutoFocus
    //   and looper thread.
    // - The application calls autoFocus.
    // - HAL sends CAMERA_MSG_FOCUS, which enters the looper message queue.
    //   Before event handler's handleMessage() is invoked, the application
    //   calls cancelAutoFocus and autoFocus.
    // - The application gets the old CAMERA_MSG_FOCUS and thinks autofocus
    //   has been completed. But in fact it is not.
    //
    // As documented in the beginning of the file, apps should not use
    // multiple threads to call autoFocus and cancelAutoFocus at the same
    // time. It is HAL's responsibility not to send a CAMERA_MSG_FOCUS
    // message after native_cancelAutoFocus is called.
    return mEventHandler->RemoveMessages(CAMERA_MSG_FOCUS);
}

ECode HardwareCamera::SetAutoFocusMoveCallback(
    /* [in] */ IAutoFocusMoveCallback* cb)
{
    mAutoFocusMoveCallback = cb;
    return enableFocusMoveCallback((mAutoFocusMoveCallback != NULL) ? 1 : 0);
}

ECode HardwareCamera::TakePicture(
    /* [in] */ IShutterCallback* shutter,
    /* [in] */ IPictureCallback* raw,
    /* [in] */ IPictureCallback* jpeg)
{
    return TakePicture(shutter, raw, NULL, jpeg);
}

ECode HardwareCamera::TakePicture(
    /* [in] */ IShutterCallback* shutter,
    /* [in] */ IPictureCallback* raw,
    /* [in] */ IPictureCallback* postview,
    /* [in] */ IPictureCallback* jpeg)
{
    mShutterCallback = shutter;
    mRawImageCallback = raw;
    mPostviewCallback = postview;
    mJpegCallback = jpeg;

    // If callback is not set, do not send me callbacks.
    Int32 msgType = 0;
    if (mShutterCallback != NULL) {
        msgType |= CAMERA_MSG_SHUTTER;
    }
    if (mRawImageCallback != NULL) {
        msgType |= CAMERA_MSG_RAW_IMAGE;
    }
    if (mPostviewCallback != NULL) {
        msgType |= CAMERA_MSG_POSTVIEW_FRAME;
    }
    if (mJpegCallback != NULL) {
        msgType |= CAMERA_MSG_COMPRESSED_IMAGE;
    }

    FAIL_RETURN(native_takePicture(msgType))
    mFaceDetectionRunning = FALSE;
    return NOERROR;
}

ECode HardwareCamera::EnableShutterSound(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success);

    if (!enabled) {
        AutoPtr<IInterface> tmpObj = ServiceManager::GetService(IContext::AUDIO_SERVICE);
        AutoPtr<IIAudioService> audioService = IIAudioService::Probe(tmpObj.Get());

        // try {
        Boolean state = FALSE;
        if (audioService->IsCameraSoundForced(&state), state) {
            *success = FALSE;
            return NOERROR;
        }
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Audio service is unavailable for queries");
        // }
    }

    return _enableShutterSound(enabled, success);
}

ECode HardwareCamera::DisableShutterSound(
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success);

    return _enableShutterSound(/*enabled*/FALSE, success);
}

ECode HardwareCamera::SetZoomChangeListener(
    /* [in] */ IOnZoomChangeListener* listener)
{
    mZoomListener = listener;
    return NOERROR;
}

ECode HardwareCamera::SetFaceDetectionListener(
    /* [in] */ IFaceDetectionListener* listener)
{
    mFaceListener = listener;
    return NOERROR;
}

ECode HardwareCamera::StartFaceDetection()
{
    if (mFaceDetectionRunning) {
        // throw new RuntimeException("Face detection is already running");
        return E_RUNTIME_EXCEPTION;
    }
    FAIL_RETURN(_startFaceDetection(CAMERA_FACE_DETECTION_HW))
    mFaceDetectionRunning = TRUE;

    return NOERROR;
}

ECode HardwareCamera::StopFaceDetection()
{
    FAIL_RETURN(_stopFaceDetection())
    mFaceDetectionRunning = FALSE;
    return NOERROR;
}

ECode HardwareCamera::SetErrorCallback(
    /* [in] */ ICameraErrorCallback* cb)
{
    mErrorCallback = cb;
    return NOERROR;
}

ECode HardwareCamera::SetParameters(
    /* [in] */ IParameters* params)
{
    // If using preview allocations, don't allow preview size changes
    if (mUsingPreviewAllocation) {
        AutoPtr<ICameraSize> newPreviewSize;
        FAIL_RETURN(params->GetPreviewSize((ICameraSize**)&newPreviewSize))
        AutoPtr<ICameraSize> currentPreviewSize;
        AutoPtr<IParameters> para;
        FAIL_RETURN(GetParameters((IParameters**)&para))
        FAIL_RETURN(para->GetPreviewSize((ICameraSize**)&currentPreviewSize))
        if (((Size*)newPreviewSize.Get())->mWidth != ((Size*)currentPreviewSize.Get())->mWidth ||
                ((Size*)newPreviewSize.Get())->mHeight != ((Size*)currentPreviewSize.Get())->mHeight) {
            //throw new IllegalStateException("Cannot change preview size" +
            //        " while a preview allocation is configured.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    String str;
    FAIL_RETURN(params->Flatten(&str))
    native_setParameters(str);
    return NOERROR;
}

ECode HardwareCamera::GetParameters(
    /* [out] */ IParameters** para)
{
    VALIDATE_NOT_NULL(para);

    *para = new HardwareCamera::Parameters(this);
    REFCOUNT_ADD(*para);

    String s;
    native_getParameters(&s);
    (*para)->Unflatten(s);
    return NOERROR;
}

ECode HardwareCamera::GetEmptyParameters(
    /* [out] */ IParameters** para)
{
    VALIDATE_NOT_NULL(para);

    AutoPtr<IHardwareCamera> camera = new HardwareCamera();
    AutoPtr<IParameters> p = new HardwareCamera::Parameters(camera);
    *para = p;
    REFCOUNT_ADD(*para);
    return NOERROR;
}

ECode HardwareCamera::GetParametersCopy(
    /* [in] */ IParameters* parameters,
    /* [out] */ IParameters** para)
{
    VALIDATE_NOT_NULL(para);

    if (parameters == NULL) {
        //throw new NullPointerException("parameters must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IHardwareCamera> camera = ((Parameters*)parameters)->GetOuter();
    AutoPtr<IParameters> p = new HardwareCamera::Parameters(camera);
    ((Parameters*)p.Get())->CopyFrom(parameters);

    *para = p;
    REFCOUNT_ADD(*para);
    return NOERROR;
}

//Native method.
ECode HardwareCamera::Unlock()
{
    Slogger::V(TAG, "unlock");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->unlock() != NOERROR) {
        // jniThrowRuntimeException(env, "unlock failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::Lock()
{
    Slogger::V(TAG, "lock");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->lock() != NOERROR) {
        // jniThrowRuntimeException(env, "lock failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::Reconnect()
{
    Slogger::V(TAG, "reconnect");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->reconnect() != NOERROR) {
        // jniThrowException(env, "java/io/IOException", "reconnect failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::SetPreviewTexture(
    /* [in] */ ISurfaceTexture* _surfaceTexture)
{
    Slogger::V(TAG, "setPreviewTexture");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    assert(0 && "TODO: SurfaceTexture_getProducer");
    android::sp<android::IGraphicBufferProducer> producer = NULL;
    // if (jSurfaceTexture != NULL) {
    //     producer = SurfaceTexture_getProducer(env, jSurfaceTexture);
    //     if (producer == NULL) {
    //         //jniThrowException(env, "java/lang/IllegalArgumentException",
    //         //        "SurfaceTexture already released in setPreviewTexture");
    //         return E_RUNTIME_EXCEPTION;
    //     }

    // }

    if (camera->setPreviewTarget(producer) != HardwareCamera_NO_ERROR) {
        //jniThrowException(env, "java/io/IOException",
        //        "setPreviewTexture failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::StartPreview()
{
    Slogger::V(TAG, "startPreview");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->startPreview() != NOERROR) {
        // jniThrowRuntimeException(env, "startPreview failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::PreviewEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);

    *enabled = FALSE;
    Slogger::V(TAG, "previewEnabled");
    android::sp<android::Camera> c = get_native_camera(this, NULL);
    if (c == 0) return NOERROR;

    *enabled = c->previewEnabled();
    return NOERROR;
}

ECode HardwareCamera::StartSmoothZoom(
    /* [in] */ Int32 value)
{
    Slogger::V(TAG, "startSmoothZoom");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    ECode rc = camera->sendCommand(CAMERA_CMD_START_SMOOTH_ZOOM, value, 0);
    if (rc == android::BAD_VALUE) {
        char msg[64];
        sprintf(msg, "invalid zoom value=%d", value);
        // jniThrowException(env, "java/lang/IllegalArgumentException", msg);
        return E_RUNTIME_EXCEPTION;
    }
    else if (rc != NOERROR) {
        // jniThrowRuntimeException(env, "start smooth zoom failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::StopSmoothZoom()
{
    Slogger::V(TAG, "stopSmoothZoom");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->sendCommand(CAMERA_CMD_STOP_SMOOTH_ZOOM, 0, 0) != NOERROR) {
        // jniThrowRuntimeException(env, "stop smooth zoom failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::SetDisplayOrientation(
    /* [in] */ Int32 degrees)
{
    Slogger::V(TAG, "setDisplayOrientation");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->sendCommand(CAMERA_CMD_SET_DISPLAY_ORIENTATION, degrees, 0) != NOERROR) {
        // jniThrowRuntimeException(env, "set display orientation failed");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode HardwareCamera::native_setup(
    /* [in] */ HardwareCamera* camera_this,
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 halVersion,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    // // Convert jstring to String16
    // const char16_t *rawClientName = env->GetStringChars(clientPackageName, NULL);
    // jsize rawClientNameLen = env->GetStringLength(clientPackageName);
    // String16 clientName(rawClientName, rawClientNameLen);
    // env->ReleaseStringChars(clientPackageName, rawClientName);
    android::String16 clientName = android::String16(packageName);

    android::sp<android::Camera> camera;
    if (halVersion == CAMERA_HAL_API_VERSION_NORMAL_CONNECT) {
        // Default path: hal version is don't care, do normal camera connect.
        camera = android::Camera::connect(cameraId, clientName,
                android::Camera::USE_CALLING_UID);
    } else {
        int status = android::Camera::connectLegacy(cameraId, halVersion, clientName,
                android::Camera::USE_CALLING_UID, camera);
        if (status != HardwareCamera_NO_ERROR) {
            *result = status;
            return NOERROR;
        }
    }

    if (camera == NULL) {
        *result = -HardwareCamera_EACCESS;
        return NOERROR;
    }

    // make sure camera hardware is alive
    if (camera->getStatus() != HardwareCamera_NO_ERROR) {
        *result = android::NO_INIT;
        return NOERROR;
    }

    // jclass clazz = env->GetObjectClass(thiz);
    // if (clazz == NULL) {
    //     // This should never happen
    //     //jniThrowRuntimeException(env, "Can't find android/hardware/Camera");
    //     *result = INVALID_OPERATION;
    //     return E_RUNTIME_EXCEPTION
    // }

    // We use a weak reference so the Camera object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    android::sp<JNICameraContext> context = new JNICameraContext(this, this, camera);
    assert(0 && "TODO: incStrong");
    //context->incStrong((void*)native_setup);
    camera->setListener(context);

    // save context in opaque field
    //env->SetIntField(thiz, fields.context, (int)context.get());
    mNativeContext = (Int64)context.get();
    return NOERROR;
}

void HardwareCamera::native_release()
{
    Slogger::V(TAG, "release camera");
    JNICameraContext* context = NULL;
    android::sp<android::Camera> camera;
    {
        AutoLock _l(sLock);
        context = reinterpret_cast<JNICameraContext*>(mNativeContext/*env->GetIntField(thiz, fields.context)*/);

        // Make sure we do not attempt to callback on a deleted Java object.
        //env->SetIntField(thiz, fields.context, 0);
        mNativeContext = 0;
    }

    // clean up if release has not been called before
    if (context != NULL) {
        camera = context->getCamera();
        context->release();
        Slogger::V(TAG, "native_release: context=%p camera=%p", context, camera.get());

        // clear callbacks
        if (camera != NULL) {
            camera->setPreviewCallbackFlags(CAMERA_FRAME_CALLBACK_FLAG_NOOP);
            camera->disconnect();
        }

        // remove context to prevent further Java access
        assert(0 && "TODO: decStrong");
        //context->decStrong((void*)native_setup);
    }
}

void HardwareCamera::_stopPreview()
{
    Slogger::V(TAG, "stopPreview");
    android::sp<android::Camera> c = get_native_camera(this, NULL);
    if (c == 0) return;

    c->stopPreview();
}

void HardwareCamera::setHasPreviewCallback(
    /* [in] */ Boolean installed,
    /* [in] */ Boolean manualBuffer)
{
    Slogger::V(TAG, "setHasPreviewCallback: installed:%d, manualBuffer:%d", (int)installed, (int)manualBuffer);
    // Important: Only install preview_callback if the Java code has called
    // setPreviewCallback() with a non-NULL value, otherwise we'd pay to memcpy
    // each preview frame for nothing.
    JNICameraContext* context;
    android::sp<android::Camera> camera = get_native_camera(this, &context);
    if (camera == 0) return;

    // setCallbackMode will take care of setting the context flags and calling
    // camera->setPreviewCallbackFlags within a mutex for us.
    context->setCallbackMode(installed, manualBuffer);
}

void HardwareCamera::_addCallbackBuffer(
    /* [in] */ ArrayOf<Byte>* callbackBuffer,
    /* [in] */ Int32 msgType)
{
    Slogger::V(TAG, "addCallbackBuffer: 0x%x", msgType);

    JNICameraContext* context = reinterpret_cast<JNICameraContext*>(mNativeContext/*env->GetIntField(thiz, fields.context)*/);

    if (context != NULL) {
        context->addCallbackBuffer(callbackBuffer, msgType);
    }
}

// ECode HardwareCamera::CreatePreviewAllocation(
//     /* [in] */ IRenderScript* rs,
//     /* [in] */ Int32 usage,
//     /* [out] */ IRenderScriptAllocation** result)
// {
//     Parameters p = getParameters();
//     Size previewSize = p.getPreviewSize();
//     Type.Builder yuvBuilder = new Type.Builder(rs,
//             Element.createPixel(rs,
//                     Element.DataType.UNSIGNED_8,
//                     Element.DataKind.PIXEL_YUV));
//     // Use YV12 for wide compatibility. Changing this requires also
//     // adjusting camera service's format selection.
//     yuvBuilder.setYuvFormat(ImageFormat.YV12);
//     yuvBuilder.setX(previewSize.width);
//     yuvBuilder.setY(previewSize.height);

//     Allocation a = Allocation.createTyped(rs, yuvBuilder.create(),
//             usage | Allocation.USAGE_IO_INPUT);

//     return a;
// }

// ECode HardwareCamera::SetPreviewCallbackAllocation(
//     /* [in] */ IRenderScriptAllocation* previewAllocation)
// {
//     Surface previewSurface = null;
//     if (previewAllocation != null) {
//          Parameters p = getParameters();
//          Size previewSize = p.getPreviewSize();
//          if (previewSize.width != previewAllocation.getType().getX() ||
//                  previewSize.height != previewAllocation.getType().getY()) {
//              throw new IllegalArgumentException(
//                  "Allocation dimensions don't match preview dimensions: " +
//                  "Allocation is " +
//                  previewAllocation.getType().getX() +
//                  ", " +
//                  previewAllocation.getType().getY() +
//                  ". Preview is " + previewSize.width + ", " +
//                  previewSize.height);
//          }
//          if ((previewAllocation.getUsage() &
//                          Allocation.USAGE_IO_INPUT) == 0) {
//              throw new IllegalArgumentException(
//                  "Allocation usage does not include USAGE_IO_INPUT");
//          }
//          if (previewAllocation.getType().getElement().getDataKind() !=
//                  Element.DataKind.PIXEL_YUV) {
//              throw new IllegalArgumentException(
//                  "Allocation is not of a YUV type");
//          }
//          previewSurface = previewAllocation.getSurface();
//          mUsingPreviewAllocation = true;
//      } else {
//          mUsingPreviewAllocation = false;
//      }
//      setPreviewCallbackSurface(previewSurface);
// }

ECode HardwareCamera::setPreviewCallbackSurface(
    /* [in] */ ISurface* s)
{
    Slogger::V(TAG, "setPreviewCallbackSurface");
    JNICameraContext* context;
    android::sp<android::Camera> camera = get_native_camera(this, &context);
    if (camera == 0) return NOERROR;

    android::sp<android::IGraphicBufferProducer> gbp;
    android::sp<android::Surface> surface;
    assert(0 && "TODO: android_view_Surface_getSurface");
    // if (jSurface) {
    //     surface = android_view_Surface_getSurface(env, jSurface);
    //     if (surface != NULL) {
    //         gbp = surface->getIGraphicBufferProducer();
    //     }
    // }
    // Clear out normal preview callbacks
    context->setCallbackMode(FALSE, FALSE);
    // Then set up callback surface
    if (camera->setPreviewCallbackTarget(gbp) != HardwareCamera_NO_ERROR) {
        //jniThrowException(env, "java/io/IOException", "setPreviewCallbackTarget failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::native_autoFocus()
{
    Slogger::V(TAG, "autoFocus");
    JNICameraContext* context;
    android::sp<android::Camera> c = get_native_camera(this, &context);
    if (c == 0) return NOERROR;

    if (c->autoFocus() != NOERROR) {
        // jniThrowRuntimeException(env, "autoFocus failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::native_cancelAutoFocus()
{
    Slogger::V(TAG, "cancelAutoFocus");
    JNICameraContext* context;
    android::sp<android::Camera> c = get_native_camera(this, &context);
    if (c == 0) return NOERROR;

    if (c->cancelAutoFocus() != NOERROR) {
        // jniThrowRuntimeException(env, "cancelAutoFocus failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::enableFocusMoveCallback(
    /* [in] */ Int32 enable)
{
    Slogger::V(TAG, "enableFocusMoveCallback");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->sendCommand(CAMERA_CMD_ENABLE_FOCUS_MOVE_MSG, enable, 0) != NOERROR) {
        // jniThrowRuntimeException(env, "enable focus move callback failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::native_takePicture(
    /* [in] */ Int32 msgType)
{
    Slogger::V(TAG, "takePicture");
    JNICameraContext* context;
    android::sp<android::Camera> camera = get_native_camera(this, &context);
    if (camera == 0) return NOERROR;

    /*
     * When CAMERA_MSG_RAW_IMAGE is requested, if the raw image callback
     * buffer is available, CAMERA_MSG_RAW_IMAGE is enabled to get the
     * notification _and_ the data; otherwise, CAMERA_MSG_RAW_IMAGE_NOTIFY
     * is enabled to receive the callback notification but no data.
     *
     * Note that CAMERA_MSG_RAW_IMAGE_NOTIFY is not exposed to the
     * Java application.
     */
    if (msgType & CAMERA_MSG_RAW_IMAGE) {
        Slogger::V(TAG, "Enable raw image callback buffer");
        if (!context->isRawImageCallbackBufferAvailable()) {
            // ALOGV("Enable raw image notification, since no callback buffer exists");
            msgType &= ~CAMERA_MSG_RAW_IMAGE;
            msgType |= CAMERA_MSG_RAW_IMAGE_NOTIFY;
        }
    }

    if (camera->takePicture(msgType) != NOERROR) {
        // jniThrowRuntimeException(env, "takePicture failed");
        return E_RUNTIME_EXCEPTION;;
    }
    return NOERROR;
}

ECode HardwareCamera::_enableShutterSound(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Slogger::V(TAG, "enableShutterSound");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) {
        *result = FALSE;
        return NOERROR;
    }

    int32_t value = (enabled == TRUE) ? 1 : 0;
    ECode rc = camera->sendCommand(CAMERA_CMD_ENABLE_SHUTTER_SOUND, value, 0);
    if (rc == NOERROR) {
        *result = TRUE;
        return NOERROR;
    } else if (rc == android::PERMISSION_DENIED) {
        *result = FALSE;
        return NOERROR;
    } else {
        // jniThrowRuntimeException(env, "enable shutter sound failed");
        *result = FALSE;
        return E_RUNTIME_EXCEPTION;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HardwareCamera::_startFaceDetection(
    /* [in] */ Int32 type)
{
    Slogger::V(TAG, "startFaceDetection");
    JNICameraContext* context;
    android::sp<android::Camera> camera = get_native_camera(this, &context);
    if (camera == 0) return NOERROR;

    ECode rc = camera->sendCommand(CAMERA_CMD_START_FACE_DETECTION, type, 0);
    if (rc == android::BAD_VALUE) {
        char msg[64];
        snprintf(msg, sizeof(msg), "invalid face detection type=%d", type);
        // jniThrowException(env, "java/lang/IllegalArgumentException", msg);
        return E_RUNTIME_EXCEPTION;
    } else if (rc != NOERROR) {
        // jniThrowRuntimeException(env, "start face detection failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::_stopFaceDetection()
{
    Slogger::V(TAG, "stopFaceDetection");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    if (camera->sendCommand(CAMERA_CMD_STOP_FACE_DETECTION, 0, 0) != NOERROR) {
        // jniThrowRuntimeException(env, "stop face detection failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode HardwareCamera::native_setParameters(
    /* [in] */ const String& params)
{
    Slogger::V(TAG, "setParameters");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    const char* str = (const char*)params/*env->GetStringCritical(params, 0)*/;
    android::String8 params8;
    if (params) {
        params8 = android::String8(str, params.GetByteLength()/*env->GetStringLength(params)*/);
        // env->ReleaseStringCritical(params, str);
    }
    if (camera->setParameters(params8) != NOERROR) {
        // jniThrowRuntimeException(env, "setParameters failed");
        return E_RUNTIME_EXCEPTION;;
    }
    return NOERROR;
}

ECode HardwareCamera::native_getParameters(
    /* [out] */ String* para)
{
    VALIDATE_NOT_NULL(para);

    Slogger::V(TAG, "getParameters");
    android::sp<android::Camera> camera = get_native_camera(this, NULL);
    if (camera == 0) return NOERROR;

    android::String8 params8 = camera->getParameters();
    if (params8.isEmpty()) {
        // jniThrowRuntimeException(env, "getParameters failed (empty parameters)");
        return E_RUNTIME_EXCEPTION;;
    }

    // return env->NewStringUTF(params8.string());
    *para = String(params8.string());
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
