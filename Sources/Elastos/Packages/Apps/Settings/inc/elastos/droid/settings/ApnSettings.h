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

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

using Elastos::Utility::IArrayList;

public class ApnSettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceChangeListener {
    static const String TAG = "ApnSettings";

    public static const String EXTRA_POSITION = "position";
    public static const String RESTORE_CARRIERS_URI =
        "content://telephony/carriers/restore";
    public static const String PREFERRED_APN_URI =
        "content://telephony/carriers/preferapn";

    public static const String APN_ID = "apn_id";

    private static const Int32 ID_INDEX = 0;
    private static const Int32 NAME_INDEX = 1;
    private static const Int32 APN_INDEX = 2;
    private static const Int32 TYPES_INDEX = 3;

    private static const Int32 MENU_NEW = Menu.FIRST;
    private static const Int32 MENU_RESTORE = Menu.FIRST + 1;

    private static const Int32 EVENT_RESTORE_DEFAULTAPN_START = 1;
    private static const Int32 EVENT_RESTORE_DEFAULTAPN_COMPLETE = 2;

    private static const Int32 DIALOG_RESTORE_DEFAULTAPN = 1001;

    private static const Uri DEFAULTAPN_URI = Uri->Parse(RESTORE_CARRIERS_URI);
    private static const Uri PREFERAPN_URI = Uri->Parse(PREFERRED_APN_URI);

    private static Boolean mRestoreDefaultApnMode;

    private RestoreApnUiHandler mRestoreApnUiHandler;
    private RestoreApnProcessHandler mRestoreApnProcessHandler;
    private HandlerThread mRestoreDefaultApnThread;

    private UserManager mUm;

    private String mSelectedKey;

    private IntentFilter mMobileStateFilter;

    private Boolean mUnavailable;

    private final BroadcastReceiver mMobileStateReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (intent->GetAction()->Equals(
                    TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED)) {
                PhoneConstants.DataState state = GetMobileDataState(intent);
                switch (state) {
                case CONNECTED:
                    if (!mRestoreDefaultApnMode) {
                        FillList();
                    } else {
                        ShowDialog(DIALOG_RESTORE_DEFAULTAPN);
                    }
                    break;
                }
            }
        }
    };

    private static PhoneConstants.DataState GetMobileDataState(Intent intent) {
        String str = intent->GetStringExtra(PhoneConstants.STATE_KEY);
        if (str != NULL) {
            return Enum->ValueOf(PhoneConstants.DataState.class, str);
        } else {
            return PhoneConstants.DataState.DISCONNECTED;
        }
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);

        mMobileStateFilter = new IntentFilter(
                TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);

        if (!mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
            SetHasOptionsMenu(TRUE);
        }
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        TextView empty = (TextView) GetView()->FindViewById(android.R.id.empty);
        if (empty != NULL) {
            empty->SetText(R::string::apn_settings_not_available);
            GetListView()->SetEmptyView(empty);
        }

        if (mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
            mUnavailable = TRUE;
            SetPreferenceScreen(new PreferenceScreen(GetActivity(), NULL));
            return;
        }

        AddPreferencesFromResource(R.xml.apn_settings);

        GetListView()->SetItemsCanFocus(TRUE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (mUnavailable) {
            return;
        }

        GetActivity()->RegisterReceiver(mMobileStateReceiver, mMobileStateFilter);

        if (!mRestoreDefaultApnMode) {
            FillList();
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        if (mUnavailable) {
            return;
        }

        GetActivity()->UnregisterReceiver(mMobileStateReceiver);
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();

        if (mRestoreDefaultApnThread != NULL) {
            mRestoreDefaultApnThread->Quit();
        }
    }

    private void FillList() {
        String where = "numeric=\""
            + android.os.SystemProperties->Get(TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC, "")
            + "\"";

        Cursor cursor = GetContentResolver()->Query(Telephony.Carriers.CONTENT_URI, new String[] {
                "_id", "name", "apn", "type"}, where, NULL,
                Telephony.Carriers.DEFAULT_SORT_ORDER);

        if (cursor != NULL) {
            PreferenceGroup apnList = (PreferenceGroup) FindPreference("apn_list");
            apnList->RemoveAll();

            ArrayList<Preference> mmsApnList = new ArrayList<Preference>();

            mSelectedKey = GetSelectedApnKey();
            cursor->MoveToFirst();
            while (!cursor->IsAfterLast()) {
                String name = cursor->GetString(NAME_INDEX);
                String apn = cursor->GetString(APN_INDEX);
                String key = cursor->GetString(ID_INDEX);
                String type = cursor->GetString(TYPES_INDEX);

                ApnPreference pref = new ApnPreference(GetActivity());

                pref->SetKey(key);
                pref->SetTitle(name);
                pref->SetSummary(apn);
                pref->SetPersistent(FALSE);
                pref->SetOnPreferenceChangeListener(this);

                Boolean selectable = ((type == NULL) || !type->Equals("mms"));
                pref->SetSelectable(selectable);
                if (selectable) {
                    if ((mSelectedKey != NULL) && mSelectedKey->Equals(key)) {
                        pref->SetChecked();
                    }
                    apnList->AddPreference(pref);
                } else {
                    mmsApnList->Add(pref);
                }
                cursor->MoveToNext();
            }
            cursor->Close();

            for (Preference preference : mmsApnList) {
                apnList->AddPreference(preference);
            }
        }
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (!mUnavailable) {
            menu->Add(0, MENU_NEW, 0,
                    GetResources()->GetString(R::string::menu_new))
                    .SetIcon(android.R.drawable.ic_menu_add)
                    .SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
            menu->Add(0, MENU_RESTORE, 0,
                    GetResources()->GetString(R::string::menu_restore))
                    .SetIcon(android.R.drawable.ic_menu_upload);
        }

        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
        case MENU_NEW:
            AddNewApn();
            return TRUE;

        case MENU_RESTORE:
            RestoreDefaultApn();
            return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    private void AddNewApn() {
        StartActivity(new Intent(IIntent::ACTION_INSERT, Telephony.Carriers.CONTENT_URI));
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        Int32 pos = Integer->ParseInt(preference->GetKey());
        Uri url = ContentUris->WithAppendedId(Telephony.Carriers.CONTENT_URI, pos);
        StartActivity(new Intent(IIntent::ACTION_EDIT, url));
        return TRUE;
    }

    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        Logger::D(TAG, "OnPreferenceChange(): Preference - " + preference
                + ", newValue - " + newValue + ", newValue type - "
                + newValue->GetClass());
        if (newValue instanceof String) {
            SetSelectedApnKey((String) newValue);
        }

        return TRUE;
    }

    private void SetSelectedApnKey(String key) {
        mSelectedKey = key;
        ContentResolver resolver = GetContentResolver();

        ContentValues values = new ContentValues();
        values->Put(APN_ID, mSelectedKey);
        resolver->Update(PREFERAPN_URI, values, NULL, NULL);
    }

    private String GetSelectedApnKey() {
        String key = NULL;

        Cursor cursor = GetContentResolver()->Query(PREFERAPN_URI, new String[] {"_id"},
                NULL, NULL, Telephony.Carriers.DEFAULT_SORT_ORDER);
        if (cursor->GetCount() > 0) {
            cursor->MoveToFirst();
            key = cursor->GetString(ID_INDEX);
        }
        cursor->Close();
        return key;
    }

    private Boolean RestoreDefaultApn() {
        ShowDialog(DIALOG_RESTORE_DEFAULTAPN);
        mRestoreDefaultApnMode = TRUE;

        if (mRestoreApnUiHandler == NULL) {
            mRestoreApnUiHandler = new RestoreApnUiHandler();
        }

        if (mRestoreApnProcessHandler == NULL ||
            mRestoreDefaultApnThread == NULL) {
            mRestoreDefaultApnThread = new HandlerThread(
                    "Restore default APN Handler: Process Thread");
            mRestoreDefaultApnThread->Start();
            mRestoreApnProcessHandler = new RestoreApnProcessHandler(
                    mRestoreDefaultApnThread->GetLooper(), mRestoreApnUiHandler);
        }

        mRestoreApnProcessHandler
                .SendEmptyMessage(EVENT_RESTORE_DEFAULTAPN_START);
        return TRUE;
    }

    private class RestoreApnUiHandler extends Handler {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_RESTORE_DEFAULTAPN_COMPLETE:
                    Activity activity = GetActivity();
                    if (activity == NULL) {
                        mRestoreDefaultApnMode = FALSE;
                        return;
                    }
                    FillList();
                    GetPreferenceScreen()->SetEnabled(TRUE);
                    mRestoreDefaultApnMode = FALSE;
                    RemoveDialog(DIALOG_RESTORE_DEFAULTAPN);
                    Toast->MakeText(
                        activity,
                        GetResources()->GetString(
                                R::string::restore_default_apn_completed),
                        Toast.LENGTH_LONG).Show();
                    break;
            }
        }
    }

    private class RestoreApnProcessHandler extends Handler {
        private Handler mRestoreApnUiHandler;

        public RestoreApnProcessHandler(Looper looper, Handler restoreApnUiHandler) {
            Super(looper);
            this.mRestoreApnUiHandler = restoreApnUiHandler;
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_RESTORE_DEFAULTAPN_START:
                    ContentResolver resolver = GetContentResolver();
                    resolver->Delete(DEFAULTAPN_URI, NULL, NULL);
                    mRestoreApnUiHandler
                        .SendEmptyMessage(EVENT_RESTORE_DEFAULTAPN_COMPLETE);
                    break;
            }
        }
    }

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        if (id == DIALOG_RESTORE_DEFAULTAPN) {
            ProgressDialog dialog = new ProgressDialog(GetActivity());
            dialog->SetMessage(GetResources()->GetString(R::string::restore_default_apn));
            dialog->SetCancelable(FALSE);
            return dialog;
        }
        return NULL;
    }
}
