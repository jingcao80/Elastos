
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCALLTYPEHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCALLTYPEHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CCallTypeHelper.h"
#include "CallTypeHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CarClass(CCallTypeHelper)
    , public CallTypeHelper
{
public:
    CAR_OBJECT_DECL()
};


} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCALLTYPEHELPER_H__
