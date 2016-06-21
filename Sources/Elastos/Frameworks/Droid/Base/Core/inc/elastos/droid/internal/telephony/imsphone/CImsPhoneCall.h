#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALL_H__

#include "_Elastos_Droid_Internal_Telephony_ImsPhone_CImsPhoneCall.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCall.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CarClass(CImsPhoneCall)
    , public ImsPhoneCall
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALL_H__
