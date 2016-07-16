
#include "elastos/droid/systemui/keyguard/CKeyguardAccountView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardAccountView::MyRunnable::Run()
{
    if (mSuccess) {
        // clear out forgotten password
        mLockPatternUtils->SetPermanentlyLocked(FALSE);
        mLockPatternUtils->SetLockPatternEnabled(FALSE);
        mLockPatternUtils->SaveLockPattern(NULL);

        // launch the 'choose lock pattern' activity so
        // the user can pick a new one if they want to
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClassName(LOCK_PATTERN_PACKAGE, LOCK_PATTERN_CLASS);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mContext->StartActivityAsUser(intent,
                new UserHandle(mLockPatternUtils.getCurrentUser()));
        mCallback->ReportUnlockAttempt(TRUE);

        // dismiss keyguard
        mCallback->Dismiss(TRUE);
    }
    else {
        mSecurityMessageDisplay->SetMessage(R::string::kg_login_invalid_input, TRUE);
        mPassword->SetText("");
        mCallback->ReportUnlockAttempt(FALSE);
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
    //try {
    ECode ec = NOERROR;
    FAIL_GOTO(ec = mCallback->UserActivity(), ERROR)
    AutoPtr<IBundle> result;
    FAIL_GOTO(ec = future->GetResult((IBundle**)&result), ERROR)
    Boolean verified;
    FAIL_GOTO(ec = result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &verified), ERROR)
    FAIL_GOTO(ec = PostOnCheckPasswordResult(verified), ERROR)
    //} catch (OperationCanceledException e) {
    if (ec == (ECode)OperationCanceledException) {
        PostOnCheckPasswordResult(FALSE);
    }
    //} catch (IOException e) {
    if (ec == (ECode)IOException) {
        PostOnCheckPasswordResult(FALSE);
    }
    //} catch (AuthenticatorException e) {
    if (ec == (ECode)AuthenticatorException) {
        PostOnCheckPasswordResult(FALSE);
    }
    //} finally {
    AutoPtr<IRunnable> r = new MyRunnable2(mHost);
    return mLogin->Post(r);
    //}
}

const String CKeyguardAccountView::LOCK_PATTERN_PACKAGE("com.android.settings");
const String CKeyguardAccountView::LOCK_PATTERN_CLASS(LOCK_PATTERN_PACKAGE + String(".ChooseLockGeneric"));

CAR_OBJECT_IMPL(CKeyguardAccountView)

CAR_INTERFACE_IMPL_3(CKeyguardAccountView, LinearLayout, IKeyguardSecurityView,
        IViewOnClickListener, ITextWatcher)

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

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    return NOERROR;
}

ECode CKeyguardAccountView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    AutoPtr<IView> view;
    FindViewById(R::id::login, (IView**)&view);
    mLogin = IEditText::Probe(view);

    AutoPtr<ArrayOf<IInputFilter*> > array = ArrayOf<IInputFilter*>::Alloc(0);
    AutoPtr<IInputFilter> filter = LoginFilter::UsernameFilterGeneric();
    array->Set(0, filter);
    mLogin->SetFilters(array);
    mLogin->AddTextChangedListener(this);

    AutoPtr<IView> view2;
    FindViewById(R::id::password, (IView**)&view2);
    mPassword = IEditText::Probe(view2);
    mPassword->AddTextChangedListener(this);

    AutoPtr<IView> view3;
    FindViewById(R::id::ok, (IView**)&view3);
    mOk = IButton::Probe(view3);
    mOk->SetOnClickListener(this);

    mSecurityMessageDisplay = new KeyguardMessageArea::Helper(this);
    Reset();
    return NOERROR;
}

ECode CKeyguardAccountView::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    mCallback = callback;
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
    if (mCallback != null) {
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
    return mLogin->RequestFocus(direction, previouslyFocusedRect, &result);
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
    mLogin->SetText("");
    mPassword->SetText("");
    mLogin->RequestFocus();
    Boolean permLocked;
    mLockPatternUtils->IsPermanentlyLocked(&permLocked);
    mSecurityMessageDisplay->SetMessage(permLocked ? R::string::kg_login_too_many_attempts :
        R::string::kg_login_instructions, permLocked ? TRUE : FALSE);
    return NOERROR;
}

ECode CKeyguardAccountView::CleanUp()
{
    if (mCheckingDialog != NULL) {
        mCheckingDialog->Hide();
    }
    mCallback = NULL;
    mLockPatternUtils = NULL;
    return NOERROR;
}

ECode CKeyguardAccountView::OnClick(
    /* [in] */ IView* v)
{
    mCallback->UserActivity();
    if (v == mOk) {
        AsyncCheckPassword();
    }
    return NOERROR;
}

void CKeyguardAccountView::PostOnCheckPasswordResult(
    /* [in] */ Boolean success)
{
    // ensure this runs on UI thread
    AutoPtr<IRunnable> r = new MyRunnable(this, success);
    mLogin->Post(r);
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
    /* [in] */ coonst String& username)
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
    accountManager->GetAccountsByTypeAsUser(String("com.google"), handle, (ArrayOf<IAccount*>**)&accounts);

    // Try to figure out which account they meant if they
    // typed only the username (and not the domain), or got
    // the case wrong.

    AutoPtr<IAccount> bestAccount;
    Int32 bestScore = 0;

    for (Int32 i = 0; i < accounts.GetLength(); i+=) {
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
    mLogin->GetText((ICharSequence**)&loginText);
    String login;
    loginText->ToString(&login);

    AutoPtr<ICharSequence> passwordText;
    mPassword->GetText((ICharSequence**)&passwordText);
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

    accountManager->ConfirmCredentialsAsUser(account, options, NULL /* activity */,
            callback, NULL /* handler */, handle);
}

AutoPtr<IDialog> CKeyguardAccountView::GetProgressDialog()
{
    if (mCheckingDialog == NULL) {
        CProgressDialog::New(mContext, (IProgressDialog**)&mCheckingDialog);

        String str;
        mContext->GetString(R::string::kg_login_checking_password, &str);
        mCheckingDialog->SetMessage(str);
        mCheckingDialog->SetIndeterminate(TRUE);
        mCheckingDialog->SetCancelable(FASLE);
        AutoPtr<IWindow> window;
        IDialog::Probe(mCheckingDialog)->GetWindow((IWindow**)&window);
        window->SetType(IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG);
    }
    return mCheckingDialog;
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
