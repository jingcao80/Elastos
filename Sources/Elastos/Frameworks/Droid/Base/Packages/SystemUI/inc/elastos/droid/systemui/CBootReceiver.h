
#ifndef  __ELASTOS_DROID_SYSTEMUI_CBOOTRECEIVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CBOOTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_CBootReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * Performs a number of miscellaneous, non-system-critical actions
 * after the system has finished booting.
 */
CarClass(CBootReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static String TAG;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CBOOTRECEIVER_H__
