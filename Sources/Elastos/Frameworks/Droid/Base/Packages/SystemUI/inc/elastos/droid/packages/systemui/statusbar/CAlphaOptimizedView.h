
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDVIEW_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CAlphaOptimizedView.h"
#include <elastos/droid/view/View.h>

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

/**
 * A View which does not have overlapping renderings commands and therefore does not need a
 * layer when alpha is changed.
 */
CarClass(CAlphaOptimizedView)
    , public Elastos::Droid::View::View
    , public IAlphaOptimizedView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

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
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CALPHAOPTIMIZEDVIEW_H__
