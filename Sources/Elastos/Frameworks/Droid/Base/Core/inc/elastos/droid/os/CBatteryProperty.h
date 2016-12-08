
#ifndef __ELASTOS_DROID_OS_CBATTERYPROPERTY_H__
#define __ELASTOS_DROID_OS_CBATTERYPROPERTY_H__

#include "_Elastos_Droid_Os_CBatteryProperty.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBatteryProperty)
    , public Object
    , public IBatteryProperty
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatteryProperty();

    CARAPI constructor();

    CARAPI GetInt64(
        /* [out] */ Int64* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int64 mValueLong;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBATTERYPROPERTY_H__
