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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableNativeByteToInteger.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::IInteger32;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableNativeByteToInteger::MarshalerNativeByteToInteger::MarshalerNativeByteToInteger(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableNativeByteToInteger* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableNativeByteToInteger::MarshalerNativeByteToInteger::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IInteger32> obj = IInteger32::Probe(value);
    Int32 _value;
    obj->GetValue(&_value);
    return buffer->Put((Byte)_value); // truncate down to byte
}

ECode MarshalQueryableNativeByteToInteger::MarshalerNativeByteToInteger::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    // expand unsigned byte to int; avoid sign extension
    Byte value;
    buffer->Get(&value);
    AutoPtr<IInteger32> obj = CoreUtils::Convert(value & UINT8_MASK);
    *outface = TO_IINTERFACE(obj);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableNativeByteToInteger::MarshalerNativeByteToInteger::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = IMarshalHelpers::SIZEOF_BYTE;
    return NOERROR;
}

const Int32 MarshalQueryableNativeByteToInteger::UINT8_MASK = (1 << Elastos::Core::Math::BYTE_SIZE) - 1;

CAR_INTERFACE_IMPL_2(MarshalQueryableNativeByteToInteger, Object,
        IMarshalQueryableNativeByteToInteger, IMarshalQueryable)

ECode MarshalQueryableNativeByteToInteger::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerNativeByteToInteger(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableNativeByteToInteger::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_BYTE) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CInteger32) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
