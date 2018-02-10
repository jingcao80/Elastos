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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAccountAuthenticatorTransport.h"
#include "elastos/droid/accounts/CAccountAuthenticatorResponse.h"
#include "elastos/droid/accounts/AbstractAccountAuthenticator.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CAccountAuthenticatorTransport)

CAR_INTERFACE_IMPL(CAccountAuthenticatorTransport, Object, IIAccountAuthenticator)

ECode CAccountAuthenticatorTransport::AddAccount(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ const String& authTokenType,
    /* [in] */ ArrayOf<String>* requiredFeatures,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "addAccount: accountType " + accountType
    //             + ", authTokenType " + authTokenType
    //             + ", features " + (features == null ? "[]" : Arrays.toString(features)));
    // }
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->AddAccount(authResponse, accountType, authTokenType,
            requiredFeatures, options, (IBundle**)&result);
    if (FAILED(ec)) {
        return mHost->HandleException(response, String("addAccount"), accountType, ec);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "addAccount: result " + AccountManager.sanitizeResult(result));
    // }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            return mHost->HandleException(response, String("addAccount"), accountType, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "addAccount", accountType, e);
    // }
}

ECode CAccountAuthenticatorTransport::ConfirmCredentials(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "confirmCredentials: " + account);
    // }
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->ConfirmCredentials(authResponse, account, options,
            (IBundle**)&result);
    if (FAILED(ec)) {
        String s;
        account->ToString(&s);
        return mHost->HandleException(response, String("confirmCredentials"), s, ec);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "confirmCredentials: result "
    //             + AccountManager.sanitizeResult(result));
    // }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            String s;
            account->ToString(&s);
            return mHost->HandleException(response, String("confirmCredentials"), s, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "confirmCredentials", account.toString(), e);
    // }
}

ECode CAccountAuthenticatorTransport::GetAuthTokenLabel(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ const String& authTokenType)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAuthTokenLabel: authTokenType " + authTokenType);
    // }
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    CBundle::New((IBundle**)&result);
    String s;
    ECode ec = mHost->GetAuthTokenLabel(authTokenType, &s);
    if (FAILED(ec)) {
        return mHost->HandleException(response, String("confirmCredentials"),
                authTokenType, ec);
    }
    ec = result->PutString(IAccountManager::KEY_AUTH_TOKEN_LABEL, s);
    if (FAILED(ec)) {
        return mHost->HandleException(response, String("confirmCredentials"),
                authTokenType, ec);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "getAuthTokenLabel: result "
    //             + AccountManager.sanitizeResult(result));
    // }
    ec = response->OnResult(result);
    if (FAILED(ec)) {
        return mHost->HandleException(response, String("confirmCredentials"),
                authTokenType, ec);
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "getAuthTokenLabel", authTokenType, e);
    // }
}

ECode CAccountAuthenticatorTransport::GetAuthToken(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "getAuthToken: " + account
    //             + ", authTokenType " + authTokenType);
    // }
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->GetAuthToken(authResponse, account, authTokenType, options,
            (IBundle**)&result);
    if (FAILED(ec)) {
        String s;
        account->ToString(&s);
        return mHost->HandleException(response, String("getAuthToken"),
                s + String(",") + authTokenType, ec);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "getAuthToken: result " + AccountManager.sanitizeResult(result));
    // }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            String s;
            account->ToString(&s);
            return mHost->HandleException(response, String("getAuthToken"),
                    s + String(",") + authTokenType, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "getAuthToken",
    //             account.toString() + "," + authTokenType, e);
    // }
}

ECode CAccountAuthenticatorTransport::UpdateCredentials(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* options)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "updateCredentials: " + account
    //             + ", authTokenType " + authTokenType);
    // }
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->UpdateCredentials(authResponse, account, authTokenType, options,
            (IBundle**)&result);
    if (FAILED(ec)) {
        String s;
        account->ToString(&s);
        return mHost->HandleException(response, String("updateCredentials"),
                s + String(",") + authTokenType, ec);
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "updateCredentials: result "
    //             + AccountManager.sanitizeResult(result));
    // }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            String s;
            account->ToString(&s);
            return mHost->HandleException(response, String("updateCredentials"),
                    s + String(",") + authTokenType, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "updateCredentials",
    //             account.toString() + "," + authTokenType, e);
    // }
}

ECode CAccountAuthenticatorTransport::EditProperties(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ const String& accountType)
{
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->EditProperties(authResponse, accountType, (IBundle**)&result);
    if (FAILED(ec)) {
        return mHost->HandleException(response, String("editProperties"),
                accountType, ec);
    }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            return mHost->HandleException(response, String("editProperties"),
                    accountType, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "editProperties", accountType, e);
    // }
}

ECode CAccountAuthenticatorTransport::HasFeatures(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<String>* features)
{
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->HasFeatures(authResponse, account, features, (IBundle**)&result);
    if (FAILED(ec)) {
        String s;
        account->ToString(&s);
        return mHost->HandleException(response, String("hasFeatures"), s, ec);
    }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            String s;
            account->ToString(&s);
            return mHost->HandleException(response, String("hasFeatures"), s, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "hasFeatures", account.toString(), e);
    // }
}

ECode CAccountAuthenticatorTransport::GetAccountRemovalAllowed(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account)
{
    FAIL_RETURN(mHost->CheckBinderPermission());
    // try {
    AutoPtr<IBundle> result;
    AutoPtr<IAccountAuthenticatorResponse> authResponse;
    CAccountAuthenticatorResponse::New(response,
            (IAccountAuthenticatorResponse**)&authResponse);
    ECode ec = mHost->GetAccountRemovalAllowed(authResponse, account, (IBundle**)&result);
    if (FAILED(ec)) {
        String s;
        account->ToString(&s);
        return mHost->HandleException(response, String("getAccountRemovalAllowed"), s, ec);
    }
    if (result != NULL) {
        ec = response->OnResult(result);
        if (FAILED(ec)) {
            String s;
            account->ToString(&s);
            return mHost->HandleException(response, String("getAccountRemovalAllowed"), s, ec);
        }
    }
    return NOERROR;
    // } catch (Exception e) {
    //     handleException(response, "getAccountRemovalAllowed", account.toString(), e);
    // }
}

ECode CAccountAuthenticatorTransport::constructor(
    /* [in] */ HANDLE accounts)
{
    mHost = (AbstractAccountAuthenticator*)accounts;
    return NOERROR;
}

ECode CAccountAuthenticatorTransport::GetAccountCredentialsForCloning(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account)
{
    FAIL_RETURN(mHost->CheckBinderPermission());

    AutoPtr<IAccountAuthenticatorResponse> aar;
    CAccountAuthenticatorResponse::New(response, (IAccountAuthenticatorResponse**)&aar);
    AutoPtr<IBundle> result;
    // AbstractAccountAuthenticator.this.getAccountCredentialsForCloning(
    //         aar, account, (IBundle**)&result);
    if (result != NULL) {
        response->OnResult(result);
    }
    return NOERROR;
}

ECode CAccountAuthenticatorTransport::AddAccountFromCredentials(
    /* [in] */ IIAccountAuthenticatorResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* accountCredentials)
{
    FAIL_RETURN(mHost->CheckBinderPermission());

    AutoPtr<IAccountAuthenticatorResponse> aar;
    CAccountAuthenticatorResponse::New(response, (IAccountAuthenticatorResponse**)&aar);
    AutoPtr<IBundle> result;
    // AbstractAccountAuthenticator.this.addAccountFromCredentials(
    //         aar, account,
    //         accountCredentials, (IBundle**)&result);
    if (result != NULL) {
        response->OnResult(result);
    }
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
