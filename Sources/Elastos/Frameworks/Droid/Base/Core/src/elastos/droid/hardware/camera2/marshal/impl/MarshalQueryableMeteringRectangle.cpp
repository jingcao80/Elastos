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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableMeteringRectangle.h"
#include "elastos/droid/hardware/camera2/params/CMeteringRectangle.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::CMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CMeteringRectangle;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::constructor(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableMeteringRectangle* host)
{
    return Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IMeteringRectangle> rectangle = IMeteringRectangle::Probe(value);

    Int32 xMin, yMin, width, height, weight;
    rectangle->GetX(&xMin);
    rectangle->GetY(&yMin);
    rectangle->GetWidth(&width);
    rectangle->GetHeight(&height);
    rectangle->GetMeteringWeight(&weight);

    Int32 xMax = xMin + width;
    Int32 yMax = yMin + height;

    buffer->PutInt32(xMin);
    buffer->PutInt32(yMin);
    buffer->PutInt32(xMax);
    buffer->PutInt32(yMax);
    buffer->PutInt32(weight);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 xMin, yMin, xMax, yMax, weight;
    buffer->GetInt32(&xMin);
    buffer->GetInt32(&yMin);
    buffer->GetInt32(&xMax);
    buffer->GetInt32(&yMax);
    buffer->GetInt32(&weight);

    Int32 width = xMax - xMin;
    Int32 height = yMax - yMin;

    AutoPtr<IMeteringRectangle> rectangle;
    CMeteringRectangle::New(xMin, yMin, width, height, weight, (IMeteringRectangle**)&rectangle);
    *outface = TO_IINTERFACE(rectangle);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::MarshalerMeteringRectangle::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableMeteringRectangle::SIZE = IMarshalHelpers::SIZEOF_INT32 * 5;

CAR_INTERFACE_IMPL_2(MarshalQueryableMeteringRectangle, Object,
        IMarshalQueryableMeteringRectangle, IMarshalQueryable)

ECode MarshalQueryableMeteringRectangle::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<MarshalerMeteringRectangle> mmr = new MarshalerMeteringRectangle();
    mmr->constructor(managedType, nativeType, this);
    *outmar = mmr.Get();
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableMeteringRectangle::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CMeteringRectangle) {
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
