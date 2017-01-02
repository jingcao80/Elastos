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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATAUSAGEFEEDBACK_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATAUSAGEFEEDBACK_H__

#include "_Elastos_Droid_Provider_CContactsContractDataUsageFeedback.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractDataUsageFeedback)
    , public Singleton
    , public IContactsContractDataUsageFeedback
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for sending usage feedback.
     * Must be used with {@link ContentResolver#update(Uri, ContentValues, String, String[])}.
     */
    CARAPI GetFEEDBACKURI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for deleting all usage information.
     * Must be used with {@link ContentResolver#delete(Uri, String, String[])}.
     * The {@code where} and {@code selectionArgs} parameters are ignored.
     */
    CARAPI GetDELETEUSAGEURI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDATAUSAGEFEEDBACK_H__
