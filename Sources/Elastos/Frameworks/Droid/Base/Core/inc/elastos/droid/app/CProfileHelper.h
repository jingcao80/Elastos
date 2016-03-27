#ifndef __ELASTOS_DROID_APP_CPROFILE_HELPER_H__
#define __ELASTOS_DROID_APP_CPROFILE_HELPER_H__

#include "_Elastos_Droid_App_CProfileHelper.h"
#include "elastos/droid/app/Profile.h"
#include <elastos/core/Singleton.h>


namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileHelper)
    , public Singleton
    , public IProfileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IProfile** result);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROFILE_HELPER_H__
