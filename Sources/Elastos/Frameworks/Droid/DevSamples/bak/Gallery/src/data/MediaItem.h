
#ifndef __MEDIAITEM_H__
#define __MEDIAITEM_H__

#include "elastos/droid/app/Activity.h"
#include "Path.h"

namespace Gallery {

extern "C" const InterfaceID EIID_MediaItem;
// MediaItem represents an image or a video item.
class MediaItem
    : public MediaObject
{
public:
    static CARAPI_(Int32) GetTargetSize(
        /* [in] */ Int32 type);

    // BitmapPool getMicroThumbPool();
    // BitmapPool getThumbPool();
    // BytesBufferPool getBytesBufferPool();

    static CARAPI_(void) SetThumbnailSizes(
        /* [in] */ Int32 size,
        /* [in] */ Int32 microSize);

public:
    MediaItem(
        /* [in] */ Path* path,
        /* [in] */ Int64 version);

    // virtual Job<Bitmap> requestImage(int type);
    // virtual Job<BitmapRegionDecoder> requestLargeImage();

    virtual CARAPI_(Int64) GetDateInMs();

    virtual CARAPI_(String) GetName();

    virtual CARAPI_(void) GetLatLong(
        /* [in] */ ArrayOf<Double>* latLong);

    virtual CARAPI_(AutoPtr< ArrayOf<String> >) GetTags();

    virtual CARAPI_(Int32) GetFullImageRotation();

    virtual CARAPI_(Int32) GetRotation();

    virtual CARAPI_(Int64) GetSize();

    virtual CARAPI_(String) GetMimeType() = 0;

    virtual CARAPI_(String) GetFilePath();

    virtual CARAPI_(Int32) GetWidth() = 0;

    virtual CARAPI_(Int32) GetHeight() = 0;

    // virtual ScreenNail getScreenNail();

public:
    // NOTE: These type numbers are stored in the image cache, so it should not
    // not be changed without resetting the cache.
    static const Int32 TYPE_THUMBNAIL;
    static const Int32 TYPE_MICROTHUMBNAIL;
    static const Int32 CACHED_IMAGE_QUALITY;
    static const Int32 IMAGE_READY;
    static const Int32 IMAGE_WAIT;
    static const Int32 IMAGE_ERROR;
    static const String MIME_TYPE_JPEG;
    static const Double INVALID_LATLNG;

private:
    static const String TAG;

    static const Int32 BYTESBUFFE_POOL_SIZE;
    static const Int32 BYTESBUFFER_SIZE;
    static Int32 sMicrothumbnailTargetSize;
    static Int32 sThumbnailTargetSize;

    // TODO
    // static BitmapPool sMicroThumbPool;
    // static final BytesBufferPool sMicroThumbBufferPool =
    //         new BytesBufferPool(BYTESBUFFE_POOL_SIZE, BYTESBUFFER_SIZE);
    // static final BitmapPool sThumbPool = new BitmapPool(4);
};

} // namespace Gallery

#endif // __MEDIAITEM_H__
