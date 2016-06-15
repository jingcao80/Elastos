#ifndef __ELASTOS_DROID_TELEPHONY_CSMSCBETWSINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSCBETWSINFO_H__

#include "_Elastos_Droid_Telephony_CSmsCbEtwsInfo.h"
#include "elastos/droid/telephony/SmsCbEtwsInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsCbEtwsInfo)
    , public SmsCbEtwsInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSCBETWSINFO_H__
