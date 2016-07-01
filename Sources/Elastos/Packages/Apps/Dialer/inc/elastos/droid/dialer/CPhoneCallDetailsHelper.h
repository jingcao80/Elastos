#ifndef __ELASTOS_DROID_DIALER_CPHONECALLDETAILSHELPER_H__
#define __ELASTOS_DROID_DIALER_CPHONECALLDETAILSHELPER_H__

#include "_Elastos_Droid_Dialer_CPhoneCallDetailsHelper.h"
#include "elastos/apps/dialer/PhoneCallDetailsHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CPhoneCallDetailsHelper)
    , public PhoneCallDetailsHelper
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CPHONECALLDETAILSHELPER_H__
