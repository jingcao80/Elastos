
#ifndef __ELASTOS_DROID_OS_CREGISTRANTLIST_H__
#define __ELASTOS_DROID_OS_CREGISTRANTLIST_H__

#include "_Elastos_Droid_Os_CRegistrantList.h"
#include "elastos/droid/os/RegistrantList.h"

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
CarClass(CRegistrantList)
    , public RegistrantList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CREGISTRANTLIST_H__
