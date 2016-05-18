
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__

#include "_Elastos.Droid.SystemUI.h"
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

class CBarBackgroundDrawable;

class BarTransitions
    : public Object
    , public IBarTransitions
{
public:
    CAR_INTERFACE_DECL()

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
    AutoPtr<IDrawable> mBarBackground;

    Int32 mMode;
    friend class CBarBackgroundDrawable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BARTRANSITIONS_H__
