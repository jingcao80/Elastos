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

#include "elastos/droid/systemui/keyguard/CKeyguardAccountView.h"
#include "elastos/droid/systemui/keyguard/CKeyguardMessageArea.h"
#include "elastos/droid/text/LoginFilter.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::EIID_IAccountManagerCallback;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::UsernameFilterGeneric;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(CKeyguardAccountView::InnerListener, Object, IViewOnClickListener, ITextWatcher)

CKeyguardAccountView::InnerListener::InnerListener(
    /* [in] */ CKeyguardAccountView* host)
    : mHost(host)
{}

ECode CKeyguardAccountView::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

ECode CKeyguardAccountView::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CKeyguardAccountView::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode CKeyguardAccountView::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}


ECode CKeyguardAccountView::MyRunnable::Run()
{
    if (mSuccess) {
        // clear out forgotten password
        mHost->mLockPatternUtils->SetPermanentlyLocked(FALSE);
        mHost->mLockPatternUtils->SetLockPatternEnabled(FALSE);
        mHost->mLockPatternUtils->SaveLockPattern(NULL);

        // launch the 'choose lock pattern' activity so
        // the user can pick a new one if they want to
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClassName(LOCK_PATTERN_PACKAGE, LOCK_PATTERN_CLASS);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        Int32 user;
        mHost->mLockPatternUtils->GetCurrentUser(&user);
        AutoPtr<IUserHandle> handle;
        CUserHandle::New(user, (IUserHandle**)&handle);
        mHost->mContext->StartActivityAsUser(intent, handle);
        mHost->mCallback->ReportUnlockAttempt(TRUE);

        // dismiss keyguard
        mHost->mCallback->Dismiss(TRUE);
    }
    else {
        mHost->mSecurityMessageDisplay->SetMessage(R::string::kg_login_invalid_input, TRUE);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
        ITextView::Probe(mHost->mPassword)->SetText(cchar);
        mHost->mCallback->ReportUnlockAttempt(FALSE);
    }
    return NOERROR;
}

ECode CKeyguardAccountView::MyRunnable2::Run()
{
    AutoPtr<IDialog> dialog = mHost->GetProgressDialog();
    return dialog->Hide();
}


CAR_INTERFACE_IMPL(CKeyguardAccountView::MyAccountManagerCallback, Object, IAccountManagerCallback)

ECode CKeyguardAccountView::MyAccountManagerCallback::Run(
    /* [in] */ IAccountManagerFuture* future)
{
    //try
    ECode ec = NOERROR;
    {
        FAIL_GOTO(ec = mHost->mCallback->UserActivity(), ERROR)
        AutoPtr<IInterface> obj;
        FAIL_GOTO(ec = future->GetResult((IInterface**)&obj), ERROR)
        AutoPtr<IBundle> result = IBundle::Probe(obj);
        Boolean verified;
        FAIL_GOTO(ec = result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &verified), ERROR)
        mHost->PostOnCheckPasswordResult(verified);
    }
    //} catch (OperationCanceledException e) {
ERROR:
    if (ec == (ECode)E_OPERATION_CANCELED_EXCEPTION) {
        mHost->PostOnCheckPasswordResult(FALSE);
    }
    //} catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        mHost->PostOnCheckPasswordResult(FALSE);
    }
    //} catch (AuthenticatorException e) {
    if (ec == (ECode)E_AUTHENTICATOR_EXCEPTION) {
        mHost->PostOnCheckPasswordResult(FALSE);
    }
    //} finally {
    AutoPtr<IRunnable> r = new MyRunnable2(mHost);
    Boolean res;
    return IView::Probe(mHost->mLogin)->Post(r, &res);
    //}
}

const String CKeyguardAccountView::LOCK_PATTERN_PACKAGE("Elastos.Droid.Settings");
const String CKeyguardAccountView::LOCK_PATTERN_CLASS(LOCK_PATTERN_PACKAGE + String(".CChooseLockGeneric"));

CAR_OBJECT_IMPL(CKeyguardAccountView)

CAR_INTERFACE_IMPL(CKeyguardAccountView, LinearLayout, IKeyguardSecurityView)

CKeyguardAccountView::CKeyguardAccountView()
    : mEnableFallback(FALSE)
{
}

ECode CKeyguardAccountView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardAccountView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardAccountView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<IContext> _context;
    GetContext((IContext**)&_context);
    return CLockPatternUtils::New(_context, (ILockPatternUtils**)&mLockPatternUtils);
}

