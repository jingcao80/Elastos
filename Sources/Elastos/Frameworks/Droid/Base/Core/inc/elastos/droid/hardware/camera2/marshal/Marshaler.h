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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALRE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALRE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Utils::ITypeReference;
using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

class Marshaler
    : public Object
    , public IMarshaler
{
public:
    CAR_INTERFACE_DECL()

    Marshaler();

    virtual ~Marshaler() {}

    /**
     * Marshal the specified object instance (value) into a byte buffer.
     *
     * <p>Upon completion, the {@link ByteBuffer#position()} will have advanced by
     * the {@link #calculateMarshalSize marshal size} of {@code value}.</p>
     *
     * @param value the value of type T that we wish to write into the byte buffer
     * @param buffer the byte buffer into which the marshaled object will be written
     */
    virtual CARAPI Marshal(
        /* [in] */ IInterface* value,
        /* [in] */ IByteBuffer* buffer) = 0;

    /**
     * Unmarshal a new object instance from the byte buffer into its managed type.
     *
     * <p>Upon completion, the {@link ByteBuffer#position()} will have advanced by
     * the {@link #calculateMarshalSize marshal size} of the returned {@code T} instance.</p>
     *
     * @param buffer the byte buffer, from which we will read the object
     * @return a new instance of type T read from the byte buffer
     */
    virtual CARAPI Unmarshal(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ IInterface** outface) = 0;

    /**
     * How many bytes a single instance of {@code T} will take up if marshalled to/from
     * {@code nativeType}.
     *
     * <p>When unmarshaling data from native to managed, the instance {@code T} is not yet
     * available. If the native size is always a fixed mapping regardless of the instance of
     * {@code T} (e.g. if the type is not a container of some sort), it can be used to preallocate
     * containers for {@code T} to avoid resizing them.</p>
     *
     * <p>In particular, the array marshaler takes advantage of this (when size is not dynamic)
     * to preallocate arrays of the right length when unmarshaling an array {@code T[]}.</p>
     *
     * @return a size in bytes, or {@link #NATIVE_SIZE_DYNAMIC} if the size is dynamic
     */
    virtual CARAPI GetNativeSize(
        /* [out] */ Int32* value) = 0;

    /**
     * Get the size in bytes for how much space would be required to write this {@code value}
     * into a byte buffer using the given {@code nativeType}.
     *
     * <p>If the size of this {@code T} instance when serialized into a buffer is always constant,
     * then this method will always return the same value (and particularly, it will return
     * an equivalent value to {@link #getNativeSize()}.</p>
     *
     * <p>Overriding this method is a must when the size is {@link NATIVE_SIZE_DYNAMIC dynamic}.</p>
     *
     * @param value the value of type T that we wish to write into the byte buffer
     * @return the size that would need to be written to the byte buffer
     */
    virtual CARAPI CalculateMarshalSize(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* outvalue);

    /**
     * The type reference for {@code T} for the managed type side of this marshaler.
     */
    CARAPI GetTypeReference(
        /* [out] */ ITypeReference** outtr);

    /** The native type corresponding to this marshaler for the native side of this marshaler.*/
    CARAPI GetNativeType(
        /* [out] */ Int32* value);

protected:
    /**
     * Instantiate a marshaler between a single managed/native type combination.
     *
     * <p>This particular managed/native type combination must be supported by
     * {@link #isTypeMappingSupported}.</p>
     *
     * @param query an instance of {@link MarshalQueryable}
     * @param typeReference the managed type reference
     *        Must be one for which {@link #isTypeMappingSupported} returns {@code true}
     * @param nativeType the native type, e.g.
     *        {@link android.hardware.camera2.impl.CameraMetadataNative#TYPE_BYTE TYPE_BYTE}.
     *        Must be one for which {@link #isTypeMappingSupported} returns {@code true}.
     *
     * @throws NullPointerException if any args were {@code null}
     * @throws UnsupportedOperationException if the type mapping was not supported
     */
    CARAPI constructor(
        /* [in] */ IMarshalQueryable* query,
        /* [in] */ ITypeReference* typeReference,
        /* [in] */ Int32 nativeType);

protected:
    AutoPtr<ITypeReference> mTypeReference;
    Int32 mNativeType;
};

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_MARSHALRE_H__