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

#include "CObjectIdentifierHelper.h"
#include "CObjectIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {
CAR_SINGLETON_IMPL(CObjectIdentifierHelper)

CAR_INTERFACE_IMPL(CObjectIdentifierHelper, Singleton, IObjectIdentifierHelper)

ECode CObjectIdentifierHelper::Validate(
    /* [in] */ ArrayOf<Int32>* oid)
{
    return CObjectIdentifier::Validate(oid);
}

ECode CObjectIdentifierHelper::ToString(
    /* [in] */ ArrayOf<Int32>* oid,
    /* [out] */ String* str)
{
    return CObjectIdentifier::ToString(oid, str);
}

ECode CObjectIdentifierHelper::ToIntArray(
    /* [in] */ const String& str,
    /* [out, callee] */ ArrayOf<Int32>** intArray)
{
    return CObjectIdentifier::ToIntArray(str, intArray);
}

ECode CObjectIdentifierHelper::IsOID(
    /* [in] */ const String& str,
    /* [out] */ Boolean* isOID)
{
    return CObjectIdentifier::IsOID(str, isOID);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

