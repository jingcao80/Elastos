
#include "Elastos.Droid.Os.h"
#include "elastos/droid/settings/wifi/WifiSettingsForSetupWizardXL.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"
#include "elastos/droid/settings/wifi/CAccessPoint.h"
#include "elastos/droid/settings/wifi/Summary.h"
#include "elastos/droid/settings/wifi/WifiConfigUiForSetupWizardXL.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::NetworkInfoDetailedState_IDLE;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_SCANNING;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_AUTHENTICATING;
using Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_SUSPENDED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::EIID_IWifiManagerActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String WifiSettingsForSetupWizardXL::TAG("SetupWizard");
const Boolean WifiSettingsForSetupWizardXL::DEBUG = TRUE;

const String WifiSettingsForSetupWizardXL::EXTRA_PREFS_LANDSCAPE_LOCK("extra_prefs_landscape_lock");
const String WifiSettingsForSetupWizardXL::EXTRA_PREFS_PORTRAIT_LOCK("extra_prefs_portrait_lock");

static const AutoPtr<IMap> InitsNetworkStateMap()
{
    AutoPtr<IMap> map;
    CHashMap::New((IMap**)&map);

    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_IDLE),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_DISCONNECTED));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_SCANNING),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_SCANNING));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTING),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTING));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_AUTHENTICATING),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTING));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_OBTAINING_IPADDR),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTING));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTED),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_CONNECTED));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_SUSPENDED),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_SUSPENDED));  // ?
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_DISCONNECTING),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_DISCONNECTED));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_DISCONNECTED),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_DISCONNECTED));
    map->Put(
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_FAILED),
            CoreUtils::Convert((Int32)NetworkInfoDetailedState_FAILED));

    return map;
}

const AutoPtr<IMap> WifiSettingsForSetupWizardXL::sNetworkStateMap = InitsNetworkStateMap();

const Int32 WifiSettingsForSetupWizardXL::SCREEN_STATE_DISCONNECTED = 0;
const Int32 WifiSettingsForSetupWizardXL::SCREEN_STATE_EDITING = 1;
const Int32 WifiSettingsForSetupWizardXL::SCREEN_STATE_CONNECTING = 2;
const Int32 WifiSettingsForSetupWizardXL::SCREEN_STATE_CONNECTED = 3;

//===============================================================================
//                  WifiSettingsForSetupWizardXL::WifiManagerActionListener
//===============================================================================

CAR_INTERFACE_IMPL(WifiSettingsForSetupWizardXL::WifiManagerActionListener, Object, IWifiManagerActionListener);

WifiSettingsForSetupWizardXL::WifiManagerActionListener::WifiManagerActionListener()
{}

WifiSettingsForSetupWizardXL::WifiManagerActionListener::~WifiManagerActionListener()
{}

ECode WifiSettingsForSetupWizardXL::WifiManagerActionListener::OnSuccess()
{
    return NOERROR;
}

ECode WifiSettingsForSetupWizardXL::WifiManagerActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    //TODO: Add failure UI
    return NOERROR;
}

//===============================================================================
//                  WifiSettingsForSetupWizardXL
//===============================================================================

CAR_INTERFACE_IMPL_2(WifiSettingsForSetupWizardXL, Activity, IWifiSettingsForSetupWizardXL, IViewOnClickListener);

WifiSettingsForSetupWizardXL::WifiSettingsForSetupWizardXL()
    : mScreenState(SCREEN_STATE_DISCONNECTED)
    , mPreviousNetworkState(NetworkInfoDetailedState_DISCONNECTED)
{
    mNetworkName = CoreUtils::Convert("");
}

WifiSettingsForSetupWizardXL::~WifiSettingsForSetupWizardXL()
{}

ECode WifiSettingsForSetupWizardXL::constructor()
{
    return NOERROR;
}

ECode WifiSettingsForSetupWizardXL::OnCreate(
    /* [in] */ IBundle*  savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Boolean res;
    RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &res);
    SetContentView(R::layout::wifi_settings_for_setup_wizard_xl);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);
    // There's no button here enabling wifi network, so we need to enable it without
    // users' request.
    mWifiManager->SetWifiEnabled(TRUE, &res);

    AutoPtr<IFragmentManager> fmanager;
    GetFragmentManager((IFragmentManager**)&fmanager);
    AutoPtr<IFragment> fragment;
    fmanager->FindFragmentById(R::id::wifi_setup_fragment, (IFragment**)&fragment);
    mWifiSettings = (CWifiSettings*)fragment.Get();
    obj = NULL;
    GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInputMethodManager = IInputMethodManager::Probe(obj);

    InitViews();

    // At first, Wifi module doesn't return SCANNING state (it's too early), so we manually
    // show it.
    ShowScanningState();
    return NOERROR;
}

void WifiSettingsForSetupWizardXL::InitViews()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    Boolean res;
    if (intent->GetBooleanExtra(String("firstRun"), FALSE, &res), res) {
        AutoPtr<IView> layoutRoot;
        FindViewById(R::id::layout_root, (IView**)&layoutRoot);
        layoutRoot->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_BACK);
    }
    if (intent->GetBooleanExtra(EXTRA_PREFS_LANDSCAPE_LOCK, FALSE, &res), res) {
        SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
    }
    if (intent->GetBooleanExtra(EXTRA_PREFS_PORTRAIT_LOCK, FALSE, &res), res) {
        SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT);
    }

    AutoPtr<IView> tmp;
    FindViewById(R::id::wifi_setup_title, (IView**)&tmp);
    mTitleView = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::scanning_progress_bar, (IView**)&tmp);
    mProgressBar = IProgressBar::Probe(tmp);
    mProgressBar->SetMax(2);
    FindViewById(R::id::top_divider_no_progress, (IView**)&mTopDividerNoProgress);
    FindViewById(R::id::bottom_padding, (IView**)&mBottomPadding);

    tmp->SetVisibility(IView::VISIBLE);
    mProgressBar->SetIndeterminate(TRUE);
    IView::Probe(mTopDividerNoProgress)->SetVisibility(IView::GONE);

    tmp = NULL;
    FindViewById(R::id::wifi_setup_add_network, (IView**)&tmp);
    mAddNetworkButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(this);
    tmp = NULL;
    FindViewById(R::id::wifi_setup_refresh_list, (IView**)&tmp);
    mRefreshButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(this);
    tmp = NULL;
    FindViewById(R::id::wifi_setup_skip_or_next, (IView**)&tmp);
    mSkipOrNextButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(this);
    tmp = NULL;
    FindViewById(R::id::wifi_setup_connect, (IView**)&tmp);
    mConnectButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(this);
    tmp = NULL;
    FindViewById(R::id::wifi_setup_cancel, (IView**)&tmp);
    mBackButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(this);

    FindViewById(R::id::top_padding, (IView**)&mTopPadding);
    FindViewById(R::id::content_padding, (IView**)&mContentPadding);

    FindViewById(R::id::wifi_settings_fragment_layout, (IView**)&mWifiSettingsFragmentLayout);
    FindViewById(R::id::connecting_status_layout, (IView**)&mConnectingStatusLayout);
    tmp = NULL;
    FindViewById(R::id::connecting_status, (IView**)&tmp);
    mConnectingStatusView = ITextView::Probe(tmp);
}

void WifiSettingsForSetupWizardXL::RestoreFirstVisibilityState()
{
    ShowDefaultTitle();
    IView::Probe(mAddNetworkButton)->SetVisibility(IView::VISIBLE);
    IView::Probe(mRefreshButton)->SetVisibility(IView::VISIBLE);
    IView::Probe(mSkipOrNextButton)->SetVisibility(IView::VISIBLE);
    IView::Probe(mConnectButton)->SetVisibility(IView::GONE);
    IView::Probe(mBackButton)->SetVisibility(IView::GONE);
    SetPaddingVisibility(IView::VISIBLE);
}

