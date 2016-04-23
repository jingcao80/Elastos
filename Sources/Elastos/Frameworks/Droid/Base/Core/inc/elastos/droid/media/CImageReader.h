#ifndef __ELASTOS_DROID_MEDIA_CIMAGEREADER_H__
#define __ELASTOS_DROID_MEDIA_CIMAGEREADER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Media_CImageReader.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/Image.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::ISurface;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * <p>The ImageReader class allows direct application access to image data
 * rendered into a {@link android.view.Surface}</p>
 *
 * <p>Several Android media API classes accept Surface objects as targets to
 * render to, including {@link MediaPlayer}, {@link MediaCodec},
 * {@link android.hardware.camera2.CameraDevice}, and
 * {@link android.renderscript.Allocation RenderScript Allocations}. The image
 * sizes and formats that can be used with each source vary, and should be
 * checked in the documentation for the specific API.</p>
 *
 * <p>The image data is encapsulated in {@link Image} objects, and multiple such
 * objects can be accessed at the same time, up to the number specified by the
 * {@code maxImages} constructor parameter. New images sent to an ImageReader
 * through its {@link Surface} are queued until accessed through the {@link #acquireLatestImage}
 * or {@link #acquireNextImage} call. Due to memory limits, an image source will
 * eventually stall or drop Images in trying to render to the Surface if the
 * ImageReader does not obtain and release Images at a rate equal to the
 * production rate.</p>
 */
// TODO: Need IAutoCloseable
    // , public IAutoCloseable
CarClass(CImageReader)
    , public Object
    , public IImageReader
{
private:
    class ListenerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CImageReader::ListenerHandler")

        ListenerHandler(
            /* [in] */ CImageReader* host,
            /* [in] */ ILooper* looper)
                : Handler(looper, NULL, TRUE /*async*/)
                , mHost(host)
            {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CImageReader* mHost;
    };

    class SurfaceImage
        : public Image
        , public IImageReaderSurfaceImage
    {
    private:
        class SurfacePlane
            : public Image::Plane
        {
        public:
            SurfacePlane(
                /* [in] */ Int32 index,
                /* [in] */ Int32 rowStride,
                /* [in] */ Int32 pixelStride,
                /* [in] */ SurfaceImage* host);

            CARAPI GetBuffer(
                /* [out] */ IByteBuffer** result);

            CARAPI GetPixelStride(
                /* [out] */ Int32* result);

            CARAPI GetRowStride(
                /* [out] */ Int32* result);

            CARAPI_(void) ClearBuffer();

        private:
            Int32 mIndex;
            Int32 mPixelStride;
            Int32 mRowStride;

            AutoPtr<IByteBuffer> mBuffer;
            SurfaceImage* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        SurfaceImage(
            /* [in] */ CImageReader* host);

        CARAPI Close();

        CARAPI GetReader(
            /* [out] */ IImageReader** result);

        CARAPI GetFormat(
            /* [out] */ Int32* result);

        CARAPI GetWidth(
            /* [out] */ Int32* result);

        CARAPI GetHeight(
            /* [out] */ Int32* result);

        CARAPI GetTimestamp(
            /* [out] */ Int64* result);

        CARAPI GetPlanes(
            /* [out, callee] */ ArrayOf<IImagePlane*>** result);

        CARAPI_(void) SetImageValid(
            /* [in] */ Boolean isValid);

        CARAPI_(Boolean) IsImageValid();

        CARAPI_(void) ClearSurfacePlanes();

        CARAPI_(void) CreateSurfacePlanes();

        CARAPI NativeImageGetBuffer(
            /* [in] */ Int32 idx,
            /* [in] */ Int32 readerFormat,
            /* [out] */ IByteBuffer** result);

        CARAPI NativeCreatePlane(
            /* [in] */ Int32 idx,
            /* [in] */ Int32 readerFormat,
            /* [out] */ SurfacePlane** result);

    public:
        /**
         * This field is used to keep track of native object and used by native code only.
         * Don't modify.
         */
        Int64 mLockedBuffer;

        /**
         * This field is set by native code during nativeImageSetup().
         */
        Int64 mTimestamp;

        AutoPtr<ArrayOf<SurfacePlane*> > mPlanes;
        Boolean mIsImageValid;
        CImageReader* mHost;
    };

public:
    CImageReader();

    virtual ~CImageReader();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 maxImages);

    /**
     * The default width of {@link Image Images}, in pixels.
     *
     * <p>The width may be overridden by the producer sending buffers to this
     * ImageReader's Surface. If so, the actual width of the images can be
     * found using {@link Image#getWidth}.</p>
     *
     * @return the expected width of an Image
     */
    CARAPI GetWidth(
        /* [out] */ Int32* result);

    /**
     * The default height of {@link Image Images}, in pixels.
     *
     * <p>The height may be overridden by the producer sending buffers to this
     * ImageReader's Surface. If so, the actual height of the images can be
     * found using {@link Image#getHeight}.</p>
     *
     * @return the expected height of an Image
     */
    CARAPI GetHeight(
        /* [out] */ Int32* result);

    /**
     * The default {@link ImageFormat image format} of {@link Image Images}.
     *
     * <p>Some color formats may be overridden by the producer sending buffers to
     * this ImageReader's Surface if the default color format allows. ImageReader
     * guarantees that all {@link Image Images} acquired from ImageReader
     * (for example, with {@link #acquireNextImage}) will have a "compatible"
     * format to what was specified in {@link #newInstance}.
     * As of now, each format is only compatible to itself.
     * The actual format of the images can be found using {@link Image#getFormat}.</p>
     *
     * @return the expected format of an Image
     *
     * @see ImageFormat
     */
    CARAPI GetImageFormat(
        /* [out] */ Int32* result);

    /**
     * Maximum number of images that can be acquired from the ImageReader by any time (for example,
     * with {@link #acquireNextImage}).
     *
     * <p>An image is considered acquired after it's returned by a function from ImageReader, and
     * until the Image is {@link Image#close closed} to release the image back to the ImageReader.
     * </p>
     *
     * <p>Attempting to acquire more than {@code maxImages} concurrently will result in the
     * acquire function throwing a {@link IllegalStateException}. Furthermore,
     * while the max number of images have been acquired by the ImageReader user, the producer
     * enqueueing additional images may stall until at least one image has been released. </p>
     *
     * @return Maximum number of images for this ImageReader.
     *
     * @see Image#close
     */
    CARAPI GetMaxImages(
        /* [out] */ Int32* result);

    /**
     * <p>Get a {@link Surface} that can be used to produce {@link Image Images} for this
     * {@code ImageReader}.</p>
     *
     * <p>Until valid image data is rendered into this {@link Surface}, the
     * {@link #acquireNextImage} method will return {@code null}. Only one source
     * can be producing data into this Surface at the same time, although the
     * same {@link Surface} can be reused with a different API once the first source is
     * disconnected from the {@link Surface}.</p>
     *
     * @return A {@link Surface} to use for a drawing target for various APIs.
     */
    CARAPI GetSurface(
        /* [out] */ ISurface** result);

    /**
     * <p>
     * Acquire the latest {@link Image} from the ImageReader's queue, dropping older
     * {@link Image images}. Returns {@code null} if no new image is available.
     * </p>
     * <p>
     * This operation will acquire all the images possible from the ImageReader,
     * but {@link #close} all images that aren't the latest. This function is
     * recommended to use over {@link #acquireNextImage} for most use-cases, as it's
     * more suited for real-time processing.
     * </p>
     * <p>
     * Note that {@link #getMaxImages maxImages} should be at least 2 for
     * {@link #acquireLatestImage} to be any different than {@link #acquireNextImage} -
     * discarding all-but-the-newest {@link Image} requires temporarily acquiring two
     * {@link Image Images} at once. Or more generally, calling {@link #acquireLatestImage}
     * with less than two images of margin, that is
     * {@code (maxImages - currentAcquiredImages < 2)} will not discard as expected.
     * </p>
     * <p>
     * This operation will fail by throwing an {@link IllegalStateException} if
     * {@code maxImages} have been acquired with {@link #acquireLatestImage} or
     * {@link #acquireNextImage}. In particular a sequence of {@link #acquireLatestImage}
     * calls greater than {@link #getMaxImages} without calling {@link Image#close} in-between
     * will exhaust the underlying queue. At such a time, {@link IllegalStateException}
     * will be thrown until more images are
     * released with {@link Image#close}.
     * </p>
     *
     * @return latest frame of image data, or {@code null} if no image data is available.
     * @throws IllegalStateException if too many images are currently acquired
     */
    CARAPI AcquireLatestImage(
        /* [out] */ IImage** result);

    /**
     * Don't throw IllegalStateException if there are too many images acquired.
     *
     * @return Image if acquiring succeeded, or null otherwise.
     *
     * @hide
     */
    CARAPI AcquireNextImageNoThrowISE(
        /* [out] */ IImage** result);

    /**
     * <p>
     * Acquire the next Image from the ImageReader's queue. Returns {@code null} if
     * no new image is available.
     * </p>
     *
     * <p><i>Warning:</i> Consider using {@link #acquireLatestImage()} instead, as it will
     * automatically release older images, and allow slower-running processing routines to catch
     * up to the newest frame. Usage of {@link #acquireNextImage} is recommended for
     * batch/background processing. Incorrectly using this function can cause images to appear
     * with an ever-increasing delay, followed by a complete stall where no new images seem to
     * appear.
     * </p>
     *
     * <p>
     * This operation will fail by throwing an {@link IllegalStateException} if
     * {@code maxImages} have been acquired with {@link #acquireNextImage} or
     * {@link #acquireLatestImage}. In particular a sequence of {@link #acquireNextImage} or
     * {@link #acquireLatestImage} calls greater than {@link #getMaxImages maxImages} without
     * calling {@link Image#close} in-between will exhaust the underlying queue. At such a time,
     * {@link IllegalStateException} will be thrown until more images are released with
     * {@link Image#close}.
     * </p>
     *
     * @return a new frame of image data, or {@code null} if no image data is available.
     * @throws IllegalStateException if {@code maxImages} images are currently acquired
     * @see #acquireLatestImage
     */
    CARAPI AcquireNextImage(
        /* [out] */ IImage** result);

    /**
     * Register a listener to be invoked when a new image becomes available
     * from the ImageReader.
     *
     * @param listener
     *            The listener that will be run.
     * @param handler
     *            The handler on which the listener should be invoked, or null
     *            if the listener should be invoked on the calling thread's looper.
     * @throws IllegalArgumentException
     *            If no handler specified and the calling thread has no looper.
     */
    CARAPI SetOnImageAvailableListener(
        /* [in] */ IImageReaderOnImageAvailableListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * <p>Create a new reader for images of the desired size and format.</p>
     *
     * <p>The {@code maxImages} parameter determines the maximum number of {@link Image}
     * objects that can be be acquired from the {@code ImageReader}
     * simultaneously. Requesting more buffers will use up more memory, so it is
     * important to use only the minimum number necessary for the use case.</p>
     *
     * <p>The valid sizes and formats depend on the source of the image
     * data.</p>
     *
     * @param width
     *            The default width in pixels of the Images that this reader will produce.
     * @param height
     *            The default height in pixels of the Images that this reader will produce.
     * @param format
     *            The format of the Image that this reader will produce. This
     *            must be one of the {@link android.graphics.ImageFormat} or
     *            {@link android.graphics.PixelFormat} constants. Note that
     *            not all formats is supported, like ImageFormat.NV21.
     * @param maxImages
     *            The maximum number of images the user will want to
     *            access simultaneously. This should be as small as possible to limit
     *            memory use. Once maxImages Images are obtained by the user, one of them
     *            has to be released before a new Image will become available for access
     *            through {@link #acquireLatestImage()} or {@link #acquireNextImage()}.
     *            Must be greater than 0.
     *
     * @see Image
     */
    static CARAPI NewInstance(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 maxImages,
        /* [out] */ IImageReader** result);

    /**
     * Free up all the resources associated with this ImageReader.
     *
     * <p>
     * After calling this method, this ImageReader can not be used. Calling
     * any methods on this ImageReader and Images previously provided by
     * {@link #acquireNextImage} or {@link #acquireLatestImage}
     * will result in an {@link IllegalStateException}, and attempting to read from
     * {@link ByteBuffer ByteBuffers} returned by an earlier
     * {@link Image.Plane#getBuffer Plane#getBuffer} call will
     * have undefined behavior.
     * </p>
     */
    CARAPI Close();

    /**
     * Called from Native code when an Event happens.
     *
     * This may be called from an arbitrary Binder thread, so access to the ImageReader must be
     * synchronized appropriately.
     */
    static CARAPI_(void) PostEventFromNative(
        /* [in] */ IInterface* selfRef);

private:
    /**
     * Attempts to acquire the next image from the underlying native implementation.
     *
     * <p>
     * Note that unexpected failures will throw at the JNI level.
     * </p>
     *
     * @param si A blank SurfaceImage.
     * @return One of the {@code ACQUIRE_*} codes that determine success or failure.
     *
     * @see #ACQUIRE_MAX_IMAGES
     * @see #ACQUIRE_NO_BUFS
     * @see #ACQUIRE_SUCCESS
     */
    CARAPI AcquireNextSurfaceImage(
        /* [in] */ SurfaceImage* si,
        /* [out] */ Int32* result);

    /**
     * <p>Return the frame to the ImageReader for reuse.</p>
     */
    CARAPI ReleaseImage(
        /* [in] */ IImage* i);

    /**
     * Only a subset of the formats defined in
     * {@link android.graphics.ImageFormat ImageFormat} and
     * {@link android.graphics.PixelFormat PixelFormat} are supported by
     * ImageReader. When reading RGB data from a surface, the formats defined in
     * {@link android.graphics.PixelFormat PixelFormat} can be used, when
     * reading YUV, JPEG or raw sensor data (for example, from camera or video
     * decoder), formats from {@link android.graphics.ImageFormat ImageFormat}
     * are used.
     */
    CARAPI GetNumPlanesFromFormat(
        /* [out] */ Int32* result);

    CARAPI NativeInit(
        /* [in] */ IImageReader* weakSelf,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 fmt,
        /* [in] */ Int32 maxImgs);

    CARAPI_(void) NativeClose();

    CARAPI_(void) NativeReleaseImage(
        /* [in] */ IImage* i);

    CARAPI NativeGetSurface(
        /* [out] */ ISurface** result);

    /**
     * @return A return code {@code ACQUIRE_*}
     *
     * @see #ACQUIRE_SUCCESS
     * @see #ACQUIRE_NO_BUFS
     * @see #ACQUIRE_MAX_IMAGES
     */
    CARAPI NativeImageSetup(
        /* [in] */ IImage* i,
        /* [out] */ Int32* result);

    /**
     * We use a class initializer to allow the native code to cache some
     * field offsets.
     */
    static CARAPI_(void) NativeClassInit();

private:
    /**
     * Returned by nativeImageSetup when acquiring the image was successful.
     */
    static const Int32 ACQUIRE_SUCCESS;
    /**
     * Returned by nativeImageSetup when we couldn't acquire the buffer,
     * because there were no buffers available to acquire.
     */
    static const Int32 ACQUIRE_NO_BUFS;
    /**
     * Returned by nativeImageSetup when we couldn't acquire the buffer
     * because the consumer has already acquired {@maxImages} and cannot
     * acquire more than that.
     */
    static const Int32 ACQUIRE_MAX_IMAGES;

    Int32 mWidth;
    Int32 mHeight;
    Int32 mFormat;
    Int32 mMaxImages;
    Int32 mNumPlanes;
    AutoPtr<ISurface> mSurface;

    Object mListenerLock;
    AutoPtr<IImageReaderOnImageAvailableListener> mListener;
    AutoPtr<ListenerHandler> mListenerHandler;

    /**
     * This field is used by native code, do not access or modify.
     */
    Int64 mNativeContext;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CIMAGEREADER_H__
