
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class BarTransitions
    : public Object
    , public IBarTransitions
{
private:
    class BarBackgroundDrawable: public Drawable
    {
    public:
        BarBackgroundDrawable(
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

public:
    CAR_INTERFACE_DECL();

    BarTransitions(
        /* [in] */ IView* view,
        /* [in] */ Int32 gradientResourceId);

    CARAPI GetMode(
        /* [out] */ Int32* mode);

    CARAPI TransitionTo(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate);

    static CARAPI_(String) ModeToString(
        /* [in] */ Int32 mode);

    CARAPI FinishAnimations();

protected:
    virtual CARAPI_(void) OnTransition(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

    virtual CARAPI_(void) ApplyModeBackground(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

private:
    static CARAPI_(Boolean) InitStatic();

public:
    static const Int32 MODE_OPAQUE = 0;
    static const Int32 MODE_SEMI_TRANSPARENT = 1;
    static const Int32 MODE_TRANSLUCENT = 2;
    static const Int32 MODE_LIGHTS_OUT = 3;
    static const Int32 MODE_TRANSPARENT = 4;
    static const Int32 MODE_WARNING = 5;

    static const Int32 LIGHTS_IN_DURATION = 250;
    static const Int32 LIGHTS_OUT_DURATION = 750;
    static const Int32 BACKGROUND_DURATION = 200;

private:
    static const Boolean DEBUG;
    static const Boolean DEBUG_COLORS;
    static const Boolean HIGH_END;

    String mTag;
    AutoPtr<IView> mView;
    AutoPtr<BarBackgroundDrawable> mBarBackground;

    Int32 mMode;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__
