
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/UsageTracker.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IHotspotController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IHotspotControllerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Hotspot **/
class HotspotTile: public QSTile
{
private:
    class Callback
        : public Object
        , public IHotspotControllerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ HotspotTile* host);

        // @Override
        CARAPI OnHotspotChanged(
            /* [in] */ Boolean enabled);

    private:
        HotspotTile* mHost;
    };

public:
    HotspotTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleDestroy();

    // @Override
    CARAPI_(AutoPtr<QSTile::State>) NewTileState();

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    AutoPtr<IHotspotController> mController;
    AutoPtr<Callback> mCallback;
    AutoPtr<UsageTracker> mUsageTracker;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_HOTSPOTTILE_H__
