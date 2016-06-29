#ifndef __ELASTOS_APPS_DIALER_CPHONECALLDETAILSHELPER_H__
#define __ELASTOS_APPS_DIALER_CPHONECALLDETAILSHELPER_H__

#include "_Elastos_Apps_Dialer_CPhoneCallDetailsHelper.h"
#include "elastos/apps/dialer/PhoneCallDetailsHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {

CarClass(CPhoneCallDetailsHelper)
    , public PhoneCallDetailsHelper
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CPHONECALLDETAILSHELPER_H__
