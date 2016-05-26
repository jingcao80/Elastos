#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CDeadZone.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CDeadZone)
    , public Elastos::Droid::View::View
    , public IDeadZone
{
private:
    class DebugFlashRunnable : public Runnable {
    public:
        DebugFlashRunnable(
            /* [in] */ CDeadZone* host);

        virtual CARAPI Run();

    private:
        CDeadZone* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDeadZone();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetFlashOnTouchCapture(
        /* [in] */ Boolean dbg);

    CARAPI Poke(
        /* [in] */ IMotionEvent* event);

    CARAPI SetFlash(
        /* [in] */ Float f);

    CARAPI GetFlash(
        /* [out] */ Float* f);

    static CARAPI_(Float) Lerp(
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [in] */ Float c);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Float) GetSize(
        /* [in] */ Int64 now);

private:
    static const Boolean CHATTY;

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

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDEADZONE_H__
