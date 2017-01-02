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

#ifndef __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__
#define __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class ContactsContractData
{
public:
    /**
     * The content:// style URI for this table, which requests a directory
     * of data rows matching the selection criteria.
     */
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * <p>
     * Build a {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}
     * style {@link Uri} for the parent {@link android.provider.ContactsContract.Contacts}
     * entry of the given {@link ContactsContract.Data} entry.
     * </p>
     * <p>
     * Returns the Uri for the contact in the first entry returned by
     * {@link ContentResolver#query(Uri, String[], String, String[], String)}
     * for the provided {@code dataUri}.  If the query returns null or empty
     * results, silently returns null.
     * </p>
     */
    static CARAPI GetContactLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* dataUri,
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTDATA_H__
