
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHERSCRIM_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHERSCRIM_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CKeyguardUserSwitcherScrim.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Gradient background for the user switcher on Keyguard.
 */
CarClass(CKeyguardUserSwitcherScrim)
    , public Drawable
    , public IKeyguardUserSwitcherScrim
    , public IViewOnLayoutChangeListener
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardUserSwitcherScrim();

    CARAPI constructor(
        /* [in] */ IView* host);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI OnLayoutChange(
        /* [in] */ IView* v,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 oldLeft,
        /* [in] */ Int32 oldTop,
        /* [in] */ Int32 oldRight,
        /* [in] */ Int32 oldBottom);

    CARAPI_(void) UpdatePaint();

private:
    static const Float OUTER_EXTENT;
    static const Float INNER_EXTENT;

    Int32 mDarkColor;
    Int32 mTop;
    Int32 mAlpha;
    AutoPtr<IPaint> mRadialGradientPaint;
    Int32 mLayoutWidth;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDUSERSWITCHERSCRIM_H__
