#ifndef __ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class Summary
{
public:
    static CARAPI_(String) Get(
        /* [in] */ IContext* context,
        /* [in] */ const String& ssid,
        /* [in] */ NetworkInfoDetailedState state);

    static CARAPI_(String) Get(
        /* [in] */ IContext* context,
        /* [in] */ NetworkInfoDetailedState state);
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__
