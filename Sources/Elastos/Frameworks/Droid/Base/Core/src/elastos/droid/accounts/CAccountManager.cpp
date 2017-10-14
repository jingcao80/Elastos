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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/accounts/AccountManagerResponse.h"
#include "elastos/droid/accounts/CChooseResponse.h"
#include "elastos/droid/accounts/CAccountManagerAmsResponse.h"
#include "elastos/droid/accounts/CAccountManagerFutureResponse.h"
#include "elastos/droid/accounts/CAccount.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::R;
using Elastos::Core::CInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String CAccountManager::TAG("AccountManager");

//===============================================================
// CAccountManager::AmsTask
//===============================================================

CAR_INTERFACE_IMPL(CAccountManager::AmsTask, FutureTask, IAccountManagerFuture)

CAccountManager::AmsTask::AmsTask(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host)
    : mHandler(handler)
    , mCallback(cb)
    , mActivity(activity)
    , mHost(host)
{
    // super(new Callable<Bundle>() {
    //     public Bundle call() throws Exception {
    //         throw new IllegalStateException("this should never be called");
    //     }
    // });
    ASSERT_SUCCEEDED(CAccountManagerAmsResponse::New((Handle32)this,
            (IAccountManagerResponse**)&mResponse));
}

ECode CAccountManager::AmsTask::Start(
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    //try {
    ECode ec = DoWork();
    if (FAILED(ec)) {
        *accountManagerFuture = NULL;
        // SetException(e);
        return ec;
    }
    // } catch (RemoteException e) {
    //     setException(e);
    // }
    *accountManagerFuture = this;
    REFCOUNT_ADD(*accountManagerFuture);
    return NOERROR;
}

void CAccountManager::AmsTask::Set(
    /* [in] */ IInterface* bundle)
{
    //TODO: somehow a null is being set as the result of the Future. Log this
    // case to help debug where this is occurring. When this bug is fixed this
    // condition statement should be removed.
    if (bundle == NULL) {
        Slogger::E(TAG, "the bundle must not be null");
    }
    FutureTask::Set(bundle);
}

ECode CAccountManager::AmsTask::InternalGetResult(
    /* [in] */ IInteger64* timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isDone = FALSE;
    IsDone(&isDone);
    if (!isDone) {
        FAIL_RETURN(mHost->EnsureNotOnMainThread());
    }
    if (timeout == NULL) {
        AutoPtr<IInterface> p;
        Get((IInterface**)&p);
        *result = IBundle::Probe(p);
        REFCOUNT_ADD(*result)
        Boolean value = FALSE;
        Cancel(TRUE /* interrupt if running */, &value);
        return NOERROR;
    }
    else {
        Int64 tm = 0;
        timeout->GetValue(&tm);
        AutoPtr<IInterface> p;
        Get(tm, unit, (IInterface**)&p);
        *result = IBundle::Probe(p);
        REFCOUNT_ADD(*result)
        Boolean value = FALSE;
        Cancel(TRUE /* interrupt if running */, &value);
        return NOERROR;
    }
    Boolean value = FALSE;
    Cancel(TRUE /* interrupt if running */, &value);
    *result = NULL;
    return E_OPERATION_CANCELED_EXCEPTION;
}

ECode CAccountManager::AmsTask::GetResult(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IBundle> bundle;
    FAIL_RETURN(InternalGetResult(NULL, NULL, (IBundle**)&bundle))
    *result = bundle.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountManager::AmsTask::GetResult(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IInteger64> integer64;
    CInteger64::New(timeout, (IInteger64**)&integer64);
    AutoPtr<IBundle> bundle;
    InternalGetResult(integer64, unit, (IBundle**)&bundle);
    *result = bundle.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CAccountManager::AmsTask::Done()
{
    if (mCallback != NULL) {
        mHost->PostToHandler(mHandler, mCallback, this);
    }
}

ECode CAccountManager::AmsTask::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return FutureTask::Cancel(mayInterruptIfRunning, result);
}

ECode CAccountManager::AmsTask::IsCancelled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return FutureTask::IsCancelled(result);
}

ECode CAccountManager::AmsTask::IsDone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return FutureTask::IsDone(result);
}

//===============================================================
// CAccountManager::BaseFutureTask
//===============================================================

CAccountManager::BaseFutureTask::BaseFutureTask(
    /* [in] */ IHandler* handler,
    /* [in] */ CAccountManager* host)
    : mHandler(handler)
    , mHost(host)
{
    // super(new Callable<T>() {
    //     public T call() throws Exception {
    //         throw new IllegalStateException("this should never be called");
    //     }
    // });
    ASSERT_SUCCEEDED(CAccountManagerFutureResponse::New((Handle32)this,
            (IAccountManagerResponse**)&mResponse));
}

void CAccountManager::BaseFutureTask::PostRunnableToHandler(
    /* [in] */ IRunnable* runnable)
{
    AutoPtr<IHandler> handler = (mHandler == NULL) ? mHost->mMainHandler : mHandler;
    Boolean result;
    handler->Post(runnable, &result);
}

ECode CAccountManager::BaseFutureTask::StartTask()
{
    //try {
    return DoWork();
    // } catch (RemoteException e) {
    //     setException(e);
    // }
}

//===============================================================
// CAccountManager::Future2Task::CallbackRunnable::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::Future2Task::CallbackRunnable, Object, IRunnable);

CAccountManager::Future2Task::CallbackRunnable::CallbackRunnable(
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ Future2Task* host)
    : mCallback(cb)
    , mHost(host)
{}

ECode CAccountManager::Future2Task::CallbackRunnable::Run()
{
    return mCallback->Run(mHost);
}

//===============================================================
// CAccountManager::Future2Task::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::Future2Task, BaseFutureTask, IAccountManagerFuture)

CAccountManager::Future2Task::Future2Task(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host)
    : BaseFutureTask(handler, host)
    , mCallback(cb)
{}

void CAccountManager::Future2Task::Done()
{
    if (mCallback != NULL) {
        AutoPtr<IRunnable> action = new CallbackRunnable(mCallback, this);
        PostRunnableToHandler(action);
    }
}

ECode CAccountManager::Future2Task::Start(
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    StartTask();
    *accountManagerFuture = this;
    REFCOUNT_ADD(*accountManagerFuture);
    return NOERROR;
}

ECode CAccountManager::Future2Task::InternalGetResult(
    /* [in] */ IInteger64* timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    Boolean isDone;
    IsDone(&isDone);
    if (!isDone) {
        FAIL_RETURN(mHost->EnsureNotOnMainThread());
    }
    //try {
    // if (timeout == null) {
    //     return get();
    // } else {
    //     return get(timeout, unit);
    // }
    // } catch (InterruptedException e) {
    //     // fall through and cancel
    // } catch (TimeoutException e) {
    //     // fall through and cancel
    // } catch (CancellationException e) {
    //     // fall through and cancel
    // } catch (ExecutionException e) {
    //     final Throwable cause = e.getCause();
    //     if (cause instanceof IOException) {
    //         throw (IOException) cause;
    //     } else if (cause instanceof UnsupportedOperationException) {
    //         throw new AuthenticatorException(cause);
    //     } else if (cause instanceof AuthenticatorException) {
    //         throw (AuthenticatorException) cause;
    //     } else if (cause instanceof RuntimeException) {
    //         throw (RuntimeException) cause;
    //     } else if (cause instanceof Error) {
    //         throw (Error) cause;
    //     } else {
    //         throw new IllegalStateException(cause);
    //     }
    // } finally {
    //     cancel(true /* interrupt if running */);
    // }
    //throw new OperationCanceledException();
    assert(0);

    return E_NOT_IMPLEMENTED;
}

ECode CAccountManager::Future2Task::GetResult(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    return InternalGetResult(NULL, NULL, result);
}

ECode CAccountManager::Future2Task::GetResult(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInteger64> integer64;
    CInteger64::New(timeout, (IInteger64**)&integer64);
    return InternalGetResult(integer64, NULL, result);
}

ECode CAccountManager::Future2Task::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAccountManager::Future2Task::IsCancelled(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAccountManager::Future2Task::IsDone(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

//===============================================================
// CAccountManager::GetAuthTokenByTypeAndFeaturesTask::GetAccountsCallback::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::GetAuthTokenByTypeAndFeaturesTask::GetAccountsCallback, Object,
        IAccountManagerCallback);

CAccountManager::GetAuthTokenByTypeAndFeaturesTask::GetAccountsCallback::GetAccountsCallback(
    /* [in] */ GetAuthTokenByTypeAndFeaturesTask* host)
    : mHost(host)
{}

ECode CAccountManager::GetAuthTokenByTypeAndFeaturesTask::GetAccountsCallback::Run(
    /* [in] */ IAccountManagerFuture* future)
{
    AutoPtr<ArrayOf<IAccount*> > accounts;
    assert(0 && "TODO"); // arrayof cannt convert to interface
    // FAIL_RETURN(future->GetResult((IInterface**)&accounts));

    mHost->mNumAccounts = accounts->GetLength();

    if (accounts->GetLength() == 0) {
        if (mHost->mActivity != NULL) {
            // no accounts, add one now. pretend that the user directly
            // made this request
            mHost->mFuture = NULL;
            FAIL_RETURN(mHost->mHost->AddAccount(mHost->mAccountType, mHost->mAuthTokenType,
                    mHost->mFeatures, mHost->mAddAccountOptions,
                    mHost->mActivity, mHost->mMyCallback, mHost->mHandler,
                    (IAccountManagerFuture**)&mHost->mFuture));
        }
        else {
            // send result since we can't prompt to add an account
            AutoPtr<IBundle> result;
            CBundle::New((IBundle**)&result);
            result->PutString(IAccountManager::KEY_ACCOUNT_NAME, String(NULL));
            result->PutString(IAccountManager::KEY_ACCOUNT_TYPE, String(NULL));
            result->PutString(IAccountManager::KEY_AUTHTOKEN, String(NULL));

            mHost->mResponse->OnResult(result);
            // we are done
        }
    }
    else if (accounts->GetLength() == 1) {
        // have a single account, return an authtoken for it
        mHost->mFuture = NULL;
        if (mHost->mActivity == NULL) {
            mHost->mHost->GetAuthToken((*accounts)[0], mHost->mAuthTokenType,
                    FALSE /* notifyAuthFailure */, mHost->mMyCallback,
                    mHost->mHandler, (IAccountManagerFuture**)&(mHost->mFuture));
        }
        else {
            mHost->mHost->GetAuthToken((*accounts)[0], mHost->mAuthTokenType,
                    mHost->mLoginOptions, mHost->mActivity, mHost->mMyCallback,
                    mHost->mHandler, (IAccountManagerFuture**)&(mHost->mFuture));
        }
    }
    else {
        if (mHost->mActivity != NULL) {
            AutoPtr<IAccountManagerResponse> chooseResponse;
            FAIL_RETURN(CChooseResponse::New((Handle32)this,
                    (IAccountManagerResponse**)&chooseResponse));
            // have many accounts, launch the chooser
            AutoPtr<IIntent> intent;
            FAIL_RETURN(CIntent::New((IIntent**)&intent));
            AutoPtr<IResourcesHelper> rhlp;
            CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rhlp);
            AutoPtr<IResources> res;
            rhlp->GetSystem((IResources**)&res);
            String str;
            res->GetString(R::string::config_chooseAccountActivity, &str);
            AutoPtr<IComponentNameHelper> chlp;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&chlp);
            AutoPtr<IComponentName> componentName;
            chlp->UnflattenFromString(str, (IComponentName**)&componentName);
            String pakName, clsName;
            componentName->GetPackageName(&pakName);
            componentName->GetClassName(&clsName);
            intent->SetClassName(pakName, clsName);

            intent->PutExtra(IAccountManager::KEY_ACCOUNTS, (Int32)accounts.Get());
            AutoPtr<AccountManagerResponse> response = new AccountManagerResponse(
                    chooseResponse);
            intent->PutExtra(IAccountManager::KEY_ACCOUNT_MANAGER_RESPONSE,
                    IParcelable::Probe(response));
            IContext::Probe(mHost->mActivity)->StartActivity(intent);
            // the result will arrive via the IAccountManagerResponse
        }
        else {
            // send result since we can't prompt to select an account
            AutoPtr<IBundle> result;
            CBundle::New((IBundle**)&result);
            result->PutString(IAccountManager::KEY_ACCOUNTS, String(NULL));
            mHost->mResponse->OnResult(result);
            // we are done
        }
    }

    return NOERROR;
}

//===============================================================
// CAccountManager::GetAuthTokenByTypeAndFeaturesTask::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::GetAuthTokenByTypeAndFeaturesTask, AmsTask, IAccountManagerCallback)

CAccountManager::GetAuthTokenByTypeAndFeaturesTask::GetAuthTokenByTypeAndFeaturesTask(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* features,
    /* [in] */ IActivity* activityForPrompting,
    /* [in] */ IBundle* addAccountOptions,
    /* [in] */ IBundle* loginOptions,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [in] */ CAccountManager* host)
    : AmsTask(activityForPrompting, handler, cb, host)
    , mAccountType(accountType)
    , mAuthTokenType(authTokenType)
    , mFeatures(features)
    , mAddAccountOptions(addAccountOptions)
    , mNumAccounts(0)
{
    assert(!accountType.IsNull());
    mMyCallback = this;
}

ECode CAccountManager::GetAuthTokenByTypeAndFeaturesTask::DoWork()
{
    AutoPtr<IAccountManagerFuture> future;
    AutoPtr<IAccountManagerCallback> callback = new GetAccountsCallback(this);
    return mHost->GetAccountsByTypeAndFeatures(mAccountType, mFeatures,
            callback, mHandler,
            (IAccountManagerFuture**)&future);
}

ECode CAccountManager::GetAuthTokenByTypeAndFeaturesTask::Run(
    /* [in] */ IAccountManagerFuture* future)
{
    //try {
    AutoPtr<IInterface> obj;
    FAIL_RETURN(future->GetResult((IInterface**)&obj));
    IBundle* result = IBundle::Probe(obj);
    if (mNumAccounts == 0) {
        String accountName, accountType;
        result->GetString(IAccountManager::KEY_ACCOUNT_NAME, &accountName);
        result->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &accountType);
        AutoPtr<ICharSequence> cs1, cs2;
        CString::New(accountName, (ICharSequence**)&cs1);
        CString::New(accountType, (ICharSequence**)&cs2);
        if (TextUtils::IsEmpty(cs1) || TextUtils::IsEmpty(cs2)) {
            Slogger::E(TAG, "account not in result");
            return E_AUTHENTICATOR_EXCEPTION;
//            setException(new AuthenticatorException("account not in result"));
        }
        AutoPtr<IAccount> account;
        CAccount::New(accountName, accountType, (IAccount**)&account);
        mNumAccounts = 1;
        AutoPtr<IAccountManagerFuture> f;
        return mHost->GetAuthToken(account, mAuthTokenType, NULL /* options */, mActivity,
                mMyCallback, mHandler, (IAccountManagerFuture**)&f);
    }
    Set(result);
    return NOERROR;
    // } catch (OperationCanceledException e) {
    //     cancel(true /* mayInterruptIfRUnning */);
    // } catch (IOException e) {
    //     setException(e);
    // } catch (AuthenticatorException e) {
    //     setException(e);
    // }
}

//===============================================================
// CAccountManager::AccountsChangedBroadcastReceiver::
//===============================================================

CAccountManager::AccountsChangedBroadcastReceiver::AccountsChangedBroadcastReceiver(
    /* [in] */ CAccountManager* host)
    : mHost(host)
{}

ECode CAccountManager::AccountsChangedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr< ArrayOf<IAccount*> > accounts;
    FAIL_RETURN(mHost->GetAccounts((ArrayOf<IAccount*>**)&accounts));
    // send the result to the listeners
    AutoLock lock(mHost->mAccountsUpdatedListenersLock);
    AutoPtr<ISet> st;
    mHost->mAccountsUpdatedListeners->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> v;
        entry->GetValue((IInterface**)&v);
        AutoPtr<IHandler> hl = IHandler::Probe(v);
        AutoPtr<IInterface> k;
        entry->GetKey((IInterface**)&k);
        AutoPtr<IOnAccountsUpdateListener> ul = IOnAccountsUpdateListener::Probe(k);
        mHost->PostToHandler(hl, ul, accounts);
    }

    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_GetAuthTokenLabel::
//===============================================================
CAccountManager::Future2Task_GetAuthTokenLabel::Future2Task_GetAuthTokenLabel(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType)
    : Future2Task(handler, cb, host)
    , mAccountType(accountType)
    , mAuthTokenType(authTokenType)
{}

ECode CAccountManager::Future2Task_GetAuthTokenLabel::DoWork()
{
    return mHost->mService->GetAuthTokenLabel(mResponse, mAccountType, mAuthTokenType);
}

ECode CAccountManager::Future2Task_GetAuthTokenLabel::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isContains;
    bundle->ContainsKey(IAccountManager::KEY_AUTH_TOKEN_LABEL, &isContains);
    if (!isContains) {
        Slogger::E(TAG, "no result in response");
        return E_AUTHENTICATOR_EXCEPTION;
        // throw new AuthenticatorException("no result in response");
    }
    String s;
    bundle->GetString(IAccountManager::KEY_AUTH_TOKEN_LABEL, &s);
    AutoPtr<ICharSequence> cs;
    CString::New(s, (ICharSequence**)&cs);
    *result = cs.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_HasFeatures::
//===============================================================

CAccountManager::Future2Task_HasFeatures::Future2Task_HasFeatures(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<String>* features)
    : Future2Task(handler, cb, host)
    , mAccount(account)
    , mFeatures(features)
{
}

ECode CAccountManager::Future2Task_HasFeatures::DoWork()
{
    return mHost->mService->HasFeatures(mResponse, mAccount, mFeatures);
}

ECode CAccountManager::Future2Task_HasFeatures::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Boolean isContains;
    bundle->ContainsKey(IAccountManager::KEY_BOOLEAN_RESULT, &isContains);
    if (!isContains) {
        Slogger::E(TAG, "no result in response");
        return E_AUTHENTICATOR_EXCEPTION;
//        throw new AuthenticatorException("no result in response");
    }
    Boolean res;
    bundle->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &res);
    AutoPtr<IBoolean> b;
    CBoolean::New(res, (IBoolean**)&b);
    *result = b.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_GetAccountsByTypeAndFeatures::
//===============================================================
CAccountManager::Future2Task_GetAccountsByTypeAndFeatures::Future2Task_GetAccountsByTypeAndFeatures(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ const String& type,
    /* [in] */ ArrayOf<String>* features)
    : Future2Task(handler, cb, host)
    , mType(type)
    , mFeatures(features)
{
}

ECode CAccountManager::Future2Task_GetAccountsByTypeAndFeatures::DoWork()
{
    return mHost->mService->GetAccountsByFeatures(mResponse, mType, mFeatures);
}

ECode CAccountManager::Future2Task_GetAccountsByTypeAndFeatures::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isContains;
    bundle->ContainsKey(IAccountManager::KEY_BOOLEAN_RESULT, &isContains);
    if (!isContains) {
        Slogger::E(TAG, "no result in response");
        return E_AUTHENTICATOR_EXCEPTION;
//        throw new AuthenticatorException("no result in response");
    }
    AutoPtr< ArrayOf<IParcelable*> > parcelables;
    bundle->GetParcelableArray(IAccountManager::KEY_ACCOUNTS,
            (ArrayOf<IParcelable*>**)&parcelables);
    AutoPtr<IArrayList> descs;
    CArrayList::New((IArrayList**)&descs);
    for (Int32 i = 0; i < parcelables->GetLength(); i++) {
        AutoPtr<IAccount> account = IAccount::Probe((*parcelables)[i]);
        descs->Add(account);
    }
    *result = IInterface::Probe(descs);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_RenameAccount::
//===============================================================
CAccountManager::Future2Task_RenameAccount::Future2Task_RenameAccount(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ const String& newName)
    : Future2Task(handler, cb, host)
{
    mAccount = account;
    mNewName = newName;
}

ECode CAccountManager::Future2Task_RenameAccount::DoWork()
{
    return mHost->mService->RenameAccount(mResponse, mAccount, mNewName);
}

ECode CAccountManager::Future2Task_RenameAccount::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    String name;
    bundle->GetString(KEY_ACCOUNT_NAME, &name);
    String type;
    bundle->GetString(KEY_ACCOUNT_TYPE, &type);
    AutoPtr<IAccount> acc;
    CAccount::New(name, type, (IAccount**)&acc);
    *result = IInterface::Probe(acc);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_RemoveAccount::
//===============================================================
CAccountManager::Future2Task_RemoveAccount::Future2Task_RemoveAccount(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account)
    : Future2Task(handler, cb, host)
    , mAccount(account)
{}

ECode CAccountManager::Future2Task_RemoveAccount::DoWork()
{
    return mHost->mService->RemoveAccount(mResponse, mAccount);
}

ECode CAccountManager::Future2Task_RemoveAccount::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isContains;
    bundle->ContainsKey(IAccountManager::KEY_BOOLEAN_RESULT, &isContains);
    if (!isContains) {
        Slogger::E(TAG, "no result in response");
        return E_AUTHENTICATOR_EXCEPTION;
//        throw new AuthenticatorException("no result in response");
    }
    Boolean res;
    bundle->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &res);
    AutoPtr<IBoolean> b;
    CBoolean::New(res, (IBoolean**)&b);
    *result = b.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//===============================================================
// CAccountManager::Future2Task_RemoveAccountAsUser::
//===============================================================
CAccountManager::Future2Task_RemoveAccountAsUser::Future2Task_RemoveAccountAsUser(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ IUserHandle* userHandle)
    : Future2Task(handler, cb, host)
    , mAccount(account)
    , mUserHandle(userHandle)
{}

ECode CAccountManager::Future2Task_RemoveAccountAsUser::DoWork()
{
    Int32 id = 0;
    mUserHandle->GetIdentifier(&id);
    return mHost->mService->RemoveAccountAsUser(mResponse, mAccount, id);
}

ECode CAccountManager::Future2Task_RemoveAccountAsUser::BundleToResult(
    /* [in] */ IBundle* bundle,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bContain = FALSE;
    if (!(bundle->ContainsKey(KEY_BOOLEAN_RESULT, &bContain), bContain)) {
        // throw new AuthenticatorException("no result in response");
        return E_AUTHENTICATOR_EXCEPTION;
    }
    Boolean kr = FALSE;
    bundle->GetBoolean(KEY_BOOLEAN_RESULT, &kr);
    AutoPtr<IBoolean> pKr;
    CBoolean::New(kr, (IBoolean**)&pKr);
    *result = pKr;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================
// CAccountManager::AmsTask_GetAuthToken::
//===============================================================
CAccountManager::AmsTask_GetAuthToken::AmsTask_GetAuthToken(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean notifyAuthFailure,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* options)
    : AmsTask(activity, handler, cb, host)
    , mAccount(account)
    , mAuthTokenType(authTokenType)
    , mNotifyAuthFailure(notifyAuthFailure)
    , mExpectActivityLaunch(expectActivityLaunch)
    , mOptions(options)
{}

ECode CAccountManager::AmsTask_GetAuthToken::DoWork()
{
    return mHost->mService->GetAuthToken(mResponse, mAccount, mAuthTokenType,
            mNotifyAuthFailure, mExpectActivityLaunch, mOptions);
}

//===============================================================
// CAccountManager::AmsTask_AddAccount::
//===============================================================
CAccountManager::AmsTask_AddAccount::AmsTask_AddAccount(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* options)
    : AmsTask(activity, handler, cb, host)
    , mAccountType(accountType)
    , mAuthTokenType(authTokenType)
    , mRequiredFeatures(requiredFeatures)
    , mOptions(options)
{
}

ECode CAccountManager::AmsTask_AddAccount::DoWork()
{
    return mHost->mService->AddAccount(mResponse, mAccountType, mAuthTokenType,
            mRequiredFeatures, mActivity != NULL, mOptions);
}

//===============================================================
// CAccountManager::AmsTask_AddAccountAsUser::
//===============================================================
CAccountManager::AmsTask_AddAccountAsUser::AmsTask_AddAccountAsUser(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* optionsIn,
    /* [in] */ Int32 id)
    : AmsTask(activity, handler, cb, host)
    , mAccountType(accountType)
    , mAuthTokenType(authTokenType)
    , mRequiredFeatures(requiredFeatures)
    , mOptionsIn(optionsIn)
    , mId(id)
{
}

ECode CAccountManager::AmsTask_AddAccountAsUser::DoWork()
{
    return mHost->mService->AddAccountAsUser(mResponse, mAccountType, mAuthTokenType,
            mRequiredFeatures, mActivity != NULL, mOptionsIn, mId);
}

//===============================================================
// CAccountManager::AmsTask_ConfirmCredentialsAsUser::
//===============================================================
CAccountManager::AmsTask_ConfirmCredentialsAsUser::AmsTask_ConfirmCredentialsAsUser(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId)
    : AmsTask(activity, handler, cb, host)
    , mAccount(account)
    , mOptions(options)
    , mUserId(userId)
{}

ECode CAccountManager::AmsTask_ConfirmCredentialsAsUser::DoWork()
{
    return mHost->mService->ConfirmCredentialsAsUser(mResponse, mAccount,
            mOptions, mActivity != NULL, mUserId);
}

//===============================================================
// CAccountManager::AmsTask_UpdateCredentials::
//===============================================================
CAccountManager::AmsTask_UpdateCredentials::AmsTask_UpdateCredentials(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options)
    : AmsTask(activity, handler, cb, host)
    , mAccount(account)
    , mAuthTokenType(authTokenType)
    , mOptions(options)
{}

ECode CAccountManager::AmsTask_UpdateCredentials::DoWork()
{
    return mHost->mService->UpdateCredentials(mResponse, mAccount,
            mAuthTokenType, mActivity != NULL, mOptions);
}

//===============================================================
// CAccountManager::AmsTask_EditProperties::
//===============================================================
CAccountManager::AmsTask_EditProperties::AmsTask_EditProperties(
    /* [in] */ IActivity* activity,
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ CAccountManager* host,
    /* [in] */ const String& authTokenType)
    : AmsTask(activity, handler, cb, host)
    , mAuthTokenType(authTokenType)
{}

ECode CAccountManager::AmsTask_EditProperties::DoWork()
{
    return mHost->mService->EditProperties(mResponse,
            mAuthTokenType, mActivity != NULL);
}

//===============================================================
// CAccountManager::CallbackAction::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::CallbackAction, Object, IRunnable);

CAccountManager::CallbackAction::CallbackAction(
    /* [in] */ IAccountManagerCallback* accountManagerCb,
    /* [in] */ IAccountManagerFuture* future)
    : mCallback(accountManagerCb)
    , mFuture(future)
{}

ECode CAccountManager::CallbackAction::Run()
{
    return mCallback->Run(mFuture);
}

//===============================================================
// CAccountManager::AccountUpdateAction::
//===============================================================
CAR_INTERFACE_IMPL(CAccountManager::AccountUpdateAction, Object, IRunnable);

CAccountManager::AccountUpdateAction::AccountUpdateAction(
    /* [in] */ IOnAccountsUpdateListener* listener,
    /* [in] */ ArrayOf<IAccount*>* accounts)
    : mListener(listener)
    , mAccounts(accounts)
{}

ECode CAccountManager::AccountUpdateAction::Run()
{
    //try {
    return mListener->OnAccountsUpdated(*mAccounts);
    // } catch (SQLException e) {
    //     // Better luck next time.  If the problem was disk-full,
    //     // the STORAGE_OK intent will re-trigger the update.
    //     Log.e(TAG, "Can't update accounts", e);
    // }
}

//===============================================================
// CAccountManager::
//===============================================================
CAR_OBJECT_IMPL(CAccountManager)

CAR_INTERFACE_IMPL(CAccountManager, Object, IAccountManager)

CAccountManager::CAccountManager()
{
}

ECode CAccountManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAccountManager* service)
{
    mContext = context;
    mService = service;
    mAccountsChangedBroadcastReceiver = new AccountsChangedBroadcastReceiver(this);
    AutoPtr<ILooper> looper;
    ASSERT_SUCCEEDED(mContext->GetMainLooper((ILooper**)&looper));
    return CHandler::New(looper, (IHandler**)&mMainHandler);
}

ECode CAccountManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAccountManager* service,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    mService = service;
    mMainHandler = handler;
    return NOERROR;
}

AutoPtr<IBundle> CAccountManager::SanitizeResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        Boolean isContains = FALSE;
        result->ContainsKey(KEY_AUTHTOKEN, &isContains);
        if (isContains) {
            String value;
            result->GetString(KEY_AUTHTOKEN, &value);
            AutoPtr<ICharSequence> cs;
            CString::New(value, (ICharSequence**)&cs);
            if (!TextUtils::IsEmpty(cs)) {
                AutoPtr<CBundle> newResult;
                CBundle::NewByFriend((CBundle**)&newResult);
                newResult->PutString(KEY_AUTHTOKEN,
                        String("<omitted for logging purposes>"));
                return newResult;
            }
        }
    }
    return result;
}

ECode CAccountManager::Get(
    /* [in] */ IContext* context,
    /* [out] */ IAccountManager** accountManager)
{
    VALIDATE_NOT_NULL(accountManager);
    *accountManager = NULL;

    if (context == NULL) {
        Slogger::E(TAG, "context is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        //throw new IllegalArgumentException("context is null");
    }

    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::ACCOUNT_SERVICE, (IInterface**)&service))

    *accountManager = IAccountManager::Probe(service);
    REFCOUNT_ADD(*accountManager)
    return NOERROR;
}

ECode CAccountManager::GetPassword(
    /* [in] */ IAccount* account,
    /* [out] */ String* password)
{
    VALIDATE_NOT_NULL(password);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    return mService->GetPassword(account, password);
}

ECode CAccountManager::GetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [out] */ String* userData)
{
    VALIDATE_NOT_NULL(userData);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("key is null");
    }
    return mService->GetUserData(account, key, userData);
}

ECode CAccountManager::GetAuthenticatorTypes(
    /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** authenticators)
{
    VALIDATE_NOT_NULL(authenticators);
    AutoPtr<IUserHandleHelper> hlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
    Int32 id = 0;
    hlp->GetCallingUserId(&id);
    return mService->GetAuthenticatorTypes(id, authenticators);
}

ECode CAccountManager::GetAuthenticatorTypesAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAuthenticatorDescription*>** authenticators)
{
    VALIDATE_NOT_NULL(authenticators);
    return mService->GetAuthenticatorTypes(userId, authenticators);
}

ECode CAccountManager::GetAccounts(
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    return mService->GetAccounts(String(NULL), accounts);
}

ECode CAccountManager::GetAccountsAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    return mService->GetAccountsAsUser(String(NULL), userId, accounts);
}

ECode CAccountManager::GetAccountsForPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    return mService->GetAccountsForPackage(packageName, uid, accounts);
}

ECode CAccountManager::GetAccountsByTypeForPackage(
    /* [in] */ const String& type,
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    return mService->GetAccountsByTypeForPackage(type, packageName, accounts);
}

ECode CAccountManager::GetAccountsByType(
    /* [in] */ const String& type,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    AutoPtr<IUserHandle> userH;
    Process::MyUserHandle((IUserHandle**)&userH);
    return GetAccountsByTypeAsUser(type, userH, accounts);
}

ECode CAccountManager::GetAccountsByTypeAsUser(
    /* [in] */ const String& type,
    /* [in] */ IUserHandle* userHandle,
    /* [out, callee] */ ArrayOf<IAccount*>** accounts)
{
    VALIDATE_NOT_NULL(accounts);
    Int32 id = 0;
    userHandle->GetIdentifier(&id);
    return mService->GetAccountsAsUser(type, id, accounts);
}

ECode CAccountManager::UpdateAppPermission(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean value)
{
    return mService->UpdateAppPermission(account, authTokenType, uid, value);
}

ECode CAccountManager::GetAuthTokenLabel(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** future)
{
    VALIDATE_NOT_NULL(future);
    *future = NULL;

    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr<Future2Task_GetAuthTokenLabel> f = new Future2Task_GetAuthTokenLabel(
            handler, cb, this, accountType, authTokenType);
    return f->Start(future);
}

ECode CAccountManager::HasFeatures(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<String>* features,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    //if (features == null) throw new IllegalArgumentException("features is null");
    AutoPtr<Future2Task_HasFeatures> future = new Future2Task_HasFeatures(
            handler, cb, this, account, features);
    return future->Start(accountManagerFuture);
}

ECode CAccountManager::GetAccountsByTypeAndFeatures(
    /* [in] */ const String& type,
    /* [in] */ ArrayOf<String>* features,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (type.IsNull()) {
        Slogger::E(TAG, "type is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("type is null");
    }
    AutoPtr<Future2Task_GetAccountsByTypeAndFeatures> future = new Future2Task_GetAccountsByTypeAndFeatures(
            handler, cb, this, type, features);
    return future->Start(accountManagerFuture);
}

ECode CAccountManager::AddAccountExplicitly(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password,
    /* [in] */ IBundle* userdata,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    return mService->AddAccountExplicitly(account, password, userdata, result);
}

ECode CAccountManager::RenameAccount(
    /* [in] */ IAccount* account,
    /* [in] */ const String& newName,
    /* [in] */ IAccountManagerCallback* callback,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** result)
{
    VALIDATE_NOT_NULL(result)
    if (account == NULL) {
        // throw new IllegalArgumentException("account is null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (TextUtils::IsEmpty(newName)) {
        // throw new IllegalArgumentException("newName is empty or null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<Future2Task_RenameAccount> future = new Future2Task_RenameAccount(
            handler, callback, this, account, newName);
    return future->Start(result);
}

ECode CAccountManager::GetPreviousName(
    /* [in] */ IAccount* account,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (account == NULL) {
        // throw new IllegalArgumentException("account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mService->GetPreviousName(account, result);
}

ECode CAccountManager::RemoveAccount(
    /* [in] */ IAccount* account,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    AutoPtr<Future2Task_RemoveAccount> future = new Future2Task_RemoveAccount(
            handler, cb, this, account);
    return future->Start(accountManagerFuture);
}

ECode CAccountManager::RemoveAccountAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ IAccountManagerCallback* callback,
    /* [in] */ IHandler* handler,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture)
    if (account == NULL) {
        // throw new IllegalArgumentException("account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (userHandle == NULL) {
        // throw new IllegalArgumentException("userHandle is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<Future2Task_RemoveAccountAsUser> future = new Future2Task_RemoveAccountAsUser(
            handler, callback, this, account, userHandle);
    return future->Start(accountManagerFuture);
}

ECode CAccountManager::InvalidateAuthToken(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authToken)
{
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("accountType is null");
    }
    if (!authToken.IsNull()) {
        return mService->InvalidateAuthToken(accountType, authToken);
    }
    return NOERROR;
}

ECode CAccountManager::PeekAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [out] */ String* token)
{
    VALIDATE_NOT_NULL(token);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("authTokenType is null");
    }
    return mService->PeekAuthToken(account, authTokenType, token);
}

ECode CAccountManager::SetPassword(
    /* [in] */ IAccount* account,
    /* [in] */ const String& password)
{
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    return mService->SetPassword(account, password);
}

ECode CAccountManager::ClearPassword(
    /* [in] */ IAccount* account)
{
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    return mService->ClearPassword(account);
}

ECode CAccountManager::SetUserData(
    /* [in] */ IAccount* account,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (key.IsNull()) {
        Slogger::E(TAG, "key is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("key is null");
    }
    return mService->SetUserData(account, key, value);
}

ECode CAccountManager::SetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ const String& authToken)
{
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("authTokenType is null");
    }
    return mService->SetAuthToken(account, authTokenType, authToken);
}

ECode CAccountManager::BlockingGetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean notifyAuthFailure,
    /* [out] */ String* token)
{
    VALIDATE_NOT_NULL(token);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("authTokenType is null");
    }

    AutoPtr<IAccountManagerFuture> accountManagerFuture;
    GetAuthToken(account, authTokenType, notifyAuthFailure, NULL /* callback */,
            NULL /* handler */, (IAccountManagerFuture**)&accountManagerFuture);
    AutoPtr<IInterface> obj;
    accountManagerFuture->GetResult((IInterface**)&obj);
    IBundle* bundle = IBundle::Probe(obj);
    if (bundle == NULL) {
        // This should never happen, but it does, occasionally. If it does return null to
        // signify that we were not able to get the authtoken.
        // TODO: remove this when the bug is found that sometimes causes a null bundle to be
        // returned
        Slogger::E(TAG, "blockingGetAuthToken: null was returned from getResult() for %p, authTokenType %s"
                , account, (const char*)authTokenType);
        *token = String(NULL);
        return NOERROR;
    }
    return bundle->GetString(KEY_AUTHTOKEN, token);
}

ECode CAccountManager::GetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    *accountManagerFuture = NULL;

    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr<IBundle> optionsIn;
    FAIL_RETURN(CBundle::New((IBundle**)&optionsIn));
    if (options != NULL) {
        optionsIn->PutAll(options);
    }
    String packageName;
    mContext->GetPackageName(&packageName);
    optionsIn->PutString(KEY_ELASTOS_PACKAGE_NAME, packageName);
    AutoPtr<AmsTask_GetAuthToken> task = new AmsTask_GetAuthToken(
            activity, handler, cb, this, account, authTokenType,
            FALSE /* notifyOnAuthFailure */, TRUE /* expectActivityLaunch */, optionsIn);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::GetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean notifyAuthFailure,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    return GetAuthToken(account, authTokenType, NULL, notifyAuthFailure, cb,
                handler, accountManagerFuture);
}

