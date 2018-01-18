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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/media/CImageReader.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <hardware/camera3.h>
#include <gui/CpuConsumer.h>
#include <gui/Surface.h>
#include <utils/Log.h>
#include <utils/List.h>
#include <utils/misc.h>
#include <utils/String8.h>

using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::View::CSurface;
using Elastos::Core::AutoLock;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::EIID_IAutoCloseable;
using Elastos::Utility::Logging::Logger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CImageReader::SurfaceImage::SurfacePlane, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

static const String TAG("CImageReader");

//================================================================================
//  JNI Code
//================================================================================

#define ALIGN(x, mask) ( ((x) + (mask) - 1) & ~((mask) - 1) )

enum {
    IMAGE_READER_MAX_NUM_PLANES = 3,
};

enum {
    ACQUIRE_SUCCESS = 0,
    ACQUIRE_NO_BUFFERS = 1,
    ACQUIRE_MAX_IMAGES = 2,
};

enum {
    /*
     * "linear" color pixel formats:
     *
     * The pixel formats below contain sRGB data but are otherwise treated
     * as linear formats, i.e.: no special operation is performed when
     * reading or writing into a buffer in one of these formats
     */
    HAL_PIXEL_FORMAT_RGBA_8888          = 1,
    HAL_PIXEL_FORMAT_RGBX_8888          = 2,
    HAL_PIXEL_FORMAT_RGB_888            = 3,
    HAL_PIXEL_FORMAT_RGB_565            = 4,
    HAL_PIXEL_FORMAT_BGRA_8888          = 5,

    /*
     * sRGB color pixel formats:
     *
     * The red, green and blue components are stored in sRGB space, and converted
     * to linear space when read, using the standard sRGB to linear equation:
     *
     * Clinear = Csrgb / 12.92                  for Csrgb <= 0.04045
     *         = (Csrgb + 0.055 / 1.055)^2.4    for Csrgb >  0.04045
     *
     * When written the inverse transformation is performed:
     *
     * Csrgb = 12.92 * Clinear                  for Clinear <= 0.0031308
     *       = 1.055 * Clinear^(1/2.4) - 0.055  for Clinear >  0.0031308
     *
     *
     *  The alpha component, if present, is always stored in linear space and
     *  is left unmodified when read or written.
     *
     */
    HAL_PIXEL_FORMAT_sRGB_A_8888        = 0xC,
    HAL_PIXEL_FORMAT_sRGB_X_8888        = 0xD,

    /*
     * 0x100 - 0x1FF
     *
     * This range is reserved for pixel formats that are specific to the HAL
     * implementation.  Implementations can use any value in this range to
     * communicate video pixel formats between their HAL modules.  These formats
     * must not have an alpha channel.  Additionally, an EGLimage created from a
     * gralloc buffer of one of these formats must be supported for use with the
     * GL_OES_EGL_image_external OpenGL ES extension.
     */

    /*
     * Android YUV format:
     *
     * This format is exposed outside of the HAL to software decoders and
     * applications.  EGLImageKHR must support it in conjunction with the
     * OES_EGL_image_external extension.
     *
     * YV12 is a 4:2:0 YCrCb planar format comprised of a WxH Y plane followed
     * by (W/2) x (H/2) Cr and Cb planes.
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels
     * - a vertical stride equal to the height
     *
     *   y_size = stride * height
     *   c_stride = ALIGN(stride/2, 16)
     *   c_size = c_stride * height/2
     *   size = y_size + c_size * 2
     *   cr_offset = y_size
     *   cb_offset = y_size + c_size
     *
     */
    HAL_PIXEL_FORMAT_YV12   = 0x32315659, // YCrCb 4:2:0 Planar


    /*
     * Android Y8 format:
     *
     * This format is exposed outside of the HAL to the framework.
     * The expected gralloc usage flags are SW_* and HW_CAMERA_*,
     * and no other HW_ flags will be used.
     *
     * Y8 is a YUV planar format comprised of a WxH Y plane,
     * with each pixel being represented by 8 bits.
     *
     * It is equivalent to just the Y plane from YV12.
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels
     * - a vertical stride equal to the height
     *
     *   size = stride * height
     *
     */
    HAL_PIXEL_FORMAT_Y8     = 0x20203859,

    /*
     * Android Y16 format:
     *
     * This format is exposed outside of the HAL to the framework.
     * The expected gralloc usage flags are SW_* and HW_CAMERA_*,
     * and no other HW_ flags will be used.
     *
     * Y16 is a YUV planar format comprised of a WxH Y plane,
     * with each pixel being represented by 16 bits.
     *
     * It is just like Y8, but has double the bits per pixel (little endian).
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels
     * - a vertical stride equal to the height
     * - strides are specified in pixels, not in bytes
     *
     *   size = stride * height * 2
     *
     */
    HAL_PIXEL_FORMAT_Y16    = 0x20363159,

    /*
     * Android RAW sensor format:
     *
     * This format is exposed outside of the HAL to applications.
     *
     * RAW_SENSOR is a single-channel 16-bit format, typically representing raw
     * Bayer-pattern images from an image sensor, with minimal processing.
     *
     * The exact pixel layout of the data in the buffer is sensor-dependent, and
     * needs to be queried from the camera device.
     *
     * Generally, not all 16 bits are used; more common values are 10 or 12
     * bits. All parameters to interpret the raw data (black and white points,
     * color space, etc) must be queried from the camera device.
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels (32 bytes).
     */
    HAL_PIXEL_FORMAT_RAW_SENSOR = 0x20,

    /*
     * Android binary blob graphics buffer format:
     *
     * This format is used to carry task-specific data which does not have a
     * standard image structure. The details of the format are left to the two
     * endpoints.
     *
     * A typical use case is for transporting JPEG-compressed images from the
     * Camera HAL to the framework or to applications.
     *
     * Buffers of this format must have a height of 1, and width equal to their
     * size in bytes.
     */
    HAL_PIXEL_FORMAT_BLOB = 0x21,

    /*
     * Android format indicating that the choice of format is entirely up to the
     * device-specific Gralloc implementation.
     *
     * The Gralloc implementation should examine the usage bits passed in when
     * allocating a buffer with this format, and it should derive the pixel
     * format from those usage flags.  This format will never be used with any
     * of the GRALLOC_USAGE_SW_* usage flags.
     *
     * If a buffer of this format is to be used as an OpenGL ES texture, the
     * framework will assume that sampling the texture will always return an
     * alpha value of 1.0 (i.e. the buffer contains only opaque pixel values).
     *
     */
    HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED = 0x22,

    /*
     * Android flexible YCbCr formats
     *
     * This format allows platforms to use an efficient YCbCr/YCrCb buffer
     * layout, while still describing the buffer layout in a way accessible to
     * the CPU in a device-independent manner.  While called YCbCr, it can be
     * used to describe formats with either chromatic ordering, as well as
     * whole planar or semiplanar layouts.
     *
     * struct android_ycbcr (below) is the the struct used to describe it.
     *
     * This format must be accepted by the gralloc module when
     * USAGE_HW_CAMERA_WRITE and USAGE_SW_READ_* are set.
     *
     * This format is locked for use by gralloc's (*lock_ycbcr) method, and
     * locking with the (*lock) method will return an error.
     */
    HAL_PIXEL_FORMAT_YCbCr_420_888 = 0x23,

    /* Legacy formats (deprecated), used by ImageFormat.java */
    HAL_PIXEL_FORMAT_YCbCr_422_SP       = 0x10, // NV16
    HAL_PIXEL_FORMAT_YCrCb_420_SP       = 0x11, // NV21
    HAL_PIXEL_FORMAT_YCbCr_422_I        = 0x14, // YUY2
};

// Some formats like JPEG defined with different values between android.graphics.ImageFormat and
// graphics.h, need convert to the one defined in graphics.h here.
static Int32 Image_getPixelFormat(Int32 format)
{
    Int32 jpegFormat;

    Logger::V(TAG, "%s: format = 0x%x", __FUNCTION__, format);

    jpegFormat = IImageFormat::JPEG;

    // Translate the JPEG to BLOB for camera purpose.
    if (format == jpegFormat) {
        format = HAL_PIXEL_FORMAT_BLOB;
    }

    return format;
}

static bool usingRGBAToJpegOverride(int32_t bufferFormat, int32_t readerCtxFormat) {
    return readerCtxFormat == HAL_PIXEL_FORMAT_BLOB && bufferFormat == HAL_PIXEL_FORMAT_RGBA_8888;
}

static int32_t applyFormatOverrides(int32_t bufferFormat, int32_t readerCtxFormat)
{
    // Using HAL_PIXEL_FORMAT_RGBA_8888 gralloc buffers containing JPEGs to get around SW
    // write limitations for some platforms (b/17379185).
    if (usingRGBAToJpegOverride(bufferFormat, readerCtxFormat)) {
        return HAL_PIXEL_FORMAT_BLOB;
    }
    return bufferFormat;
}

