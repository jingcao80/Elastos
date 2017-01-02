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

#include "ContactUpdateUtils.h"

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

const String ContactUpdateUtils::TAG("ContactUpdateUtils");

void ContactUpdateUtils::SetSuperPrimary(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId)
{
    if (dataId == -1) {
        Logger::E(TAG, "Invalid arguments for setSuperPrimary request");
        return;
    }

    // Update the primary values in the data record.
    AutoPtr<IContentValues> values;
    CContentValues::New(2, (IContentValues**)&values);
    values->Put(IContactsContractDataColumns::IS_SUPER_PRIMARY, 1);
    values->Put(IContactsContractDataColumns::IS_PRIMARY, 1);


    AutoPtr<IContactsContractData> data;
    CContactsContractData::New((IContactsContractData**)&data);
    AutoPtr<IUri> uri;
    data->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> newUri;
    contentUris->WithAppendedId(uri, dataId, (IUri**)&newUri);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->Update(newUri, values, NULL, NULL);
}

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos
