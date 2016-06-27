
#ifndef __ELASTOS_DROID_SETTINGS_CZONEPICKER_H__
#define __ELASTOS_DROID_SETTINGS_CZONEPICKER_H__

#include "_Elastos_Droid_Settings_CZonePicker.h"
#include "elastos/droid/settings/ZonePicker.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CZonePicker)
    , public ZonePicker
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CZONEPICKER_H__
