
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTILEVIEW_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTILEVIEW_H__

#include "_Elastos_Droid_Settings_Dashboard_CDashboardTileView.h"
#include "elastos/droid/settings/dashboard/DashboardTileView.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CarClass(CDashboardTileView)
    , public DashboardTileView
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDTILEVIEW_H__
