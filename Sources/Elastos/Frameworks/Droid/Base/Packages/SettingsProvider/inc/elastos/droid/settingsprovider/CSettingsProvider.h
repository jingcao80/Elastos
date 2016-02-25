
#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSPROVIDER_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSPROVIDER_H__

#include "_Elastos_Droid_SettingsProvider_CSettingsProvider.h"
#include "elastos/droid/settingsprovider/SettingsProvider.h"

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

CarClass(CSettingsProvider)
    , public SettingsProvider
{
public:
    CAR_OBJECT_DECL();
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSPROVIDER_H__
