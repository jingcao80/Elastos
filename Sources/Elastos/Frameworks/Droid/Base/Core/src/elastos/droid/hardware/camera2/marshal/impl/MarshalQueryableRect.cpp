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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableRect.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ECLSID_CRect;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

ECode MarshalQueryableRect::MarshalerRect::constructor(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableRect* host)
{
    return Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableRect::MarshalerRect::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    AutoPtr<IRect> rect = IRect::Probe(value);
    Int32 left, top, width, height;
    rect->GetLeft(&left);
    rect->GetTop(&top);
    rect->GetWidth(&width);
    rect->GetHeight(&height);

    buffer->PutInt32(left);
    buffer->PutInt32(top);
    buffer->PutInt32(width);
    buffer->PutInt32(height);
    return NOERROR;
}

ECode MarshalQueryableRect::MarshalerRect::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 left, top, width, height;
    buffer->GetInt32(&left);
    buffer->GetInt32(&top);
    buffer->GetInt32(&width);
    buffer->GetInt32(&height);

    Int32 right = left + width;
    Int32 bottom = top + height;

    AutoPtr<IRect> rect;
    CRect::New(left, top, right, bottom, (IRect**)&rect);
    *outface = TO_IINTERFACE(rect);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableRect::MarshalerRect::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableRect::SIZE = IMarshalHelpers::SIZEOF_INT32 * 4;

CAR_INTERFACE_IMPL_2(MarshalQueryableRect, Object,
        IMarshalQueryableRect, IMarshalQueryable)

ECode MarshalQueryableRect::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<MarshalerRect> mr = new MarshalerRect();
    mr->constructor(managedType, nativeType, this);
    *outmar = mr.Get();
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableRect::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CRect) {
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
