
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/DngCreator.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <img_utils/DngUtils.h>
#include <img_utils/TiffEntry.h>
#include <img_utils/TagDefinitions.h>
#include <utils/String8.h>

using Elastos::Droid::Hardware::Camera2::CaptureResult;
using Elastos::Droid::Media::IExifInterface;
using Elastos::Droid::Media::IImagePlane;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger64;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;
using Elastos::Core::AutoLock;
using Elastos::IO::IBuffer;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Text::IFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;

using android::img_utils::OpcodeListBuilder;
using android::img_utils::TiffEntry;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

#define BAIL_IF_INVALID(expr, tagId, writer) \
    if ((expr) != android::OK) { \
        Slogger::E("DngCreator", "Invalid metadata for tag %s (%x)", (writer)->getTagName(tagId), (tagId)); \
        return E_ILLEGAL_ARGUMENT_EXCEPTION; \
    }

#define BAIL_IF_EMPTY(entry, tagId, writer) \
    if (entry.count == 0) { \
        Slogger::E("DngCreator", "Missing metadata fields for tag %s (%x)", (writer)->getTagName(tagId), (tagId)); \
        return E_ILLEGAL_ARGUMENT_EXCEPTION; \
    }

enum {
    BITS_PER_SAMPLE = 16,
    BYTES_PER_SAMPLE = 2,
    BYTES_PER_RGB_PIXEL = 3,
    BITS_PER_RGB_SAMPLE = 8,
    BYTES_PER_RGB_SAMPLE = 1,
    SAMPLES_PER_RGB_PIXEL = 3,
    SAMPLES_PER_RAW_PIXEL = 1,
    TIFF_IFD_0 = 0,
    TIFF_IFD_SUB1 = 1,
    TIFF_IFD_GPSINFO = 2,
};

DngCreator::NativeContext::NativeContext()
    : mThumbnailWidth(0)
    , mThumbnailHeight(0)
{
}

DngCreator::NativeContext::~NativeContext()
{
}

TiffWriter* DngCreator::NativeContext::getWriter()
{
    return &mWriter;
}

uint32_t DngCreator::NativeContext::getThumbnailWidth()
{
    return mThumbnailWidth;
}

uint32_t DngCreator::NativeContext::getThumbnailHeight()
{
    return mThumbnailHeight;
}

const uint8_t* DngCreator::NativeContext::getThumbnail()
{
    return mCurrentThumbnail.array();
}

bool DngCreator::NativeContext::setThumbnail(
    /* [in] */ const uint8_t* buffer,
    /* [in] */ uint32_t width,
    /* [in] */ uint32_t height)
{
    mThumbnailWidth = width;
    mThumbnailHeight = height;

    size_t size = BYTES_PER_RGB_PIXEL * width * height;
    if (mCurrentThumbnail.resize(size) < 0) {
        ALOGE("%s: Could not resize thumbnail buffer.", __FUNCTION__);
        return false;
    }

    uint8_t* thumb = mCurrentThumbnail.editArray();
    memcpy(thumb, buffer, size);
    return true;
}

DngCreator::JniOutputStream::JniOutputStream(
    /* [in] */ IOutputStream* outStream)
    : mOutputStream(outStream)
{
    mByteArray = ArrayOf<Byte>::Alloc(BYTE_ARRAY_LENGTH);
    if (mByteArray == NULL) {
        //jniThrowException(env, "java/lang/OutOfMemoryError", "Could not allocate byte array.");
        Slogger::E("DngCreator::JniOutputStream", "java/lang/OutOfMemoryError : Could not allocate byte array.");
        //return E_OUT_OF_MEMORY_ERROR;
    }
}

DngCreator::JniOutputStream::~JniOutputStream()
{
}

status_t DngCreator::JniOutputStream::open()
{
    // Do nothing
    return android::OK;
}

status_t DngCreator::JniOutputStream::write(
    /* [in] */ const uint8_t* buf,
    /* [in] */ size_t offset,
    /* [in] */ size_t count)
{
    while(count > 0) {
        size_t len = BYTE_ARRAY_LENGTH;
        len = (count > len) ? len : count;

        mByteArray->Copy(0, reinterpret_cast<const Byte*>(buf + offset), len);

        // if (mEnv->ExceptionCheck()) {
        //     return BAD_VALUE;
        // }

        mOutputStream->Write(mByteArray, 0, len);

        // if (mEnv->ExceptionCheck()) {
        //     return BAD_VALUE;
        // }

        count -= len;
        offset += len;
    }
    return android::OK;
}

status_t DngCreator::JniOutputStream::close()
{
    // Do nothing
    return android::OK;
}

DngCreator::JniInputStream::JniInputStream(
    /* [in] */ IInputStream* inStream)
    : mInStream(inStream)
{
    mByteArray = ArrayOf<Byte>::Alloc(BYTE_ARRAY_LENGTH);
    if (mByteArray == NULL) {
        //jniThrowException(env, "java/lang/OutOfMemoryError", "Could not allocate byte array.");
        Slogger::E("DngCreator::JniInputStream", "java/lang/OutOfMemoryError : Could not allocate byte array.");
        //return E_OUT_OF_MEMORY_ERROR;
    }
}

DngCreator::JniInputStream::~JniInputStream()
{
}

ssize_t DngCreator::JniInputStream::read(
    /* [in] */ uint8_t* buf,
    /* [in] */ size_t offset,
    /* [in] */ size_t count)
{
    Int32 realCount = BYTE_ARRAY_LENGTH;
    if (count < BYTE_ARRAY_LENGTH) {
        realCount = count;
    }
    Int32 actual;
    ECode ec = mInStream->Read(mByteArray, 0, realCount, &actual);

    if (actual < 0) {
        return android::NOT_ENOUGH_DATA;
    }

    if (FAILED(ec)) {
        return android::BAD_VALUE;
    }

    memcpy(reinterpret_cast<Byte*>(buf + offset), mByteArray->GetPayload(), actual);

    // if (mEnv->ExceptionCheck()) {
    //     return BAD_VALUE;
    // }
    return actual;
}

ssize_t DngCreator::JniInputStream::skip(
    /* [in] */ size_t count)
{
    Int64 actual;
    ECode ec = mInStream->Skip(static_cast<Int64>(count), &actual);

    if (FAILED(ec)) {
        return android::BAD_VALUE;
    }
    if (actual < 0) {
        return android::NOT_ENOUGH_DATA;
    }
    return actual;
}

status_t DngCreator::JniInputStream::open() {
    // Do nothing
    return android::OK;
}

status_t DngCreator::JniInputStream::close() {
    // Do nothing
    return android::OK;
}

DngCreator::JniInputByteBuffer::JniInputByteBuffer(
    /* [in] */ IByteBuffer* inBuf)
    : mInBuf(inBuf)
{
    mByteArray = ArrayOf<Byte>::Alloc(BYTE_ARRAY_LENGTH);
    if (mByteArray == NULL) {
        //jniThrowException(env, "java/lang/OutOfMemoryError", "Could not allocate byte array.");
        Slogger::E("DngCreator::JniInputByteBuffer", "java/lang/OutOfMemoryError : Could not allocate byte array.");
        //return E_OUT_OF_MEMORY_ERROR;
    }
}

DngCreator::JniInputByteBuffer::~JniInputByteBuffer()
{
}

ssize_t DngCreator::JniInputByteBuffer::read(
    /* [in] */ uint8_t* buf,
    /* [in] */ size_t offset,
    /* [in] */ size_t count)
{
    Int32 realCount = BYTE_ARRAY_LENGTH;
    if (count < BYTE_ARRAY_LENGTH) {
        realCount = count;
    }

    ECode ec = mInBuf->Get(mByteArray, 0, realCount);

    if (FAILED(ec)) {
        return android::BAD_VALUE;
    }

    memcpy(reinterpret_cast<Byte*>(buf + offset), mByteArray->GetPayload(), realCount);
    // if (mEnv->ExceptionCheck()) {
    //     return BAD_VALUE;
    // }
    return realCount;
}

status_t DngCreator::JniInputByteBuffer::open()
{
    // Do nothing
    return android::OK;
}

status_t DngCreator::JniInputByteBuffer::close()
{
    // Do nothing
    return android::OK;
}

DngCreator::InputStripSource::InputStripSource(
    /* [in] */ android::img_utils::Input& input,
    /* [in] */ uint32_t ifd,
    /* [in] */ uint32_t width,
    /* [in] */ uint32_t height,
    /* [in] */ uint32_t pixStride,
    /* [in] */ uint32_t rowStride,
    /* [in] */ uint64_t offset,
    /* [in] */ uint32_t bytesPerSample,
    /* [in] */ uint32_t samplesPerPixel)
    : mIfd(ifd)
    , mInput(&input)
    , mWidth(width)
    , mHeight(height)
    , mPixStride(pixStride)
    , mRowStride(rowStride)
    , mOffset(offset)
    , mBytesPerSample(bytesPerSample)
    , mSamplesPerPixel(samplesPerPixel)
{
}

DngCreator::InputStripSource::~InputStripSource()
{
}

status_t DngCreator::InputStripSource::writeToStream(
    /* [in] */ Output& stream,
    /* [in] */ uint32_t count)
{
    status_t err = android::OK;
    uint32_t fullSize = mWidth * mHeight * mBytesPerSample * mSamplesPerPixel;
    Int64 offset = mOffset;

    if (fullSize != count) {
        ALOGE("%s: Amount to write %u doesn't match image size %u", __FUNCTION__, count,
                fullSize);
        //jniThrowException(mEnv, "java/lang/IllegalStateException", "Not enough data to write");
        Slogger::E("DngCreator::InputStripSource", "java/lang/IllegalStateException : Not enough data to write");
        return android::BAD_VALUE;
    }

    // Skip offset
    while (offset > 0) {
        ssize_t skipped = mInput->skip(offset);
        if (skipped <= 0) {
            if (skipped == android::NOT_ENOUGH_DATA || skipped == 0) {
                // jniThrowExceptionFmt(mEnv, "java/io/IOException",
                //         "Early EOF encountered in skip, not enough pixel data for image of size %u",
                //         fullSize);
                Slogger::E("DngCreator::InputStripSource", "java/io/IOException "
                        "Early EOF encountered in skip, not enough pixel data for image of size %u",
                        fullSize);
                skipped = android::NOT_ENOUGH_DATA;
                return E_IO_EXCEPTION;
            }
            else {
                // if (!mEnv->ExceptionCheck()) {
                //     jniThrowException(mEnv, "java/io/IOException",
                //             "Error encountered while skip bytes in input stream.");
                // }
            }

            return skipped;
        }
        offset -= skipped;
    }

    android::Vector<uint8_t> row;
    if (row.resize(mRowStride) < 0) {
        //jniThrowException(mEnv, "java/lang/OutOfMemoryError", "Could not allocate row vector.");
        Slogger::E("DngCreator::InputStripSource", "java/lang/OutOfMemoryError : Could not allocate row vector.");
        return android::BAD_VALUE;
    }

    uint8_t* rowBytes = row.editArray();

    for (uint32_t i = 0; i < mHeight; ++i) {
        size_t rowFillAmt = 0;
        size_t rowSize = mPixStride;

        while (rowFillAmt < mRowStride) {
            ssize_t bytesRead = mInput->read(rowBytes, rowFillAmt, rowSize);
            if (bytesRead <= 0) {
                if (bytesRead == android::NOT_ENOUGH_DATA || bytesRead == 0) {
                    // jniThrowExceptionFmt(mEnv, "java/io/IOException",
                    //         "Early EOF encountered, not enough pixel data for image of size %u",
                    //         fullSize);
                    Slogger::E("DngCreator::InputStripSource", "java/io/IOException :"
                            "Early EOF encountered, not enough pixel data for image of size %u",
                            fullSize);
                    bytesRead = android::NOT_ENOUGH_DATA;
                    return E_IO_EXCEPTION;
                }
                else {
                    // if (!mEnv->ExceptionCheck()) {
                    //     jniThrowException(mEnv, "java/io/IOException",
                    //             "Error encountered while reading");
                    // }
                }
                return bytesRead;
            }
            rowFillAmt += bytesRead;
            rowSize -= bytesRead;
        }

        if (mPixStride == mBytesPerSample * mSamplesPerPixel) {
            ALOGV("%s: Using stream per-row write for strip.", __FUNCTION__);

            if (stream.write(rowBytes, 0, mBytesPerSample * mSamplesPerPixel * mWidth) != android::OK/* ||
                    mEnv->ExceptionCheck()*/) {
                // if (!mEnv->ExceptionCheck()) {
                //     jniThrowException(mEnv, "java/io/IOException", "Failed to write pixel data");
                // }
                return android::BAD_VALUE;
            }
        }
        else {
            ALOGV("%s: Using stream per-pixel write for strip.", __FUNCTION__);
            // jniThrowException(mEnv, "java/lang/IllegalStateException",
            //         "Per-pixel strides are not supported for RAW16 -- pixels must be contiguous");
            Slogger::E("DngCreator::InputStripSource", "java/lang/IllegalStateException :"
                    "Per-pixel strides are not supported for RAW16 -- pixels must be contiguous");
            return android::BAD_VALUE;

            // TODO: Add support for non-contiguous pixels if needed.
        }
    }
    return android::OK;
}

uint32_t DngCreator::InputStripSource::getIfd() const
{
    return mIfd;
}

DngCreator::DirectStripSource::DirectStripSource(
    /* [in] */ const uint8_t* pixelBytes,
    /* [in] */ uint32_t ifd,
    /* [in] */ uint32_t width,
    /* [in] */ uint32_t height,
    /* [in] */ uint32_t pixStride,
    /* [in] */ uint32_t rowStride,
    /* [in] */ uint64_t offset,
    /* [in] */ uint32_t bytesPerSample,
    /* [in] */ uint32_t samplesPerPixel)
    : mIfd(ifd)
    , mPixelBytes(pixelBytes)
    , mWidth(width)
    , mHeight(height)
    , mPixStride(pixStride)
    , mRowStride(rowStride)
    , mOffset(offset)
    , mBytesPerSample(bytesPerSample)
    , mSamplesPerPixel(samplesPerPixel)
{
}

DngCreator::DirectStripSource::~DirectStripSource()
{
}

status_t DngCreator::DirectStripSource::writeToStream(
    /* [in] */ Output& stream,
    /* [in] */ uint32_t count)
{
    uint32_t fullSize = mWidth * mHeight * mBytesPerSample * mSamplesPerPixel;

    if (fullSize != count) {
        ALOGE("%s: Amount to write %u doesn't match image size %u", __FUNCTION__, count,
                fullSize);
        //jniThrowException(mEnv, "java/lang/IllegalStateException", "Not enough data to write");
        Slogger::E("DngCreator::InputStripSource", "java/lang/IllegalStateException :Not enough data to write");
        return android::BAD_VALUE;
    }

    if (mPixStride == mBytesPerSample * mSamplesPerPixel
            && mRowStride == mWidth * mBytesPerSample * mSamplesPerPixel) {
        ALOGV("%s: Using direct single-pass write for strip.", __FUNCTION__);

        if (stream.write(mPixelBytes, mOffset, fullSize) != android::OK/* || mEnv->ExceptionCheck()*/) {
            // if (!mEnv->ExceptionCheck()) {
            //     jniThrowException(mEnv, "java/io/IOException", "Failed to write pixel data");
            // }
            return android::BAD_VALUE;
        }
    }
    else if (mPixStride == mBytesPerSample * mSamplesPerPixel) {
        ALOGV("%s: Using direct per-row write for strip.", __FUNCTION__);

        for (size_t i = 0; i < mHeight; ++i) {
            if (stream.write(mPixelBytes, mOffset + i * mRowStride, mPixStride * mWidth) != android::OK/* ||
                        mEnv->ExceptionCheck()*/) {
                // if (!mEnv->ExceptionCheck()) {
                //     jniThrowException(mEnv, "java/io/IOException", "Failed to write pixel data");
                // }
                return android::BAD_VALUE;
            }
        }
    }
    else {
        ALOGV("%s: Using direct per-pixel write for strip.", __FUNCTION__);

        // jniThrowException(mEnv, "java/lang/IllegalStateException",
        //         "Per-pixel strides are not supported for RAW16 -- pixels must be contiguous");
        Slogger::E("DngCreator::InputStripSource", "java/lang/IllegalStateException :"
                "Per-pixel strides are not supported for RAW16 -- pixels must be contiguous");
        return android::BAD_VALUE;

        // TODO: Add support for non-contiguous pixels if needed.
    }
    return android::OK;
}

uint32_t DngCreator::DirectStripSource::getIfd() const
{
    return mIfd;
}

