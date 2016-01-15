
#ifndef __ELASTOS_DROID_OS_CHANDLER_H__
#define __ELASTOS_DROID_OS_CHANDLER_H__

#include "_Elastos_Droid_Os_CHandler.h"
#include "Handler.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CHandler)
    , public Handler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CHANDLER_H__
