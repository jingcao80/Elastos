
#ifndef __ELASTOS_DROID_OS_CREGISTRANT_H__
#define __ELASTOS_DROID_OS_CREGISTRANT_H__

#include "_Elastos_Droid_Os_CRegistrant.h"
#include "elastos/droid/os/Registrant.h"

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
CarClass(CRegistrant)
    , public Registrant
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CREGISTRANT_H__
