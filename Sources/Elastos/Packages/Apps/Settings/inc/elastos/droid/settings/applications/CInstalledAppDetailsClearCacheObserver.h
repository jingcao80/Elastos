
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARCACHEOBSERVER_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARCACHEOBSERVER_H__

#include "_Elastos_Droid_Settings_Applications_CInstalledAppDetailsClearCacheObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CInstalledAppDetailsClearCacheObserver)
    , public CInstalledAppDetails::ClearCacheObserver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILSCLEARCACHEOBSERVER_H__
