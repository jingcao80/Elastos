
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLERGGBCHANNELVECTOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLERGGBCHANNELVECTOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

class MarshalQueryableRggbChannelVector
    : public Object
    , public IMarshalQueryableRggbChannelVector
    , public IMarshalQueryable
{
private:
    class MarshalerRggbChannelVector
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalerRggbChannelVector")

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

    protected:
        friend class MarshalQueryableRggbChannelVector;

        MarshalerRggbChannelVector(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableRggbChannelVector* host);
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
    static const Int32 SIZE;
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLERGGBCHANNELVECTOR_H__