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

#include "CASN1OidHelper.h"
#include "ASN1Oid.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1OidHelper)

CAR_INTERFACE_IMPL(CASN1OidHelper, Singleton, IASN1OidHelper)

ECode CASN1OidHelper::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    return ASN1Oid::GetInstance(instance);
}

ECode CASN1OidHelper::GetInstanceForString(
    /* [out] */ IASN1Type** instance)
{
    return ASN1Oid::GetInstanceForString(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