ECode CAccountManager::GetAuthToken(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options,
    /* [in] */ Boolean notifyAuthFailure,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr<IBundle> optionsIn;
    FAIL_RETURN(CBundle::New((IBundle**)&optionsIn));
    if (options != NULL) {
        optionsIn->PutAll(options);
    }
    String packageName;
    mContext->GetPackageName(&packageName);
    optionsIn->PutString(KEY_ELASTOS_PACKAGE_NAME, packageName);
    AutoPtr<AmsTask_GetAuthToken> task = new AmsTask_GetAuthToken(
            NULL, handler, cb, this, account, authTokenType,
            notifyAuthFailure, FALSE /* expectActivityLaunch */, optionsIn);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::AddAccount(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* addAccountOptions,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("accountType is null");
    }
    AutoPtr<IBundle> optionsIn;
    FAIL_RETURN(CBundle::New((IBundle**)&optionsIn));
    if (addAccountOptions != NULL) {
        optionsIn->PutAll(addAccountOptions);
    }
    String packageName;
    mContext->GetPackageName(&packageName);
    optionsIn->PutString(KEY_ELASTOS_PACKAGE_NAME, packageName);
    AutoPtr<AmsTask_AddAccount> task = new AmsTask_AddAccount(
            activity, handler, cb, this, accountType, authTokenType,
            requiredFeatures, optionsIn);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::AddAccountAsUser(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* addAccountOptions,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* callback,
    /* [in] */ IHandler* handler,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ IAccountManagerFuture** result)
{
    VALIDATE_NOT_NULL(result)
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    if (userHandle == NULL) {
        Slogger::E(TAG, "userHandle is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr<IBundle> optionsIn;
    FAIL_RETURN(CBundle::New((IBundle**)&optionsIn));
    if (addAccountOptions != NULL) {
        optionsIn->PutAll(addAccountOptions);
    }
    String packageName;
    mContext->GetPackageName(&packageName);
    optionsIn->PutString(KEY_ELASTOS_PACKAGE_NAME, packageName);

    Int32 id = 0;
    userHandle->GetIdentifier(&id);
    AutoPtr<AmsTask_AddAccountAsUser> task = new AmsTask_AddAccountAsUser(
            activity, handler, callback, this, accountType, authTokenType,
            requiredFeatures, optionsIn, id);
    return task->Start(result);
}

ECode CAccountManager::AddSharedAccount(
    /* [in] */ IAccount* account,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 id = 0;
    user->GetIdentifier(&id);
    return mService->AddSharedAccountAsUser(account, id, result);
}

ECode CAccountManager::RemoveSharedAccount(
    /* [in] */ IAccount* account,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 id = 0;
    user->GetIdentifier(&id);
    return mService->RemoveSharedAccountAsUser(account, id, result);
}

ECode CAccountManager::GetSharedAccounts(
    /* [in] */ IUserHandle* user,
    /* [out] */ ArrayOf<IAccount*>** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 id = 0;
    user->GetIdentifier(&id);
    return mService->GetSharedAccountsAsUser(id, result);

}

ECode CAccountManager::ConfirmCredentials(
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    return ConfirmCredentialsAsUser(account, options, activity, cb, handler,
            userHandle, accountManagerFuture);
}

ECode CAccountManager::ConfirmCredentialsAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_AUTHENTICATOR_EXCEPTION;
        // throw new IllegalArgumentException("account is null");
    }
    Int32 userId;
    userHandle->GetIdentifier(&userId);
    AutoPtr<AmsTask_ConfirmCredentialsAsUser> task = new AmsTask_ConfirmCredentialsAsUser(
        activity, handler, cb, this, account, options, userId);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::UpdateCredentials(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (account == NULL) {
        Slogger::E(TAG, "account is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account is null");
    }
    AutoPtr<AmsTask_UpdateCredentials> task = new AmsTask_UpdateCredentials(
            activity, handler, cb, this, account, authTokenType, options);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::EditProperties(
    /* [in] */ const String& accountType,
    /* [in] */ IActivity* activity,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("accountType is null");
    }
    AutoPtr<AmsTask_EditProperties> task = new AmsTask_EditProperties(
            activity, handler, cb, this, accountType);
    return task->Start(accountManagerFuture);
}

ECode CAccountManager::EnsureNotOnMainThread()
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    AutoPtr<ILooper> mainLooper;
    if (looper != NULL &&
            (mContext->GetMainLooper((ILooper**)&mainLooper), looper == mainLooper)) {
        // final IllegalStateException exception = new IllegalStateException(
        //         "calling this from your main thread can lead to deadlock");
        Slogger::E(TAG, "calling this from your main thread can lead to deadlock and/or ANRs");
        AutoPtr<IApplicationInfo> info;
        ASSERT_SUCCEEDED(mContext->GetApplicationInfo((IApplicationInfo**)&info));
        Int32 version;
        info->GetTargetSdkVersion(&version);
        if (version >= 8/*Build.VERSION_CODES.FROYO*/) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NOERROR;
}

void CAccountManager::PostToHandler(
    /* [in] */ IHandler* handler,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IAccountManagerFuture* future)
{
    AutoPtr<IRunnable> action = new CallbackAction(cb, future);
    AutoPtr<IHandler> h = handler == NULL ? mMainHandler.Get() : handler;
    Boolean result;
    h->Post(action, &result);
}

void CAccountManager::PostToHandler(
    /* [in] */ IHandler* handler,
    /* [in] */ IOnAccountsUpdateListener* listener,
    /* [in] */ ArrayOf<IAccount*>* accounts)
{
    // send a copy to make sure that one doesn't
    // change what another sees
    AutoPtr< ArrayOf<IAccount*> > accountsCopy = ArrayOf<IAccount*>::Alloc(accounts->GetLength());
    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        accountsCopy->Set(i, (*accounts)[i]);
    }

    AutoPtr<IHandler> h = (handler == NULL) ? mMainHandler.Get() : handler;
    AutoPtr<IRunnable> action = new AccountUpdateAction(listener, accountsCopy);
    Boolean result;
    h->Post(action, &result);
}

ECode CAccountManager::ConvertErrorToException(
    /* [in] */ Int32 code,
    /* [in] */ const String& message)
{
    if (code == ERROR_CODE_NETWORK_ERROR) {
        Slogger::E(TAG, "%s", (const char*)message);
        return E_IO_EXCEPTION;
//        return new IOException(message);
    }

    if (code == ERROR_CODE_UNSUPPORTED_OPERATION) {
        Slogger::E(TAG, "%s", (const char*)message);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
//        return new UnsupportedOperationException(message);
    }

    if (code == ERROR_CODE_INVALID_RESPONSE) {
        Slogger::E(TAG, "%s", (const char*)message);
        return E_AUTHENTICATOR_EXCEPTION;
//        return new AuthenticatorException(message);
    }

    if (code == ERROR_CODE_BAD_ARGUMENTS) {
        Slogger::E(TAG, "%s", (const char*)message);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        return new IllegalArgumentException(message);
    }

    Slogger::E(TAG, "%s", (const char*)message);
    return E_AUTHENTICATOR_EXCEPTION;
//    return new AuthenticatorException(message);
}

ECode CAccountManager::GetAuthTokenByFeatures(
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* features,
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* addAccountOptions,
    /* [in] */ IBundle* getAuthTokenOptions,
    /* [in] */ IAccountManagerCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IAccountManagerFuture** accountManagerFuture)
{
    VALIDATE_NOT_NULL(accountManagerFuture);
    if (accountType.IsNull()) {
        Slogger::E(TAG, "account type is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("account type is null");
    }
    if (authTokenType.IsNull()) {
        Slogger::E(TAG, "authTokenType type is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("authTokenType is null");
    }
    AutoPtr<GetAuthTokenByTypeAndFeaturesTask> task =
            new GetAuthTokenByTypeAndFeaturesTask(accountType, authTokenType, features,
            activity, addAccountOptions, getAuthTokenOptions, cb, handler, this);
    AutoPtr<IAccountManagerFuture> future;
    task->Start((IAccountManagerFuture**)&future);
    *accountManagerFuture = task.Get();
    REFCOUNT_ADD(*accountManagerFuture);
    return NOERROR;
}

ECode CAccountManager::NewChooseAccountIntent(
    /* [in] */ IAccount* selectedAccount,
    /* [in] */ ArrayOf<IAccount*>* allowableAccounts,
    /* [in] */ ArrayOf<String>* allowableAccountTypes,
    /* [in] */ Boolean alwaysPromptForAccount,
    /* [in] */ const String& descriptionOverrideText,
    /* [in] */ const String& addAccountAuthTokenType,
    /* [in] */ ArrayOf<String>* addAccountRequiredFeatures,
    /* [in] */ IBundle* addAccountOptions,
    /* [out] */ IIntent** _intent)
{
    VALIDATE_NOT_NULL(_intent);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New((IIntent**)&intent));
    AutoPtr<IResourcesHelper> rhlp;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rhlp);
    AutoPtr<IResources> res;
    rhlp->GetSystem((IResources**)&res);
    String str;
    res->GetString(R::string::config_chooseTypeAndAccountActivity, &str);
    AutoPtr<IComponentNameHelper> chlp;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&chlp);
    AutoPtr<IComponentName> componentName;
    chlp->UnflattenFromString(str, (IComponentName**)&componentName);
    String pakName, clsName;
    componentName->GetPackageName(&pakName);
    componentName->GetClassName(&clsName);
    intent->SetClassName(pakName, clsName);
    AutoPtr< ArrayOf<IParcelable*> > parcelables = ArrayOf<IParcelable*>::Alloc(
            allowableAccounts->GetLength());
    for (Int32 i = 0; i < allowableAccounts->GetLength(); ++i) {
        AutoPtr<IParcelable> p = IParcelable::Probe((*allowableAccounts)[i]);
        parcelables->Set(i, p);
    }
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNTS_ARRAYLIST,
            parcelables);
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            allowableAccountTypes);
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE,
            addAccountOptions);
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_SELECTED_ACCOUNT,
            (Int32)selectedAccount);
    intent->PutBooleanExtra(IChooseTypeAndAccountActivity::EXTRA_ALWAYS_PROMPT_FOR_ACCOUNT,
            alwaysPromptForAccount);
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_DESCRIPTION_TEXT_OVERRIDE,
            descriptionOverrideText);
    intent->PutExtra(IChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING,
            addAccountAuthTokenType);
    intent->PutExtra(
            IChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY,
            addAccountRequiredFeatures);
    *_intent = intent;
    REFCOUNT_ADD(*_intent);
    return NOERROR;
}

