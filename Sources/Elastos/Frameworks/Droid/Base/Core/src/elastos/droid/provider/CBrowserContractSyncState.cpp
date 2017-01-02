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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CBrowserContractSyncState.h"
#include "elastos/droid/provider/BrowserContract.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractSyncState)

CAR_INTERFACE_IMPL(CBrowserContractSyncState, Object, IBrowserContractSyncState)

ECode CBrowserContractSyncState::GetCONTENT_DIRECTORY(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BrowserContract::SyncState::CONTENT_DIRECTORY;
    return NOERROR;
}

ECode CBrowserContractSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::SyncState::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    return BrowserContract::SyncState::Get(provider, account, value);
}

ECode CBrowserContractSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    return BrowserContract::SyncState::GetWithUri(provider, account, value);
}

ECode CBrowserContractSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    return BrowserContract::SyncState::Set(provider, account, data);
}

ECode CBrowserContractSyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return BrowserContract::SyncState::NewSetOperation(account, data, operation);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
