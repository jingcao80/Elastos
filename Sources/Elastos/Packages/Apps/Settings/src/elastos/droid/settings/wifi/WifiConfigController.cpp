
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.KeyStore.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/wifi/WifiConfigController.h"
#include "elastos/droid/settings/wifi/WifiSettingsForSetupWizardXL.h"
#include "elastos/droid/settings/wifi/Summary.h"
#include "elastos/droid/settings/ProxySelector.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::KeyStore::Security::IKeyStore;
// using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::UNASSIGNED_IpAssignment;
using Elastos::Droid::Net::STATIC_IpAssignment;
using Elastos::Droid::Net::DHCP_IpAssignment;
using Elastos::Droid::Net::UNASSIGNED_ProxySettings;
using Elastos::Droid::Net::STATIC_ProxySettings;
using Elastos::Droid::Net::PAC_ProxySettings;
using Elastos::Droid::Net::NONE_ProxySettings;
using Elastos::Droid::Net::NetworkInfoDetailedState_NONE;
using Elastos::Droid::Net::IpConfigurationProxySettings;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CProxyInfo;
using Elastos::Droid::Net::CStaticIpConfiguration;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Settings::Wifi::EIID_IWifiConfigController;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::CWifiEnterpriseConfig;
using Elastos::Droid::Wifi::IWifiEnterpriseConfig;
using Elastos::Droid::Wifi::IWifiEnterpriseConfigEap;
using Elastos::Droid::Wifi::IWifiEnterpriseConfigPhase2;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::IWifiConfigurationAuthAlgorithm;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IBitSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const Int32 WifiConfigController::PROXY_NONE = 0;
const Int32 WifiConfigController::PROXY_STATIC = 1;
const Int32 WifiConfigController::PROXY_PAC = 2;
const Int32 WifiConfigController::WIFI_EAP_METHOD_PEAP;
const Int32 WifiConfigController::WIFI_EAP_METHOD_TLS;
const Int32 WifiConfigController::WIFI_EAP_METHOD_TTLS;
const Int32 WifiConfigController::WIFI_EAP_METHOD_PWD;
const Int32 WifiConfigController::WIFI_PEAP_PHASE2_NONE;
const Int32 WifiConfigController::WIFI_PEAP_PHASE2_MSCHAPV2;
const Int32 WifiConfigController::WIFI_PEAP_PHASE2_GTC;
const String WifiConfigController::TAG("WifiConfigController");
const Int32 WifiConfigController::DHCP = 0;
const Int32 WifiConfigController::STATIC_IP = 1;
const Int32 WifiConfigController::unspecifiedCertIndex = 0;

//===============================================================================
//                  WifiConfigController::AfterTextChangedRunnable
//===============================================================================

WifiConfigController::AfterTextChangedRunnable::AfterTextChangedRunnable(
    /* [in] */ WifiConfigController* host)
    : mHost(host)
{}

WifiConfigController::AfterTextChangedRunnable::~AfterTextChangedRunnable()
{}

ECode WifiConfigController::AfterTextChangedRunnable::Run()
{
    mHost->EnableSubmitIfAppropriate();
    return NOERROR;
}

//===============================================================================
//                  WifiConfigController
//===============================================================================

CAR_INTERFACE_IMPL_5(WifiConfigController, Object, IWifiConfigController, ITextWatcher, INoCopySpan, IAdapterViewOnItemSelectedListener, ICompoundButtonOnCheckedChangeListener);

WifiConfigController::WifiConfigController(
    /* [in] */ IWifiConfigUiBase* parent,
    /* [in] */ IView* _view,
    /* [in] */ AccessPoint* accessPoint,
    /* [in] */ Boolean edit)
    : mInXlSetupWizard(FALSE)
    , mAccessPointSecurity(0)
    , mUnspecifiedCert(String("unspecified"))
    , mIpAssignment(UNASSIGNED_IpAssignment)
    , mProxySettings(UNASSIGNED_ProxySettings)
    , mEdit(FALSE)
{
    mConfigUi = parent;
    mInXlSetupWizard = IWifiConfigUiForSetupWizardXL::Probe(parent) != NULL;

    mView = _view;
    mAccessPoint = accessPoint;
    mAccessPointSecurity = (accessPoint == NULL) ? AccessPoint::SECURITY_NONE :
            accessPoint->mSecurity;
    mEdit = edit;

    CHandler::New((IHandler**)&mTextViewChangedHandler);
    mConfigUi->GetContext((IContext**)&mContext);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    res->GetStringArray(R::array::wifi_signal, (ArrayOf<String>**)&mLevels);


    AutoPtr<ArrayOf<String> > arrayStr;
    res->GetStringArray(R::array::wifi_peap_phase2_entries, (ArrayOf<String>**)&arrayStr);
    AutoPtr< ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(arrayStr->GetLength());
    for (Int32 i = 0; i < arrayStr->GetLength(); i++) {
        (*array)[i] = CoreUtils::Convert((*arrayStr)[i]);
    }
    CArrayAdapter::New(mContext, Elastos::Droid::R::layout::simple_spinner_item,
            array, (IArrayAdapter**)&PHASE2_PEAP_ADAPTER);
    PHASE2_PEAP_ADAPTER->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);

    arrayStr = NULL;
    res->GetStringArray(R::array::wifi_phase2_entries, (ArrayOf<String>**)&arrayStr);
    array = NULL;
    array = ArrayOf<IInterface*>::Alloc(arrayStr->GetLength());
    for (Int32 i = 0; i < arrayStr->GetLength(); i++) {
        (*array)[i] = CoreUtils::Convert((*arrayStr)[i]);
    }

    CArrayAdapter::New(mContext, Elastos::Droid::R::layout::simple_spinner_item,
            array, (IArrayAdapter**)&PHASE2_FULL_ADAPTER);
    PHASE2_FULL_ADAPTER->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);

    mContext->GetString(R::string::wifi_unspecified, &mUnspecifiedCert);
    AutoPtr<IView> view;
    mView->FindViewById(R::id::ip_settings, (IView**)&view);
    mIpSettingsSpinner = ISpinner::Probe(view);
    IAdapterView::Probe(mIpSettingsSpinner)->SetOnItemSelectedListener((IAdapterViewOnItemSelectedListener*)this);
    view = NULL;
    mView->FindViewById(R::id::proxy_settings, (IView**)&view);
    mProxySettingsSpinner = ISpinner::Probe(view);
    IAdapterView::Probe(mProxySettingsSpinner)->SetOnItemSelectedListener((IAdapterViewOnItemSelectedListener*)this);

    if (mAccessPoint == NULL) { // new network
        mConfigUi->SetTitle(R::string::wifi_add_network);

        view = NULL;
        mView->FindViewById(R::id::ssid, (IView**)&view);
        mSsidView = ITextView::Probe(view);
        mSsidView->AddTextChangedListener((ITextWatcher*)this);
        view = NULL;
        mView->FindViewById(R::id::security, (IView**)&view);
        mSecuritySpinner = ISpinner::Probe(view);
        IAdapterView::Probe(mSecuritySpinner)->SetOnItemSelectedListener((IAdapterViewOnItemSelectedListener*)this);
        if (mInXlSetupWizard) {
            view = NULL;
            mView->FindViewById(R::id::type_ssid, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            view = NULL;
            mView->FindViewById(R::id::type_security, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            // We want custom layout. The content must be same as the other cases.

            arrayStr = NULL;
            res->GetStringArray(R::array::wifi_security_no_eap, (ArrayOf<String>**)&arrayStr);
            array = NULL;
            array = ArrayOf<IInterface*>::Alloc(arrayStr->GetLength());
            for (Int32 i = 0; i < arrayStr->GetLength(); i++) {
                (*array)[i] = CoreUtils::Convert((*arrayStr)[i]);
            }

            AutoPtr<IArrayAdapter> adapter;
            CArrayAdapter::New(mContext, R::layout::wifi_setup_custom_list_item_1,
                    Elastos::Droid::R::id::text1, array, (IArrayAdapter**)&adapter);
            IAdapterView::Probe(mSecuritySpinner)->SetAdapter(IAdapter::Probe(adapter));
        }
        else {
            view = NULL;
            mView->FindViewById(R::id::type, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
        }

        ShowIpConfigFields();
        ShowProxyFields();
        view = NULL;
        mView->FindViewById(R::id::wifi_advanced_toggle, (IView**)&view);
        view->SetVisibility(IView::VISIBLE);
        view = NULL;
        mView->FindViewById(R::id::wifi_advanced_togglebox, (IView**)&view);
        ICompoundButton::Probe(view)->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)this);

        String str;
        res->GetString(R::string::wifi_save, &str);
        mConfigUi->SetSubmitButton(CoreUtils::Convert(str));
    }
    else {
        mConfigUi->SetTitle(CoreUtils::Convert(mAccessPoint->mSsid));

        view = NULL;
        mView->FindViewById(R::id::info, (IView**)&view);
        AutoPtr<IViewGroup> group = IViewGroup::Probe(view);

        Boolean showAdvancedFields = FALSE;
        if (mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
            AutoPtr<IWifiConfiguration> config = mAccessPoint->GetConfig();
            IpConfigurationIpAssignment ips;
            config->GetIpAssignment(&ips);
            if (ips == STATIC_IpAssignment) {
                IAdapterView::Probe(mIpSettingsSpinner)->SetSelection(STATIC_IP);
                showAdvancedFields = TRUE;
                // Display IP address.
                AutoPtr<IStaticIpConfiguration> staticConfig;
                config->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticConfig);
                AutoPtr<ILinkAddress> ipAddress;
                staticConfig->GetIpAddress((ILinkAddress**)&ipAddress);
                if (staticConfig != NULL && ipAddress != NULL) {
                    AutoPtr<IInetAddress> address;
                    ipAddress->GetAddress((IInetAddress**)&address);
                    String hostAddress;
                    address->GetHostAddress(&hostAddress);
                    AddRow(group, R::string::wifi_ip_address, hostAddress);
                }
            }
            else {
                IAdapterView::Probe(mIpSettingsSpinner)->SetSelection(DHCP);
            }


            IpConfigurationProxySettings icp;
            config->GetProxySettings(&icp);
            if (icp == STATIC_ProxySettings) {
                IAdapterView::Probe(mProxySettingsSpinner)->SetSelection(PROXY_STATIC);
                showAdvancedFields = TRUE;
            }
            else if (icp == PAC_ProxySettings) {
                IAdapterView::Probe(mProxySettingsSpinner)->SetSelection(PROXY_PAC);
                showAdvancedFields = TRUE;
            }
            else {
                IAdapterView::Probe(mProxySettingsSpinner)->SetSelection(PROXY_NONE);
            }
        }

        if (mAccessPoint->mNetworkId == IWifiConfiguration::INVALID_NETWORK_ID || mEdit) {
            ShowSecurityFields();
            ShowIpConfigFields();
            ShowProxyFields();
            view = NULL;
            mView->FindViewById(R::id::wifi_advanced_toggle, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            view = NULL;
            mView->FindViewById(R::id::wifi_advanced_togglebox, (IView**)&view);
            ICompoundButton::Probe(view)->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)this);

            if (showAdvancedFields) {
                view = NULL;
                mView->FindViewById(R::id::wifi_advanced_togglebox, (IView**)&view);
                ICheckable::Probe(view)->SetChecked(TRUE);
                view = NULL;
                mView->FindViewById(R::id::wifi_advanced_fields, (IView**)&view);
                view->SetVisibility(IView::VISIBLE);
            }
        }

        if (mEdit) {
            String str;
            res->GetString(R::string::wifi_save, &str);
            mConfigUi->SetSubmitButton(CoreUtils::Convert(str));
        }
        else {
            NetworkInfoDetailedState state = mAccessPoint->GetState();
            String signalLevel = GetSignalString();

            if (state == NetworkInfoDetailedState_NONE && !signalLevel.IsNull()) {
                String str;
                res->GetString(R::string::wifi_connect, &str);
                mConfigUi->SetSubmitButton(CoreUtils::Convert(str));
            }
            else {
                if (state != NetworkInfoDetailedState_NONE) {
                    AutoPtr<IContext> context;
                    mConfigUi->GetContext((IContext**)&context);
                    AddRow(group, R::string::wifi_status, Summary::Get(context, state));
                }

                if (!signalLevel.IsNull()) {
                    AddRow(group, R::string::wifi_signal, signalLevel);
                }

                AutoPtr<IWifiInfo> info = mAccessPoint->GetInfo();
                Int32 linkSpeed;
                if (info != NULL && (info->GetLinkSpeed(&linkSpeed), linkSpeed) != -1) {
                    AddRow(group, R::string::wifi_speed, StringUtils::ToString(linkSpeed)
                            + IWifiInfo::LINK_SPEED_UNITS);
                }

                Int32 result;
                if (info != NULL && (info->GetFrequency(&result), result) != -1) {
                    Int32 frequency = result;
                    String band;

                    if (frequency >= AccessPoint::LOWER_FREQ_24GHZ
                            && frequency < AccessPoint::HIGHER_FREQ_24GHZ) {
                        res->GetString(R::string::wifi_band_24ghz, &band);
                    }
                    else if (frequency >= AccessPoint::LOWER_FREQ_5GHZ
                            && frequency < AccessPoint::HIGHER_FREQ_5GHZ) {
                        res->GetString(R::string::wifi_band_5ghz, &band);
                    }
                    else {
                        Logger::E(TAG, "Unexpected frequency %d", frequency);
                    }
                    if (!band.IsNull()) {
                        AddRow(group, R::string::wifi_frequency, band);
                    }
                }

                AddRow(group, R::string::wifi_security, mAccessPoint->GetSecurityString(FALSE));
                view = NULL;
                mView->FindViewById(R::id::ip_fields, (IView**)&view);
                view->SetVisibility(IView::GONE);
            }

            AutoPtr<IActivityManagerHelper> helper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
            Int32 currentUser;
            helper->GetCurrentUser(&currentUser);
            if (mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID
                    && currentUser == IUserHandle::USER_OWNER) {
                String str;
                res->GetString(R::string::wifi_forget, &str);
                mConfigUi->SetForgetButton(CoreUtils::Convert(str));
            }
        }
    }

    if (mEdit || (mAccessPoint->GetState() == NetworkInfoDetailedState_NONE
            && mAccessPoint->GetLevel() != -1)){
        String str;
        res->GetString(R::string::wifi_cancel, &str);
        mConfigUi->SetCancelButton(CoreUtils::Convert(str));
    }
    else{
        String str;
        res->GetString(R::string::wifi_display_options_done, &str);
        mConfigUi->SetCancelButton(CoreUtils::Convert(str));
    }
    AutoPtr<IButton> button;
    mConfigUi->GetSubmitButton((IButton**)&button);
    if (button != NULL) {
        EnableSubmitIfAppropriate();
    }
}

