
#ifndef __ELASTOS_DROID_MEDIA_CREMOTECONTROLLER_H__
#define __ELASTOS_DROID_MEDIA_CREMOTECONTROLLER_H__

#include "_Elastos_Droid_Media_CRemoteController.h"
#include "elastos/droid/media/RemoteController.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRemoteController)
    , public RemoteController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CREMOTECONTROLLER_H__
