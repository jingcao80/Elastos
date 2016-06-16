#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGE_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatCmdMessage.h"
#include "elastos/droid/internal/telephony/cat/CatCmdMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatCmdMessage)
    , public CatCmdMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATCMDMESSAGE_H__
