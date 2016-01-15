#ifndef __ELASTOS_DROID_OS_CUSERHANDLE_H__
#define __ELASTOS_DROID_OS_CUSERHANDLE_H__

#include "_Elastos_Droid_Os_CUserHandle.h"
#include "elastos/droid/os/UserHandle.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CUserHandle)
    , public UserHandle
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUSERHANDLE_H__
