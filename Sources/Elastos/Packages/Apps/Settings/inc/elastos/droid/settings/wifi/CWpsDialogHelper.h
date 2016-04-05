
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOGHELPER_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOGHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Settings_Wifi_CWpsDialogHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Settings::Wifi::WpsDialogState;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

CarClass(CWpsDialogHelper)
    , public Singleton
    , public IWpsDialogHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI WpsDialogStateFromString(
        /* [in] */ const String& str,
        /* [out] */ WpsDialogState* state);

    CARAPI WpsDialogStateToString(
        /* [in] */ WpsDialogState state,
        /* [out] */ String* str);

    CARAPI Index(
        /* [in] */ WpsDialogState state,
        /* [out] */ Int32* index);
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOGHELPER_H__
