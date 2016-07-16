
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKERGUARDSIMPINVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKERGUARDSIMPINVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSimPinView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays a PIN pad for unlocking.
 */
CarClass(CKeyguardSimPinView)
    , public KeyguardPinBasedInputView
{
private:
    /**
     * Since the IPC can block, we want to run the request in a separate thread
     * with a callback.
     */
    class CheckSimPin
        : public Thread
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("CKeyguardSimPinView::CheckSimPin::MyRunnable")

            MyRunnable(
                /* [in] */ CheckSimPin* host,)
                : mHost(host)
            {}

            CARAPI Run();

        private:
            CheckSimPin* mHost;
        };

        class MyRunnable2
            : public Runnable
        {
        public:
            TO_STRING_IMPL("CKeyguardSimPinView::CheckSimPin::MyRunnable2")

            MyRunnable2(
                /* [in] */ CheckSimPin* host,)
                : mHost(host)
            {}

            CARAPI Run();

        private:
            CheckSimPin* mHost;
        };

    public:
        TO_STRING_IMPL("CKeyguardSimPinView::CheckSimPin")

        virtual CARAPI OnSimCheckResponse(
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining) = 0;

        //@Override
        CARAPI Run();

    protected:
        CheckSimPin(
            /* [in] */ const String& pin)
            : mPin(pin)
        {}

    private:
        String mPin;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPinView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardSimPinView* host,
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining)
            : mHost(host)
            , mResult(result)
            , mAttemptsRemaining(attemptsRemaining)
        {}

        CARAPI Run();

    private:
        CKeyguardSimPinView* mHost;
        Int32 mResult;
        Int32 mAttemptsRemaining;
    };

    class MyCheckSimPin
        : public CheckSimPin
    {
    public:
        TO_STRING_IMPL("CKeyguardSimPinView::MyCheckSimPin")

        MyCheckSimPin(
            /* [in] */ const String& pin,
            /* [in] */ CKeyguardSimPinView* host)
            : CheckSimPin(pin)
            , mHost(host)
        {}

        CARAPI OnSimCheckResponse(
            /* [in] */ Int32 result,
            /* [in] */ Int32 attemptsRemaining);

    private:
        CKeyguardSimPinView* mHost;
    };

public:
    CAR_OBJECT_DECL()

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
    CARAPI_(String) GetPinPasswordErrorMessage(
        /* [in] */ Int32 attemptsRemaining);

    CARAPI_(AutoPtr<IDialog>) GetSimUnlockProgressDialog();

    CARAPI_(AutoPtr<IDialog>) GetSimRemainingAttemptsDialog(
        /* [in] */ Int32 remaining);

public:
    static final String TAG;

private:
    static const String LOG_TAG;
    static const Boolean DEBUG;

    AutoPtr<IProgressDialog> mSimUnlockProgressDialog;
    AutoPtr<CheckSimPin> mCheckSimPinThread;

    AutoPtr<IAlertDialog> mRemainingAttemptsDialog;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKERGUARDSIMPINVIEW_H__
