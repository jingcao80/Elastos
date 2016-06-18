
#ifndef __ELASTOS_DROID_SYSTEMUI_POWER_CPOWERUI_RECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_POWER_CPOWERUI_RECEIVER_H__

#include "_Elastos_Droid_SystemUI_Power_CPowerUIReceiver.h"
#include "elastos/droid/systemui/power/CPowerUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Power {

CarClass(CPowerUIReceiver)
    , public CPowerUI::Receiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Power
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_POWER_CPOWERUI_RECEIVER_H__