#ifndef __ELASTOS_APPS_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__
#define __ELASTOS_APPS_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__

#include "_Elastos_Apps_Dialer_CCallDetailActivityQueryHandler.h"
#include "elastos/apps/dialer/CallDetailActivityQueryHandler.h"

namespace Elastos {
namespace Apps {
namespace Dialer {

CarClass(CCallDetailActivityQueryHandler)
    , public CallDetailActivityQueryHandler
{
public:
    CAR_OBJECT_DECL();
};


} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__
