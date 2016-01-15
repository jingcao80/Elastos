#ifndef __ELASTOS_DROID_INTERNAL_OS_CPROCESSCPUTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CPROCESSCPUTRACKER_H__

#include "_Elastos_Droid_Internal_Os_CProcessCpuTracker.h"
#include "ProcessCpuTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CProcessCpuTracker)
    , public ProcessCpuTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CPROCESSCPUTRACKER_H__
