
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSALERTDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSALERTDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Settings_Applications_CInstalledAppDetailsAlertDialogFragment.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CInstalledAppDetailsAlertDialogFragment)
    , public CInstalledAppDetails::MyAlertDialogFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSALERTDIALOGFRAGMENT_H__
