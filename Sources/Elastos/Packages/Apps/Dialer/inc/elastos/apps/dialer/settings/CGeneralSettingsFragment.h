#ifndef __ELASTOS_APPS_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__

#include "_Elastos_Apps_Dialer_Settings_CGeneralSettingsFragment.h"
#include "GeneralSettingsFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Settings {

CarClass(CGeneralSettingsFragment)
    , public GeneralSettingsFragment
{
public:
    CAR_OBJECT_DECL()
};

} // Settings
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__
