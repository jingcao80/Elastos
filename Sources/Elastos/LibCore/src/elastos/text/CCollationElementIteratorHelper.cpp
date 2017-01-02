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

#include "CCollationElementIteratorHelper.h"
#include "CCollationElementIteratorICUHelper.h"

using Libcore::ICU::ICollationElementIteratorICUHelper;
using Libcore::ICU::CCollationElementIteratorICUHelper;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CCollationElementIteratorHelper, Singleton, ICollationElementIteratorHelper)

CAR_SINGLETON_IMPL(CCollationElementIteratorHelper)

ECode CCollationElementIteratorHelper::PrimaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int32* primaryOrder)
{
    VALIDATE_NOT_NULL(primaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper**)&icuhelper));
    return icuhelper->PrimaryOrder(order,primaryOrder);
}

ECode CCollationElementIteratorHelper::SecondaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int16* secondaryOrder)
{
    VALIDATE_NOT_NULL(secondaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper**)&icuhelper));
    Int32 value(0);
    icuhelper->SecondaryOrder(order,&value);
    *secondaryOrder = (Int16)value;

    return NOERROR;
}

ECode CCollationElementIteratorHelper::TertiaryOrder(
    /* [in] */ Int32 order,
    /* [out] */ Int16* tertiaryOrder)
{
    VALIDATE_NOT_NULL(tertiaryOrder);
    AutoPtr<ICollationElementIteratorICUHelper> icuhelper;
    FAIL_RETURN(CCollationElementIteratorICUHelper::AcquireSingleton((ICollationElementIteratorICUHelper**)&icuhelper));
    Int32 value(0);
    icuhelper->TertiaryOrder(order,&value);
    *tertiaryOrder = (Int16)value;

    return NOERROR;
}

} // namespace Text
} // namespace Elastos