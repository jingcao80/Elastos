#include "LocalSource.h"
#include "DataManager.h"
#include "LocalAlbum.h"
#include "LocalImage.h"
#include "DataSourceHelper.h"
#include <elastos/Mutex.h>
#include <elastos/Logger.h>
#include <elastos/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::Threading::Mutex;
using Elastos::Droid::Content::CUriMatcher;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String LocalSource::KEY_BUCKET_ID = String("bucketId");
const String LocalSource::TAG("LocalSource");
const Int32 LocalSource::NO_MATCH = -1;
const Int32 LocalSource::LOCAL_IMAGE_ALBUMSET = 0;
const Int32 LocalSource::LOCAL_VIDEO_ALBUMSET = 1;
const Int32 LocalSource::LOCAL_IMAGE_ALBUM = 2;
const Int32 LocalSource::LOCAL_VIDEO_ALBUM = 3;
const Int32 LocalSource::LOCAL_IMAGE_ITEM = 4;
const Int32 LocalSource::LOCAL_VIDEO_ITEM = 5;
const Int32 LocalSource::LOCAL_ALL_ALBUMSET = 6;
const Int32 LocalSource::LOCAL_ALL_ALBUM = 7;

// The media type bit passed by the intent
const Int32 LocalSource::MEDIA_TYPE_ALL = 0;
const Int32 LocalSource::MEDIA_TYPE_IMAGE = 1;
const Int32 LocalSource::MEDIA_TYPE_VIDEO = 4;

LocalSource::LocalSource(
    /* [in] */ IContext* context)
    : MediaSource(String("local"))
    , mApplication(context)
{
	CUriMatcher::New(NO_MATCH, (IUriMatcher**)&mUriMatcher);
	mMatcher = new PathMatcher();
    mMatcher->Add(String("/local/image"), LOCAL_IMAGE_ALBUMSET);
    mMatcher->Add(String("/local/video"), LOCAL_VIDEO_ALBUMSET);
    mMatcher->Add(String("/local/all"), LOCAL_ALL_ALBUMSET);
    mMatcher->Add(String("/local/image/*"), LOCAL_IMAGE_ALBUM);
    mMatcher->Add(String("/local/video/*"), LOCAL_VIDEO_ALBUM);
    mMatcher->Add(String("/local/all/*"), LOCAL_ALL_ALBUM);
    mMatcher->Add(String("/local/image/item/*"), LOCAL_IMAGE_ITEM);
    mMatcher->Add(String("/local/video/item/*"), LOCAL_VIDEO_ITEM);

    mUriMatcher->AddURI(String("media") /*MediaStore.AUTHORITY*/, String("external/images/media/#"), LOCAL_IMAGE_ITEM);
    mUriMatcher->AddURI(String("media") /*MediaStore.AUTHORITY*/, String("external/video/media/#"), LOCAL_VIDEO_ITEM);
    mUriMatcher->AddURI(String("media") /*MediaStore.AUTHORITY*/, String("external/images/media"), LOCAL_IMAGE_ALBUM);
    mUriMatcher->AddURI(String("media") /*MediaStore.AUTHORITY*/, String("external/video/media"), LOCAL_VIDEO_ALBUM);
    mUriMatcher->AddURI(String("media") /*MediaStore.AUTHORITY*/, String("external/file"), LOCAL_ALL_ALBUM);
}

AutoPtr<MediaObject> LocalSource::CreateMediaObject(
	/* [in] */ Path* path)
{
    Logger::D(TAG, "CreateMediaObject()-----");
	switch (mMatcher->Match(path)) {
        // case LOCAL_ALL_ALBUMSET:
        // case LOCAL_IMAGE_ALBUMSET:
        // case LOCAL_VIDEO_ALBUMSET:
        //     return new LocalAlbumSet(path, mApplication);
        case LOCAL_IMAGE_ALBUM:
            return new LocalAlbum(path, mApplication, mMatcher->GetInt32Var(0), TRUE);
        case LOCAL_VIDEO_ALBUM:
            return new LocalAlbum(path, mApplication, mMatcher->GetInt32Var(0), FALSE);
        // case LOCAL_ALL_ALBUM: {
        //     int bucketId = mMatcher.getIntVar(0);
        //     DataManager dataManager = app.getDataManager();
        //     MediaSet imageSet = (MediaSet) dataManager.getMediaObject(
        //             LocalAlbumSet.PATH_IMAGE.getChild(bucketId));
        //     MediaSet videoSet = (MediaSet) dataManager.getMediaObject(
        //             LocalAlbumSet.PATH_VIDEO.getChild(bucketId));
        //     Comparator<MediaItem> comp = DataManager.sDateTakenComparator;
        //     return new LocalMergeAlbum(
        //             path, comp, new MediaSet[] {imageSet, videoSet}, bucketId);
        // }
        case LOCAL_IMAGE_ITEM:
            // return new LocalImage(path, mApplication, mMatcher->GetInt32Var(0));
        // case LOCAL_VIDEO_ITEM:
        //     return new LocalVideo(path, mApplication, mMatcher.getIntVar(0));
        default:
            // throw new RuntimeException("bad path: " + path);
        	Logger::E(TAG, "bad path!");
        	return NULL;
    }
}

