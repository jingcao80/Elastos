#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CZENMODEPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CZENMODEPANEL_H__

#include "_Elastos_Droid_SystemUI_Volume_CZenModePanel.h"
#include "elastos/droid/systemui/volume/ZenModePanel.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CZenModePanel)
    , public ZenModePanel
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CZENMODEPANEL_H__
