#ifndef __ELASTOS_APPS_DIALER_CDIALERAPPLICATION_H__
#define __ELASTOS_APPS_DIALER_CDIALERAPPLICATION_H__

#include "_Elastos_Apps_Dialer_CDialerApplication.h"
#include "DialerApplication.h"

namespace Elastos{
namespace Apps{
namespace Dialer{

CarClass(CDialerApplication)
    , public DialerApplication
{
public:
    CAR_OBJECT_DECL()
};


} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CDIALERAPPLICATION_H__
