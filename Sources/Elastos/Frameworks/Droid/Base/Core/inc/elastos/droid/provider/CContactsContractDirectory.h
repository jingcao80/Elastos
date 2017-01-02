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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDIRECTORY_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDIRECTORY_H__

#include "_Elastos_Droid_Provider_CContactsContractDirectory.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractDirectory)
    , public Singleton
    , public IContactsContractDirectory
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table.  Requests to this URI can be
     * performed on the UI thread because they are always unblocking.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Notifies the system of a change in the list of directories handled by
     * a particular directory provider. The Contacts provider will turn around
     * and send a query to the directory provider for the full list of directories,
     * which will replace the previous list.
     */
    CARAPI NotifyDirectoryChange(
        /* [in] */ IContentResolver* resolver);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTDIRECTORY_H__
