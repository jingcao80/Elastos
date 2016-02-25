#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_FASTCOLORDRAWABLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_FASTCOLORDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::ICanvas;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class FastColorDrawable : public Elastos::Droid::Graphics::Drawable::Drawable
{
public:
    FastColorDrawable();

    FastColorDrawable(
        /* [in] */ Int32 color);

    //@Override
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    virtual CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    virtual CARAPI_(Int32) GetOpacity();

    virtual CARAPI SetBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI SetBounds(
        /* [in] */ IRect* bounds);

protected:
    CARAPI Init(
        /* [in] */ Int32 color);

private:
    Int32 mColor;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_FASTCOLORDRAWABLE_H__
