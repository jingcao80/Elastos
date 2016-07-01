

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERUTILSWRAPPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERUTILSWRAPPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CPhoneNumberUtilsWrapper.h"
#include "elastos/apps/dialer/calllog/PhoneNumberUtilsWrapper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CPhoneNumberUtilsWrapper)
    , public PhoneNumberUtilsWrapper
{
public:
    CAR_OBJECT_DECL()

};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CPHONENUMBERUTILSWRAPPER_H__
