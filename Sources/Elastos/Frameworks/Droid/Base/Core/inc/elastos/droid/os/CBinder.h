#ifndef __ELASTOS_DROID_OS_CBINDER_H__
#define __ELASTOS_DROID_OS_CBINDER_H__

#include "_Elastos_Droid_Os_CBinder.h"
#include "elastos/droid/os/Binder.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBinder)
    , public Binder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBINDER_H__