static uint32_t Image_getJpegSize(android::CpuConsumer::LockedBuffer* buffer, bool usingRGBAOverride)
{
    ALOG_ASSERT(buffer != NULL, "Input buffer is NULL!!!");
    uint32_t size = 0;
    uint32_t width = buffer->width;
    uint8_t* jpegBuffer = buffer->data;

    if (usingRGBAOverride) {
        width = (buffer->width + buffer->stride * (buffer->height - 1)) * 4;
    }

    // First check for JPEG transport header at the end of the buffer
    uint8_t* header = jpegBuffer + (width - sizeof(struct camera3_jpeg_blob));
    struct camera3_jpeg_blob *blob = (struct camera3_jpeg_blob*)(header);
    if (blob->jpeg_blob_id == CAMERA3_JPEG_BLOB_ID) {
        size = blob->jpeg_size;
        Logger::V(TAG, "%s: Jpeg size = %d", __FUNCTION__, size);
    }

    // failed to find size, default to whole buffer
    if (size == 0) {
        /*
         * This is a problem because not including the JPEG header
         * means that in certain rare situations a regular JPEG blob
         * will be misidentified as having a header, in which case
         * we will get a garbage size value.
         */
        Logger::W(TAG, "%s: No JPEG header detected, defaulting to size=width=%d",
                __FUNCTION__, width);
        size = width;
    }

    return size;
}

static ECode Image_getLockedBufferInfo(android::CpuConsumer::LockedBuffer* buffer, int idx,
                                uint8_t **base, uint32_t *size, int32_t readerFormat)
{
    ALOG_ASSERT(buffer != NULL, "Input buffer is NULL!!!");
    ALOG_ASSERT(base != NULL, "base is NULL!!!");
    ALOG_ASSERT(size != NULL, "size is NULL!!!");
    ALOG_ASSERT((idx < IMAGE_READER_MAX_NUM_PLANES) && (idx >= 0));

    Logger::V(TAG, "%s: buffer: %p", __FUNCTION__, buffer);

    uint32_t dataSize, ySize, cSize, cStride;
    uint8_t *cb, *cr;
    uint8_t *pData = NULL;
    int bytesPerPixel = 0;

    dataSize = ySize = cSize = cStride = 0;
    int32_t fmt = buffer->format;

    bool usingRGBAOverride = usingRGBAToJpegOverride(fmt, readerFormat);
    fmt = applyFormatOverrides(fmt, readerFormat);
    switch (fmt) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            pData =
                (idx == 0) ?
                    buffer->data :
                (idx == 1) ?
                    buffer->dataCb :
                buffer->dataCr;
            if (idx == 0) {
                dataSize = buffer->stride * buffer->height;
            }
            else {
                dataSize = buffer->chromaStride * buffer->height / 2;
            }
            break;
        // NV21
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
            cr = buffer->data + (buffer->stride * buffer->height);
            cb = cr + 1;
            ySize = buffer->width * buffer->height;
            cSize = buffer->width * buffer->height / 2;

            pData =
                (idx == 0) ?
                    buffer->data :
                (idx == 1) ?
                    cb:
                cr;

            dataSize = (idx == 0) ? ySize : cSize;
            break;
        case HAL_PIXEL_FORMAT_YV12:
            // Y and C stride need to be 16 pixel aligned.
            LOG_ALWAYS_FATAL_IF(buffer->stride % 16,
                                "Stride is not 16 pixel aligned %d", buffer->stride);

            ySize = buffer->stride * buffer->height;
            cStride = ALIGN(buffer->stride / 2, 16);
            cr = buffer->data + ySize;
            cSize = cStride * buffer->height / 2;
            cb = cr + cSize;

            pData =
                (idx == 0) ?
                    buffer->data :
                (idx == 1) ?
                    cb :
                cr;
            dataSize = (idx == 0) ? ySize : cSize;
            break;
        case HAL_PIXEL_FORMAT_Y8:
            // Single plane, 8bpp.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);

            pData = buffer->data;
            dataSize = buffer->stride * buffer->height;
            break;
        case HAL_PIXEL_FORMAT_Y16:
            bytesPerPixel = 2;
            // Single plane, 16bpp, strides are specified in pixels, not in bytes
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);

            pData = buffer->data;
            dataSize = buffer->stride * buffer->height * bytesPerPixel;
            break;
        case HAL_PIXEL_FORMAT_BLOB:
            // Used for JPEG data, height must be 1, width == size, single plane.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            ALOG_ASSERT(buffer->height == 1, "JPEG should has height value %d", buffer->height);

            pData = buffer->data;
            dataSize = Image_getJpegSize(buffer, usingRGBAOverride);
            break;
        case HAL_PIXEL_FORMAT_RAW_SENSOR:
            // Single plane 16bpp bayer data.
            bytesPerPixel = 2;
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pData = buffer->data;
            dataSize = buffer->stride * buffer->height * bytesPerPixel;
            break;
        case HAL_PIXEL_FORMAT_RAW10:
            // Single plane 10bpp bayer data.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            LOG_ALWAYS_FATAL_IF(buffer->width % 4,
                                "Width is not multiple of 4 %d", buffer->width);
            LOG_ALWAYS_FATAL_IF(buffer->height % 2,
                                "Height is not even %d", buffer->height);
            LOG_ALWAYS_FATAL_IF(buffer->stride < (buffer->width * 10 / 8),
                                "stride (%d) should be at least %d",
                                buffer->stride, buffer->width * 10 / 8);
            pData = buffer->data;
            dataSize = buffer->stride * buffer->height;
            break;
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
            // Single plane, 32bpp.
            bytesPerPixel = 4;
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pData = buffer->data;
            dataSize = buffer->stride * buffer->height * bytesPerPixel;
            break;
        case HAL_PIXEL_FORMAT_RGB_565:
            // Single plane, 16bpp.
            bytesPerPixel = 2;
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pData = buffer->data;
            dataSize = buffer->stride * buffer->height * bytesPerPixel;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            // Single plane, 24bpp.
            bytesPerPixel = 3;
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pData = buffer->data;
            dataSize = buffer->stride * buffer->height * bytesPerPixel;
            break;
        default:
            // jniThrowExceptionFmt(env, "java/lang/UnsupportedOperationException",
            //                      "Pixel format: 0x%x is unsupported", fmt);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
            break;
    }

    *base = pData;
    *size = dataSize;
    return NOERROR;
}

