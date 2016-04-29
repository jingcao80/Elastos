
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBLACKBARDRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBLACKBARDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CBlackBarDrawable.h"
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class KeyguardPreviewContainer;

CarClass(CBlackBarDrawable), public Drawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IKeyguardPreviewContainer* host);

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

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBLACKBARDRAWABLE_H__
