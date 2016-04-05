

package com.android.settings.vpn2;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Security::ICredentials;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::ContextMenu::IContextMenuInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::AdapterView::IAdapterContextMenuInfo;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Net::ILegacyVpnInfo;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Internal::Net::IVpnProfile;
using Elastos::Droid::Internal::Utility::IArrayUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using com::Google::Android::Collect::ILists;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

public class VpnSettings extends SettingsPreferenceFragment implements
        Handler.Callback, Preference.OnPreferenceClickListener,
        DialogInterface.OnClickListener, DialogInterface.OnDismissListener {
    private static const String TAG = "VpnSettings";

    private static const String TAG_LOCKDOWN = "lockdown";

    private static const String EXTRA_PICK_LOCKDOWN = "android.net.vpn.PICK_LOCKDOWN";

    // TODO: migrate to using DialogFragment when editing

    private final IConnectivityManager mService = IConnectivityManager.Stub
            .AsInterface(ServiceManager->GetService(Context.CONNECTIVITY_SERVICE));
    private final KeyStore mKeyStore = KeyStore->GetInstance();
    private Boolean mUnlocking = FALSE;

    private HashMap<String, VpnPreference> mPreferences = new HashMap<String, VpnPreference>();
    private VpnDialog mDialog;

    private Handler mUpdater;
    private LegacyVpnInfo mInfo;
    private UserManager mUm;

    // The key of the profile for the current ContextMenu.
    private String mSelectedKey;

    private Boolean mUnavailable;

    //@Override
    CARAPI OnCreate(Bundle savedState) {
        super->OnCreate(savedState);

        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);

        if (mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_VPN)) {
            mUnavailable = TRUE;
            SetPreferenceScreen(new PreferenceScreen(GetActivity(), NULL));
            return;
        }

        SetHasOptionsMenu(TRUE);
        AddPreferencesFromResource(R.xml.vpn_settings2);

        if (savedState != NULL) {
            VpnProfile profile = VpnProfile->Decode(savedState->GetString("VpnKey"),
                    savedState->GetByteArray("VpnProfile"));
            if (profile != NULL) {
                mDialog = new VpnDialog(GetActivity(), this, profile,
                        savedState->GetBoolean("VpnEditing"));
            }
        }
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super->OnCreateOptionsMenu(menu, inflater);
        inflater->Inflate(R.menu.vpn, menu);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        super->OnPrepareOptionsMenu(menu);

        // Hide lockdown VPN on devices that require IMS authentication
        if (SystemProperties->GetBoolean("persist.radio.imsregrequired", FALSE)) {
            menu->FindItem(R.id.vpn_lockdown).SetVisible(FALSE);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case R.id.vpn_create: {
                // Generate a new key. Here we just use the current time.
                Int64 millis = System->CurrentTimeMillis();
                while (mPreferences->ContainsKey(Long->ToHexString(millis))) {
                    ++millis;
                }
                mDialog = new VpnDialog(
                        GetActivity(), this, new VpnProfile(Long->ToHexString(millis)), TRUE);
                mDialog->SetOnDismissListener(this);
                mDialog->Show();
                return TRUE;
            }
            case R.id.vpn_lockdown: {
                LockdownConfigFragment->Show(this);
                return TRUE;
            }
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle savedState) {
        // We do not save view hierarchy, as they are just profiles.
        if (mDialog != NULL) {
            VpnProfile profile = mDialog->GetProfile();
            savedState->PutString("VpnKey", profile.key);
            savedState->PutByteArray("VpnProfile", profile->Encode());
            savedState->PutBoolean("VpnEditing", mDialog->IsEditing());
        }
        // else?
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (mUnavailable) {
            TextView emptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
            GetListView()->SetEmptyView(emptyView);
            if (emptyView != NULL) {
                emptyView->SetText(R::string::vpn_settings_not_available);
            }
            return;
        }

        final Boolean pickLockdown = GetActivity()
                .GetIntent()->GetBooleanExtra(EXTRA_PICK_LOCKDOWN, FALSE);
        if (pickLockdown) {
            LockdownConfigFragment->Show(this);
        }

        // Check KeyStore here, so others do not need to deal with it.
        if (!mKeyStore->IsUnlocked()) {
            if (!mUnlocking) {
                // Let us unlock KeyStore. See you later!
                Credentials->GetInstance()->Unlock(GetActivity());
            } else {
                // We already tried, but it is still not working!
                FinishFragment();
            }
            mUnlocking = !mUnlocking;
            return;
        }

        // Now KeyStore is always unlocked. Reset the flag.
        mUnlocking = FALSE;

        // Currently we are the only user of profiles in KeyStore.
        // Assuming KeyStore and KeyGuard do the right thing, we can
        // safely cache profiles in the memory.
        if (mPreferences->Size() == 0) {
            PreferenceGroup group = GetPreferenceScreen();

            final Context context = GetActivity();
            final List<VpnProfile> profiles = LoadVpnProfiles(mKeyStore);
            for (VpnProfile profile : profiles) {
                final VpnPreference pref = new VpnPreference(context, profile);
                pref->SetOnPreferenceClickListener(this);
                mPreferences->Put(profile.key, pref);
                group->AddPreference(pref);
            }
        }

        // Show the dialog if there is one.
        if (mDialog != NULL) {
            mDialog->SetOnDismissListener(this);
            mDialog->Show();
        }

        // Start monitoring.
        if (mUpdater == NULL) {
            mUpdater = new Handler(this);
        }
        mUpdater->SendEmptyMessage(0);

        // Register for context menu. Hmmm, GetListView() is hidden?
        RegisterForContextMenu(GetListView());
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        if (mUnavailable) {
            return;
        }

        // Hide the dialog if there is one.
        if (mDialog != NULL) {
            mDialog->SetOnDismissListener(NULL);
            mDialog->Dismiss();
        }

        // Unregister for context menu.
        if (GetView() != NULL) {
            UnregisterForContextMenu(GetListView());
        }
    }

    //@Override
    CARAPI OnDismiss(DialogInterface dialog) {
        // Here is the exit of a dialog.
        mDialog = NULL;
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 button) {
        if (button == DialogInterface.BUTTON_POSITIVE) {
            // Always save the profile.
            VpnProfile profile = mDialog->GetProfile();
            mKeyStore->Put(Credentials.VPN + profile.key, profile->Encode(), KeyStore.UID_SELF,
                    KeyStore.FLAG_ENCRYPTED);

            // Update the preference.
            VpnPreference preference = mPreferences->Get(profile.key);
            if (preference != NULL) {
                Disconnect(profile.key);
                preference->Update(profile);
            } else {
                preference = new VpnPreference(GetActivity(), profile);
                preference->SetOnPreferenceClickListener(this);
                mPreferences->Put(profile.key, preference);
                GetPreferenceScreen()->AddPreference(preference);
            }

            // If we are not editing, connect!
            if (!mDialog->IsEditing()) {
                try {
                    Connect(profile);
                } catch (Exception e) {
                    Logger::E(TAG, "connect", e);
                }
            }
        }
    }

    //@Override
    CARAPI OnCreateContextMenu(ContextMenu menu, View view, ContextMenuInfo info) {
        if (mDialog != NULL) {
            Logger::V(TAG, "OnCreateContextMenu() is called when mDialog != NULL");
            return;
        }

        if (info instanceof AdapterContextMenuInfo) {
            Preference preference = (Preference) GetListView()->GetItemAtPosition(
                    ((AdapterContextMenuInfo) info).position);
            if (preference instanceof VpnPreference) {
                VpnProfile profile = ((VpnPreference) preference).GetProfile();
                mSelectedKey = profile.key;
                menu->SetHeaderTitle(profile.name);
                menu->Add(Menu.NONE, R::string::vpn_menu_edit, 0, R::string::vpn_menu_edit);
                menu->Add(Menu.NONE, R::string::vpn_menu_delete, 0, R::string::vpn_menu_delete);
            }
        }
    }

    //@Override
    public Boolean OnContextItemSelected(MenuItem item) {
        if (mDialog != NULL) {
            Logger::V(TAG, "OnContextItemSelected() is called when mDialog != NULL");
            return FALSE;
        }

        VpnPreference preference = mPreferences->Get(mSelectedKey);
        if (preference == NULL) {
            Logger::V(TAG, "OnContextItemSelected() is called but no preference is found");
            return FALSE;
        }

        switch (item->GetItemId()) {
            case R::string::vpn_menu_edit:
                mDialog = new VpnDialog(GetActivity(), this, preference->GetProfile(), TRUE);
                mDialog->SetOnDismissListener(this);
                mDialog->Show();
                return TRUE;
            case R::string::vpn_menu_delete:
                Disconnect(mSelectedKey);
                GetPreferenceScreen()->RemovePreference(preference);
                mPreferences->Remove(mSelectedKey);
                mKeyStore->Delete(Credentials.VPN + mSelectedKey);
                return TRUE;
        }
        return FALSE;
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (mDialog != NULL) {
            Logger::V(TAG, "OnPreferenceClick() is called when mDialog != NULL");
            return TRUE;
        }

        if (preference instanceof VpnPreference) {
            VpnProfile profile = ((VpnPreference) preference).GetProfile();
            if (mInfo != NULL && profile.key->Equals(mInfo.key) &&
                    mInfo.state == LegacyVpnInfo.STATE_CONNECTED) {
                try {
                    mInfo.intent->Send();
                    return TRUE;
                } catch (Exception e) {
                    // ignore
                }
            }
            mDialog = new VpnDialog(GetActivity(), this, profile, FALSE);
        } else {
            // Generate a new key. Here we just use the current time.
            Int64 millis = System->CurrentTimeMillis();
            while (mPreferences->ContainsKey(Long->ToHexString(millis))) {
                ++millis;
            }
            mDialog = new VpnDialog(GetActivity(), this,
                    new VpnProfile(Long->ToHexString(millis)), TRUE);
        }
        mDialog->SetOnDismissListener(this);
        mDialog->Show();
        return TRUE;
    }

    //@Override
    public Boolean HandleMessage(Message message) {
        mUpdater->RemoveMessages(0);

        if (IsResumed()) {
            try {
                LegacyVpnInfo info = mService->GetLegacyVpnInfo();
                if (mInfo != NULL) {
                    VpnPreference preference = mPreferences->Get(mInfo.key);
                    if (preference != NULL) {
                        preference->Update(-1);
                    }
                    mInfo = NULL;
                }
                if (info != NULL) {
                    VpnPreference preference = mPreferences->Get(info.key);
                    if (preference != NULL) {
                        preference->Update(info.state);
                        mInfo = info;
                    }
                }
            } catch (Exception e) {
                // ignore
            }
            mUpdater->SendEmptyMessageDelayed(0, 1000);
        }
        return TRUE;
    }

    private void Connect(VpnProfile profile) throws Exception {
        try {
            mService->StartLegacyVpn(profile);
        } catch (IllegalStateException e) {
            Toast->MakeText(GetActivity(), R::string::vpn_no_network, Toast.LENGTH_LONG).Show();
        }
    }

    private void Disconnect(String key) {
        if (mInfo != NULL && key->Equals(mInfo.key)) {
            try {
                mService->PrepareVpn(VpnConfig.LEGACY_VPN, VpnConfig.LEGACY_VPN);
            } catch (Exception e) {
                // ignore
            }
        }
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_vpn;
    }

    private static class VpnPreference extends Preference {
        private VpnProfile mProfile;
        private Int32 mState = -1;

        VpnPreference(Context context, VpnProfile profile) {
            Super(context);
            SetPersistent(FALSE);
            SetOrder(0);

            mProfile = profile;
            Update();
        }

        VpnProfile GetProfile() {
            return mProfile;
        }

        void Update(VpnProfile profile) {
            mProfile = profile;
            Update();
        }

        void Update(Int32 state) {
            mState = state;
            Update();
        }

        void Update() {
            if (mState < 0) {
                String[] types = GetContext()->GetResources()
                        .GetStringArray(R.array.vpn_types_long);
                SetSummary(types[mProfile.type]);
            } else {
                String[] states = GetContext()->GetResources()
                        .GetStringArray(R.array.vpn_states);
                SetSummary(states[mState]);
            }
            SetTitle(mProfile.name);
            NotifyHierarchyChanged();
        }

        //@Override
        public Int32 CompareTo(Preference preference) {
            Int32 result = -1;
            if (preference instanceof VpnPreference) {
                VpnPreference another = (VpnPreference) preference;
                if ((result = another.mState - mState) == 0 &&
                        (result = mProfile.name->CompareTo(another.mProfile.name)) == 0 &&
                        (result = mProfile.type - another.mProfile.type) == 0) {
                    result = mProfile.key->CompareTo(another.mProfile.key);
                }
            }
            return result;
        }
    }

    /**
     * Dialog to configure always-on VPN.
     */
    public static class LockdownConfigFragment extends DialogFragment {
        private List<VpnProfile> mProfiles;
        private List<CharSequence> mTitles;
        private Int32 mCurrentIndex;

        private static class TitleAdapter extends ArrayAdapter<CharSequence> {
            public TitleAdapter(Context context, List<CharSequence> objects) {
                Super(context, R.layout.select_dialog_singlechoice_material,
                        android.R.id.text1, objects);
            }
        }

        public static void Show(VpnSettings parent) {
            if (!parent->IsAdded()) return;

            final LockdownConfigFragment dialog = new LockdownConfigFragment();
            dialog->Show(parent->GetFragmentManager(), TAG_LOCKDOWN);
        }

        private static String GetStringOrNull(KeyStore keyStore, String key) {
            final Byte[] value = keyStore->Get(Credentials.LOCKDOWN_VPN);
            return value == NULL ? NULL : new String(value);
        }

        private void InitProfiles(KeyStore keyStore, Resources res) {
            final String lockdownKey = GetStringOrNull(keyStore, Credentials.LOCKDOWN_VPN);

            mProfiles = LoadVpnProfiles(keyStore, VpnProfile.TYPE_PPTP);
            mTitles = Lists->NewArrayList();
            mTitles->Add(res->GetText(R::string::vpn_lockdown_none));
            mCurrentIndex = 0;

            for (VpnProfile profile : mProfiles) {
                if (TextUtils->Equals(profile.key, lockdownKey)) {
                    mCurrentIndex = mTitles->Size();
                }
                mTitles->Add(profile.name);
            }
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();
            final KeyStore keyStore = KeyStore->GetInstance();

            InitProfiles(keyStore, context->GetResources());

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            final LayoutInflater dialogInflater = LayoutInflater->From(builder->GetContext());

            builder->SetTitle(R::string::vpn_menu_lockdown);

            final View view = dialogInflater->Inflate(R.layout.vpn_lockdown_editor, NULL, FALSE);
            final ListView listView = (ListView) view->FindViewById(android.R.id.list);
            listView->SetChoiceMode(ListView.CHOICE_MODE_SINGLE);
            listView->SetAdapter(new TitleAdapter(context, mTitles));
            listView->SetItemChecked(mCurrentIndex, TRUE);
            builder->SetView(view);

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final Int32 newIndex = listView->GetCheckedItemPosition();
                    if (mCurrentIndex == newIndex) return;

                    if (newIndex == 0) {
                        keyStore->Delete(Credentials.LOCKDOWN_VPN);

                    } else {
                        final VpnProfile profile = mProfiles->Get(newIndex - 1);
                        if (!profile->IsValidLockdownProfile()) {
                            Toast->MakeText(context, R::string::vpn_lockdown_config_error,
                                    Toast.LENGTH_LONG).Show();
                            return;
                        }
                        keyStore->Put(Credentials.LOCKDOWN_VPN, profile.key->GetBytes(),
                                KeyStore.UID_SELF, KeyStore.FLAG_ENCRYPTED);
                    }

                    // kick profiles since we changed them
                    ConnectivityManager->From(GetActivity()).UpdateLockdownVpn();
                }
            });

            return builder->Create();
        }
    }

    private static List<VpnProfile> LoadVpnProfiles(KeyStore keyStore, Int32... excludeTypes) {
        final ArrayList<VpnProfile> result = Lists->NewArrayList();
        final String[] keys = keyStore->Saw(Credentials.VPN);
        if (keys != NULL) {
            for (String key : keys) {
                final VpnProfile profile = VpnProfile->Decode(
                        key, keyStore->Get(Credentials.VPN + key));
                if (profile != NULL && !ArrayUtils->Contains(excludeTypes, profile.type)) {
                    result->Add(profile);
                }
            }
        }
        return result;
    }
}
