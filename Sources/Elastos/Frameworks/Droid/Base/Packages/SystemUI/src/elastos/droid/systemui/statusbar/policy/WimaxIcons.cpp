#include "elastos/droid/systemui/statusbar/policy/WimaxIcons.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


const Int32 WimaxIcons::WIMAX_SIGNAL_STRENGTH[2][5] = {  // TELEPHONY_SIGNAL_STRENGTH
    {
        SystemUIR::drawable::stat_sys_signal_0,
        SystemUIR::drawable::stat_sys_signal_1,
        SystemUIR::drawable::stat_sys_signal_2,
        SystemUIR::drawable::stat_sys_signal_3,
        SystemUIR::drawable::stat_sys_signal_4
    },

    {
        SystemUIR::drawable::stat_sys_signal_0_fully,
        SystemUIR::drawable::stat_sys_signal_1_fully,
        SystemUIR::drawable::stat_sys_signal_2_fully,
        SystemUIR::drawable::stat_sys_signal_3_fully,
        SystemUIR::drawable::stat_sys_signal_4_fully
    }
};

const Int32 WimaxIcons::WIMAX_DISCONNECTED = SystemUIR::drawable::stat_sys_signal_0;

const Int32 WimaxIcons::WIMAX_IDLE = SystemUIR::drawable::stat_sys_signal_0;


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
