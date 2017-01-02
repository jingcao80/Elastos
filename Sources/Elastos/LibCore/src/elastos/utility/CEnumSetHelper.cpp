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

#include "CEnumSetHelper.h"
#include "EnumSet.h"

namespace Elastos{
namespace Utility{

CAR_SINGLETON_IMPL(CEnumSetHelper)

CAR_INTERFACE_IMPL(CEnumSetHelper, Singleton, IEnumSetHelper)

ECode CEnumSetHelper::NoneOf(
    /* [in] */ const InterfaceID& type,
    /* [out] */ IEnumSet** res)
{
    return EnumSet::NoneOf(type, res);
}

ECode CEnumSetHelper::AllOf(
    /* [in] */ const InterfaceID& type,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::AllOf(type);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::CopyOf(
    /* [in] */ IEnumSet* s,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::CopyOf(s);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::CopyOf(
    /* [in] */ ICollection* c,
    /* [out] */ IEnumSet** set)
{
    return EnumSet::CopyOf(c, set);
}

ECode CEnumSetHelper::ComplementOf(
    /* [in] */ IEnumSet* s,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::ComplementOf(s);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3, i4);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4,
    /* [in] */ IInterface* i5,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3, i4, i5);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(array);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Range(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ IEnumSet** set)
{
    return EnumSet::Range(start, end, set);
}

} // namespace Utility
} // namespace Elastos