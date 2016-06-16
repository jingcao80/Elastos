#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATRESPONSEMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATRESPONSEMESSAGE_H__

#include "_Elastos_Droid_Internal_Telephony_Cat_CCatResponseMessage.h"
#include "elastos/droid/internal/telephony/cat/CatResponseMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

CarClass(CCatResponseMessage)
    , public CatResponseMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CCATRESPONSEMESSAGE_H__
