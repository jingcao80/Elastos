
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMESSAGEAREA_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDMESSAGEAREA_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardMessageArea.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/***
 * Manages a number of views inside of the given layout. See below for a list of widgets.
 */
CarClass(CKeyguardMessageArea)
    , public TextView
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
        AutoPtr<IWeakReference> mHost;
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
    static const Object ANNOUNCE_TOKEN;

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