ECode CKeyguardAccountView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    AutoPtr<InnerListener> listener = new InnerListener(this);

    AutoPtr<IView> view;
    FindViewById(R::id::login, (IView**)&view);
    mLogin = IEditText::Probe(view);

    AutoPtr<ArrayOf<IInputFilter*> > array = ArrayOf<IInputFilter*>::Alloc(0);
    AutoPtr<IInputFilter> filter = new UsernameFilterGeneric();
    array->Set(0, filter);
    ITextView::Probe(mLogin)->SetFilters(array);
    ITextView::Probe(mLogin)->AddTextChangedListener(listener);

    AutoPtr<IView> view2;
    FindViewById(R::id::password, (IView**)&view2);
    mPassword = IEditText::Probe(view2);
    ITextView::Probe(mPassword)->AddTextChangedListener(listener);

    AutoPtr<IView> view3;
    FindViewById(R::id::ok, (IView**)&view3);
    mOk = IButton::Probe(view3);
    IView::Probe(mOk)->SetOnClickListener(listener);

    AutoPtr<CKeyguardMessageArea::Helper> tmp =
            new CKeyguardMessageArea::Helper();
    tmp->constructor((IView*)this);
    mSecurityMessageDisplay = ISecurityMessageDisplay::Probe(tmp);
    Reset();
    return NOERROR;
}

ECode CKeyguardAccountView::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CKeyguardAccountView::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    return NOERROR;
}

ECode CKeyguardAccountView::GetCallback(
    /* [out] */ IKeyguardSecurityCallback** callback)
{
    VALIDATE_NOT_NULL(callback)

    *callback = mCallback;
    REFCOUNT_ADD(*callback)
    return NOERROR;
}

ECode CKeyguardAccountView::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

ECode CKeyguardAccountView::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CKeyguardAccountView::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    if (mCallback != NULL) {
        mCallback->UserActivity();
    }
    return NOERROR;
}

ECode CKeyguardAccountView::OnRequestFocusInDescendants(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // send focus to the login field
    return IView::Probe(mLogin)->RequestFocus(direction, previouslyFocusedRect, result);
}

ECode CKeyguardAccountView::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

ECode CKeyguardAccountView::Reset()
{
    // start fresh
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));

    ITextView::Probe(mLogin)->SetText(cchar);
    ITextView::Probe(mPassword)->SetText(cchar);
    Boolean res;
    IView::Probe(mLogin)->RequestFocus(&res);
    Boolean permLocked;
    mLockPatternUtils->IsPermanentlyLocked(&permLocked);
    return mSecurityMessageDisplay->SetMessage(permLocked ? R::string::kg_login_too_many_attempts :
        R::string::kg_login_instructions, permLocked ? TRUE : FALSE);
}

ECode CKeyguardAccountView::CleanUp()
{
    if (mCheckingDialog != NULL) {
        IDialog::Probe(mCheckingDialog)->Hide();
    }
    mCallback = NULL;
    mLockPatternUtils = NULL;
    return NOERROR;
}

ECode CKeyguardAccountView::OnClick(
    /* [in] */ IView* v)
{
    mCallback->UserActivity();
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mOk)) {
        AsyncCheckPassword();
    }
    return NOERROR;
}

void CKeyguardAccountView::PostOnCheckPasswordResult(
    /* [in] */ Boolean success)
{
    // ensure this runs on UI thread
    AutoPtr<IRunnable> r = new MyRunnable(this, success);
    Boolean res;
    IView::Probe(mLogin)->Post(r, &res);
}

ECode CKeyguardAccountView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    event->GetAction(&action);
    Int32 code;
    event->GetKeyCode(&code);
    if (action == IKeyEvent::ACTION_DOWN
            && code == IKeyEvent::KEYCODE_BACK) {
        Boolean res;
        mLockPatternUtils->IsPermanentlyLocked(&res);
        if (res) {
            mCallback->Dismiss(FALSE);
        }
        else {
            // TODO: mCallback.forgotPattern(false);
        }
        *result = TRUE;
        return NOERROR;
    }
    return LinearLayout::DispatchKeyEvent(event, result);
}

AutoPtr<IAccount> CKeyguardAccountView::FindIntendedAccount(
    /* [in] */ const String& username)
{
    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    AutoPtr<IUserHandle> handle;
    CUserHandle::New((IUserHandle**)&handle);
    AutoPtr<IAccountManagerHelper> helper;
    CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper);
    AutoPtr<IAccountManager> accountManager;
    helper->Get(mContext, (IAccountManager**)&accountManager);
    AutoPtr<ArrayOf<IAccount*> > accounts;
    accountManager->GetAccountsByTypeAsUser(String("com.google"),
            handle, (ArrayOf<IAccount*>**)&accounts);

    // Try to figure out which account they meant if they
    // typed only the username (and not the domain), or got
    // the case wrong.

    AutoPtr<IAccount> bestAccount;
    Int32 bestScore = 0;

    for (Int32 i = 0; i < accounts->GetLength(); i++) {
        AutoPtr<IAccount> a = (*accounts)[i];

        Int32 score = 0;
        String name;
        a->GetName(&name);
        if (username.Equals(name)) {
            score = 4;
        }
        else if (username.EqualsIgnoreCase(name)) {
            score = 3;
        }
        else if (username.IndexOf('@') < 0) {
            Int32 i = name.IndexOf('@');
            if (i >= 0) {
                String aUsername = name.Substring(0, i);
                if (username.Equals(aUsername)) {
                    score = 2;
                }
                else if (username.EqualsIgnoreCase(aUsername)) {
                    score = 1;
                }
            }
        }
        if (score > bestScore) {
            bestAccount = a;
            bestScore = score;
        }
        else if (score == bestScore) {
            bestAccount = NULL;
        }
    }

    return bestAccount;
}

void CKeyguardAccountView::AsyncCheckPassword()
{
    mCallback->UserActivity();

    AutoPtr<ICharSequence> loginText;
    ITextView::Probe(mLogin)->GetText((ICharSequence**)&loginText);
    String login;
    loginText->ToString(&login);

    AutoPtr<ICharSequence> passwordText;
    ITextView::Probe(mPassword)->GetText((ICharSequence**)&passwordText);
    String password;
    passwordText->ToString(&password);

    AutoPtr<IAccount> account = FindIntendedAccount(login);
    if (account == NULL) {
        PostOnCheckPasswordResult(FALSE);
        return;
    }

    AutoPtr<IDialog> dialog = GetProgressDialog();
    dialog->Show();
    AutoPtr<IBundle> options;
    CBundle::New((IBundle**)&options);
    options->PutString(IAccountManager::KEY_PASSWORD, password);
    AutoPtr<IAccountManagerCallback> callback = new MyAccountManagerCallback(this);

    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    AutoPtr<IUserHandle> handle;
    CUserHandle::New((IUserHandle**)&handle);
    AutoPtr<IAccountManagerHelper> helper;
    CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper);
    AutoPtr<IAccountManager> accountManager;
    helper->Get(mContext, (IAccountManager**)&accountManager);

    AutoPtr<IAccountManagerFuture> tmp;
    accountManager->ConfirmCredentialsAsUser(account, options, NULL /* activity */,
           callback, NULL /* handler */, handle, (IAccountManagerFuture**)&tmp);
}

AutoPtr<IDialog> CKeyguardAccountView::GetProgressDialog()
{
    if (mCheckingDialog == NULL) {
        CProgressDialog::New(mContext, (IProgressDialog**)&mCheckingDialog);

        String str;
        mContext->GetString(R::string::kg_login_checking_password, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        IAlertDialog::Probe(mCheckingDialog)->SetMessage(cchar);
        mCheckingDialog->SetIndeterminate(TRUE);
        IDialog::Probe(mCheckingDialog)->SetCancelable(FALSE);
        AutoPtr<IWindow> window;
        IDialog::Probe(mCheckingDialog)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    return IDialog::Probe(mCheckingDialog);
}

ECode CKeyguardAccountView::OnPause()
{
    return NOERROR;
}

ECode CKeyguardAccountView::OnResume(
    /* [in] */ Int32 reason)
{
    Reset();
    return NOERROR;
}

ECode CKeyguardAccountView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardAccountView::ShowBouncer(
    /* [in] */ Int32 duration)
{
    return NOERROR;
}

ECode CKeyguardAccountView::HideBouncer(
    /* [in] */ Int32 duration)
{
    return NOERROR;
}

ECode CKeyguardAccountView::StartAppearAnimation()
{
    // TODO.
    return NOERROR;
}

ECode CKeyguardAccountView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
