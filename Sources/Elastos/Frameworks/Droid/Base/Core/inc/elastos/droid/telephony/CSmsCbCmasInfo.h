#ifndef __ELASTOS_DROID_TELEPHONY_CSMSCBCMASINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSCBCMASINFO_H__

#include "_Elastos_Droid_Telephony_CSmsCbCmasInfo.h"
#include "elastos/droid/telephony/SmsCbCmasInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsCbCmasInfo)
    , public SmsCbCmasInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSCBCMASINFO_H__
