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
#include "elastos/droid/accounts/AccountManagerResponse.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_INTERFACE_IMPL(AccountManagerResponse, Object, IParcelable);

AccountManagerResponse::AccountManagerResponse(
    /* [in] */ IAccountManagerResponse* response)
    : mResponse(response)
{}

AccountManagerResponse::AccountManagerResponse(
    /* [in] */ IParcel* parcel)
{
    ASSERT_SUCCEEDED(parcel->ReadInterfacePtr(
            (Handle32*)(IAccountManagerResponse**)&mResponse));
}

ECode AccountManagerResponse::OnResult(
    /* [in] */ IBundle* result)
{
    return mResponse->OnResult(result);
}

ECode AccountManagerResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return mResponse->OnError(errorCode, errorMessage);
}

ECode AccountManagerResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mResponse);
}

ECode AccountManagerResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr((Handle32*)(IAccountManagerResponse**)&mResponse);
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
