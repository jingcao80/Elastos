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

#include "elastos/droid/provider/CSyncStateContractHelpers.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSyncStateContractHelpers)

CAR_INTERFACE_IMPL(CSyncStateContractHelpers, Singleton, ISyncStateContractHelpers)

ECode CSyncStateContractHelpers::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

ECode CSyncStateContractHelpers::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

ECode CSyncStateContractHelpers::Insert(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IUri** retUri)
{
    return SyncStateContractHelpers::Insert(provider, uri, account, data, retUri);
}

ECode CSyncStateContractHelpers::Update(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data)
{
    return SyncStateContractHelpers::Update(provider, uri, data);
}

ECode CSyncStateContractHelpers::NewSetOperation(
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return SyncStateContractHelpers::NewSetOperation(uri, account, data, operation);
}

ECode CSyncStateContractHelpers::NewUpdateOperation(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return SyncStateContractHelpers::NewUpdateOperation(uri, data, operation);
}

ECode CSyncStateContractHelpers::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

} //Provider
} //Droid
} //Elastos