void WifiConfigController::AddRow(
    /* [in] */ IViewGroup* group,
    /* [in] */ Int32 name,
    /* [in] */ const String& value)
{
    AutoPtr<ILayoutInflater> inflater;
    mConfigUi->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> row;
    inflater->Inflate(R::layout::wifi_dialog_row, group, FALSE, (IView**)&row);
    AutoPtr<IView> view;
    row->FindViewById(R::id::name, (IView**)&view);
    ITextView::Probe(view)->SetText(CoreUtils::Convert(StringUtils::ToString(name)));
    view = NULL;
    row->FindViewById(R::id::value, (IView**)&view);
    ITextView::Probe(view)->SetText(CoreUtils::Convert(value));
    group->AddView(row);
}

String WifiConfigController::GetSignalString()
{
    Int32 level = mAccessPoint->GetLevel();

    return (level > -1 && level < mLevels->GetLength()) ? (*mLevels)[level] : String(NULL);
}

void WifiConfigController::HideSubmitButton()
{
    AutoPtr<IButton> submit;
    mConfigUi->GetSubmitButton((IButton**)&submit);
    if (submit == NULL) return;

    IView::Probe(submit)->SetVisibility(IView::GONE);
}

void WifiConfigController::EnableSubmitIfAppropriate()
{
    AutoPtr<IButton> submit;
    mConfigUi->GetSubmitButton((IButton**)&submit);
    if (submit == NULL) return;

    Boolean enabled = FALSE;
    Boolean passwordInvalid = FALSE;

    Int32 length;
    if (mPasswordView != NULL &&
        ((mAccessPointSecurity == AccessPoint::SECURITY_WEP && (mPasswordView->GetLength(&length), length) == 0) ||
        (mAccessPointSecurity == AccessPoint::SECURITY_PSK && (mPasswordView->GetLength(&length), length) < 8))) {
        passwordInvalid = TRUE;
    }

    if ((mSsidView != NULL && (mSsidView->GetLength(&length), length) == 0) ||
        ((mAccessPoint == NULL || mAccessPoint->mNetworkId == IWifiConfiguration::INVALID_NETWORK_ID) &&
        passwordInvalid)) {
        enabled = FALSE;
    }
    else {
        if (IpAndProxyFieldsAreValid()) {
            enabled = TRUE;
        }
        else {
            enabled = FALSE;
        }
    }
    IView::Probe(submit)->SetEnabled(enabled);
}

AutoPtr<IWifiConfiguration> WifiConfigController::GetConfig()
{
    if (mAccessPoint != NULL && mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID && !mEdit) {
        return NULL;
    }

    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);

    if (mAccessPoint == NULL) {
        AutoPtr<ICharSequence> seq;
        mSsidView->GetText((ICharSequence**)&seq);
        config->SetSSID(AccessPoint::ConvertToQuotedString(TO_STR(seq)));
        // If the user adds a network manually, assume that it is hidden.
        config->SetHiddenSSID(TRUE);
    }
    else if (mAccessPoint->mNetworkId == IWifiConfiguration::INVALID_NETWORK_ID) {
        config->SetSSID(AccessPoint::ConvertToQuotedString(mAccessPoint->mSsid));
    }
    else {
        config->SetNetworkId(mAccessPoint->mNetworkId);
    }

    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    AutoPtr<IBitSet> allowedAuthAlgorithms;
    config->GetAllowedAuthAlgorithms((IBitSet**)&allowedAuthAlgorithms);

    switch (mAccessPointSecurity) {
        case AccessPoint::SECURITY_NONE:
            allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::NONE);
            break;

        case AccessPoint::SECURITY_WEP: {
            allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::NONE);
            allowedAuthAlgorithms->Set(IWifiConfigurationAuthAlgorithm::OPEN);
            allowedAuthAlgorithms->Set(IWifiConfigurationAuthAlgorithm::SHARED);
            Int32 len;
            if ((mPasswordView->GetLength(&len), len) != 0) {
                Int32 length = len;
                AutoPtr<ICharSequence> seq;
                mPasswordView->GetText((ICharSequence**)&seq);
                String password = TO_STR(seq);
                // WEP-40, WEP-104, and 256-bit WEP (WEP-232?)
                Boolean res;
                StringUtils::Matches(password, "[0-9A-Fa-f]*", &res);
                AutoPtr< ArrayOf<String> > args;
                config->GetWepKeys((ArrayOf<String>**)&args);
                if ((length == 10 || length == 26 || length == 58) && res) {
                    (*args)[0] = password;
                    config->SetWepKeys(args);
                }
                else {
                    StringBuilder builder;
                    builder += '"';
                    builder += password;
                    builder += '"';
                    (*args)[0] = builder.ToString();
                    config->SetWepKeys(args);
                }
            }
            break;
        }

        case AccessPoint::SECURITY_PSK: {
            allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::WPA_PSK);
            Int32 length;
            if ((mPasswordView->GetLength(&length), length) != 0) {
                AutoPtr<ICharSequence> seq;
                mPasswordView->GetText((ICharSequence**)&seq);
                String password = TO_STR(seq);
                Boolean res;
                StringUtils::Matches(password, "[0-9A-Fa-f]{64}", &res);
                if (res) {
                    config->SetPreSharedKey(password);
                }
                else {
                    StringBuilder builder;
                    builder += '"';
                    builder += password;
                    builder += '"';
                    config->SetPreSharedKey(builder.ToString());
                }
            }
            break;
        }

        case AccessPoint::SECURITY_EAP: {
            allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::WPA_EAP);
            allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::IEEE8021X);

            AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
            CWifiEnterpriseConfig::New((IWifiEnterpriseConfig**)&enterpriseConfig);
            config->SetEnterpriseConfig(enterpriseConfig);
            Int32 eapMethod;
            IAdapterView::Probe(mEapMethodSpinner)->GetSelectedItemPosition(&eapMethod);
            Int32 phase2Method;
            IAdapterView::Probe(mPhase2Spinner)->GetSelectedItemPosition(&phase2Method);
            enterpriseConfig->SetEapMethod(eapMethod);
            switch (eapMethod) {
                case IWifiEnterpriseConfigEap::PEAP:
                    // PEAP supports limited phase2 values
                    // Map the index from the PHASE2_PEAP_ADAPTER to the one used
                    // by the API which has the full list of PEAP methods.
                    switch(phase2Method) {
                        case WIFI_PEAP_PHASE2_NONE:
                            enterpriseConfig->SetPhase2Method(IWifiEnterpriseConfigPhase2::NONE);
                            break;
                        case WIFI_PEAP_PHASE2_MSCHAPV2:
                            enterpriseConfig->SetPhase2Method(IWifiEnterpriseConfigPhase2::MSCHAPV2);
                            break;
                        case WIFI_PEAP_PHASE2_GTC:
                            enterpriseConfig->SetPhase2Method(IWifiEnterpriseConfigPhase2::GTC);
                            break;
                        default:
                            Logger::E(TAG, "Unknown phase2 method" + phase2Method);
                            break;
                    }
                    break;
                default:
                    // The default index from PHASE2_FULL_ADAPTER maps to the API
                    enterpriseConfig->SetPhase2Method(phase2Method);
                    break;
            }

            AutoPtr<IInterface> obj;
            IAdapterView::Probe(mEapCaCertSpinner)->GetSelectedItem((IInterface**)&obj);
            String caCert = TO_STR(obj);
            if (caCert.Equals(mUnspecifiedCert)) caCert = "";
            enterpriseConfig->SetCaCertificateAlias(caCert);
            obj = NULL;
            IAdapterView::Probe(mEapUserCertSpinner)->GetSelectedItem((IInterface**)&obj);
            String clientCert = TO_STR(obj);
            if (clientCert.Equals(mUnspecifiedCert)) clientCert = "";
            enterpriseConfig->SetClientCertificateAlias(clientCert);
            AutoPtr<ICharSequence> seq;
            mEapIdentityView->GetText((ICharSequence**)&seq);
            enterpriseConfig->SetIdentity(TO_STR(seq));
            seq = NULL;
            mEapAnonymousView->GetText((ICharSequence**)&seq);
            enterpriseConfig->SetAnonymousIdentity(TO_STR(seq));

            Boolean res;
            if (IView::Probe(mPasswordView)->IsShown(&res), res) {
                // For security reasons, a previous password is not displayed to user.
                // Update only if it has been changed.
                Int32 length;
                if ((mPasswordView->GetLength(&length), length) > 0) {
                    seq = NULL;
                    mPasswordView->GetText((ICharSequence**)&seq);
                    enterpriseConfig->SetPassword(TO_STR(seq));
                }
            }
            else {
                // clear password
                seq = NULL;
                mPasswordView->GetText((ICharSequence**)&seq);
                enterpriseConfig->SetPassword(TO_STR(seq));
            }
            break;
        }
        default:
            return NULL;
    }

    AutoPtr<IIpConfiguration> ipc;
    CIpConfiguration::New(mIpAssignment, mProxySettings,
            mStaticIpConfiguration, mHttpProxy, (IIpConfiguration**)&ipc);
    config->SetIpConfiguration(ipc);

    return config;
}

Boolean WifiConfigController::IpAndProxyFieldsAreValid()
{
    mIpAssignment = DHCP_IpAssignment;
    if (mIpSettingsSpinner != NULL) {
        Int32 position;
        IAdapterView::Probe(mIpSettingsSpinner)->GetSelectedItemPosition(&position);
        if (position == STATIC_IP) {
            mIpAssignment = STATIC_IpAssignment;
        }
    }

    if (mIpAssignment == STATIC_IpAssignment) {
        CStaticIpConfiguration::New((IStaticIpConfiguration**)&mStaticIpConfiguration);
        Int32 result = ValidateIpConfigFields(mStaticIpConfiguration);
        if (result != 0) {
            return FALSE;
        }
    }

    Int32 selectedPosition;
    IAdapterView::Probe(mProxySettingsSpinner)->GetSelectedItemPosition(&selectedPosition);
    mProxySettings = NONE_ProxySettings;
    mHttpProxy = NULL;
    if (selectedPosition == PROXY_STATIC && mProxyHostView != NULL) {
        mProxySettings = STATIC_ProxySettings;
        AutoPtr<ICharSequence> seq;
        mProxyHostView->GetText((ICharSequence**)&seq);
        String host = TO_STR(seq);
        seq = NULL;
        mProxyPortView->GetText((ICharSequence**)&seq);
        String portStr = TO_STR(seq);
        seq = NULL;
        mProxyExclusionListView->GetText((ICharSequence**)&seq);
        String exclusionList = TO_STR(seq);
        Int32 port = 0;
        Int32 result = 0;
        // try {
        port = StringUtils::ParseInt32(portStr);
        result = ProxySelector::Validate(host, portStr, exclusionList);
        // } catch (NumberFormatException e) {
        //     result = R::string::proxy_error_invalid_port;
        // }
        if (result == 0) {
            CProxyInfo::New(host, port, exclusionList, (IProxyInfo**)&mHttpProxy);
        }
        else {
            return FALSE;
        }
    }
    else if (selectedPosition == PROXY_PAC && mProxyPacView != NULL) {
        mProxySettings = PAC_ProxySettings;
        AutoPtr<ICharSequence> uriSequence;
        mProxyPacView->GetText((ICharSequence**)&uriSequence);
        if (TextUtils::IsEmpty(uriSequence)) {
            return FALSE;
        }
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(TO_STR(uriSequence), (IUri**)&uri);
        if (uri == NULL) {
            return FALSE;
        }
        CProxyInfo::New(uri, (IProxyInfo**)&mHttpProxy);
    }
    return TRUE;
}

