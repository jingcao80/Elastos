#include "LocalImage.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Droid::Media::IExifInterface;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String LocalImage::TAG("LocalImage");
const Int32 LocalImage::INDEX_ID = 0;
const Int32 LocalImage::INDEX_CAPTION = 1;
const Int32 LocalImage::INDEX_MIME_TYPE = 2;
const Int32 LocalImage::INDEX_LATITUDE = 3;
const Int32 LocalImage::INDEX_LONGITUDE = 4;
const Int32 LocalImage::INDEX_DATE_TAKEN = 5;
const Int32 LocalImage::INDEX_DATE_ADDED = 6;
const Int32 LocalImage::INDEX_DATE_MODIFIED = 7;
const Int32 LocalImage::INDEX_DATA = 8;
const Int32 LocalImage::INDEX_ORIENTATION = 9;
const Int32 LocalImage::INDEX_BUCKET_ID = 10;
const Int32 LocalImage::INDEX_SIZE = 11;
const Int32 LocalImage::INDEX_WIDTH = 12;
const Int32 LocalImage::INDEX_HEIGHT = 13;
AutoPtr< ArrayOf<String> > LocalImage::PROJECTION = ArrayOf<String>::Alloc(14);
Boolean LocalImage::sArrayInit = UpdateWidthAndHeightProjection();

LocalImage::UpdateHelper::UpdateHelper()
    : mUpdated(FALSE)
{}

Int32 LocalImage::UpdateHelper::Update(
    /* [in] */ Int32 original,
    /* [in] */ Int32 update)
{
    if (original != update) {
        mUpdated = TRUE;
        original = update;
    }
    return original;
}

Int64 LocalImage::UpdateHelper::Update(
    /* [in] */ Int64 original,
    /* [in] */ Int64 update)
{
    if (original != update) {
        mUpdated = TRUE;
        original = update;
    }
    return original;
}

Double LocalImage::UpdateHelper::Update(
    /* [in] */ Double original,
    /* [in] */ Double update)
{
    if (original != update) {
        mUpdated = TRUE;
        original = update;
    }
    return original;
}

String LocalImage::UpdateHelper::Update(
    /* [in] */ const String& original,
    /* [in] */ const String& update)
{
    String tmp(original);
    if (original.IsNullOrEmpty() || !original.Equals(update)) {
        mUpdated = TRUE;
        tmp = update;
    }
    return tmp;
}

Boolean LocalImage::UpdateHelper::IsUpdated()
{
    return mUpdated;
}

LocalImage::LocalImage(
    /* [in] */ Path* path,
    /* [in] */ IContext* application,
    /* [in] */ ICursor* cursor)
    : LocalMediaItem(path, NextVersionNumber())
    , rotation(0)
    , mApplication(application)
{
    LoadFromCursor(cursor);
}

LocalImage::LocalImage(
    /* [in] */ Path* path,
    /* [in] */ IContext* application,
    /* [in] */ Int32 id)
    : LocalMediaItem(path, NextVersionNumber())
    , rotation(0)
    , mApplication(application)
{
    // ContentResolver resolver = mApplication.getContentResolver();
    // Uri uri = Images.Media.EXTERNAL_CONTENT_URI;
    // Cursor cursor = LocalAlbum.getItemCursor(resolver, uri, PROJECTION, id);
    // if (cursor == null) {
    //     throw new RuntimeException("cannot get cursor for: " + path);
    // }
    // try {
    //     if (cursor.moveToNext()) {
    //         loadFromCursor(cursor);
    //     } else {
    //         throw new RuntimeException("cannot find data for: " + path);
    //     }
    // } finally {
    //     cursor.close();
    // }
}

PInterface LocalImage::Probe(
    /* [in] */ REIID riid)
{
    Logger::I(TAG, "Probe()----");
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_MediaObject) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_MediaItem) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_LocalMediaItem) {
        return reinterpret_cast<PInterface>(this);
    }

    return NULL;
}

Int32 LocalImage::GetSupportedOperations()
{
    // StitchingProgressManager progressManager = mApplication.getStitchingProgressManager();
    // if (progressManager != null && progressManager.getProgress(getContentUri()) != null) {
    //     return 0; // doesn't support anything while stitching!
    // }
    // int operation = SUPPORT_DELETE | SUPPORT_SHARE | SUPPORT_CROP
    //         | SUPPORT_SETAS | SUPPORT_EDIT | SUPPORT_INFO;
    // if (BitmapUtils.isSupportedByRegionDecoder(mimeType)) {
    //     operation |= SUPPORT_FULL_IMAGE;
    // }

    // if (BitmapUtils.isRotationSupported(mimeType)) {
    //     operation |= SUPPORT_ROTATE;
    // }

    // if (GalleryUtils.isValidLocation(latitude, longitude)) {
    //     operation |= SUPPORT_SHOW_ON_MAP;
    // }
    // return operation;
    return 0;
}

