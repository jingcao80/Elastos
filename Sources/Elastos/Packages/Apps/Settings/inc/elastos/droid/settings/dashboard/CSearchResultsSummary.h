
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CSEARCHRESULTSSUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CSEARCHRESULTSSUMMARY_H__

#include "_Elastos_Droid_Settings_Dashboard_CSearchResultsSummary.h"
#include "elastos/droid/settings/dashboard/SearchResultsSummary.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CarClass(CSearchResultsSummary)
    , public SearchResultsSummary
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CSEARCHRESULTSSUMMARY_H__
