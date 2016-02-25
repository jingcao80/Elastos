#ifndef __ELASTOS_DROID_MEDIA_CTEXTTRACKCUE_H__
#define __ELASTOS_DROID_MEDIA_CTEXTTRACKCUE_H__

#include "_Elastos_Droid_Media_CTextTrackCue.h"
#include "elastos/droid/media/TextTrackCue.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CTextTrackCue)
    , public TextTrackCue
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTEXTTRACKCUE_H__