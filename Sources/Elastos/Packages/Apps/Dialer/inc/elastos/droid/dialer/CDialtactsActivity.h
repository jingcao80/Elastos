#ifndef __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITY_H__
#define __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITY_H__

#include "_Elastos_Droid_Dialer_CDialtactsActivity.h"
#include "elastos/droid/dialer/DialtactsActivity.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CDialtactsActivity)
    , public DialtactsActivity
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CDIALTACTSACTIVITY_H__
