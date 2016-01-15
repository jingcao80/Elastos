
#ifndef __ELASTOS_DROID_UTILITY_CPAIR_H__
#define __ELASTOS_DROID_UTILITY_CPAIR_H__

#include "_Elastos_Droid_Utility_CPair.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CPair)
    , public Object
    , public IPair
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetFirst(
        /* [out] */ IInterface **first);

    CARAPI GetSecond(
        /* [out] */ IInterface **second);

    CARAPI Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ IInterface *first,
        /* [in] */ IInterface *second);

    static CARAPI Create(
        /* [in] */ IInterface *A,
        /* [in] */ IInterface *B,
        /* [out] */ IPair **pair);

public:
    AutoPtr<IInterface> mFirst;
    AutoPtr<IInterface> mSecond;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CPAIR_H__
