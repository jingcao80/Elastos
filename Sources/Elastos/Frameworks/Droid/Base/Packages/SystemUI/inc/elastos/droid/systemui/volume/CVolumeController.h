#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMECONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMECONTROLLER_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumeController.h"
#include "elastos/droid/systemui/volume/CVolumeUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CVolumeController)
    , public CVolumeUI::VolumeController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMECONTROLLER_H__
