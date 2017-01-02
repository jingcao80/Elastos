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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableStreamConfigurationDuration.h"
#include "elastos/droid/hardware/camera2/params/CStreamConfigurationDuration.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Params::CStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfigurationDuration;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableStreamConfigurationDuration::MarshalerStreamConfigurationDuration::MarshalerStreamConfigurationDuration(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableStreamConfigurationDuration* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableStreamConfigurationDuration::MarshalerStreamConfigurationDuration::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IStreamConfigurationDuration> configurationDuration = IStreamConfigurationDuration::Probe(value);

    Int32 format, width, height;
    configurationDuration->GetFormat(&format);
    configurationDuration->GetWidth(&width);
    configurationDuration->GetHeight(&height);
    Int64 duration;
    configurationDuration->GetDuration(&duration);

    buffer->PutInt64(format & MASK_UNSIGNED_INT); // unsigned int -> long
    buffer->PutInt64(width);
    buffer->PutInt64(height);
    return buffer->PutInt64(duration);
}

ECode MarshalQueryableStreamConfigurationDuration::MarshalerStreamConfigurationDuration::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int64 format, width, height;
    buffer->GetInt64(&format);
    buffer->GetInt64(&width);
    buffer->GetInt64(&height);
    Int64 durationNs;
    buffer->GetInt64(&durationNs);

    AutoPtr<IStreamConfigurationDuration> configurationDuration;
    CStreamConfigurationDuration::New(format, width, height, durationNs,
        (IStreamConfigurationDuration**)&configurationDuration);
    *outface = TO_IINTERFACE(configurationDuration);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableStreamConfigurationDuration::MarshalerStreamConfigurationDuration::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableStreamConfigurationDuration::SIZE = IMarshalHelpers::SIZEOF_INT64 * 4;

const Int64 MarshalQueryableStreamConfigurationDuration::MASK_UNSIGNED_INT = 0x00000000ffffffffL;

CAR_INTERFACE_IMPL_2(MarshalQueryableStreamConfigurationDuration, Object,
        IMarshalQueryableStreamConfigurationDuration, IMarshalQueryable)

ECode MarshalQueryableStreamConfigurationDuration::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerStreamConfigurationDuration(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableStreamConfigurationDuration::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT64) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CStreamConfigurationDuration) {
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
