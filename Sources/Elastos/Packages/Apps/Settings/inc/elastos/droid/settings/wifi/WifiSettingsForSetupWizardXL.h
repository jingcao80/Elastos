#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/wifi/CWifiSettings.h"
#include "elastos/droid/app/Activity.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerActionListener;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * WifiSetings Activity specific for SetupWizard with X-Large screen size.
 */
class WifiSettingsForSetupWizardXL
    : public Activity
    , public IWifiSettingsForSetupWizardXL
    , public IViewOnClickListener
{
private:
    class WifiManagerActionListener
        : public Object
        , public IWifiManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        WifiManagerActionListener();

        ~WifiManagerActionListener();

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    };

public:
    CAR_INTERFACE_DECL();

    WifiSettingsForSetupWizardXL();

    ~WifiSettingsForSetupWizardXL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* view);

    // Called from WifiSettings
    /* package */ CARAPI_(void) UpdateConnectionState(
        /* [in] */ NetworkInfoDetailedState originalState);

    /**
     * Called when the screen enters wifi configuration UI. UI widget for configuring network
     * (a.k.a. ConfigPreference) should be taken care of by caller side.
     * This method should handle buttons' visibility/enabled.
     * @param selectedAccessPoint AccessPoint object being selected. NULL when a user pressed
     * "Add network" button, meaning there's no selected access point.
     */
    /* package */ CARAPI_(void) ShowConfigUi(
        /* [in] */ IAccessPoint* selectedAccessPoint,
        /* [in] */ Boolean edit);

    /**
     * Called before security fields are correctly set by {@link WifiConfigController}.
     *
     * @param view security field view
     * @param accessPointSecurity type of security. e.g. AccessPoint::SECURITY_NONE
     * @return TRUE when it is ok for the caller to init security fields. FALSE when
     * all security fields are managed by this method, and thus the caller shouldn't touch them.
     */
    /* package */ CARAPI_(Boolean) InitSecurityFields(
        /* [in] */ IView* view,
        /* [in] */ Int32 accessPointSecurity);

    // May be called when user press "connect" button in WifiDialog
    /* package */ CARAPI_(void) OnConnectButtonPressed();

    /**
     * @param connected TRUE when the device is connected to a specific network.
     */
    /* package */ CARAPI_(void) ChangeNextButtonState(
        /* [in] */Boolean connected);

    /**
     * Called when the list of AccessPoints are modified and this Activity needs to refresh
     * the list.
     * @param preferenceScreen
     */
    /* package */ CARAPI_(void) OnAccessPointsUpdated(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ ICollection* accessPoints); //Collection<AccessPoint>

    /**
     * Called when {@link WifiSettings} received
     * {@link WifiManager#SUPPLICANT_STATE_CHANGED_ACTION}.
     */
    /* package */ CARAPI_(void) OnSupplicantStateChanged(
        /* [in] */ IIntent* intent);

    // Used by WifiConfigUiForSetupWizardXL
    /* package */
    CARAPI SetPaddingVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Called when WifiManager is requested to save a network.
     */
    /* package */ CARAPI_(void) OnSaveNetwork(
    /* [in] */ IWifiConfiguration* config);

private:
    CARAPI_(void) InitViews();

    CARAPI_(void) RestoreFirstVisibilityState();

    CARAPI_(void) HideSoftwareKeyboard();

    CARAPI_(void) ShowDisconnectedState(
        /* [in] */ const String& stateString);

    CARAPI_(void) ShowConnectingState();

    CARAPI_(void) ShowConnectedState();

    CARAPI_(void) ShowDefaultTitle();

    CARAPI_(void) ShowAddNetworkTitle();

    CARAPI_(void) ShowEditingTitle();

    CARAPI_(void) ShowConnectingTitle();

    CARAPI_(void) ShowConnectedTitle();

    /**
     * Shows top divider with ProgressBar without defining the state of the ProgressBar.
     *
     * @see #ShowScanningProgressBar()
     * @see #ShowConnectedProgressBar()
     * @see #ShowConnectingProgressBar()
     */
    CARAPI_(void) ShowTopDividerWithProgressBar();

    CARAPI_(void) ShowScanningState();

    CARAPI_(void) OnAddNetworkButtonPressed();

    CARAPI_(void) OnEapNetworkSelected();

    CARAPI_(void) ShowEditingButtonState();

    CARAPI_(void) OnBackButtonPressed();

    CARAPI_(void) RefreshAccessPoints(
        /* [in] */ Boolean disconnectNetwork);

    /**
     * Called once when Authentication failed.
     */
    CARAPI_(void) OnAuthenticationFailure();

    CARAPI_(void) ShowDisconnectedProgressBar();

    /**
     * Shows top divider with ProgressBar, whose state is intermediate.
     */
    CARAPI_(void) ShowScanningProgressBar();

    /**
     * Shows top divider with ProgressBar, showing "connecting" state.
     */
    CARAPI_(void) ShowConnectingProgressBar();

    CARAPI_(void) ShowConnectedProgressBar();

private:
    static const String TAG;
    static const Boolean DEBUG;

    // // lock orientation into landscape or portrait
    static const String EXTRA_PREFS_LANDSCAPE_LOCK;
    static const String EXTRA_PREFS_PORTRAIT_LOCK;

    // private static const EnumMap<NetworkInfoDetailedState, NetworkInfoDetailedState> sNetworkStateMap =
    //         new EnumMap<NetworkInfoDetailedState, NetworkInfoDetailedState>(NetworkInfoDetailedState.class);
    static const AutoPtr<IMap> sNetworkStateMap;

    AutoPtr<CWifiSettings> mWifiSettings;
    AutoPtr<IWifiManager> mWifiManager;

    /** Used for resizing a padding above title. Hiden when software keyboard is shown. */
    AutoPtr<IView> mTopPadding;

    /** Used for resizing a padding of main content. Hiden when software keyboard is shown. */
    AutoPtr<IView> mContentPadding;

    AutoPtr<ITextView> mTitleView;
    /**
     * The name of a network currently connecting, or trying to connect.
     * This may be empty ("") at first, and updated when configuration is changed.
     */
    AutoPtr<ICharSequence> mNetworkName;
    AutoPtr<ICharSequence> mEditingTitle;

    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IView> mTopDividerNoProgress;
    /**
     * Used for resizing a padding between WifiSettings preference and bottom bar when
     * ProgressBar is visible as a top divider.
     */
    AutoPtr<IView> mBottomPadding;

    AutoPtr<IButton> mAddNetworkButton;
    AutoPtr<IButton> mRefreshButton;
    AutoPtr<IButton> mSkipOrNextButton;
    AutoPtr<IButton> mBackButton;

    AutoPtr<IButton> mConnectButton;

    /**
     * View enclosing {@link WifiSettings}.
     */
    AutoPtr<IView> mWifiSettingsFragmentLayout;
    AutoPtr<IView> mConnectingStatusLayout;
    AutoPtr<ITextView> mConnectingStatusView;

    /*
     * States of current screen, which should be saved and restored when Activity is relaunched
     * with orientation change, etc.
     */
    static const Int32 SCREEN_STATE_DISCONNECTED;
    static const Int32 SCREEN_STATE_EDITING;
    static const Int32 SCREEN_STATE_CONNECTING;
    static const Int32 SCREEN_STATE_CONNECTED;

    /** Current screen state. */
    Int32 mScreenState;

    AutoPtr<IWifiConfigUiForSetupWizardXL> mWifiConfig;

    AutoPtr<IInputMethodManager> mInputMethodManager;

    /**
     * Previous network connection state reported by main Wifi module.
     *
     * Note that we don't use original {@link DetailedState} object but simplified one translated
     * using sNetworkStateMap.
     */
    NetworkInfoDetailedState mPreviousNetworkState;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__
