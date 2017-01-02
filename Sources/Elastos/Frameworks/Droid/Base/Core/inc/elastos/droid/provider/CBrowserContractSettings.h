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

#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__

#include "_Elastos_Droid_Provider_CBrowserContractSettings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractSettings)
    , public Singleton
    , public IBrowserContractSettings
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns true if bookmark sync is enabled
     */
    CARAPI IsSyncEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    /**
     * Sets the bookmark sync enabled setting.
     */
    CARAPI SetSyncEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__
