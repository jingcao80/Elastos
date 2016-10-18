
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPARCELABLE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPARCELABLE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

class MarshalQueryableParcelable
    : public Object
    , public IMarshalQueryableParcelable
    , public IMarshalQueryable
{
private:
    class MarshalerParcelable
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalerParcelable")

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
        friend class MarshalQueryableParcelable;

        MarshalerParcelable(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableParcelable* host);

    private:
        ClassID mClass;
        //private final Parcelable.Creator<T> mCreator;
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

    static const String FIELD_CREATOR;
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPARCELABLE_H__