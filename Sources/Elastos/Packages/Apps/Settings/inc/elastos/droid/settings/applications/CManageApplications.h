
#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CMANAGEAPPLICATIONS_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CMANAGEAPPLICATIONS_H__

#include "_Elastos_Droid_Settings_Applications_CManageApplications.h"
#include "elastos/droid/settings/applications/ManageApplications.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CManageApplications)
    , public ManageApplications
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CMANAGEAPPLICATIONS_H__