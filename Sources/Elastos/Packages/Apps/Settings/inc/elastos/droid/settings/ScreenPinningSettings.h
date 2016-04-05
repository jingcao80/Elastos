
package com.android.settings;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Widget::ISwitchBar;

/**
 * Screen pinning settings.
 */
public class ScreenPinningSettings extends SettingsPreferenceFragment
        implements SwitchBar.OnSwitchChangeListener, Indexable {

    private SwitchBar mSwitchBar;

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final SettingsActivity activity = (SettingsActivity) GetActivity();

        mSwitchBar = activity->GetSwitchBar();
        mSwitchBar->AddOnSwitchChangeListener(this);
        mSwitchBar->Show();
        mSwitchBar->SetChecked(IsLockToAppEnabled());
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mSwitchBar->Hide();
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        return inflater->Inflate(R.layout.screen_pinning_instructions, NULL);
    }

    private Boolean IsLockToAppEnabled() {
        try {
            return Settings.System->GetInt(GetContentResolver(), Settings.System.LOCK_TO_APP_ENABLED)
                != 0;
        } catch (SettingNotFoundException e) {
            return FALSE;
        }
    }

    private void SetLockToAppEnabled(Boolean isEnabled) {
        Settings.System->PutInt(GetContentResolver(), Settings.System.LOCK_TO_APP_ENABLED,
                isEnabled ? 1 : 0);
    }

    /**
     * Listens to the state change of the lock-to-app master switch.
     */
    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        SetLockToAppEnabled(isChecked);
    }

    /**
     * For search
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

                final Resources res = context->GetResources();

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::screen_pinning_title);
                data.screenTitle = res->GetString(R::string::screen_pinning_title);
                result->Add(data);

                // Screen pinning description.
                data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::screen_pinning_description);
                data.screenTitle = res->GetString(R::string::screen_pinning_title);
                result->Add(data);

                return result;
            }
        };
}
