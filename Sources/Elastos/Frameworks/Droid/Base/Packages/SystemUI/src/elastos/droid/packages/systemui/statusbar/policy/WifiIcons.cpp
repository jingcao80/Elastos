#include "elastos/droid/systemui/statusbar/policy/WifiIcons.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


const Int32 WifiIcons::WIFI_SIGNAL_STRENGTH[2][5] = {
    {
        SystemUIR::drawable::stat_sys_wifi_signal_0,
        SystemUIR::drawable::stat_sys_wifi_signal_1,
        SystemUIR::drawable::stat_sys_wifi_signal_2,
        SystemUIR::drawable::stat_sys_wifi_signal_3,
        SystemUIR::drawable::stat_sys_wifi_signal_4
    },

    {
        SystemUIR::drawable::stat_sys_wifi_signal_0,
        SystemUIR::drawable::stat_sys_wifi_signal_1_fully,
        SystemUIR::drawable::stat_sys_wifi_signal_2_fully,
        SystemUIR::drawable::stat_sys_wifi_signal_3_fully,
        SystemUIR::drawable::stat_sys_wifi_signal_4_fully
    }
};

const Int32 WifiIcons::QS_WIFI_SIGNAL_STRENGTH[2][5] = {
    {
        SystemUIR::drawable::ic_qs_wifi_0,
        SystemUIR::drawable::ic_qs_wifi_1,
        SystemUIR::drawable::ic_qs_wifi_2,
        SystemUIR::drawable::ic_qs_wifi_3,
        SystemUIR::drawable::ic_qs_wifi_4
    },

    {
        SystemUIR::drawable::ic_qs_wifi_0,
        SystemUIR::drawable::ic_qs_wifi_full_1,
        SystemUIR::drawable::ic_qs_wifi_full_2,
        SystemUIR::drawable::ic_qs_wifi_full_3,
        SystemUIR::drawable::ic_qs_wifi_full_4
    }
};

const Int32 WifiIcons::WIFI_LEVEL_COUNT = 5;


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
