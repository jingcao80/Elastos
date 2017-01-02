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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROFILE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROFILE_H__

#include "_Elastos_Droid_Provider_CContactsContractProfile.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractProfile)
    , public Singleton
    , public IContactsContractProfile
    , public IBaseColumns
    , public IContactsContractContactsColumns
    , public IContactsContractContactOptionsColumns
    , public IContactsContractContactNameColumns
    , public IContactsContractContactStatusColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table, which requests the contact entry
     * representing the user's personal profile data.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * {@link Uri} for referencing the user's profile {@link Contacts} entry,
     * Provides {@link OpenableColumns} columns when queried, or returns the
     * user's profile contact formatted as a vCard when opened through
     * {@link ContentResolver#openAssetFileDescriptor(Uri, String)}.
     */
    CARAPI GetCONTENT_VCARD_URI(
        /* [out] */ IUri** uri);

    /**
     * {@link Uri} for referencing the raw contacts that make up the user's profile
     * {@link Contacts} entry.  An individual raw contact entry within the profile
     * can be addressed by appending the raw contact ID.  The entities or data within
     * that specific raw contact can be requested by appending the entity or data
     * path as well.
     */
    CARAPI GetCONTENT_RAW_CONTACTS_URI(
        /* [out] */ IUri** uri);

    /**
     * The minimum ID for any entity that belongs to the profile.  This essentially
     * defines an ID-space in which profile data is stored, and is used by the provider
     * to determine whether a request via a non-profile-specific URI should be directed
     * to the profile data rather than general contacts data, along with all the special
     * permission checks that entails.
     *
     * Callers may use {@link #isProfileId} to check whether a specific ID falls into
     * the set of data intended for the profile.
     */
    CARAPI GetMINID(
        /* [out] */ Int64* id);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROFILE_H__
