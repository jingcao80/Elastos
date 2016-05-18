
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONGUTS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONGUTS_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationGuts.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * The guts of a notification revealed when performing a long press.
 */
CarClass(CNotificationGuts)
    , public FrameLayout
    , public INotificationGuts
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationGuts();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI SetActualHeight(
        /* [in] */ Int32 actualHeight);

    CARAPI GetActualHeight(
        /* [out] */ Int32* height);

    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

private:
    CARAPI_(void) Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* drawable);

    CARAPI_(void) DrawableStateChanged(
        /* [in] */ IDrawable* d);

private:
    AutoPtr<IDrawable> mBackground;
    Int32 mClipTopAmount;
    Int32 mActualHeight;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONGUTS_H__
