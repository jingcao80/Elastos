#ifndef __ELASTOS_DROID_UTILITY_CARRAYMAPHELPER_H__
#define __ELASTOS_DROID_UTILITY_CARRAYMAPHELPER_H__

#include "_Elastos_Droid_Utility_CArrayMapHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CArrayMapHelper)
    , public Singleton
    , public IArrayMapHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEMPTY(
        /* [out] */ IArrayMap** map);
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CARRAYMAPHELPER_H__
