
#ifndef __ELASTOS_DROID_SETTINGS_CUSERSPINNERADAPTER_H__
#define __ELASTOS_DROID_SETTINGS_CUSERSPINNERADAPTER_H__

#include "_Elastos_Droid_Settings_CUserSpinnerAdapter.h"
#include "elastos/droid/settings/UserSpinnerAdapter.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CUserSpinnerAdapter)
    , public UserSpinnerAdapter
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CUSERSPINNERADAPTER_H__
