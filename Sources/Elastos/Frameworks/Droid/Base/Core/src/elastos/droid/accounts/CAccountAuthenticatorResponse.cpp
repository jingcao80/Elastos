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

#include "elastos/droid/accounts/CAccountAuthenticatorResponse.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

const String CAccountAuthenticatorResponse::TAG("AccountAuthenticator");

CAR_OBJECT_IMPL(CAccountAuthenticatorResponse)

CAR_INTERFACE_IMPL(CAccountAuthenticatorResponse, Object, IAccountAuthenticatorResponse)

ECode CAccountAuthenticatorResponse::OnResult(
    /* [in] */ IBundle* result)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     result.keySet(); // force it to be unparcelled
    //     Log.v(TAG, "AccountAuthenticatorResponse.onResult: "
    //             + AccountManager.sanitizeResult(result));
    // }
    // try {
    return mAccountAuthenticatorResponse->OnResult(result);
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::OnRequestContinued()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "AccountAuthenticatorResponse.onRequestContinued");
    // }
    // try {
    return mAccountAuthenticatorResponse->OnRequestContinued();
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "AccountAuthenticatorResponse.onRequestContinued");
    // }
    // try {
    return mAccountAuthenticatorResponse->OnRequestContinued();
    // } catch (RemoteException e) {
    //     // this should never happen
    // }
}

ECode CAccountAuthenticatorResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mAccountAuthenticatorResponse);
}

ECode CAccountAuthenticatorResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr(
            (Handle32*)(IIAccountAuthenticatorResponse**)&mAccountAuthenticatorResponse);
}

ECode CAccountAuthenticatorResponse::constructor(
    /* [in] */ IIAccountAuthenticatorResponse* response)
{
    mAccountAuthenticatorResponse = response;
    return NOERROR;
}

ECode CAccountAuthenticatorResponse::constructor()
{
    return NOERROR;
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
