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

#include "org/apache/harmony/security/x509/tsp/CPKIStatusInfoHelper.h"
#include "org/apache/harmony/security/x509/tsp/PKIStatusInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CPKIStatusInfoHelper)

CAR_INTERFACE_IMPL(CPKIStatusInfoHelper, Singleton, IPKIStatusInfoHelper)

ECode CPKIStatusInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    return PKIStatusInfo::GetASN1(ppAsn1);
}

ECode CPKIStatusInfoHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    return PKIStatusInfo::SetASN1(pAsn1);
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org