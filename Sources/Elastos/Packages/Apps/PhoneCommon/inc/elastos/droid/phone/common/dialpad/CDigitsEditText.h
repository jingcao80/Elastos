
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIGITSEDITTEXT_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIGITSEDITTEXT_H__

#include "_Elastos_Droid_Phone_Common_Dialpad_CDigitsEditText.h"
#include "elastos/droid/phone/common/dialpad/DigitsEditText.h"

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

CarClass(CDigitsEditText)
    , public DigitsEditText
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIGITSEDITTEXT_H__