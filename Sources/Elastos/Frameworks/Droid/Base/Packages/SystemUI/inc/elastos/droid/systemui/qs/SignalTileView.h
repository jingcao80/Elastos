
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__

#include "elastos/droid/systemui/qs/QSTileView.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** View that represents a custom quick settings tile for displaying signal info (wifi/cell). **/
class SignalTileView
    : public QSTileView
{
public:
    SignalTileView(
        /* [in] */ IContext* context);

protected:
    // @Override
    CARAPI_(AutoPtr<IView>) CreateIcon();

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI HandleStateChanged(
        /* [in] */ IQSTileState* state);

private:
    CARAPI_(AutoPtr<IImageView>) AddTrafficView(
        /* [in] */ Int32 icon);

    CARAPI_(void) LayoutIndicator(
        /* [in] */ IView* indicator);

    CARAPI_(void) SetVisibility(
        /* [in] */ IView* view,
        /* [in] */ Boolean shown,
        /* [in] */ Boolean visible);

    static CARAPI_(Int64) InitStatic();

private:
    static const Int64 DEFAULT_DURATION;
    static const Int64 SHORT_DURATION;

    AutoPtr<IFrameLayout> mIconFrame;
    AutoPtr<IImageView> mSignal;
    AutoPtr<IImageView> mOverlay;
    AutoPtr<IImageView> mIn;
    AutoPtr<IImageView> mOut;

    Int32 mWideOverlayIconStartPadding;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__
