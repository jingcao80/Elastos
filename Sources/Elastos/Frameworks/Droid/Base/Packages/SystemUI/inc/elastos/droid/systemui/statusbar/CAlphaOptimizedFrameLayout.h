
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDFRAMELAYOUT_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDFRAMELAYOUT_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CAlphaOptimizedFrameLayout.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A frame layout which does not have overlapping renderings commands and therefore does not need a
 * layer when alpha is changed.
 */
CarClass(CAlphaOptimizedFrameLayout)
    , public FrameLayout
    , public IAlphaOptimizedFrameLayout
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

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

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDFRAMELAYOUT_H__
