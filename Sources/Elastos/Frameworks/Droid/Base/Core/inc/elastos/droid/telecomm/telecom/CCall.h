#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CCALL_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CCALL_H__

#include "_Elastos_Droid_Telecomm_Telecom_CCall.h"
#include "elastos/droid/telecomm/telecom/Call.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CCall)
    , public Call
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CCALL_H__