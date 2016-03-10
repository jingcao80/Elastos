
#ifndef __ELASTOS_DROID_APP_CPROFILEGROUPHELEPR_H__
#define __ELASTOS_DROID_APP_CPROFILEGROUPHELEPR_H__

#include "_Elastos_Droid_App_CProfileGroupHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileGroupHelper)
    , public Singleton
    , public IProfileGroupHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IProfileGroup** group);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CPROFILEGROUPHELEPR_H__
