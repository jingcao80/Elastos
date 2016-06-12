#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGCONTROLLER_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGCONTROLLER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::IpConfigurationIpAssignment;
using Elastos::Droid::Net::IpConfigurationProxySettings;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Net::IInet4Address;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * The class for allowing UIs like {@link WifiDialog} and {@link WifiConfigUiBase} to
 * share the logic for controlling buttons, text fields, etc.
 */
class WifiConfigController
    : public Object
    , public IWifiConfigController
    , public ITextWatcher
    , public INoCopySpan
    , public IAdapterViewOnItemSelectedListener
    , public ICompoundButtonOnCheckedChangeListener
{
private:
    class AfterTextChangedRunnable
        : public Runnable
    {
    public:
        AfterTextChangedRunnable(
            /* [in] */ WifiConfigController* host);

        ~AfterTextChangedRunnable();

        CARAPI Run();

    private:
        WifiConfigController* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    WifiConfigController(
        /* [in] */ IWifiConfigUiBase* parent,
        /* [in] */ IView* view,
        /* [in] */ IAccessPoint* accessPoint,
        /* [in] */ Boolean edit);

    CARAPI_(void) HideSubmitButton();

    /* show submit button if password, ip and proxy settings are valid */
    CARAPI_(void) EnableSubmitIfAppropriate();

    /* package */ CARAPI_(AutoPtr<IWifiConfiguration>) GetConfig();

    CARAPI_(Boolean) IsEdit();

    // @Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    // @Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    // @Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    // @Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* view,
        /* [in] */ Boolean isChecked);

    // @Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // @Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

private:
    CARAPI_(void) AddRow(
        /* [in] */ IViewGroup* group,
        /* [in] */ Int32 name,
        /* [in] */ const String& value);

    CARAPI_(String) GetSignalString();

    CARAPI_(Boolean) IpAndProxyFieldsAreValid();

    CARAPI GetIPv4Address(
        /* [in] */ const String& text,
        /* [out] */ IInet4Address** result);

    CARAPI_(Int32) ValidateIpConfigFields(
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration);

    CARAPI_(void) ShowSecurityFields();

    /**
     * EAP-PWD valid fields include
     *   identity
     *   password
     * EAP-PEAP valid fields include
     *   phase2: MSCHAPV2, GTC
     *   ca_cert
     *   identity
     *   anonymous_identity
     *   password
     * EAP-TLS valid fields include
     *   user_cert
     *   ca_cert
     *   identity
     * EAP-TTLS valid fields include
     *   phase2: PAP, MSCHAP, MSCHAPV2, GTC
     *   ca_cert
     *   identity
     *   anonymous_identity
     *   password
     */
    CARAPI_(void) ShowEapFieldsByMethod(
        /* [in] */ Int32 eapMethod);

    CARAPI_(void) SetPhase2Invisible();

    CARAPI_(void) SetCaCertInvisible();

    CARAPI_(void) SetUserCertInvisible();

    CARAPI_(void) SetAnonymousIdentInvisible();

    CARAPI_(void) SetPasswordInvisible();

    CARAPI_(void) ShowIpConfigFields();

    CARAPI_(void) ShowProxyFields();

    CARAPI_(void) SetVisibility(
        /* [in] */ Int32 id,
        /* [in] */ Int32 visibility);

    CARAPI_(void) LoadCertificates(
        /* [in] */ ISpinner* spinner,
        /* [in] */ const String& prefix);

    CARAPI_(void) SetSelection(
        /* [in] */ ISpinner* spinner,
        /* [in] */ const String& value);

    /**
     * Make the characters of the password visible if show_password is checked.
     */
    CARAPI_(void) UpdatePasswordVisibility(
        /* [in] */ Boolean checked);

public:
    /* These values come from "wifi_proxy_settings" resource array */
    static const Int32 PROXY_NONE;
    static const Int32 PROXY_STATIC;
    static const Int32 PROXY_PAC;

    /* These values come from "wifi_eap_method" resource array */
    static const Int32 WIFI_EAP_METHOD_PEAP = 0;
    static const Int32 WIFI_EAP_METHOD_TLS  = 1;
    static const Int32 WIFI_EAP_METHOD_TTLS = 2;
    static const Int32 WIFI_EAP_METHOD_PWD  = 3;

    /* These values come from "wifi_peap_phase2_entries" resource array */
    static const Int32 WIFI_PEAP_PHASE2_NONE         = 0;
    static const Int32 WIFI_PEAP_PHASE2_MSCHAPV2     = 1;
    static const Int32 WIFI_PEAP_PHASE2_GTC        = 2;

private:
    static const String TAG;

    AutoPtr<IWifiConfigUiBase> mConfigUi;
    AutoPtr<IView> mView;
    AutoPtr<IAccessPoint> mAccessPoint;

    /* This value comes from "wifi_ip_settings" resource array */
    static const Int32 DHCP;
    static const Int32 STATIC_IP;

    /* Phase2 methods supported by PEAP are limited */
    /* ArrayAdapter<String> PHASE2_PEAP_ADAPTER;*/
    AutoPtr<IArrayAdapter> PHASE2_PEAP_ADAPTER;
    /* Full list of phase2 methods */
    /*ArrayAdapter<String> PHASE2_FULL_ADAPTER;*/
    AutoPtr<IArrayAdapter> PHASE2_FULL_ADAPTER;

    // True when this instance is used in SetupWizard XL context.
    Boolean mInXlSetupWizard;

    AutoPtr<IHandler> mTextViewChangedHandler;

    // e.g. AccessPoint.SECURITY_NONE
    Int32 mAccessPointSecurity;
    AutoPtr<ITextView> mPasswordView;

    String mUnspecifiedCert;
    static const Int32 unspecifiedCertIndex;

    AutoPtr<ISpinner> mSecuritySpinner;
    AutoPtr<ISpinner> mEapMethodSpinner;
    AutoPtr<ISpinner> mEapCaCertSpinner;
    AutoPtr<ISpinner> mPhase2Spinner;
    // Associated with mPhase2Spinner, one of PHASE2_FULL_ADAPTER or PHASE2_PEAP_ADAPTER
    /*ArrayAdapter<String> mPhase2Adapter;*/
    AutoPtr<IArrayAdapter> mPhase2Adapter;
    AutoPtr<ISpinner> mEapUserCertSpinner;
    AutoPtr<ITextView> mEapIdentityView;
    AutoPtr<ITextView> mEapAnonymousView;

    AutoPtr<ISpinner> mIpSettingsSpinner;
    AutoPtr<ITextView> mIpAddressView;
    AutoPtr<ITextView> mGatewayView;
    AutoPtr<ITextView> mNetworkPrefixLengthView;
    AutoPtr<ITextView> mDns1View;
    AutoPtr<ITextView> mDns2View;

    AutoPtr<ISpinner> mProxySettingsSpinner;
    AutoPtr<ITextView> mProxyHostView;
    AutoPtr<ITextView> mProxyPortView;
    AutoPtr<ITextView> mProxyExclusionListView;
    AutoPtr<ITextView> mProxyPacView;

    IpConfigurationIpAssignment mIpAssignment;
    IpConfigurationProxySettings mProxySettings;
    AutoPtr<IProxyInfo> mHttpProxy;
    AutoPtr<IStaticIpConfiguration> mStaticIpConfiguration;

    AutoPtr< ArrayOf<String> > mLevels;
    Boolean mEdit;
    AutoPtr<ITextView> mSsidView;

    AutoPtr<IContext> mContext;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFICONFIGCONTROLLER_H__
