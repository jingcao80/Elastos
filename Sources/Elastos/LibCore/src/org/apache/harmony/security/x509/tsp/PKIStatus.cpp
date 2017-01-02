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

#include "org/apache/harmony/security/x509/tsp/PKIStatus.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_INTERFACE_IMPL(PKIStatus, Object, IPKIStatus)

PKIStatus::PKIStatus(
    /* [in] */ Int32 status)
    : mStatus(status)
{
}

ECode PKIStatus::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = mStatus;
    return NOERROR;
}

ECode PKIStatus::GetInstance(
    /* [in] */ Int32 status,
    /* [out] */ IPKIStatus** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<IPKIStatus> tmp;
    switch (status) {
        case PKIStatus_GRANTED :
            tmp = new PKIStatus(PKIStatus_GRANTED);
            break;
        case PKIStatus_GRANTED_WITH_MODS :
            tmp = new PKIStatus(PKIStatus_GRANTED_WITH_MODS);
            break;
        case PKIStatus_REJECTION :
            tmp = new PKIStatus(PKIStatus_REJECTION);
            break;
        case PKIStatus_WAITING :
            tmp = new PKIStatus(PKIStatus_WAITING);
            break;
        case PKIStatus_REVOCATION_WARNING :
            tmp = new PKIStatus(PKIStatus_REVOCATION_WARNING);
            break;
        case PKIStatus_REVOCATION_NOTIFICATION :
            tmp = new PKIStatus(PKIStatus_REVOCATION_NOTIFICATION);
            break;
        default: {
            *instance = NULL;
            return E_INVALID_PARAMETER_EXCEPTION;
        }
    }
    *instance = tmp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org