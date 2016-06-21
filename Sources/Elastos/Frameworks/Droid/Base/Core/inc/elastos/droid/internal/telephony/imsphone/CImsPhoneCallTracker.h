#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALLTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALLTRACKER_H__

#include "_Elastos_Droid_Internal_Telephony_ImsPhone_CImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCallTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CarClass(CImsPhoneCallTracker)
    , public ImsPhoneCallTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONECALLTRACKER_H__
