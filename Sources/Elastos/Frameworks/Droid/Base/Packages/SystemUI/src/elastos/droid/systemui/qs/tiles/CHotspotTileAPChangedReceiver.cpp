
#include "elastos/droid/systemui/qs/tiles/CHotspotTileAPChangedReceiver.h"
#include "elastos/droid/systemui/qs/tiles/HotspotTile.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_OBJECT_IMPL(CHotspotTileAPChangedReceiver)
ECode CHotspotTileAPChangedReceiver::constructor(
    /* [in] */ IQSTile* host)
{
    mHost = (HotspotTile*)host;
    return NOERROR;
}

ECode CHotspotTileAPChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mUsageTracker == NULL) {
        mUsageTracker = new UsageTracker(context, mHost/*HotspotTile.class*/);
    }
    mUsageTracker->TrackUsage();
    return NOERROR;
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
