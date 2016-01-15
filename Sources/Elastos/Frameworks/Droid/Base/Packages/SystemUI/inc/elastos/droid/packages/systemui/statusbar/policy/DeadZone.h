
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_DEADZONE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_DEADZONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/View.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


class DeadZone : public Elastos::Droid::View::View
{
private:
    class DebugFlashRunnable : public Runnable {
    public:
        DebugFlashRunnable(
            /* [in] */ DeadZone* host);

        virtual CARAPI Run();
    private:
        DeadZone* mHost;
    };

public:
    DeadZone();

    DeadZone(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    DeadZone(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    CARAPI SetFlashOnTouchCapture(
        /* [in] */ Boolean dbg);

    CARAPI Poke(
        /* [in] */ IMotionEvent* event);

    CARAPI SetFlash(
        /* [in] */ Float f);

    CARAPI_(Float) GetFlash();

    CARAPI_(Float) Lerp(
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [in] */ Float c);

    virtual CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

protected:

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

private:
    CARAPI_(Float) GetSize(
        /* [in] */ Int64 now);

private:
    static const Boolean CHATTY = TRUE;

    Boolean mShouldFlash;
    Float mFlashFrac;

    Int32 mSizeMax;
    Int32 mSizeMin;

    // Upon activity elsewhere in the UI, the dead zone will hold steady for
    // mHold ms, then move back over the course of mDecay ms
    Int32 mHold;
    Int32 mDecay;
    Boolean mVertical;
    Int64 mLastPokeTime;

    AutoPtr<IRunnable> mDebugFlash;
};


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_DEADZONE_H__
