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

#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractSyncState.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractSyncState)

CAR_INTERFACE_IMPL_2(CContactsContractSyncState, Singleton, IContactsContractSyncState, ISyncStateContractColumns)

ECode CContactsContractSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), IContactsContractSyncState::CONTENT_DIRECTORY, uri);
}

ECode CContactsContractSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#get
 */
ECode CContactsContractSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

ECode CContactsContractSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

ECode CContactsContractSyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::NewSetOperation(uri, account, data, operation);
}

} //Provider
} //Droid
} //Elastos