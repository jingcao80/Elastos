
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CWAPPUSHOVERSMS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CWAPPUSHOVERSMS_H__

#include "_Elastos_Droid_Internal_Telephony_CWapPushOverSms.h"
#include "elastos/droid/internal/telephony/WapPushOverSms.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CWapPushOverSms)
    , public WapPushOverSms
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CWAPPUSHOVERSMS_H__