ECode WifiConfigController::GetIPv4Address(
    /* [in] */ const String& text,
    /* [out] */ IInet4Address** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // try {
    AutoPtr<IInetAddress> ress;
    ECode ec = NetworkUtils::NumericToInetAddress(text, (IInetAddress**)&ress);
    if (SUCCEEDED(ec)) {
        *result = IInet4Address::Probe(ress);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
    // } catch (IllegalArgumentException|ClassCastException e) {
    //     return NULL;
    // }
}

Int32 WifiConfigController::ValidateIpConfigFields(
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration)
{
    if (mIpAddressView == NULL) return 0;

    AutoPtr<ICharSequence> seq;
    mIpAddressView->GetText((ICharSequence**)&seq);
    String ipAddr = TO_STR(seq);
    if (TextUtils::IsEmpty(ipAddr)) return R::string::wifi_ip_settings_invalid_ip_address;

    AutoPtr<IInet4Address> inetAddr;
    GetIPv4Address(ipAddr, (IInet4Address**)&inetAddr);
    if (inetAddr == NULL) {
        return R::string::wifi_ip_settings_invalid_ip_address;
    }

    Int32 networkPrefixLength = -1;
    seq = NULL;
    mNetworkPrefixLengthView->GetText((ICharSequence**)&seq);
    // try {
    networkPrefixLength = StringUtils::ParseInt32(TO_STR(seq));
    if (networkPrefixLength < 0 || networkPrefixLength > 32) {
        return R::string::wifi_ip_settings_invalid_network_prefix_length;
    }
    AutoPtr<ILinkAddress> address;
    ECode ec = CLinkAddress::New(IInetAddress::Probe(inetAddr),
            networkPrefixLength, (ILinkAddress**)&address);
    if (SUCCEEDED(ec)) {
        staticIpConfiguration->SetIpAddress(address);
    }
    else {
        // } catch (NumberFormatException e) {
        // Set the hint as default after user types in ip address
        AutoPtr<IContext> context;
        mConfigUi->GetContext((IContext**)&context);
        String str;
        context->GetString(
                R::string::wifi_network_prefix_length_hint, &str);
        mNetworkPrefixLengthView->SetText(CoreUtils::Convert(str));
        // }
    }

    seq = NULL;
    mGatewayView->GetText((ICharSequence**)&seq);
    String gateway = TO_STR(seq);
    if (TextUtils::IsEmpty(gateway)) {
        // try {
        //Extract a default gateway from IP address
        AutoPtr<IInetAddress> netPart;
        NetworkUtils::GetNetworkPart(IInetAddress::Probe(inetAddr),
                networkPrefixLength, (IInetAddress**)&netPart);
        AutoPtr< ArrayOf<Byte> > addr;
        netPart->GetAddress((ArrayOf<Byte>**)&addr);
        (*addr)[addr->GetLength() - 1] = 1;

        AutoPtr<IInetAddressHelper> inetaddresshelper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetaddresshelper);
        AutoPtr<IInetAddress> net;
        inetaddresshelper->GetByAddress(addr, (IInetAddress**)&net);
        String str;
        net->GetHostAddress(&str);
        mGatewayView->SetText(CoreUtils::Convert(str));
        // } catch (RuntimeException ee) {
        // } catch (java.net.UnknownHostException u) {
        // }
    }
    else {
        AutoPtr<IInet4Address> inet4Address;
        GetIPv4Address(gateway, (IInet4Address**)&inet4Address);
        AutoPtr<IInetAddress> gatewayAddr = IInetAddress::Probe(inet4Address);
        if (gatewayAddr == NULL) {
            return R::string::wifi_ip_settings_invalid_gateway;
        }
        staticIpConfiguration->SetGateway(gatewayAddr);
    }

    seq = NULL;
    mDns1View->GetText((ICharSequence**)&seq);
    String dns = TO_STR(seq);
    AutoPtr<IInetAddress> dnsAddr;

    if (TextUtils::IsEmpty(dns)) {
        //If everything else is valid, provide hint as a default option
        AutoPtr<IContext> context;
        mConfigUi->GetContext((IContext**)&context);
        String str;
        context->GetString(R::string::wifi_dns1_hint, &str);
        mDns1View->SetText(CoreUtils::Convert(str));
    }
    else {
        AutoPtr<IInet4Address> inet4Address;
        GetIPv4Address(dns, (IInet4Address**)&inet4Address);
        dnsAddr = IInetAddress::Probe(inet4Address);
        if (dnsAddr == NULL) {
            return R::string::wifi_ip_settings_invalid_dns;
        }
        AutoPtr<IArrayList> dnsServers;
        staticIpConfiguration->GetDnsServers((IArrayList**)&dnsServers);
        dnsServers->Add(dnsAddr);
    }

    Int32 length;
    if ((mDns2View->GetLength(&length), length) > 0) {
        seq = NULL;
        mDns2View->GetText((ICharSequence**)&seq);
        dns = TO_STR(seq);
        AutoPtr<IInet4Address> inet4Address;
        GetIPv4Address(dns, (IInet4Address**)&inet4Address);
        dnsAddr = IInetAddress::Probe(inet4Address);
        if (dnsAddr == NULL) {
            return R::string::wifi_ip_settings_invalid_dns;
        }

        AutoPtr<IArrayList> dnsServers;
        staticIpConfiguration->GetDnsServers((IArrayList**)&dnsServers);
        dnsServers->Add(dnsAddr);
    }
    return 0;
}

void WifiConfigController::ShowSecurityFields()
{
    AutoPtr<IView> view;
    if (mInXlSetupWizard) {
        // Note: XL SetupWizard won't hide "EAP" settings here.
        AutoPtr<IContext> context;
        mConfigUi->GetContext((IContext**)&context);
        if (!((WifiSettingsForSetupWizardXL*)context.Get())->InitSecurityFields(mView,
                    mAccessPointSecurity)) {
            return;
        }
    }
    if (mAccessPointSecurity == AccessPoint::SECURITY_NONE) {
        view = NULL;
        mView->FindViewById(R::id::security_fields, (IView**)&view);
        view->SetVisibility(IView::GONE);
        return;
    }
    view = NULL;
    mView->FindViewById(R::id::security_fields, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);

    if (mPasswordView == NULL) {
        view = NULL;
        mView->FindViewById(R::id::password, (IView**)&view);
        mPasswordView = ITextView::Probe(view);
        mPasswordView->AddTextChangedListener((ITextWatcher*)this);
        view = NULL;
        mView->FindViewById(R::id::show_password, (IView**)&view);
        ICompoundButton::Probe(view)->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)this);

        if (mAccessPoint != NULL && mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
            mPasswordView->SetHint(R::string::wifi_unchanged);
        }
    }

    if (mAccessPointSecurity != AccessPoint::SECURITY_EAP) {
        view = NULL;
        mView->FindViewById(R::id::eap, (IView**)&view);
        view->SetVisibility(IView::GONE);
        return;
    }
    view = NULL;
    mView->FindViewById(R::id::eap, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);

    if (mEapMethodSpinner == NULL) {
        view = NULL;
        mView->FindViewById(R::id::method, (IView**)&view);
        mEapMethodSpinner = ISpinner::Probe(view);
        IAdapterView::Probe(mEapMethodSpinner)->SetOnItemSelectedListener((IAdapterViewOnItemSelectedListener*)this);
        view = NULL;
        mView->FindViewById(R::id::phase2, (IView**)&view);
        mPhase2Spinner = ISpinner::Probe(view);
        view = NULL;
        mView->FindViewById(R::id::ca_cert, (IView**)&view);
        mEapCaCertSpinner = ISpinner::Probe(view);
        view = NULL;
        mView->FindViewById(R::id::user_cert, (IView**)&view);
        mEapUserCertSpinner = ISpinner::Probe(view);
        view = NULL;
        mView->FindViewById(R::id::identity, (IView**)&view);
        mEapIdentityView = ITextView::Probe(view);
        view = NULL;
        mView->FindViewById(R::id::anonymous, (IView**)&view);
        mEapAnonymousView = ITextView::Probe(view);

        LoadCertificates(mEapCaCertSpinner, ICredentials::CA_CERTIFICATE);
        LoadCertificates(mEapUserCertSpinner, ICredentials::USER_PRIVATE_KEY);

        // Modifying an existing network
        if (mAccessPoint != NULL && mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
            AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
            mAccessPoint->GetConfig()->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
            Int32 eapMethod;
            enterpriseConfig->GetEapMethod(&eapMethod);
            Int32 phase2Method;
            enterpriseConfig->GetPhase2Method(&phase2Method);
            IAdapterView::Probe(mEapMethodSpinner)->SetSelection(eapMethod);
            ShowEapFieldsByMethod(eapMethod);
            switch (eapMethod) {
                case IWifiEnterpriseConfigEap::PEAP:
                    switch (phase2Method) {
                        case IWifiEnterpriseConfigPhase2::NONE:
                            IAdapterView::Probe(mPhase2Spinner)->SetSelection(WIFI_PEAP_PHASE2_NONE);
                            break;
                        case IWifiEnterpriseConfigPhase2::MSCHAPV2:
                            IAdapterView::Probe(mPhase2Spinner)->SetSelection(WIFI_PEAP_PHASE2_MSCHAPV2);
                            break;
                        case IWifiEnterpriseConfigPhase2::GTC:
                            IAdapterView::Probe(mPhase2Spinner)->SetSelection(WIFI_PEAP_PHASE2_GTC);
                            break;
                        default:
                            Logger::E(TAG, "Invalid phase 2 method %d", phase2Method);
                            break;
                    }
                    break;
                default:
                    IAdapterView::Probe(mPhase2Spinner)->SetSelection(phase2Method);
                    break;
            }
            String str;
            enterpriseConfig->GetCaCertificateAlias(&str);
            SetSelection(mEapCaCertSpinner, str);
            enterpriseConfig->GetClientCertificateAlias(&str);
            SetSelection(mEapUserCertSpinner, str);
            enterpriseConfig->GetIdentity(&str);
            mEapIdentityView->SetText(CoreUtils::Convert(str));
            enterpriseConfig->GetAnonymousIdentity(&str);
            mEapAnonymousView->SetText(CoreUtils::Convert(str));
        }
        else {
            // Choose a default for a new network and show only appropriate
            // fields
            IAdapterView::Probe(mEapMethodSpinner)->SetSelection(IWifiEnterpriseConfigEap::PEAP);
            ShowEapFieldsByMethod(IWifiEnterpriseConfigEap::PEAP);
        }
    }
    else {
        Int32 position;
        IAdapterView::Probe(mEapMethodSpinner)->GetSelectedItemPosition(&position);
        ShowEapFieldsByMethod(position);
    }
}

void WifiConfigController::ShowEapFieldsByMethod(
    /* [in] */ Int32 eapMethod)
{
    // Common defaults
    AutoPtr<IView> view;
    mView->FindViewById(R::id::l_method, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);
    view = NULL;
    mView->FindViewById(R::id::l_identity, (IView**)&view);view->SetVisibility(IView::VISIBLE);

    // Defaults for most of the EAP methods and over-riden by
    // by certain EAP methods
    view = NULL;
    mView->FindViewById(R::id::l_ca_cert, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);
    view = NULL;
    mView->FindViewById(R::id::password_layout, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);
    view = NULL;
    mView->FindViewById(R::id::show_password_layout, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);

    AutoPtr<IContext> context;
    mConfigUi->GetContext((IContext**)&context);
    switch (eapMethod) {
        case WIFI_EAP_METHOD_PWD:
            SetPhase2Invisible();
            SetCaCertInvisible();
            SetAnonymousIdentInvisible();
            SetUserCertInvisible();
            break;
        case WIFI_EAP_METHOD_TLS:
            view = NULL;
            mView->FindViewById(R::id::l_user_cert, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            SetPhase2Invisible();
            SetAnonymousIdentInvisible();
            SetPasswordInvisible();
            break;
        case WIFI_EAP_METHOD_PEAP:
            // Reset adapter if needed
            if (mPhase2Adapter != PHASE2_PEAP_ADAPTER) {
                mPhase2Adapter = PHASE2_PEAP_ADAPTER;
                IAdapterView::Probe(mPhase2Spinner)->SetAdapter(IAdapter::Probe(mPhase2Adapter));
            }
            view = NULL;
            mView->FindViewById(R::id::l_phase2, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            view = NULL;
            mView->FindViewById(R::id::l_anonymous, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            SetUserCertInvisible();
            break;
        case WIFI_EAP_METHOD_TTLS:
            // Reset adapter if needed
            if (mPhase2Adapter != PHASE2_FULL_ADAPTER) {
                mPhase2Adapter = PHASE2_FULL_ADAPTER;
                IAdapterView::Probe(mPhase2Spinner)->SetAdapter(IAdapter::Probe(mPhase2Adapter));
            }
            view = NULL;
            mView->FindViewById(R::id::l_phase2, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            view = NULL;
            mView->FindViewById(R::id::l_anonymous, (IView**)&view);
            view->SetVisibility(IView::VISIBLE);
            SetUserCertInvisible();
            break;
    }
}

void WifiConfigController::SetPhase2Invisible()
{
    AutoPtr<IView> view;
    mView->FindViewById(R::id::l_phase2, (IView**)&view);
    view->SetVisibility(IView::GONE);
    IAdapterView::Probe(mPhase2Spinner)->SetSelection(IWifiEnterpriseConfigPhase2::NONE);
}

void WifiConfigController::SetCaCertInvisible()
{
    AutoPtr<IView> view;
    mView->FindViewById(R::id::l_ca_cert, (IView**)&view);
    view->SetVisibility(IView::GONE);
    IAdapterView::Probe(mEapCaCertSpinner)->SetSelection(unspecifiedCertIndex);
}

void WifiConfigController::SetUserCertInvisible()
{
    AutoPtr<IView> view;
    mView->FindViewById(R::id::l_user_cert, (IView**)&view);
    view->SetVisibility(IView::GONE);
    IAdapterView::Probe(mEapUserCertSpinner)->SetSelection(unspecifiedCertIndex);
}

void WifiConfigController::SetAnonymousIdentInvisible()
{
    AutoPtr<IView> view;
    mView->FindViewById(R::id::l_anonymous, (IView**)&view);
    view->SetVisibility(IView::GONE);
    mEapAnonymousView->SetText(CoreUtils::Convert(""));
}

void WifiConfigController::SetPasswordInvisible()
{
    mPasswordView->SetText(CoreUtils::Convert(""));
    AutoPtr<IView> view;
    mView->FindViewById(R::id::password_layout, (IView**)&view);
    view->SetVisibility(IView::GONE);
    view = NULL;
    mView->FindViewById(R::id::show_password_layout, (IView**)&view);
    view->SetVisibility(IView::GONE);
}

void WifiConfigController::ShowIpConfigFields()
{
    AutoPtr<IWifiConfiguration> config;

    AutoPtr<IView> view;
    mView->FindViewById(R::id::ip_fields, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);

    if (mAccessPoint != NULL && mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        config = mAccessPoint->GetConfig();
    }

    Int32 position;
    IAdapterView::Probe(mIpSettingsSpinner)->GetSelectedItemPosition(&position);
    if (position == STATIC_IP) {
        view = NULL;
        mView->FindViewById(R::id::staticip, (IView**)&view);
        view->SetVisibility(IView::VISIBLE);
        if (mIpAddressView == NULL) {
            view = NULL;
            mView->FindViewById(R::id::ipaddress, (IView**)&view);
            mIpAddressView = ITextView::Probe(view);
            mIpAddressView->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(R::id::gateway, (IView**)&view);
            mGatewayView = ITextView::Probe(view);
            mGatewayView->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(
                    R::id::network_prefix_length, (IView**)&view);
            mNetworkPrefixLengthView = ITextView::Probe(view);
            mNetworkPrefixLengthView->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(R::id::dns1, (IView**)&view);
            mDns1View = ITextView::Probe(view);
            mDns1View->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(R::id::dns2, (IView**)&view);
            mDns2View = ITextView::Probe(view);
            mDns2View->AddTextChangedListener((ITextWatcher*)this);
        }
        if (config != NULL) {
            AutoPtr<IStaticIpConfiguration> staticConfig;
            config->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticConfig);
            if (staticConfig != NULL) {
                AutoPtr<ILinkAddress> ipAddress;
                staticConfig->GetIpAddress((ILinkAddress**)&ipAddress);
                if (ipAddress != NULL) {
                    AutoPtr<IInetAddress> address;
                    ipAddress->GetAddress((IInetAddress**)&address);
                    String hostAddress;
                    address->GetHostAddress(&hostAddress);
                    mIpAddressView->SetText(CoreUtils::Convert(hostAddress));
                    Int32 result;
                    ipAddress->GetNetworkPrefixLength(&result);
                    mNetworkPrefixLengthView->SetText(CoreUtils::Convert(StringUtils::ToString(result)));
                }

                AutoPtr<IInetAddress> gateway;
                staticConfig->GetGateway((IInetAddress**)&gateway);
                if (gateway != NULL) {
                    String hostAddress;
                    gateway->GetHostAddress(&hostAddress);
                    mGatewayView->SetText(CoreUtils::Convert(hostAddress));
                }

                AutoPtr<IArrayList> dnsServers;
                staticConfig->GetDnsServers((IArrayList**)&dnsServers);
                AutoPtr<IIterator> dnsIterator;
                dnsServers->GetIterator((IIterator**)&dnsIterator);
                // Iterator<InetAddress>  = staticConfig.dnsServers->Iterator();
                Boolean res;
                if (dnsIterator->HasNext(&res), res) {
                    AutoPtr<IInterface> obj;
                    dnsIterator->GetNext((IInterface**)&obj);
                    String hostAddress;
                    IInetAddress::Probe(obj)->GetHostAddress(&hostAddress);
                    mDns1View->SetText(CoreUtils::Convert(hostAddress));
                }
                if (dnsIterator->HasNext(&res), res) {
                    AutoPtr<IInterface> obj;
                    dnsIterator->GetNext((IInterface**)&obj);
                    String hostAddress;
                    IInetAddress::Probe(obj)->GetHostAddress(&hostAddress);
                    mDns2View->SetText(CoreUtils::Convert(hostAddress));
                }
            }
        }
    }
    else {
        view = NULL;
        mView->FindViewById(R::id::staticip, (IView**)&view);
        view->SetVisibility(IView::GONE);
    }
}

void WifiConfigController::ShowProxyFields()
{
    AutoPtr<IWifiConfiguration> config;

    AutoPtr<IView> view;
    mView->FindViewById(R::id::proxy_settings_fields, (IView**)&view);
    view->SetVisibility(IView::VISIBLE);

    if (mAccessPoint != NULL && mAccessPoint->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        config = mAccessPoint->GetConfig();
    }

    Int32 position;
    IAdapterView::Probe(mProxySettingsSpinner)->GetSelectedItemPosition(&position);
    if (position == PROXY_STATIC) {
        SetVisibility(R::id::proxy_warning_limited_support, IView::VISIBLE);
        SetVisibility(R::id::proxy_fields, IView::VISIBLE);
        SetVisibility(R::id::proxy_pac_field, IView::GONE);
        if (mProxyHostView == NULL) {
            view = NULL;
            mView->FindViewById(R::id::proxy_hostname, (IView**)&view);
            mProxyHostView = ITextView::Probe(view);
            mProxyHostView->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(R::id::proxy_port, (IView**)&view);
            mProxyPortView = ITextView::Probe(view);
            mProxyPortView->AddTextChangedListener((ITextWatcher*)this);
            view = NULL;
            mView->FindViewById(R::id::proxy_exclusionlist, (IView**)&view);
            mProxyExclusionListView = ITextView::Probe(view);
            mProxyExclusionListView->AddTextChangedListener((ITextWatcher*)this);
        }
        if (config != NULL) {
            AutoPtr<IProxyInfo> proxyProperties;
            config->GetHttpProxy((IProxyInfo**)&proxyProperties);
            if (proxyProperties != NULL) {
                String host;
                proxyProperties->GetHost(&host);
                mProxyHostView->SetText(CoreUtils::Convert(host));
                Int32 result;
                proxyProperties->GetPort(&result);
                mProxyPortView->SetText(CoreUtils::Convert(StringUtils::ToString(result)));
                String str;
                proxyProperties->GetExclusionListAsString(&str);
                mProxyExclusionListView->SetText(CoreUtils::Convert(str));
            }
        }
    }
    else if (position == PROXY_PAC) {
        SetVisibility(R::id::proxy_warning_limited_support, IView::GONE);
        SetVisibility(R::id::proxy_fields, IView::GONE);
        SetVisibility(R::id::proxy_pac_field, IView::VISIBLE);

        if (mProxyPacView == NULL) {
            view = NULL;
            mView->FindViewById(R::id::proxy_pac, (IView**)&view);
            mProxyPacView = ITextView::Probe(view);
            mProxyPacView->AddTextChangedListener((ITextWatcher*)this);
        }
        if (config != NULL) {
            AutoPtr<IProxyInfo> proxyInfo;
            config->GetHttpProxy((IProxyInfo**)&proxyInfo);
            if (proxyInfo != NULL) {
                AutoPtr<IUri> uri;
                proxyInfo->GetPacFileUrl((IUri**)&uri);
                mProxyPacView->SetText(CoreUtils::Convert(TO_STR(uri)));
            }
        }
    }
    else {
        SetVisibility(R::id::proxy_warning_limited_support, IView::GONE);
        SetVisibility(R::id::proxy_fields, IView::GONE);
        SetVisibility(R::id::proxy_pac_field, IView::GONE);
    }
}

void WifiConfigController::SetVisibility(
    /* [in] */ Int32 id,
    /* [in] */ Int32 visibility)
{
    AutoPtr<IView> v;
    mView->FindViewById(id, (IView**)&v);
    if (v != NULL) {
        v->SetVisibility(visibility);
    }
}

void WifiConfigController::LoadCertificates(
    /* [in] */ ISpinner* spinner,
    /* [in] */ const String& prefix)
{
    AutoPtr<IContext> context;
    mConfigUi->GetContext((IContext**)&context);

    assert(0 && "TODO");
    // AutoPtr<IKeyStoreHelper> helper;
    // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    AutoPtr<IKeyStore> keyStore;
    // helper->GetInstance((IKeyStore**)&keyStore);
    AutoPtr< ArrayOf<String> > certs;
    keyStore->Saw(prefix, IProcess::WIFI_UID, (ArrayOf<String>**)&certs);
    if (certs == NULL || certs->GetLength() == 0) {
        certs = ArrayOf<String>::Alloc(1);
        (*certs)[0] = mUnspecifiedCert;
    }
    else {
        AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(certs->GetLength() + 1);
        (*array)[0] = mUnspecifiedCert;
        array->Copy(1, certs, 0, certs->GetLength());
        certs = array;
    }

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(certs->GetLength());
    for (Int32 i = 0; i < certs->GetLength(); i++) {
        (*args)[i] = CoreUtils::Convert((*certs)[i]);
    }
    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(context, Elastos::Droid::R::layout::simple_spinner_item,
            args, (IArrayAdapter**)&adapter);
    adapter->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);
    IAdapterView::Probe(spinner)->SetAdapter(IAdapter::Probe(adapter));
}

void WifiConfigController::SetSelection(
    /* [in] */ ISpinner* spinner,
    /* [in] */ const String& value)
{
    if (!value.IsNull()) {
        // @SuppressWarnings("unchecked")
        AutoPtr<IAdapter> adap;
        IAdapterView::Probe(spinner)->GetAdapter((IAdapter**)&adap);
        AutoPtr<IArrayAdapter> adapter = IArrayAdapter::Probe(adap);
        Int32 count;
        IAdapter::Probe(adapter)->GetCount(&count);
        for (Int32 i = count - 1; i >= 0; --i) {
            AutoPtr<IInterface> obj;
            IAdapter::Probe(adapter)->GetItem(i, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            if (value.Equals(str)) {
                IAdapterView::Probe(spinner)->SetSelection(i);
                break;
            }
        }
    }
}

Boolean WifiConfigController::IsEdit()
{
    return mEdit;
}

ECode WifiConfigController::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    AutoPtr<AfterTextChangedRunnable> runnable = new AfterTextChangedRunnable(this);
    Boolean res;
    mTextViewChangedHandler->Post((IRunnable*)runnable, &res);
    return NOERROR;
}

ECode WifiConfigController::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    // work done in afterTextChanged
    return NOERROR;
}

ECode WifiConfigController::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    // work done in afterTextChanged
    return NOERROR;
}

