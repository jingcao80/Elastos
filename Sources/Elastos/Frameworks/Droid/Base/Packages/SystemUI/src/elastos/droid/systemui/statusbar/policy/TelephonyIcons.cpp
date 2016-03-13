
#include "elastos/droid/systemui/statusbar/policy/TelephonyIcons.h"
#include "../../R.h"

using Elastos::Droid::SystemUI::R;

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

const Int32 TelephonyIcons::QS_TELEPHONY_SIGNAL_STRENGTH[2][5] = {
    {
        R::drawable::ic_qs_signal_0,
        R::drawable::ic_qs_signal_1,
        R::drawable::ic_qs_signal_2,
        R::drawable::ic_qs_signal_3,
        R::drawable::ic_qs_signal_4
    },

    {
        R::drawable::ic_qs_signal_full_0,
        R::drawable::ic_qs_signal_full_1,
        R::drawable::ic_qs_signal_full_2,
        R::drawable::ic_qs_signal_full_3,
        R::drawable::ic_qs_signal_full_4
    }
};

const Int32 TelephonyIcons::TELEPHONY_SIGNAL_STRENGTH_ROAMING[2][5] = {
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

const Int32 TelephonyIcons::QS_DATA_R[2] = {
    R::drawable::ic_qs_signal_r,
    R::drawable::ic_qs_signal_r
};

const Int32 TelephonyIcons::DATA_SIGNAL_STRENGTH[2][5] = {  // TELEPHONY_SIGNAL_STRENGTH
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

//***** Data connection icons

//GSM/UMTS
//
const Int32 TelephonyIcons::DATA_G[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g
    },
    {
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g,
        R::drawable::stat_sys_data_fully_connected_g
    }
};

const Int32 TelephonyIcons::QS_DATA_G[2] = {
    R::drawable::ic_qs_signal_g,
    R::drawable::ic_qs_signal_g
};

const Int32 TelephonyIcons::DATA_3G[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g
    },
    {
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g,
        R::drawable::stat_sys_data_fully_connected_3g
    }
};

const Int32 TelephonyIcons::QS_DATA_3G[2] = {
    R::drawable::ic_qs_signal_3g,
    R::drawable::ic_qs_signal_3g
};

const Int32 TelephonyIcons::DATA_E[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e
    },
    {
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e,
        R::drawable::stat_sys_data_fully_connected_e
    }
};

const Int32 TelephonyIcons::QS_DATA_E[2] = {
    R::drawable::ic_qs_signal_e,
    R::drawable::ic_qs_signal_e
};

//3.5G
const Int32 TelephonyIcons::DATA_H[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h
    },
    {
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h,
        R::drawable::stat_sys_data_fully_connected_h
    }
};

const Int32 TelephonyIcons::QS_DATA_H[2] = {
        R::drawable::ic_qs_signal_h,
        R::drawable::ic_qs_signal_h
};

//CDMA
// Use 3G icons for EVDO data and 1x icons for 1XRTT data
const Int32 TelephonyIcons::DATA_1X[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x
    },
    {
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x,
        R::drawable::stat_sys_data_fully_connected_1x
    }
};

const Int32 TelephonyIcons::QS_DATA_1X[2] = {
    R::drawable::ic_qs_signal_1x,
    R::drawable::ic_qs_signal_1x
};

// LTE and eHRPD
const Int32 TelephonyIcons::DATA_4G[2][4] = {
    {
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g
    },
    {
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g,
        R::drawable::stat_sys_data_fully_connected_4g
    }
};

const Int32 TelephonyIcons::QS_DATA_4G[2] = {
    R::drawable::ic_qs_signal_4g,
    R::drawable::ic_qs_signal_4g
};

// LTE branded "LTE"
const Int32 TelephonyIcons::DATA_LTE[2][4] = {
        {
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte
        },
        {
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte,
            R::drawable::stat_sys_data_fully_connected_lte
        }
};

const Int32 TelephonyIcons::QS_DATA_LTE[2] = {
    R::drawable::ic_qs_signal_lte,
    R::drawable::ic_qs_signal_lte
};

const Int32 TelephonyIcons::FLIGHT_MODE_ICON = R::drawable::stat_sys_airplane_mode;
const Int32 TelephonyIcons::ROAMING_ICON = R::drawable::stat_sys_data_fully_connected_roam;
const Int32 TelephonyIcons::ICON_LTE = R::drawable::stat_sys_data_fully_connected_lte;
const Int32 TelephonyIcons::ICON_3G = R::drawable::stat_sys_data_fully_connected_3g;
const Int32 TelephonyIcons::ICON_4G = R::drawable::stat_sys_data_fully_connected_4g;
const Int32 TelephonyIcons::ICON_1X = R::drawable::stat_sys_data_fully_connected_1x;

const Int32 TelephonyIcons::QS_ICON_LTE = R::drawable::ic_qs_signal_lte;
const Int32 TelephonyIcons::QS_ICON_3G = R::drawable::ic_qs_signal_3g;
const Int32 TelephonyIcons::QS_ICON_4G = R::drawable::ic_qs_signal_4g;
const Int32 TelephonyIcons::QS_ICON_1X = R::drawable::ic_qs_signal_1x;

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
