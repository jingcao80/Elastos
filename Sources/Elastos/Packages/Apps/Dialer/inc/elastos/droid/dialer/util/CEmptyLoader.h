#ifndef __ELASTOS_DROID_DIALER_UTIL_CEMPTYLOADER_H__
#define __ELASTOS_DROID_DIALER_UTIL_CEMPTYLOADER_H__

#include "_Elastos_Droid_Dialer_Util_CEmptyLoader.h"
#include "elastos/apps/dialer/util/EmptyLoader.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

CarClass(CEmptyLoader)
    , public EmptyLoader
{
public:
    CAR_OBJECT_DECL();
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_CEMPTYLOADER_H__
