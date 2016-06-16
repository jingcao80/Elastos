#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CCONTACTINFOHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CCONTACTINFOHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CContactInfoHelper.h"
#include "elastos/apps/dialer/ContactInfoHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CarClass(CContactInfoHelper)
    , public ContactInfoHelper
{
public:
    CAR_OBJECT_DECL()
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CCONTACTINFOHELPER_H__