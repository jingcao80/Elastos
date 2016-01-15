#ifndef __ELASTOS_DROID_MEDIA_CSUBTITLECONTROLLER_H__
#define __ELASTOS_DROID_MEDIA_CSUBTITLECONTROLLER_H__

#include "_Elastos_Droid_Media_CSubtitleController.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/SubtitleController.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CSubtitleController)
    , public SubtitleController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CSUBTITLECONTROLLER_H__