#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEMMICODE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEMMICODE_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Internal_Telephony_ImsPhone_CImsPhoneMmiCode.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneMmiCode.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CarClass(CImsPhoneMmiCode)
    , public ImsPhoneMmiCode
{
public:
    CAR_OBJECT_DECL()
};

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_CIMSPHONEMMICODE_H__
