
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_BLUETOOTHTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_BLUETOOTHTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBluetoothController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBluetoothControllerCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Bluetooth **//*QSTile.BooleanState*/
class BluetoothTile: public QSTile
{
private:
    class Callback
        : public Object
        , public IBluetoothControllerCallback
    {
    public:
        CAR_INTERFACE_DECL();

        Callback(
            /* [in] */ BluetoothTile* host);

        // @Override
        CARAPI OnBluetoothStateChange(
            /* [in] */ Boolean enabled,
            /* [in] */ Boolean connecting);

        // @Override
        CARAPI OnBluetoothPairedDevicesChanged();

    private:
        BluetoothTile* mHost;
    };

    class BluetoothDetailAdapter
        : public Object
        , public IQSTileDetailAdapter
        , public IQSDetailItemsCallback
    {
    public:
        CAR_INTERFACE_DECL();

        BluetoothDetailAdapter(
            /* [in] */ BluetoothTile* host);

        // @Override
        CARAPI GetTitle(
            /* [out] */ Int32* result);

        // @Override
        CARAPI GetToggleState(
            /* [out] */ IBoolean** result);

        // @Override
        CARAPI GetSettingsIntent(
            /* [out] */ IIntent** result);

        // @Override
        CARAPI SetToggleState(
            /* [in] */ IBoolean* state);

        // @Override
        CARAPI CreateDetailView(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI SetItemsVisible(
            /* [in] */ Boolean visible);

        // @Override
        CARAPI OnDetailItemClick(
            /* [in] */ IQSDetailItemsItem* item);

        // @Override
        CARAPI OnDetailItemDisconnect(
            /* [in] */ IQSDetailItemsItem* item);

        CARAPI_(void) UpdateItems();

    private:
        AutoPtr<IQSDetailItems> mItems;
        BluetoothTile* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ BluetoothTile* host);

        // @Override
        CARAPI Run();

    private:
        BluetoothTile* mHost;
    };

public:
    BluetoothTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SupportsDualTargets(
        /* [in] */ Boolean* result);

    // @Override
    CARAPI GetDetailAdapter(
        /* [out] */ IQSTileDetailAdapter** result);

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
    CARAPI_(void) HandleSecondaryClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    static CARAPI_(AutoPtr<IIntent>) InitStatic();

private:
    static AutoPtr<IIntent> BLUETOOTH_SETTINGS;

    AutoPtr<IBluetoothController> mController;
    AutoPtr<BluetoothDetailAdapter> mDetailAdapter;
    AutoPtr<IBluetoothControllerCallback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_BLUETOOTHTILE_H__
