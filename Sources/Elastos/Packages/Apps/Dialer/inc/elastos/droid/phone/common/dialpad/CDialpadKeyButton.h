
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADKEYBUTTON_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADKEYBUTTON_H__

#include "_Elastos_Droid_Phone_Common_Dialpad_CDialpadKeyButton.h"
#include "elastos/droid/phone/common/dialpad/DialpadKeyButton.h"

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

CarClass(CDialpadKeyButton)
    , public DialpadKeyButton
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADKEYBUTTON_H__