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

#include "elastos/droid/hardware/camera2/utils/CArrayUtils.h"
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CArrayUtils, Singleton, IArrayUtils)

CAR_SINGLETON_IMPL(CArrayUtils)

ECode CArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return ArrayUtils::GetArrayIndex(array, needle, value);
}

ECode CArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return ArrayUtils::GetArrayIndex(array, needle, value);
}

ECode CArrayUtils::ConvertStringListToIntArray(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    return ArrayUtils::ConvertStringListToIntArray(list, convertFrom, convertTo, outarr);
}

ECode CArrayUtils::ConvertStringListToIntList(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    return ArrayUtils::ConvertStringListToIntList(list, convertFrom, convertTo, outlist);
}

ECode CArrayUtils::ToIntArray(
    /* [in] */ IList* list,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    return ArrayUtils::ToIntArray(list, outarr);
}

ECode CArrayUtils::Contains(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return ArrayUtils::Contains(array, elem, result);
}

ECode CArrayUtils::Contains(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return ArrayUtils::Contains(array, elem, result);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos