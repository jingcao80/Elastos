
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEWIMAGEBUTTON_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEWIMAGEBUTTON_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CDismissViewImageButton.h"
#include <elastos/droid/widget/ImageButton.h>

using Elastos::Droid::Widget::ImageButton;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CarClass(CDismissViewImageButton)
    , public ImageButton
    , public IDismissViewImageButton
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

    /**
     * This method returns the drawing rect for the view which is different from the regular
     * drawing rect, since we layout all children in the {@link NotificationStackScrollLayout} at
     * position 0 and usually the translation is neglected. The standard implementation doesn't
     * account for translation.
     *
     * @param outRect The (scrolled) drawing bounds of the view.
     */
    // @Override
    CARAPI GetDrawingRect(
        /* [in] */ IRect* outRect);
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEWIMAGEBUTTON_H__
