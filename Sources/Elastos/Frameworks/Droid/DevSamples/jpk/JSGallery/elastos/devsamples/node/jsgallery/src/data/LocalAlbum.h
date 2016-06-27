
#ifndef __LOCALALBUM_H__
#define __LOCALALBUM_H__

#include "app/Activity.h"
#include "MediaSet.h"
#include "MediaItem.h"
#include "Path.h"
#include "DataManager.h"

namespace Gallery {

class LocalAlbum
    : public MediaSet
{
public:
    static CARAPI_(AutoPtr< ArrayOf<MediaItem*> >) GetMediaItemById(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isImage,
        /* [in] */ ArrayOf<Int32>* ids);

    static CARAPI_(AutoPtr<ICursor>) GetItemCursor(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ Int32 id);

    static CARAPI_(String) GetLocalizedName(
        /* [in] */ IResources* res,
        /* [in] */ Int32 bucketId,
        /* [in] */ const String& name);

public:
    LocalAlbum(
        /* [in] */ Path* path,
        /* [in] */ IContext* context,
        /* [in] */ Int32 bucketId,
        /* [in] */ Boolean isImage,
        /* [in] */ const String& name);

    LocalAlbum(
        /* [in] */ Path* path,
        /* [in] */ IContext* context,
        /* [in] */ Int32 bucketId,
        /* [in] */ Boolean isImage);

    virtual CARAPI_(AutoPtr<IUri>) GetContentUri();

    virtual CARAPI_(AutoPtr< ArrayOf<MediaItem*> >) GetMediaItem(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    virtual CARAPI_(Int32) GetMediaItemCount();

    virtual CARAPI_(String) GetName();

    virtual CARAPI_(Int64) Reload();

    virtual CARAPI_(Int32) GetSupportedOperations();

    virtual CARAPI Delete();

    virtual CARAPI_(Boolean) IsLeafAlbum();

private:
    static CARAPI_(AutoPtr<MediaItem>) LoadOrUpdateItem(
        /* [in] */ Path* path,
        /* [in] */ ICursor* cursor,
        /* [in] */ DataManager* dataManager,
        /* [in] */ IContext* context,
        /* [in] */ Boolean isImage);

    static CARAPI_(Boolean) ArrayInit();

    CARAPI_(void) Init(
        /* [in] */ const String& name);

private:
    static const String TAG;
    static const Int32 INVALID_COUNT;
    static AutoPtr< ArrayOf<String> > COUNT_PROJECTION;
    static Boolean sIsInit;

    AutoPtr<IContext> mApplication;
    Int32 mBucketId;
    Int32 mCachedCount;
    Boolean mIsImage;
    String mWhereClause;
    String mOrderClause;
    String mName;
    AutoPtr<IUri> mBaseUri;
    AutoPtr< ArrayOf<String> > mProjection;
    AutoPtr<IContentResolver> mResolver;
    // ChangeNotifier mNotifier;
    AutoPtr<Path> mItemPath;

};

} // namespace Gallery

#endif // __LOCALALBUM_H__
