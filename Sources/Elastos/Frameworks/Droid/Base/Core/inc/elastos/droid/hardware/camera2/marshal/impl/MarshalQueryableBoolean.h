
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEBOOLEAN_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEBOOLEAN_H__

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

class MarshalQueryableBoolean
    : public Object
    , public IMarshalQueryableBoolean
    , public IMarshalQueryable
{
private:
    class MarshalerBoolean
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalQueryableBoolean::MarshalerBoolean")

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
        friend class MarshalQueryableBoolean;

        MarshalerBoolean(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableBoolean* host);
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

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEBOOLEAN_H__