#ifndef __ELASTOS_DROID_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__
#define __ELASTOS_DROID_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__

#include "_Elastos_Droid_Dialer_CCallDetailActivityQueryHandler.h"
#include "elastos/apps/dialer/CallDetailActivityQueryHandler.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CCallDetailActivityQueryHandler)
    , public CallDetailActivityQueryHandler
{
public:
    CAR_OBJECT_DECL();
};


} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CCALLDETAILACTIVITYQUERYHANDLER_H__
