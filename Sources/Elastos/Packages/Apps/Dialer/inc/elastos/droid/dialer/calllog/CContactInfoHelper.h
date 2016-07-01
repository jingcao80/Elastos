#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFOHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFOHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CContactInfoHelper.h"
#include "elastos/apps/dialer/calllog/ContactInfoHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CContactInfoHelper)
    , public ContactInfoHelper
{
public:
    CAR_OBJECT_DECL();
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCONTACTINFOHELPER_H__