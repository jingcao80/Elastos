
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLISTHELPER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLISTHELPER_H__

#include "_Elastos_Utility_Concurrent_CCopyOnWriteArrayListHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CCopyOnWriteArrayListHelper)
    , public Singleton
    , public ICopyOnWriteArrayListHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ContainsAll(
        /* [in] */ ICollection* collection,
        /* [in] */ ArrayOf<IInterface*>* snapshot,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to,
        /* [out] */ Boolean* value);

    CARAPI LastIndexOf(
        /* [in] */ IInterface* o,
        /* [in] */ ArrayOf<IInterface*>* data,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to,
        /* [out] */ Int32* value);

    CARAPI IndexOf(
        /* [in] */ IInterface* o,
        /* [in] */ ArrayOf<IInterface*>* data,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to,
        /* [out] */ Int32* value);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLISTHELPER_H__
