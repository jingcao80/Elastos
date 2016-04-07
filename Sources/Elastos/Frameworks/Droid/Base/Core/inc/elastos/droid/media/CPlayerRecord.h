#ifndef __ELASTOS_DROID_MEDIA_CPLAYERRECORD_H__
#define __ELASTOS_DROID_MEDIA_CPLAYERRECORD_H__

#include "_Elastos_Droid_Media_CPlayerRecord.h"
#include "elastos/droid/media/PlayerRecord.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CPlayerRecord)
    , public PlayerRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CPLAYERRECORD_H__
