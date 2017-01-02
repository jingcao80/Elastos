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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEARRAY_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEARRAY_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

class MarshalQueryableArray
    : public Object
    , public IMarshalQueryableArray
    , public IMarshalQueryable
{
private:
    class MarshalerArray
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalQueryableArray::MarshalerArray")

        //@Override
        CARAPI Marshal(
            /* [in] */ IInterface* value,
            /* [in] */ IByteBuffer* buffer);

        //@Override
        CARAPI Unmarshal(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ IInterface** outface);

        //@Override
        CARAPI GetNativeSize(
            /* [out] */ Int32* value);

        //@Override
        CARAPI CalculateMarshalSize(
            /* [in] */ IInterface* value,
            /* [out] */ Int32* outvalue);

    protected:
        friend class MarshalQueryableArray;

        MarshalerArray(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableArray* host);

    private:
        /*
         * Helpers to avoid compiler errors regarding types with wildcards (?)
         */
        //@SuppressWarnings("unchecked")
        CARAPI MarshalArrayElement(
            /* [in] */ IMarshaler* marshaler,
            /* [in] */ IByteBuffer* buffer,
            /* [in] */ IInterface* element);

        //@SuppressWarnings("unchecked")
        CARAPI_(AutoPtr<IInterface>) CopyListToArray(
            /* [in] */ IArrayList* arrayList);

        //@SuppressWarnings("unchecked")
        CARAPI_(Int32) CalculateElementMarshalSize(
            /* [in] */ IMarshaler* marshaler,
            /* [in] */ IInterface* element);

    private:
        AutoPtr<IMarshaler> mComponentMarshaler;
        AutoPtr<ITypeReference> mComponent;
    };

public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI CreateMarshaler(
        /* [in] */ ITypeReference* managedType,
        /* [in] */ Int32 nativeType,
        /* [out] */ IMarshaler** outmar);

    //@Override
    CARAPI IsTypeMappingSupported(
        /* [in] */ ITypeReference* managedType,
        /* [in] */ Int32 nativeType,
        /* [out] */ Boolean* value);

private:
    static const String TAG;
    static const Boolean VERBOSE;
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEARRAY_H__