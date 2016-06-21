#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECONNECTION_H__

#include "_Elastos_Droid_Internal_Telephony_ImsPhone_CImsPhoneConnection.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CarClass(CImsPhoneConnection)
    , public ImsPhoneConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECONNECTION_H__