static ECode validateDngHeader(
    /* [in] */ TiffWriter* writer,
    /* [in] */ int width,
    /* [in] */ int height,
    /* [out] */ bool* result)
{
    VALIDATE_NOT_NULL(result);

    bool hasThumbnail = writer->hasIfd(TIFF_IFD_SUB1);

    // TODO: handle lens shading map, etc. conversions for other raw buffer sizes.
    uint32_t metadataWidth = *(writer->getEntry(android::img_utils::TAG_IMAGEWIDTH, (hasThumbnail) ?
            TIFF_IFD_SUB1 : TIFF_IFD_0)->getData<uint32_t>());
    uint32_t metadataHeight = *(writer->getEntry(android::img_utils::TAG_IMAGELENGTH, (hasThumbnail) ?
            TIFF_IFD_SUB1 : TIFF_IFD_0)->getData<uint32_t>());

    if (width < 0 || metadataWidth != static_cast<uint32_t>(width)) {
        // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException", \
        //                 "Metadata width %d doesn't match image width %d", metadataWidth, width);
        *result = FALSE;
        Slogger::E("DngCreator::validateDngHeader", "java/lang/IllegalArgumentException :"
                "Metadata width %d doesn't match image width %d", metadataWidth, width);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (height < 0 || metadataHeight != static_cast<uint32_t>(height)) {
        // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException", \
        //                 "Metadata height %d doesn't match image height %d", metadataHeight, height);
        *result = FALSE;
        Slogger::E("DngCreator::validateDngHeader", "java/lang/IllegalArgumentException :"
                "Metadata height %d doesn't match image height %d", metadataHeight, height);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = TRUE;
    return NOERROR;
}

static status_t moveEntries(
    /* [in] */ TiffWriter* writer,
    /* [in] */ uint32_t ifdFrom,
    /* [in] */ uint32_t ifdTo,
    /* [in] */ const android::Vector<uint16_t>& entries)
{
    for (size_t i = 0; i < entries.size(); ++i) {
        uint16_t tagId = entries[i];
        android::sp<TiffEntry> entry = writer->getEntry(tagId, ifdFrom);
        if (entry == NULL) {
            ALOGE("%s: moveEntries failed, entry %u not found in IFD %u", __FUNCTION__, tagId,
                    ifdFrom);
            return android::BAD_VALUE;
        }
        if (writer->addEntry(entry, ifdTo) != android::OK) {
            ALOGE("%s: moveEntries failed, could not add entry %u to IFD %u", __FUNCTION__, tagId,
                    ifdFrom);
            return android::BAD_VALUE;
        }
        writer->removeEntry(tagId, ifdFrom);
    }
    return android::OK;
}

/**
 * Write CFA pattern for given CFA enum into cfaOut.  cfaOut must have length >= 4.
 * Returns OK on success, or a negative error code if the CFA enum was invalid.
 */
static status_t convertCFA(
    /* [in] */ uint8_t cfaEnum,
    /* [in] */ uint8_t* cfaOut)
{
    camera_metadata_enum_android_sensor_info_color_filter_arrangement_t cfa =
            static_cast<camera_metadata_enum_android_sensor_info_color_filter_arrangement_t>(
            cfaEnum);
    switch(cfa) {
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB: {
            cfaOut[0] = 0;
            cfaOut[1] = 1;
            cfaOut[2] = 1;
            cfaOut[3] = 2;
            break;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG: {
            cfaOut[0] = 1;
            cfaOut[1] = 0;
            cfaOut[2] = 2;
            cfaOut[3] = 1;
            break;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG: {
            cfaOut[0] = 1;
            cfaOut[1] = 2;
            cfaOut[2] = 0;
            cfaOut[3] = 1;
            break;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR: {
            cfaOut[0] = 2;
            cfaOut[1] = 1;
            cfaOut[2] = 1;
            cfaOut[3] = 0;
            break;
        }
        default: {
            return android::BAD_VALUE;
        }
    }
    return android::OK;
}

/**
 * Convert the CFA layout enum to an OpcodeListBuilder::CfaLayout enum, defaults to
 * RGGB for an unknown enum.
 */
static OpcodeListBuilder::CfaLayout convertCFAEnumToOpcodeLayout(
    /* [in] */ uint8_t cfaEnum)
{
    camera_metadata_enum_android_sensor_info_color_filter_arrangement_t cfa =
            static_cast<camera_metadata_enum_android_sensor_info_color_filter_arrangement_t>(
            cfaEnum);
    switch(cfa) {
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB: {
            return OpcodeListBuilder::CFA_RGGB;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG: {
            return OpcodeListBuilder::CFA_GRBG;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG: {
            return OpcodeListBuilder::CFA_GBRG;
        }
        case ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR: {
            return OpcodeListBuilder::CFA_BGGR;
        }
        default: {
            return OpcodeListBuilder::CFA_RGGB;
        }
    }
}

/**
 * For each color plane, find the corresponding noise profile coefficients given in the
 * per-channel noise profile.  If multiple channels in the CFA correspond to a color in the color
 * plane, this method takes the pair of noise profile coefficients with the higher S coefficient.
 *
 * perChannelNoiseProfile - numChannels * 2 noise profile coefficients.
 * cfa - numChannels color channels corresponding to each of the per-channel noise profile
 *       coefficients.
 * numChannels - the number of noise profile coefficient pairs and color channels given in
 *       the perChannelNoiseProfile and cfa arguments, respectively.
 * planeColors - the color planes in the noise profile output.
 * numPlanes - the number of planes in planeColors and pairs of coefficients in noiseProfile.
 * noiseProfile - 2 * numPlanes doubles containing numPlanes pairs of noise profile coefficients.
 *
 * returns OK, or a negative error code on failure.
 */
static status_t generateNoiseProfile(
    /* [in] */ const double* perChannelNoiseProfile,
    /* [in] */ uint8_t* cfa,
    /* [in] */ size_t numChannels,
    /* [in] */ const uint8_t* planeColors,
    /* [in] */ size_t numPlanes,
    /* [in] */ double* noiseProfile)
{
    for (size_t p = 0; p < numPlanes; ++p) {
        size_t S = p * 2;
        size_t O = p * 2 + 1;

        noiseProfile[S] = 0;
        noiseProfile[O] = 0;
        bool uninitialized = true;
        for (size_t c = 0; c < numChannels; ++c) {
            if (cfa[c] == planeColors[p] && perChannelNoiseProfile[c * 2] > noiseProfile[S]) {
                noiseProfile[S] = perChannelNoiseProfile[c * 2];
                noiseProfile[O] = perChannelNoiseProfile[c * 2 + 1];
                uninitialized = false;
            }
        }
        if (uninitialized) {
            ALOGE("%s: No valid NoiseProfile coefficients for color plane %u", __FUNCTION__, p);
            return android::BAD_VALUE;
        }
    }
    return android::OK;
}

const String DngCreator::TAG("DngCreator");

const String DngCreator::GPS_LAT_REF_NORTH("N");
const String DngCreator::GPS_LAT_REF_SOUTH("S");
const String DngCreator::GPS_LONG_REF_EAST("E");
const String DngCreator::GPS_LONG_REF_WEST("W");

const String DngCreator::GPS_DATE_FORMAT_STR("yyyy:MM:dd");
const String DngCreator::TIFF_DATETIME_FORMAT("yyyy:MM:dd kk:mm:ss");

const Int32 DngCreator::DEFAULT_PIXEL_STRIDE = 2; // bytes per sample
const Int32 DngCreator::BYTES_PER_RGB_PIX = 3; // byts per pixel

Int64 DngCreator::mNativeContext = 0;

Int32 DngCreator::mNativeInit = NativeClassInit();

AutoPtr<IDateFormat> DngCreator::InitExifGPSDateStamp()
{
    AutoPtr<IDateFormat> format;
    CSimpleDateFormat::New(GPS_DATE_FORMAT_STR, (IDateFormat**)&format);

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> timeZone;
    tzHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);

    format->SetTimeZone(timeZone);

    return format;
}

AutoPtr<IDateFormat> DngCreator::InitDateTimeStampFormat()
{
    AutoPtr<IDateFormat> format;
    CSimpleDateFormat::New(TIFF_DATETIME_FORMAT, (IDateFormat**)&format);

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> timeZone;
    tzHelper->GetDefault((ITimeZone**)&timeZone);

    format->SetTimeZone(timeZone);

    return format;
}

AutoPtr<IDateFormat> DngCreator::sExifGPSDateStamp = InitExifGPSDateStamp();

AutoPtr<IDateFormat> DngCreator::sDateTimeStampFormat = InitDateTimeStampFormat();

CAR_INTERFACE_IMPL_2(DngCreator, Object, IDngCreator, ICloseable)

DngCreator::DngCreator()
{
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> timeZone;
    tzHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);

    AutoPtr<ICalendarHelper> chelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&chelper);
    chelper->GetInstance(timeZone, (ICalendar**)&mGPSTimeStampCalendar);
}

DngCreator::~DngCreator()
{
    //try {
    Close();
    //} finally {
    //    super.finalize();
    //}
}

ECode DngCreator::constructor()
{
    return NOERROR;
}

ECode DngCreator::constructor(
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ ICaptureResult* metadata)
{
    if (characteristics == NULL || metadata == NULL) {
        //throw new IllegalArgumentException("Null argument to DngCreator constructor");
        Slogger::E(TAG, "Null argument to DngCreator constructor");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Find current time
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTime;
    system->GetCurrentTimeMillis(&currentTime);

    // Find boot time
    Int64 bootTimeMillis = currentTime - SystemClock::GetElapsedRealtime();

    // Find capture time (nanos since boot)
    AutoPtr<IInterface> obj;
    metadata->Get(CaptureResult::SENSOR_TIMESTAMP, (IInterface**)&obj);
    AutoPtr<IInteger64> int64Obj = IInteger64::Probe(obj);
    Int64 captureTime = currentTime;
    if (int64Obj != NULL) {
        Int64 timestamp;
        int64Obj->GetValue(&timestamp);
        captureTime = timestamp / 1000000 + bootTimeMillis;
    }

    // Format for metadata
    String formattedCaptureTime;
    AutoPtr<IInteger64> formatObj = CoreUtils::Convert(captureTime);
    IFormat::Probe(sDateTimeStampFormat)->Format(TO_IINTERFACE(formatObj), &formattedCaptureTime);

    AutoPtr<ICameraMetadataNative> copy;
    characteristics->GetNativeCopy((ICameraMetadataNative**)&copy);
    AutoPtr<ICameraMetadataNative> copy2;
    metadata->GetNativeCopy((ICameraMetadataNative**)&copy2);
    NativeInit(copy, copy2, formattedCaptureTime);
    return NOERROR;
}

ECode DngCreator::SetOrientation(
    /* [in] */ Int32 orientation)
{
    if (orientation < IExifInterface::ORIENTATION_UNDEFINED ||
            orientation > IExifInterface::ORIENTATION_ROTATE_270) {
        // throw new IllegalArgumentException("Orientation " + orientation +
        //         " is not a valid EXIF orientation value");
        Slogger::E(TAG, "Orientation %d is not a valid EXIF orientation "
                "value", orientation);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeSetOrientation(orientation);
    return NOERROR;
}

ECode DngCreator::SetThumbnail(
    /* [in] */ IBitmap* pixels)
{
    if (pixels == NULL) {
        //throw new IllegalArgumentException("Null argument to setThumbnail");
        Slogger::E(TAG, "Null argument to setThumbnail");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width;
    pixels->GetWidth(&width);
    Int32 height;
    pixels->GetHeight(&height);

    if (width > MAX_THUMBNAIL_DIMENSION || height > MAX_THUMBNAIL_DIMENSION) {
        // throw new IllegalArgumentException("Thumbnail dimensions width,height (" + width +
        //         "," + height + ") too large, dimensions must be smaller than " +
        //         MAX_THUMBNAIL_DIMENSION);
        Slogger::E(TAG, "Thumbnail dimensions width,height (%d,%d) too large, "
                "dimensions must be smaller than %d", width, height, MAX_THUMBNAIL_DIMENSION);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IByteBuffer> rgbBuffer = ConvertToRGB(pixels);
    NativeSetThumbnail(rgbBuffer, width, height);
    return NOERROR;
}

ECode DngCreator::SetThumbnail(
    /* [in] */ IImage* pixels)
{
    if (pixels == NULL) {
        //throw new IllegalArgumentException("Null argument to setThumbnail");
        Slogger::E(TAG, "Null argument to setThumbnail");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 format;
    pixels->GetFormat(&format);
    if (format != IImageFormat::YUV_420_888) {
        //throw new IllegalArgumentException("Unsupported Image format " + format);
        Slogger::E(TAG, "Unsupported Image format %d", format);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width;
    pixels->GetWidth(&width);
    Int32 height;
    pixels->GetHeight(&height);

    if (width > MAX_THUMBNAIL_DIMENSION || height > MAX_THUMBNAIL_DIMENSION) {
        // throw new IllegalArgumentException("Thumbnail dimensions width,height (" + width +
        //         "," + height + ") too large, dimensions must be smaller than " +
        //         MAX_THUMBNAIL_DIMENSION);
        Slogger::E(TAG, "Thumbnail dimensions width,height (%d,%d) too large, dimensions "
                "must be smaller than %d", width, height, MAX_THUMBNAIL_DIMENSION);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IByteBuffer> rgbBuffer = ConvertToRGB(pixels);
    NativeSetThumbnail(rgbBuffer, width, height);
    return NOERROR;
}

ECode DngCreator::SetLocation(
    /* [in] */ ILocation* location)
{
    if (location == NULL) {
        //throw new IllegalArgumentException("Null location passed to setLocation");
        Slogger::E(TAG, "Null location passed to setLocation");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Double latitude;
    location->GetLatitude(&latitude);
    Double longitude;
    location->GetLongitude(&longitude);
    Int64 _time;
    location->GetTime(&_time);

    AutoPtr<ArrayOf<Int32> > latTag = ToExifLatLong(latitude);
    AutoPtr<ArrayOf<Int32> > longTag = ToExifLatLong(longitude);

    String latRef = latitude >= 0 ? GPS_LAT_REF_NORTH : GPS_LAT_REF_SOUTH;
    String longRef = longitude >= 0 ? GPS_LONG_REF_EAST : GPS_LONG_REF_WEST;

    String dateTag;
    AutoPtr<IInteger64> formatObj = CoreUtils::Convert(_time);
    IFormat::Probe(sExifGPSDateStamp)->Format(TO_IINTERFACE(formatObj), &dateTag);
    mGPSTimeStampCalendar->SetTimeInMillis(_time);
    AutoPtr<ArrayOf<Int32> > timeTag = ArrayOf<Int32>::Alloc(6);
    Int32 hour;
    mGPSTimeStampCalendar->Get(ICalendar::HOUR_OF_DAY, &hour);
    (*timeTag)[0] = hour;
    (*timeTag)[1] = 1;
    Int32 minite;
    mGPSTimeStampCalendar->Get(ICalendar::MINUTE, &minite);
    (*timeTag)[2] = minite;
    (*timeTag)[3] = 1;
    Int32 secend;
    mGPSTimeStampCalendar->Get(ICalendar::SECOND, &secend);
    (*timeTag)[4] = secend;
    (*timeTag)[5] = 1;

    NativeSetGpsTags(latTag, latRef, longTag, longRef, dateTag, timeTag);
    return NOERROR;
}

ECode DngCreator::SetDescription(
    /* [in] */ const String& description)
{
    if (description == NULL) {
        //throw new IllegalArgumentException("Null description passed to setDescription.");
        Slogger::E(TAG, "Null description passed to setDescription.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeSetDescription(description);
    return NOERROR;
}

ECode DngCreator::WriteInputStream(
    /* [in] */ IOutputStream* dngOutput,
    /* [in] */ ISize* size,
    /* [in] */ IInputStream* pixels,
    /* [in] */ Int64 offset)
{
    if (dngOutput == NULL) {
        //throw new IllegalArgumentException("Null dngOutput passed to writeInputStream");
        Slogger::E(TAG, "Null dngOutput passed to writeInputStream");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (size == NULL) {
        //throw new IllegalArgumentException("Null size passed to writeInputStream");
        Slogger::E(TAG, "Null size passed to writeInputStream");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (pixels == NULL) {
        //throw new IllegalArgumentException("Null pixels passed to writeInputStream");
        Slogger::E(TAG, "Null pixels passed to writeInputStream");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (offset < 0) {
        //throw new IllegalArgumentException("Negative offset passed to writeInputStream");
        Slogger::E(TAG, "Negative offset passed to writeInputStream");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width;
    size->GetWidth(&width);
    Int32 height;
    size->GetHeight(&height);

    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("Size with invalid width, height: (" + width + "," +
        //         height + ") passed to writeInputStream");
        Slogger::E(TAG, "Size with invalid width, height: (%d,%d) passed to "
                "writeInputStream", width, height);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeWriteInputStream(dngOutput, pixels, width, height, offset);
}


ECode DngCreator::WriteByteBuffer(
    /* [in] */ IOutputStream* dngOutput,
    /* [in] */ ISize* size,
    /* [in] */ IByteBuffer* pixels,
    /* [in] */ Int64 offset)
{
    if (dngOutput == NULL) {
        //throw new IllegalArgumentException("Null dngOutput passed to writeByteBuffer");
        Slogger::E(TAG, "Null dngOutput passed to writeByteBuffer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (size == NULL) {
        //throw new IllegalArgumentException("Null size passed to writeByteBuffer");
        Slogger::E(TAG, "Null size passed to writeByteBuffer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (pixels == NULL) {
        //throw new IllegalArgumentException("Null pixels passed to writeByteBuffer");
        Slogger::E(TAG, "Null pixels passed to writeByteBuffer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (offset < 0) {
        //throw new IllegalArgumentException("Negative offset passed to writeByteBuffer");
        Slogger::E(TAG, "Negative offset passed to writeByteBuffer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 width;
    size->GetWidth(&width);
    Int32 height;
    size->GetHeight(&height);

    return WriteByteBuffer(width, height, pixels, dngOutput, DEFAULT_PIXEL_STRIDE,
            width * DEFAULT_PIXEL_STRIDE, offset);
}


ECode DngCreator::WriteImage(
    /* [in] */ IOutputStream* dngOutput,
    /* [in] */ IImage* pixels)
{
    if (dngOutput == NULL) {
        //throw new IllegalArgumentException("Null dngOutput to writeImage");
        Slogger::E(TAG, "Null dngOutput to writeImage");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (pixels == NULL) {
        //throw new IllegalArgumentException("Null pixels to writeImage");
        Slogger::E(TAG, "Null pixels to writeImage");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 format;
    pixels->GetFormat(&format);
    if (format != IImageFormat::RAW_SENSOR) {
        //throw new IllegalArgumentException("Unsupported image format " + format);
        Slogger::E(TAG, "Unsupported image format %d", format);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<IImagePlane*> > planes;
    pixels->GetPlanes((ArrayOf<IImagePlane*>**)&planes);
    if (planes == NULL) {
        //throw new IllegalArgumentException("Image with no planes passed to writeImage");
        Slogger::E(TAG, "Image with no planes passed to writeImage");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (planes->GetLength() <= 0) {
            //throw new IllegalArgumentException("Image with no planes passed to writeImage");
            Slogger::E(TAG, "Image with no planes passed to writeImage");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IByteBuffer> buf;
    (*planes)[0]->GetBuffer((IByteBuffer**)&buf);
    Int32 width;
    pixels->GetWidth(&width);
    Int32 height;
    pixels->GetHeight(&height);
    Int32 pixelStride;
    (*planes)[0]->GetPixelStride(&pixelStride);
    Int32 rowStride;
    (*planes)[0]->GetRowStride(&rowStride);
    return WriteByteBuffer(width, height, buf, dngOutput, pixelStride, rowStride, 0);
}

ECode DngCreator::Close()
{
    NativeDestroy();
    return NOERROR;
}

ECode DngCreator::WriteByteBuffer(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IByteBuffer* pixels,
    /* [in] */ IOutputStream* dngOutput,
    /* [in] */ Int32 pixelStride,
    /* [in] */ Int32 rowStride,
    /* [in] */ Int64 offset)
{
    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("Image with invalid width, height: (" + width + "," +
        //         height + ") passed to write");
        Slogger::E(TAG, "Image with invalid width, height: (%d,%d) passed to"
                "write", width, height);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 capacity;
    IBuffer::Probe(pixels)->GetCapacity(&capacity);
    Int64 totalSize = rowStride * height + offset;
    if (capacity < totalSize) {
        // throw new IllegalArgumentException("Image size " + capacity +
        //         " is too small (must be larger than " + totalSize + ")");
        Slogger::E(TAG, "Image size %d is too small (must be larger than %d)",
                capacity, totalSize);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 minRowStride = pixelStride * width;
    if (minRowStride > rowStride) {
        // throw new IllegalArgumentException("Invalid image pixel stride, row byte width " +
        //         minRowStride + " is too large, expecting " + rowStride);
        Slogger::E(TAG, "Invalid image pixel stride, row byte width %d is too large,"
                "expecting %d", minRowStride, rowStride);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    IBuffer::Probe(pixels)->Clear(); // Reset mark and limit
    Boolean result;
    IBuffer::Probe(pixels)->IsDirect(&result);
    FAIL_RETURN(NativeWriteImage(dngOutput, width, height, pixels, rowStride, pixelStride, offset,
            result))
    return IBuffer::Probe(pixels)->Clear();
}


void DngCreator::YuvToRgb(
    /* [in] */ ArrayOf<Byte>* yuvData,
    /* [in] */ Int32 outOffset,
    /* [in] */ ArrayOf<Byte>* rgbOut)
{
    const Int32 COLOR_MAX = 255;

    Float y = (*yuvData)[0] & 0xFF;  // Y channel
    Float cb = (*yuvData)[1] & 0xFF; // U channel
    Float cr = (*yuvData)[2] & 0xFF; // V channel

    // convert YUV -> RGB (from JFIF's "Conversion to and from RGB" section)
    Float r = y + 1.402f * (cr - 128);
    Float g = y - 0.34414f * (cb - 128) - 0.71414f * (cr - 128);
    Float b = y + 1.772f * (cb - 128);

    // clamp to [0,255]
    (*rgbOut)[outOffset] = (Byte)Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min((Float)COLOR_MAX, r));
    (*rgbOut)[outOffset + 1] = (Byte)Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min((Float)COLOR_MAX, g));
    (*rgbOut)[outOffset + 2] = (Byte)Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min((Float)COLOR_MAX, b));
    return;
}

void DngCreator::ColorToRgb(
    /* [in] */ Int32 color,
    /* [in] */ Int32 outOffset,
    /* [in] */ ArrayOf<Byte>* rgbOut)
{
    (*rgbOut)[outOffset] = (Byte)Color::Red(color);
    (*rgbOut)[outOffset + 1] = (Byte)Color::Green(color);
    (*rgbOut)[outOffset + 2] = (Byte)Color::Blue(color);
    // Discards Alpha
    return;
}

AutoPtr<IByteBuffer> DngCreator::ConvertToRGB(
    /* [in] */ IImage* yuvImage)
{
    // TODO: Optimize this with renderscript intrinsic.
    Int32 width;
    yuvImage->GetWidth(&width);
    Int32 height;
    yuvImage->GetHeight(&height);

    AutoPtr<IByteBufferHelper> bufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferHelper);
    AutoPtr<IByteBuffer> buf;
    bufferHelper->AllocateDirect(BYTES_PER_RGB_PIX * width * height, (IByteBuffer**)&buf);

    AutoPtr<ArrayOf<IImagePlane*> > plane;
    yuvImage->GetPlanes((ArrayOf<IImagePlane*>**)&plane);
    AutoPtr<IImagePlane> yPlane = (*plane)[0];
    AutoPtr<IImagePlane> uPlane = (*plane)[1];
    AutoPtr<IImagePlane> vPlane = (*plane)[2];

    AutoPtr<IByteBuffer> yBuf;
    yPlane->GetBuffer((IByteBuffer**)&yBuf);
    AutoPtr<IByteBuffer> uBuf;
    uPlane->GetBuffer((IByteBuffer**)&uBuf);
    AutoPtr<IByteBuffer> vBuf;
    vPlane->GetBuffer((IByteBuffer**)&vBuf);

    IBuffer::Probe(yBuf)->Rewind();
    IBuffer::Probe(uBuf)->Rewind();
    IBuffer::Probe(vBuf)->Rewind();

    Int32 yRowStride;
    yPlane->GetRowStride(&yRowStride);
    Int32 vRowStride;
    vPlane->GetRowStride(&vRowStride);
    Int32 uRowStride;
    uPlane->GetRowStride(&uRowStride);

    Int32 yPixStride;
    yPlane->GetPixelStride(&yPixStride);
    Int32 vPixStride;
    vPlane->GetPixelStride(&vPixStride);
    Int32 uPixStride;
    uPlane->GetPixelStride(&uPixStride);

    AutoPtr<ArrayOf<Byte> > yuvPixel = ArrayOf<Byte>::Alloc(3);
    (*yuvPixel)[0] = 0;
    (*yuvPixel)[1] = 0;
    (*yuvPixel)[2] = 0;
    AutoPtr<ArrayOf<Byte> > yFullRow = ArrayOf<Byte>::Alloc(yPixStride * width);
    AutoPtr<ArrayOf<Byte> > uFullRow = ArrayOf<Byte>::Alloc(uPixStride * width / 2);
    AutoPtr<ArrayOf<Byte> > vFullRow = ArrayOf<Byte>::Alloc(vPixStride * width / 2);
    AutoPtr<ArrayOf<Byte> > finalRow = ArrayOf<Byte>::Alloc(BYTES_PER_RGB_PIX * width);
    for (Int32 i = 0; i < height; i++) {
        Int32 halfH = i / 2;
        IBuffer::Probe(yBuf)->SetPosition(yRowStride * i);
        yBuf->Get(yFullRow);
        IBuffer::Probe(uBuf)->SetPosition(uRowStride * halfH);
        uBuf->Get(uFullRow);
        IBuffer::Probe(vBuf)->SetPosition(vRowStride * halfH);
        vBuf->Get(vFullRow);
        for (Int32 j = 0; j < width; j++) {
            Int32 halfW = j / 2;
            (*yuvPixel)[0] = (*yFullRow)[yPixStride * j];
            (*yuvPixel)[1] = (*uFullRow)[uPixStride * halfW];
            (*yuvPixel)[2] = (*vFullRow)[vPixStride * halfW];
            YuvToRgb(yuvPixel, j * BYTES_PER_RGB_PIX, /*out*/finalRow);
        }
        buf->Put(finalRow);
    }

    IBuffer::Probe(yBuf)->Rewind();
    IBuffer::Probe(uBuf)->Rewind();
    IBuffer::Probe(vBuf)->Rewind();
    IBuffer::Probe(buf)->Rewind();
    return buf;
}

AutoPtr<IByteBuffer> DngCreator::ConvertToRGB(
    /* [in] */ IBitmap* argbBitmap)
{
    // TODO: Optimize this.
    Int32 width;
    argbBitmap->GetWidth(&width);
    Int32 height;
    argbBitmap->GetHeight(&height);

    AutoPtr<IByteBufferHelper> bufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferHelper);
    AutoPtr<IByteBuffer> buf;
    bufferHelper->AllocateDirect(BYTES_PER_RGB_PIX * width * height, (IByteBuffer**)&buf);

    AutoPtr<ArrayOf<Int32> > pixelRow = ArrayOf<Int32>::Alloc(width);
    AutoPtr<ArrayOf<Byte> > finalRow = ArrayOf<Byte>::Alloc(BYTES_PER_RGB_PIX * width);
    for (Int32 i = 0; i < height; i++) {
        argbBitmap->GetPixels(pixelRow, /*offset*/0, /*stride*/width, /*x*/0, /*y*/i,
                /*width*/width, /*height*/1);
        for (Int32 j = 0; j < width; j++) {
            ColorToRgb((*pixelRow)[j], j * BYTES_PER_RGB_PIX, /*out*/finalRow);
        }
        buf->Put(finalRow);
    }

    IBuffer::Probe(buf)->Rewind();
    return buf;
}

AutoPtr<ArrayOf<Int32> > DngCreator::ToExifLatLong(
    /* [in] */ Double value)
{
    // convert to the format dd/1 mm/1 ssss/100
    value = Elastos::Core::Math::Abs(value);
    Int32 degrees = (Int32)value;
    value = (value - degrees) * 60;
    Int32 minutes = (Int32)value;
    value = (value - minutes) * 6000;
    Int32 seconds = (Int32)value;
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(6);
    (*array)[0] = degrees;
    (*array)[1] = 1;
    (*array)[2] = minutes;
    (*array)[3] = 1;
    (*array)[4] = seconds;
    (*array)[5] = 100;
    return array;
}

Elastos::Droid::Hardware::Camera2::DngCreator::NativeContext* DngCreator::DngCreator_getNativeContext()
{
    ALOGV("%s:", __FUNCTION__);
    return reinterpret_cast<NativeContext*>(mNativeContext);
}

void DngCreator::DngCreator_setNativeContext(
    /* [in] */ android::sp<NativeContext> context)
{
    ALOGV("%s:", __FUNCTION__);
    NativeContext* current = DngCreator_getNativeContext();

    if (context != NULL) {
        context->incStrong((void*) DngCreator_setNativeContext);
    }

    if (current) {
        current->decStrong((void*) DngCreator_setNativeContext);
    }

    mNativeContext = reinterpret_cast<Int64>(context.get());
}

TiffWriter* DngCreator::DngCreator_getCreator()
{
    ALOGV("%s:", __FUNCTION__);
    NativeContext* current = DngCreator_getNativeContext();
    if (current) {
        return current->getWriter();
    }
    return NULL;
}

Int32 DngCreator::NativeClassInit()
{
    ALOGV("%s:", __FUNCTION__);

    // gDngCreatorClassInfo.mNativeContext = env->GetFieldID(clazz,
    //         ANDROID_DNGCREATOR_CTX_JNI_ID, "J");
    // LOG_ALWAYS_FATAL_IF(gDngCreatorClassInfo.mNativeContext == NULL,
    //         "can't find android/hardware/camera2/DngCreator.%s",
    //         ANDROID_DNGCREATOR_CTX_JNI_ID);

    // jclass outputStreamClazz = env->FindClass("java/io/OutputStream");
    // LOG_ALWAYS_FATAL_IF(outputStreamClazz == NULL, "Can't find java/io/OutputStream class");
    // gOutputStreamClassInfo.mWriteMethod = env->GetMethodID(outputStreamClazz, "write", "([BII)V");
    // LOG_ALWAYS_FATAL_IF(gOutputStreamClassInfo.mWriteMethod == NULL, "Can't find write method");

    // jclass inputStreamClazz = env->FindClass("java/io/InputStream");
    // LOG_ALWAYS_FATAL_IF(inputStreamClazz == NULL, "Can't find java/io/InputStream class");
    // gInputStreamClassInfo.mReadMethod = env->GetMethodID(inputStreamClazz, "read", "([BII)I");
    // LOG_ALWAYS_FATAL_IF(gInputStreamClassInfo.mReadMethod == NULL, "Can't find read method");
    // gInputStreamClassInfo.mSkipMethod = env->GetMethodID(inputStreamClazz, "skip", "(J)J");
    // LOG_ALWAYS_FATAL_IF(gInputStreamClassInfo.mSkipMethod == NULL, "Can't find skip method");

    // jclass inputBufferClazz = env->FindClass("java/nio/ByteBuffer");
    // LOG_ALWAYS_FATAL_IF(inputBufferClazz == NULL, "Can't find java/nio/ByteBuffer class");
    // gInputByteBufferClassInfo.mGetMethod = env->GetMethodID(inputBufferClazz, "get",
    //         "([BII)Ljava/nio/ByteBuffer;");
    // LOG_ALWAYS_FATAL_IF(gInputByteBufferClassInfo.mGetMethod == NULL, "Can't find get method");
    return 0;
}

ECode DngCreator::NativeInit(
    /* [in] */ ICameraMetadataNative* nativeCharacteristics,
    /* [in] */ ICameraMetadataNative* nativeResult,
    /* [in] */ const String& formattedCaptureTime)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);
        android::CameraMetadata characteristics;
        android::CameraMetadata results;
        assert(0 && "need CameraMetadata_getNativeMetadata");
        // if (CameraMetadata_getNativeMetadata(env, characteristicsPtr, &characteristics) != android::OK) {
        //     // jniThrowException(env, "java/lang/AssertionError",
        //     //         "No native metadata defined for camera characteristics.");
        //     Slogger::E(TAG, "No native metadata defined for camera characteristics.");
        //     return E_ASSERTION_ERROR;
        // }
        // if (CameraMetadata_getNativeMetadata(env, resultsPtr, &results) != android::OK) {
        //     // jniThrowException(env, "java/lang/AssertionError",
        //     //         "No native metadata defined for capture results.");
        //     Slogger::E(TAG, "No native metadata defined for capture results.");
        //     return E_ASSERTION_ERROR;
        // }

        android::sp<NativeContext> nativeContext = new NativeContext();
        TiffWriter* writer = nativeContext->getWriter();

        writer->addIfd(TIFF_IFD_0);

        status_t err = android::OK;

        const uint32_t samplesPerPixel = 1;
        const uint32_t bitsPerSample = BITS_PER_SAMPLE;
        const uint32_t bitsPerByte = BITS_PER_SAMPLE / BYTES_PER_SAMPLE;
        uint32_t imageWidth = 0;
        uint32_t imageHeight = 0;

        OpcodeListBuilder::CfaLayout opcodeCfaLayout = OpcodeListBuilder::CFA_RGGB;
        uint8_t cfaPlaneColor[3] = {0, 1, 2};
        uint8_t cfaEnum = -1;

        // TODO: Greensplit.
        // TODO: Add remaining non-essential tags

        // Setup main image tags

        {
            // Set orientation
            uint16_t orientation = 1; // Normal
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_ORIENTATION, 1, &orientation, TIFF_IFD_0),
                    android::img_utils::TAG_ORIENTATION, writer);
        }

        {
            // Set subfiletype
            uint32_t subfileType = 0; // Main image
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_NEWSUBFILETYPE, 1, &subfileType, TIFF_IFD_0),
                    android::img_utils::TAG_NEWSUBFILETYPE, writer);
        }

        {
            // Set bits per sample
            uint16_t bits = static_cast<uint16_t>(bitsPerSample);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_BITSPERSAMPLE, 1, &bits, TIFF_IFD_0),
                    android::img_utils::TAG_BITSPERSAMPLE, writer);
        }

        {
            // Set compression
            uint16_t compression = 1; // None
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_COMPRESSION, 1, &compression, TIFF_IFD_0),
                    android::img_utils::TAG_COMPRESSION, writer);
        }

        {
            // Set dimensions
            camera_metadata_entry entry =
                    characteristics.find(ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_IMAGEWIDTH, writer);
            uint32_t width = static_cast<uint32_t>(entry.data.i32[2]);
            uint32_t height = static_cast<uint32_t>(entry.data.i32[3]);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_IMAGEWIDTH, 1, &width, TIFF_IFD_0),
                    android::img_utils::TAG_IMAGEWIDTH, writer);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_IMAGELENGTH, 1, &height, TIFF_IFD_0),
                    android::img_utils::TAG_IMAGELENGTH, writer);
            imageWidth = width;
            imageHeight = height;
        }

        {
            // Set photometric interpretation
            uint16_t interpretation = 32803; // CFA
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_PHOTOMETRICINTERPRETATION, 1, &interpretation,
                    TIFF_IFD_0), android::img_utils::TAG_PHOTOMETRICINTERPRETATION, writer);
        }

        {
            // Set blacklevel tags
            camera_metadata_entry entry =
                    characteristics.find(ANDROID_SENSOR_BLACK_LEVEL_PATTERN);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_BLACKLEVEL, writer);
            const uint32_t* blackLevel = reinterpret_cast<const uint32_t*>(entry.data.i32);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_BLACKLEVEL, entry.count, blackLevel, TIFF_IFD_0),
                    android::img_utils::TAG_BLACKLEVEL, writer);

            uint16_t repeatDim[2] = {2, 2};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_BLACKLEVELREPEATDIM, 2, repeatDim, TIFF_IFD_0),
                    android::img_utils::TAG_BLACKLEVELREPEATDIM, writer);
        }

        {
            // Set samples per pixel
            uint16_t samples = static_cast<uint16_t>(samplesPerPixel);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_SAMPLESPERPIXEL, 1, &samples, TIFF_IFD_0),
                    android::img_utils::TAG_SAMPLESPERPIXEL, writer);
        }

        {
            // Set planar configuration
            uint16_t config = 1; // Chunky
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_PLANARCONFIGURATION, 1, &config, TIFF_IFD_0),
                    android::img_utils::TAG_PLANARCONFIGURATION, writer);
        }

        {
            // Set CFA pattern dimensions
            uint16_t repeatDim[2] = {2, 2};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CFAREPEATPATTERNDIM, 2, repeatDim, TIFF_IFD_0),
                    android::img_utils::TAG_CFAREPEATPATTERNDIM, writer);
        }

        {
            // Set CFA pattern
            camera_metadata_entry entry =
                            characteristics.find(ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_CFAPATTERN, writer);

            const int cfaLength = 4;
            cfaEnum = entry.data.u8[0];
            uint8_t cfa[cfaLength];
            if ((err = convertCFA(cfaEnum, /*out*/cfa)) != android::OK) {
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //             "Invalid metadata for tag %d", TAG_CFAPATTERN);
                Slogger::E(TAG, "Invalid metadata for tag %d", android::img_utils::TAG_CFAPATTERN);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CFAPATTERN, cfaLength, cfa, TIFF_IFD_0),
                    android::img_utils::TAG_CFAPATTERN, writer);

            opcodeCfaLayout = convertCFAEnumToOpcodeLayout(cfaEnum);
        }

        {
            // Set CFA plane color
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CFAPLANECOLOR, 3, cfaPlaneColor, TIFF_IFD_0),
                    android::img_utils::TAG_CFAPLANECOLOR, writer);
        }

        {
            // Set CFA layout
            uint16_t cfaLayout = 1;
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CFALAYOUT, 1, &cfaLayout, TIFF_IFD_0),
                    android::img_utils::TAG_CFALAYOUT, writer);
        }

        {
            // image description
            uint8_t imageDescription = '\0'; // empty
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_IMAGEDESCRIPTION, 1, &imageDescription, TIFF_IFD_0),
                    android::img_utils::TAG_IMAGEDESCRIPTION, writer);
        }

        {
            // make
            char manufacturer[PROPERTY_VALUE_MAX];

            // Use "" to represent unknown make as suggested in TIFF/EP spec.
            property_get("ro.product.manufacturer", manufacturer, "");
            uint32_t count = static_cast<uint32_t>(strlen(manufacturer)) + 1;

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_MAKE, count, reinterpret_cast<uint8_t*>(manufacturer),
                    TIFF_IFD_0), android::img_utils::TAG_MAKE, writer);
        }

        {
            // model
            char model[PROPERTY_VALUE_MAX];

            // Use "" to represent unknown model as suggested in TIFF/EP spec.
            property_get("ro.product.model", model, "");
            uint32_t count = static_cast<uint32_t>(strlen(model)) + 1;

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_MODEL, count, reinterpret_cast<uint8_t*>(model),
                    TIFF_IFD_0), android::img_utils::TAG_MODEL, writer);
        }

        {
            // x resolution
            uint32_t xres[] = { 72, 1 }; // default 72 ppi
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_XRESOLUTION, 1, xres, TIFF_IFD_0),
                    android::img_utils::TAG_XRESOLUTION, writer);

            // y resolution
            uint32_t yres[] = { 72, 1 }; // default 72 ppi
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_YRESOLUTION, 1, yres, TIFF_IFD_0),
                    android::img_utils::TAG_YRESOLUTION, writer);

            uint16_t unit = 2; // inches
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_RESOLUTIONUNIT, 1, &unit, TIFF_IFD_0),
                    android::img_utils::TAG_RESOLUTIONUNIT, writer);
        }

        {
            // software
            char software[PROPERTY_VALUE_MAX];
            property_get("ro.build.fingerprint", software, "");
            uint32_t count = static_cast<uint32_t>(strlen(software)) + 1;
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_SOFTWARE, count,
                    reinterpret_cast<uint8_t*>(software), TIFF_IFD_0), android::img_utils::TAG_SOFTWARE, writer);
        }

        {
            // datetime
            const size_t DATETIME_COUNT = 20;
            const char* captureTime = formattedCaptureTime.string();

            size_t len = strlen(captureTime) + 1;
            if (len != DATETIME_COUNT) {
                // jniThrowException(env, "java/lang/IllegalArgumentException",
                //         "Timestamp string length is not required 20 characters");
                Slogger::E(TAG, "Timestamp string length is not required 20 characters");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            if (writer->addEntry(android::img_utils::TAG_DATETIME, DATETIME_COUNT,
                    reinterpret_cast<const uint8_t*>(captureTime), TIFF_IFD_0) != android::OK) {
                // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
                //         "Invalid metadata for tag %x", TAG_DATETIME);
                Slogger::E(TAG, "Invalid metadata for tag %x", android::img_utils::TAG_DATETIME);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // datetime original
            if (writer->addEntry(android::img_utils::TAG_DATETIMEORIGINAL, DATETIME_COUNT,
                    reinterpret_cast<const uint8_t*>(captureTime), TIFF_IFD_0) != android::OK) {
                // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
                //         "Invalid metadata for tag %x", TAG_DATETIMEORIGINAL);
                Slogger::E(TAG, "Invalid metadata for tag %x", android::img_utils::TAG_DATETIMEORIGINAL);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        {
            // TIFF/EP standard id
            uint8_t standardId[] = { 1, 0, 0, 0 };
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_TIFFEPSTANDARDID, 4, standardId,
                    TIFF_IFD_0), android::img_utils::TAG_TIFFEPSTANDARDID, writer);
        }

        {
            // copyright
            uint8_t copyright = '\0'; // empty
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_COPYRIGHT, 1, &copyright,
                    TIFF_IFD_0), android::img_utils::TAG_COPYRIGHT, writer);
        }

        {
            // exposure time
            camera_metadata_entry entry =
                results.find(ANDROID_SENSOR_EXPOSURE_TIME);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_EXPOSURETIME, writer);

            int64_t exposureTime = *(entry.data.i64);

            if (exposureTime < 0) {
                // Should be unreachable
                // jniThrowException(env, "java/lang/IllegalArgumentException",
                //         "Negative exposure time in metadata");
                Slogger::E(TAG, "Negative exposure time in metadata");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // Ensure exposure time doesn't overflow (for exposures > 4s)
            uint32_t denominator = 1000000000;
            while (exposureTime > UINT32_MAX) {
                exposureTime >>= 1;
                denominator >>= 1;
                if (denominator == 0) {
                    // Should be unreachable
                    // jniThrowException(env, "java/lang/IllegalArgumentException",
                    //         "Exposure time too long");
                    Slogger::E(TAG, "Exposure time too long");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }

            uint32_t exposure[] = { static_cast<uint32_t>(exposureTime), denominator };
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_EXPOSURETIME, 1, exposure,
                    TIFF_IFD_0), android::img_utils::TAG_EXPOSURETIME, writer);

        }

        {
            // ISO speed ratings
            camera_metadata_entry entry =
                results.find(ANDROID_SENSOR_SENSITIVITY);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_ISOSPEEDRATINGS, writer);

            int32_t tempIso = *(entry.data.i32);
            if (tempIso < 0) {
                // jniThrowException(env, "java/lang/IllegalArgumentException",
                //                         "Negative ISO value");
                Slogger::E(TAG, "Negative ISO value");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            if (tempIso > UINT16_MAX) {
                ALOGW("%s: ISO value overflows UINT16_MAX, clamping to max", __FUNCTION__);
                tempIso = UINT16_MAX;
            }

            uint16_t iso = static_cast<uint16_t>(tempIso);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_ISOSPEEDRATINGS, 1, &iso,
                    TIFF_IFD_0), android::img_utils::TAG_ISOSPEEDRATINGS, writer);
        }

        {
            // focal length
            camera_metadata_entry entry =
                results.find(ANDROID_LENS_FOCAL_LENGTH);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_FOCALLENGTH, writer);

            uint32_t focalLength[] = { static_cast<uint32_t>(*(entry.data.f) * 100), 100 };
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_FOCALLENGTH, 1, focalLength,
                    TIFF_IFD_0), android::img_utils::TAG_FOCALLENGTH, writer);
        }

        {
            // f number
            camera_metadata_entry entry =
                results.find(ANDROID_LENS_APERTURE);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_FNUMBER, writer);

            uint32_t fnum[] = { static_cast<uint32_t>(*(entry.data.f) * 100), 100 };
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_FNUMBER, 1, fnum,
                    TIFF_IFD_0), android::img_utils::TAG_FNUMBER, writer);
        }

        {
            // Set DNG version information
            uint8_t version[4] = {1, 4, 0, 0};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_DNGVERSION, 4, version, TIFF_IFD_0),
                    android::img_utils::TAG_DNGVERSION, writer);

            uint8_t backwardVersion[4] = {1, 1, 0, 0};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_DNGBACKWARDVERSION, 4, backwardVersion, TIFF_IFD_0),
                    android::img_utils::TAG_DNGBACKWARDVERSION, writer);
        }

        {
            // Set whitelevel
            camera_metadata_entry entry =
                    characteristics.find(ANDROID_SENSOR_INFO_WHITE_LEVEL);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_WHITELEVEL, writer);
            uint32_t whiteLevel = static_cast<uint32_t>(entry.data.i32[0]);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_WHITELEVEL, 1, &whiteLevel, TIFF_IFD_0),
                    android::img_utils::TAG_WHITELEVEL, writer);
        }

        {
            // Set default scale
            uint32_t defaultScale[4] = {1, 1, 1, 1};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_DEFAULTSCALE, 2, defaultScale, TIFF_IFD_0),
                    android::img_utils::TAG_DEFAULTSCALE, writer);
        }

        bool singleIlluminant = false;
        {
            // Set calibration illuminants
            camera_metadata_entry entry1 =
                characteristics.find(ANDROID_SENSOR_REFERENCE_ILLUMINANT1);
            BAIL_IF_EMPTY(entry1, android::img_utils::TAG_CALIBRATIONILLUMINANT1, writer);
            camera_metadata_entry entry2 =
                characteristics.find(ANDROID_SENSOR_REFERENCE_ILLUMINANT2);
            if (entry2.count == 0) {
                singleIlluminant = true;
            }
            uint16_t ref1 = entry1.data.u8[0];

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CALIBRATIONILLUMINANT1, 1, &ref1,
                    TIFF_IFD_0), android::img_utils::TAG_CALIBRATIONILLUMINANT1, writer);

            if (!singleIlluminant) {
                uint16_t ref2 = entry2.data.u8[0];
                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CALIBRATIONILLUMINANT2, 1, &ref2,
                        TIFF_IFD_0), android::img_utils::TAG_CALIBRATIONILLUMINANT2, writer);
            }
        }

        {
            // Set color transforms
            camera_metadata_entry entry1 =
                characteristics.find(ANDROID_SENSOR_COLOR_TRANSFORM1);
            BAIL_IF_EMPTY(entry1, android::img_utils::TAG_COLORMATRIX1, writer);

            int32_t colorTransform1[entry1.count * 2];

            size_t ctr = 0;
            for(size_t i = 0; i < entry1.count; ++i) {
                colorTransform1[ctr++] = entry1.data.r[i].numerator;
                colorTransform1[ctr++] = entry1.data.r[i].denominator;
            }

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_COLORMATRIX1, entry1.count, colorTransform1,
                    TIFF_IFD_0), android::img_utils::TAG_COLORMATRIX1, writer);

            if (!singleIlluminant) {
                camera_metadata_entry entry2 = characteristics.find(ANDROID_SENSOR_COLOR_TRANSFORM2);
                BAIL_IF_EMPTY(entry2, android::img_utils::TAG_COLORMATRIX2, writer);
                int32_t colorTransform2[entry2.count * 2];

                ctr = 0;
                for(size_t i = 0; i < entry2.count; ++i) {
                    colorTransform2[ctr++] = entry2.data.r[i].numerator;
                    colorTransform2[ctr++] = entry2.data.r[i].denominator;
                }

                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_COLORMATRIX2, entry2.count, colorTransform2,
                        TIFF_IFD_0), android::img_utils::TAG_COLORMATRIX2, writer);
            }
        }

        {
            // Set calibration transforms
            camera_metadata_entry entry1 =
                characteristics.find(ANDROID_SENSOR_CALIBRATION_TRANSFORM1);
            BAIL_IF_EMPTY(entry1, android::img_utils::TAG_CAMERACALIBRATION1, writer);

            int32_t calibrationTransform1[entry1.count * 2];

            size_t ctr = 0;
            for(size_t i = 0; i < entry1.count; ++i) {
                calibrationTransform1[ctr++] = entry1.data.r[i].numerator;
                calibrationTransform1[ctr++] = entry1.data.r[i].denominator;
            }

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CAMERACALIBRATION1, entry1.count,
                    calibrationTransform1, TIFF_IFD_0), android::img_utils::TAG_CAMERACALIBRATION1, writer);

            if (!singleIlluminant) {
                camera_metadata_entry entry2 =
                    characteristics.find(ANDROID_SENSOR_CALIBRATION_TRANSFORM2);
                BAIL_IF_EMPTY(entry2, android::img_utils::TAG_CAMERACALIBRATION2, writer);
                int32_t calibrationTransform2[entry2.count * 2];

                ctr = 0;
                for(size_t i = 0; i < entry2.count; ++i) {
                    calibrationTransform2[ctr++] = entry2.data.r[i].numerator;
                    calibrationTransform2[ctr++] = entry2.data.r[i].denominator;
                }

                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_CAMERACALIBRATION2, entry2.count,
                        calibrationTransform1, TIFF_IFD_0), android::img_utils::TAG_CAMERACALIBRATION2, writer);
            }
        }

        {
            // Set forward transforms
            camera_metadata_entry entry1 =
                characteristics.find(ANDROID_SENSOR_FORWARD_MATRIX1);
            BAIL_IF_EMPTY(entry1, android::img_utils::TAG_FORWARDMATRIX1, writer);

            int32_t forwardTransform1[entry1.count * 2];

            size_t ctr = 0;
            for(size_t i = 0; i < entry1.count; ++i) {
                forwardTransform1[ctr++] = entry1.data.r[i].numerator;
                forwardTransform1[ctr++] = entry1.data.r[i].denominator;
            }

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_FORWARDMATRIX1, entry1.count, forwardTransform1,
                    TIFF_IFD_0), android::img_utils::TAG_FORWARDMATRIX1, writer);

            if (!singleIlluminant) {
                camera_metadata_entry entry2 =
                    characteristics.find(ANDROID_SENSOR_FORWARD_MATRIX2);
                BAIL_IF_EMPTY(entry2, android::img_utils::TAG_FORWARDMATRIX2, writer);
                int32_t forwardTransform2[entry2.count * 2];

                ctr = 0;
                for(size_t i = 0; i < entry2.count; ++i) {
                    forwardTransform2[ctr++] = entry2.data.r[i].numerator;
                    forwardTransform2[ctr++] = entry2.data.r[i].denominator;
                }

                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_FORWARDMATRIX2, entry2.count, forwardTransform2,
                        TIFF_IFD_0), android::img_utils::TAG_FORWARDMATRIX2, writer);
            }
        }

        {
            // Set camera neutral
            camera_metadata_entry entry =
                results.find(ANDROID_SENSOR_NEUTRAL_COLOR_POINT);
            BAIL_IF_EMPTY(entry, android::img_utils::TAG_ASSHOTNEUTRAL, writer);
            uint32_t cameraNeutral[entry.count * 2];

            size_t ctr = 0;
            for(size_t i = 0; i < entry.count; ++i) {
                cameraNeutral[ctr++] =
                        static_cast<uint32_t>(entry.data.r[i].numerator);
                cameraNeutral[ctr++] =
                        static_cast<uint32_t>(entry.data.r[i].denominator);
            }

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_ASSHOTNEUTRAL, entry.count, cameraNeutral,
                    TIFF_IFD_0), android::img_utils::TAG_ASSHOTNEUTRAL, writer);
        }

        {
            // Setup data strips
            // TODO: Switch to tiled implementation.
            if (writer->addStrip(TIFF_IFD_0) != android::OK) {
                ALOGE("%s: Could not setup strip tags.", __FUNCTION__);
                // jniThrowException(env, "java/lang/IllegalStateException",
                //         "Failed to setup strip tags.");
                Slogger::E(TAG, "Failed to setup strip tags.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        {
            // Setup default crop + crop origin tags
            uint32_t margin = 8; // Default margin recommended by Adobe for interpolation.
            uint32_t dimensionLimit = 128; // Smallest image dimension crop margin from.
            if (imageWidth >= dimensionLimit && imageHeight >= dimensionLimit) {
                uint32_t defaultCropOrigin[] = {margin, margin};
                uint32_t defaultCropSize[] = {imageWidth - margin, imageHeight - margin};
                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_DEFAULTCROPORIGIN, 2, defaultCropOrigin,
                        TIFF_IFD_0), android::img_utils::TAG_DEFAULTCROPORIGIN, writer);
                BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_DEFAULTCROPSIZE, 2, defaultCropSize,
                        TIFF_IFD_0), android::img_utils::TAG_DEFAULTCROPSIZE, writer);
            }
        }

        {
            // Setup unique camera model tag
            char model[PROPERTY_VALUE_MAX];
            property_get("ro.product.model", model, "");

            char manufacturer[PROPERTY_VALUE_MAX];
            property_get("ro.product.manufacturer", manufacturer, "");

            char brand[PROPERTY_VALUE_MAX];
            property_get("ro.product.brand", brand, "");

            android::String8 cameraModel(model);
            cameraModel += "-";
            cameraModel += manufacturer;
            cameraModel += "-";
            cameraModel += brand;

            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_UNIQUECAMERAMODEL, cameraModel.size() + 1,
                    reinterpret_cast<const uint8_t*>(cameraModel.string()), TIFF_IFD_0),
                    android::img_utils::TAG_UNIQUECAMERAMODEL, writer);
        }

        {
            // Setup sensor noise model
            camera_metadata_entry entry =
                results.find(ANDROID_SENSOR_NOISE_PROFILE);

            const status_t numPlaneColors = 3;
            const status_t numCfaChannels = 4;

            uint8_t cfaOut[numCfaChannels];
            if ((err = convertCFA(cfaEnum, /*out*/cfaOut)) != android::OK) {
                // jniThrowException(env, "java/lang/IllegalArgumentException",
                //         "Invalid CFA from camera characteristics");
                Slogger::E(TAG, "Invalid CFA from camera characteristics");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            double noiseProfile[numPlaneColors * 2];

            if (entry.count > 0) {
                if (entry.count != numCfaChannels * 2) {
                    ALOGW("%s: Invalid entry count %u for noise profile returned in characteristics,"
                            " no noise profile tag written...", __FUNCTION__, entry.count);
                }
                else {
                    if ((err = generateNoiseProfile(entry.data.d, cfaOut, numCfaChannels,
                            cfaPlaneColor, numPlaneColors, /*out*/ noiseProfile)) == android::OK) {

                        BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_NOISEPROFILE, numPlaneColors * 2,
                                noiseProfile, TIFF_IFD_0), android::img_utils::TAG_NOISEPROFILE, writer);
                    }
                    else {
                        ALOGW("%s: Error converting coefficients for noise profile, no noise profile"
                                " tag written...", __FUNCTION__);
                    }
                }
            }
            else {
                ALOGW("%s: No noise profile found in result metadata.  Image quality may be reduced.",
                        __FUNCTION__);
            }
        }

        {
            // Setup opcode List 2
            camera_metadata_entry entry1 =
                    characteristics.find(ANDROID_LENS_INFO_SHADING_MAP_SIZE);

            uint32_t lsmWidth = 0;
            uint32_t lsmHeight = 0;

            if (entry1.count != 0) {
                lsmWidth = static_cast<uint32_t>(entry1.data.i32[0]);
                lsmHeight = static_cast<uint32_t>(entry1.data.i32[1]);
            }

            camera_metadata_entry entry2 =
                    results.find(ANDROID_STATISTICS_LENS_SHADING_MAP);

            if (entry2.count > 0 && entry2.count == lsmWidth * lsmHeight * 4) {

                OpcodeListBuilder builder;
                status_t err = builder.addGainMapsForMetadata(lsmWidth,
                                                              lsmHeight,
                                                              0,
                                                              0,
                                                              imageHeight,
                                                              imageWidth,
                                                              opcodeCfaLayout,
                                                              entry2.data.f);
                if (err == android::OK) {
                    size_t listSize = builder.getSize();
                    uint8_t opcodeListBuf[listSize];
                    err = builder.buildOpList(opcodeListBuf);
                    if (err == android::OK) {
                        BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_OPCODELIST2, listSize, opcodeListBuf,
                                TIFF_IFD_0), android::img_utils::TAG_OPCODELIST2, writer);
                    }
                    else {
                        ALOGE("%s: Could not build Lens shading map opcode.", __FUNCTION__);
                        //jniThrowRuntimeException(env, "failed to construct lens shading map opcode.");
                        Slogger::E(TAG, "failed to construct lens shading map opcode.");
                        return E_RUNTIME_EXCEPTION;
                    }
                }
                else {
                    ALOGE("%s: Could not add Lens shading map.", __FUNCTION__);
                    //jniThrowRuntimeException(env, "failed to add lens shading map.");
                    Slogger::E(TAG, "failed to add lens shading map.");
                    return E_RUNTIME_EXCEPTION;
                }
            }
            else {
                ALOGW("%s: No lens shading map found in result metadata. Image quality may be reduced.",
                        __FUNCTION__);
            }
        }

        DngCreator_setNativeContext(nativeContext);
    }
}

