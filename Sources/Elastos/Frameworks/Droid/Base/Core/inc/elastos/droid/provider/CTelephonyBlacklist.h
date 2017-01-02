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

#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__

#include "_Elastos_Droid_Provider_CTelephonyBlacklist.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyBlacklist)
    , public Singleton
    , public ITelephonyBlacklist
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_FILTER_BYNUMBER_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_PHONE_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_MESSAGE_URI(
        /* [out] */ IUri** result);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
