#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/media/CImageReader.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <gui/CpuConsumer.h>
#include <gui/Surface.h>

using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::Looper;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::Utility::Logging::Logger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CImageReader::SurfaceImage::SurfacePlane, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

//================================================================================
//  CImageReader::ListenerHandler
//================================================================================

ECode CImageReader::ListenerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IImageReaderOnImageAvailableListener> listener;
    Object& lock = mHost->mListenerLock;
    synchronized(lock) {
        listener = mHost->mListener;
    }
    if (listener != NULL) {
        listener->OnImageAvailable(IImageReader::Probe(mHost));
    }
    return NOERROR;
}

//================================================================================
//  CImageReader::SurfaceImage::SurfacePlane
//================================================================================

CImageReader::SurfaceImage::SurfacePlane::SurfacePlane(
    /* [in] */ Int32 index,
    /* [in] */ Int32 rowStride,
    /* [in] */ Int32 pixelStride,
    /* [in] */ SurfaceImage* host)
    : mIndex(index)
    , mPixelStride(pixelStride)
    , mRowStride(rowStride)
    , mHost(host)
{
}

ECode CImageReader::SurfaceImage::SurfacePlane::GetBuffer(
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mHost->IsImageValid() == FALSE) {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mBuffer != NULL) {
        *result = mBuffer;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        mHost->NativeImageGetBuffer(mIndex,
                mHost->mHost->mFormat, (IByteBuffer**)&mBuffer);
        // Set the byteBuffer order according to host endianness (native order),
        // otherwise, the byteBuffer order defaults to ByteOrder.BIG_ENDIAN.
        AutoPtr<IByteOrderHelper> orderHelper;
        CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
        ByteOrder nativeOrder;
        orderHelper->GetNativeOrder(&nativeOrder);
        mBuffer->SetOrder(nativeOrder);
        *result = mBuffer;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CImageReader::SurfaceImage::SurfacePlane::GetPixelStride(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mHost->IsImageValid()) {
        *result = mPixelStride;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CImageReader::SurfaceImage::SurfacePlane::GetRowStride(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mHost->IsImageValid()) {
        *result = mRowStride;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

void CImageReader::SurfaceImage::SurfacePlane::ClearBuffer()
{
    mBuffer = NULL;
}

//================================================================================
//  CImageReader::SurfaceImage
//================================================================================

CAR_INTERFACE_IMPL(CImageReader::SurfaceImage, Image, IImageReaderSurfaceImage)

CImageReader::SurfaceImage::SurfaceImage(
    /* [in] */ CImageReader* host)
    : mIsImageValid(FALSE)
    , mHost(host)
{
}

ECode CImageReader::SurfaceImage::Close()
{
    if (mIsImageValid) {
        mHost->ReleaseImage(this);
    }
    return NOERROR;
}

ECode CImageReader::SurfaceImage::GetReader(
    /* [out] */ IImageReader** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CImageReader::SurfaceImage::GetFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mIsImageValid) {
        *result = mHost->mFormat;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CImageReader::SurfaceImage::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mIsImageValid) {
        *result = mHost->mWidth;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CImageReader::SurfaceImage::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mIsImageValid) {
        *result = mHost->mHeight;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CImageReader::SurfaceImage::GetTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mIsImageValid) {
        *result = mTimestamp;
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CImageReader::SurfaceImage::GetPlanes(
    /* [out, callee] */ ArrayOf<IImagePlane*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mIsImageValid) {
        // Shallow copy is fine.
        AutoPtr<ArrayOf<IImagePlane*> > array = ArrayOf<IImagePlane*>::Alloc(mPlanes->GetLength());
        for (Int32 i = 0; i < mPlanes->GetLength(); i++) {
            array->Set(i, IImagePlane::Probe((*mPlanes)[i]));
        }
        *result = array;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        // throw new IllegalStateException("Image is already released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

void CImageReader::SurfaceImage::SetImageValid(
    /* [in] */ Boolean isValid)
{
    mIsImageValid = isValid;
}

Boolean CImageReader::SurfaceImage::IsImageValid()
{
    return mIsImageValid;
}

void CImageReader::SurfaceImage::ClearSurfacePlanes()
{
    if (mIsImageValid) {
        for (Int32 i = 0; i < mPlanes->GetLength(); i++) {
            if ((*mPlanes)[i] != NULL) {
                (*mPlanes)[i]->ClearBuffer();
                (*mPlanes)[i] = NULL;
            }
        }
    }
}

void CImageReader::SurfaceImage::CreateSurfacePlanes()
{
    mPlanes = ArrayOf<SurfacePlane*>::Alloc(mHost->mNumPlanes);
    for (Int32 i = 0; i < mHost->mNumPlanes; i++) {
        AutoPtr<SurfacePlane> sp;
        NativeCreatePlane(i, mHost->mFormat, (SurfacePlane**)&sp);
        mPlanes->Set(i, sp);
    }
}

// TODO: Need JNI CODE
ECode CImageReader::SurfaceImage::NativeImageGetBuffer(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 readerFormat,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    uint8_t *base = NULL;
    uint32_t size = 0;
    AutoPtr<IByteBuffer> byteBuffer;

    ALOGV("%s: buffer index: %d", __FUNCTION__, idx);

    // CpuConsumer::LockedBuffer* buffer = Image_getLockedBuffer(env, thiz);

    // if (buffer == NULL) {
    //     // jniThrowException(env, "java/lang/IllegalStateException", "Image was released");
    //     return E_ILLEGAL_STATE_EXCEPTION;
    // }

    // readerFormat = Image_getPixelFormat(env, readerFormat);

    // // Create byteBuffer from native buffer
    // Image_getLockedBufferInfo(env, buffer, idx, &base, &size, readerFormat);

    // if (size > static_cast<uint32_t>(INT32_MAX)) {
    //     // Byte buffer have 'int capacity', so check the range
    //     jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
    //             "Size too large for bytebuffer capacity %" PRIu32, size);
    //     return NULL;
    // }

    // byteBuffer = env->NewDirectByteBuffer(base, size);
    // // TODO: throw dvm exOutOfMemoryError?
    // if ((byteBuffer == NULL) && (env->ExceptionCheck() == false)) {
    //     jniThrowException(env, "java/lang/IllegalStateException", "Failed to allocate ByteBuffer");
    // }

    // return byteBuffer;
    return NOERROR;
}

ECode CImageReader::SurfaceImage::NativeCreatePlane(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 readerFormat,
    /* [out] */ SurfacePlane** result)
{
    VALIDATE_NOT_NULL(result)
    // int rowStride, pixelStride;
    // ALOGV("%s: buffer index: %d", __FUNCTION__, idx);

    // CpuConsumer::LockedBuffer* buffer = Image_getLockedBuffer(env, thiz);

    // ALOG_ASSERT(buffer != NULL);
    // if (buffer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException", "Image was released");
    // }

    // readerFormat = Image_getPixelFormat(env, readerFormat);

    // rowStride = Image_imageGetRowStride(env, buffer, idx, readerFormat);
    // pixelStride = Image_imageGetPixelStride(env, buffer, idx, readerFormat);

    // jobject surfPlaneObj = env->NewObject(gSurfacePlaneClassInfo.clazz,
    //         gSurfacePlaneClassInfo.ctor, thiz, idx, rowStride, pixelStride);

    // return surfPlaneObj;
    return NOERROR;
}

//================================================================================
//  CImageReader
//================================================================================

const Int32 CImageReader::ACQUIRE_SUCCESS = 0;
const Int32 CImageReader::ACQUIRE_NO_BUFS = 1;
const Int32 CImageReader::ACQUIRE_MAX_IMAGES = 2;

CAR_INTERFACE_IMPL(CImageReader, Object, IImageReader)
// CAR_INTERFACE_IMPL_2(CImageReader, Object, IImageReader, IAutoCloseable)

CAR_OBJECT_IMPL(CImageReader)

CImageReader::CImageReader()
    : mWidth(0)
    , mHeight(0)
    , mFormat(0)
    , mMaxImages(0)
    , mNumPlanes(0)
    , mNativeContext(0)
{
}

CImageReader::~CImageReader()
{
}

ECode CImageReader::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 maxImages)
{
    mWidth = width;
    mHeight = height;
    mFormat = format;
    mMaxImages = maxImages;

    if (width < 1 || height < 1) {
        // throw new IllegalArgumentException(
        //     "The image dimensions must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mMaxImages < 1) {
        // throw new IllegalArgumentException(
        //     "Maximum outstanding image count must be at least 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (format == IImageFormat::NV21) {
        // throw new IllegalArgumentException(
        //         "NV21 format is not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    GetNumPlanesFromFormat(&mNumPlanes);

    NativeInit(THIS_PROBE(IImageReader), width, height, format, maxImages);

    NativeGetSurface((ISurface**)&mSurface);
    return NOERROR;
}

ECode CImageReader::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWidth;
    return NOERROR;
}

ECode CImageReader::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHeight;
    return NOERROR;
}

ECode CImageReader::GetImageFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFormat;
    return NOERROR;
}

ECode CImageReader::GetMaxImages(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaxImages;
    return NOERROR;
}

ECode CImageReader::GetSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSurface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CImageReader::AcquireLatestImage(
    /* [out] */ IImage** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IImage> image;
    AcquireNextImage((IImage**)&image);
    if (image == NULL) {
        return NOERROR;
    }
    // try {
    for (;;) {
        AutoPtr<IImage> next;
        AcquireNextImageNoThrowISE((IImage**)&next);
        if (next == NULL) {
            *result = image;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
// TODO: Need IAutoCloseable
        // image->Close();
        image = next;
    }
    // } finally {
    if (image != NULL) {
// TODO: Need IAutoCloseable
        // image->Close();
    }
    // }
    return NOERROR;
}

ECode CImageReader::AcquireNextImageNoThrowISE(
    /* [out] */ IImage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<SurfaceImage> si = new SurfaceImage(this);
    Int32 val;
    AcquireNextSurfaceImage(si, &val);
    *result = val == ACQUIRE_SUCCESS ? si : NULL;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CImageReader::AcquireNextImage(
    /* [out] */ IImage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<SurfaceImage> si = new SurfaceImage(this);
    Int32 status;
    AcquireNextSurfaceImage(si, &status);

    switch (status) {
        case ACQUIRE_SUCCESS:
            *result = si;
            REFCOUNT_ADD(*result)
            return NOERROR;
        case ACQUIRE_NO_BUFS:
            *result = NULL;
            return NOERROR;
        case ACQUIRE_MAX_IMAGES:
            // throw new IllegalStateException(
            //         String.format(
            //                 "maxImages (%d) has already been acquired, " +
            //                 "call #close before acquiring more.", mMaxImages));
            return E_ILLEGAL_STATE_EXCEPTION;
        default:
            // throw new AssertionError("Unknown nativeImageSetup return code " + status);
            return E_ASSERTION_ERROR;
    }
}

ECode CImageReader::SetOnImageAvailableListener(
    /* [in] */ IImageReaderOnImageAvailableListener* listener,
    /* [in] */ IHandler* handler)
{
    synchronized(mListenerLock) {
        if (listener != NULL) {
            AutoPtr<ILooper> looper;
            if (handler != NULL) {
                handler->GetLooper((ILooper**)&looper);
            }
            else {
                looper = Looper::GetMyLooper();
            }
            if (looper == NULL) {
                // throw new IllegalArgumentException(
                //         "handler is NULL but the current thread is not a looper");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            AutoPtr<ILooper> l;
            if (mListenerHandler == NULL ||
                    (mListenerHandler->GetLooper((ILooper**)&l), l) != looper) {
                mListenerHandler = new ListenerHandler(this, looper);
            }
            mListener = listener;
        }
        else {
            mListener = NULL;
            mListenerHandler = NULL;
        }
    }
    return NOERROR;
}

ECode CImageReader::NewInstance(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 maxImages,
    /* [out] */ IImageReader** result)
{
    VALIDATE_NOT_NULL(result)
    return CImageReader::New(width, height, format, maxImages, result);
}

ECode CImageReader::Close()
{
    SetOnImageAvailableListener(NULL, NULL);
    NativeClose();
    return NOERROR;
}

ECode CImageReader::AcquireNextSurfaceImage(
    /* [in] */ SurfaceImage* si,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 status;
    NativeImageSetup(si, &status);

    switch (status) {
        case ACQUIRE_SUCCESS:
            si->CreateSurfacePlanes();
            si->SetImageValid(TRUE);
        case ACQUIRE_NO_BUFS:
        case ACQUIRE_MAX_IMAGES:
            break;
        default:
            // throw new AssertionError("Unknown nativeImageSetup return code " + status);
            return E_ASSERTION_ERROR;
    }
    *result = status;
    return NOERROR;
}

ECode CImageReader::ReleaseImage(
    /* [in] */ IImage* i)
{
    if (IImageReaderSurfaceImage::Probe(i) != NULL) {
        // throw new IllegalArgumentException(
        //     "This image was not produced by an ImageReader");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<SurfaceImage> si = (SurfaceImage*) i;
    AutoPtr<IImageReader> r;
    si->GetReader((IImageReader**)&r);
    if (r != this) {
        // throw new IllegalArgumentException(
        //     "This image was not produced by this ImageReader");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    si->ClearSurfacePlanes();
    NativeReleaseImage(i);
    si->SetImageValid(FALSE);
    return NOERROR;
}

ECode CImageReader::GetNumPlanesFromFormat(
    /* [out] */ Int32* result)
{
    switch (mFormat) {
        case IImageFormat::YV12:
        case IImageFormat::YUV_420_888:
        case IImageFormat::NV21:
            *result = 3;
        case IImageFormat::NV16:
            *result = 2;
        case IPixelFormat::RGB_565:
        case IPixelFormat::RGBA_8888:
        case IPixelFormat::RGBX_8888:
        case IPixelFormat::RGB_888:
        case IImageFormat::JPEG:
        case IImageFormat::YUY2:
        case IImageFormat::Y8:
        case IImageFormat::Y16:
        case IImageFormat::RAW_SENSOR:
        case IImageFormat::RAW10:
            *result = 1;
        default:
            // throw new UnsupportedOperationException(
            //         String.format("Invalid format specified %d", mFormat));
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

void CImageReader::PostEventFromNative(
    /* [in] */ IInterface* selfRef)
{
    // @SuppressWarnings("unchecked")
    AutoPtr<IImageReader> ir = IImageReader::Probe(selfRef);
    AutoPtr<CImageReader> cir = (CImageReader*)ir.Get();
    if (ir == NULL) {
        return;
    }

    AutoPtr<IHandler> handler;
    Object& lock = cir->mListenerLock;
    synchronized(lock) {
        handler = cir->mListenerHandler;
    }
    if (handler != NULL) {
        Boolean b;
        handler->SendEmptyMessage(0, &b);
    }
}

ECode CImageReader::NativeInit(
    /* [in] */ IInterface* weakSelf,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 maxImages)
{
    android::status_t res;
    int nativeFormat;

    ALOGV("%s: width:%d, height: %d, format: 0x%x, maxImages:%d",
          __FUNCTION__, width, height, format, maxImages);

    // nativeFormat = Image_getPixelFormat(env, format);

    // sp<IGraphicBufferProducer> gbProducer;
    // sp<IGraphicBufferConsumer> gbConsumer;
    // BufferQueue::createBufferQueue(&gbProducer, &gbConsumer);
    // sp<CpuConsumer> consumer = new CpuConsumer(gbConsumer, maxImages,
    //                                            /*controlledByApp*/true);
    // // TODO: throw dvm exOutOfMemoryError?
    // if (consumer == NULL) {
    //     jniThrowRuntimeException(env, "Failed to allocate native CpuConsumer");
    //     return;
    // }

    // jclass clazz = env->GetObjectClass(thiz);
    // if (clazz == NULL) {
    //     jniThrowRuntimeException(env, "Can't find android/graphics/ImageReader");
    //     return;
    // }
    // sp<JNIImageReaderContext> ctx(new JNIImageReaderContext(env, weakThiz, clazz, maxImages));
    // ctx->setCpuConsumer(consumer);
    // ctx->setProducer(gbProducer);
    // consumer->setFrameAvailableListener(ctx);
    // ImageReader_setNativeContext(env, thiz, ctx);
    // ctx->setBufferFormat(nativeFormat);
    // ctx->setBufferWidth(width);
    // ctx->setBufferHeight(height);

    // // Set the width/height/format to the CpuConsumer
    // res = consumer->setDefaultBufferSize(width, height);
    // if (res != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to set CpuConsumer buffer size");
    //     return;
    // }
    // res = consumer->setDefaultBufferFormat(nativeFormat);
    // if (res != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to set CpuConsumer buffer format");
    // }
    return NOERROR;
}

void CImageReader::NativeClose()
{
    ALOGV("%s:", __FUNCTION__);

    // JNIImageReaderContext* const ctx = ImageReader_getContext(env, thiz);
    // if (ctx == NULL) {
    //     // ImageReader is already closed.
    //     return;
    // }

    // CpuConsumer* consumer = ImageReader_getCpuConsumer(env, thiz);
    // if (consumer != NULL) {
    //     consumer->abandon();
    //     consumer->setFrameAvailableListener(NULL);
    // }
    // ImageReader_setNativeContext(env, thiz, NULL);
}

void CImageReader::NativeReleaseImage(
    /* [in] */ IImage* i)
{
    ALOGV("%s:", __FUNCTION__);
    // JNIImageReaderContext* ctx = ImageReader_getContext(env, thiz);
    // if (ctx == NULL) {
    //     ALOGW("ImageReader#close called before Image#close, consider calling Image#close first");
    //     return;
    // }

    // CpuConsumer* consumer = ctx->getCpuConsumer();
    // CpuConsumer::LockedBuffer* buffer = Image_getLockedBuffer(env, image);
    // if (!buffer) {
    //     ALOGW("Image already released!!!");
    //     return;
    // }
    // consumer->unlockBuffer(*buffer);
    // Image_setBuffer(env, image, NULL);
    // ctx->returnLockedBuffer(buffer);
}

ECode CImageReader::NativeGetSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("%s: ", __FUNCTION__);

    // IGraphicBufferProducer* gbp = ImageReader_getProducer(env, thiz);
    // if (gbp == NULL) {
    //     jniThrowRuntimeException(env, "CpuConsumer is uninitialized");
    //     return NULL;
    // }

    // // Wrap the IGBP in a Java-language Surface.
    // return android_view_Surface_createFromIGraphicBufferProducer(env, gbp);
    return NOERROR;
}

ECode CImageReader::NativeImageSetup(
    /* [in] */ IImage* i,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("%s:", __FUNCTION__);
    // JNIImageReaderContext* ctx = ImageReader_getContext(env, thiz);
    // if (ctx == NULL) {
    //     jniThrowRuntimeException(env, "ImageReaderContext is not initialized");
    //     return -1;
    // }

    // CpuConsumer* consumer = ctx->getCpuConsumer();
    // CpuConsumer::LockedBuffer* buffer = ctx->getLockedBuffer();
    // if (buffer == NULL) {
    //     ALOGW("Unable to acquire a lockedBuffer, very likely client tries to lock more than"
    //         " maxImages buffers");
    //     return ACQUIRE_MAX_IMAGES;
    // }
    // status_t res = consumer->lockNextBuffer(buffer);
    // if (res != NO_ERROR) {
    //     ctx->returnLockedBuffer(buffer);
    //     if (res != BAD_VALUE /*no buffers*/) {
    //         if (res == NOT_ENOUGH_DATA) {
    //             return ACQUIRE_MAX_IMAGES;
    //         } else {
    //             ALOGE("%s Fail to lockNextBuffer with error: %d ",
    //                   __FUNCTION__, res);
    //             jniThrowExceptionFmt(env, "java/lang/AssertionError",
    //                       "Unknown error (%d) when we tried to lock buffer.",
    //                       res);
    //         }
    //     }
    //     return ACQUIRE_NO_BUFFERS;
    // }

    // if (buffer->format == HAL_PIXEL_FORMAT_YCrCb_420_SP) {
    //     jniThrowException(env, "java/lang/UnsupportedOperationException",
    //             "NV21 format is not supported by ImageReader");
    //     return -1;
    // }

    // // Check if the left-top corner of the crop rect is origin, we currently assume this point is
    // // zero, will revist this once this assumption turns out problematic.
    // Point lt = buffer->crop.leftTop();
    // if (lt.x != 0 || lt.y != 0) {
    //     jniThrowExceptionFmt(env, "java/lang/UnsupportedOperationException",
    //             "crop left top corner [%d, %d] need to be at origin", lt.x, lt.y);
    //     return -1;
    // }

    // // Check if the producer buffer configurations match what ImageReader configured.
    // // We want to fail for the very first image because this case is too bad.
    // int outputWidth = buffer->width;
    // int outputHeight = buffer->height;

    // // Correct width/height when crop is set.
    // if (!buffer->crop.isEmpty()) {
    //     outputWidth = buffer->crop.getWidth();
    //     outputHeight = buffer->crop.getHeight();
    // }

    // int imgReaderFmt = ctx->getBufferFormat();
    // int imageReaderWidth = ctx->getBufferWidth();
    // int imageReaderHeight = ctx->getBufferHeight();
    // if ((buffer->format != HAL_PIXEL_FORMAT_BLOB) && (imgReaderFmt != HAL_PIXEL_FORMAT_BLOB) &&
    //         (imageReaderWidth != outputWidth || imageReaderHeight > outputHeight)) {
    //     /**
    //      * For video decoder, the buffer height is actually the vertical stride,
    //      * which is always >= actual image height. For future, decoder need provide
    //      * right crop rectangle to CpuConsumer to indicate the actual image height,
    //      * see bug 9563986. After this bug is fixed, we can enforce the height equal
    //      * check. Right now, only make sure buffer height is no less than ImageReader
    //      * height.
    //      */
    //     jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
    //             "Producer buffer size: %dx%d, doesn't match ImageReader configured size: %dx%d",
    //             outputWidth, outputHeight, imageReaderWidth, imageReaderHeight);
    //     return -1;
    // }

    // int bufFmt = buffer->format;
    // if (imgReaderFmt != bufFmt) {

    //     if (imgReaderFmt == HAL_PIXEL_FORMAT_YCbCr_420_888 && (bufFmt ==
    //             HAL_PIXEL_FORMAT_YCrCb_420_SP || bufFmt == HAL_PIXEL_FORMAT_YV12)) {
    //         // Special casing for when producer switches to a format compatible with flexible YUV
    //         // (HAL_PIXEL_FORMAT_YCbCr_420_888).
    //         ctx->setBufferFormat(bufFmt);
    //         ALOGD("%s: Overriding buffer format YUV_420_888 to %x.", __FUNCTION__, bufFmt);
    //     } else if (imgReaderFmt == HAL_PIXEL_FORMAT_BLOB && bufFmt == HAL_PIXEL_FORMAT_RGBA_8888) {
    //         // Using HAL_PIXEL_FORMAT_RGBA_8888 gralloc buffers containing JPEGs to get around SW
    //         // write limitations for (b/17379185).
    //         ALOGD("%s: Receiving JPEG in HAL_PIXEL_FORMAT_RGBA_8888 buffer.", __FUNCTION__);
    //     } else {
    //         // Return the buffer to the queue.
    //         consumer->unlockBuffer(*buffer);
    //         ctx->returnLockedBuffer(buffer);

    //         // Throw exception
    //         ALOGE("Producer output buffer format: 0x%x, ImageReader configured format: 0x%x",
    //                 buffer->format, ctx->getBufferFormat());
    //         String8 msg;
    //         msg.appendFormat("The producer output buffer format 0x%x doesn't "
    //                 "match the ImageReader's configured buffer format 0x%x.",
    //                 buffer->format, ctx->getBufferFormat());
    //         jniThrowException(env, "java/lang/UnsupportedOperationException",
    //                 msg.string());
    //         return -1;
    //     }
    // }
    // // Set SurfaceImage instance member variables
    // Image_setBuffer(env, image, buffer);
    // env->SetLongField(image, gSurfaceImageClassInfo.mTimestamp,
    //         static_cast<jlong>(buffer->timestamp));

    // return ACQUIRE_SUCCESS;
    return NOERROR;
}

void CImageReader::NativeClassInit()
{
    ALOGV("%s:", __FUNCTION__);

    // jclass imageClazz = env->FindClass("android/media/ImageReader$SurfaceImage");
    // LOG_ALWAYS_FATAL_IF(imageClazz == NULL,
    //                     "can't find android/graphics/ImageReader$SurfaceImage");
    // gSurfaceImageClassInfo.mLockedBuffer = env->GetFieldID(
    //         imageClazz, ANDROID_MEDIA_SURFACEIMAGE_BUFFER_JNI_ID, "J");
    // LOG_ALWAYS_FATAL_IF(gSurfaceImageClassInfo.mLockedBuffer == NULL,
    //                     "can't find android/graphics/ImageReader.%s",
    //                     ANDROID_MEDIA_SURFACEIMAGE_BUFFER_JNI_ID);

    // gSurfaceImageClassInfo.mTimestamp = env->GetFieldID(
    //         imageClazz, ANDROID_MEDIA_SURFACEIMAGE_TS_JNI_ID, "J");
    // LOG_ALWAYS_FATAL_IF(gSurfaceImageClassInfo.mTimestamp == NULL,
    //                     "can't find android/graphics/ImageReader.%s",
    //                     ANDROID_MEDIA_SURFACEIMAGE_TS_JNI_ID);

    // gImageReaderClassInfo.mNativeContext = env->GetFieldID(
    //         clazz, ANDROID_MEDIA_IMAGEREADER_CTX_JNI_ID, "J");
    // LOG_ALWAYS_FATAL_IF(gImageReaderClassInfo.mNativeContext == NULL,
    //                     "can't find android/graphics/ImageReader.%s",
    //                       ANDROID_MEDIA_IMAGEREADER_CTX_JNI_ID);

    // gImageReaderClassInfo.postEventFromNative = env->GetStaticMethodID(
    //         clazz, "postEventFromNative", "(Ljava/lang/Object;)V");
    // LOG_ALWAYS_FATAL_IF(gImageReaderClassInfo.postEventFromNative == NULL,
    //                     "can't find android/graphics/ImageReader.postEventFromNative");

    // jclass planeClazz = env->FindClass("android/media/ImageReader$SurfaceImage$SurfacePlane");
    // LOG_ALWAYS_FATAL_IF(planeClazz == NULL, "Can not find SurfacePlane class");
    // // FindClass only gives a local reference of jclass object.
    // gSurfacePlaneClassInfo.clazz = (jclass) env->NewGlobalRef(planeClazz);
    // gSurfacePlaneClassInfo.ctor = env->GetMethodID(gSurfacePlaneClassInfo.clazz, "<init>",
    //         "(Landroid/media/ImageReader$SurfaceImage;III)V");
    // LOG_ALWAYS_FATAL_IF(gSurfacePlaneClassInfo.ctor == NULL,
    //         "Can not find SurfacePlane constructor");
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
