#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CREMOTEVOLUMECONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CREMOTEVOLUMECONTROLLER_H__

#include "_Elastos_Droid_SystemUI_Volume_CRemoteVolumeController.h"
#include "elastos/droid/systemui/volume/VolumeUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CRemoteVolumeController)
    , public VolumeUI::RemoteVolumeController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CREMOTEVOLUMECONTROLLER_H__
