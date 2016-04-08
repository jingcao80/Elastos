
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitor;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardMonitorCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::ILocationController;
using Elastos::Droid::SystemUI::StatusBar::Policy::ILocationSettingsChangeCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Location **/
class LocationTile: public QSTile
{
private:
    class Callback
        : public Object
        , public ILocationSettingsChangeCallback
        , public IKeyguardMonitorCallback
    {
    public:
        CAR_INTERFACE_DECL();

        Callback(
            /* [in] */ LocationTile* host);

        // @Override
        CARAPI OnLocationSettingsChanged(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI OnKeyguardChanged();

    private:
        LocationTile* mHost;
    };

public:
    LocationTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(AutoPtr<State>) NewTileState();

    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    AutoPtr<ILocationController> mController;
    AutoPtr<IKeyguardMonitor> mKeyguard;
    AutoPtr<Callback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_LOCATIONTILE_H__
