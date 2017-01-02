//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