ECode WifiSettingsForSetupWizardXL::OnClick(
    /* [in] */ IView* view)
{
    HideSoftwareKeyboard();
    if (view == IView::Probe(mAddNetworkButton)) {
        if (DEBUG) Logger::D(TAG, "AddNetwork button pressed");
        OnAddNetworkButtonPressed();
    }
    else if (view == IView::Probe(mRefreshButton)) {
        if (DEBUG) Logger::D(TAG, "Refresh button pressed");
        RefreshAccessPoints(TRUE);
    }
    else if (view == IView::Probe(mSkipOrNextButton)) {
        if (DEBUG) Logger::D(TAG, "Skip/Next button pressed");
        String str1;
        GetString(R::string::wifi_setup_skip, &str1);
        AutoPtr<ICharSequence> seq;
        ITextView::Probe(view)->GetText((ICharSequence**)&seq);
        if (TextUtils::Equals(str1, CoreUtils::Unbox(seq))) {
            // We don't want to let Wifi enabled when a user press skip without choosing
            // any access point.
            Boolean res;
            mWifiManager->SetWifiEnabled(FALSE, &res);
            // Notify "skip"
            SetResult(RESULT_FIRST_USER);
        }
        else {
            SetResult(RESULT_OK);
        }
        Finish();
    }
    else if (view == IView::Probe(mConnectButton)) {
        if (DEBUG) Logger::D(TAG, "Connect button pressed");
        OnConnectButtonPressed();
    }
    else if (view == IView::Probe(mBackButton)) {
        if (DEBUG) Logger::D(TAG, "Back button pressed");
        OnBackButtonPressed();
    }
    return NOERROR;
}

void WifiSettingsForSetupWizardXL::HideSoftwareKeyboard()
{
    if (DEBUG) Logger::I(TAG, "Hiding software keyboard.");
    AutoPtr<IView> focusedView;
    GetCurrentFocus((IView**)&focusedView);
    if (focusedView != NULL) {
        AutoPtr<IBinder> token;
        focusedView->GetWindowToken((IBinder**)&token);
        Boolean res;
        mInputMethodManager->HideSoftInputFromWindow(token, 0, &res);
    }
}

void WifiSettingsForSetupWizardXL::UpdateConnectionState(
    /* [in] */ NetworkInfoDetailedState originalState)
{
    AutoPtr<IInterface> obj;
    sNetworkStateMap->Get(CoreUtils::Convert((Int32)originalState), (IInterface**)&obj);

    NetworkInfoDetailedState state = (NetworkInfoDetailedState) CoreUtils::Unbox(IInteger32::Probe(obj));

    if (originalState == NetworkInfoDetailedState_FAILED) {
        // We clean up the current connectivity status and let users select another network
        // if they want.
        RefreshAccessPoints(TRUE);
    }

    switch (state) {
        case NetworkInfoDetailedState_SCANNING: {
            if (mScreenState == SCREEN_STATE_DISCONNECTED) {
                if (mWifiSettings->GetAccessPointsCount() == 0) {
                    ShowScanningState();
                }
                else {
                    ShowDisconnectedProgressBar();
                    mWifiSettingsFragmentLayout->SetVisibility(IView::VISIBLE);
                    mBottomPadding->SetVisibility(IView::GONE);
                }
            }
            else {
                ShowDisconnectedProgressBar();
            }
            break;
        }
        case NetworkInfoDetailedState_CONNECTING: {
            if (mScreenState == SCREEN_STATE_CONNECTING) {
                ShowConnectingState();
            }
            break;
        }
        case NetworkInfoDetailedState_CONNECTED: {
            ShowConnectedState();
            break;
        }
        default:  // DISCONNECTED, FAILED
            if (mScreenState != SCREEN_STATE_CONNECTED &&
                    mWifiSettings->GetAccessPointsCount() > 0) {
                ShowDisconnectedState(Summary::Get(this, state));
            }
            break;
    }
    mPreviousNetworkState = state;
}

void WifiSettingsForSetupWizardXL::ShowDisconnectedState(
    /* [in] */ const String& stateString)
{
    ShowDisconnectedProgressBar();
    if (mScreenState == SCREEN_STATE_DISCONNECTED &&
            mWifiSettings->GetAccessPointsCount() > 0) {
        mWifiSettingsFragmentLayout->SetVisibility(IView::VISIBLE);
        mBottomPadding->SetVisibility(IView::GONE);
    }
    IView::Probe(mAddNetworkButton)->SetEnabled(TRUE);
    IView::Probe(mRefreshButton)->SetEnabled(TRUE);
}

void WifiSettingsForSetupWizardXL::ShowConnectingState()
{
    mScreenState = SCREEN_STATE_CONNECTING;

    IView::Probe(mBackButton)->SetVisibility(IView::VISIBLE);
    // We save this title and show it when authentication failed.
    mTitleView->GetText((ICharSequence**)&mEditingTitle);
    ShowConnectingTitle();
    ShowConnectingProgressBar();

    SetPaddingVisibility(IView::VISIBLE);
}

void WifiSettingsForSetupWizardXL::ShowConnectedState()
{
    // Once we show "connected" screen, we won't change it even when the device becomes
    // disconnected afterwards. We keep the state unless a user explicitly cancel it
    // (by pressing "back" button).
    mScreenState = SCREEN_STATE_CONNECTED;

    HideSoftwareKeyboard();
    SetPaddingVisibility(IView::VISIBLE);

    ShowConnectedTitle();
    ShowConnectedProgressBar();

    mWifiSettingsFragmentLayout->SetVisibility(IView::GONE);
    mConnectingStatusLayout->SetVisibility(IView::VISIBLE);

    mConnectingStatusView->SetText(R::string::wifi_setup_description_connected);
    IView::Probe(mConnectButton)->SetVisibility(IView::GONE);
    IView::Probe(mAddNetworkButton)->SetVisibility(IView::GONE);
    IView::Probe(mRefreshButton)->SetVisibility(IView::GONE);
    IView::Probe(mBackButton)->SetVisibility(IView::VISIBLE);
    ITextView::Probe(mBackButton)->SetText(R::string::wifi_setup_back);
    IView* skipOrNextButton = IView::Probe(mSkipOrNextButton);
    skipOrNextButton->SetVisibility(IView::VISIBLE);
    skipOrNextButton->SetEnabled(TRUE);
}

void WifiSettingsForSetupWizardXL::ShowDefaultTitle()
{
    String str;
    GetString(R::string::wifi_setup_title, &str);
    mTitleView->SetText(CoreUtils::Convert(str));
}

void WifiSettingsForSetupWizardXL::ShowAddNetworkTitle()
{
    mNetworkName = CoreUtils::Convert("");
    mTitleView->SetText(R::string::wifi_setup_title_add_network);
}

void WifiSettingsForSetupWizardXL::ShowEditingTitle()
{
    if (TextUtils::IsEmpty(mNetworkName) && mWifiConfig != NULL) {
        AutoPtr<IWifiConfigController> controller;
        ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->GetController((IWifiConfigController**)&controller);
        if (controller != NULL) {
            AutoPtr<IWifiConfiguration> config = ((WifiConfigController*)controller.Get())->GetConfig();
            if (config != NULL) {
                String SSID;
                config->GetSSID(&SSID);
                mNetworkName = CoreUtils::Convert(SSID);
            }
        }
        else {
            Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). Ignore them.");
        }
    }

    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, mNetworkName);
    String str;
    GetString(R::string::wifi_setup_title_editing_network, formatArgs, &str);
    mTitleView->SetText(CoreUtils::Convert(str));
}

void WifiSettingsForSetupWizardXL::ShowConnectingTitle()
{
    if (TextUtils::IsEmpty(mNetworkName) && mWifiConfig != NULL) {
        AutoPtr<IWifiConfigController> controller;
        ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->GetController((IWifiConfigController**)&controller);
        if (controller != NULL) {
            AutoPtr<IWifiConfiguration> config = ((WifiConfigController*)controller.Get())->GetConfig();
            if (config != NULL) {
                String SSID;
                config->GetSSID(&SSID);
                mNetworkName = CoreUtils::Convert(SSID);
            }
        }
        else {
            Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). Ignore them.");
        }
    }

    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, mNetworkName);
    String str;
    GetString(R::string::wifi_setup_title_connecting_network, formatArgs, &str);
    mTitleView->SetText(CoreUtils::Convert(str));
}

void WifiSettingsForSetupWizardXL::ShowConnectedTitle()
{
    if (TextUtils::IsEmpty(mNetworkName) && mWifiConfig != NULL) {
        AutoPtr<IWifiConfigController> controller;
        ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->GetController((IWifiConfigController**)&controller);
        if (controller != NULL) {
            AutoPtr<IWifiConfiguration> config = ((WifiConfigController*)controller.Get())->GetConfig();
            if (config != NULL) {
                String SSID;
                config->GetSSID(&SSID);
                mNetworkName = CoreUtils::Convert(SSID);
            }
        }
        else {
            Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). Ignore them.");
        }
    }

    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, mNetworkName);
    String str;
    GetString(R::string::wifi_setup_title_connected_network, formatArgs, &str);
    mTitleView->SetText(CoreUtils::Convert(str));
}

void WifiSettingsForSetupWizardXL::ShowTopDividerWithProgressBar()
{
    IView::Probe(mProgressBar)->SetVisibility(IView::VISIBLE);
    mTopDividerNoProgress->SetVisibility(IView::GONE);
    mBottomPadding->SetVisibility(IView::GONE);
}

void WifiSettingsForSetupWizardXL::ShowScanningState()
{
    SetPaddingVisibility(IView::VISIBLE);
    mWifiSettingsFragmentLayout->SetVisibility(IView::GONE);
    ShowScanningProgressBar();
}

void WifiSettingsForSetupWizardXL::OnAddNetworkButtonPressed()
{
    mWifiSettings->OnAddNetworkPressed();
}

void WifiSettingsForSetupWizardXL::ShowConfigUi(
    /* [in] */ IAccessPoint* selectedAccessPoint,
    /* [in] */ Boolean edit)
{
    mScreenState = SCREEN_STATE_EDITING;

    if (selectedAccessPoint != NULL &&
            (((CAccessPoint*)selectedAccessPoint)->mSecurity == AccessPoint::SECURITY_WEP ||
                    ((CAccessPoint*)selectedAccessPoint)->mSecurity == AccessPoint::SECURITY_PSK)) {
        // We forcibly set edit as TRUE so that users can modify every field if they want,
        // while config UI doesn't allow them to edit some of them when edit is FALSE
        // (e.g. password field is hiden when edit==FALSE).
        edit = TRUE;
    }

    // We don't want to keep scanning Wifi networks during users' configuring a network.
    mWifiSettings->PauseWifiScan();

    mWifiSettingsFragmentLayout->SetVisibility(IView::GONE);
    mConnectingStatusLayout->SetVisibility(IView::GONE);
    AutoPtr<IView> tmp;
    FindViewById(R::id::wifi_config_ui, (IView**)&tmp);
    IViewGroup* parent = IViewGroup::Probe(tmp);
    tmp->SetVisibility(IView::VISIBLE);
    parent->RemoveAllViews();
    mWifiConfig = new WifiConfigUiForSetupWizardXL(this, parent, selectedAccessPoint, edit);

    if (selectedAccessPoint == NULL) {  // "Add network" flow
        ShowAddNetworkTitle();
        IView::Probe(mConnectButton)->SetVisibility(IView::VISIBLE);

        ShowDisconnectedProgressBar();
        ShowEditingButtonState();
    }
    else if (((CAccessPoint*)selectedAccessPoint)->mSecurity == AccessPoint::SECURITY_NONE) {
        AutoPtr<ICharSequence> seq;
        IPreference::Probe(selectedAccessPoint)->GetTitle((ICharSequence**)&seq);
        mNetworkName = seq;

        // OnConnectButtonPressed() will change visibility status.
        Boolean res;
        IView::Probe(mConnectButton)->PerformClick(&res);
    }
    else {
        AutoPtr<ICharSequence> seq;
        IPreference::Probe(selectedAccessPoint)->GetTitle((ICharSequence**)&seq);
        mNetworkName = seq;
        ShowEditingTitle();
        ShowDisconnectedProgressBar();
        ShowEditingButtonState();
        if (((CAccessPoint*)selectedAccessPoint)->mSecurity == AccessPoint::SECURITY_EAP) {
            OnEapNetworkSelected();
        }
        else {
            IView::Probe(mConnectButton)->SetVisibility(IView::VISIBLE);

            // WifiConfigController shows Connect button as "Save" when edit==TRUE and a user
            // tried to connect the network.
            // In SetupWizard, we just show the button as "Connect" instead.
            ITextView::Probe(mConnectButton)->SetText(R::string::wifi_connect);
            ITextView::Probe(mBackButton)->SetText(R::string::wifi_setup_cancel);
        }
    }
}

Boolean WifiSettingsForSetupWizardXL::InitSecurityFields(
    /* [in] */ IView* view,
    /* [in] */ Int32 accessPointSecurity)
{
    // Reset all states tweaked below.
    AutoPtr<IView> viewBy;
    view->FindViewById(R::id::eap_not_supported, (IView**)&viewBy);
    viewBy->SetVisibility(IView::GONE);
    viewBy = NULL;
    view->FindViewById(R::id::eap_not_supported_for_add_network, (IView**)&viewBy);
    viewBy->SetVisibility(IView::GONE);
    viewBy = NULL;
    view->FindViewById(R::id::ssid_text, (IView**)&viewBy);
    viewBy->SetVisibility(IView::VISIBLE);
    viewBy = NULL;
    view->FindViewById(R::id::ssid_layout, (IView**)&viewBy);
    viewBy->SetVisibility(IView::VISIBLE);

    if (accessPointSecurity == AccessPoint::SECURITY_EAP) {
        SetPaddingVisibility(IView::VISIBLE);
        HideSoftwareKeyboard();

        // In SetupWizard for XLarge screen, we don't have enough space for showing
        // configurations needed for EAP. We instead disable the whole feature there and let
        // users configure those networks after the setup.
        viewBy = NULL;
        view->FindViewById(R::id::type_ssid, (IView**)&viewBy);
        Int32 visibility;
        viewBy->GetVisibility(&visibility);
        if (visibility == IView::VISIBLE) {
            viewBy = NULL;
            view->FindViewById(R::id::eap_not_supported_for_add_network, (IView**)&viewBy);
            viewBy->SetVisibility(IView::VISIBLE);
        }
        else {
            viewBy = NULL;
            view->FindViewById(R::id::eap_not_supported, (IView**)&viewBy);
            viewBy->SetVisibility(IView::VISIBLE);
        }
        viewBy = NULL;
        view->FindViewById(R::id::security_fields, (IView**)&viewBy);
        viewBy->SetVisibility(IView::GONE);
        viewBy = NULL;
        view->FindViewById(R::id::ssid_text, (IView**)&viewBy);
        viewBy->SetVisibility(IView::GONE);
        viewBy = NULL;
        view->FindViewById(R::id::ssid_layout, (IView**)&viewBy);
        viewBy->SetVisibility(IView::GONE);
        OnEapNetworkSelected();

        // This method did init security fields by itself. The caller must not do it.
        return FALSE;
    }

    IView::Probe(mConnectButton)->SetVisibility(IView::VISIBLE);
    SetPaddingVisibility(IView::GONE);

    // In "add network" flow, we'll see multiple InitSecurityFields() calls with different
    // accessPointSecurity variable. We want to show software keyboard conditionally everytime
    // when this method is called.
    if (mWifiConfig != NULL) {
        if (accessPointSecurity == AccessPoint::SECURITY_PSK ||
                accessPointSecurity == AccessPoint::SECURITY_WEP) {
            ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->RequestFocusAndShowKeyboard(R::id::password);
        }
        else {
            ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->RequestFocusAndShowKeyboard(R::id::ssid);
        }
    }

    // Let the caller init security fields.
    return TRUE;
}

void WifiSettingsForSetupWizardXL::OnEapNetworkSelected()
{
    IView::Probe(mConnectButton)->SetVisibility(IView::GONE);
    ITextView::Probe(mBackButton)->SetText(R::string::wifi_setup_back);
}

void WifiSettingsForSetupWizardXL::ShowEditingButtonState()
{
    IView::Probe(mSkipOrNextButton)->SetVisibility(IView::GONE);
    IView::Probe(mAddNetworkButton)->SetVisibility(IView::GONE);
    IView::Probe(mRefreshButton)->SetVisibility(IView::GONE);
    IView::Probe(mBackButton)->SetVisibility(IView::VISIBLE);
}

void WifiSettingsForSetupWizardXL::OnConnectButtonPressed()
{
    mScreenState = SCREEN_STATE_CONNECTING;

    AutoPtr<IWifiConfigController> controller;
    ((WifiConfigUiForSetupWizardXL*)mWifiConfig.Get())->GetController((IWifiConfigController**)&controller);

    mWifiSettings->Submit(controller);

    // UpdateConnectionState() isn't called soon by the main Wifi module after the user's
    // "connect" request, and the user still sees "not connected" message for a while, which
    // looks strange for users though legitimate from the view of the module.
    //
    // We instead manually show "connecting" message before the system gets actual
    // "connecting" message from Wifi module.
    ShowConnectingState();

    // Might be better to delay showing this button.
    IView::Probe(mBackButton)->SetVisibility(IView::VISIBLE);
    ITextView::Probe(mBackButton)->SetText(R::string::wifi_setup_back);

    AutoPtr<IView> parent;
    FindViewById(R::id::wifi_config_ui, (IView**)&parent);
    parent->SetVisibility(IView::GONE);
    mConnectingStatusLayout->SetVisibility(IView::VISIBLE);
    mConnectingStatusView->SetText(R::string::wifi_setup_description_connecting);

    IView* skipOrNextButton = IView::Probe(mSkipOrNextButton);
    skipOrNextButton->SetVisibility(IView::VISIBLE);
    skipOrNextButton->SetEnabled(FALSE);
    IView::Probe(mConnectButton)->SetVisibility(IView::GONE);
    IView::Probe(mAddNetworkButton)->SetVisibility(IView::GONE);
    IView::Probe(mRefreshButton)->SetVisibility(IView::GONE);
}

void WifiSettingsForSetupWizardXL::OnBackButtonPressed()
{
    if (mScreenState == SCREEN_STATE_CONNECTING || mScreenState == SCREEN_STATE_CONNECTED) {
        if (DEBUG) Logger::D(TAG, "Back button pressed after connect action.");
        mScreenState = SCREEN_STATE_DISCONNECTED;

        // When a user press "Back" button after pressing "Connect" button, we want to cancel
        // the "Connect" request and refresh the whole Wifi status.
        RestoreFirstVisibilityState();

        IView::Probe(mSkipOrNextButton)->SetEnabled(TRUE);
        ChangeNextButtonState(FALSE);  // Skip

        // Wifi list becomes empty for a moment. We show "scanning" effect to a user so that
        // he/she won't be astonished there. This stops once the scan finishes.
        ShowScanningState();

        // Remembered networks may be re-used during SetupWizard, which confuse users.
        // We force the module to forget them to reduce UX complexity
        AutoPtr<IList> configs; // List<WifiConfiguration>
        mWifiManager->GetConfiguredNetworks((IList**)&configs);
        Int32 count;
        configs->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            configs->Get(i, (IInterface**)&obj);
            IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
            String SSID;
            config->GetSSID(&SSID);
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if (DEBUG) {
                Logger::D(TAG, "forgeting Wi-Fi network \"%s\" (id: %d)", SSID.string(), networkId);
            }
            AutoPtr<WifiManagerActionListener> listener = new WifiManagerActionListener();
            mWifiManager->Forget(networkId, listener);
        }

        mWifiSettingsFragmentLayout->SetVisibility(IView::GONE);
        RefreshAccessPoints(TRUE);
    }
    else { // During user's Wifi configuration.
        mScreenState = SCREEN_STATE_DISCONNECTED;
        mWifiSettings->ResumeWifiScan();

        RestoreFirstVisibilityState();

        IView::Probe(mAddNetworkButton)->SetEnabled(TRUE);
        IView::Probe(mRefreshButton)->SetEnabled(TRUE);
        IView::Probe(mSkipOrNextButton)->SetEnabled(TRUE);
        ShowDisconnectedProgressBar();
        mWifiSettingsFragmentLayout->SetVisibility(IView::VISIBLE);
        mBottomPadding->SetVisibility(IView::GONE);
    }

    SetPaddingVisibility(IView::VISIBLE);
    mConnectingStatusLayout->SetVisibility(IView::GONE);
    AutoPtr<IView> tmp;
    FindViewById(R::id::wifi_config_ui, (IView**)&tmp);
    IViewGroup* parent = IViewGroup::Probe(tmp);
    parent->RemoveAllViews();
    tmp->SetVisibility(IView::GONE);
    mWifiConfig = NULL;
}

void WifiSettingsForSetupWizardXL::ChangeNextButtonState(
    /* [in] */ Boolean connected)
{
    if (connected) {
        ITextView::Probe(mSkipOrNextButton)->SetText(R::string::wifi_setup_next);
    }
    else {
        ITextView::Probe(mSkipOrNextButton)->SetText(R::string::wifi_setup_skip);
    }
}

void WifiSettingsForSetupWizardXL::OnAccessPointsUpdated(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ ICollection* accessPoints) //Collection<AccessPoint>
{
    // If we already show some of access points but the bar still shows "scanning" state, it
    // should be stopped.
    Int32 size;
    Boolean res;
    if ((mProgressBar->IsIndeterminate(&res), res) && (accessPoints->GetSize(&size), size > 0)) {
        ShowDisconnectedProgressBar();
        if (mScreenState == SCREEN_STATE_DISCONNECTED) {
            mWifiSettingsFragmentLayout->SetVisibility(IView::VISIBLE);
            mBottomPadding->SetVisibility(IView::GONE);
        }
        IView::Probe(mAddNetworkButton)->SetEnabled(TRUE);
        IView::Probe(mRefreshButton)->SetEnabled(TRUE);
    }

    AutoPtr<IIterator> iterator;
    accessPoints->GetIterator((IIterator**)&iterator);
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        IAccessPoint* accessPoint = IAccessPoint::Probe(obj);
        IPreference::Probe(accessPoint)->SetLayoutResource(R::layout::custom_preference);
        Boolean result;
        IPreferenceGroup::Probe(preferenceScreen)->AddPreference(IPreference::Probe(accessPoint), &result);
    }
}

void WifiSettingsForSetupWizardXL::RefreshAccessPoints(
    /* [in] */ Boolean disconnectNetwork)
{
    ShowScanningState();

    if (disconnectNetwork) {
        Boolean res;
        mWifiManager->Disconnect(&res);
    }

    mWifiSettings->RefreshAccessPoints();
}

void WifiSettingsForSetupWizardXL::OnSupplicantStateChanged(
    /* [in] */ IIntent* intent)
{
    Int32 errorCode;
    intent->GetInt32Extra(IWifiManager::EXTRA_SUPPLICANT_ERROR, -1, &errorCode);
    if (errorCode == IWifiManager::ERROR_AUTHENTICATING) {
        Logger::I(TAG, "Received authentication error event.");
        OnAuthenticationFailure();
    }
}

void WifiSettingsForSetupWizardXL::OnAuthenticationFailure()
{
    mScreenState = SCREEN_STATE_EDITING;

    IView::Probe(mSkipOrNextButton)->SetVisibility(IView::GONE);
    IView* connectButton = IView::Probe(mConnectButton);
    connectButton->SetVisibility(IView::VISIBLE);
    connectButton->SetEnabled(TRUE);

    if (!TextUtils::IsEmpty(mEditingTitle)) {
        mTitleView->SetText(mEditingTitle);
    }
    else {
        Logger::W(TAG, "Title during editing/adding a network was empty.");
        ShowEditingTitle();
    }

    AutoPtr<IView> parent;
    FindViewById(R::id::wifi_config_ui, (IView**)&parent);
    parent->SetVisibility(IView::VISIBLE);
    mConnectingStatusLayout->SetVisibility(IView::GONE);

    ShowDisconnectedProgressBar();
    SetPaddingVisibility(IView::GONE);
}

ECode WifiSettingsForSetupWizardXL::SetPaddingVisibility(
    /* [in] */ Int32 visibility)
{
    mTopPadding->SetVisibility(visibility);
    mContentPadding->SetVisibility(visibility);
    return NOERROR;
}

void WifiSettingsForSetupWizardXL::ShowDisconnectedProgressBar()
{
    // The device may report DISCONNECTED during connecting to a network, at which we don't
    // want to lose bottom padding of top divider implicitly added by ProgressBar.
    if (mScreenState == SCREEN_STATE_DISCONNECTED) {
        IView::Probe(mProgressBar)->SetVisibility(IView::GONE);
        mProgressBar->SetIndeterminate(FALSE);
        mTopDividerNoProgress->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mProgressBar)->SetVisibility(IView::VISIBLE);
        mProgressBar->SetIndeterminate(FALSE);
        mProgressBar->SetProgress(0);
        mTopDividerNoProgress->SetVisibility(IView::GONE);
    }
}

void WifiSettingsForSetupWizardXL::ShowScanningProgressBar()
{
    ShowTopDividerWithProgressBar();
    mProgressBar->SetIndeterminate(TRUE);
}

void WifiSettingsForSetupWizardXL::ShowConnectingProgressBar()
{
    ShowTopDividerWithProgressBar();
    mProgressBar->SetIndeterminate(FALSE);
    mProgressBar->SetProgress(1);
}

void WifiSettingsForSetupWizardXL::ShowConnectedProgressBar()
{
    ShowTopDividerWithProgressBar();
    mProgressBar->SetIndeterminate(FALSE);
    mProgressBar->SetProgress(2);
}

void WifiSettingsForSetupWizardXL::OnSaveNetwork(
    /* [in] */ IWifiConfiguration* config)
{
    // We want to both save and connect a network. ConnectNetwork() does both.
    AutoPtr<IWifiManagerActionListener> listener = new WifiManagerActionListener();
    mWifiManager->Connect(config, listener);
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
