
#ifndef __LOCALSOURCE_H__
#define __LOCALSOURCE_H__

#include "app/Activity.h"
#include "MediaSource.h"
#include "MediaObject.h"
#include "PathMatcher.h"
#include "Path.h"
#include "MediaSet.h"

using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IUriMatcher;

namespace Gallery {

class LocalSource
    : public MediaSource
{
public:
	LocalSource(
        /* [in] */ IContext* context);

	virtual CARAPI_(AutoPtr<MediaObject>) CreateMediaObject(
		/* [in] */ Path* path);

    virtual CARAPI_(AutoPtr<Path>) FindPathByUri(
        /* [in] */ IUri* uri,
        /* [in] */ const String& type);

    virtual CARAPI_(AutoPtr<Path>) GetDefaultSetOf(
        /* [in] */ Path* item);

    virtual CARAPI_(void) MapMediaItems(
        /* [in] */ ArrayOf<PathId*>* list,
        /* [in] */ MediaSet::ItemConsumer* consumer);

    virtual CARAPI_(void) Resume();

    virtual CARAPI_(void) Pause();

private:
    static CARAPI_(Int32) GetMediaType(
        /* [in] */ const String& type,
        /* [in] */ Int32 defaultType);

    CARAPI_(AutoPtr<Path>) GetAlbumPath(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 defaultType);

    CARAPI_(void) ProcessMapMediaItems(
        /* [in] */ ArrayOf<PathId*>* list,
        /* [in] */ MediaSet::ItemConsumer* consumer,
        /* [in] */ Boolean isImage);

public:
	static const String KEY_BUCKET_ID;
	// static final Comparator<PathId> sIdComparator = new IdComparator();

private:
    static const String TAG;
	static const Int32 NO_MATCH;

    static const Int32 LOCAL_IMAGE_ALBUMSET;
    static const Int32 LOCAL_VIDEO_ALBUMSET;
    static const Int32 LOCAL_IMAGE_ALBUM;
    static const Int32 LOCAL_VIDEO_ALBUM;
    static const Int32 LOCAL_IMAGE_ITEM;
    static const Int32 LOCAL_VIDEO_ITEM;
    static const Int32 LOCAL_ALL_ALBUMSET;
    static const Int32 LOCAL_ALL_ALBUM;

    // The media type bit passed by the intent
    static const Int32 MEDIA_TYPE_ALL;
    static const Int32 MEDIA_TYPE_IMAGE;
    static const Int32 MEDIA_TYPE_VIDEO;

    AutoPtr<IContext> mApplication;
    AutoPtr<PathMatcher> mMatcher;
    AutoPtr<IUriMatcher> mUriMatcher;
    AutoPtr<IContentProviderClient> mClient;

};

} // namespace Gallery

#endif // __LOCALSOURCE_H__