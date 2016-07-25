#ifndef __ELASTOS_DROID_TELECOM_CCALL_H__
#define __ELASTOS_DROID_TELECOM_CCALL_H__

#include "_Elastos_Droid_Telecom_CCall.h"
#include "elastos/droid/telecom/Call.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CCall)
    , public Call
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCALL_H__