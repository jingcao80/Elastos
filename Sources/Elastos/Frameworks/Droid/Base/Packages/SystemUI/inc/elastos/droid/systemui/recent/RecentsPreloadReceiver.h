
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_TASKDESCRIPTION_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_TASKDESCRIPTION_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class RecentsPreloadReceiver
    : public BroadcastReceiver
{
public:
    RecentsPreloadReceiver();

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_TASKDESCRIPTION_H__