ECode CAccountManager::AddOnAccountsUpdatedListener(
    /* [in] */ IOnAccountsUpdateListener* listener,
    /* [in] */ IHandler* handler,
    /* [in] */ Boolean updateImmediately)
{
    if (listener == NULL) {
        Slogger::E(TAG, "the listener is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("the listener is null");
    }

    {
        AutoLock lock(mAccountsUpdatedListenersLock);
        Boolean bContain = FALSE;
        if ((mAccountsUpdatedListeners->ContainsKey(listener, &bContain), bContain)) {
            Slogger::E(TAG, "this listener is already added");
            return E_ILLEGAL_STATE_EXCEPTION;
            //throw new IllegalStateException("this listener is already added");
        }

        Boolean wasEmpty = FALSE;
        mAccountsUpdatedListeners->IsEmpty(&wasEmpty);

        mAccountsUpdatedListeners->Put(listener, handler);

        if (wasEmpty) {
            // Register a broadcast receiver to monitor account changes
            AutoPtr<IIntentFilter> intentFilter;
            FAIL_RETURN(CIntentFilter::New((IIntentFilter**)&intentFilter));
            intentFilter->AddAction(LOGIN_ACCOUNTS_CHANGED_ACTION);
            // To recover from disk-full.
            intentFilter->AddAction(IIntent::ACTION_DEVICE_STORAGE_OK);
            AutoPtr<IIntent> intent;
            FAIL_RETURN(mContext->RegisterReceiver(
                mAccountsChangedBroadcastReceiver.Get(),
                intentFilter, (IIntent**)&intent));
        }
    }

    if (updateImmediately) {
        AutoPtr< ArrayOf<IAccount*> > accounts;
        GetAccounts((ArrayOf<IAccount*>**)&accounts);
        PostToHandler(handler, listener, accounts);
    }
    return NOERROR;
}

ECode CAccountManager::RemoveOnAccountsUpdatedListener(
    /* [in] */ IOnAccountsUpdateListener* listener)
{
    if (listener == NULL) {
        Slogger::E(TAG, "listener is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("listener is null");
    }
    AutoLock lock(mAccountsUpdatedListenersLock);
    Boolean bContain = FALSE;
    if (!(mAccountsUpdatedListeners->ContainsKey(listener, &bContain), bContain)) {
        Slogger::E(TAG, "Listener was not previously added");
        return NOERROR;
    }

    mAccountsUpdatedListeners->Remove(listener);
    Boolean bEmp = FALSE;
    if ((mAccountsUpdatedListeners->IsEmpty(&bEmp), bEmp)) {
        return mContext->UnregisterReceiver(IBroadcastReceiver::Probe(mAccountsChangedBroadcastReceiver));
    }

    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
