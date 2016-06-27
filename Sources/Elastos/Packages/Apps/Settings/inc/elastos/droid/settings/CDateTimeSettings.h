
#ifndef __ELASTOS_DROID_SETTINGS_CDATETIMESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDATETIMESETTINGS_H__

#include "_Elastos_Droid_Settings_CDateTimeSettings.h"
#include "elastos/droid/settings/DateTimeSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDateTimeSettings)
    , public DateTimeSettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDATETIMESETTINGS_H__