static Int32 Image_imageGetRowStride(android::CpuConsumer::LockedBuffer* buffer, int idx,
        int32_t readerFormat)
{
    Logger::V(TAG, "%s: buffer index: %d", __FUNCTION__, idx);
    ALOG_ASSERT((idx < IMAGE_READER_MAX_NUM_PLANES) && (idx >= 0));

    int rowStride = 0;
    ALOG_ASSERT(buffer != NULL, "buffer is NULL");

    int32_t fmt = buffer->format;

    fmt = applyFormatOverrides(fmt, readerFormat);

    switch (fmt) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            rowStride = (idx == 0) ? buffer->stride : buffer->chromaStride;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
            rowStride = buffer->width;
            break;
        case HAL_PIXEL_FORMAT_YV12:
            LOG_ALWAYS_FATAL_IF(buffer->stride % 16,
                                "Stride is not 16 pixel aligned %d", buffer->stride);
            rowStride = (idx == 0) ? buffer->stride : ALIGN(buffer->stride / 2, 16);
            break;
        case HAL_PIXEL_FORMAT_BLOB:
            // Blob is used for JPEG data, RAW10 is used for 10-bit raw data, they are
            // single plane, row and pixel strides are 0.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            rowStride = 0;
            break;
        case HAL_PIXEL_FORMAT_RAW10:
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            rowStride = buffer->stride;
            break;
        case HAL_PIXEL_FORMAT_Y8:
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            LOG_ALWAYS_FATAL_IF(buffer->stride % 16,
                                "Stride is not 16 pixel aligned %d", buffer->stride);
            rowStride = buffer->stride;
            break;
        case HAL_PIXEL_FORMAT_Y16:
        case HAL_PIXEL_FORMAT_RAW_SENSOR:
            // In native side, strides are specified in pixels, not in bytes.
            // Single plane 16bpp bayer data. even width/height,
            // row stride multiple of 16 pixels (32 bytes)
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            LOG_ALWAYS_FATAL_IF(buffer->stride % 16,
                                "Stride is not 16 pixel aligned %d", buffer->stride);
            rowStride = buffer->stride * 2;
            break;
        case HAL_PIXEL_FORMAT_RGB_565:
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            rowStride = buffer->stride * 2;
            break;
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            rowStride = buffer->stride * 4;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            // Single plane, 24bpp.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            rowStride = buffer->stride * 3;
            break;
        default:
            ALOGE("%s Pixel format: 0x%x is unsupported", __FUNCTION__, fmt);
            // jniThrowException(env, "java/lang/UnsupportedOperationException",
            //                   "unsupported buffer format");
            return 0;
          break;
    }

    return rowStride;
}

