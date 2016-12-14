#ifndef __ELASTOS_DROID_OS_CBATTERYSTATSHISTORYITEM_H__
#define __ELASTOS_DROID_OS_CBATTERYSTATSHISTORYITEM_H__

#include "_Elastos_Droid_Os_CBatteryStatsHistoryItem.h"
#include "elastos/droid/os/BatteryStats.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBatteryStatsHistoryItem)
    , public BatteryStats::HistoryItem
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBATTERYSTATSHISTORYITEM_H__
