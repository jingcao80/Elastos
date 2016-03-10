
#ifndef __ELASTOS_DROID_APP_CRINGMODESETTINGSHELPER_H__
#define __ELASTOS_DROID_APP_CRINGMODESETTINGSHELPER_H__

#include "_Elastos_Droid_App_CRingModeSettingsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CRingModeSettingsHelper)
    , public Singleton
    , public IRingModeSettingsHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IRingModeSettings** obj);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CRINGMODESETTINGSHELPER_H__
