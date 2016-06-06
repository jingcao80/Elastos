#ifndef __ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADER_H__
#define __ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADER_H__

#include "_Elastos_Apps_Dialer_Util_CEmptyLoader.h"
#include "EmptyLoader.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

CarClass(CEmptyLoader)
    , public EmptyLoader
{
public:
    CAR_OBJECT_DECL()
};

} // Util
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADER_H__
