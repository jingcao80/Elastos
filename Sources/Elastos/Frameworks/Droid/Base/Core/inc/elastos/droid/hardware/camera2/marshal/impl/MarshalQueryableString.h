
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLESTRING_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLESTRING_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/hardware/camera2/marshal/Marshaler.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::IO::Charset::ICharset;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

class MarshalQueryableString
    : public Object
    , public IMarshalQueryableString
    , public IMarshalQueryable
{
private:
    class MarshalerString
        : public Marshaler
    {
    public:
        TO_STRING_IMPL("MarshalerString")

        //@Override
        CARAPI Marshal(
            /* [in] */ IInterface* value,
            /* [in] */ IByteBuffer* buffer);

        //@Override
        CARAPI CalculateMarshalSize(
            /* [in] */ IInterface* value,
            /* [out] */ Int32* outvalue);

        //@Override
        CARAPI Unmarshal(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ IInterface** outface);

        //@Override
        CARAPI GetNativeSize(
            /* [out] */ Int32* value);

    protected:
        friend class MarshalQueryableString;

        MarshalerString(
            /* [in] */ ITypeReference* typeReference,
            /* [in] */ Int32 nativeType,
            /* [in] */ MarshalQueryableString* host);
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

    static const AutoPtr<ICharset> UTF8_CHARSET;
    static const Byte NUL; // used as string terminator
};

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_MARSHAL_IMPL_MARSHALQUERYABLESTRING_H__