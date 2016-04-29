
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_CUSAGETRACKERRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_CUSAGETRACKERRECEIVER_H__

#include "_Elastos_Droid_SystemUI_Qs_CUsageTrackerReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::SystemUI::StatusBar::Policy::IListenable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

class UsageTracker;

CarClass(CUsageTrackerReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IListenable* host);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    UsageTracker* mHost;
};

}// namespace Qs
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_QS_CUSAGETRACKERRECEIVER_H__
