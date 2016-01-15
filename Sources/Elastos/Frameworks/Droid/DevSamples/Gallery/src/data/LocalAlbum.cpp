#include "LocalAlbum.h"
#include "MediaDetails.h"
#include "LocalImage.h"
#include "BucketHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String LocalAlbum::TAG("LocalAlbum");
const Int32 LocalAlbum::INVALID_COUNT = -1;
AutoPtr< ArrayOf<String> > LocalAlbum::COUNT_PROJECTION;
Boolean LocalAlbum::sIsInit = ArrayInit();

AutoPtr< ArrayOf<MediaItem*> > LocalAlbum::GetMediaItemById(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isImage,
    /* [in] */ ArrayOf<Int32>* ids)
{
    return NULL;
}

AutoPtr<ICursor> LocalAlbum::GetItemCursor(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ Int32 id)
{
    return NULL;
}

String LocalAlbum::GetLocalizedName(
    /* [in] */ IResources* res,
    /* [in] */ Int32 bucketId,
    /* [in] */ const String& name)
{
    return String(NULL);
}

LocalAlbum::LocalAlbum(
    /* [in] */ Path* path,
    /* [in] */ IContext* context,
    /* [in] */ Int32 bucketId,
    /* [in] */ Boolean isImage,
    /* [in] */ const String& name)
    : MediaSet(path, NextVersionNumber())
    , mApplication(context)
    , mBucketId(bucketId)
    , mCachedCount(INVALID_COUNT)
    , mIsImage(isImage)
{
    mApplication->GetContentResolver((IContentResolver**)&mResolver);
    Init(name);
}

LocalAlbum::LocalAlbum(
    /* [in] */ Path* path,
    /* [in] */ IContext* context,
    /* [in] */ Int32 bucketId,
    /* [in] */ Boolean isImage)
    : MediaSet(path, NextVersionNumber())
    , mApplication(context)
    , mBucketId(bucketId)
    , mCachedCount(INVALID_COUNT)
    , mIsImage(isImage)
{
    mApplication->GetContentResolver((IContentResolver**)&mResolver);
    String name = BucketHelper::GetBucketName(mResolver, bucketId);
    Init(name);
}

AutoPtr<IUri> LocalAlbum::GetContentUri()
{
    return NULL;
}

AutoPtr< ArrayOf<MediaItem*> > LocalAlbum::GetMediaItem(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    return NULL;
}

Int32 LocalAlbum::GetMediaItemCount()
{
    return 0;
}

String LocalAlbum::GetName()
{
    return String(NULL);
}

Int64 LocalAlbum::Reload()
{
    return 0;
}

Int32 LocalAlbum::GetSupportedOperations()
{
    return 0;
}

ECode LocalAlbum::Delete()
{
    return 0;
}

Boolean LocalAlbum::IsLeafAlbum()
{
    return TRUE;
}

AutoPtr<MediaItem> LocalAlbum::LoadOrUpdateItem(
    /* [in] */ Path* path,
    /* [in] */ ICursor* cursor,
    /* [in] */ DataManager* dataManager,
    /* [in] */ IContext* context,
    /* [in] */ Boolean isImage)
{
    return NULL;
}

Boolean LocalAlbum::ArrayInit()
{
    COUNT_PROJECTION = ArrayOf<String>::Alloc(1);
    (*COUNT_PROJECTION)[0] = String("count(*)");
    return TRUE;
}

void LocalAlbum::Init(
    /* [in] */ const String& name)
{
    Logger::D(TAG, "Init()-----");
    AutoPtr<IResources> res;
    mApplication->GetResources((IResources**)&res);
    mName = GetLocalizedName(res, mBucketId, name);
    if (mIsImage) {
        mWhereClause = String("bucket_id = ?"); // ImageColumns.BUCKET_ID + " = ?";
        mOrderClause = String("datetaken DESC, _id DESC"); // ImageColumns.DATE_TAKEN + " DESC, " + ImageColumns._ID + " DESC";
        // mBaseUri = Images.Media.EXTERNAL_CONTENT_URI;
        mProjection = LocalImage::PROJECTION;
        mItemPath = Path::ITEM_PATH;
    }
    else {
        // mWhereClause = VideoColumns.BUCKET_ID + " = ?";
        // mOrderClause = VideoColumns.DATE_TAKEN + " DESC, " + VideoColumns._ID + " DESC";
        // mBaseUri = Video.Media.EXTERNAL_CONTENT_URI;
        // mProjection = LocalVideo.PROJECTION;
        // mItemPath = LocalVideo.ITEM_PATH;
    }

    // mNotifier = new ChangeNotifier(this, mBaseUri, application);
}

} // namespace Gallery
