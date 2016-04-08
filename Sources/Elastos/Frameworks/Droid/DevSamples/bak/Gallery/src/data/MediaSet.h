
#ifndef __MEDIASET_H__
#define __MEDIASET_H__

#include <elautoptr.h>
#include <cmdef.h>
#include "elastos/droid/app/Activity.h"
#include "MediaObject.h"
#include "MediaItem.h"
#include "Path.h"

namespace Gallery {

class MediaSet
    : public MediaObject
{
public:
    class ItemConsumer
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        virtual CARAPI_(void) Consume(
            /* [in] */ Int32 index,
            /* [in] */ MediaItem* item) = 0;
    };

public:
    MediaSet(
        /* [in] */ Path* path,
        /* [in] */ Int64 version);

    virtual CARAPI_(Int32) GetMediaItemCount();

    virtual CARAPI_(AutoPtr< ArrayOf<MediaItem*> >) GetMediaItem(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    virtual CARAPI_(AutoPtr<MediaItem>) GetCoverMediaItem();

    virtual CARAPI_(Int32) GetSubMediaSetCount();

    virtual CARAPI_(AutoPtr<MediaSet>) GetSubMediaSet(
        /* [in] */ Int32 index);

    virtual CARAPI_(Boolean) IsLeafAlbum();

    virtual CARAPI_(Boolean) IsLoading();

    virtual CARAPI_(Int32) GetTotalMediaItemCount();

    virtual CARAPI_(Int32) GetIndexOfItem(
        /* [in] */ Path* path,
        /* [in] */ Int32 hint);

    virtual CARAPI_(String) GetName() = 0;

    // void addContentListener(ContentListener listener);
    // void removeContentListener(ContentListener listener);
    // void notifyContentChanged();

    virtual CARAPI_(Int64) Reload() = 0;

    virtual CARAPI_(AutoPtr<MediaDetails>) GetDetails();

    // enumerateMediaItems(ItemConsumer consumer);
    // enumerateTotalMediaItems(ItemConsumer consumer);
    // Future<Integer> requestSync(SyncListener listener);

protected:
    CARAPI_(Int32) GetIndexOf(
        /* [in] */ Path* path,
        /* [in] */ ArrayOf<MediaItem*>* list);

    // int enumerateMediaItems(ItemConsumer consumer, int startIndex);
    // int enumerateTotalMediaItems(ItemConsumer consumer, int startIndex);
    // Future<Integer> requestSyncOnMultipleSets(MediaSet[] sets, SyncListener listener);

public:
    static const Int32 MEDIAITEM_BATCH_FETCH_COUNT;
    static const Int32 INDEX_NOT_FOUND;
    static const Int32 SYNC_RESULT_SUCCESS;
    static const Int32 SYNC_RESULT_CANCELLED;
    static const Int32 SYNC_RESULT_ERROR;

private:
    static const String TAG;

};

} // namespace Gallery

#endif // __MEDIASET_H__
