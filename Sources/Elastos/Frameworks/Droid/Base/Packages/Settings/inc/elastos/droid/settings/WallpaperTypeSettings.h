

package com.android.settings;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class WallpaperTypeSettings extends SettingsPreferenceFragment implements Indexable {

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.wallpaper_settings);
        PopulateWallpaperTypes();
    }

    private void PopulateWallpaperTypes() {
        // Search for activities that satisfy the ACTION_SET_WALLPAPER action
        final Intent intent = new Intent(IIntent::ACTION_SET_WALLPAPER);
        final PackageManager pm = GetPackageManager();
        final List<ResolveInfo> rList = pm->QueryIntentActivities(intent,
                PackageManager.MATCH_DEFAULT_ONLY);

        final PreferenceScreen parent = GetPreferenceScreen();
        parent->SetOrderingAsAdded(FALSE);
        // Add Preference items for each of the matching activities
        for (ResolveInfo info : rList) {
            Preference pref = new Preference(GetActivity());
            Intent prefIntent = new Intent(intent);
            prefIntent->SetComponent(new ComponentName(
                    info.activityInfo.packageName, info.activityInfo.name));
            pref->SetIntent(prefIntent);
            CharSequence label = info->LoadLabel(pm);
            if (label == NULL) label = info.activityInfo.packageName;
            pref->SetTitle(label);
            parent->AddPreference(pref);
        }
    }

    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

                final Intent intent = new Intent(IIntent::ACTION_SET_WALLPAPER);
                final PackageManager pm = context->GetPackageManager();
                final List<ResolveInfo> rList = pm->QueryIntentActivities(intent,
                        PackageManager.MATCH_DEFAULT_ONLY);

                // Add indexable data for each of the matching activities
                for (ResolveInfo info : rList) {
                    CharSequence label = info->LoadLabel(pm);
                    if (label == NULL) label = info.activityInfo.packageName;

                    SearchIndexableRaw data = new SearchIndexableRaw(context);
                    data.title = label->ToString();
                    data.screenTitle = context->GetResources()->GetString(
                            R::string::wallpaper_settings_fragment_title);
                    data.intentAction = IIntent::ACTION_SET_WALLPAPER;
                    data.intentTargetPackage = info.activityInfo.packageName;
                    data.intentTargetClass = info.activityInfo.name;
                    result->Add(data);
                }

                return result;
            }
        };
}
