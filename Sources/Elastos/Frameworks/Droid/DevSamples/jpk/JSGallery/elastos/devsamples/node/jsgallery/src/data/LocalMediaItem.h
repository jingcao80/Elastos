
#ifndef __LOCALMEDIAITEM_H__
#define __LOCALMEDIAITEM_H__

#include "app/Activity.h"
#include "Path.h"
#include "MediaItem.h"

using Elastos::Droid::Database::ICursor;

namespace Gallery {

extern "C" const InterfaceID EIID_LocalMediaItem;

// LocalMediaItem is an abstract class captures those common fields
// in LocalImage and LocalVideo.
class LocalMediaItem
    : public MediaItem
{
public:
    LocalMediaItem(
        /* [in] */ Path* path,
        /* [in] */ Int64 version);

    // virtual Job<Bitmap> requestImage(int type);
    // virtual Job<BitmapRegionDecoder> requestLargeImage();

    virtual CARAPI_(Int64) GetDateInMs();

    virtual CARAPI_(String) GetName();

    virtual CARAPI_(void) GetLatLong(
        /* [in] */ ArrayOf<Double>* latLong);

    virtual CARAPI_(Int32) GetBucketId();

    virtual CARAPI_(AutoPtr<MediaDetails>) GetDetails();

    virtual CARAPI_(Int64) GetSize();

    virtual CARAPI_(String) GetMimeType();

protected:
    virtual CARAPI_(Boolean) UpdateFromCursor(
        /* [in] */ ICursor* cursor) = 0;

    virtual CARAPI_(void) UpdateContent(
        /* [in] */ ICursor* cursor);

public:
    // database fields
    Int32 id;
    Int32 bucketId;
    Int32 width;
    Int32 height;
    Int64 fileSize;
    Int64 dateTakenInMs;
    Int64 dateAddedInSec;
    Int64 dateModifiedInSec;
    Double latitude;
    Double longitude;
    String caption;
    String mimeType;
    String filePath;

private:
    static const String TAG;

};

} // namespace Gallery

#endif // __LOCALMEDIAITEM_H__
