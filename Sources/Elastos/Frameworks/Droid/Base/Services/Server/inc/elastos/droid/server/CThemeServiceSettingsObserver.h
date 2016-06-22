

#ifndef __ELASTOS_DROID_SERVER_CTHEMESERVICE_SETTINGS_OBSERVER_H__
#define __ELASTOS_DROID_SERVER_CTHEMESERVICE_SETTINGS_OBSERVER_H__

#include "_Elastos_Droid_Server_CThemeServiceSettingsObserver.h"
#include <elastos/droid/server/CThemeService.h>

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CThemeServiceSettingsObserver)
    , public CThemeService::SettingsObserver
{
public:
    CAR_OBJECT_DECL()
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_CTHEMESERVICE_SETTINGS_OBSERVER_H__