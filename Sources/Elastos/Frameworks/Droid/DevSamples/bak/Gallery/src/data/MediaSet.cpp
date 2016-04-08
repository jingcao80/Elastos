#include "MediaSet.h"
#include "MediaDetails.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String MediaSet::TAG("MediaSet");
const Int32 MediaSet::MEDIAITEM_BATCH_FETCH_COUNT = 500;
const Int32 MediaSet::INDEX_NOT_FOUND = -1;
const Int32 MediaSet::SYNC_RESULT_SUCCESS = 0;
const Int32 MediaSet::SYNC_RESULT_CANCELLED = 1;
const Int32 MediaSet::SYNC_RESULT_ERROR = 2;

CAR_INTERFACE_IMPL(MediaSet::ItemConsumer, IInterface)

MediaSet::MediaSet(
    /* [in] */ Path* path,
    /* [in] */ Int64 version)
    : MediaObject(path, version)
{}

Int32 MediaSet::GetMediaItemCount()
{
    return 0;
}

AutoPtr< ArrayOf<MediaItem*> > MediaSet::GetMediaItem(
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    return ArrayOf<MediaItem*>::Alloc(0);
}

AutoPtr<MediaItem> MediaSet::GetCoverMediaItem()
{
    AutoPtr< ArrayOf<MediaItem*> > items = GetMediaItem(0, 1);
    if (items->GetLength() > 0) return (*items)[0];
    for (Int32 i = 0, n = GetSubMediaSetCount(); i < n; i++) {
        AutoPtr<MediaItem> cover = GetSubMediaSet(i)->GetCoverMediaItem();
        if (cover != NULL) return cover;
    }
    return NULL;
}

Int32 MediaSet::GetSubMediaSetCount()
{
    return 0;
}

AutoPtr<MediaSet> MediaSet::GetSubMediaSet(
    /* [in] */ Int32 index)
{
    // throw new IndexOutOfBoundsException();
    return NULL;
}

Boolean MediaSet::IsLeafAlbum()
{
    return FALSE;
}

Boolean MediaSet::IsLoading()
{
    return FALSE;
}

Int32 MediaSet::GetTotalMediaItemCount()
{
    Int32 total = GetMediaItemCount();
    for (Int32 i = 0, n = GetSubMediaSetCount(); i < n; i++) {
        total += GetSubMediaSet(i)->GetTotalMediaItemCount();
    }
    return total;
}

Int32 MediaSet::GetIndexOfItem(
    /* [in] */ Path* path,
    /* [in] */ Int32 hint)
{
    // hint < 0 is handled below
    // first, try to find it around the hint
    Int32 start = Elastos::Core::Math::Max(0, hint - MEDIAITEM_BATCH_FETCH_COUNT / 2);
    AutoPtr< ArrayOf<MediaItem*> > list = GetMediaItem(start, MEDIAITEM_BATCH_FETCH_COUNT);
    Int32 index = GetIndexOf(path, list);
    if (index != INDEX_NOT_FOUND) return start + index;

    // try to find it globally
    start = start == 0 ? MEDIAITEM_BATCH_FETCH_COUNT : 0;
    list = GetMediaItem(start, MEDIAITEM_BATCH_FETCH_COUNT);
    while (true) {
        index = GetIndexOf(path, list);
        if (index != INDEX_NOT_FOUND) return start + index;
        if (list->GetLength() < MEDIAITEM_BATCH_FETCH_COUNT) return INDEX_NOT_FOUND;
        start += MEDIAITEM_BATCH_FETCH_COUNT;
        list = GetMediaItem(start, MEDIAITEM_BATCH_FETCH_COUNT);
    }
}

AutoPtr<MediaDetails> MediaSet::GetDetails()
{
    AutoPtr<MediaDetails> details = MediaObject::GetDetails();
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(GetName(), (ICharSequence**)&cs);
    details->AddDetail(MediaDetails::INDEX_TITLE, cs);
    return details;
}

Int32 MediaSet::GetIndexOf(
    /* [in] */ Path* path,
    /* [in] */ ArrayOf<MediaItem*>* list)
{
    for (Int32 i = 0, n = list->GetLength(); i < n; ++i) {
        // item could be null only in ClusterAlbum
        AutoPtr<MediaObject> item = (*list)[i];
        if (item != NULL && item->mPath.Get() == path) return i;
    }
    return INDEX_NOT_FOUND;
}

} // namespace Gallery
