
#ifndef __ELASTOS_DROID_MEDIA_THUMBNAILUTILS_H__
#define __ELASTOS_DROID_MEDIA_THUMBNAILUTILS_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {
/**
 * Thumbnail generation routines for media provider.
 */

class ThumbnailUtils
{
public:
    /**
     * This method first examines if the thumbnail embedded in EXIF is bigger than our target
     * size. If not, then it'll create a thumbnail from original image. Due to efficiency
     * consideration, we want to let MediaThumbRequest avoid calling this method twice for
     * both kinds, so it only requests for MICRO_KIND and set saveImage to TRUE.
     *
     * This method always returns a "square thumbnail" for MICRO_KIND thumbnail.
     *
     * @param filePath the path of image file
     * @param kind could be MINI_KIND or MICRO_KIND
     * @return Bitmap
     *
     * @hide This method is only used by media framework and media provider internally.
     */
    static CARAPI CreateImageThumbnail(
        /* [in] */ const String& filePath,
        /* [in] */ Int32 kind,
        /* [out] */ IBitmap** result);

    /**
     * Create a video thumbnail for a video. May return NULL if the video is
     * corrupt or the format is not supported.
     *
     * @param filePath the path of video file
     * @param kind could be MINI_KIND or MICRO_KIND
     */
    static CARAPI CreateVideoThumbnail(
        /* [in] */ const String& filePath,
        /* [in] */ Int32 kind,
        /* [out] */ IBitmap** result);

    /**
     * Creates a centered bitmap of the desired size.
     *
     * @param source original bitmap source
     * @param width targeted width
     * @param height targeted height
     */
    static CARAPI ExtractThumbnail(
        /* [in] */ IBitmap* source,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IBitmap** result);

    /**
     * Creates a centered bitmap of the desired size.
     *
     * @param source original bitmap source
     * @param width targeted width
     * @param height targeted height
     * @param options options used during thumbnail extraction
     */
    static CARAPI ExtractThumbnail(
        /* [in] */ IBitmap* source,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 options,
        /* [out] */ IBitmap** result);

private:
    /*
     * Compute the sample size as a function of minSideLength
     * and maxNumOfPixels.
     * minSideLength is used to specify that minimal width or height of a
     * bitmap.
     * maxNumOfPixels is used to specify the maximal size in pixels that is
     * tolerable in terms of memory usage.
     *
     * The function returns a sample size based on the constraints.
     * Both size and minSideLength can be passed in as IImage.UNCONSTRAINED,
     * which indicates no care of the corresponding constraint.
     * The functions prefers returning a sample size that
     * generates a smaller bitmap, unless minSideLength = IImage.UNCONSTRAINED.
     *
     * Also, the function rounds up the sample size to a power of 2 or multiple
     * of 8 because BitmapFactory only honors sample size this way.
     * For example, BitmapFactory downsamples an image by 2 even though the
     * request is 3. So we round up the sample size to avoid OOM.
     */
    static CARAPI_(Int32) ComputeSampleSize(
        /* [in] */ IBitmapFactoryOptions* options,
        /* [in] */ Int32 minSideLength,
        /* [in] */ Int32 maxNumOfPixels);

    static CARAPI_(Int32) ComputeInitialSampleSize(
        /* [in] */ IBitmapFactoryOptions* options,
        /* [in] */ Int32 minSideLength,
        /* [in] */ Int32 maxNumOfPixels);

    /**
     * Make a bitmap from a given Uri, minimal side length, and maximum number of pixels.
     * The image data will be read from specified pfd if it's not NULL, otherwise
     * a new input stream will be created using specified ContentResolver.
     *
     * Clients are allowed to pass their own BitmapFactory.Options used for bitmap decoding. A
     * new BitmapFactory.Options will be created if options is NULL.
     */
    static CARAPI_(AutoPtr<IBitmap>) MakeBitmap(
        /* [in] */ Int32 minSideLength,
        /* [in] */ Int32 maxNumOfPixels,
        /* [in] */ IUri* uri,
        /* [in] */ IContentResolver* cr,
        /* [in] */ IParcelFileDescriptor* pfd,
        /* [in] */ IBitmapFactoryOptions* options);

    static CARAPI_(void) CloseSilently(
        /* [in] */ IParcelFileDescriptor* c);

    static CARAPI_(AutoPtr<IParcelFileDescriptor>) MakeInputStream(
        /* [in] */ IUri* uri,
        /* [in] */ IContentResolver* cr);

    /**
     * Transform source Bitmap to targeted width and height.
     */
    static CARAPI_(AutoPtr<IBitmap>) Transform(
        /* [in] */ IMatrix* scaler,
        /* [in] */ IBitmap* source,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ Int32 options);

    /**
     * SizedThumbnailBitmap contains the bitmap, which is downsampled either from
     * the thumbnail in exif or the full image.
     * mThumbnailData, mThumbnailWidth and mThumbnailHeight are set together only if mThumbnail
     * is not NULL.
     *
     * The width/height of the sized bitmap may be different from mThumbnailWidth/mThumbnailHeight.
     */
    class SizedThumbnailBitmap
        : public Object
    {
    public:
        SizedThumbnailBitmap()
            : mThumbnailWidth(0)
            , mThumbnailHeight(0)
        {}

        AutoPtr<ArrayOf<Byte> > mThumbnailData;
        AutoPtr<IBitmap> mBitmap;
        Int32 mThumbnailWidth;
        Int32 mThumbnailHeight;
    };

    /**
     * Creates a bitmap by either downsampling from the thumbnail in EXIF or the full image.
     * The functions returns a SizedThumbnailBitmap,
     * which contains a downsampled bitmap and the thumbnail data in EXIF if exists.
     */
    static CARAPI_(void) CreateThumbnailFromEXIF(
        /* [in] */ const String& filePath,
        /* [in] */ Int32 targetSize,
        /* [in] */ Int32 maxPixels,
        /* [in] */ SizedThumbnailBitmap* sizedThumbBitmap);

private:
    static String TAG;

    /* Maximum pixels size for created bitmap. */
    static const Int32 MAX_NUM_PIXELS_THUMBNAIL;
    static const Int32 MAX_NUM_PIXELS_MICRO_THUMBNAIL;
    static const Int32 UNCONSTRAINED;

    /* Options used internally. */
    static const Int32 OPTIONS_NONE;
    static const Int32 OPTIONS_SCALE_UP;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif
