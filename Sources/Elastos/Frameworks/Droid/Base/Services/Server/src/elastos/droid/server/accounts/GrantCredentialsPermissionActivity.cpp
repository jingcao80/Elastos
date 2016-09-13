
#include "elastos/droid/R.h"
#include "elastos/droid/server/accounts/GrantCredentialsPermissionActivity.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::EIID_IAccount;
using Elastos::Droid::Accounts::EIID_IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountAuthenticatorResponse;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

//=============================================================================
// GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::OnCreateAccountManagerCallbackRunnable
//=============================================================================
GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::OnCreateAccountManagerCallbackRunnable::OnCreateAccountManagerCallbackRunnable(
    /* [in] */ ICharSequence* authTokenLabel,
    /* [in] */ OnCreateAccountManagerCallback* host)
    : mAuthTokenLabel(authTokenLabel)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(
        GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::OnCreateAccountManagerCallbackRunnable,
        Object, IRunnable);

ECode GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::
OnCreateAccountManagerCallbackRunnable::Run()
{
    Boolean isFinishing;
    mHost->mHost->IsFinishing(&isFinishing);
    if (!isFinishing) {
        mHost->mAuthTokenTypeView->SetText(mAuthTokenLabel);
        IView::Probe(mHost->mAuthTokenTypeView)->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}

//=============================================================================
// GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback
//=============================================================================
GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::OnCreateAccountManagerCallback(
    /* [in] */ ITextView* textView,
    /* [in] */ GrantCredentialsPermissionActivity* host)
    : mAuthTokenTypeView(textView)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback, Object,
        IAccountManagerCallback);

ECode GrantCredentialsPermissionActivity::OnCreateAccountManagerCallback::Run(
    /* [in] */ IAccountManagerFuture* future)
{
    // try {
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(future->GetResult((IInterface**)&obj));
    ICharSequence* authTokenLabel = ICharSequence::Probe(obj);
    if (!TextUtils::IsEmpty(authTokenLabel)) {
        AutoPtr<IRunnable> runnable = new OnCreateAccountManagerCallbackRunnable(
            authTokenLabel, this);
        return mHost->RunOnUiThread(runnable);
    }
    return NOERROR;
    // } catch (OperationCanceledException e) {
    // } catch (IOException e) {
    // } catch (AuthenticatorException e) {
    // }
}

//=============================================================================
// GrantCredentialsPermissionActivity::InnerListener
//=============================================================================
CAR_INTERFACE_IMPL(GrantCredentialsPermissionActivity::InnerListener, Object, IViewOnClickListener)

GrantCredentialsPermissionActivity::InnerListener::InnerListener(
    /* [in] */ GrantCredentialsPermissionActivity* host)
{}

ECode GrantCredentialsPermissionActivity::InnerListener::OnClick(
    /* [in] */ IView* vs)
{
    return mHost->OnClick(vs);
}

//=============================================================================
// GrantCredentialsPermissionActivity
//=============================================================================
const String GrantCredentialsPermissionActivity::EXTRAS_ACCOUNT("account");
const String GrantCredentialsPermissionActivity::EXTRAS_AUTH_TOKEN_LABEL("authTokenLabel");
const String GrantCredentialsPermissionActivity::EXTRAS_AUTH_TOKEN_TYPE("authTokenType");
const String GrantCredentialsPermissionActivity::EXTRAS_RESPONSE("response");
const String GrantCredentialsPermissionActivity::EXTRAS_ACCOUNT_TYPE_LABEL("accountTypeLabel");
const String GrantCredentialsPermissionActivity::EXTRAS_PACKAGES("application");
const String GrantCredentialsPermissionActivity::EXTRAS_REQUESTING_UID("uid");

GrantCredentialsPermissionActivity::GrantCredentialsPermissionActivity()
    : mUid(0)
{}

ECode GrantCredentialsPermissionActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    ASSERT_SUCCEEDED(SetContentView(R::layout::grant_credentials_permission));
    AutoPtr<ICharSequence> csq;
    CString::New(StringUtils::ToString(R::string::grant_permissions_header_text),
            (ICharSequence**)&csq);
    SetTitle(csq);

    AutoPtr<IInterface> service;
    ASSERT_SUCCEEDED(GetSystemService(IContext::LAYOUT_INFLATER_SERVICE,
            (IInterface**)&service));
    mInflater = ILayoutInflater::Probe(service);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras == NULL) {
        // we were somehow started with bad parameters. abort the activity.
        SetResult(IActivity::RESULT_CANCELED);
        return Finish();
    }

    // Grant 'account'/'type' to mUID
    AutoPtr<IParcelable> parcelable;
    extras->GetParcelable(EXTRAS_ACCOUNT, (IParcelable**)&parcelable);
    mAccount = (IAccount*)parcelable->Probe(Elastos::Droid::Accounts::EIID_IAccount);
    extras->GetString(EXTRAS_AUTH_TOKEN_TYPE, &mAuthTokenType);
    extras->GetInt32(EXTRAS_REQUESTING_UID, &mUid);
    AutoPtr<IPackageManager> pm;
    ASSERT_SUCCEEDED(GetPackageManager((IPackageManager**)&pm));
    AutoPtr< ArrayOf<String> > packages;
    pm->GetPackagesForUid(mUid, (ArrayOf<String>**)&packages);

    if (mAccount == NULL || mAuthTokenType.IsNull() || packages == NULL) {
        // we were somehow started with bad parameters. abort the activity.
        SetResult(IActivity::RESULT_CANCELED);
        return Finish();
    }

    String accountTypeLabel;
    // try {
    ECode ec = GetAccountLabel(mAccount, &accountTypeLabel);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        SetResult(IActivity::RESULT_CANCELED);
        return Finish();
    }
    // } catch (IllegalArgumentException e) {
    //     // label or resource was missing. abort the activity.
    //     setResult(Activity.RESULT_CANCELED);
    //     finish();
    //     return;
    // }

    AutoPtr<IView> view;
    ASSERT_SUCCEEDED(FindViewById(R::id::authtoken_type, (IView**)&view));
    AutoPtr<ITextView> authTokenTypeView = ITextView::Probe(view);
    view->SetVisibility(IView::GONE);

    AutoPtr<IAccountManagerCallback> callback
            = (IAccountManagerCallback*)new OnCreateAccountManagerCallback(authTokenTypeView, this);
    AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
    AutoPtr<IAccountManagerHelper> helper;
    ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton(
            (IAccountManagerHelper**)&helper));
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(helper->Get(ctx, (IAccountManager**)&accountManager));
    String type;
    mAccount->GetType(&type);
    AutoPtr<IAccountManagerFuture> future;
    accountManager->GetAuthTokenLabel(type, mAuthTokenType, callback,
            NULL, (IAccountManagerFuture**)&future);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    view = NULL;
    ASSERT_SUCCEEDED(FindViewById(R::id::allow_button, (IView**)&view));
    view->SetOnClickListener(listener);
    view = NULL;
    ASSERT_SUCCEEDED(FindViewById(R::id::deny_button, (IView**)&view));
    view->SetOnClickListener(listener);

    view = NULL;
    ASSERT_SUCCEEDED(FindViewById(R::id::packages_list, (IView**)&view));
    AutoPtr<IViewGroup> packagesListView = IViewGroup::Probe(view);
    for (Int32 i = 0; i < packages->GetLength(); ++i) {
        String pkg = (*packages)[i];
        String packageLabel;
        // try {
        AutoPtr<IApplicationInfo> appInfo;
        if (FAILED(pm->GetApplicationInfo(pkg, 0, (IApplicationInfo**)&appInfo))) {
            packageLabel = pkg;
            IViewGroup::Probe(packagesListView)->AddView(NewPackageView(packageLabel));
            continue;
        }
        AutoPtr<ICharSequence> csq;
        if (FAILED(pm->GetApplicationLabel(appInfo, (ICharSequence**)&csq))) {
            packageLabel = pkg;
            IViewGroup::Probe(packagesListView)->AddView(NewPackageView(packageLabel));
            continue;
        }
        csq->ToString(&packageLabel);
        // } catch (PackageManager.NameNotFoundException e) {
        //     packageLabel = pkg;
        // }
        IViewGroup::Probe(packagesListView)->AddView(NewPackageView(packageLabel));
    }

    String name;
    mAccount->GetName(&name);
    CString::New(name, (ICharSequence**)&csq);
    view = NULL;
    ASSERT_SUCCEEDED(FindViewById(R::id::account_name, (IView**)&view));
    ITextView::Probe(view)->SetText(csq);

    view = NULL;
    ASSERT_SUCCEEDED(FindViewById(R::id::account_type, (IView**)&view));
    csq = NULL;
    CString::New(accountTypeLabel, (ICharSequence**)&csq);
    ITextView::Probe(view)->SetText(csq);

    return NOERROR;
}

ECode GrantCredentialsPermissionActivity::GetAccountLabel(
    /* [in] */ IAccount* account,
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);

    AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
    AutoPtr<IAccountManagerHelper> helper;
    ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton(
            (IAccountManagerHelper**)&helper));
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(helper->Get(ctx, (IAccountManager**)&accountManager));
    AutoPtr< ArrayOf<IAuthenticatorDescription*> > authenticatorTypes;
    accountManager->GetAuthenticatorTypes(
            (ArrayOf<IAuthenticatorDescription*>**)&authenticatorTypes);
    for (Int32 i = 0; i < authenticatorTypes->GetLength(); ++i) {
        AutoPtr<IAuthenticatorDescription> desc = (*authenticatorTypes)[i];
        String descType, accountType;
        desc->GetType(&descType);
        account->GetType(&accountType);
        if (descType.Equals(accountType)) {
            // try {
            AutoPtr<IContext> ctx;
            String packageName;
            desc->GetPackageName(&packageName);
            ECode ec = CreatePackageContext(packageName, 0, (IContext**)&ctx);
            if (FAILED(ec)) {
                *label = accountType;
                return ec;
            }
            Int32 labelId;
            desc->GetLabelId(&labelId);
            ec = ctx->GetString(labelId, label);
            if (FAILED(ec)) {
                *label = accountType;
            }
            return ec;
            // return createPackageContext().getString();
            // } catch (PackageManager.NameNotFoundException e) {
            //     return account.type;
            // } catch (Resources.NotFoundException e) {
            //     return account.type;
            // }
        }
    }
    return account->GetType(label);
}

AutoPtr<IView> GrantCredentialsPermissionActivity::NewPackageView(
    /* [in] */ const String& packageLabel)
{
    AutoPtr<IView> view, textView;
    ASSERT_SUCCEEDED(mInflater->Inflate(R::layout::permissions_package_list_item, NULL, (IView**)&view));
    ASSERT_SUCCEEDED(view->FindViewById(R::id::package_label, (IView**)&textView));
    AutoPtr<ICharSequence> csq;
    CString::New(packageLabel, (ICharSequence**)&csq);
    ITextView::Probe(textView)->SetText(csq);
    return view;
}

ECode GrantCredentialsPermissionActivity::OnClick(
    /* [in] */ IView* vs)
{
    Int32 id;
    vs->GetId(&id);
    switch (id) {
        case R::id::allow_button: {
            AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
            AutoPtr<IAccountManagerHelper> helper;
            ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton(
                    (IAccountManagerHelper**)&helper));
            AutoPtr<IAccountManager> accountManager;
            ASSERT_SUCCEEDED(helper->Get(ctx, (IAccountManager**)&accountManager));
            accountManager->UpdateAppPermission(mAccount, mAuthTokenType, mUid, TRUE);
            AutoPtr<IIntent> result;
            ASSERT_SUCCEEDED(CIntent::New((IIntent**)&result));
            result->PutBooleanExtra(String("retry"), TRUE);
            SetResult(RESULT_OK, result);
            AutoPtr<IBundle> extras;
            result->GetExtras((IBundle**)&extras);
            SetAccountAuthenticatorResult(extras);
        }
        break;
        case R::id::deny_button: {
            AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
            AutoPtr<IAccountManagerHelper> helper;
            ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton(
                    (IAccountManagerHelper**)&helper));
            AutoPtr<IAccountManager> accountManager;
            ASSERT_SUCCEEDED(helper->Get(ctx, (IAccountManager**)&accountManager));
            accountManager->UpdateAppPermission(mAccount, mAuthTokenType, mUid, FALSE);
            SetResult(RESULT_CANCELED);
        }
        break;
    }
    return Finish();
}

void GrantCredentialsPermissionActivity::SetAccountAuthenticatorResult(
    /* [in] */ IBundle* result)
{
    mResultBundle = result;
}

ECode GrantCredentialsPermissionActivity::Finish()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcel;
    ASSERT_SUCCEEDED(intent->GetParcelableExtra(EXTRAS_RESPONSE, (IParcelable**)&parcel));
    AutoPtr<IAccountAuthenticatorResponse> response =
            (IAccountAuthenticatorResponse*)parcel->Probe(
                    Elastos::Droid::Accounts::EIID_IAccountAuthenticatorResponse);
    if (response != NULL) {
        // send the result bundle back if set, otherwise send an error.
        if (mResultBundle != NULL) {
            return response->OnResult(mResultBundle);
        }
        else {
            return response->OnError(IAccountManager::ERROR_CODE_CANCELED,
                    String("canceled"));
        }
    }
    return Activity::Finish();
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
