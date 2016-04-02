#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_CMEDIAPROJECTIONPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_CMEDIAPROJECTIONPERMISSIONACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Media_CMediaProjectionPermissionActivity.h"
#include "elastos/droid/systemui/media/MediaProjectionPermissionActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

CarClass(CMediaProjectionPermissionActivity)
    , public MediaProjectionPermissionActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_CMEDIAPROJECTIONPERMISSIONACTIVITY_H__
