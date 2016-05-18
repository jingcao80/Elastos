
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CHOTSPOTTILEAPCHANGEDRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CHOTSPOTTILEAPCHANGEDRECEIVER_H__

#include "_Elastos_Droid_SystemUI_Qs_Tiles_CHotspotTileAPChangedReceiver.h"
#include "elastos/droid/systemui/qs/UsageTracker.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

class HotspotTile;

/**
 * This will catch broadcasts for changes in hotspot state so we can show
 * the hotspot tile for a number of days after use.
 */
CarClass(CHotspotTileAPChangedReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IQSTile* host);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    HotspotTile* mHost;
    AutoPtr<UsageTracker> mUsageTracker;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CHOTSPOTTILEAPCHANGEDRECEIVER_H__
