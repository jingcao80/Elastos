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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMESSAGEAREA_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMESSAGEAREA_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardMessageArea.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/droid/widget/TextView.h>
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include "R.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::TextView;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/***
 * Manages a number of views inside of the given layout. See below for a list of widgets.
 */
CarClass(CKeyguardMessageArea)
    , public TextView
    , public IKeyguardMessageArea
{
public:
    class Helper
        : public Object
        , public ISecurityMessageDisplay
    {
    public:
        TO_STRING_IMPL("CKeyguardMessageArea::Helper")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IView* v);

        CARAPI SetMessage(
            /* [in] */ ICharSequence* msg,
            /* [in] */ Boolean important);

        CARAPI SetMessage(
            /* [in] */ Int32 resId,
            /* [in] */ Boolean important);

        CARAPI SetMessage(
            /* [in] */ Int32 resId,
            /* [in] */ Boolean important,
            /* [in] */ ArrayOf<IInterface*>* formatArgs);

        //@Override
        CARAPI ShowBouncer(
            /* [in] */ Int32 duration);

        //@Override
        CARAPI HideBouncer(
            /* [in] */ Int32 duration);

        //@Override
        CARAPI SetTimeout(
            /* [in] */ Int32 timeoutMs);

    public:
        CKeyguardMessageArea* mMessageArea;
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardMessageArea::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardMessageArea* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardMessageArea* mHost;
    };

    /**
     * Runnable used to delay accessibility announcements.
     */
    class AnnounceRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardMessageArea::AnnounceRunnable")

        AnnounceRunnable(
            /* [in] */ IView* host,
            /* [in] */ ICharSequence* textToAnnounce);

        //@Override
        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
        AutoPtr<ICharSequence> mTextToAnnounce;
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardMessageArea::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardMessageArea* host)
            : mHost(host)
        {}

        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        CARAPI OnScreenTurnedOn();

    private:
        CKeyguardMessageArea* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CKeyguardMessageArea::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ CKeyguardMessageArea* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CKeyguardMessageArea* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardMessageArea();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SecurityMessageChanged();

    /**
     * Update the status lines based on these rules:
     * AlarmStatus: Alarm state always gets it's own line.
     * Status1 is shared between help, battery status and generic unlock instructions,
     * prioritized in that order.
     * @param showStatusLines status lines are shown if true
     */
    CARAPI Update();


    CARAPI GetCurrentMessage(
        /* [out] */ ICharSequence** cchar);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) HideMessage(
        /* [in] */ Int32 duration,
        /* [in] */ Boolean thenUpdate);

    CARAPI_(void) ShowMessage(
        /* [in] */ Int32 duration);

protected:
    static const Int32 FADE_DURATION;

private:
    /** Handler token posted with accessibility announcement runnables. */
    static const AutoPtr<IInterface> ANNOUNCE_TOKEN;

    /**
     * Delay before speaking an accessibility announcement. Used to prevent
     * lift-to-type from interrupting itself.
     */
    static const Int64 ANNOUNCEMENT_DELAY;

    static const Int32 SECURITY_MESSAGE_DURATION;

    static const String TAG;

    // is the bouncer up?
    Boolean mShowingBouncer;

    AutoPtr<IKeyguardUpdateMonitor> mUpdateMonitor;

    // Timeout before we reset the message to show charging/owner info
    Int64 mTimeout;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ICharSequence> mMessage;
    Boolean mShowingMessage;
    AutoPtr<ICharSequence> mSeparator;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<IRunnable> mClearMessageRunnable;

    AutoPtr<MyKeyguardUpdateMonitorCallback> mInfoCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMESSAGEAREA_H__
