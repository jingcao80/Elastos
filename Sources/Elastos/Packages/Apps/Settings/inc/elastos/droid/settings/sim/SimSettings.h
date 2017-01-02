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

package com.android.settings.sim;

using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Settings::IRestrictedSettingsFragment;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Notification::IDropDownPreference;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::Indexable::ISearchIndexProvider;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class SimSettings extends RestrictedSettingsFragment implements Indexable {
    private static const String TAG = "SimSettings";

    private static const String DISALLOW_CONFIG_SIM = "no_config_sim";
    private static const String SIM_CARD_CATEGORY = "sim_cards";
    private static const String KEY_CELLULAR_DATA = "sim_cellular_data";
    private static const String KEY_CALLS = "sim_calls";
    private static const String KEY_SMS = "sim_sms";
    private static const String KEY_ACTIVITIES = "activities";

    /**
     * By UX design we have use only one Subscription Information(SubInfo) record per SIM slot.
     * mAvalableSubInfos is the list of SubInfos we present to the user.
     * mSubInfoList is the list of all SubInfos.
     */
    private List<SubInfoRecord> mAvailableSubInfos = NULL;
    private List<SubInfoRecord> mSubInfoList = NULL;

    private SubInfoRecord mCellularData = NULL;
    private SubInfoRecord mCalls = NULL;
    private SubInfoRecord mSMS = NULL;

    private Int32 mNumSims;

    public SimSettings() {
        Super(DISALLOW_CONFIG_SIM);
    }

    //@Override
    CARAPI OnCreate(final Bundle bundle) {
        super->OnCreate(bundle);

        if (mSubInfoList == NULL) {
            mSubInfoList = SubscriptionManager->GetActiveSubInfoList();
        }

        CreatePreferences();
        UpdateAllOptions();
    }

    private void CreatePreferences() {
        final TelephonyManager tm =
            (TelephonyManager) GetActivity()->GetSystemService(Context.TELEPHONY_SERVICE);

        AddPreferencesFromResource(R.xml.sim_settings);

        final PreferenceCategory simCards = (PreferenceCategory)FindPreference(SIM_CARD_CATEGORY);

        final Int32 numSlots = tm->GetSimCount();
        mAvailableSubInfos = new ArrayList<SubInfoRecord>(numSlots);
        mNumSims = 0;
        for (Int32 i = 0; i < numSlots; ++i) {
            final SubInfoRecord sir = FindRecordBySlotId(i);
            simCards->AddPreference(new SimPreference(GetActivity(), sir, i));
            mAvailableSubInfos->Add(sir);
            if (sir != NULL) {
                mNumSims++;
            }
        }

        UpdateActivitesCategory();
    }

    private void UpdateAllOptions() {
        UpdateSimSlotValues();
        UpdateActivitesCategory();
    }

    private void UpdateSimSlotValues() {
        SubscriptionManager->GetAllSubInfoList();
        final PreferenceCategory simCards = (PreferenceCategory)FindPreference(SIM_CARD_CATEGORY);
        final PreferenceScreen prefScreen = GetPreferenceScreen();

        final Int32 prefSize = prefScreen->GetPreferenceCount();
        for (Int32 i = 0; i < prefSize; ++i) {
            Preference pref = prefScreen->GetPreference(i);
            if (pref instanceof SimPreference) {
                ((SimPreference)pref).Update();
            }
        }
    }

    private void UpdateActivitesCategory() {
        CreateDropDown((DropDownPreference) FindPreference(KEY_CELLULAR_DATA));
        CreateDropDown((DropDownPreference) FindPreference(KEY_CALLS));
        CreateDropDown((DropDownPreference) FindPreference(KEY_SMS));

        UpdateCellularDataValues();
        UpdateCallValues();
        UpdateSmsValues();
    }

    /**
     * finds a record with subId.
     * Since the number of SIMs are few, an array is fine.
     */
    private SubInfoRecord FindRecordBySubId(final Int64 subId) {
        final Int32 availableSubInfoLength = mAvailableSubInfos->Size();

        for (Int32 i = 0; i < availableSubInfoLength; ++i) {
            final SubInfoRecord sir = mAvailableSubInfos->Get(i);
            if (sir != NULL && sir.subId == subId) {
                return sir;
            }
        }

        return NULL;
    }

    /**
     * finds a record with slotId.
     * Since the number of SIMs are few, an array is fine.
     */
    private SubInfoRecord FindRecordBySlotId(final Int32 slotId) {
        if (mSubInfoList != NULL){
            final Int32 availableSubInfoLength = mSubInfoList->Size();

            for (Int32 i = 0; i < availableSubInfoLength; ++i) {
                final SubInfoRecord sir = mSubInfoList->Get(i);
                if (sir.slotId == slotId) {
                    //Right now we take the first subscription on a SIM.
                    return sir;
                }
            }
        }

        return NULL;
    }

    private void UpdateSmsValues() {
        final DropDownPreference simPref = (DropDownPreference) FindPreference(KEY_SMS);
        final SubInfoRecord sir = FindRecordBySubId(SubscriptionManager->GetDefaultSmsSubId());
        if (sir != NULL) {
            simPref->SetSelectedItem(sir.slotId + 1);
        }
        simPref->SetEnabled(mNumSims > 1);
    }

    private void UpdateCellularDataValues() {
        final DropDownPreference simPref = (DropDownPreference) FindPreference(KEY_CELLULAR_DATA);
        final SubInfoRecord sir = FindRecordBySubId(SubscriptionManager->GetDefaultDataSubId());
        if (sir != NULL) {
            simPref->SetSelectedItem(sir.slotId);
        }
        simPref->SetEnabled(mNumSims > 1);
    }

    private void UpdateCallValues() {
        final DropDownPreference simPref = (DropDownPreference) FindPreference(KEY_CALLS);
        final SubInfoRecord sir = FindRecordBySubId(SubscriptionManager->GetDefaultVoiceSubId());
        if (sir != NULL) {
            simPref->SetSelectedItem(sir.slotId + 1);
        }
        simPref->SetEnabled(mNumSims > 1);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        UpdateAllOptions();
    }

    //@Override
    public Boolean OnPreferenceTreeClick(final PreferenceScreen preferenceScreen,
            final Preference preference) {
        if (preference instanceof SimPreference) {
            ((SimPreference)preference).CreateEditDialog((SimPreference)preference);
        }

        return TRUE;
    }

    CARAPI CreateDropDown(DropDownPreference preference) {
        final DropDownPreference simPref = preference;
        final String keyPref = simPref->GetKey();
        final Boolean askFirst = keyPref->Equals(KEY_CALLS) || keyPref->Equals(KEY_SMS);

        simPref->ClearItems();

        if (askFirst) {
            simPref->AddItem(GetResources()->GetString(
                    R::string::sim_calls_ask_first_prefs_title), NULL);
        }

        final Int32 subAvailableSize = mAvailableSubInfos->Size();
        for (Int32 i = 0; i < subAvailableSize; ++i) {
            final SubInfoRecord sir = mAvailableSubInfos->Get(i);
            If(sir != NULL){
                simPref->AddItem(sir.displayName, sir);
            }
        }

        simPref->SetCallback(new DropDownPreference->Callback() {
            //@Override
            public Boolean OnItemSelected(Int32 pos, Object value) {
                final Int64 subId = value == NULL ? 0 : ((SubInfoRecord)value).subId;

                if (simPref->GetKey()->Equals(KEY_CELLULAR_DATA)) {
                    SubscriptionManager->SetDefaultDataSubId(subId);
                } else if (simPref->GetKey()->Equals(KEY_CALLS)) {
                    SubscriptionManager->SetDefaultVoiceSubId(subId);
                } else if (simPref->GetKey()->Equals(KEY_SMS)) {
                    // TODO: uncomment once implemented. Bug: 16520931
                    // SubscriptionManager->SetDefaultSMSSubId(subId);
                }

                return TRUE;
            }
        });
    }

    private void SetActivity(Preference preference, SubInfoRecord sir) {
        final String key = preference->GetKey();

        if (key->Equals(KEY_CELLULAR_DATA)) {
            mCellularData = sir;
        } else if (key->Equals(KEY_CALLS)) {
            mCalls = sir;
        } else if (key->Equals(KEY_SMS)) {
            mSMS = sir;
        }

        UpdateActivitesCategory();
    }

    private class SimPreference extends Preference{
        private SubInfoRecord mSubInfoRecord;
        private Int32 mSlotId;

        public SimPreference(Context context, SubInfoRecord subInfoRecord, Int32 slotId) {
            Super(context);

            mSubInfoRecord = subInfoRecord;
            mSlotId = slotId;
            SetKey("sim" + mSlotId);
            Update();
        }

        CARAPI Update() {
            final Resources res = GetResources();

            SetTitle(res->GetString(R::string::sim_card_number_title, mSlotId + 1));
            if (mSubInfoRecord != NULL) {
                SetSummary(res->GetString(R::string::sim_settings_summary,
                            mSubInfoRecord.displayName, mSubInfoRecord.number));
                SetEnabled(TRUE);
            } else {
                SetSummary(R::string::sim_slot_empty);
                SetFragment(NULL);
                SetEnabled(FALSE);
            }
        }

        CARAPI CreateEditDialog(SimPreference simPref) {
            AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());

            final View dialogLayout = GetActivity()->GetLayoutInflater().Inflate(
                    R.layout.multi_sim_dialog, NULL);
            builder->SetView(dialogLayout);

            EditText nameText = (EditText)dialogLayout->FindViewById(R.id.sim_name);
            nameText->SetText(mSubInfoRecord.displayName);

            TextView numberView = (TextView)dialogLayout->FindViewById(R.id.number);
            numberView->SetText(mSubInfoRecord.number);

            TextView carrierView = (TextView)dialogLayout->FindViewById(R.id.carrier);
            carrierView->SetText(mSubInfoRecord.displayName);

            builder->SetTitle(R::string::sim_editor_title);

            builder->SetPositiveButton(R::string::okay, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    final EditText nameText = (EditText)dialogLayout->FindViewById(R.id.sim_name);
                    final Spinner displayNumbers =
                        (Spinner)dialogLayout->FindViewById(R.id.display_numbers);

                    SubscriptionManager->SetDisplayNumberFormat(
                        displayNumbers->GetSelectedItemPosition() == 0
                            ? SubscriptionManager.DISPLAY_NUMBER_LAST
                            : SubscriptionManager.DISPLAY_NUMBER_FIRST, mSubInfoRecord.subId);

                    mSubInfoRecord.displayName = nameText->GetText()->ToString();
                    SubscriptionManager->SetDisplayName(mSubInfoRecord.displayName,
                        mSubInfoRecord.subId);

                    UpdateAllOptions();
                    Update();
                }
            });

            builder->SetNegativeButton(R::string::cancel, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 whichButton) {
                    dialog->Dismiss();
                }
            });

            builder->Create()->Show();
        }
    }

    /**
     * For search
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {
                //@Override
                public List<SearchIndexableResource> GetXmlResourcesToIndex(Context context,
                        Boolean enabled) {
                    ArrayList<SearchIndexableResource> result =
                            new ArrayList<SearchIndexableResource>();

                    if (Utils->ShowSimCardTile(context)) {
                        SearchIndexableResource sir = new SearchIndexableResource(context);
                        sir.xmlResId = R.xml.sim_settings;
                        result->Add(sir);
                    }

                    return result;
                }
            };

}
