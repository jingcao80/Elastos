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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/accounts/CChooseResponse.h"
#include "elastos/droid/accounts/CAccount.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CChooseResponse)

CAR_INTERFACE_IMPL(CChooseResponse, Object, IAccountManagerResponse)

ECode CChooseResponse::OnResult(
    /* [in] */ IBundle* value)
{
    AutoPtr<IAccount> account;
    String name, type;
    value->GetString(IAccountManager::KEY_ACCOUNT_NAME, &name);
    value->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &type);
    CAccount::New(name, type, (IAccount**)&account);
    mHost->mFuture = NULL;
    return mHost->mHost->GetAuthToken(account, mHost->mAuthTokenType, mHost->mLoginOptions,
            mHost->mActivity, mHost->mMyCallback, mHost->mHandler,
            (IAccountManagerFuture**)&mHost->mFuture);
}

ECode CChooseResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return mHost->mResponse->OnError(errorCode, errorMessage);
}

ECode CChooseResponse::constructor (
    /* [in] */ HANDLE host)
{
    mHost = (CAccountManager::GetAuthTokenByTypeAndFeaturesTask*)host;
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
