
#ifndef __ELASTOS_DROID_APP_CBRIGHTNESSSETTINGSHELPER_H__
#define __ELASTOS_DROID_APP_CBRIGHTNESSSETTINGSHELPER_H__

#include "_Elastos_Droid_App_CBrightnessSettingsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CBrightnessSettingsHelper)
    , public Singleton
    , public IBrightnessSettingsHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IBrightnessSettings** obj);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CBRIGHTNESSSETTINGSHELPER_H__
