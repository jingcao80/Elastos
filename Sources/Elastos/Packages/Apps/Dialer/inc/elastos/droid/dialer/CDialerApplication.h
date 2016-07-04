#ifndef __ELASTOS_DROID_DIALER_CDIALERAPPLICATION_H__
#define __ELASTOS_DROID_DIALER_CDIALERAPPLICATION_H__

#include "_Elastos_Droid_Dialer_CDialerApplication.h"
#include "elastos/droid/dialer/DialerApplication.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CDialerApplication)
    , public DialerApplication
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CDIALERAPPLICATION_H__
