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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableStreamConfiguration.h"
#include "elastos/droid/hardware/camera2/params/CStreamConfiguration.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::CStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfiguration;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableStreamConfiguration::MarshalerStreamConfiguration::MarshalerStreamConfiguration(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableStreamConfiguration* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableStreamConfiguration::MarshalerStreamConfiguration::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IStreamConfiguration> configuration = IStreamConfiguration::Probe(value);

    Int32 format;
    configuration->GetFormat(&format);
    buffer->PutInt32(format);
    Int32 width;
    configuration->GetWidth(&width);
    buffer->PutInt32(width);
    Int32 height;
    configuration->GetHeight(&height);
    buffer->PutInt32(height);
    Boolean isInput;
    configuration->IsInput(&isInput);
    if (isInput) {
        buffer->PutInt32(1);
    }
    else {
        buffer->PutInt32(0);
    }
    return NOERROR;
}

ECode MarshalQueryableStreamConfiguration::MarshalerStreamConfiguration::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 format;
    buffer->GetInt32(&format);
    Int32 width;
    buffer->GetInt32(&width);
    Int32 height;
    buffer->GetInt32(&height);
    Int32 isInput;
    buffer->GetInt32(&isInput);
    Boolean input = isInput != 0;

    AutoPtr<IStreamConfiguration> configuration;
    CStreamConfiguration::New(format, width, height, input, (IStreamConfiguration**)&configuration);
    *outface = TO_IINTERFACE(configuration);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableStreamConfiguration::MarshalerStreamConfiguration::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableStreamConfiguration::SIZE = IMarshalHelpers::SIZEOF_INT32 * 4;

CAR_INTERFACE_IMPL_2(MarshalQueryableStreamConfiguration, Object,
        IMarshalQueryableStreamConfiguration, IMarshalQueryable)

ECode MarshalQueryableStreamConfiguration::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerStreamConfiguration(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableStreamConfiguration::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CStreamConfiguration) {
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
