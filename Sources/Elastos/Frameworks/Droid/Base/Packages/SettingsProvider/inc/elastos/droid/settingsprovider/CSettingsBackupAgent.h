#ifndef __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSBACKUPAGENT_H__
#define __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSBACKUPAGENT_H__

#include "elastos/droid/settingsprovider/SettingsBackupAgent.h"
#include "_Elastos_Droid_SettingsProvider_CSettingsBackupAgent.h"

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

CarClass(CSettingsBackupAgent)
    , public SettingsBackupAgent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SETTINGSPROVIDER_CSETTINGSBACKUPAGENT_H__
