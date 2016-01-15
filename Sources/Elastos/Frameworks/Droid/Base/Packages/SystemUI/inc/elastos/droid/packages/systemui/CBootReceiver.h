
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBOOTRECEIVER_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBOOTRECEIVER_H__

#include "_Elastos_Droid_Packages_SystemUI_CBootReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

/**
 * Performs a number of miscellaneous, non-system-critical actions
 * after the system has finished booting.
 */
CarClass(CBootReceiver)
    , public BroadcastReceiver
{
public:
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static String TAG;
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_CBOOTRECEIVER_H__
