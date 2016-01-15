#include "elastos/droid/media/CMediaSyncEventHelper.h"
#include "elastos/droid/media/MediaSyncEvent.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaSyncEventHelper)

CAR_INTERFACE_IMPL(CMediaSyncEventHelper, Singleton, IMediaSyncEventHelper)

ECode CMediaSyncEventHelper::CreateEvent(
    /* [in] */ Int32 eventType,
    /* [out] */ IMediaSyncEvent** result)
{
    return MediaSyncEvent::CreateEvent(eventType, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
