
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDLINEARLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDLINEARLAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CAlphaOptimizedLinearLayout.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CAlphaOptimizedLinearLayout)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDLINEARLAYOUT_H__
