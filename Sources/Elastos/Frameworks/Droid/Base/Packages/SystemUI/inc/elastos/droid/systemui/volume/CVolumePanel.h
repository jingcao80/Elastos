#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_H__

#include "_Elastos_Droid_SystemUI_Volume_CVolumePanel.h"
#include "elastos/droid/systemui/volume/VolumePanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CVolumePanel)
    , public VolumePanel
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CVolumePanel")
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CVOLUMEPANEL_H__
