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

#include "elastos/droid/hardware/camera2/utils/CListUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CListUtils, Singleton, IListUtils)

CAR_SINGLETON_IMPL(CListUtils)

ECode CListUtils::ListContains(
    /* [in] */ IList* list,
    /* [in] */ IInterface* needle,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return ListUtils::ListContains(list, needle, value);
}

ECode CListUtils::ListElementsEqualTo(
    /* [in] */ IList* list,
    /* [in] */ IInterface* single,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return ListUtils::ListElementsEqualTo(list, single, value);
}

ECode CListUtils::ListToString(
    /* [in] */ IList* list,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return ListUtils::ListToString(list, str);
}

ECode CListUtils::ListSelectFirstFrom(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<IInterface*>* choices,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    return ListUtils::ListSelectFirstFrom(list, choices, outface);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos