#ifndef __ELASTOS_DROID_DATABASE_CCURSORWINDOWHELPER_H__
#define __ELASTOS_DROID_DATABASE_CCURSORWINDOWHELPER_H__

#include "_Elastos_Droid_Database_CCursorWindowHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CCursorWindowHelper)
    , public Singleton
    , public ICursorWindowHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewFromParcel(
        /* [in] */ IParcel* p,
        /* [out] */ ICursorWindow** result);
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CCURSORWINDOWHELPER_H__