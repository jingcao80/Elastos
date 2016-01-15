
#ifndef __ELASTOS_DROID_WEBKIT_CWEBSTORAGEHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBSTORAGEHELPER_H__

#include "_Elastos_Droid_Webkit_CWebStorageHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebStorageHelper)
    , public Singleton
    , public IWebStorageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IWebStorage** instance);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBSTORAGEHELPER_H__
