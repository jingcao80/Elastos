
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYBUTTONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYBUTTONVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class KeyButtonView : public ImageView
{
private:
    class CheckLongPressRunnable : public Runnable {
    public:
        CheckLongPressRunnable(
            /* [in] */ KeyButtonView* host);

        virtual CARAPI Run();
    private:
        KeyButtonView* mHost;
    };

public:
    KeyButtonView();

    KeyButtonView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    KeyButtonView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetDrawingAlpha(
        /* [in] */ Float alpha);

    CARAPI_(Float) GetDrawingAlpha();

    CARAPI SetGlowAlpha(
        /* [in] */ Float alpha);

    CARAPI_(Float) GetGlowAlpha();

    CARAPI SetGlowScale(
        /* [in] */ Float scale);

    CARAPI_(Float) GetGlowScale();

    // @Override
    CARAPI SetPressed(
        /* [in] */ Boolean pressed);

    // @Override
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) SendEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 flags);

    CARAPI_(void) SendEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 when);

private:
    static const Float GLOW_MAX_SCALE_FACTOR;
    static const Float BUTTON_QUIESCENT_ALPHA;

    Int64 mDownTime;
    Int32 mCode;
    Int32 mTouchSlop;
    AutoPtr<IDrawable> mGlowBG;
    Int32 mGlowWidth, mGlowHeight;
    Float mGlowAlpha;
    Float mGlowScale;
    Float mDrawingAlpha;
    Boolean mSupportsLongpress;
    AutoPtr<IRectF> mRect;
    AutoPtr<IAnimatorSet> mPressedAnim;

    AutoPtr<IRunnable> mCheckLongPress;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYBUTTONVIEW_H__
