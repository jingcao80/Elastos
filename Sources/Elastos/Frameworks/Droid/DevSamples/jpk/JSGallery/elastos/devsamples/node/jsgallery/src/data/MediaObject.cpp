#include "MediaObject.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Gallery {

// 98f52e89-3ac3-4a77-ae0e-f4672952a922
extern "C" const InterfaceID EIID_MediaObject =
    { 0x98f52e89, 0x3ac3, 0x4a77, { 0xae, 0x0e, 0xf4, 0x67, 0x29, 0x52, 0xa9, 0x22 } };

const Int32 MediaObject::INVALID_DATA_VERSION = -1;

const Int32 MediaObject::SUPPORT_DELETE = 1 << 0;
const Int32 MediaObject::SUPPORT_ROTATE = 1 << 1;
const Int32 MediaObject::SUPPORT_SHARE = 1 << 2;
const Int32 MediaObject::SUPPORT_CROP = 1 << 3;
const Int32 MediaObject::SUPPORT_SHOW_ON_MAP = 1 << 4;
const Int32 MediaObject::SUPPORT_SETAS = 1 << 5;
const Int32 MediaObject::SUPPORT_FULL_IMAGE = 1 << 6;
const Int32 MediaObject::SUPPORT_PLAY = 1 << 7;
const Int32 MediaObject::SUPPORT_CACHE = 1 << 8;
const Int32 MediaObject::SUPPORT_EDIT = 1 << 9;
const Int32 MediaObject::SUPPORT_INFO = 1 << 10;
const Int32 MediaObject::SUPPORT_IMPORT = 1 << 11;
const Int32 MediaObject::SUPPORT_TRIM = 1 << 12;
const Int32 MediaObject::SUPPORT_UNLOCK = 1 << 13;
const Int32 MediaObject::SUPPORT_BACK = 1 << 14;
const Int32 MediaObject::SUPPORT_ACTION = 1 << 15;
const Int32 MediaObject::SUPPORT_CAMERA_SHORTCUT = 1 << 16;
const Int32 MediaObject::SUPPORT_ALL = 0xffffffff;

const Int32 MediaObject::MEDIA_TYPE_UNKNOWN = 1;
const Int32 MediaObject::MEDIA_TYPE_IMAGE = 2;
const Int32 MediaObject::MEDIA_TYPE_VIDEO = 4;
const Int32 MediaObject::MEDIA_TYPE_ALL = MEDIA_TYPE_IMAGE | MEDIA_TYPE_VIDEO;

const String MediaObject::MEDIA_TYPE_IMAGE_STRING = String("image");
const String MediaObject::MEDIA_TYPE_VIDEO_STRING = String("video");
const String MediaObject::MEDIA_TYPE_ALL_STRING = String("all");

const Int32 MediaObject::CACHE_FLAG_NO = 0;
const Int32 MediaObject::CACHE_FLAG_SCREENNAIL = 1;
const Int32 MediaObject::CACHE_FLAG_FULL = 2;

const Int32 MediaObject::CACHE_STATUS_NOT_CACHED = 0;
const Int32 MediaObject::CACHE_STATUS_CACHING = 1;
const Int32 MediaObject::CACHE_STATUS_CACHED_SCREENNAIL = 2;
const Int32 MediaObject::CACHE_STATUS_CACHED_FULL = 3;

const String MediaObject::TAG("MediaObject");
Int64 MediaObject::sVersionSerial = 0;

Int64 MediaObject::NextVersionNumber()
{
    return ++sVersionSerial;
}

Int64 MediaObject::GetTypeFromString(
    /* [in] */ const String& s)
{
    if (MEDIA_TYPE_ALL_STRING.Equals(s)) return MEDIA_TYPE_ALL;
    if (MEDIA_TYPE_IMAGE_STRING.Equals(s)) return MEDIA_TYPE_IMAGE;
    if (MEDIA_TYPE_VIDEO_STRING.Equals(s)) return MEDIA_TYPE_VIDEO;
    // throw new IllegalArgumentException(s);
    Logger::E(TAG, "IllegalArgumentException");
    return 0;
}

String MediaObject::GetTypeString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case MEDIA_TYPE_IMAGE: return MEDIA_TYPE_IMAGE_STRING;
        case MEDIA_TYPE_VIDEO: return MEDIA_TYPE_VIDEO_STRING;
        case MEDIA_TYPE_ALL: return MEDIA_TYPE_ALL_STRING;
    }
    // throw new IllegalArgumentException();
    Logger::E(TAG, "IllegalArgumentException");
    return String(NULL);
}

MediaObject::MediaObject(
    /* [in] */ Path* path,
    /* [in] */ Int64 version)
    : mDataVersion(version)
    , mPath(path)
{
    path->SetObject(this);
}

CAR_INTERFACE_IMPL(MediaObject, IInterface)

AutoPtr<Path> MediaObject::GetPath()
{
    return mPath;
}

Int32 MediaObject::GetSupportedOperations()
{
    return 0;
}

ECode MediaObject::Delete()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MediaObject::Rotate(
    /* [in] */ Int32 degrees)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

AutoPtr<IUri> MediaObject::GetContentUri()
{
    Logger::E(TAG, "UnsupportedOperationException");
    return NULL;
}

AutoPtr<IUri> MediaObject::GetPlayUri()
{
    Logger::E(TAG, "UnsupportedOperationException");
    return NULL;
}

Int32 MediaObject::GetMediaType()
{
    return MEDIA_TYPE_UNKNOWN;
}

Boolean MediaObject::Import()
{
    Logger::E(TAG, "UnsupportedOperationException");
    return FALSE;
}

AutoPtr<MediaDetails> MediaObject::GetDetails()
{
    AutoPtr<MediaDetails> details = new MediaDetails();
    return details;
}

Int64 MediaObject::GetDataVersion()
{
    return mDataVersion;
}

Int32 MediaObject::GetCacheFlag()
{
    return CACHE_FLAG_NO;
}

Int32 MediaObject::GetCacheStatus()
{
    Logger::E(TAG, "UnsupportedOperationException");
    return 0;
}

Int64 MediaObject::GetCacheSize()
{
    Logger::E(TAG, "UnsupportedOperationException");
    return 0;
}

ECode MediaObject::Cache(
    /* [in] */ Int32 flag)
{
    Logger::E(TAG, "UnsupportedOperationException");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Gallery
