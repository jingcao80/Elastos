
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_ALPHAOPTIMIZEDLINEARLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_ALPHAOPTIMIZEDLINEARLAYOUT_H__

#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class AlphaOptimizedLinearLayout
    : public LinearLayout
{
public:
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

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_ALPHAOPTIMIZEDLINEARLAYOUT_H__
