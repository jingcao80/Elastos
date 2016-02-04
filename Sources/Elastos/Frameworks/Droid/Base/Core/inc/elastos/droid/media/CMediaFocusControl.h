
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAFOCUSCONTROL_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAFOCUSCONTROL_H__

#include "_Elastos_Droid_Media_CMediaFocusControl.h"
#include "elastos/droid/media/MediaFocusControl.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaFocusControl)
    , public MediaFocusControl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAFOCUSCONTROL_H__
