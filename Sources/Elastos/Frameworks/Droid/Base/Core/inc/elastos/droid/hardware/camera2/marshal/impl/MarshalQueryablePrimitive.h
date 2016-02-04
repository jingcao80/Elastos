
#ifndef  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPRIMITIVE_H__
#define  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPRIMITIVE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IRational;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

class MarshalQueryablePrimitive
    : public Object
    , public IMarshalQueryablePrimitive
    , public IMarshalQueryable
{
private:
    class MarshalerPrimitive
        : public Marshaler
    {
    public:
        //@Override
        CARAPI Marshal(
            /* [in] */ IInterface* value,
            /* [in] */ IByteBuffer* buffer);

        //@Override
        CARAPI Unmarshal(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ IInterface** outface);

        //@Override
        CARAPI CalculateMarshalSize(
            /* [in] */ IInterface* value,
            /* [out] */ Int32* outvalue);

        //@Override
        CARAPI GetNativeSize(
            /* [out] */ Int32* value);

    protected:
        friend class MarshalQueryablePrimitive;

        MarshalerPrimitive(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryablePrimitive* host);

    private:
        CARAPI MarshalPrimitive(
            /* [in] */ Int32 value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI MarshalPrimitive(
            /* [in] */ Float value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI MarshalPrimitive(
            /* [in] */ Double value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI MarshalPrimitive(
            /* [in] */ Int64 value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI MarshalPrimitive(
            /* [in] */ IRational* value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI MarshalPrimitive(
            /* [in] */ Byte value,
            /* [in] */ IByteBuffer* buffer);

        CARAPI UnmarshalObject(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ IInterface** obj);

    private:
        /** Always the wrapped class variant of the primitive class for {@code T} */
        ClassID mClass;
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
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPRIMITIVE_H__