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

#include "CCollationElementIteratorICUHelper.h"
#include "CollationElementIteratorICU.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CCollationElementIteratorICUHelper)

CAR_INTERFACE_IMPL(CCollationElementIteratorICUHelper, Singleton, ICollationElementIteratorICUHelper)

ECode CCollationElementIteratorICUHelper::PrimaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = CollationElementIteratorICU::PrimaryOrder(order);
    return NOERROR;
}

ECode CCollationElementIteratorICUHelper::SecondaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = CollationElementIteratorICU::SecondaryOrder(order);
    return NOERROR;
}

ECode CCollationElementIteratorICUHelper::TertiaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = CollationElementIteratorICU::TertiaryOrder(order);
    return NOERROR;
}

ECode CCollationElementIteratorICUHelper::GetInstance(
    /* [in] */ Int64 collatorAddress,
    /* [in] */ const String& source,
    /* [out] */ ICollationElementIteratorICU ** outiterICU)
{
    VALIDATE_NOT_NULL(outiterICU)
    AutoPtr<ICollationElementIteratorICU> icu = CollationElementIteratorICU::GetInstance(collatorAddress,source);
    *outiterICU = icu;
    REFCOUNT_ADD(*outiterICU);
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
