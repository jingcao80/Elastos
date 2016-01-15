
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FIXEDSIZEDRAWABLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FIXEDSIZEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class FixedSizeDrawable : public Elastos::Droid::Graphics::Drawable::Drawable
{
public:
    FixedSizeDrawable();

    FixedSizeDrawable(
        /* [in] */ IDrawable* that);

    CARAPI SetFixedBounds(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI SetBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI SetBounds(
        /* [in] */ IRect* bounds);

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(Int32) GetOpacity();

    virtual CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    virtual CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

protected:
    CARAPI Init(
        /* [in] */ IDrawable* that);
private:
    AutoPtr<IDrawable> mDrawable;
    Int32 mLeft;
    Int32 mTop;
    Int32 mRight;
    Int32 mBottom;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_FIXEDSIZEDRAWABLE_H__