void DngCreator::NativeDestroy(){
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);
        DngCreator_setNativeContext(NULL);
    }
}

ECode DngCreator::NativeSetOrientation(
    /* [in] */ Int32 orient)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);

        TiffWriter* writer = DngCreator_getCreator();
        if (writer == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "setOrientation called with uninitialized DngCreator");
            Slogger::E(TAG, "setOrientation called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        uint16_t orientation = static_cast<uint16_t>(orient);
        BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_ORIENTATION, 1, &orientation, TIFF_IFD_0),
                    android::img_utils::TAG_ORIENTATION, writer);

        // Set main image orientation also if in a separate IFD
        if (writer->hasIfd(TIFF_IFD_SUB1)) {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_ORIENTATION, 1, &orientation,
                    TIFF_IFD_SUB1), android::img_utils::TAG_ORIENTATION, writer);
        }
    }
    return NOERROR;
}

ECode DngCreator::NativeSetDescription(
    /* [in] */ const String& description)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);

        TiffWriter* writer = DngCreator_getCreator();
        if (writer == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "setDescription called with uninitialized DngCreator");
            Slogger::E(TAG, "setDescription called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        const char* desc = description.string();
        size_t len = strlen(desc) + 1;

        if (writer->addEntry(android::img_utils::TAG_IMAGEDESCRIPTION, len,
                reinterpret_cast<const uint8_t*>(desc), TIFF_IFD_0) != android::OK) {
            // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
            //         "Invalid metadata for tag %x", TAG_IMAGEDESCRIPTION);
            Slogger::E(TAG, "Invalid metadata for tag %x", android::img_utils::TAG_IMAGEDESCRIPTION);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode DngCreator::NativeSetGpsTags(
    /* [in] */ ArrayOf<Int32>* latTag,
    /* [in] */ const String& latRef,
    /* [in] */ ArrayOf<Int32>* longTag,
    /* [in] */ const String& longRef,
    /* [in] */ const String& dateTag,
    /* [in] */ ArrayOf<Int32>* timeTag)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);

        TiffWriter* writer = DngCreator_getCreator();
        if (writer == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "setGpsTags called with uninitialized DngCreator");
            Slogger::E(TAG, "setGpsTags called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        if (!writer->hasIfd(TIFF_IFD_GPSINFO)) {
            if (writer->addSubIfd(TIFF_IFD_0, TIFF_IFD_GPSINFO, TiffWriter::GPSINFO) != android::OK) {
                ALOGE("%s: Failed to add GpsInfo IFD %u to IFD %u", __FUNCTION__, TIFF_IFD_GPSINFO,
                        TIFF_IFD_0);
                //jniThrowException(env, "java/lang/IllegalStateException", "Failed to add GPSINFO");
                Slogger::E(TAG, "Failed to add GPSINFO");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        const Int32 GPS_VALUE_LENGTH = 6;
        Int32 latLen = latTag->GetLength();
        Int32 longLen = longTag->GetLength();
        Int32 timeLen = timeTag->GetLength();
        if (latLen != GPS_VALUE_LENGTH) {
            // jniThrowException(env, "java/lang/IllegalArgumentException",
            //         "invalid latitude tag length");
            Slogger::E(TAG, "invalid latitude tag length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (longLen != GPS_VALUE_LENGTH) {
            // jniThrowException(env, "java/lang/IllegalArgumentException",
            //         "invalid longitude tag length");
            Slogger::E(TAG, "invalid longitude tag length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (timeLen != GPS_VALUE_LENGTH) {
            // jniThrowException(env, "java/lang/IllegalArgumentException",
            //         "invalid time tag length");
            Slogger::E(TAG, "invalid time tag length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        uint32_t latitude[GPS_VALUE_LENGTH];
        uint32_t longitude[GPS_VALUE_LENGTH];
        uint32_t timestamp[GPS_VALUE_LENGTH];

        latTag->Copy(0, reinterpret_cast<Int32*>(&latitude), static_cast<Int32>(GPS_VALUE_LENGTH));
        longTag->Copy(0, reinterpret_cast<Int32*>(&longitude), static_cast<Int32>(GPS_VALUE_LENGTH));
        timeTag->Copy(0, reinterpret_cast<Int32*>(&timestamp), static_cast<Int32>(GPS_VALUE_LENGTH));

        const Int32 GPS_REF_LENGTH = 2;
        const Int32 GPS_DATE_LENGTH = 11;
        uint8_t latitudeRef[GPS_REF_LENGTH];
        uint8_t longitudeRef[GPS_REF_LENGTH];
        uint8_t date[GPS_DATE_LENGTH];

        latitudeRef[0] = (uint8_t)latRef.GetChar(0);
        latitudeRef[GPS_REF_LENGTH - 1] = '\0';
        longitudeRef[0] = (uint8_t)longRef.GetChar(0);
        longitudeRef[GPS_REF_LENGTH - 1] = '\0';

        date[0] = (uint8_t)dateTag.GetChar(0);
        date[1] = (uint8_t)dateTag.GetChar(1);
        date[2] = (uint8_t)dateTag.GetChar(2);
        date[3] = (uint8_t)dateTag.GetChar(3);
        date[4] = (uint8_t)dateTag.GetChar(4);

        date[5] = (uint8_t)dateTag.GetChar(5);
        date[6] = (uint8_t)dateTag.GetChar(6);
        date[7] = (uint8_t)dateTag.GetChar(7);
        date[8] = (uint8_t)dateTag.GetChar(8);
        date[9] = (uint8_t)dateTag.GetChar(9);
        date[GPS_DATE_LENGTH - 1] = '\0';

        {
            uint8_t version[] = {2, 3, 0, 0};
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSVERSIONID, 4, version,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSVERSIONID, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSLATITUDEREF, GPS_REF_LENGTH, latitudeRef,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSLATITUDEREF, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSLONGITUDEREF, GPS_REF_LENGTH, longitudeRef,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSLONGITUDEREF, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSLATITUDE, 3, latitude,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSLATITUDE, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSLONGITUDE, 3, longitude,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSLONGITUDE, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSTIMESTAMP, 3, timestamp,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSTIMESTAMP, writer);
        }

        {
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_GPSDATESTAMP, GPS_DATE_LENGTH, date,
                    TIFF_IFD_GPSINFO), android::img_utils::TAG_GPSDATESTAMP, writer);
        }
    }
    return NOERROR;
}

ECode DngCreator::NativeSetThumbnail(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);

        NativeContext* context = DngCreator_getNativeContext();
        TiffWriter* writer = DngCreator_getCreator();
        if (writer == NULL || context == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "setThumbnail called with uninitialized DngCreator");
            Slogger::E(TAG, "setThumbnail called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        size_t fullSize = width * height * BYTES_PER_RGB_PIXEL;
        Int32 _capacity;
        IBuffer::Probe(buffer)->GetCapacity(&_capacity);
        Int64 capacity = _capacity;
        if (capacity != fullSize) {
            // jniThrowExceptionFmt(env, "java/lang/AssertionError",
            //         "Invalid size %d for thumbnail, expected size was %d",
            //         capacity, fullSize);
            Slogger::E(TAG, "Invalid size %d for thumbnail, expected size was %d",
                    capacity, fullSize);
            return E_ASSERTION_ERROR;
        }

        uint8_t* pixelBytes = reinterpret_cast<uint8_t*>(buffer);
        if (pixelBytes == NULL) {
            ALOGE("%s: Could not get native ByteBuffer", __FUNCTION__);
            //jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid ByteBuffer");
            Slogger::E(TAG, "Invalid ByteBuffer");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (!writer->hasIfd(TIFF_IFD_SUB1)) {
            if (writer->addSubIfd(TIFF_IFD_0, TIFF_IFD_SUB1) != android::OK) {
                ALOGE("%s: Failed to add SubIFD %u to IFD %u", __FUNCTION__, TIFF_IFD_SUB1,
                        TIFF_IFD_0);
                //jniThrowException(env, "java/lang/IllegalStateException", "Failed to add SubIFD");
                Slogger::E(TAG, "Failed to add SubIFD");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            android::Vector<uint16_t> tagsToMove;
            tagsToMove.add(android::img_utils::TAG_ORIENTATION);
            tagsToMove.add(android::img_utils::TAG_NEWSUBFILETYPE);
            tagsToMove.add(android::img_utils::TAG_BITSPERSAMPLE);
            tagsToMove.add(android::img_utils::TAG_COMPRESSION);
            tagsToMove.add(android::img_utils::TAG_IMAGEWIDTH);
            tagsToMove.add(android::img_utils::TAG_IMAGELENGTH);
            tagsToMove.add(android::img_utils::TAG_PHOTOMETRICINTERPRETATION);
            tagsToMove.add(android::img_utils::TAG_BLACKLEVEL);
            tagsToMove.add(android::img_utils::TAG_BLACKLEVELREPEATDIM);
            tagsToMove.add(android::img_utils::TAG_SAMPLESPERPIXEL);
            tagsToMove.add(android::img_utils::TAG_PLANARCONFIGURATION);
            tagsToMove.add(android::img_utils::TAG_CFAREPEATPATTERNDIM);
            tagsToMove.add(android::img_utils::TAG_CFAPATTERN);
            tagsToMove.add(android::img_utils::TAG_CFAPLANECOLOR);
            tagsToMove.add(android::img_utils::TAG_CFALAYOUT);
            tagsToMove.add(android::img_utils::TAG_XRESOLUTION);
            tagsToMove.add(android::img_utils::TAG_YRESOLUTION);
            tagsToMove.add(android::img_utils::TAG_RESOLUTIONUNIT);
            tagsToMove.add(android::img_utils::TAG_WHITELEVEL);
            tagsToMove.add(android::img_utils::TAG_DEFAULTSCALE);
            tagsToMove.add(android::img_utils::TAG_ROWSPERSTRIP);
            tagsToMove.add(android::img_utils::TAG_STRIPBYTECOUNTS);
            tagsToMove.add(android::img_utils::TAG_STRIPOFFSETS);
            tagsToMove.add(android::img_utils::TAG_DEFAULTCROPORIGIN);
            tagsToMove.add(android::img_utils::TAG_DEFAULTCROPSIZE);
            tagsToMove.add(android::img_utils::TAG_OPCODELIST2);

            if (moveEntries(writer, TIFF_IFD_0, TIFF_IFD_SUB1, tagsToMove) != android::OK) {
                //jniThrowException(env, "java/lang/IllegalStateException", "Failed to move entries");
                Slogger::E(TAG, "Failed to move entries");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // Make sure both IFDs get the same orientation tag
            android::sp<TiffEntry> orientEntry = writer->getEntry(android::img_utils::TAG_ORIENTATION, TIFF_IFD_SUB1);
            if (orientEntry != NULL) {
                writer->addEntry(orientEntry, TIFF_IFD_0);
            }
        }

        // // Setup thumbnail tags

        {
            // Set photometric interpretation
            uint16_t interpretation = 2; // RGB
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_PHOTOMETRICINTERPRETATION, 1, &interpretation,
                    TIFF_IFD_0), android::img_utils::TAG_PHOTOMETRICINTERPRETATION, writer);
        }

        {
            // Set planar configuration
            uint16_t config = 1; // Chunky
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_PLANARCONFIGURATION, 1, &config, TIFF_IFD_0),
                    android::img_utils::TAG_PLANARCONFIGURATION, writer);
        }

        {
            // Set samples per pixel
            uint16_t samples = SAMPLES_PER_RGB_PIXEL;
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_SAMPLESPERPIXEL, 1, &samples, TIFF_IFD_0),
                    android::img_utils::TAG_SAMPLESPERPIXEL, writer);
        }

        {
            // Set bits per sample
            uint16_t bits = BITS_PER_RGB_SAMPLE;
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_BITSPERSAMPLE, 1, &bits, TIFF_IFD_0),
                    android::img_utils::TAG_BITSPERSAMPLE, writer);
        }

        {
            // Set subfiletype
            uint32_t subfileType = 1; // Thumbnail image
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_NEWSUBFILETYPE, 1, &subfileType, TIFF_IFD_0),
                    android::img_utils::TAG_NEWSUBFILETYPE, writer);
        }

        {
            // Set compression
            uint16_t compression = 1; // None
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_COMPRESSION, 1, &compression, TIFF_IFD_0),
                    android::img_utils::TAG_COMPRESSION, writer);
        }

        {
            // Set dimensions
            uint32_t uWidth = static_cast<uint32_t>(width);
            uint32_t uHeight = static_cast<uint32_t>(height);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_IMAGEWIDTH, 1, &uWidth, TIFF_IFD_0),
                    android::img_utils::TAG_IMAGEWIDTH, writer);
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_IMAGELENGTH, 1, &uHeight, TIFF_IFD_0),
                    android::img_utils::TAG_IMAGELENGTH, writer);
        }

        {
            // x resolution
            uint32_t xres[] = { 72, 1 }; // default 72 ppi
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_XRESOLUTION, 1, xres, TIFF_IFD_0),
                    android::img_utils::TAG_XRESOLUTION, writer);

            // y resolution
            uint32_t yres[] = { 72, 1 }; // default 72 ppi
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_YRESOLUTION, 1, yres, TIFF_IFD_0),
                    android::img_utils::TAG_YRESOLUTION, writer);

            uint16_t unit = 2; // inches
            BAIL_IF_INVALID(writer->addEntry(android::img_utils::TAG_RESOLUTIONUNIT, 1, &unit, TIFF_IFD_0),
                    android::img_utils::TAG_RESOLUTIONUNIT, writer);
        }

        {
            // Setup data strips
            if (writer->addStrip(TIFF_IFD_0) != android::OK) {
                ALOGE("%s: Could not setup thumbnail strip tags.", __FUNCTION__);
                // jniThrowException(env, "java/lang/IllegalStateException",
                //         "Failed to setup thumbnail strip tags.");
                Slogger::E(TAG, "Failed to setup thumbnail strip tags.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (writer->addStrip(TIFF_IFD_SUB1) != android::OK) {
                ALOGE("%s: Could not main image strip tags.", __FUNCTION__);
                // jniThrowException(env, "java/lang/IllegalStateException",
                //         "Failed to setup main image strip tags.");
                Slogger::E(TAG, "Failed to setup main image strip tags.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        if (!context->setThumbnail(pixelBytes, width, height)) {
            // jniThrowException(env, "java/lang/IllegalStateException",
            //         "Failed to set thumbnail.");
            Slogger::E(TAG, "Failed to set thumbnail.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
}

ECode DngCreator::NativeWriteImage(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IByteBuffer* inBuffer,
    /* [in] */ Int32 rowStride,
    /* [in] */ Int32 pixStride,
    /* [in] */ Int64 offset,
    /* [in] */ Boolean isDirect)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);
        ALOGV("%s: nativeWriteImage called with: width=%d, height=%d, rowStride=%d, pixStride=%d,"
                  " offset=%lld", __FUNCTION__, width, height, rowStride, pixStride, offset);
        uint32_t rStride = static_cast<uint32_t>(rowStride);
        uint32_t pStride = static_cast<uint32_t>(pixStride);
        uint32_t uWidth = static_cast<uint32_t>(width);
        uint32_t uHeight = static_cast<uint32_t>(height);
        uint64_t uOffset = static_cast<uint64_t>(offset);

        android::sp<JniOutputStream> out = new JniOutputStream(outStream);
        // if(env->ExceptionCheck()) {
        //     ALOGE("%s: Could not allocate buffers for output stream", __FUNCTION__);
        //     return;
        // }

        TiffWriter* writer = DngCreator_getCreator();
        NativeContext* context = DngCreator_getNativeContext();
        if (writer == NULL || context == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "Write called with uninitialized DngCreator");
            Slogger::E(TAG, "Write called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        // Validate DNG header
        bool result;
        FAIL_RETURN(validateDngHeader(writer, width, height, &result))
        if (!result) {
            return NOERROR;
        }

        android::sp<JniInputByteBuffer> inBuf;
        android::Vector<StripSource*> sources;
        android::sp<DirectStripSource> thumbnailSource;
        uint32_t targetIfd = TIFF_IFD_0;

        bool hasThumbnail = writer->hasIfd(TIFF_IFD_SUB1);

        if (hasThumbnail) {
            ALOGV("%s: Adding thumbnail strip sources.", __FUNCTION__);
            uint32_t bytesPerPixel = SAMPLES_PER_RGB_PIXEL * BYTES_PER_RGB_SAMPLE;
            uint32_t thumbWidth = context->getThumbnailWidth();
            thumbnailSource = new DirectStripSource(context->getThumbnail(), TIFF_IFD_0,
                    thumbWidth, context->getThumbnailHeight(), bytesPerPixel,
                    bytesPerPixel * thumbWidth, /*offset*/0, BYTES_PER_RGB_SAMPLE,
                    SAMPLES_PER_RGB_PIXEL);
            sources.add(thumbnailSource.get());
            targetIfd = TIFF_IFD_SUB1;
        }

        if (isDirect) {
            size_t fullSize = rStride * uHeight;
            Int32 _capacity;
            IBuffer::Probe(inBuffer)->GetCapacity(&_capacity);
            Int64 capacity = _capacity;
            if (capacity < 0 || fullSize + uOffset > static_cast<uint64_t>(capacity)) {
                // jniThrowExceptionFmt(env, "java/lang/IllegalStateException",
                //         "Invalid size %d for Image, size given in metadata is %d at current stride",
                //         capacity, fullSize);
                Slogger::E(TAG, "Invalid size %d for Image, size given in metadata "
                        "is %d at current stride", capacity, fullSize);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            uint8_t* pixelBytes = reinterpret_cast<uint8_t*>(inBuffer);
            if (pixelBytes == NULL) {
                ALOGE("%s: Could not get native ByteBuffer", __FUNCTION__);
                //jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid ByteBuffer");
                Slogger::E(TAG, "Invalid ByteBuffer");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            ALOGV("%s: Using direct-type strip source.", __FUNCTION__);
            DirectStripSource* stripSource = new DirectStripSource(pixelBytes, targetIfd, uWidth, uHeight, pStride,
                    rStride, uOffset, BYTES_PER_SAMPLE, SAMPLES_PER_RAW_PIXEL);
            sources.add(stripSource);

            status_t ret = android::OK;
            if ((ret = writer->write(out.get(), sources.editArray(), sources.size())) != android::OK) {
                ALOGE("%s: write failed with error %d.", __FUNCTION__, ret);
                // if (!env->ExceptionCheck()) {
                //     jniThrowExceptionFmt(env, "java/io/IOException",
                //             "Encountered error %d while writing file.", ret);
                // }
                return NOERROR;
            }
        }
        else {
            inBuf = new JniInputByteBuffer(inBuffer);

            ALOGV("%s: Using input-type strip source.", __FUNCTION__);
            InputStripSource* stripSource = new InputStripSource(*inBuf, targetIfd, uWidth, uHeight, pStride,
                     rStride, uOffset, BYTES_PER_SAMPLE, SAMPLES_PER_RAW_PIXEL);
            sources.add(stripSource);

            status_t ret = android::OK;
            if ((ret = writer->write(out.get(), sources.editArray(), sources.size())) != android::OK) {
                ALOGE("%s: write failed with error %d.", __FUNCTION__, ret);
                // if (!env->ExceptionCheck()) {
                //     jniThrowExceptionFmt(env, "java/io/IOException",
                //             "Encountered error %d while writing file.", ret);
                // }
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode DngCreator::NativeWriteInputStream(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ IInputStream* inStream,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int64 offset)
{
    synchronized(this) {
        ALOGV("%s:", __FUNCTION__);

        uint32_t rowStride = width * BYTES_PER_SAMPLE;
        uint32_t pixStride = BYTES_PER_SAMPLE;
        uint32_t uWidth = static_cast<uint32_t>(width);
        uint32_t uHeight = static_cast<uint32_t>(height);
        uint64_t uOffset = static_cast<uint32_t>(offset);

        ALOGV("%s: nativeWriteInputStream called with: width=%d, height=%d, rowStride=%u,"
                "pixStride=%u, offset=%lld", __FUNCTION__, width, height, rowStride, pixStride,
                offset);

        android::sp<JniOutputStream> out = new JniOutputStream(outStream);
        // if(env->ExceptionCheck()) {
        //     Slogger::E(TAG,"%s: Could not allocate buffers for output stream", __FUNCTION__);
        //     return;
        // }

        TiffWriter* writer = DngCreator_getCreator();
        NativeContext* context = DngCreator_getNativeContext();
        if (writer == NULL || context == NULL) {
            ALOGE("%s: Failed to initialize DngCreator", __FUNCTION__);
            // jniThrowException(env, "java/lang/AssertionError",
            //         "Write called with uninitialized DngCreator");
            Slogger::E(TAG, "Write called with uninitialized DngCreator");
            return E_ASSERTION_ERROR;
        }

        // Validate DNG header
        bool result;
        FAIL_RETURN(validateDngHeader(writer, width, height, &result))
        if (!result) {
            return NOERROR;
        }

        android::sp<DirectStripSource> thumbnailSource;
        uint32_t targetIfd = TIFF_IFD_0;
        bool hasThumbnail = writer->hasIfd(TIFF_IFD_SUB1);
        android::Vector<StripSource*> sources;

        if (hasThumbnail) {
            ALOGV("%s: Adding thumbnail strip sources.", __FUNCTION__);
            uint32_t bytesPerPixel = SAMPLES_PER_RGB_PIXEL * BYTES_PER_RGB_SAMPLE;
            uint32_t width = context->getThumbnailWidth();
            thumbnailSource = new DirectStripSource(context->getThumbnail(), TIFF_IFD_0,
                    width, context->getThumbnailHeight(), bytesPerPixel,
                    bytesPerPixel * width, /*offset*/0, BYTES_PER_RGB_SAMPLE,
                    SAMPLES_PER_RGB_PIXEL);
            sources.add(thumbnailSource.get());
            targetIfd = TIFF_IFD_SUB1;
        }

        android::sp<JniInputStream> in = new JniInputStream(inStream);

        ALOGV("%s: Using input-type strip source.", __FUNCTION__);
        InputStripSource stripSource(*in, targetIfd, uWidth, uHeight, pixStride,
                 rowStride, uOffset, BYTES_PER_SAMPLE, SAMPLES_PER_RAW_PIXEL);
        sources.add(&stripSource);

        status_t ret = android::OK;
        if ((ret = writer->write(out.get(), sources.editArray(), sources.size())) != android::OK) {
            ALOGE("%s: write failed with error %d.", __FUNCTION__, ret);
            // if (!env->ExceptionCheck()) {
            //     // jniThrowExceptionFmt(env, "java/io/IOException",
            //     //         "Encountered error %d while writing file.", ret);
            //     Slogger::E(TAG, "Encountered error %d while writing file.", ret);
            //     return E_IO_EXCEPTION;
            // }
            return NOERROR;
        }
    }
    return NOERROR;
}


} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos