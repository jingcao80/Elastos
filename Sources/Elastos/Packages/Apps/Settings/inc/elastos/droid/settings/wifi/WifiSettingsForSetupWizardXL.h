#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Content::Pm::IActivityInfo;
// using Elastos::Droid::Net::NetworkInfo::IDetailedState;
// using Elastos::Droid::Net::Wifi::IWifiConfiguration;
// using Elastos::Droid::Net::Wifi::IWifiManager;
// using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::Os::IMessage;
// using Elastos::Droid::Preference::IPreferenceScreen;
// using Elastos::Droid::View::IViewGroup;
// using Elastos::Droid::View::IWindow;
// using Elastos::Droid::Widget::IProgressBar;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Utility::ICollection;
// using Elastos::Utility::IEnumMap;
// using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * WifiSetings Activity specific for SetupWizard with X-Large screen size.
 */
class WifiSettingsForSetupWizardXL
    : public Activity
    , public IViewOnClickListener
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    // CARAPI OnCreate(Bundle savedInstanceState);

    //@Override
    // CARAPI OnClick(View view);

    // Called from WifiSettings
    // /* package */ void UpdateConnectionState(DetailedState originalState);

    /**
     * Called when the screen enters wifi configuration UI. UI widget for configuring network
     * (a.k.a. ConfigPreference) should be taken care of by caller side.
     * This method should handle buttons' visibility/enabled.
     * @param selectedAccessPoint AccessPoint object being selected. NULL when a user pressed
     * "Add network" button, meaning there's no selected access point.
     */
    // /* package */ void ShowConfigUi(AccessPoint selectedAccessPoint, Boolean edit);

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
    // /* package */ void OnConnectButtonPressed();

    /**
     * @param connected TRUE when the device is connected to a specific network.
     */
    // /* package */ void ChangeNextButtonState(Boolean connected);

    /**
     * Called when the list of AccessPoints are modified and this Activity needs to refresh
     * the list.
     * @param preferenceScreen
     */
    // /* package */ void OnAccessPointsUpdated(
    //         PreferenceScreen preferenceScreen, Collection<AccessPoint> accessPoints);

    /**
     * Called when {@link WifiSettings} received
     * {@link WifiManager#SUPPLICANT_STATE_CHANGED_ACTION}.
     */
    // /* package */ void OnSupplicantStateChanged(Intent intent);

    // Used by WifiConfigUiForSetupWizardXL
    /* package */
    CARAPI_(void) SetPaddingVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Called when WifiManager is requested to save a network.
     */
    // /* package */ void OnSaveNetwork(WifiConfiguration config);

private:
    // void InitViews();

    // void RestoreFirstVisibilityState();

    CARAPI_(void) HideSoftwareKeyboard();

    // void ShowDisconnectedState(String stateString);

    // void ShowConnectingState();

    // void ShowConnectedState();

    // void ShowDefaultTitle();

    // void ShowAddNetworkTitle();

    // void ShowEditingTitle();

    // void ShowConnectingTitle();

    // void ShowConnectedTitle();

    /**
     * Shows top divider with ProgressBar without defining the state of the ProgressBar.
     *
     * @see #ShowScanningProgressBar()
     * @see #ShowConnectedProgressBar()
     * @see #ShowConnectingProgressBar()
     */
    // void ShowTopDividerWithProgressBar();

    // void ShowScanningState();

    // void OnAddNetworkButtonPressed();

    CARAPI_(void) OnEapNetworkSelected();

    // void ShowEditingButtonState();

    // void OnBackButtonPressed();

    // void RefreshAccessPoints(Boolean disconnectNetwork);

    /**
     * Called once when Authentication failed.
     */
    // void OnAuthenticationFailure();

    // void ShowDisconnectedProgressBar();

    /**
     * Shows top divider with ProgressBar, whose state is intermediate.
     */
    // void ShowScanningProgressBar();

    /**
     * Shows top divider with ProgressBar, showing "connecting" state.
     */
    // void ShowConnectingProgressBar();

    // void ShowConnectedProgressBar();

