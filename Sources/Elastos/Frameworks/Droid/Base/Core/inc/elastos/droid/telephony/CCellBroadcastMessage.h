#ifndef __ELASTOS_DROID_TELEPHONY_CCELLBROADCASTMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLBROADCASTMESSAGE_H__

#include "_Elastos_Droid_Telephony_CCellBroadcastMessage.h"
#include "elastos/droid/telephony/CellBroadcastMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellBroadcastMessage)
    , public CellBroadcastMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLBROADCASTMESSAGE_H__
