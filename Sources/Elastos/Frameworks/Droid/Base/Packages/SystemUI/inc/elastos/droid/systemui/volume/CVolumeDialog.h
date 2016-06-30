#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEDIALOGL_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEDIALOGL_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumeDialog.h"
#include "elastos/droid/systemui/volume/VolumePanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CVolumeDialog)
    , public VolumePanel::VolumeDialog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEDIALOGL_H__
