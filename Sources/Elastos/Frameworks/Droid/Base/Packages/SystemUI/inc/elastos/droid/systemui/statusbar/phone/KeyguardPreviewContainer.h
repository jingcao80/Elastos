#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view group which contains the preview of phone/camera and draws a black bar at the bottom as
 * the fake navigation bar.
 */
class KeyguardPreviewContainer
    : public FrameLayout
    , public IKeyguardPreviewContainer
{
public:
    CAR_INTERFACE_DECL()

    KeyguardPreviewContainer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** sets);

private:
    AutoPtr<IDrawable> mBlackBarDrawable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
