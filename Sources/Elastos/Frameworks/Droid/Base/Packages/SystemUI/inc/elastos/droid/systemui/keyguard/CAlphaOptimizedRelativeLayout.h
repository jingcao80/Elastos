
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDRELATIVELAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDRELATIVELAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CAlphaOptimizedRelativeLayout.h"
#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * A frame layout which does not have overlapping renderings commands and therefore does not need a
 * layer when alpha is changed.
 */
CarClass(CAlphaOptimizedRelativeLayout)
    , public RelativeLayout
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDRELATIVELAYOUT_H__
