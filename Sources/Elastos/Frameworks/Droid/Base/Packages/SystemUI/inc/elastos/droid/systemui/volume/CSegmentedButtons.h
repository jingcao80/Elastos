#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_CSEGMENTEDBUTTONS_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_CSEGMENTEDBUTTONS_H__

#include "_Elastos_Droid_SystemUI_Volume_CSegmentedButtons.h"
#include "elastos/droid/systemui/volume/SegmentedButtons.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CarClass(CSegmentedButtons)
    , public SegmentedButtons
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_VOLUME_CSEGMENTEDBUTTONS_H__
