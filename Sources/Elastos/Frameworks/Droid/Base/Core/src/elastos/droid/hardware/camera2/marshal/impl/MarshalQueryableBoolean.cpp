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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableBoolean.h"
#include <Elastos.CoreLibrary.Core.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::ECLSID_CBoolean;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableBoolean::MarshalerBoolean::MarshalerBoolean(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableBoolean* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableBoolean::MarshalerBoolean::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IBoolean> obj = IBoolean::Probe(value);
    Boolean unboxValue;
    obj->GetValue(&unboxValue);
    if (unboxValue) {
        buffer->Put((Byte)1);
    }
    else{
        buffer->Put((Byte)0);
    }
    return NOERROR;
}

ECode MarshalQueryableBoolean::MarshalerBoolean::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Byte value;
    buffer->Get(&value);
    Boolean res = value != 0;

    AutoPtr<IBoolean> obj;
    CBoolean::New(res, (IBoolean**)&obj);
    *outface = TO_IINTERFACE(obj);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableBoolean::MarshalerBoolean::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = IMarshalHelpers::SIZEOF_BYTE;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(MarshalQueryableBoolean, Object,
        IMarshalQueryableBoolean, IMarshalQueryable)

ECode MarshalQueryableBoolean::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerBoolean(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableBoolean::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_BYTE) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CBoolean) {
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
