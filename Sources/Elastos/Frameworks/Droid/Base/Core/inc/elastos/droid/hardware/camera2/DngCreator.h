
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DNGCREATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DNGCREATOR_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

#include <system/camera_metadata.h>
#include <camera/CameraMetadata.h>

#include <img_utils/TagDefinitions.h>
#include <img_utils/TiffIfd.h>
#include <img_utils/TiffWriter.h>
#include <img_utils/Output.h>
#include <img_utils/Input.h>
#include <img_utils/StripSource.h>

#include <utils/Log.h>
#include <utils/Errors.h>
#include <utils/StrongPointer.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <cutils/properties.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IImage;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteBuffer;
using Elastos::Text::IDateFormat;
using Elastos::Utility::ICalendar;
using android::status_t;
using android::LightRefBase;
using android::img_utils::Input;
using android::img_utils::Output;
using android::img_utils::TiffWriter;
using android::img_utils::StripSource;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class DngCreator
    : public Object
    , public IDngCreator
    , public ICloseable
{
private:
    /**
     * Container class for the persistent native context.
     */
    class NativeContext
        : public LightRefBase<NativeContext>
    {
    public:
        NativeContext();
        virtual ~NativeContext();

        TiffWriter* getWriter();

        uint32_t getThumbnailWidth();
        uint32_t getThumbnailHeight();
        const uint8_t* getThumbnail();

        bool setThumbnail(
            /* [in] */ const uint8_t* buffer,
            /* [in] */ uint32_t width,
            /* [in] */ uint32_t height);

    private:
        android::Vector<uint8_t> mCurrentThumbnail;
        TiffWriter mWriter;
        uint32_t mThumbnailWidth;
        uint32_t mThumbnailHeight;
    };

    /**
     * Wrapper class for a Java OutputStream.
     *
     * This class is not intended to be used across JNI calls.
     */
    class JniOutputStream
        : public Output
        , public LightRefBase<JniOutputStream>
    {
    public:
        JniOutputStream(
            /* [in] */ IOutputStream* outStream);

        virtual ~JniOutputStream();

        status_t open();

        status_t write(
            /* [in] */ const uint8_t* buf,
            /* [in] */ size_t offset,
            /* [in] */ size_t count);

        status_t close();
    private:
        enum {
            BYTE_ARRAY_LENGTH = 4096
        };
        AutoPtr<IOutputStream> mOutputStream;
        AutoPtr<ArrayOf<Byte> > mByteArray;
    };

    /**
     * Wrapper class for a Java InputStream.
     *
     * This class is not intended to be used across JNI calls.
     */
    class JniInputStream
        : public android::img_utils::Input
        , public LightRefBase<JniInputStream>
    {
    public:
        JniInputStream(
            /* [in] */ IInputStream* inStream);

        status_t open();

        status_t close();

        ssize_t read(
            /* [in] */ uint8_t* buf,
            /* [in] */ size_t offset,
            /* [in] */ size_t count);

        ssize_t skip(
            /* [in] */ size_t count);

        virtual ~JniInputStream();
    private:
        enum {
            BYTE_ARRAY_LENGTH = 4096
        };
        AutoPtr<IInputStream> mInStream;
        AutoPtr<ArrayOf<Byte> > mByteArray;
    };

    /**
     * Wrapper class for a non-direct Java ByteBuffer.
     *
     * This class is not intended to be used across JNI calls.
     */
    class JniInputByteBuffer
        : public android::img_utils::Input
        , public LightRefBase<JniInputByteBuffer>
    {
    public:
        JniInputByteBuffer(
            /* [in] */ IByteBuffer* inBuf);

        status_t open();

        status_t close();

        ssize_t read(
            /* [in] */ uint8_t* buf,
            /* [in] */ size_t offset,
            /* [in] */ size_t count);

        virtual ~JniInputByteBuffer();
    private:
        enum {
            BYTE_ARRAY_LENGTH = 4096
        };
        AutoPtr<IByteBuffer> mInBuf;
        AutoPtr<ArrayOf<Byte> > mByteArray;
    };

    /**
     * StripSource subclass for Input types.
     *
     * This class is not intended to be used across JNI calls.
     */
    class InputStripSource
        : public StripSource
        , public LightRefBase<InputStripSource>
    {
    public:
        InputStripSource(
            /* [in] */ android::img_utils::Input& input,
            /* [in] */ uint32_t ifd,
            /* [in] */ uint32_t width,
            /* [in] */ uint32_t height,
            /* [in] */ uint32_t pixStride,
            /* [in] */ uint32_t rowStride,
            /* [in] */ uint64_t offset,
            /* [in] */ uint32_t bytesPerSample,
            /* [in] */ uint32_t samplesPerPixel);

        virtual ~InputStripSource();

        virtual status_t writeToStream(
            /* [in] */ Output& stream,
            /* [in] */ uint32_t count);

        virtual uint32_t getIfd() const;
    protected:
        uint32_t mIfd;
        android::img_utils::Input* mInput;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mPixStride;
        uint32_t mRowStride;
        uint64_t mOffset;
        uint32_t mBytesPerSample;
        uint32_t mSamplesPerPixel;
    };

    /**
     * StripSource subclass for direct buffer types.
     *
     * This class is not intended to be used across JNI calls.
     */
    class DirectStripSource
        : public StripSource
        , public LightRefBase<DirectStripSource>
    {
    public:
        DirectStripSource(
            /* [in] */ const uint8_t* pixelBytes,
            /* [in] */ uint32_t ifd,
            /* [in] */ uint32_t width,
            /* [in] */ uint32_t height,
            /* [in] */ uint32_t pixStride,
            /* [in] */ uint32_t rowStride,
            /* [in] */ uint64_t offset,
            /* [in] */ uint32_t bytesPerSample,
            /* [in] */ uint32_t samplesPerPixel);

        virtual ~DirectStripSource();

        virtual status_t writeToStream(
            /* [in] */ Output& stream,
            /* [in] */ uint32_t count);

        virtual uint32_t getIfd() const;
    protected:
        uint32_t mIfd;
        const uint8_t* mPixelBytes;
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mPixStride;
        uint32_t mRowStride;
        uint16_t mOffset;
        uint32_t mBytesPerSample;
        uint32_t mSamplesPerPixel;
    };

public:
    CAR_INTERFACE_DECL()

    DngCreator();

    virtual ~DngCreator();

    CARAPI constructor();

    /**
     * Create a new DNG object.
     *
     * <p>
     * It is not necessary to call any set methods to write a well-formatted DNG file.
     * </p>
     * <p>
     * DNG metadata tags will be generated from the corresponding parameters in the
     * {@link android.hardware.camera2.CaptureResult} object.
     * </p>
     * <p>
     * For best quality DNG files, it is strongly recommended that lens shading map output is
     * enabled if supported. See {@link CaptureRequest#STATISTICS_LENS_SHADING_MAP_MODE}.
     * </p>
     * @param characteristics an object containing the static
     *          {@link android.hardware.camera2.CameraCharacteristics}.
     * @param metadata a metadata object to generate tags from.
     */
    CARAPI constructor(
        /* [in] */ ICameraCharacteristics* characteristics,
        /* [in] */ ICaptureResult* metadata);

    /**
     * Set the orientation value to write.
     *
     * <p>
     * This will be written as the TIFF "Orientation" tag {@code (0x0112)}.
     * Calling this will override any prior settings for this tag.
     * </p>
     *
     * @param orientation the orientation value to set, one of:
     *                    <ul>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_NORMAL}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_FLIP_HORIZONTAL}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_ROTATE_180}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_FLIP_VERTICAL}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_TRANSPOSE}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_ROTATE_90}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_TRANSVERSE}</li>
     *                      <li>{@link android.media.ExifInterface#ORIENTATION_ROTATE_270}</li>
     *                    </ul>
     * @return this {@link #DngCreator} object.
     */
    CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    /**
     * Set the thumbnail image.
     *
     * <p>
     * Pixel data will be converted to a Baseline TIFF RGB image, with 8 bits per color channel.
     * The alpha channel will be discarded.  Thumbnail images with a dimension larger than
     * {@link #MAX_THUMBNAIL_DIMENSION} will be rejected.
     * </p>
     *
     * @param pixels a {@link android.graphics.Bitmap} of pixel data.
     * @return this {@link #DngCreator} object.
     * @throws java.lang.IllegalArgumentException if the given thumbnail image has a dimension
     *      larger than {@link #MAX_THUMBNAIL_DIMENSION}.
     */
    CARAPI SetThumbnail(
        /* [in] */ IBitmap* pixels);

    /**
     * Set the thumbnail image.
     *
     * <p>
     * Pixel data is interpreted as a {@link android.graphics.ImageFormat#YUV_420_888} image.
     * Thumbnail images with a dimension larger than {@link #MAX_THUMBNAIL_DIMENSION} will be
     * rejected.
     * </p>
     *
     * @param pixels an {@link android.media.Image} object with the format
     *               {@link android.graphics.ImageFormat#YUV_420_888}.
     * @return this {@link #DngCreator} object.
     * @throws java.lang.IllegalArgumentException if the given thumbnail image has a dimension
     *      larger than {@link #MAX_THUMBNAIL_DIMENSION}.
     */
    CARAPI SetThumbnail(
        /* [in] */ IImage* pixels);

    /**
     * Set image location metadata.
     *
     * <p>
     * The given location object must contain at least a valid time, latitude, and longitude
     * (equivalent to the values returned by {@link android.location.Location#getTime()},
     * {@link android.location.Location#getLatitude()}, and
     * {@link android.location.Location#getLongitude()} methods).
     * </p>
     *
     * @param location an {@link android.location.Location} object to set.
     * @return this {@link #DngCreator} object.
     *
     * @throws java.lang.IllegalArgumentException if the given location object doesn't
     *          contain enough information to set location metadata.
     */
    CARAPI SetLocation(
        /* [in] */ ILocation* location);

    /**
     * Set the user description string to write.
     *
     * <p>
     * This is equivalent to setting the TIFF "ImageDescription" tag {@code (0x010E)}.
     * </p>
     *
     * @param description the user description string.
     * @return this {@link #DngCreator} object.
     */
    CARAPI SetDescription(
        /* [in] */ const String& description);

    /**
     * Write the {@link android.graphics.ImageFormat#RAW_SENSOR} pixel data to a DNG file with
     * the currently configured metadata.
     *
     * <p>
     * Raw pixel data must have 16 bits per pixel, and the input must contain at least
     * {@code offset + 2 * width * height)} bytes.  The width and height of
     * the input are taken from the width and height set in the {@link DngCreator} metadata tags,
     * and will typically be equal to the width and height of
     * {@link CameraCharacteristics#SENSOR_INFO_ACTIVE_ARRAY_SIZE}.
     * The pixel layout in the input is determined from the reported color filter arrangement (CFA)
     * set in {@link CameraCharacteristics#SENSOR_INFO_COLOR_FILTER_ARRANGEMENT}.  If insufficient
     * metadata is available to write a well-formatted DNG file, an
     * {@link java.lang.IllegalStateException} will be thrown.
     * </p>
     *
     * @param dngOutput an {@link java.io.OutputStream} to write the DNG file to.
     * @param size the {@link Size} of the image to write, in pixels.
     * @param pixels an {@link java.io.InputStream} of pixel data to write.
     * @param offset the offset of the raw image in bytes.  This indicates how many bytes will
     *               be skipped in the input before any pixel data is read.
     *
     * @throws IOException if an error was encountered in the input or output stream.
     * @throws java.lang.IllegalStateException if not enough metadata information has been
     *          set to write a well-formatted DNG file.
     * @throws java.lang.IllegalArgumentException if the size passed in does not match the
     */
    CARAPI WriteInputStream(
        /* [in] */ IOutputStream* dngOutput,
        /* [in] */ ISize* size,
        /* [in] */ IInputStream* pixels,
        /* [in] */ Int64 offset);

    /**
     * Write the {@link android.graphics.ImageFormat#RAW_SENSOR} pixel data to a DNG file with
     * the currently configured metadata.
     *
     * <p>
     * Raw pixel data must have 16 bits per pixel, and the input must contain at least
     * {@code offset + 2 * width * height)} bytes.  The width and height of
     * the input are taken from the width and height set in the {@link DngCreator} metadata tags,
     * and will typically be equal to the width and height of
     * {@link CameraCharacteristics#SENSOR_INFO_ACTIVE_ARRAY_SIZE}.
     * The pixel layout in the input is determined from the reported color filter arrangement (CFA)
     * set in {@link CameraCharacteristics#SENSOR_INFO_COLOR_FILTER_ARRANGEMENT}.  If insufficient
     * metadata is available to write a well-formatted DNG file, an
     * {@link java.lang.IllegalStateException} will be thrown.
     * </p>
     *
     * <p>
     * Any mark or limit set on this {@link ByteBuffer} is ignored, and will be cleared by this
     * method.
     * </p>
     *
     * @param dngOutput an {@link java.io.OutputStream} to write the DNG file to.
     * @param size the {@link Size} of the image to write, in pixels.
     * @param pixels an {@link java.nio.ByteBuffer} of pixel data to write.
     * @param offset the offset of the raw image in bytes.  This indicates how many bytes will
     *               be skipped in the input before any pixel data is read.
     *
     * @throws IOException if an error was encountered in the input or output stream.
     * @throws java.lang.IllegalStateException if not enough metadata information has been
     *          set to write a well-formatted DNG file.
     */
    CARAPI WriteByteBuffer(
        /* [in] */ IOutputStream* dngOutput,
        /* [in] */ ISize* size,
        /* [in] */ IByteBuffer* pixels,
        /* [in] */ Int64 offset);

    /**
     * Write the pixel data to a DNG file with the currently configured metadata.
     *
     * <p>
     * For this method to succeed, the {@link android.media.Image} input must contain
     * {@link android.graphics.ImageFormat#RAW_SENSOR} pixel data, otherwise an
     * {@link java.lang.IllegalArgumentException} will be thrown.
     * </p>
     *
     * @param dngOutput an {@link java.io.OutputStream} to write the DNG file to.
     * @param pixels an {@link android.media.Image} to write.
     *
     * @throws java.io.IOException if an error was encountered in the output stream.
     * @throws java.lang.IllegalArgumentException if an image with an unsupported format was used.
     * @throws java.lang.IllegalStateException if not enough metadata information has been
     *          set to write a well-formatted DNG file.
     */
    CARAPI WriteImage(
        /* [in] */ IOutputStream* dngOutput,
        /* [in] */ IImage* pixels);

    //@Override
    CARAPI Close();

private:
    static CARAPI_(AutoPtr<IDateFormat>) InitExifGPSDateStamp();

    static CARAPI_(AutoPtr<IDateFormat>) InitDateTimeStampFormat();

    /**
     * Offset, rowStride, and pixelStride are given in bytes.  Height and width are given in pixels.
     */
    CARAPI WriteByteBuffer(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IByteBuffer* pixels,
        /* [in] */ IOutputStream* dngOutput,
        /* [in] */ Int32 pixelStride,
        /* [in] */ Int32 rowStride,
        /* [in] */ Int64 offset);

    /**
     * Convert a single YUV pixel to RGB.
     */
    static CARAPI_(void) YuvToRgb(
        /* [in] */ ArrayOf<Byte>* yuvData,
        /* [in] */ Int32 outOffset,
        /* [in] */ ArrayOf<Byte>* rgbOut);

    /**
     * Convert a single {@link Color} pixel to RGB.
     */
    static CARAPI_(void) ColorToRgb(
        /* [in] */ Int32 color,
        /* [in] */ Int32 outOffset,
        /* [in] */ ArrayOf<Byte>* rgbOut);

    /**
     * Generate a direct RGB {@link ByteBuffer} from a YUV420_888 {@link Image}.
     */
    static CARAPI_(AutoPtr<IByteBuffer>) ConvertToRGB(
        /* [in] */ IImage* yuvImage);

    /**
     * Generate a direct RGB {@link ByteBuffer} from a {@link Bitmap}.
     */
    static CARAPI_(AutoPtr<IByteBuffer>) ConvertToRGB(
        /* [in] */ IBitmap* argbBitmap);

    /**
     * Convert coordinate to EXIF GPS tag format.
     */
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) ToExifLatLong(
        /* [in] */ Double value);

    static CARAPI_(NativeContext*) DngCreator_getNativeContext();

    static CARAPI_(void) DngCreator_setNativeContext(
        /* [in] */ android::sp<NativeContext> context);

    static CARAPI_(TiffWriter*) DngCreator_getCreator();

    static CARAPI_(Int32) NativeClassInit();

    CARAPI NativeInit(
        /* [in] */ ICameraMetadataNative* nativeCharacteristics,
        /* [in] */ ICameraMetadataNative* nativeResult,
        /* [in] */ const String& captureTime);

    CARAPI_(void) NativeDestroy();

    CARAPI NativeSetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI NativeSetDescription(
        /* [in] */ const String& description);

    CARAPI NativeSetGpsTags(
        /* [in] */ ArrayOf<Int32>* latTag,
        /* [in] */ const String& latRef,
        /* [in] */ ArrayOf<Int32>* longTag,
        /* [in] */ const String& longRef,
        /* [in] */ const String& dateTag,
        /* [in] */ ArrayOf<Int32>* timeTag);

    CARAPI NativeSetThumbnail(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI NativeWriteImage(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IByteBuffer* rawBuffer,
        /* [in] */ Int32 rowStride,
        /* [in] */ Int32 pixStride,
        /* [in] */ Int64 offset,
        /* [in] */ Boolean isDirect);

    CARAPI NativeWriteInputStream(
        /* [in] */ IOutputStream* out,
        /* [in] */ IInputStream* rawStream,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int64 offset);

private:
    static const String TAG;

    static const String GPS_LAT_REF_NORTH;
    static const String GPS_LAT_REF_SOUTH;
    static const String GPS_LONG_REF_EAST;
    static const String GPS_LONG_REF_WEST;

    static const String GPS_DATE_FORMAT_STR;
    static const String TIFF_DATETIME_FORMAT;

    static AutoPtr<IDateFormat> sExifGPSDateStamp;
    static AutoPtr<IDateFormat> sDateTimeStampFormat;
    AutoPtr<ICalendar> mGPSTimeStampCalendar;

    static const Int32 DEFAULT_PIXEL_STRIDE;
    static const Int32 BYTES_PER_RGB_PIX;

    /**
     * This field is used by native code, do not access or modify.
     */
    static Int64 mNativeContext;

    static Int32 mNativeInit;
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DNGCREATOR_H__
