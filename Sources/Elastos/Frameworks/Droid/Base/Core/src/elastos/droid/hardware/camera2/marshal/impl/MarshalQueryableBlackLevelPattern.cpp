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

#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableBlackLevelPattern.h"
#include "elastos/droid/hardware/camera2/params/CBlackLevelPattern.h"
#include "elastos/droid/utility/CSize.h"

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IBlackLevelPattern;
using Elastos::Droid::Hardware::Camera2::Params::CBlackLevelPattern;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CBlackLevelPattern;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::CSize;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableBlackLevelPattern::MarshalerBlackLevelPattern::MarshalerBlackLevelPattern(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableBlackLevelPattern* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableBlackLevelPattern::MarshalerBlackLevelPattern::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    for (Int32 i = 0; i < IBlackLevelPattern::COUNT / 2; ++i) {
        for (Int32 j = 0; j < IBlackLevelPattern::COUNT / 2; ++j) {
            AutoPtr<IBlackLevelPattern> pattern = IBlackLevelPattern::Probe(value);
            Int32 val;
            pattern->GetOffsetForIndex(j, i, &val);
            buffer->PutInt32(val);
        }
    }
    return NOERROR;
}

ECode MarshalQueryableBlackLevelPattern::MarshalerBlackLevelPattern::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<ArrayOf<Int32> > channelOffsets = ArrayOf<Int32>::Alloc(IBlackLevelPattern::COUNT);
    for (Int32 i = 0; i < IBlackLevelPattern::COUNT; ++i) {
        Int32 val;
        buffer->GetInt32(&val);
        (*channelOffsets)[i] = val;
    }

    AutoPtr<IBlackLevelPattern> pattern;
    CBlackLevelPattern::New(channelOffsets, (IBlackLevelPattern**)&pattern);
    *outface = TO_IINTERFACE(pattern);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableBlackLevelPattern::MarshalerBlackLevelPattern::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = SIZE;
    return NOERROR;
}

const Int32 MarshalQueryableBlackLevelPattern::SIZE = IMarshalHelpers::SIZEOF_INT32 * IBlackLevelPattern::COUNT;

CAR_INTERFACE_IMPL_2(MarshalQueryableBlackLevelPattern, Object,
        IMarshalQueryableBlackLevelPattern, IMarshalQueryable)

ECode MarshalQueryableBlackLevelPattern::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerBlackLevelPattern(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableBlackLevelPattern::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CBlackLevelPattern) {
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
