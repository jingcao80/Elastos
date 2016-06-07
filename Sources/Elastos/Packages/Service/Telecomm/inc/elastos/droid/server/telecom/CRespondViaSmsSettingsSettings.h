
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CRESPONDVIASMSSETTINGSSETTINGS_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CRESPONDVIASMSSETTINGSSETTINGS_H__

#include "_Elastos_Droid_Server_Telecom_CRespondViaSmsSettingsSettings.h"
#include "elastos/droid/server/telecom/RespondViaSmsSettings.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CRespondViaSmsSettingsSettings)
    , public RespondViaSmsSettings::Settings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CRESPONDVIASMSSETTINGSSETTINGS_H__
