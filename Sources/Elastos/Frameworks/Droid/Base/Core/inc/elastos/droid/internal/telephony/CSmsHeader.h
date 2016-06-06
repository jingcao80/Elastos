
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADER_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsHeader.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsHeader), public SmsHeader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSHEADER_H__
