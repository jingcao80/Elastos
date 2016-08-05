
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CAPPLICATIONSSTATEPACKAGESTATSOBSERVER_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CAPPLICATIONSSTATEPACKAGESTATSOBSERVER_H__

#include "_Elastos_Droid_Settings_Applications_CApplicationsStatePackageStatsObserver.h"
#include "elastos/droid/settings/applications/ApplicationsState.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CApplicationsStatePackageStatsObserver)
    , public ApplicationsState::ApplicationsStatePackageStatsObserver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CAPPLICATIONSSTATEPACKAGESTATSOBSERVER_H__
