
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDIMAGEBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDIMAGEBUTTON_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CAlphaOptimizedImageButton.h"
#include <elastos/droid/widget/ImageButton.h>

using Elastos::Droid::Widget::ImageButton;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CAlphaOptimizedImageButton)
    , public ImageButton
{
public:
    CAR_OBJECT_DECL();

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

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CALPHAOPTIMIZEDIMAGEBUTTON_H__
