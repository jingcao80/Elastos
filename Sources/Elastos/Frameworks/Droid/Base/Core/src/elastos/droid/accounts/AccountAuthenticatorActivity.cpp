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
#include "elastos/droid/accounts/AccountAuthenticatorActivity.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_INTERFACE_IMPL(AccountAuthenticatorActivity, Activity, IAccountAuthenticatorActivity)

ECode AccountAuthenticatorActivity::SetAccountAuthenticatorResult(
    /* [in] */ IBundle* result)
{
    mResultBundle = result;
    return NOERROR;
}

ECode AccountAuthenticatorActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(Activity::OnCreate(icicle));

    AutoPtr<IParcelable> parcelable;
    AutoPtr<IIntent> intent;
    FAIL_RETURN(GetIntent((IIntent**)&intent));
    FAIL_RETURN(intent->GetParcelableExtra(
            IAccountManager::KEY_ACCOUNT_AUTHENTICATOR_RESPONSE, (IParcelable**)&parcelable));
    mAccountAuthenticatorResponse = IAccountAuthenticatorResponse::Probe(parcelable);
    if (mAccountAuthenticatorResponse != NULL) {
        return mAccountAuthenticatorResponse->OnRequestContinued();
    }
    return NOERROR;
}

ECode AccountAuthenticatorActivity::Finish()
{
    if (mAccountAuthenticatorResponse != NULL) {
        // send the result bundle back if set, otherwise send an error.
        if (mResultBundle != NULL) {
            return mAccountAuthenticatorResponse->OnResult(mResultBundle);
        }
        else {
            return mAccountAuthenticatorResponse->OnError(IAccountManager::ERROR_CODE_CANCELED,
                    String("canceled"));
        }
        mAccountAuthenticatorResponse = NULL;
    }
    return Activity::Finish();
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
