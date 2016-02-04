
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

    virtual ~Marshaler() {}

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
    CARAPI CalculateMarshalSize(
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
    Marshaler();

    CARAPI constructor();

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