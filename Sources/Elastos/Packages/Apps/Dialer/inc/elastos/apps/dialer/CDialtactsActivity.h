#ifndef __ELASTOS_APPS_DIALER_CDIALTACTSACTIVITY_H__
#define __ELASTOS_APPS_DIALER_CDIALTACTSACTIVITY_H__

#include "_Elastos_Apps_Dialer_CDialtactsActivity.h"
#include "DialtactsActivity.h"

namespace Elastos{
namespace Apps{
namespace Dialer{

CarClass(CDialtactsActivity)
    , public DialtactsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CDIALTACTSACTIVITY_H__
