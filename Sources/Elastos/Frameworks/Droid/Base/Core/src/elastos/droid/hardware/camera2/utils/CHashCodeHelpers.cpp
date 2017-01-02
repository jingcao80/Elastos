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

#include "elastos/droid/hardware/camera2/utils/CHashCodeHelpers.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CHashCodeHelpers, Singleton, IHashCodeHelpers)

CAR_SINGLETON_IMPL(CHashCodeHelpers)

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<Int32>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<Float>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [in] */ IInterface* c,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, c, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [in] */ IInterface* c,
    /* [in] */ IInterface* d,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, c, d, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, w, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w,
    /* [in] */ Int32 t,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, w, t, value);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos