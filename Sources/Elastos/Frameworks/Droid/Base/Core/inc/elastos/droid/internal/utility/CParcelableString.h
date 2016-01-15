
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__

#include "_Elastos_Droid_Internal_Utility_CParcelableString.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CParcelableString)
    , public Object
    , public IParcelableString
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI SetString(
        /* [in] */ const String& str);

    CARAPI GetString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    String mString;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__
