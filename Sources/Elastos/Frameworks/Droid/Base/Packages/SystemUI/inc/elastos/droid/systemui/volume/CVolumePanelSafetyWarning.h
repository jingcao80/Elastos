#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_SAFETY_WANING_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_SAFETY_WANING_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumePanelSafetyWarning.h"
#include "elastos/droid/systemui/volume/VolumePanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CVolumePanelSafetyWarning)
    , public VolumePanel::SafetyWarning
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_SAFETY_WANING_H__
