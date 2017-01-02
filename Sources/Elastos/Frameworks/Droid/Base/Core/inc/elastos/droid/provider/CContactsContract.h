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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__

#include "_Elastos_Droid_Provider_CContactsContract.h"
#include "elastos/droid/provider/ContactsContract.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContract)
    , public Singleton
    , public IContactsContract
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /** A content:// style uri to the authority for the contacts provider */
    CARAPI GetAUTHORITY_URI(
        /* [out] */ IUri** uri);

    /**
     * This method can be used to identify whether the given ID is associated with profile
     * data.  It does not necessarily indicate that the ID is tied to valid data, merely
     * that accessing data using this ID will result in profile access checks and will only
     * return data from the profile.
     *
     * @param id The ID to check.
     * @return Whether the ID is associated with profile data.
     */
    CARAPI IsProfileId(
        /* [in] */ Int64 id,
        /* [out] */ Boolean* res);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACT_H__