private:
    static const String TAG;
    static const Boolean DEBUG;

    // // lock orientation into landscape or portrait
    static const String EXTRA_PREFS_LANDSCAPE_LOCK;
    static const String EXTRA_PREFS_PORTRAIT_LOCK;

    // private static const EnumMap<DetailedState, DetailedState> sNetworkStateMap =
    //         new EnumMap<DetailedState, DetailedState>(DetailedState.class);

    // static {
    //     sNetworkStateMap->Put(DetailedState.IDLE, DetailedState.DISCONNECTED);
    //     sNetworkStateMap->Put(DetailedState.SCANNING, DetailedState.SCANNING);
    //     sNetworkStateMap->Put(DetailedState.CONNECTING, DetailedState.CONNECTING);
    //     sNetworkStateMap->Put(DetailedState.AUTHENTICATING, DetailedState.CONNECTING);
    //     sNetworkStateMap->Put(DetailedState.OBTAINING_IPADDR, DetailedState.CONNECTING);
    //     sNetworkStateMap->Put(DetailedState.CONNECTED, DetailedState.CONNECTED);
    //     sNetworkStateMap->Put(DetailedState.SUSPENDED, DetailedState.SUSPENDED);  // ?
    //     sNetworkStateMap->Put(DetailedState.DISCONNECTING, DetailedState.DISCONNECTED);
    //     sNetworkStateMap->Put(DetailedState.DISCONNECTED, DetailedState.DISCONNECTED);
    //     sNetworkStateMap->Put(DetailedState.FAILED, DetailedState.FAILED);
    // }

    // private WifiSettings mWifiSettings;
    // private WifiManager mWifiManager;

    /** Used for resizing a padding above title. Hiden when software keyboard is shown. */
    AutoPtr<IView> mTopPadding;

    /** Used for resizing a padding of main content. Hiden when software keyboard is shown. */
    AutoPtr<IView> mContentPadding;

    // private TextView mTitleView;
    /**
     * The name of a network currently connecting, or trying to connect.
     * This may be empty ("") at first, and updated when configuration is changed.
     */
    // private CharSequence mNetworkName = "";
    // private CharSequence mEditingTitle;

    // private ProgressBar mProgressBar;
    // private View mTopDividerNoProgress;
    /**
     * Used for resizing a padding between WifiSettings preference and bottom bar when
     * ProgressBar is visible as a top divider.
     */
    // private View mBottomPadding;

    // private Button mAddNetworkButton;
    // private Button mRefreshButton;
    // private Button mSkipOrNextButton;
    AutoPtr<IButton> mBackButton;

    AutoPtr<IButton> mConnectButton;

    /**
     * View enclosing {@link WifiSettings}.
     */
    // private View mWifiSettingsFragmentLayout;
    // private View mConnectingStatusLayout;
    // private TextView mConnectingStatusView;

    /*
     * States of current screen, which should be saved and restored when Activity is relaunched
     * with orientation change, etc.
     */
    // private static const Int32 SCREEN_STATE_DISCONNECTED = 0;
    // private static const Int32 SCREEN_STATE_EDITING = 1;
    // private static const Int32 SCREEN_STATE_CONNECTING = 2;
    // private static const Int32 SCREEN_STATE_CONNECTED = 3;

    /** Current screen state. */
    // private Int32 mScreenState = SCREEN_STATE_DISCONNECTED;

    AutoPtr<IWifiConfigUiForSetupWizardXL> mWifiConfig;

    AutoPtr<IInputMethodManager> mInputMethodManager;

    /**
     * Previous network connection state reported by main Wifi module.
     *
     * Note that we don't use original {@link DetailedState} object but simplified one translated
     * using sNetworkStateMap.
     */
    // private DetailedState mPreviousNetworkState = DetailedState.DISCONNECTED;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGSFORSETUPWIZARDXL_H__
