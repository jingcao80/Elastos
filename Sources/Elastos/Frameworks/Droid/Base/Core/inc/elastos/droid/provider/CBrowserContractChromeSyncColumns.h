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

#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__

#include "_Elastos_Droid_Provider_CBrowserContractChromeSyncColumns.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractChromeSyncColumns)
    , public Singleton
    , public IBrowserContractChromeSyncColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSERVER_UNIQUE(
        /* [out] */ String* result);

    CARAPI GetCLIENT_UNIQUE(
        /* [out] */ String* result);

public:
    static const String SERVER_UNIQUE;
    static const String CLIENT_UNIQUE;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__
