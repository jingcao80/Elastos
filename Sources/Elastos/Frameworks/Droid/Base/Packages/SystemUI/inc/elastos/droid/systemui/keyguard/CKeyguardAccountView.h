
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDACCOUNTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDACCOUNTVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardAccountView.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IButton;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * When the user forgets their password a bunch of times, we fall back on their
 * account's login/password to unlock the phone (and reset their lock pattern).
 */
CarClass(CKeyguardAccountView)
    , public LinearLayout
    , public IKeyguardSecurityView
{
private:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public ITextWatcher
    {
    public:
        TO_STRING_IMPL("CKeyguardAccountView::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CKeyguardAccountView* host);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardAccountView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardAccountView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardAccountView* host,
            /* [in] */ Boolean success)
            : mHost(host)
            , mSuccess(success)
        {}

        CARAPI Run();

    private:
        CKeyguardAccountView* mHost;
        Boolean mSuccess;
    };

    class MyAccountManagerCallback
        : public Object
        , public IAccountManagerCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardAccountView::MyAccountManagerCallback")

        CAR_INTERFACE_DECL()

        MyAccountManagerCallback(
            /* [in] */ CKeyguardAccountView* host)
            : mHost(host)
        {}

        CARAPI Run(
            /* [in] */ IAccountManagerFuture* future);

    private:
        CKeyguardAccountView* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardAccountView::MyRunnable2")

        MyRunnable2(
            /* [in] */ CKeyguardAccountView* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CKeyguardAccountView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardAccountView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetKeyguardCallback(
        /* [in] */ IKeyguardSecurityCallback* callback);

    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    CARAPI GetCallback(
        /* [out] */ IKeyguardSecurityCallback** callback);

    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    CARAPI Reset();

    /** {@inheritDoc} */
    CARAPI CleanUp();

    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI HideBouncer(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) PostOnCheckPasswordResult(
        /* [in] */ Boolean success);

    /**
     * Given the string the user entered in the 'username' field, find
     * the stored account that they probably intended.  Prefer, in order:
     *
     *   - an exact match for what was typed, or
     *   - a case-insensitive match for what was typed, or
     *   - if they didn't include a domain, an exact match of the username, or
     *   - if they didn't include a domain, a case-insensitive
     *     match of the username.
     *
     * If there is a tie for the best match, choose neither --
     * the user needs to be more specific.
     *
     * @return an account name from the database, or null if we can't
     * find a single best match.
     */
    CARAPI_(AutoPtr<IAccount>) FindIntendedAccount(
        /* [in] */ const String& username);

    CARAPI_(void) AsyncCheckPassword();

    CARAPI_(AutoPtr<IDialog>) GetProgressDialog();

private:
    static const String LOCK_PATTERN_PACKAGE;
    static const String LOCK_PATTERN_CLASS;

    AutoPtr<IKeyguardSecurityCallback> mCallback;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IEditText> mLogin;
    AutoPtr<IEditText> mPassword;
    AutoPtr<IButton> mOk;
    Boolean mEnableFallback;
    AutoPtr<ISecurityMessageDisplay> mSecurityMessageDisplay;

    /**
     * Shown while making asynchronous check of password.
     */
    AutoPtr<IProgressDialog> mCheckingDialog;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDACCOUNTVIEW_H__
