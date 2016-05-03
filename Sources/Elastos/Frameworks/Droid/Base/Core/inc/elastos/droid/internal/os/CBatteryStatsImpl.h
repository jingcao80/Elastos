#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSIMPL_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSIMPL_H__

#include "_Elastos_Droid_Internal_Os_CProcessCpuTracker.h"
#include "elastos/droid/internal/os/BatteryStatsImpl.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBatteryStatsImpl)
    , public BatteryStatsImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSIMPL_H__
