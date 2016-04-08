
#ifndef __MEDIAOBJECT_H__
#define __MEDIAOBJECT_H__

#include "elastos/droid/app/Activity.h"
#include "Path.h"
#include "MediaDetails.h"

using Elastos::Droid::Net::IUri;

namespace Gallery {

extern "C" const InterfaceID EIID_MediaObject;

class Path;

class MediaObject
    : public ElRefBase
    , public IInterface
{
public:
    static CARAPI_(Int64) NextVersionNumber();

    static CARAPI_(Int64) GetTypeFromString(
        /* [in] */ const String& s);

    static CARAPI_(String) GetTypeString(
        /* [in] */ Int32 type);

public:
    MediaObject(
        /* [in] */ Path* path,
        /* [in] */ Int64 version);

    CAR_INTERFACE_DECL();

    virtual CARAPI_(AutoPtr<Path>) GetPath();

    virtual CARAPI_(Int32) GetSupportedOperations();

    virtual CARAPI Delete();

    virtual CARAPI Rotate(
        /* [in] */ Int32 degrees);

    virtual CARAPI_(AutoPtr<IUri>) GetContentUri();

    virtual CARAPI_(AutoPtr<IUri>) GetPlayUri();

    virtual CARAPI_(Int32) GetMediaType();

    virtual CARAPI_(Boolean) Import();

    virtual CARAPI_(AutoPtr<MediaDetails>) GetDetails();

    virtual CARAPI_(Int64) GetDataVersion();

    virtual CARAPI_(Int32) GetCacheFlag();

    virtual CARAPI_(Int32) GetCacheStatus();

    virtual CARAPI_(Int64) GetCacheSize();

    virtual CARAPI Cache(
        /* [in] */ Int32 flag);

public:
    static const Int32 INVALID_DATA_VERSION;

    // These are the bits returned from getSupportedOperations():
    static const Int32 SUPPORT_DELETE;
    static const Int32 SUPPORT_ROTATE;
    static const Int32 SUPPORT_SHARE;
    static const Int32 SUPPORT_CROP;
    static const Int32 SUPPORT_SHOW_ON_MAP;
    static const Int32 SUPPORT_SETAS;
    static const Int32 SUPPORT_FULL_IMAGE;
    static const Int32 SUPPORT_PLAY;
    static const Int32 SUPPORT_CACHE;
    static const Int32 SUPPORT_EDIT;
    static const Int32 SUPPORT_INFO;
    static const Int32 SUPPORT_IMPORT;
    static const Int32 SUPPORT_TRIM;
    static const Int32 SUPPORT_UNLOCK;
    static const Int32 SUPPORT_BACK;
    static const Int32 SUPPORT_ACTION;
    static const Int32 SUPPORT_CAMERA_SHORTCUT;
    static const Int32 SUPPORT_ALL;

    // These are the bits returned from getMediaType():
    static const Int32 MEDIA_TYPE_UNKNOWN;
    static const Int32 MEDIA_TYPE_IMAGE;
    static const Int32 MEDIA_TYPE_VIDEO;
    static const Int32 MEDIA_TYPE_ALL;

    static const String MEDIA_TYPE_IMAGE_STRING;
    static const String MEDIA_TYPE_VIDEO_STRING;
    static const String MEDIA_TYPE_ALL_STRING;

    // These are flags for cache() and return values for getCacheFlag():
    static const Int32 CACHE_FLAG_NO;
    static const Int32 CACHE_FLAG_SCREENNAIL;
    static const Int32 CACHE_FLAG_FULL;

    // These are return values for getCacheStatus():
    static const Int32 CACHE_STATUS_NOT_CACHED;
    static const Int32 CACHE_STATUS_CACHING;
    static const Int32 CACHE_STATUS_CACHED_SCREENNAIL;
    static const Int32 CACHE_STATUS_CACHED_FULL;

public:
    Int64 mDataVersion;
    AutoPtr<Path> mPath;

private:
    static const String TAG;
    static Int64 sVersionSerial;

};

} // namespace Gallery

#endif // __MEDIAOBJECT_H__
