#include "MediaSource.h"
#include "DataManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String MediaSource::TAG("MediaSource");

MediaSource::PathId::PathId(
    /* [in] */ Path* path,
    /* [in] */ Int32 id)
    : path(path)
    , id(id)
{}

CAR_INTERFACE_IMPL(MediaSource::PathId, IInterface)

MediaSource::MediaSource(
    /* [in] */ const String& prefix)
    : mPrefix(prefix)
{}

CAR_INTERFACE_IMPL(MediaSource, IInterface)

String MediaSource::GetPrefix()
{
    return mPrefix;
}

AutoPtr<Path> MediaSource::FindPathByUri(
    /* [in] */ IUri* uri,
    /* [in] */ const String& type)
{
    return NULL;
}

void MediaSource::Resume()
{}

void MediaSource::Pause()
{}

AutoPtr<Path> MediaSource::GetDefaultSetOf(
    /* [in] */ Path* item)
{
    return NULL;
}

Int64 MediaSource::GetTotalUsedCacheSize()
{
    return 0;
}

Int64 MediaSource::GetTotalTargetCacheSize()
{
    return 0;
}

void MediaSource::MapMediaItems(
    /* [in] */ ArrayOf<PathId*>* list,
    /* [in] */ MediaSet::ItemConsumer* consumer)
{
    Int32 n = list->GetLength();
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<PathId> pid = (*list)[i];
        AutoPtr<MediaObject> obj;
        {
            AutoLock lock(DataManager::sLOCK);
            obj = pid->path->GetObject();
            if (obj == NULL) {
                obj = CreateMediaObject(pid->path);
            }
        }
        if (obj != NULL) {
            consumer->Consume(pid->id, (MediaItem*) obj.Get());
        }
    }
}

} // namespace Gallery
