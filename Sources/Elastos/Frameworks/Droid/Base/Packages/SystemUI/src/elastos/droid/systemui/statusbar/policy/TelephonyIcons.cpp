#include "elastos/droid/systemui/statusbar/policy/TelephonyIcons.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

//***** Signal strength icons

//GSM/UMTS
//
const Int32 TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH[2][5] = {
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

const Int32 TelephonyIcons::QS_TELEPHONY_SIGNAL_STRENGTH[2][5] = {
    {
        SystemUIR::drawable::ic_qs_signal_0,
        SystemUIR::drawable::ic_qs_signal_1,
        SystemUIR::drawable::ic_qs_signal_2,
        SystemUIR::drawable::ic_qs_signal_3,
        SystemUIR::drawable::ic_qs_signal_4
    },

    {
        SystemUIR::drawable::ic_qs_signal_full_0,
        SystemUIR::drawable::ic_qs_signal_full_1,
        SystemUIR::drawable::ic_qs_signal_full_2,
        SystemUIR::drawable::ic_qs_signal_full_3,
        SystemUIR::drawable::ic_qs_signal_full_4
    }
};

const Int32 TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH_ROAMING[2][5] = {
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

const Int32 TelephonyIcons::DATA_SIGNAL_STRENGTH[2][5] = {  // TELEPHONY_SIGNAL_STRENGTH
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

//***** Data connection icons

//GSM/UMTS
//
const Int32 TelephonyIcons::DATA_G[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_g,
        SystemUIR::drawable::stat_sys_data_connected_g,
        SystemUIR::drawable::stat_sys_data_connected_g,
        SystemUIR::drawable::stat_sys_data_connected_g
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_g,
        SystemUIR::drawable::stat_sys_data_fully_connected_g,
        SystemUIR::drawable::stat_sys_data_fully_connected_g,
        SystemUIR::drawable::stat_sys_data_fully_connected_g
    }
};

const Int32 TelephonyIcons::DATA_3G[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_3g,
        SystemUIR::drawable::stat_sys_data_connected_3g,
        SystemUIR::drawable::stat_sys_data_connected_3g,
        SystemUIR::drawable::stat_sys_data_connected_3g
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_3g,
        SystemUIR::drawable::stat_sys_data_fully_connected_3g,
        SystemUIR::drawable::stat_sys_data_fully_connected_3g,
        SystemUIR::drawable::stat_sys_data_fully_connected_3g
    }
};

const Int32 TelephonyIcons::DATA_E[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_e,
        SystemUIR::drawable::stat_sys_data_connected_e,
        SystemUIR::drawable::stat_sys_data_connected_e,
        SystemUIR::drawable::stat_sys_data_connected_e
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_e,
        SystemUIR::drawable::stat_sys_data_fully_connected_e,
        SystemUIR::drawable::stat_sys_data_fully_connected_e,
        SystemUIR::drawable::stat_sys_data_fully_connected_e
    }
};

//3.5G
const Int32 TelephonyIcons::DATA_H[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_h,
        SystemUIR::drawable::stat_sys_data_connected_h,
        SystemUIR::drawable::stat_sys_data_connected_h,
        SystemUIR::drawable::stat_sys_data_connected_h
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_h,
        SystemUIR::drawable::stat_sys_data_fully_connected_h,
        SystemUIR::drawable::stat_sys_data_fully_connected_h,
        SystemUIR::drawable::stat_sys_data_fully_connected_h
    }
};

//CDMA
// Use 3G icons for EVDO data and 1x icons for 1XRTT data
const Int32 TelephonyIcons::DATA_1X[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_1x,
        SystemUIR::drawable::stat_sys_data_connected_1x,
        SystemUIR::drawable::stat_sys_data_connected_1x,
        SystemUIR::drawable::stat_sys_data_connected_1x
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_1x,
        SystemUIR::drawable::stat_sys_data_fully_connected_1x,
        SystemUIR::drawable::stat_sys_data_fully_connected_1x,
        SystemUIR::drawable::stat_sys_data_fully_connected_1x
    }
};

// LTE and eHRPD
const Int32 TelephonyIcons::DATA_4G[2][4] = {
    {
        SystemUIR::drawable::stat_sys_data_connected_4g,
        SystemUIR::drawable::stat_sys_data_connected_4g,
        SystemUIR::drawable::stat_sys_data_connected_4g,
        SystemUIR::drawable::stat_sys_data_connected_4g
    },
    {
        SystemUIR::drawable::stat_sys_data_fully_connected_4g,
        SystemUIR::drawable::stat_sys_data_fully_connected_4g,
        SystemUIR::drawable::stat_sys_data_fully_connected_4g,
        SystemUIR::drawable::stat_sys_data_fully_connected_4g
    }
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
