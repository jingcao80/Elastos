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

#include "elastos/droid/service/fingerprint/CFingerprintUtils.h"
#include "elastos/droid/service/fingerprint/FingerprintUtils.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CAR_SINGLETON_IMPL(CFingerprintUtils)

CAR_INTERFACE_IMPL(CFingerprintUtils, Singleton, IFingerprintUtils)

ECode CFingerprintUtils::GetFingerprintIdsForUser(
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    return FingerprintUtils::GetFingerprintIdsForUser(res, userId, ids);
}

ECode CFingerprintUtils::AddFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId)
{
    return FingerprintUtils::AddFingerprintIdForUser(fingerId, res, userId);
}

ECode CFingerprintUtils::RemoveFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* ret)
{
    return FingerprintUtils::RemoveFingerprintIdForUser(fingerId, res, userId, ret);
}

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos