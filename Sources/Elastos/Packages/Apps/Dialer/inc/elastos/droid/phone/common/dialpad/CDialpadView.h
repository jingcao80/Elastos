
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADVIEW_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADVIEW_H__

#include "_Elastos_Droid_Phone_Common_Dialpad_CDialpadView.h"
#include "elastos/droid/phone/common/dialpad/DialpadView.h"

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

CarClass(CDialpadView)
    , public DialpadView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_CDIALPADVIEW_H__
