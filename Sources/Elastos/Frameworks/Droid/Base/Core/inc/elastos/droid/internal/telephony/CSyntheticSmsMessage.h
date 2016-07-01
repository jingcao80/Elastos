
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGE_H__

#include "_Elastos_Droid_Internal_Telephony_CSyntheticSmsMessage.h"
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSyntheticSmsMessage)
    , public SyntheticSmsMessage
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSYNTHETICSMSMESSAGE_H__
