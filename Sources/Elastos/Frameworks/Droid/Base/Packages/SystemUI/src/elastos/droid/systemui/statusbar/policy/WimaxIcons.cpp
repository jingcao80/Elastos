#include "elastos/droid/systemui/statusbar/policy/WimaxIcons.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Int32 WimaxIcons::WIMAX_SIGNAL_STRENGTH[2][5] = {  // TELEPHONY_SIGNAL_STRENGTH
    {
        R::drawable::stat_sys_signal_0,
        R::drawable::stat_sys_signal_1,
        R::drawable::stat_sys_signal_2,
        R::drawable::stat_sys_signal_3,
        R::drawable::stat_sys_signal_4
    },

    {
        R::drawable::stat_sys_signal_0_fully,
        R::drawable::stat_sys_signal_1_fully,
        R::drawable::stat_sys_signal_2_fully,
        R::drawable::stat_sys_signal_3_fully,
        R::drawable::stat_sys_signal_4_fully
    }
};

const Int32 WimaxIcons::WIMAX_DISCONNECTED = R::drawable::stat_sys_signal_0;

const Int32 WimaxIcons::WIMAX_IDLE = R::drawable::stat_sys_signal_0;

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