ECode WifiConfigController::OnCheckedChanged(
    /* [in] */ ICompoundButton* view,
    /* [in] */ Boolean isChecked)
{
    Int32 id;
    IView::Probe(view)->GetId(&id);
    if (id == R::id::show_password) {
        Int32 pos;
        mPasswordView->GetSelectionEnd(&pos);
        mPasswordView->SetInputType(
                IInputType::TYPE_CLASS_TEXT | (isChecked ?
                        IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                            IInputType::TYPE_TEXT_VARIATION_PASSWORD));
        if (pos >= 0) {
            IEditText::Probe(mPasswordView)->SetSelection(pos);
        }
    }
    else if (id == R::id::wifi_advanced_togglebox) {
        if (isChecked) {
            AutoPtr<IView> viewBy;
            mView->FindViewById(R::id::wifi_advanced_fields, (IView**)&viewBy);
            viewBy->SetVisibility(IView::VISIBLE);
        }
        else {
            AutoPtr<IView> viewBy;
            mView->FindViewById(R::id::wifi_advanced_fields, (IView**)&viewBy);
            viewBy->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

ECode WifiConfigController::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (parent == IAdapterView::Probe(mSecuritySpinner)) {
        mAccessPointSecurity = position;
        ShowSecurityFields();
    }
    else if (parent == IAdapterView::Probe(mEapMethodSpinner)) {
        ShowSecurityFields();
    }
    else if (parent == IAdapterView::Probe(mProxySettingsSpinner)) {
        ShowProxyFields();
    }
    else {
        ShowIpConfigFields();
    }
    EnableSubmitIfAppropriate();
    return NOERROR;
}

ECode WifiConfigController::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    //
    return NOERROR;
}

void WifiConfigController::UpdatePasswordVisibility(
    /* [in] */ Boolean checked)
{
    Int32 pos;
    mPasswordView->GetSelectionEnd(&pos);
    mPasswordView->SetInputType(
            IInputType::TYPE_CLASS_TEXT | (checked ?
                    IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                        IInputType::TYPE_TEXT_VARIATION_PASSWORD));
    if (pos >= 0) {
        IEditText::Probe(mPasswordView)->SetSelection(pos);
    }
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
