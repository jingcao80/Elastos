#ifndef __ELASTOS_DROID_TELEPHONY_CSMSCBLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSCBLOCATION_H__

#include "_Elastos_Droid_Telephony_CSmsCbLocation.h"
#include "elastos/droid/telephony/SmsCbLocation.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsCbLocation)
    , public SmsCbLocation
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSCBLOCATION_H__
