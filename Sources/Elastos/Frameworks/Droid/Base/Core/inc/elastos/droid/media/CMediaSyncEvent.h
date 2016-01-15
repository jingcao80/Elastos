#ifndef __ELASTOS_DROID_MEDIA_CMEDIASYNCEVENT_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASYNCEVENT_H__

#include "_Elastos_Droid_Media_CMediaSyncEvent.h"
#include "elastos/droid/media/MediaSyncEvent.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaSyncEvent)
    , public MediaSyncEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASYNCEVENT_H__