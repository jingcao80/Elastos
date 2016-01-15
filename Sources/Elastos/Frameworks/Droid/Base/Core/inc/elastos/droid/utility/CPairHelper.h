
#ifndef __ELASTOS_DROID_UTILITY_CPAIRHELPER_H__
#define __ELASTOS_DROID_UTILITY_CPAIRHELPER_H__

#include "_Elastos_Droid_Utility_CPairHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CPairHelper)
    , public Singleton
    , public IPairHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ IInterface *A,
        /* [in] */ IInterface *B,
        /* [out] */ IPair **pair);
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CPAIRHELPER_H__
