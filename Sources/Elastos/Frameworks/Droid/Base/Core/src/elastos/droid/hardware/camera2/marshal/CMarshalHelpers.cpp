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

#include "elastos/droid/hardware/camera2/marshal/CMarshalHelpers.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

CAR_INTERFACE_IMPL(CMarshalHelpers, Singleton, IMarshalHelpers)

CAR_SINGLETON_IMPL(CMarshalHelpers)

ECode CMarshalHelpers::GetPrimitiveTypeSize(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::GetPrimitiveTypeSize(nativeType, value);
}

ECode CMarshalHelpers::CheckPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcla)
{
    VALIDATE_NOT_NULL(outcla);

    return MarshalHelpers::CheckPrimitiveClass(klass, outcla);
}

ECode CMarshalHelpers::IsPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::IsPrimitiveClass(klass, value);
}

ECode CMarshalHelpers::WrapClassIfPrimitive(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcl)
{
    VALIDATE_NOT_NULL(outcl);

    return MarshalHelpers::WrapClassIfPrimitive(klass, outcl);
}

ECode CMarshalHelpers::ToStringNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return MarshalHelpers::ToStringNativeType(nativeType, str);
}

ECode CMarshalHelpers::CheckNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::CheckNativeType(nativeType, value);
}

ECode CMarshalHelpers::CheckNativeTypeEquals(
    /* [in] */ Int32 expectedNativeType,
    /* [in] */ Int32 actualNativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::CheckNativeTypeEquals(expectedNativeType, actualNativeType, value);
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
