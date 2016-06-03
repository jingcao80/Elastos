
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDSUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDSUMMARY_H__

#include "_Elastos_Droid_Settings_Dashboard_CDashboardSummary.h"
#include "elastos/droid/settings/dashboard/DashboardSummary.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CarClass(CDashboardSummary)
    , public DashboardSummary
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDSUMMARY_H__
