
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPAIR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPAIR_H__

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

class MarshalQueryablePair
    : public Object
    , public IMarshalQueryablePair
    , public IMarshalQueryable
{
private:
    class MarshalerPair
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalerPair")

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
        friend class MarshalQueryablePair;

        MarshalerPair(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryablePair* host);

    private:
        // final Class<? super Pair<T1, T2>> mClass;
        // final Constructor<Pair<T1, T2>> mConstructor;
        /** Marshal the {@code T1} inside of {@code Pair<T1, T2>} */
        AutoPtr<IMarshaler> mNestedTypeMarshalerFirst;
        /** Marshal the {@code T1} inside of {@code Pair<T1, T2>} */
        AutoPtr<IMarshaler> mNestedTypeMarshalerSecond;
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

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEPAIR_H__