AutoPtr<Path> LocalSource::FindPathByUri(
    /* [in] */ IUri* uri,
    /* [in] */ const String& type)
{
    Logger::D(TAG, "FindPathByUri()-----");
    Int32 matchCode = 0;
    mUriMatcher->Match(uri, &matchCode);
    switch (matchCode) {
        case LOCAL_IMAGE_ITEM: {
            // long id = ContentUris.parseId(uri);
            Int64 id = 0;
            String last;
            uri->GetLastPathSegment(&last);
            id = last.IsNullOrEmpty() ? -1 : StringUtils::ParseInt64(last);
            return id >= 0 ? Path::ITEM_PATH->GetChild(id) : NULL;
        }
        // case LOCAL_VIDEO_ITEM: {
        //     long id = ContentUris.parseId(uri);
        //     return id >= 0 ? LocalVideo.ITEM_PATH.getChild(id) : null;
        // }
        case LOCAL_IMAGE_ALBUM: {
            return GetAlbumPath(uri, MEDIA_TYPE_IMAGE);
        }
        case LOCAL_VIDEO_ALBUM: {
            return GetAlbumPath(uri, MEDIA_TYPE_VIDEO);
        }
        case LOCAL_ALL_ALBUM: {
            return GetAlbumPath(uri, MEDIA_TYPE_ALL);
        }
        default: {
            String str;
            uri->ToString(&str);
            Logger::W(TAG, "no match uri:%s", str.string());
            return NULL;
        }
    }
}

AutoPtr<Path> LocalSource::GetDefaultSetOf(
    /* [in] */ Path* item)
{
    // AutoPtr<MediaObject> object = DataSourceHelper::GetDataManager(mApplication)->GetMediaObject(item);
    // if (object instanceof LocalMediaItem) {
    //     return Path.fromString("/local/all").getChild(
    //             String.valueOf(((LocalMediaItem) object).getBucketId()));
    // }

    return NULL;
}

void LocalSource::MapMediaItems(
    /* [in] */ ArrayOf<PathId*>* list,
    /* [in] */ MediaSet::ItemConsumer* consumer)
{
    // ArrayList<PathId> imageList = new ArrayList<PathId>();
    // ArrayList<PathId> videoList = new ArrayList<PathId>();
    // int n = list.size();
    // for (int i = 0; i < n; i++) {
    //     PathId pid = list.get(i);
    //     // We assume the form is: "/local/{image,video}/item/#"
    //     // We don't use mMatcher for efficiency's reason.
    //     Path parent = pid.path.getParent();
    //     if (parent == LocalImage.ITEM_PATH) {
    //         imageList.add(pid);
    //     } else if (parent == LocalVideo.ITEM_PATH) {
    //         videoList.add(pid);
    //     }
    // }
    // // TODO: use "files" table so we can merge the two cases.
    // processMapMediaItems(imageList, consumer, true);
    // processMapMediaItems(videoList, consumer, false);
}

void LocalSource::Resume()
{
    if (mClient != NULL) {
        mClient->ReleaseProvider(&ret);
        mClient = NULL;
    }

    AutoPtr<IContentResolver> resolver;
    mApplication->GetContentResolver((IContentResolver**)&resolver);
    resolver->AcquireContentProviderClientEx(String("media") /*MediaStore.AUTHORITY*/, (IContentProviderClient**)&mClient);
}

void LocalSource::Pause()
{
    Boolean ret = FALSE;
    mClient->ReleaseProvider(&ret);
    mClient = NULL;
}

Int32 LocalSource::GetMediaType(
    /* [in] */ const String& type,
    /* [in] */ Int32 defaultType)
{
    if (type.IsNullOrEmpty()) return defaultType;
    Int32 value = StringUtils::ParseInt32(type);
    if ((value & (MEDIA_TYPE_IMAGE | MEDIA_TYPE_VIDEO)) != 0) return value;
    return defaultType;
}

AutoPtr<Path> LocalSource::GetAlbumPath(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 defaultType)
{
    String param;
    uri->GetQueryParameter(String("mediaTypes")/*Gallery.KEY_MEDIA_TYPES*/, &param);
    Int32 mediaType = GetMediaType(param, defaultType);
    String bucketId;
    uri->GetQueryParameter(KEY_BUCKET_ID, &bucketId);
    Int32 id = StringUtils::ParseInt32(bucketId);
    if (id == 0) {
        Logger::W(TAG, "invalid bucket id:%s", bucketId.string());
        return NULL;
    }
    switch (mediaType) {
        case MEDIA_TYPE_IMAGE:
            return Path::FromString(String("/local/image"))->GetChild(id);
        case MEDIA_TYPE_VIDEO:
            return Path::FromString(String("/local/video"))->GetChild(id);
        default:
            return Path::FromString(String("/local/all"))->GetChild(id);
    }
}

void LocalSource::ProcessMapMediaItems(
    /* [in] */ ArrayOf<PathId*>* list,
    /* [in] */ MediaSet::ItemConsumer* consumer,
    /* [in] */ Boolean isImage)
{
    // // Sort path by path id
    // Collections.sort(list, sIdComparator);
    // int n = list.size();
    // for (int i = 0; i < n; ) {
    //     PathId pid = list.get(i);

    //     // Find a range of items.
    //     ArrayList<Integer> ids = new ArrayList<Integer>();
    //     int startId = Integer.parseInt(pid.path.getSuffix());
    //     ids.add(startId);

    //     int j;
    //     for (j = i + 1; j < n; j++) {
    //         PathId pid2 = list.get(j);
    //         int curId = Integer.parseInt(pid2.path.getSuffix());
    //         if (curId - startId >= MediaSet.MEDIAITEM_BATCH_FETCH_COUNT) {
    //             break;
    //         }
    //         ids.add(curId);
    //     }

    //     MediaItem[] items = LocalAlbum.getMediaItemById(
    //             mApplication, isImage, ids);
    //     for(int k = i ; k < j; k++) {
    //         PathId pid2 = list.get(k);
    //         consumer.consume(pid2.id, items[k - i]);
    //     }

    //     i = j;
    // }
}

} // namespace Gallery