ECode LocalImage::Delete()
{
    // GalleryUtils.assertNotInRenderThread();
    // Uri baseUri = Images.Media.EXTERNAL_CONTENT_URI;
    // mApplication.getContentResolver().delete(baseUri, "_id=?", new String[]{String.valueOf(id)});
    return NOERROR;
}

ECode LocalImage::Rotate(
    /* [in] */ Int32 degrees)
{
    // GalleryUtils.assertNotInRenderThread();
    // Uri baseUri = Images.Media.EXTERNAL_CONTENT_URI;
    // ContentValues values = new ContentValues();
    // int rotation = (this.rotation + degrees) % 360;
    // if (rotation < 0) rotation += 360;

    // if (mimeType.equalsIgnoreCase("image/jpeg")) {
    //     try {
    //         ExifInterface exif = new ExifInterface(filePath);
    //         exif.setAttribute(ExifInterface.TAG_ORIENTATION,
    //                 getExifOrientation(rotation));
    //         exif.saveAttributes();
    //     } catch (IOException e) {
    //         Log.w(TAG, "cannot set exif data: " + filePath);
    //     }

    //     // We need to update the filesize as well
    //     fileSize = new File(filePath).length();
    //     values.put(Images.Media.SIZE, fileSize);
    // }

    // values.put(Images.Media.ORIENTATION, rotation);
    // mApplication.getContentResolver().update(baseUri, values, "_id=?",
    //         new String[]{String.valueOf(id)});
    return NOERROR;
}

AutoPtr<IUri> LocalImage::GetContentUri()
{
    // Uri baseUri = Images.Media.EXTERNAL_CONTENT_URI;
    // return baseUri.buildUpon().appendPath(String.valueOf(id)).build();
    return NULL;
}

Int32 LocalImage::GetMediaType()
{
    return MEDIA_TYPE_IMAGE;
}

AutoPtr<MediaDetails> LocalImage::GetDetails()
{
    AutoPtr<MediaDetails> details = LocalMediaItem::GetDetails();
    AutoPtr<IInteger32> r;
    CInteger32::New(rotation, (IInteger32**)&r);
    details->AddDetail(MediaDetails::INDEX_ORIENTATION, r);
    if (MIME_TYPE_JPEG.Equals(mimeType)) {
        // ExifInterface returns incorrect values for photos in other format.
        // For example, the width and height of an webp images is always '0'.
        MediaDetails::ExtractExifInfo(details, filePath);
    }
    return details;
}

Int32 LocalImage::GetRotation()
{
    return rotation;
}

Int32 LocalImage::GetWidth()
{
    return LocalMediaItem::width;
}

Int32 LocalImage::GetHeight()
{
    return LocalMediaItem::height;
}

String LocalImage::GetFilePath()
{
    return LocalMediaItem::filePath;
}

Boolean LocalImage::UpdateFromCursor(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<UpdateHelper> uh = new UpdateHelper();
    Int32 tmpInt32 = 0;
    Int64 tmpInt64 = 0;
    Double tmpDouble = 0.0;
    String tmpStr;
    id = uh->Update(id, (cursor->GetInt32(INDEX_ID, &tmpInt32), tmpInt32));
    caption = uh->Update(caption, (cursor->GetString(INDEX_CAPTION, &tmpStr), tmpStr));
    mimeType = uh->Update(mimeType, (cursor->GetString(INDEX_MIME_TYPE, &tmpStr), tmpStr));
    latitude = uh->Update(latitude, (cursor->GetDouble(INDEX_LATITUDE, &tmpDouble), tmpDouble));
    longitude = uh->Update(longitude, (cursor->GetDouble(INDEX_LONGITUDE, &tmpDouble), tmpDouble));
    dateTakenInMs = uh->Update(
            dateTakenInMs, (cursor->GetInt64(INDEX_DATE_TAKEN, &tmpInt64), tmpInt64));
    dateAddedInSec = uh->Update(
            dateAddedInSec, (cursor->GetInt64(INDEX_DATE_ADDED, &tmpInt64), tmpInt64));
    dateModifiedInSec = uh->Update(
            dateModifiedInSec, (cursor->GetInt64(INDEX_DATE_MODIFIED, &tmpInt64), tmpInt64));
    filePath = uh->Update(filePath, (cursor->GetString(INDEX_DATA, &tmpStr), tmpStr));
    rotation = uh->Update(rotation, (cursor->GetInt32(INDEX_ORIENTATION, &tmpInt32), tmpInt32));
    bucketId = uh->Update(bucketId, (cursor->GetInt32(INDEX_BUCKET_ID, &tmpInt32), tmpInt32));
    fileSize = uh->Update(fileSize, (cursor->GetInt64(INDEX_SIZE, &tmpInt64), tmpInt64));
    width = uh->Update(width, (cursor->GetInt32(INDEX_WIDTH, &tmpInt32), tmpInt32));
    height = uh->Update(height, (cursor->GetInt32(INDEX_HEIGHT, &tmpInt32), tmpInt32));
    return uh->IsUpdated();
}

