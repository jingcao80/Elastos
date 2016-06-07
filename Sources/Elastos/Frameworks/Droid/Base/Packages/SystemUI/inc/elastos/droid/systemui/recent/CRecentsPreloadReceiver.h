
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPRELOADRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPRELOADRECEIVER_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentsPreloadReceiver.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentsPreloadReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPRELOADRECEIVER_H__