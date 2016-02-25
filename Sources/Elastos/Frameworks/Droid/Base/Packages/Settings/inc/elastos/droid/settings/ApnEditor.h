/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;


public class ApnEditor extends PreferenceActivity
        implements SharedPreferences.OnSharedPreferenceChangeListener,
                    Preference.OnPreferenceChangeListener {

    private static const String TAG = ApnEditor.class->GetSimpleName();

    private static const String SAVED_POS = "pos";
    private static const String KEY_AUTH_TYPE = "auth_type";
    private static const String KEY_PROTOCOL = "apn_protocol";
    private static const String KEY_ROAMING_PROTOCOL = "apn_roaming_protocol";
    private static const String KEY_CARRIER_ENABLED = "carrier_enabled";
    private static const String KEY_BEARER = "bearer";
    private static const String KEY_MVNO_TYPE = "mvno_type";

    private static const Int32 MENU_DELETE = Menu.FIRST;
    private static const Int32 MENU_SAVE = Menu.FIRST + 1;
    private static const Int32 MENU_CANCEL = Menu.FIRST + 2;
    private static const Int32 ERROR_DIALOG_ID = 0;

    private static String sNotSet;
    private EditTextPreference mName;
    private EditTextPreference mApn;
    private EditTextPreference mProxy;
    private EditTextPreference mPort;
    private EditTextPreference mUser;
    private EditTextPreference mServer;
    private EditTextPreference mPassword;
    private EditTextPreference mMmsc;
    private EditTextPreference mMcc;
    private EditTextPreference mMnc;
    private EditTextPreference mMmsProxy;
    private EditTextPreference mMmsPort;
    private ListPreference mAuthType;
    private EditTextPreference mApnType;
    private ListPreference mProtocol;
    private ListPreference mRoamingProtocol;
    private CheckBoxPreference mCarrierEnabled;
    private ListPreference mBearer;
    private ListPreference mMvnoType;
    private EditTextPreference mMvnoMatchData;

    private String mCurMnc;
    private String mCurMcc;

    private Uri mUri;
    private Cursor mCursor;
    private Boolean mNewApn;
    private Boolean mFirstTime;
    private Resources mRes;
    private TelephonyManager mTelephonyManager;

    /**
     * Standard projection for the interesting columns of a normal note.
     */
    private static const String[] sProjection = new String[] {
            Telephony.Carriers._ID,     // 0
            Telephony.Carriers.NAME,    // 1
            Telephony.Carriers.APN,     // 2
            Telephony.Carriers.PROXY,   // 3
            Telephony.Carriers.PORT,    // 4
            Telephony.Carriers.USER,    // 5
            Telephony.Carriers.SERVER,  // 6
            Telephony.Carriers.PASSWORD, // 7
            Telephony.Carriers.MMSC, // 8
            Telephony.Carriers.MCC, // 9
            Telephony.Carriers.MNC, // 10
            Telephony.Carriers.NUMERIC, // 11
            Telephony.Carriers.MMSPROXY,// 12
            Telephony.Carriers.MMSPORT, // 13
            Telephony.Carriers.AUTH_TYPE, // 14
            Telephony.Carriers.TYPE, // 15
            Telephony.Carriers.PROTOCOL, // 16
            Telephony.Carriers.CARRIER_ENABLED, // 17
            Telephony.Carriers.BEARER, // 18
            Telephony.Carriers.ROAMING_PROTOCOL, // 19
            Telephony.Carriers.MVNO_TYPE,   // 20
            Telephony.Carriers.MVNO_MATCH_DATA  // 21
    };

    private static const Int32 ID_INDEX = 0;
    private static const Int32 NAME_INDEX = 1;
    private static const Int32 APN_INDEX = 2;
    private static const Int32 PROXY_INDEX = 3;
    private static const Int32 PORT_INDEX = 4;
    private static const Int32 USER_INDEX = 5;
    private static const Int32 SERVER_INDEX = 6;
    private static const Int32 PASSWORD_INDEX = 7;
    private static const Int32 MMSC_INDEX = 8;
    private static const Int32 MCC_INDEX = 9;
    private static const Int32 MNC_INDEX = 10;
    private static const Int32 MMSPROXY_INDEX = 12;
    private static const Int32 MMSPORT_INDEX = 13;
    private static const Int32 AUTH_TYPE_INDEX = 14;
    private static const Int32 TYPE_INDEX = 15;
    private static const Int32 PROTOCOL_INDEX = 16;
    private static const Int32 CARRIER_ENABLED_INDEX = 17;
    private static const Int32 BEARER_INDEX = 18;
    private static const Int32 ROAMING_PROTOCOL_INDEX = 19;
    private static const Int32 MVNO_TYPE_INDEX = 20;
    private static const Int32 MVNO_MATCH_DATA_INDEX = 21;


    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.apn_editor);

        sNotSet = GetResources()->GetString(R::string::apn_not_set);
        mName = (EditTextPreference) FindPreference("apn_name");
        mApn = (EditTextPreference) FindPreference("apn_apn");
        mProxy = (EditTextPreference) FindPreference("apn_http_proxy");
        mPort = (EditTextPreference) FindPreference("apn_http_port");
        mUser = (EditTextPreference) FindPreference("apn_user");
        mServer = (EditTextPreference) FindPreference("apn_server");
        mPassword = (EditTextPreference) FindPreference("apn_password");
        mMmsProxy = (EditTextPreference) FindPreference("apn_mms_proxy");
        mMmsPort = (EditTextPreference) FindPreference("apn_mms_port");
        mMmsc = (EditTextPreference) FindPreference("apn_mmsc");
        mMcc = (EditTextPreference) FindPreference("apn_mcc");
        mMnc = (EditTextPreference) FindPreference("apn_mnc");
        mApnType = (EditTextPreference) FindPreference("apn_type");

        mAuthType = (ListPreference) FindPreference(KEY_AUTH_TYPE);
        mAuthType->SetOnPreferenceChangeListener(this);

        mProtocol = (ListPreference) FindPreference(KEY_PROTOCOL);
        mProtocol->SetOnPreferenceChangeListener(this);

        mRoamingProtocol = (ListPreference) FindPreference(KEY_ROAMING_PROTOCOL);
        mRoamingProtocol->SetOnPreferenceChangeListener(this);

        mCarrierEnabled = (CheckBoxPreference) FindPreference(KEY_CARRIER_ENABLED);

        mBearer = (ListPreference) FindPreference(KEY_BEARER);
        mBearer->SetOnPreferenceChangeListener(this);

        mMvnoType = (ListPreference) FindPreference(KEY_MVNO_TYPE);
        mMvnoType->SetOnPreferenceChangeListener(this);
        mMvnoMatchData = (EditTextPreference) FindPreference("mvno_match_data");

        mRes = GetResources();

        final Intent intent = GetIntent();
        final String action = intent->GetAction();

        mFirstTime = icicle == NULL;

        if (action->Equals(IIntent::ACTION_EDIT)) {
            mUri = intent->GetData();
        } else if (action->Equals(IIntent::ACTION_INSERT)) {
            if (mFirstTime || icicle->GetInt(SAVED_POS) == 0) {
                mUri = GetContentResolver()->Insert(intent->GetData(), new ContentValues());
            } else {
                mUri = ContentUris->WithAppendedId(Telephony.Carriers.CONTENT_URI,
                        icicle->GetInt(SAVED_POS));
            }
            mNewApn = TRUE;
            // If we were unable to create a new note, then just finish
            // this activity.  A RESULT_CANCELED will be sent back to the
            // original activity if they requested a result.
            if (mUri == NULL) {
                Logger::W(TAG, "Failed to insert new telephony provider into "
                        + GetIntent()->GetData());
                Finish();
                return;
            }

            // The new entry was created, so assume all will end well and
            // set the result to be returned.
            SetResult(RESULT_OK, (new Intent()).SetAction(mUri->ToString()));

        } else {
            Finish();
            return;
        }

        mCursor = ManagedQuery(mUri, sProjection, NULL, NULL);
        mCursor->MoveToFirst();

        mTelephonyManager = (TelephonyManager) GetSystemService(TELEPHONY_SERVICE);

        FillUi();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        GetPreferenceScreen()->GetSharedPreferences()
                .RegisterOnSharedPreferenceChangeListener(this);
    }

    //@Override
    CARAPI OnPause() {
        GetPreferenceScreen()->GetSharedPreferences()
                .UnregisterOnSharedPreferenceChangeListener(this);
        super->OnPause();
    }

    private void FillUi() {
        if (mFirstTime) {
            mFirstTime = FALSE;
            // Fill in all the values from the db in both text editor and summary
            mName->SetText(mCursor->GetString(NAME_INDEX));
            mApn->SetText(mCursor->GetString(APN_INDEX));
            mProxy->SetText(mCursor->GetString(PROXY_INDEX));
            mPort->SetText(mCursor->GetString(PORT_INDEX));
            mUser->SetText(mCursor->GetString(USER_INDEX));
            mServer->SetText(mCursor->GetString(SERVER_INDEX));
            mPassword->SetText(mCursor->GetString(PASSWORD_INDEX));
            mMmsProxy->SetText(mCursor->GetString(MMSPROXY_INDEX));
            mMmsPort->SetText(mCursor->GetString(MMSPORT_INDEX));
            mMmsc->SetText(mCursor->GetString(MMSC_INDEX));
            mMcc->SetText(mCursor->GetString(MCC_INDEX));
            mMnc->SetText(mCursor->GetString(MNC_INDEX));
            mApnType->SetText(mCursor->GetString(TYPE_INDEX));
            if (mNewApn) {
                String numeric =
                    SystemProperties->Get(TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC);
                // MCC is first 3 chars and then in 2 - 3 chars of MNC
                if (numeric != NULL && numeric->Length() > 4) {
                    // Country code
                    String mcc = numeric->Substring(0, 3);
                    // Network code
                    String mnc = numeric->Substring(3);
                    // Auto populate MNC and MCC for new entries, based on what SIM reports
                    mMcc->SetText(mcc);
                    mMnc->SetText(mnc);
                    mCurMnc = mnc;
                    mCurMcc = mcc;
                }
            }
            Int32 authVal = mCursor->GetInt(AUTH_TYPE_INDEX);
            if (authVal != -1) {
                mAuthType->SetValueIndex(authVal);
            } else {
                mAuthType->SetValue(NULL);
            }

            mProtocol->SetValue(mCursor->GetString(PROTOCOL_INDEX));
            mRoamingProtocol->SetValue(mCursor->GetString(ROAMING_PROTOCOL_INDEX));
            mCarrierEnabled->SetChecked(mCursor->GetInt(CARRIER_ENABLED_INDEX)==1);
            mBearer->SetValue(mCursor->GetString(BEARER_INDEX));
            mMvnoType->SetValue(mCursor->GetString(MVNO_TYPE_INDEX));
            mMvnoMatchData->SetEnabled(FALSE);
            mMvnoMatchData->SetText(mCursor->GetString(MVNO_MATCH_DATA_INDEX));
        }

        mName->SetSummary(CheckNull(mName->GetText()));
        mApn->SetSummary(CheckNull(mApn->GetText()));
        mProxy->SetSummary(CheckNull(mProxy->GetText()));
        mPort->SetSummary(CheckNull(mPort->GetText()));
        mUser->SetSummary(CheckNull(mUser->GetText()));
        mServer->SetSummary(CheckNull(mServer->GetText()));
        mPassword->SetSummary(Starify(mPassword->GetText()));
        mMmsProxy->SetSummary(CheckNull(mMmsProxy->GetText()));
        mMmsPort->SetSummary(CheckNull(mMmsPort->GetText()));
        mMmsc->SetSummary(CheckNull(mMmsc->GetText()));
        mMcc->SetSummary(CheckNull(mMcc->GetText()));
        mMnc->SetSummary(CheckNull(mMnc->GetText()));
        mApnType->SetSummary(CheckNull(mApnType->GetText()));

        String authVal = mAuthType->GetValue();
        if (authVal != NULL) {
            Int32 authValIndex = Integer->ParseInt(authVal);
            mAuthType->SetValueIndex(authValIndex);

            String []values = mRes->GetStringArray(R.array.apn_auth_entries);
            mAuthType->SetSummary(values[authValIndex]);
        } else {
            mAuthType->SetSummary(sNotSet);
        }

        mProtocol->SetSummary(
                CheckNull(ProtocolDescription(mProtocol->GetValue(), mProtocol)));
        mRoamingProtocol->SetSummary(
                CheckNull(ProtocolDescription(mRoamingProtocol->GetValue(), mRoamingProtocol)));
        mBearer->SetSummary(
                CheckNull(BearerDescription(mBearer->GetValue())));
        mMvnoType->SetSummary(
                CheckNull(MvnoDescription(mMvnoType->GetValue())));
        mMvnoMatchData->SetSummary(CheckNull(mMvnoMatchData->GetText()));
        // allow user to edit carrier_enabled for some APN
        Boolean ceEditable = GetResources()->GetBoolean(R.bool.config_allow_edit_carrier_enabled);
        if (ceEditable) {
            mCarrierEnabled->SetEnabled(TRUE);
        } else {
            mCarrierEnabled->SetEnabled(FALSE);
        }
    }

    /**
     * Returns the UI choice (e.g., "IPv4/IPv6") corresponding to the given
     * raw value of the protocol preference (e.g., "IPV4V6"). If unknown,
     * return NULL.
     */
    private String ProtocolDescription(String raw, ListPreference protocol) {
        Int32 protocolIndex = protocol->FindIndexOfValue(raw);
        if (protocolIndex == -1) {
            return NULL;
        } else {
            String[] values = mRes->GetStringArray(R.array.apn_protocol_entries);
            try {
                return values[protocolIndex];
            } catch (ArrayIndexOutOfBoundsException e) {
                return NULL;
            }
        }
    }

    private String BearerDescription(String raw) {
        Int32 mBearerIndex = mBearer->FindIndexOfValue(raw);
        if (mBearerIndex == -1) {
            return NULL;
        } else {
            String[] values = mRes->GetStringArray(R.array.bearer_entries);
            try {
                return values[mBearerIndex];
            } catch (ArrayIndexOutOfBoundsException e) {
                return NULL;
            }
        }
    }

    private String MvnoDescription(String newValue) {
        Int32 mvnoIndex = mMvnoType->FindIndexOfValue(newValue);
        String oldValue = mMvnoType->GetValue();

        if (mvnoIndex == -1) {
            return NULL;
        } else {
            String[] values = mRes->GetStringArray(R.array.mvno_type_entries);
            if (values[mvnoIndex].Equals("None")) {
                mMvnoMatchData->SetEnabled(FALSE);
            } else {
                mMvnoMatchData->SetEnabled(TRUE);
            }
            if (newValue != NULL && newValue->Equals(oldValue) == FALSE) {
                if (values[mvnoIndex].Equals("SPN")) {
                    mMvnoMatchData->SetText(mTelephonyManager->GetSimOperatorName());
                } else if (values[mvnoIndex].Equals("IMSI")) {
                    String numeric =
                            SystemProperties->Get(TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC);
                    mMvnoMatchData->SetText(numeric + "x");
                } else if (values[mvnoIndex].Equals("GID")) {
                    mMvnoMatchData->SetText(mTelephonyManager->GetGroupIdLevel1());
                }
            }

            try {
                return values[mvnoIndex];
            } catch (ArrayIndexOutOfBoundsException e) {
                return NULL;
            }
        }
    }

    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        String key = preference->GetKey();
        if (KEY_AUTH_TYPE->Equals(key)) {
            try {
                Int32 index = Integer->ParseInt((String) newValue);
                mAuthType->SetValueIndex(index);

                String []values = mRes->GetStringArray(R.array.apn_auth_entries);
                mAuthType->SetSummary(values[index]);
            } catch (NumberFormatException e) {
                return FALSE;
            }
        } else if (KEY_PROTOCOL->Equals(key)) {
            String protocol = ProtocolDescription((String) newValue, mProtocol);
            if (protocol == NULL) {
                return FALSE;
            }
            mProtocol->SetSummary(protocol);
            mProtocol->SetValue((String) newValue);
        } else if (KEY_ROAMING_PROTOCOL->Equals(key)) {
            String protocol = ProtocolDescription((String) newValue, mRoamingProtocol);
            if (protocol == NULL) {
                return FALSE;
            }
            mRoamingProtocol->SetSummary(protocol);
            mRoamingProtocol->SetValue((String) newValue);
        } else if (KEY_BEARER->Equals(key)) {
            String bearer = BearerDescription((String) newValue);
            if (bearer == NULL) {
                return FALSE;
            }
            mBearer->SetValue((String) newValue);
            mBearer->SetSummary(bearer);
        } else if (KEY_MVNO_TYPE->Equals(key)) {
            String mvno = MvnoDescription((String) newValue);
            if (mvno == NULL) {
                return FALSE;
            }
            mMvnoType->SetValue((String) newValue);
            mMvnoType->SetSummary(mvno);
        }

        return TRUE;
    }

    //@Override
    public Boolean OnCreateOptionsMenu(Menu menu) {
        super->OnCreateOptionsMenu(menu);
        // If it's a new APN, then cancel will delete the new entry in onPause
        if (!mNewApn) {
            menu->Add(0, MENU_DELETE, 0, R::string::menu_delete)
                .SetIcon(R.drawable.ic_menu_delete);
        }
        menu->Add(0, MENU_SAVE, 0, R::string::menu_save)
            .SetIcon(android.R.drawable.ic_menu_save);
        menu->Add(0, MENU_CANCEL, 0, R::string::menu_cancel)
            .SetIcon(android.R.drawable.ic_menu_close_clear_cancel);
        return TRUE;
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
        case MENU_DELETE:
            DeleteApn();
            return TRUE;
        case MENU_SAVE:
            if (ValidateAndSave(FALSE)) {
                Finish();
            }
            return TRUE;
        case MENU_CANCEL:
            if (mNewApn) {
                GetContentResolver()->Delete(mUri, NULL, NULL);
            }
            Finish();
            return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    public Boolean OnKeyDown(Int32 keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK: {
                if (ValidateAndSave(FALSE)) {
                    Finish();
                }
                return TRUE;
            }
        }
        return super->OnKeyDown(keyCode, event);
    }

    //@Override
    protected void OnSaveInstanceState(Bundle icicle) {
        super->OnSaveInstanceState(icicle);
        if (ValidateAndSave(TRUE)) {
            icicle->PutInt(SAVED_POS, mCursor->GetInt(ID_INDEX));
        }
    }

    /**
     * Check the key fields' validity and save if valid.
     * @param force save even if the fields are not valid, if the app is
     *        being suspended
     * @return TRUE if the data was saved
     */
    private Boolean ValidateAndSave(Boolean force) {
        String name = CheckNotSet(mName->GetText());
        String apn = CheckNotSet(mApn->GetText());
        String mcc = CheckNotSet(mMcc->GetText());
        String mnc = CheckNotSet(mMnc->GetText());

        if (GetErrorMsg() != NULL && !force) {
            ShowDialog(ERROR_DIALOG_ID);
            return FALSE;
        }

        if (!mCursor->MoveToFirst()) {
            Logger::W(TAG,
                    "Could not go to the first row in the Cursor when saving data.");
            return FALSE;
        }

        // If it's a new APN and a name or apn haven't been entered, then erase the entry
        if (force && mNewApn && name->Length() < 1 && apn->Length() < 1) {
            GetContentResolver()->Delete(mUri, NULL, NULL);
            return FALSE;
        }

        ContentValues values = new ContentValues();

        // Add a dummy name "Untitled", if the user exits the screen without adding a name but 
        // entered other information worth keeping.
        values->Put(Telephony.Carriers.NAME,
                name->Length() < 1 ? GetResources()->GetString(R::string::untitled_apn) : name);
        values->Put(Telephony.Carriers.APN, apn);
        values->Put(Telephony.Carriers.PROXY, CheckNotSet(mProxy->GetText()));
        values->Put(Telephony.Carriers.PORT, CheckNotSet(mPort->GetText()));
        values->Put(Telephony.Carriers.MMSPROXY, CheckNotSet(mMmsProxy->GetText()));
        values->Put(Telephony.Carriers.MMSPORT, CheckNotSet(mMmsPort->GetText()));
        values->Put(Telephony.Carriers.USER, CheckNotSet(mUser->GetText()));
        values->Put(Telephony.Carriers.SERVER, CheckNotSet(mServer->GetText()));
        values->Put(Telephony.Carriers.PASSWORD, CheckNotSet(mPassword->GetText()));
        values->Put(Telephony.Carriers.MMSC, CheckNotSet(mMmsc->GetText()));

        String authVal = mAuthType->GetValue();
        if (authVal != NULL) {
            values->Put(Telephony.Carriers.AUTH_TYPE, Integer->ParseInt(authVal));
        }

        values->Put(Telephony.Carriers.PROTOCOL, CheckNotSet(mProtocol->GetValue()));
        values->Put(Telephony.Carriers.ROAMING_PROTOCOL, CheckNotSet(mRoamingProtocol->GetValue()));

        values->Put(Telephony.Carriers.TYPE, CheckNotSet(mApnType->GetText()));

        values->Put(Telephony.Carriers.MCC, mcc);
        values->Put(Telephony.Carriers.MNC, mnc);

        values->Put(Telephony.Carriers.NUMERIC, mcc + mnc);

        if (mCurMnc != NULL && mCurMcc != NULL) {
            if (mCurMnc->Equals(mnc) && mCurMcc->Equals(mcc)) {
                values->Put(Telephony.Carriers.CURRENT, 1);
            }
        }

        String bearerVal = mBearer->GetValue();
        if (bearerVal != NULL) {
            values->Put(Telephony.Carriers.BEARER, Integer->ParseInt(bearerVal));
        }

        values->Put(Telephony.Carriers.MVNO_TYPE, CheckNotSet(mMvnoType->GetValue()));
        values->Put(Telephony.Carriers.MVNO_MATCH_DATA, CheckNotSet(mMvnoMatchData->GetText()));

        values->Put(Telephony.Carriers.CARRIER_ENABLED, mCarrierEnabled->IsChecked() ? 1 : 0);
        GetContentResolver()->Update(mUri, values, NULL, NULL);

        return TRUE;
    }

    private String GetErrorMsg() {
        String errorMsg = NULL;

        String name = CheckNotSet(mName->GetText());
        String apn = CheckNotSet(mApn->GetText());
        String mcc = CheckNotSet(mMcc->GetText());
        String mnc = CheckNotSet(mMnc->GetText());

        if (name->Length() < 1) {
            errorMsg = mRes->GetString(R::string::error_name_empty);
        } else if (apn->Length() < 1) {
            errorMsg = mRes->GetString(R::string::error_apn_empty);
        } else if (mcc->Length() != 3) {
            errorMsg = mRes->GetString(R::string::error_mcc_not3);
        } else if ((mnc->Length() & 0xFFFE) != 2) {
            errorMsg = mRes->GetString(R::string::error_mnc_not23);
        }

        return errorMsg;
    }

    //@Override
    protected Dialog OnCreateDialog(Int32 id) {

        if (id == ERROR_DIALOG_ID) {
            String msg = GetErrorMsg();

            return new AlertDialog->Builder(this)
                    .SetTitle(R::string::error_title)
                    .SetPositiveButton(android.R::string::ok, NULL)
                    .SetMessage(msg)
                    .Create();
        }

        return super->OnCreateDialog(id);
    }

    //@Override
    protected void OnPrepareDialog(Int32 id, Dialog dialog) {
        super->OnPrepareDialog(id, dialog);

        if (id == ERROR_DIALOG_ID) {
            String msg = GetErrorMsg();

            if (msg != NULL) {
                ((AlertDialog)dialog).SetMessage(msg);
            }
        }
    }

    private void DeleteApn() {
        GetContentResolver()->Delete(mUri, NULL, NULL);
        Finish();
    }

    private String Starify(String value) {
        if (value == NULL || value->Length() == 0) {
            return sNotSet;
        } else {
            Char32[] password = new Char32[value->Length()];
            for (Int32 i = 0; i < password.length; i++) {
                password[i] = '*';
            }
            return new String(password);
        }
    }

    private String CheckNull(String value) {
        if (value == NULL || value->Length() == 0) {
            return sNotSet;
        } else {
            return value;
        }
    }

    private String CheckNotSet(String value) {
        if (value == NULL || value->Equals(sNotSet)) {
            return "";
        } else {
            return value;
        }
    }

    CARAPI OnSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        Preference pref = FindPreference(key);
        if (pref != NULL) {
            if (pref->Equals(mPassword)){
                pref->SetSummary(Starify(sharedPreferences->GetString(key, "")));
            } else if (pref->Equals(mCarrierEnabled)) {
                // do nothing
            } else {
                pref->SetSummary(CheckNull(sharedPreferences->GetString(key, "")));
            }
        }
    }
}
