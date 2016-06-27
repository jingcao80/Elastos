#include "MediaItem.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Gallery {

// ebba7bbb-8d25-4760-b075-ccc55ab205ef
extern "C" const InterfaceID EIID_MediaItem =
    { 0xebba7bbb, 0x8d25, 0x4760, { 0xb0, 0x75, 0xcc, 0xc5, 0x5a, 0xb2, 0x05, 0xef } };

const Int32 MediaItem::TYPE_THUMBNAIL = 1;
const Int32 MediaItem::TYPE_MICROTHUMBNAIL = 2;
const Int32 MediaItem::CACHED_IMAGE_QUALITY = 95;
const Int32 MediaItem::IMAGE_READY = 0;
const Int32 MediaItem::IMAGE_WAIT = 1;
const Int32 MediaItem::IMAGE_ERROR = -1;
const String MediaItem::MIME_TYPE_JPEG = String("image/jpeg");
const Double MediaItem::INVALID_LATLNG = 0.0;
const String MediaItem::TAG("MediaItem");
const Int32 MediaItem::BYTESBUFFE_POOL_SIZE = 4;
const Int32 MediaItem::BYTESBUFFER_SIZE = 200 * 1024;
Int32 MediaItem::sMicrothumbnailTargetSize = 200;
Int32 MediaItem::sThumbnailTargetSize = 640;

Int32 MediaItem::GetTargetSize(
    /* [in] */ Int32 type)
{
     switch (type) {
        case TYPE_THUMBNAIL:
            return sThumbnailTargetSize;
        case TYPE_MICROTHUMBNAIL:
            return sMicrothumbnailTargetSize;
        default:
            // throw new RuntimeException("should only request thumb/microthumb from cache");
            Logger::E(TAG, "should only request thumb/microthumb from cache");
            return 0;
    }
}

void MediaItem::SetThumbnailSizes(
    /* [in] */ Int32 size,
    /* [in] */ Int32 microSize)
{
    sThumbnailTargetSize = size;
    if (sMicrothumbnailTargetSize != microSize) {
        sMicrothumbnailTargetSize = microSize;
        // initializeMicroThumbPool();
    }
}

MediaItem::MediaItem(
    /* [in] */ Path* path,
    /* [in] */ Int64 version)
    : MediaObject(path, version)
{}

Int64 MediaItem::GetDateInMs()
{
    return 0;
}

String MediaItem::GetName()
{
    return String(NULL);
}

void MediaItem::GetLatLong(
    /* [in] */ ArrayOf<Double>* latLong)
{
    (*latLong)[0] = INVALID_LATLNG;
    (*latLong)[1] = INVALID_LATLNG;
}

AutoPtr< ArrayOf<String> > MediaItem::GetTags()
{
    return NULL;
}

Int32 MediaItem::GetFullImageRotation()
{
    return GetRotation();
}

Int32 MediaItem::GetRotation()
{
    return 0;
}

Int64 MediaItem::GetSize()
{
    return 0;
}

String MediaItem::GetFilePath()
{
    return String("");
}

} // namespace Gallery
