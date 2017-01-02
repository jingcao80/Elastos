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

#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Marshal::MarshalHelpers;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

CAR_INTERFACE_IMPL(Marshaler, Object, IMarshaler)

Marshaler::Marshaler()
    : mNativeType(0)
{
}

ECode Marshaler::constructor(
    /* [in] */ IMarshalQueryable* query,
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType)
{
    FAIL_RETURN(Preconditions::CheckNotNull(typeReference, String("typeReference must not be null")))
    FAIL_RETURN(MarshalHelpers::CheckNativeType(nativeType, &mNativeType))
    mTypeReference = typeReference;

    Boolean result;
    query->IsTypeMappingSupported(typeReference, nativeType, &result);
    if (!result) {
        String type;
        MarshalHelpers::ToStringNativeType(nativeType, &type);
        Logger::E("Marshaler", "Unsupported type marshaling for managed "
            "type %s and native type %s", TO_CSTR(typeReference), type.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode Marshaler::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    *outvalue = 0;

    Int32 nativeSize;
    GetNativeSize(&nativeSize);

    if (nativeSize == IMarshaler::NATIVE_SIZE_DYNAMIC) {
        Logger::E("Marshaler", "Override this function for dynamically-sized objects");
        return E_ASSERTION_ERROR;
    }

    *outvalue = nativeSize;
    return NOERROR;
}

ECode Marshaler::GetTypeReference(
    /* [out] */ ITypeReference** outtr)
{
    VALIDATE_NOT_NULL(outtr);

    *outtr = mTypeReference;
    REFCOUNT_ADD(*outtr);
    return NOERROR;
}

ECode Marshaler::GetNativeType(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mNativeType;
    return NOERROR;
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
