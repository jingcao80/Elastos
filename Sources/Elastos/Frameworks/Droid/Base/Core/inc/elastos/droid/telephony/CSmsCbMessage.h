#ifndef __ELASTOS_DROID_TELEPHONY_CSMSCBMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSCBMESSAGE_H__

#include "_Elastos_Droid_Telephony_CSmsCbMessage.h"
#include "elastos/droid/telephony/SmsCbMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsCbMessage)
    , public SmsCbMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSCBMESSAGE_H__
