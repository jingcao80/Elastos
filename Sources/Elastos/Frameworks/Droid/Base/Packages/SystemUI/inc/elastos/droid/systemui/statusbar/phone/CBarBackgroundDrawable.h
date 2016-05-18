
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBARBACKGROUNDDRAWABLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBARBACKGROUNDDRAWABLE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CBarBackgroundDrawable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::Drawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CBarBackgroundDrawable), public Drawable
{
public:
    CAR_OBJECT_DECL()

    CBarBackgroundDrawable();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 gradientResourceId);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    CARAPI ApplyModeBackground(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    CARAPI FinishAnimation();

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

protected:
    // @Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

private:
    Int32 mOpaque;
    Int32 mSemiTransparent;
    Int32 mTransparent;
    Int32 mWarning;
    AutoPtr<IDrawable> mGradient;
    AutoPtr<ITimeInterpolator> mInterpolator;

    Int32 mMode;
    Boolean mAnimating;
    Int64 mStartTime;
    Int64 mEndTime;

    Int32 mGradientAlpha;
    Int32 mColor;

    Int32 mGradientAlphaStart;
    Int32 mColorStart;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CBARBACKGROUNDDRAWABLE_H__
