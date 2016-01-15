#ifndef __ELASTOS_DROID_MEDIA_CSUBTITLEDATA_H__
#define __ELASTOS_DROID_MEDIA_CSUBTITLEDATA_H__

#include "_Elastos_Droid_Media_CSubtitleData.h"
#include "elastos/droid/media/SubtitleData.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CSubtitleData)
    , public SubtitleData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CSUBTITLEDATA_H__