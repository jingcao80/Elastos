
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSimPukView.h"
#include <elastos/droid/widget/ImageButton.h>

using Elastos::Droid::Widget::ImageButton;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays a PIN pad for entering a PUK (Pin Unlock Kode) provided by a carrier.
 */
CarClass(CKeyguardSimPukView)
    , public KeyguardPinBasedInputView
{
private:
    class StateMachine
        : public Object
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::StateMachine")

        StateMachine(
            /* [in] */ CKeyguardSimPukView* host);

        CARAPI Next();

        CARAPI Reset();

    public:
        const Int32 ENTER_PUK;
        const Int32 ENTER_PIN;
        const Int32 CONFIRM_PIN;
        const Int32 DONE;

    private:
        Int32 mState = ENTER_PUK;
        CKeyguardSimPukView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardSimPukView* host,
            /* [in] */ Int32 result1,
            /* [in] */ Int32 result2)
            : mHost(host)
            , mResult1(result1)
            , mResult2(result2)

        CARAPI Run();

    private:
        CKeyguardSimPukView* mHost;
        Int32 mResult1;
        Int32 mResult2;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::MyRunnable2")

        MyRunnable2(
            /* [in] */ CKeyguardSimPukView* host)
            : mHost(host)

        CARAPI Run();

    private:
        CKeyguardSimPukView* mHost;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::MyRunnable3")

        MyRunnable3(
            /* [in] */ CKeyguardSimPukView* host,
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining)
            : mHost(host)
            , mResult(result)
            , mAttemptsRemaining(attemptsRemaining)
        {}

        CARAPI Run();

    private:
        CKeyguardSimPukView* mHost;
        Int32 mResult;
        Int32 mAttemptsRemaining;
    };

    /**
     * Since the IPC can block, we want to run the request in a separate thread
     * with a callback.
     */
    class CheckSimPuk
        : public Thread
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::CheckSimPuk")

        //@Override
        CARAPI Run();

    protected:
        CheckSimPuk(
            /* [in] */ CKeyguardSimPukView* host,
            /* [in] */ const String& puk,
            /* [in] */ const String& pin)
            : mHost(host)
            , mPuk(puk)
            , mPin(pin)
        {}

        virtual CARAPI OnSimLockChangedResponse(
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining) = 0;

    private:
        CKeyguardSimPukView* mHost;
        String mPin, mPuk;
    };

    class MyCheckSimPuk
        : public CheckSimPuk
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPukView::MyCheckSimPuk")

        CARAPI OnSimLockChangedResponse(
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining);
    }

public:
    CAR_OBJECT_DECL()

    CKeyguardSimPukView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI ResetState();

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI OnPause();

    CARAPI ConfirmPin(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI ShouldLockout(
        /* [in] */ Int64 deadline,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetPasswordTextViewId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI VerifyPasswordAndUnlock();

private:
    CARAPI_(String) GetPukPasswordErrorMessage(
        /* [in] */ Int32 attemptsRemaining);

    CARAPI_(AutoPtr<IDialog>) GetSimUnlockProgressDialog();

    CARAPI_(AutoPtr<IDialog>) GetPukRemainingAttemptsDialog(
        /* [in] */ Int32 remaining);

    CARAPI_(Boolean) CheckPuk();

    CARAPI_(Boolean) CheckPin();

    CARAPI_(void) UpdateSim();

public:
    static const String TAG;

private:
    static const String LOG_TAG;
    static const Boolean DEBUG;

    AutoPtr<IProgressDialog> mSimUnlockProgressDialog;
    AutoPtr<ICheckSimPuk> mCheckSimPukThread;
    String mPukText;
    String mPinText;
    AutoPtr<StateMachine> mStateMachine;
    AutoPtr<IAlertDialog> mRemainingAttemptsDialog;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPUKVIEW_H__
