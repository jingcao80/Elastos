
#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWDATABASEHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWDATABASEHELPER_H__

#include "_Elastos_Droid_Webkit_CWebViewDatabaseHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewDatabaseHelper)
    , public Singleton
    , public IWebViewDatabaseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IWebViewDatabase** instance);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBVIEWDATABASEHELPER_H__
