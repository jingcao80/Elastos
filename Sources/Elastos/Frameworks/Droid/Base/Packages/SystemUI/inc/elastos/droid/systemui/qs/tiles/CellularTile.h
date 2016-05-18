
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CELLULARTILE_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CELLULARTILE_H__

#include "elastos/droid/systemui/qs/QSTile.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkController;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkSignalChangedCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/** Quick settings tile: Cellular **/
class CellularTile: public QSTile
{
private:
    class CallbackInfo: public Object
    {
    public:
        CallbackInfo();

    public:
        Boolean mEnabled;
        Boolean mWifiEnabled;
        Boolean mWifiConnected;
        Boolean mAirplaneModeEnabled;
        Int32 mMobileSignalIconId;
        String mSignalContentDescription;
        Int32 mDataTypeIconId;
        String mDataContentDescription;
        Boolean mActivityIn;
        Boolean mActivityOut;
        String mEnabledDesc;
        Boolean mNoSim;
        Boolean mIsDataTypeIconWide;
    };

    class NetworkSignalChangedCallback
        : public Object
        , public INetworkSignalChangedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkSignalChangedCallback(
            /* [in] */ CellularTile* host);

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

        CARAPI OnMobileDataEnabled(
            /* [in] */ Boolean enabled);

    private:
        CellularTile* mHost;
        Boolean mWifiEnabled;
        Boolean mWifiConnected;
        Boolean mAirplaneModeEnabled;
    };

    class CellularDetailAdapter
        : public Object
        , public IQSTileDetailAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        CellularDetailAdapter(
            /* [in] */ CellularTile* host);

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
            /* [out] */ IView** result);

        CARAPI SetMobileDataEnabled(
            /* [in] */ Boolean enabled);

    private:
        CellularTile* mHost;
    };
public:
    CellularTile(
        /* [in] */ IQSTileHost* host);

    // @Override
    CARAPI GetDetailAdapter(
        /* [out] */ IQSTileDetailAdapter** result);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI CreateTileView(
        /* [in] */ IContext* context,
        /* [out] */ IQSTileView** result);

    // Remove the period from the network name
    static CARAPI_(String) RemoveTrailingPeriod(
        /* [in] */ const String& string);

    CARAPI_(String) GetSimpleName();

protected:
    // @Override
    CARAPI_(void) HandleClick();

    // @Override
    CARAPI_(void) HandleUpdateState(
        /* [in] */ State* state,
        /* [in] */ IInterface* arg);

    // @Override
    CARAPI_(AutoPtr<QSTile::State>) NewTileState();

private:
    static CARAPI_(AutoPtr<IIntent>) InitStatic();

private:
    static AutoPtr<IIntent> CELLULAR_SETTINGS;

    AutoPtr<INetworkController> mController;
    AutoPtr<CellularDetailAdapter> mDetailAdapter;
    AutoPtr<INetworkSignalChangedCallback> mCallback;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CELLULARTILE_H__
