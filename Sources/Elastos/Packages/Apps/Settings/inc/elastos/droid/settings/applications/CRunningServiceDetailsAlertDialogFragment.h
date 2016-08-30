
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILSALERTDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILSALERTDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Settings_Applications_CRunningServiceDetailsAlertDialogFragment.h"
#include "elastos/droid/settings/applications/CRunningServiceDetails.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CRunningServiceDetailsAlertDialogFragment)
    , public CRunningServiceDetails::MyAlertDialogFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILSALERTDIALOGFRAGMENT_H__
