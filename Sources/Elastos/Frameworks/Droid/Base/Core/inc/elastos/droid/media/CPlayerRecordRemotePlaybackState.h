#ifndef __ELASTOS_DROID_MEDIA_CPLAYERRECORDREMOTEPLAYBACKSTATE_H__
#define __ELASTOS_DROID_MEDIA_CPLAYERRECORDREMOTEPLAYBACKSTATE_H__

#include "_Elastos_Droid_Media_CPlayerRecordRemotePlaybackState.h"
#include "elastos/droid/media/PlayerRecord.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CPlayerRecordRemotePlaybackState)
    , public PlayerRecord::RemotePlaybackState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CPLAYERRECORDREMOTEPLAYBACKSTATE_H__
