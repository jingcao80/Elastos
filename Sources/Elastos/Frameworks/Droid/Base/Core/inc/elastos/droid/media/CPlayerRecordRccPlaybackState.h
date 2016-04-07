#ifndef __ELASTOS_DROID_MEDIA_CPLAYERRECORDRCCPLAYBACKSTATE_H__
#define __ELASTOS_DROID_MEDIA_CPLAYERRECORDRCCPLAYBACKSTATE_H__

#include "_Elastos_Droid_Media_CPlayerRecordRccPlaybackState.h"
#include "elastos/droid/media/PlayerRecord.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CPlayerRecordRccPlaybackState)
    , public PlayerRecord::RccPlaybackState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CPLAYERRECORDRCCPLAYBACKSTATE_H__
