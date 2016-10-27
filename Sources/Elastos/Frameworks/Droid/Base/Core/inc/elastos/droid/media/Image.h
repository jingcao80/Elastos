
#ifndef __ELASTOS_DROID_MEDIA_IMAGE_H__
#define __ELASTOS_DROID_MEDIA_IMAGE_H__

#include "Elastos.Droid.Media.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IAutoCloseable;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * <p>A single complete image buffer to use with a media source such as a
 * {@link MediaCodec} or a
 * {@link android.hardware.camera2.CameraDevice CameraDevice}.</p>
 *
 * <p>This class allows for efficient direct application access to the pixel
 * data of the Image through one or more
 * {@link java.nio.ByteBuffer ByteBuffers}. Each buffer is encapsulated in a
 * {@link Plane} that describes the layout of the pixel data in that plane. Due
 * to this direct access, and unlike the {@link android.graphics.Bitmap Bitmap} class,
 * Images are not directly usable as as UI resources.</p>
 *
 * <p>Since Images are often directly produced or consumed by hardware
 * components, they are a limited resource shared across the system, and should
 * be closed as soon as they are no longer needed.</p>
 *
 * <p>For example, when using the {@link ImageReader} class to read out Images
 * from various media sources, not closing old Image objects will prevent the
 * availability of new Images once
 * {@link ImageReader#getMaxImages the maximum outstanding image count} is
 * reached. When this happens, the function acquiring new Images will typically
 * throw an {@link IllegalStateException}.</p>
 *
 * @see ImageReader
 */
class Image
    : public Object
    , public IImage
    , public IAutoCloseable
{
public:
    /**
     * <p>A single color plane of image data.</p>
     *
     * <p>The number and meaning of the planes in an Image are determined by the
     * format of the Image.</p>
     *
     * <p>Once the Image has been closed, any access to the the plane's
     * ByteBuffer will fail.</p>
     *
     * @see #getFormat
     */
     class Plane
        : public Object
        , public IImagePlane
    {
    public:
        Plane();

        virtual ~Plane();

        CAR_INTERFACE_DECL()

        /**
         * <p>The row stride for this color plane, in bytes.</p>
         *
         * <p>This is the distance between the start of two consecutive rows of
         * pixels in the image. The row stride is always greater than 0.</p>
         */
        CARAPI GetRowStride(
            /* [out] */ Int32* result);

        /**
         * <p>The distance between adjacent pixel samples, in bytes.</p>
         *
         * <p>This is the distance between two consecutive pixel values in a row
         * of pixels. It may be larger than the size of a single pixel to
         * account for interleaved image data or padded formats.
         * The pixel stride is always greater than 0.</p>
         */
        CARAPI GetPixelStride(
            /* [out] */ Int32* result);

        /**
         * <p>Get a direct {@link java.nio.ByteBuffer ByteBuffer}
         * containing the frame data.</p>
         *
         * <p>In particular, the buffer returned will always have
         * {@link java.nio.ByteBuffer#isDirect isDirect} return {@code true}, so
         * the underlying data could be mapped as a pointer in JNI without doing
         * any copies with {@code GetDirectBufferAddress}.</p>
         *
         * @return the byte buffer containing the image data for this plane.
         */
        CARAPI GetBuffer(
            /* [out] */ IByteBuffer** result);
    };

public:
    Image();

    virtual ~Image();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Get the format for this image. This format determines the number of
     * ByteBuffers needed to represent the image, and the general layout of the
     * pixel data in each in ByteBuffer.
     *
     * <p>
     * The format is one of the values from
     * {@link android.graphics.ImageFormat ImageFormat}. The mapping between the
     * formats and the planes is as follows:
     * </p>
     *
     * <table>
     * <tr>
     *   <th>Format</th>
     *   <th>Plane count</th>
     *   <th>Layout details</th>
     * </tr>
     * <tr>
     *   <td>{@link android.graphics.ImageFormat#JPEG JPEG}</td>
     *   <td>1</td>
     *   <td>Compressed data, so row and pixel strides are 0. To uncompress, use
     *      {@link android.graphics.BitmapFactory#decodeByteArray BitmapFactory#decodeByteArray}.
     *   </td>
     * </tr>
     * <tr>
     *   <td>{@link android.graphics.ImageFormat#YUV_420_888 YUV_420_888}</td>
     *   <td>3</td>
     *   <td>A luminance plane followed by the Cb and Cr chroma planes.
     *     The chroma planes have half the width and height of the luminance
     *     plane (4:2:0 subsampling). Each pixel sample in each plane has 8 bits.
     *     Each plane has its own row stride and pixel stride.</td>
     * </tr>
     * <tr>
     *   <td>{@link android.graphics.ImageFormat#RAW_SENSOR RAW_SENSOR}</td>
     *   <td>1</td>
     *   <td>A single plane of raw sensor image data, with 16 bits per color
     *     sample. The details of the layout need to be queried from the source of
     *     the raw sensor data, such as
     *     {@link android.hardware.camera2.CameraDevice CameraDevice}.
     *   </td>
     * </tr>
     * </table>
     *
     * @see android.graphics.ImageFormat
     */
    CARAPI GetFormat(
        /* [out] */ Int32* result);

    /**
     * The width of the image in pixels. For formats where some color channels
     * are subsampled, this is the width of the largest-resolution plane.
     */
    CARAPI GetWidth(
        /* [out] */ Int32* result);

    /**
     * The height of the image in pixels. For formats where some color channels
     * are subsampled, this is the height of the largest-resolution plane.
     */
    CARAPI GetHeight(
        /* [out] */ Int32* result);

    /**
     * Get the timestamp associated with this frame.
     * <p>
     * The timestamp is measured in nanoseconds, and is monotonically
     * increasing. However, the zero point and whether the timestamp can be
     * compared against other sources of time or images depend on the source of
     * this image.
     * </p>
     */
    CARAPI GetTimestamp(
        /* [out] */ Int64* result);

    /**
     * Get the crop rectangle associated with this frame.
     * <p>
     * The crop rectangle specifies the region of valid pixels in the image,
     * using coordinates in the largest-resolution plane.
     */
    CARAPI GetCropRect(
        /* [out] */ IRect** result);

    /**
     * Set the crop rectangle associated with this frame.
     * <p>
     * The crop rectangle specifies the region of valid pixels in the image,
     * using coordinates in the largest-resolution plane.
     */
    CARAPI SetCropRect(
        /* [in] */ IRect* cropRect);

    /**
     * Get the array of pixel planes for this Image. The number of planes is
     * determined by the format of the Image.
     */
    CARAPI GetPlanes(
        /* [out, callee] */ ArrayOf<IImagePlane*>** result);

    /**
     * Free up this frame for reuse.
     * <p>
     * After calling this method, calling any methods on this {@code Image} will
     * result in an {@link IllegalStateException}, and attempting to read from
     * {@link ByteBuffer ByteBuffers} returned by an earlier
     * {@link Plane#getBuffer} call will have undefined behavior.
     * </p>
     */
    CARAPI Close();

private:
    AutoPtr<IRect> mCropRect;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_IMAGE_H__
