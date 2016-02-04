
#ifndef  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEENUM_H__
#define  __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEENUM_H__

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

class MarshalQueryableEnum
    : public Object
    , public IMarshalQueryableEnum
    , public IMarshalQueryable
{
private:
    class MarshalerEnum
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
        CARAPI GetNativeSize(
            /* [out] */ Int32* value);

    protected:
        friend class MarshalQueryableEnum;

        MarshalerEnum(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableEnum* host);

    private:
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

    /**
     * Register a non-sequential set of values to be used with the marshal/unmarshal functions.
     *
     * <p>This enables get/set to correctly marshal the enum into a value that is C-compatible.</p>
     *
     * @param enumType The class for an enum
     * @param values A list of values mapping to the ordinals of the enum
     */
    static CARAPI RegisterEnumValues(
        /* [in] */ ClassID enumType,
        /* [in] */ ArrayOf<Int32>* values);

private:
    /**
     * Get the numeric value from an enum.
     *
     * <p>This is usually the same as the ordinal value for
     * enums that have fully sequential values, although for C-style enums the range of values
     * may not map 1:1.</p>
     *
     * @param enumValue Enum instance
     * @return Int guaranteed to be ABI-compatible with the C enum equivalent
     */
    static CARAPI_(Int32) GetEnumValue(
        /* [in] */ IInterface* enumValue);

    /**
     * Finds the enum corresponding to it's numeric value. Opposite of {@link #getEnumValue} method.
     *
     * @param enumType Class of the enum we want to find
     * @param value The numeric value of the enum
     * @return An instance of the enum
     */
    static CARAPI GetEnumFromValue(
        /* [in] */ ClassID enumType,
        /* [in] */ Int32 value,
        /* [out] */ IInterface** result);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    static const Int32 MARSHALQUERYABLEENUM_UINT8_MIN;
    static const Int32 MARSHALQUERYABLEENUM_UINT8_MAX;
    static const Int32 MARSHALQUERYABLEENUM_UINT8_MASK;

    //@SuppressWarnings("rawtypes")
    static AutoPtr<IHashMap> sEnumValues;
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLEENUM_H__