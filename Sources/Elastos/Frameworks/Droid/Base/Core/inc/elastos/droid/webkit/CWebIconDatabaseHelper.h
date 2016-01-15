
#ifndef __ELASTOS_DROID_WEBKIT_CWEBICONDATABASEHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBICONDATABASEHELPER_H__

#include "_Elastos_Droid_Webkit_CWebIconDatabaseHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebIconDatabaseHelper)
    , public Singleton
    , public IWebIconDatabaseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IWebIconDatabase** database);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBICONDATABASEHELPER_H__
