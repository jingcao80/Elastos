
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_WIFITILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_WIFITILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkController;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerAccessPoint;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerAccessPointCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkSignalChangedCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Wifi **/
class WifiTile: public QSTile
{
private:
    class CallbackInfo: public Object
    {
    public:
        CallbackInfo();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Boolean mEnabled;
        Boolean mConnected;
        Int32 mWifiSignalIconId;
        String mEnabledDesc;
        Boolean mActivityIn;
        Boolean mActivityOut;
        String mWifiSignalContentDescription;
    };

    class WifiDetailAdapter
        : public Object
        , public IQSTileDetailAdapter
        , public INetworkControllerAccessPointCallback
        , public IQSDetailItemsCallback
    {
    public:
        CAR_INTERFACE_DECL()

        WifiDetailAdapter(
            /* [in] */ WifiTile* host);

        // @Override
        CARAPI GetTitle(
            /* [out] */ Int32* result);

        CARAPI GetSettingsIntent(
            /* [out] */ IIntent** result);

        // @Override
        CARAPI GetToggleState(
            /* [out] */ IBoolean** result);

        // @Override
        CARAPI SetToggleState(
            /* [in] */ IBoolean* state);

        // @Override
        CARAPI CreateDetailView(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI OnAccessPointsChanged(
            /* [in] */ ArrayOf<INetworkControllerAccessPoint*>* accessPoints);

        // @Override
        CARAPI OnDetailItemClick(
            /* [in] */ IQSDetailItemsItem* item);

        // @Override
        CARAPI OnDetailItemDisconnect(
            /* [in] */ IQSDetailItemsItem* item);

        CARAPI SetItemsVisible(
            /* [in] */ Boolean visible);

    private:
        CARAPI_(void) UpdateItems();

    private:
        AutoPtr<IQSDetailItems> mItems;
        AutoPtr<ArrayOf<INetworkControllerAccessPoint*> > mAccessPoints;
        WifiTile* mHost;
    };

    class Callback
        : public Object
        , public INetworkSignalChangedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        Callback(
            /* [in] */ WifiTile* host);

        // @Override
        CARAPI OnWifiSignalChanged(
            /* [in] */ Boolean enabled,
            /* [in] */ Boolean connected,
            /* [in] */ Int32 wifiSignalIconId,
            /* [in] */ Boolean activityIn,
            /* [in] */ Boolean activityOut,
            /* [in] */ const String& wifiSignalContentDescriptionId,
            /* [in] */ const String& description);

        // @Override
        CARAPI OnMobileDataSignalChanged(
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 mobileSignalIconId,
            /* [in] */ const String& mobileSignalContentDescriptionId,
            /* [in] */ Int32 dataTypeIconId,
            /* [in] */ Boolean activityIn,
            /* [in] */ Boolean activityOut,
            /* [in] */ const String& dataTypeContentDescriptionId,
            /* [in] */ const String& description,
            /* [in] */ Boolean noSim,
            /* [in] */ Boolean isDataTypeIconWide);

        // @Override
        CARAPI OnAirplaneModeChanged(
            /* [in] */ Boolean enabled);

        // @Override
        CARAPI OnMobileDataEnabled(
            /* [in] */ Boolean enabled);

    private:
        WifiTile* mHost;
    };

public:
    WifiTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI SupportsDualTargets(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI GetDetailAdapter(
        /* [out] */ IQSTileDetailAdapter** result);

    // @Override
    CARAPI CreateTileView(
        /* [in] */ IContext* context,
        /* [out] */ IQSTileView** view);

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
    CARAPI_(Boolean) ShouldAnnouncementBeDelayed();

    // @Override
    CARAPI_(String) ComposeChangeAnnouncement();

private:
    static CARAPI_(String) RemoveDoubleQuotes(
        /* [in] */ const String& string);

    static CARAPI_(AutoPtr<IIntent>) InitStatic();

private:
    static AutoPtr<IIntent> WIFI_SETTINGS;

    AutoPtr<INetworkController> mController;
    AutoPtr<WifiDetailAdapter> mDetailAdapter;
    AutoPtr<QSTile::State> mStateBeforeClick;
    AutoPtr<INetworkSignalChangedCallback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_WIFITILE_H__
