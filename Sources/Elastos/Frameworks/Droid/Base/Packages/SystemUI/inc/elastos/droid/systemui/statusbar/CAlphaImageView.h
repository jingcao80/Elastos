
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAIMAGEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAIMAGEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CAlphaImageView.h"
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * An ImageView which does not have overlapping renderings commands and therefore does not need a
 * layer when alpha is changed.
 */
CarClass(CAlphaImageView)
    , public ImageView
    , public IAlphaImageView
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
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CALPHAIMAGEVIEW_H__
