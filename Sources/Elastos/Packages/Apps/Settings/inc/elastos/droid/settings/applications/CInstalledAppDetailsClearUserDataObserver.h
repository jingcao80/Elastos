
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARUSERDATAOBSERVER_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARUSERDATAOBSERVER_H__

#include "_Elastos_Droid_Settings_Applications_CInstalledAppDetailsClearUserDataObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CInstalledAppDetailsClearUserDataObserver)
    , public CInstalledAppDetails::ClearUserDataObserver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARUSERDATAOBSERVER_H__
