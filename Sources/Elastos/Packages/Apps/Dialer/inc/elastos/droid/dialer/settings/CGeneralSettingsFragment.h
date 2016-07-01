#ifndef __ELASTOS_DROID_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__

#include "_Elastos_Droid_Dialer_Settings_CGeneralSettingsFragment.h"
#include "elastos/apps/dialer/settings/GeneralSettingsFragment.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

CarClass(CGeneralSettingsFragment)
    , public GeneralSettingsFragment
{
public:
    CAR_OBJECT_DECL();
};

} // Settings
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_CGENERALSETTINGSFRAGMENT_H__
