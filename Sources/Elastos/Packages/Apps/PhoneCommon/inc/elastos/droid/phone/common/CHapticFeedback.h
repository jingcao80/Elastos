
#ifndef __ELASTOS_DROID_PHONE_COMMON_CHAPTICFEEDBACK_H__
#define __ELASTOS_DROID_PHONE_COMMON_CHAPTICFEEDBACK_H__

#include "_Elastos_Droid_Phone_Common_CHapticFeedback.h"
#include "elastos/droid/phone/common/HapticFeedback.h"

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {

CarClass(CHapticFeedback)
    , public HapticFeedback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_CHAPTICFEEDBACK_H__