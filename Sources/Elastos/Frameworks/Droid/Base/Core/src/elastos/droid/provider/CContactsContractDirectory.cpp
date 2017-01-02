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

#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractDirectory.h"
#include "elastos/droid/provider/ContactsContract.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractDirectory)

CAR_INTERFACE_IMPL(CContactsContractDirectory, Singleton, IContactsContractDirectory)

ECode CContactsContractDirectory::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("directories"), uri);
}

ECode CContactsContractDirectory::NotifyDirectoryChange(
    /* [in] */ IContentResolver* resolver)
{
    // This is done to trigger a query by Contacts Provider back to the directory provider.
    // No data needs to be sent back, because the provider can infer the calling
    // package from binder.
    AutoPtr<IContentValues> contentValues;
    CContentValues::New((IContentValues**)&contentValues);

    AutoPtr<IUri> auUri;
    GetCONTENT_URI((IUri**)&auUri);
    Int32 result;
    return resolver->Update(auUri, contentValues, String(NULL), NULL, &result);
}

} //Provider
} //Droid
} //Elastos