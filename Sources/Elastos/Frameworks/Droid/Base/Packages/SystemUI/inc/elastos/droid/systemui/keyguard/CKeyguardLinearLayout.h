
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDLINEARLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDLINEARLAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardLinearLayout.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardLinearLayout)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL()

    CKeyguardLinearLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetTopChild(
        /* [in] */ IView* child);

public:
    Int32 mTopChild;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDLINEARLAYOUT_H__