static Int32 Image_imageGetPixelStride(android::CpuConsumer::LockedBuffer* buffer, int idx,
        int32_t readerFormat)
{
    Logger::V(TAG, "%s: buffer index: %d", __FUNCTION__, idx);
    ALOG_ASSERT((idx < IMAGE_READER_MAX_NUM_PLANES) && (idx >= 0), "Index is out of range:%d", idx);

    int pixelStride = 0;
    ALOG_ASSERT(buffer != NULL, "buffer is NULL");

    int32_t fmt = buffer->format;

    fmt = applyFormatOverrides(fmt, readerFormat);

    switch (fmt) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            pixelStride = (idx == 0) ? 1 : buffer->chromaStep;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
            pixelStride = (idx == 0) ? 1 : 2;
            break;
        case HAL_PIXEL_FORMAT_Y8:
            // Single plane 8bpp data.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pixelStride = 0;
            break;
        case HAL_PIXEL_FORMAT_YV12:
            pixelStride = 1;
            break;
        case HAL_PIXEL_FORMAT_BLOB:
        case HAL_PIXEL_FORMAT_RAW10:
            // Blob is used for JPEG data, RAW10 is used for 10-bit raw data, they are
            // single plane, row and pixel strides are 0.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pixelStride = 0;
            break;
        case HAL_PIXEL_FORMAT_Y16:
        case HAL_PIXEL_FORMAT_RAW_SENSOR:
        case HAL_PIXEL_FORMAT_RGB_565:
            // Single plane 16bpp data.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pixelStride = 2;
            break;
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pixelStride = 4;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            // Single plane, 24bpp.
            ALOG_ASSERT(idx == 0, "Wrong index: %d", idx);
            pixelStride = 3;
            break;
        default:
            // jniThrowExceptionFmt(env, "java/lang/UnsupportedOperationException",
            //                      "Pixel format: 0x%x is unsupported", fmt);
            return 0;
            break;
    }

    return pixelStride;
}

//================================================================================
//  JNIImageReaderContext
//================================================================================

class JNIImageReaderContext
    : public android::CpuConsumer::FrameAvailableListener
{
public:
    JNIImageReaderContext(IImageReader* weakThiz, int maxImages);

    virtual ~JNIImageReaderContext();

    virtual void onFrameAvailable();

    android::CpuConsumer::LockedBuffer* getLockedBuffer();

    void returnLockedBuffer(android::CpuConsumer::LockedBuffer* buffer);

    void setCpuConsumer(const android::sp<android::CpuConsumer>& consumer) { mConsumer = consumer; }
    android::CpuConsumer* getCpuConsumer() { return mConsumer.get(); }

    void setProducer(const android::sp<android::IGraphicBufferProducer>& producer) { mProducer = producer; }
    android::IGraphicBufferProducer* getProducer() { return mProducer.get(); }

    void setBufferFormat(int format) { mFormat = format; }
    int getBufferFormat() { return mFormat; }

    void setBufferWidth(int width) { mWidth = width; }
    int getBufferWidth() { return mWidth; }

    void setBufferHeight(int height) { mHeight = height; }
    int getBufferHeight() { return mHeight; }

private:
    android::List<android::CpuConsumer::LockedBuffer*> mBuffers;
    android::sp<android::CpuConsumer> mConsumer;
    android::sp<android::IGraphicBufferProducer> mProducer;
    IImageReader* mWeakThiz;
    int mFormat;
    int mWidth;
    int mHeight;
};

JNIImageReaderContext::JNIImageReaderContext(IImageReader* weakThiz, int maxImages)
    : mWeakThiz(weakThiz)
{
    for (int i = 0; i < maxImages; i++) {
        android::CpuConsumer::LockedBuffer *buffer = new android::CpuConsumer::LockedBuffer;
        mBuffers.push_back(buffer);
    }
}

android::CpuConsumer::LockedBuffer* JNIImageReaderContext::getLockedBuffer()
{
    if (mBuffers.empty()) {
        return NULL;
    }
    // Return a LockedBuffer pointer and remove it from the list
    android::List<android::CpuConsumer::LockedBuffer*>::iterator it = mBuffers.begin();
    android::CpuConsumer::LockedBuffer* buffer = *it;
    mBuffers.erase(it);
    return buffer;
}

void JNIImageReaderContext::returnLockedBuffer(android::CpuConsumer::LockedBuffer* buffer)
{
    mBuffers.push_back(buffer);
}

JNIImageReaderContext::~JNIImageReaderContext()
{
    // Delete LockedBuffers
    for (android::List<android::CpuConsumer::LockedBuffer *>::iterator it = mBuffers.begin();
            it != mBuffers.end(); it++) {
        delete *it;
    }
    mBuffers.clear();
    mConsumer.clear();
}

void JNIImageReaderContext::onFrameAvailable()
{
    Logger::V(TAG, "%s: frame available", __FUNCTION__);
    CImageReader::PostEventFromNative(mWeakThiz);
}

//================================================================================
//  CImageReader::ListenerHandler
//================================================================================

ECode CImageReader::ListenerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IImageReaderOnImageAvailableListener> listener;
    Object& lock = mHost->mListenerLock;
    {    AutoLock syncLock(lock);
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

ECode CImageReader::SurfaceImage::NativeImageGetBuffer(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 _readerFormat,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 readerFormat = _readerFormat;
    uint8_t *base = NULL;
    uint32_t size = 0;
    AutoPtr<IByteBuffer> byteBuffer;

    Logger::V(TAG, "%s: buffer index: %d", __FUNCTION__, idx);

    android::CpuConsumer::LockedBuffer* buffer =
            reinterpret_cast<android::CpuConsumer::LockedBuffer*>(mLockedBuffer);

    if (buffer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", "Image was released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    readerFormat = Image_getPixelFormat(readerFormat);

    // Create byteBuffer from native buffer
    FAIL_RETURN(Image_getLockedBufferInfo(buffer, idx, &base, &size, readerFormat));

    if (size > static_cast<uint32_t>(INT32_MAX)) {
        // Byte buffer have 'int capacity', so check the range
        // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
        //         "Size too large for bytebuffer capacity %" PRIu32, size);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // byteBuffer = env->NewDirectByteBuffer(base, size);
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    helper->AllocateDirect(size, (IByteBuffer**)&byteBuffer);

    // TODO: throw dvm exOutOfMemoryError?
    if (byteBuffer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", "Failed to allocate ByteBuffer");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = byteBuffer;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CImageReader::SurfaceImage::NativeCreatePlane(
    /* [in] */ Int32 idx,
    /* [in] */ Int32 readerFormat,
    /* [out] */ SurfacePlane** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 rowStride, pixelStride;
    Logger::V(TAG, "%s: buffer index: %d", __FUNCTION__, idx);

    android::CpuConsumer::LockedBuffer* buffer =
            reinterpret_cast<android::CpuConsumer::LockedBuffer*>(mLockedBuffer);

    ALOG_ASSERT(buffer != NULL);
    if (buffer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", "Image was released");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    readerFormat = Image_getPixelFormat(readerFormat);

    rowStride = Image_imageGetRowStride(buffer, idx, readerFormat);
    pixelStride = Image_imageGetPixelStride(buffer, idx, readerFormat);

    // jobject surfPlaneObj = env->NewObject(gSurfacePlaneClassInfo.clazz,
    //         gSurfacePlaneClassInfo.ctor, thiz, idx, rowStride, pixelStride);
    AutoPtr<SurfacePlane> surfPlane = new SurfacePlane(idx, rowStride, pixelStride, this);

    *result = surfPlane;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//================================================================================
//  CImageReader
//================================================================================

const Int32 CImageReader::ACQUIRE_SUCCESS = 0;
const Int32 CImageReader::ACQUIRE_NO_BUFS = 1;
const Int32 CImageReader::ACQUIRE_MAX_IMAGES = 2;

CAR_INTERFACE_IMPL_2(CImageReader, Object, IImageReader, IAutoCloseable)

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
    NativeClose();
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

    NativeInit(this, width, height, format, maxImages);

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
    {    AutoLock syncLock(mListenerLock);
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
            Logger::E(TAG, "Unknown nativeImageSetup return code %d", status);
            return E_ASSERTION_ERROR;
    }
    *result = status;
    return NOERROR;
}

ECode CImageReader::ReleaseImage(
    /* [in] */ IImage* i)
{
    if (IImageReaderSurfaceImage::Probe(i) != NULL) {
        Logger::E(TAG, "This image was not produced by an ImageReader");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<SurfaceImage> si = (SurfaceImage*) i;
    AutoPtr<IImageReader> r;
    si->GetReader((IImageReader**)&r);
    if (r != this) {
        Logger::E(TAG, "This image was not produced by this ImageReader");
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
    {    AutoLock syncLock(lock);
        handler = cir->mListenerHandler;
    }
    if (handler != NULL) {
        Boolean b;
        handler->SendEmptyMessage(0, &b);
    }
}

ECode CImageReader::NativeInit(
    /* [in] */ IImageReader* weakThiz,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 maxImages)
{
    android::status_t res;
    int nativeFormat;

    Logger::V(TAG, "%s: width:%d, height: %d, format: 0x%x, maxImages:%d",
          __FUNCTION__, width, height, format, maxImages);

    nativeFormat = Image_getPixelFormat(format);

    android::sp<android::IGraphicBufferProducer> gbProducer;
    android::sp<android::IGraphicBufferConsumer> gbConsumer;
    android::BufferQueue::createBufferQueue(&gbProducer, &gbConsumer);
    android::sp<android::CpuConsumer> consumer = new android::CpuConsumer(
            gbConsumer, maxImages, /*controlledByApp*/true);
    // TODO: throw dvm exOutOfMemoryError?
    if (consumer == NULL) {
        Logger::E(TAG, "Failed to allocate native CpuConsumer");
        return E_RUNTIME_EXCEPTION;
    }

    android::sp<JNIImageReaderContext> ctx = new JNIImageReaderContext(weakThiz, maxImages);
    ctx->setCpuConsumer(consumer);
    ctx->setProducer(gbProducer);
    consumer->setFrameAvailableListener(ctx);

    mNativeContext = reinterpret_cast<Int64>(ctx.get());
    ctx->incStrong(this);
    ctx->setBufferFormat(nativeFormat);
    ctx->setBufferWidth(width);
    ctx->setBufferHeight(height);

    // Set the width/height/format to the CpuConsumer
    res = consumer->setDefaultBufferSize(width, height);
    if (res != android::OK) {
        Logger::E(TAG, "Failed to set CpuConsumer buffer size");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    res = consumer->setDefaultBufferFormat(nativeFormat);
    if (res != android::OK) {
        Logger::E(TAG, "Failed to set CpuConsumer buffer format");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void CImageReader::NativeClose()
{
    Logger::V(TAG, "%s:", __FUNCTION__);

    android::sp<JNIImageReaderContext> ctx = reinterpret_cast<JNIImageReaderContext *>(mNativeContext);
    if (ctx == NULL) {
        // ImageReader is already closed.
        return;
    }

    android::CpuConsumer* consumer = ctx->getCpuConsumer();

    if (consumer != NULL) {
        consumer->abandon();
        consumer->setFrameAvailableListener(NULL);
    }

    ctx->decStrong(this);
    mNativeContext = 0;
}

void CImageReader::NativeReleaseImage(
    /* [in] */ IImage* image)
{
    Logger::V(TAG, "%s:", __FUNCTION__);
    JNIImageReaderContext* ctx = reinterpret_cast<JNIImageReaderContext *>(mNativeContext);
    if (ctx == NULL) {
        Logger::W(TAG, "ImageReader#close called before Image#close, consider calling Image#close first");
        return;
    }

    android::CpuConsumer* consumer = ctx->getCpuConsumer();
    android::CpuConsumer::LockedBuffer* buffer =
            reinterpret_cast<android::CpuConsumer::LockedBuffer*>(((SurfaceImage*)image)->mLockedBuffer);

    if (!buffer) {
        Logger::W(TAG, "Image already released!!!");
        return;
    }
    consumer->unlockBuffer(*buffer);
    ((SurfaceImage*)image)->mLockedBuffer = reinterpret_cast<Int64>((Int64)0);
    ctx->returnLockedBuffer(buffer);
}

ECode CImageReader::NativeGetSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Logger::V(TAG, "%s: ", __FUNCTION__);

    JNIImageReaderContext* const ctx = reinterpret_cast<JNIImageReaderContext *>(mNativeContext);
    if (ctx == NULL) {
        Logger::E(TAG, "ImageReaderContext is not initialized");
        return E_RUNTIME_EXCEPTION;
    }

    android::IGraphicBufferProducer* gbp = ctx->getProducer();
    if (gbp == NULL) {
        Logger::E(TAG, "CpuConsumer is uninitialized");
        return E_RUNTIME_EXCEPTION;
    }

    // Wrap the IGBP in a Java-language Surface.
    android::sp<android::Surface> surface(new android::Surface(gbp, true));
    if (surface == NULL) {
        return NOERROR;
    }

    return CSurface::New((Int64)surface.get(), result);
}

ECode CImageReader::NativeImageSetup(
    /* [in] */ IImage* image,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Logger::V(TAG, "%s:", __FUNCTION__);
    JNIImageReaderContext* ctx = reinterpret_cast<JNIImageReaderContext *>(mNativeContext);
    if (ctx == NULL) {
        Logger::E(TAG, "ImageReaderContext is not initialized");
        *result = -1;
        return E_RUNTIME_EXCEPTION;
    }

    android::CpuConsumer* consumer = ctx->getCpuConsumer();
    android::CpuConsumer::LockedBuffer* buffer = ctx->getLockedBuffer();
    if (buffer == NULL) {
        Logger::W(TAG, "Unable to acquire a lockedBuffer, very likely client tries to lock more than"
            " maxImages buffers");
        *result = ACQUIRE_MAX_IMAGES;
        return NOERROR;
    }
    android::status_t res = consumer->lockNextBuffer(buffer);
    if (res != android::NO_ERROR) {
        ctx->returnLockedBuffer(buffer);
        if (res != android::BAD_VALUE /*no buffers*/) {
            if (res == android::NOT_ENOUGH_DATA) {
                return ACQUIRE_MAX_IMAGES;
            }
            else {
                Logger::E(TAG, "%s Fail to lockNextBuffer with error: %d ",
                      __FUNCTION__, res);
                Logger::E(TAG, "Unknown error (%d) when we tried to lock buffer.", res);
                return E_ASSERTION_ERROR;
            }
        }
        *result = ACQUIRE_NO_BUFFERS;
        return NOERROR;
    }

    if (buffer->format == HAL_PIXEL_FORMAT_YCrCb_420_SP) {
        Logger::E(TAG, "NV21 format is not supported by ImageReader");
        *result = -1;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    // Check if the left-top corner of the crop rect is origin, we currently assume this point is
    // zero, will revist this once this assumption turns out problematic.
    android::Point lt = buffer->crop.leftTop();
    if (lt.x != 0 || lt.y != 0) {
        Logger::E(TAG, "crop left top corner [%d, %d] need to be at origin", lt.x, lt.y);
        *result = -1;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    // Check if the producer buffer configurations match what ImageReader configured.
    // We want to fail for the very first image because this case is too bad.
    Int32 outputWidth = buffer->width;
    Int32 outputHeight = buffer->height;

    // Correct width/height when crop is set.
    if (!buffer->crop.isEmpty()) {
        outputWidth = buffer->crop.getWidth();
        outputHeight = buffer->crop.getHeight();
    }

    Int32 imgReaderFmt = ctx->getBufferFormat();
    Int32 imageReaderWidth = ctx->getBufferWidth();
    Int32 imageReaderHeight = ctx->getBufferHeight();
    if ((buffer->format != HAL_PIXEL_FORMAT_BLOB) && (imgReaderFmt != HAL_PIXEL_FORMAT_BLOB) &&
            (imageReaderWidth != outputWidth || imageReaderHeight > outputHeight)) {
        /**
         * For video decoder, the buffer height is actually the vertical stride,
         * which is always >= actual image height. For future, decoder need provide
         * right crop rectangle to CpuConsumer to indicate the actual image height,
         * see bug 9563986. After this bug is fixed, we can enforce the height equal
         * check. Right now, only make sure buffer height is no less than ImageReader
         * height.
         */
        Logger::E(TAG, "Producer buffer size: %dx%d, doesn't match ImageReader configured size: %dx%d",
            outputWidth, outputHeight, imageReaderWidth, imageReaderHeight);
        *result = -1;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 bufFmt = buffer->format;
    if (imgReaderFmt != bufFmt) {

        if (imgReaderFmt == HAL_PIXEL_FORMAT_YCbCr_420_888 && (bufFmt ==
                HAL_PIXEL_FORMAT_YCrCb_420_SP || bufFmt == HAL_PIXEL_FORMAT_YV12)) {
            // Special casing for when producer switches to a format compatible with flexible YUV
            // (HAL_PIXEL_FORMAT_YCbCr_420_888).
            ctx->setBufferFormat(bufFmt);
            Logger::D(TAG, "%s: Overriding buffer format YUV_420_888 to %x.", __FUNCTION__, bufFmt);
        }
        else if (imgReaderFmt == HAL_PIXEL_FORMAT_BLOB && bufFmt == HAL_PIXEL_FORMAT_RGBA_8888) {
            // Using HAL_PIXEL_FORMAT_RGBA_8888 gralloc buffers containing JPEGs to get around SW
            // write limitations for (b/17379185).
            Logger::D(TAG, "%s: Receiving JPEG in HAL_PIXEL_FORMAT_RGBA_8888 buffer.", __FUNCTION__);
        }
        else {
            // Return the buffer to the queue.
            consumer->unlockBuffer(*buffer);
            ctx->returnLockedBuffer(buffer);

            // Throw exception
            ALOGE("Producer output buffer format: 0x%x, ImageReader configured format: 0x%x",
                    buffer->format, ctx->getBufferFormat());
            android::String8 msg;
            msg.appendFormat("The producer output buffer format 0x%x doesn't "
                    "match the ImageReader's configured buffer format 0x%x.",
                    buffer->format, ctx->getBufferFormat());
            Logger::E(TAG, msg.string());
            *result = -1;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    }
    // Set SurfaceImage instance member variables
    ((SurfaceImage*)image)->mLockedBuffer = reinterpret_cast<Int64>(buffer);
    ((SurfaceImage*)image)->mTimestamp = buffer->timestamp;
    *result = ACQUIRE_SUCCESS;
    return NOERROR;
}

void CImageReader::NativeClassInit()
{
    Logger::V(TAG, "%s:", __FUNCTION__);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
