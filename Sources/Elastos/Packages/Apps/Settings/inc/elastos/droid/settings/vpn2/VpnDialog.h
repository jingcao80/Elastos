//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

package com.android.settings.vpn2;

using Elastos::Droid::Internal::Net::IVpnProfile;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Security::ICredentials;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

using Elastos::Net::IInetAddress;

class VpnDialog extends AlertDialog implements TextWatcher,
        View.OnClickListener, AdapterView.OnItemSelectedListener {
    private final KeyStore mKeyStore = KeyStore->GetInstance();
    private final DialogInterface.OnClickListener mListener;
    private final VpnProfile mProfile;

    private Boolean mEditing;

    private View mView;

    private TextView mName;
    private Spinner mType;
    private TextView mServer;
    private TextView mUsername;
    private TextView mPassword;
    private TextView mSearchDomains;
    private TextView mDnsServers;
    private TextView mRoutes;
    private CheckBox mMppe;
    private TextView mL2tpSecret;
    private TextView mIpsecIdentifier;
    private TextView mIpsecSecret;
    private Spinner mIpsecUserCert;
    private Spinner mIpsecCaCert;
    private Spinner mIpsecServerCert;
    private CheckBox mSaveLogin;

    VpnDialog(Context context, DialogInterface.OnClickListener listener,
            VpnProfile profile, Boolean editing) {
        Super(context);
        mListener = listener;
        mProfile = profile;
        mEditing = editing;
    }

    //@Override
    protected void OnCreate(Bundle savedState) {
        mView = GetLayoutInflater()->Inflate(R.layout.vpn_dialog, NULL);
        SetView(mView);
        SetInverseBackgroundForced(TRUE);

        Context context = GetContext();

        // First, find out all the fields.
        mName = (TextView) mView->FindViewById(R.id.name);
        mType = (Spinner) mView->FindViewById(R.id.type);
        mServer = (TextView) mView->FindViewById(R.id.server);
        mUsername = (TextView) mView->FindViewById(R.id.username);
        mPassword = (TextView) mView->FindViewById(R.id.password);
        mSearchDomains = (TextView) mView->FindViewById(R.id.search_domains);
        mDnsServers = (TextView) mView->FindViewById(R.id.dns_servers);
        mRoutes = (TextView) mView->FindViewById(R.id.routes);
        mMppe = (CheckBox) mView->FindViewById(R.id.mppe);
        mL2tpSecret = (TextView) mView->FindViewById(R.id.l2tp_secret);
        mIpsecIdentifier = (TextView) mView->FindViewById(R.id.ipsec_identifier);
        mIpsecSecret = (TextView) mView->FindViewById(R.id.ipsec_secret);
        mIpsecUserCert = (Spinner) mView->FindViewById(R.id.ipsec_user_cert);
        mIpsecCaCert = (Spinner) mView->FindViewById(R.id.ipsec_ca_cert);
        mIpsecServerCert = (Spinner) mView->FindViewById(R.id.ipsec_server_cert);
        mSaveLogin = (CheckBox) mView->FindViewById(R.id.save_login);

        // Second, copy values from the profile.
        mName->SetText(mProfile.name);
        mType->SetSelection(mProfile.type);
        mServer->SetText(mProfile.server);
        if (mProfile.saveLogin) {
            mUsername->SetText(mProfile.username);
            mPassword->SetText(mProfile.password);
        }
        mSearchDomains->SetText(mProfile.searchDomains);
        mDnsServers->SetText(mProfile.dnsServers);
        mRoutes->SetText(mProfile.routes);
        mMppe->SetChecked(mProfile.mppe);
        mL2tpSecret->SetText(mProfile.l2tpSecret);
        mIpsecIdentifier->SetText(mProfile.ipsecIdentifier);
        mIpsecSecret->SetText(mProfile.ipsecSecret);
        LoadCertificates(mIpsecUserCert, Credentials.USER_PRIVATE_KEY,
                0, mProfile.ipsecUserCert);
        LoadCertificates(mIpsecCaCert, Credentials.CA_CERTIFICATE,
                R::string::vpn_no_ca_cert, mProfile.ipsecCaCert);
        LoadCertificates(mIpsecServerCert, Credentials.USER_CERTIFICATE,
                R::string::vpn_no_server_cert, mProfile.ipsecServerCert);
        mSaveLogin->SetChecked(mProfile.saveLogin);

        // Third, add listeners to required fields.
        mName->AddTextChangedListener(this);
        mType->SetOnItemSelectedListener(this);
        mServer->AddTextChangedListener(this);
        mUsername->AddTextChangedListener(this);
        mPassword->AddTextChangedListener(this);
        mDnsServers->AddTextChangedListener(this);
        mRoutes->AddTextChangedListener(this);
        mIpsecSecret->AddTextChangedListener(this);
        mIpsecUserCert->SetOnItemSelectedListener(this);

        // Forth, determine to do editing or connecting.
        Boolean valid = Validate(TRUE);
        mEditing = mEditing || !valid;

        if (mEditing) {
            SetTitle(R::string::vpn_edit);

            // Show common fields.
            mView->FindViewById(R.id.editor).SetVisibility(View.VISIBLE);

            // Show type-specific fields.
            ChangeType(mProfile.type);

            // Show advanced options directly if any of them is set.
            View showOptions = mView->FindViewById(R.id.show_options);
            if (mProfile.searchDomains->IsEmpty() && mProfile.dnsServers->IsEmpty() &&
                    mProfile.routes->IsEmpty()) {
                showOptions->SetOnClickListener(this);
            } else {
                OnClick(showOptions);
            }

            // Create a button to save the profile.
            SetButton(DialogInterface.BUTTON_POSITIVE,
                    context->GetString(R::string::vpn_save), mListener);
        } else {
            SetTitle(context->GetString(R::string::vpn_connect_to, mProfile.name));

            // Not editing, just show username and password.
            mView->FindViewById(R.id.login).SetVisibility(View.VISIBLE);

            // Create a button to connect the network.
            SetButton(DialogInterface.BUTTON_POSITIVE,
                    context->GetString(R::string::vpn_connect), mListener);
        }

        // Always provide a cancel button.
        SetButton(DialogInterface.BUTTON_NEGATIVE,
                context->GetString(R::string::vpn_cancel), mListener);

        // Let AlertDialog create everything.
        super->OnCreate(NULL);

        // Disable the action button if necessary.
        GetButton(DialogInterface.BUTTON_POSITIVE)
                .SetEnabled(mEditing ? valid : Validate(FALSE));

        // Workaround to resize the dialog for the input method.
        GetWindow()->SetSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE |
                WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);
    }

    //@Override
    CARAPI AfterTextChanged(Editable field) {
        GetButton(DialogInterface.BUTTON_POSITIVE).SetEnabled(Validate(mEditing));
    }

    //@Override
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
    }

    //@Override
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
    }

    //@Override
    CARAPI OnClick(View showOptions) {
        showOptions->SetVisibility(View.GONE);
        mView->FindViewById(R.id.options).SetVisibility(View.VISIBLE);
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        if (parent == mType) {
            ChangeType(position);
        }
        GetButton(DialogInterface.BUTTON_POSITIVE).SetEnabled(Validate(mEditing));
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
    }

    private void ChangeType(Int32 type) {
        // First, hide everything.
        mMppe->SetVisibility(View.GONE);
        mView->FindViewById(R.id.l2tp).SetVisibility(View.GONE);
        mView->FindViewById(R.id.ipsec_psk).SetVisibility(View.GONE);
        mView->FindViewById(R.id.ipsec_user).SetVisibility(View.GONE);
        mView->FindViewById(R.id.ipsec_peer).SetVisibility(View.GONE);

        // Then, unhide type-specific fields.
        switch (type) {
            case VpnProfile.TYPE_PPTP:
                mMppe->SetVisibility(View.VISIBLE);
                break;

            case VpnProfile.TYPE_L2TP_IPSEC_PSK:
                mView->FindViewById(R.id.l2tp).SetVisibility(View.VISIBLE);
                // fall through
            case VpnProfile.TYPE_IPSEC_XAUTH_PSK:
                mView->FindViewById(R.id.ipsec_psk).SetVisibility(View.VISIBLE);
                break;

            case VpnProfile.TYPE_L2TP_IPSEC_RSA:
                mView->FindViewById(R.id.l2tp).SetVisibility(View.VISIBLE);
                // fall through
            case VpnProfile.TYPE_IPSEC_XAUTH_RSA:
                mView->FindViewById(R.id.ipsec_user).SetVisibility(View.VISIBLE);
                // fall through
            case VpnProfile.TYPE_IPSEC_HYBRID_RSA:
                mView->FindViewById(R.id.ipsec_peer).SetVisibility(View.VISIBLE);
                break;
        }
    }

    private Boolean Validate(Boolean editing) {
        if (!editing) {
            return mUsername->GetText()->Length() != 0 && mPassword->GetText()->Length() != 0;
        }
        if (mName->GetText()->Length() == 0 || mServer->GetText()->Length() == 0 ||
                !ValidateAddresses(mDnsServers->GetText()->ToString(), FALSE) ||
                !ValidateAddresses(mRoutes->GetText()->ToString(), TRUE)) {
            return FALSE;
        }
        switch (mType->GetSelectedItemPosition()) {
            case VpnProfile.TYPE_PPTP:
            case VpnProfile.TYPE_IPSEC_HYBRID_RSA:
                return TRUE;

            case VpnProfile.TYPE_L2TP_IPSEC_PSK:
            case VpnProfile.TYPE_IPSEC_XAUTH_PSK:
                return mIpsecSecret->GetText()->Length() != 0;

            case VpnProfile.TYPE_L2TP_IPSEC_RSA:
            case VpnProfile.TYPE_IPSEC_XAUTH_RSA:
                return mIpsecUserCert->GetSelectedItemPosition() != 0;
        }
        return FALSE;
    }

    private Boolean ValidateAddresses(String addresses, Boolean cidr) {
        try {
            for (String address : addresses->Split(" ")) {
                if (address->IsEmpty()) {
                    continue;
                }
                // Legacy VPN currently only supports IPv4.
                Int32 prefixLength = 32;
                if (cidr) {
                    String[] parts = address->Split("/", 2);
                    address = parts[0];
                    prefixLength = Integer->ParseInt(parts[1]);
                }
                Byte[] bytes = InetAddress->ParseNumericAddress(address).GetAddress();
                Int32 integer = (bytes[3] & 0xFF) | (bytes[2] & 0xFF) << 8 |
                        (bytes[1] & 0xFF) << 16 | (bytes[0] & 0xFF) << 24;
                if (bytes.length != 4 || prefixLength < 0 || prefixLength > 32 ||
                        (prefixLength < 32 && (integer << prefixLength) != 0)) {
                    return FALSE;
                }
            }
        } catch (Exception e) {
            return FALSE;
        }
        return TRUE;
    }

    private void LoadCertificates(Spinner spinner, String prefix, Int32 firstId, String selected) {
        Context context = GetContext();
        String first = (firstId == 0) ? "" : context->GetString(firstId);
        String[] certificates = mKeyStore->Saw(prefix);

        if (certificates == NULL || certificates.length == 0) {
            certificates = new String[] {first};
        } else {
            String[] array = new String[certificates.length + 1];
            array[0] = first;
            System->Arraycopy(certificates, 0, array, 1, certificates.length);
            certificates = array;
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                context, android.R.layout.simple_spinner_item, certificates);
        adapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner->SetAdapter(adapter);

        for (Int32 i = 1; i < certificates.length; ++i) {
            if (certificates[i].Equals(selected)) {
                spinner->SetSelection(i);
                break;
            }
        }
    }

    Boolean IsEditing() {
        return mEditing;
    }

    VpnProfile GetProfile() {
        // First, save common fields.
        VpnProfile profile = new VpnProfile(mProfile.key);
        profile.name = mName->GetText()->ToString();
        profile.type = mType->GetSelectedItemPosition();
        profile.server = mServer->GetText()->ToString().Trim();
        profile.username = mUsername->GetText()->ToString();
        profile.password = mPassword->GetText()->ToString();
        profile.searchDomains = mSearchDomains->GetText()->ToString().Trim();
        profile.dnsServers = mDnsServers->GetText()->ToString().Trim();
        profile.routes = mRoutes->GetText()->ToString().Trim();

        // Then, save type-specific fields.
        switch (profile.type) {
            case VpnProfile.TYPE_PPTP:
                profile.mppe = mMppe->IsChecked();
                break;

            case VpnProfile.TYPE_L2TP_IPSEC_PSK:
                profile.l2tpSecret = mL2tpSecret->GetText()->ToString();
                // fall through
            case VpnProfile.TYPE_IPSEC_XAUTH_PSK:
                profile.ipsecIdentifier = mIpsecIdentifier->GetText()->ToString();
                profile.ipsecSecret = mIpsecSecret->GetText()->ToString();
                break;

            case VpnProfile.TYPE_L2TP_IPSEC_RSA:
                profile.l2tpSecret = mL2tpSecret->GetText()->ToString();
                // fall through
            case VpnProfile.TYPE_IPSEC_XAUTH_RSA:
                if (mIpsecUserCert->GetSelectedItemPosition() != 0) {
                    profile.ipsecUserCert = (String) mIpsecUserCert->GetSelectedItem();
                }
                // fall through
            case VpnProfile.TYPE_IPSEC_HYBRID_RSA:
                if (mIpsecCaCert->GetSelectedItemPosition() != 0) {
                    profile.ipsecCaCert = (String) mIpsecCaCert->GetSelectedItem();
                }
                if (mIpsecServerCert->GetSelectedItemPosition() != 0) {
                    profile.ipsecServerCert = (String) mIpsecServerCert->GetSelectedItem();
                }
                break;
        }

        profile.saveLogin = mSaveLogin->IsChecked();
        return profile;
    }
}
