
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CFastColorDrawable.h"
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CFastColorDrawable), public Drawable
{
public:
    CAR_OBJECT_DECL()

    CFastColorDrawable();

    CARAPI constructor(
        /* [in] */ Int32 color);

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
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI SetBounds(
        /* [in] */ IRect* bounds);

private:
    Int32 mColor;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CFASTCOLORDRAWABLE_H__
