
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__

#include "_Elastos_Droid_Internal_Utility_CParcelableString.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CParcelableString)
    , public Object
    , public IParcelableString
    , public ICharSequence
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& str);

    CARAPI constructor(
        /* [in] */ ICharSequence* cs);

    CARAPI SetString(
        /* [in] */ const String& str);

    CARAPI GetString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetLength(
        /* [out] */ Int32* number);

    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* c);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

public:
    String mString;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CPARCELABLESTRING_H__
