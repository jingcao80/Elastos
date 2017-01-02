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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableRange.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/utility/CRange.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Marshal::MarshalRegistry;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::IRange;
using Elastos::Droid::Utility::CRange;
using Elastos::Droid::Utility::ECLSID_CRange;
using Elastos::Droid::Utility::EIID_IRange;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableRange::MarshalerRange::MarshalerRange(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableRange* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    AutoPtr<ITypeReference> actualTypeArgToken;
    typeReference->GetComponent((ITypeReference**)&actualTypeArgToken);

    MarshalRegistry::GetMarshaler(actualTypeArgToken, mNativeType, (IMarshaler**)&mNestedTypeMarshaler);
}

ECode MarshalQueryableRange::MarshalerRange::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    IRange* range = IRange::Probe(value);
    AutoPtr<IInterface> lower, upper;
    range->GetLower((IInterface**)&lower);
    range->GetUpper((IInterface**)&upper);

    mNestedTypeMarshaler->Marshal(lower, buffer);
    mNestedTypeMarshaler->Marshal(upper, buffer);
    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    AutoPtr<IInterface> lower, upper;
    mNestedTypeMarshaler->Unmarshal(buffer, (IInterface**)&lower);
    mNestedTypeMarshaler->Unmarshal(buffer, (IInterface**)&upper);

    AutoPtr<ITypeReference> component;
    mTypeReference->GetComponent((ITypeReference**)&component);
    InterfaceID componentType;
    component->GetInterfaceType(&componentType);
    AutoPtr<IRange> range;
    CRange::New(componentType, lower, upper, (IRange**)&range);

    *outface = range;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 nestedSize;
    mNestedTypeMarshaler->GetNativeSize(&nestedSize);

    if (nestedSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *value = nestedSize * RANGE_COUNT;
        return NOERROR;
    }
    else {
        *value = IMarshaler::NATIVE_SIZE_DYNAMIC;
        return NOERROR;
    }

    return NOERROR;
}

ECode MarshalQueryableRange::MarshalerRange::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(value);
    *outvalue = 0;

    Int32 nativeSize;
    GetNativeSize(&nativeSize);

    if (nativeSize != IMarshaler::NATIVE_SIZE_DYNAMIC) {
        *outvalue = nativeSize;
    }
    else {
        IRange* range = IRange::Probe(value);
        AutoPtr<IInterface> lower, upper;
        range->GetLower((IInterface**)&lower);
        range->GetUpper((IInterface**)&upper);

        Int32 lowerSize, upperSize;
        mNestedTypeMarshaler->CalculateMarshalSize(lower, &lowerSize);
        mNestedTypeMarshaler->CalculateMarshalSize(upper, &upperSize);
        *outvalue = lowerSize + upperSize;
    }
    return NOERROR;
}

const Int32 MarshalQueryableRange::RANGE_COUNT = 2;

CAR_INTERFACE_IMPL_2(MarshalQueryableRange, Object,
        IMarshalQueryableRange, IMarshalQueryable)

ECode MarshalQueryableRange::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerRange(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableRange::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    ClassID cls;
    managedType->GetClassType(&cls);
    if (cls == ECLSID_CRange) {
        *value = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