Boolean LocalImage::UpdateWidthAndHeightProjection()
{
    // PROJECTION = ArrayOf<String>::Alloc(14);
    (*PROJECTION)[0] = String("_id"); // ImageColumns._ID
    (*PROJECTION)[1] = String("title"); // ImageColumns.TITLE
    (*PROJECTION)[2] = String("mime_type"); // ImageColumns.MIME_TYPE
    (*PROJECTION)[3] = String("latitude"); // ImageColumns.LATITUDE
    (*PROJECTION)[4] = String("longitude"); // ImageColumns.LONGITUDE
    (*PROJECTION)[5] = String("datetaken"); // ImageColumns.DATE_TAKEN
    (*PROJECTION)[6] = String("date_added"); // ImageColumns.DATE_ADDED
    (*PROJECTION)[7] = String("date_modified"); // ImageColumns.DATE_MODIFIED
    (*PROJECTION)[8] = String("_data"); // ImageColumns.DATA
    (*PROJECTION)[9] = String("orientation"); // ImageColumns.ORIENTATION
    (*PROJECTION)[10] = String("bucket_id"); // ImageColumns.BUCKET_ID
    (*PROJECTION)[11] = String("_size"); // ImageColumns.SIZE
    (*PROJECTION)[INDEX_WIDTH] = String("width"); // MediaColumns.WIDTH
    (*PROJECTION)[INDEX_HEIGHT] = String("height"); // MediaColumns.HEIGHT
    return TRUE;
}

String LocalImage::GetExifOrientation(
    /* [in] */ Int32 orientation)
{
    switch (orientation) {
        case 0:
            return StringUtils::Int32ToString(IExifInterface::ORIENTATION_NORMAL);
        case 90:
            return StringUtils::Int32ToString(IExifInterface::ORIENTATION_ROTATE_90);
        case 180:
            return StringUtils::Int32ToString(IExifInterface::ORIENTATION_ROTATE_180);
        case 270:
            return StringUtils::Int32ToString(IExifInterface::ORIENTATION_ROTATE_270);
        default:
            // throw new AssertionError("invalid: " + orientation);
            Logger::E(TAG, "invalid:%d", orientation);
            return String(NULL);
    }
}

void LocalImage::LoadFromCursor(
    /* [in] */ ICursor* cursor)
{
    cursor->GetInt32(INDEX_ID, &id);
    cursor->GetString(INDEX_CAPTION, &caption);
    cursor->GetString(INDEX_MIME_TYPE, &mimeType);
    cursor->GetDouble(INDEX_LATITUDE, &latitude);
    cursor->GetDouble(INDEX_LONGITUDE, &longitude);
    cursor->GetInt64(INDEX_DATE_TAKEN, &dateTakenInMs);
    cursor->GetInt64(INDEX_DATE_ADDED, &dateAddedInSec);
    cursor->GetInt64(INDEX_DATE_MODIFIED, &dateModifiedInSec);
    cursor->GetString(INDEX_DATA, &filePath);
    cursor->GetInt32(INDEX_ORIENTATION, &rotation);
    cursor->GetInt32(INDEX_BUCKET_ID, &bucketId);
    cursor->GetInt64(INDEX_SIZE, &fileSize);
    cursor->GetInt32(INDEX_WIDTH, &width);
    cursor->GetInt32(INDEX_HEIGHT, &height);
}

} // namespace Gallery
