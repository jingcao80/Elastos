#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEMEASUREMENTSTATSOBSERVER_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEMEASUREMENTSTATSOBSERVER_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CStorageMeasurementStatsObserver.h"
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CarClass(CStorageMeasurementStatsObserver)
    , public StorageMeasurement::StatsObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEMEASUREMENTSTATSOBSERVER_H__