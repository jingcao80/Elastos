#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "_SystemUI.h"
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
private:
    class BlackBarDrawable: public Drawable
    {
    public:
        BlackBarDrawable(
            /* [in] */ KeyguardPreviewContainer* host);

        // @Override
        CARAPI Draw(
            /* [in] */ ICanvas* canvas);

        // @Override
        CARAPI SetAlpha(
            /* [in] */ Int32 alpha);

        // @Override
        CARAPI SetColorFilter(
            /* [in] */ IColorFilter* cf);

        // @Override
        CARAPI GetOpacity(
            /* [out] */ Int32* opacity);

    private:
        KeyguardPreviewContainer* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    KeyguardPreviewContainer(
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
