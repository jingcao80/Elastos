
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CINTRINSICSIZE_DRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CINTRINSICSIZE_DRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CIntrinsicSizeDrawable.h"
#include "elastos/droid/systemui/statusbar/phone/CKeyguardBottomAreaView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CIntrinsicSizeDrawable)
    , public CKeyguardBottomAreaView::IntrinsicSizeDrawable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CINTRINSICSIZE_DRAWABLE_H__
