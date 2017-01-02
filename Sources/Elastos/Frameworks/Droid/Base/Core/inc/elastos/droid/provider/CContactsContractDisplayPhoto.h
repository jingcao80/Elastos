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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDISPLAYPHOTO_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDISPLAYPHOTO_H__

#include "_Elastos_Droid_Provider_CContactsContractDisplayPhoto.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractDisplayPhoto)
    , public Singleton
    , public IContactsContractDisplayPhoto
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this class, which allows access to full-size photos,
     * given a key.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * This URI allows the caller to query for the maximum dimensions of a display photo
     * or thumbnail.  Requests to this URI can be performed on the UI thread because
     * they are always unblocking.
     */
    CARAPI GetCONTENTMAXDIMENSIONSURI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDISPLAYPHOTO_H__
