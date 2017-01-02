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

#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__

#include "_Elastos_Droid_Service_Fingerprint_CFingerprintUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CarClass(CFingerprintUtils)
    , public Singleton
    , public IFingerprintUtils
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFingerprintIdsForUser(
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI AddFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId);

    CARAPI RemoveFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* ret);
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__
