
#ifndef __ELASTOS_DROID_SETTINGS_HOMESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_HOMESETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// using Elastos::Droid::App::IActivity;
// using Elastos::Droid::App::IActivityManager;
// using Elastos::Droid::Content::IBroadcastReceiver;
// using Elastos::Droid::Content::IComponentName;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Content::IIntentFilter;
// using Elastos::Droid::Content::ISharedPreferences;
// using Elastos::Droid::Content::Pm::IActivityInfo;
// using Elastos::Droid::Content::Pm::IApplicationInfo;
// using Elastos::Droid::Content::Pm::IPackageInfo;
// using Elastos::Droid::Content::Pm::IPackageManager;
// using Elastos::Droid::Content::Pm::IResolveInfo;
// using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Content::Pm::IUserInfo;
// using Elastos::Droid::Graphics::IColorFilter;
// using Elastos::Droid::Graphics::IColorMatrix;
// using Elastos::Droid::Graphics::IColorMatrixColorFilter;
// using Elastos::Droid::Graphics::Drawable::IDrawable;
// using Elastos::Droid::Net::IUri;
// using Elastos::Droid::Os::IBuild;
// using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::Os::IUserManager;
// using Elastos::Droid::Preference::IPreference;
// using Elastos::Droid::Preference::IPreferenceGroup;
// using Elastos::Droid::Text::ITextUtils;
// using Elastos::Droid::Utility::ILog;
// using Elastos::Droid::View::IView;
// using Elastos::Droid::View::View::IOnClickListener;
// using Elastos::Droid::Widget::IImageView;
// using Elastos::Droid::Widget::IRadioButton;
// using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
// using Elastos::Droid::Settings::Search::IIndex;
// using Elastos::Droid::Settings::Search::IIndexable;
// using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
// using Elastos::Utility::IArrayList;
// using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

class HomeSettings
/*extends SettingsPreferenceFragment implements Indexable*/
    : public Object
{
   //  private class HomePackageReceiver extends BroadcastReceiver {
   //      //@Override
   //      CARAPI OnReceive(Context context, Intent intent) {
   //          BuildHomeActivitiesList();
   //          Index->GetInstance(context).UpdateFromClassNameResource(
   //                  HomeSettings.class->GetName(), TRUE, TRUE);
   //      }
   //  };

   //  public HomeSettings() {
   //      mHomeFilter = new IntentFilter(IIntent::ACTION_MAIN);
   //      mHomeFilter->AddCategory(Intent.CATEGORY_HOME);
   //      mHomeFilter->AddCategory(Intent.CATEGORY_DEFAULT);
   //  }

   //  OnClickListener mHomeClickListener = new OnClickListener() {
   //      //@Override
   //      CARAPI OnClick(View v) {
   //          Int32 index = (Integer)v->GetTag();
   //          HomeAppPreference pref = mPrefs->Get(index);
   //          if (!pref.isChecked) {
   //              MakeCurrentHome(pref);
   //          }
   //      }
   //  };

   //  OnClickListener mDeleteClickListener = new OnClickListener() {
   //      //@Override
   //      CARAPI OnClick(View v) {
   //          Int32 index = (Integer)v->GetTag();
   //          UninstallApp(mPrefs->Get(index));
   //      }
   //  };

   //  void MakeCurrentHome(HomeAppPreference newHome) {
   //      if (mCurrentHome != NULL) {
   //          mCurrentHome->SetChecked(FALSE);
   //      }
   //      newHome->SetChecked(TRUE);
   //      mCurrentHome = newHome;

   //      mPm->ReplacePreferredActivity(mHomeFilter, IntentFilter.MATCH_CATEGORY_EMPTY,
   //              mHomeComponentSet, newHome.activityName);

   //      GetActivity()->SetResult(Activity.RESULT_OK);
   //  }

   //  void UninstallApp(HomeAppPreference pref) {
   //      // Uninstallation is done by asking the OS to do it
   //     Uri packageURI = Uri->Parse("package:" + pref.uninstallTarget);
   //     Intent uninstallIntent = new Intent(IIntent::ACTION_UNINSTALL_PACKAGE, packageURI);
   //     uninstallIntent->PutExtra(Intent.EXTRA_UNINSTALL_ALL_USERS, FALSE);
   //     Int32 requestCode = REQUESTING_UNINSTALL + (pref.isChecked ? 1 : 0);
   //     StartActivityForResult(uninstallIntent, requestCode);
   // }

   //  //@Override
   //  CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
   //      super->OnActivityResult(requestCode, resultCode, data);

   //      // Rebuild the list now that we might have nuked something
   //      BuildHomeActivitiesList();

   //      // if the previous home app is now gone, fall back to the system one
   //      if (requestCode > REQUESTING_UNINSTALL) {
   //          // if mCurrentHome has gone NULL, it means we didn't find the previously-
   //          // default home app when rebuilding the list, i.e. it was the one we
   //          // just uninstalled.  When that happens we make the system-bundled
   //          // home app the active default.
   //          if (mCurrentHome == NULL) {
   //              for (Int32 i = 0; i < mPrefs->Size(); i++) {
   //                  HomeAppPreference pref = mPrefs->Get(i);
   //                  if (pref.isSystem) {
   //                      MakeCurrentHome(pref);
   //                      break;
   //                  }
   //              }
   //          }
   //      }

   //      // If we're down to just one possible home app, back out of this settings
   //      // fragment and show a dialog explaining to the user that they won't see
   //      // 'Home' settings now until such time as there are multiple available.
   //      if (mPrefs->Size() < 2) {
   //          if (mShowNotice) {
   //              mShowNotice = FALSE;
   //              SettingsActivity->RequestHomeNotice();
   //          }
   //          FinishFragment();
   //      }
   //  }

   //  private void BuildHomeActivitiesList() {
   //      ArrayList<ResolveInfo> homeActivities = new ArrayList<ResolveInfo>();
   //      ComponentName currentDefaultHome  = mPm->GetHomeActivities(homeActivities);

   //      Context context = GetActivity();
   //      mCurrentHome = NULL;
   //      mPrefGroup->RemoveAll();
   //      mPrefs = new ArrayList<HomeAppPreference>();
   //      mHomeComponentSet = new ComponentName[homeActivities->Size()];
   //      Int32 prefIndex = 0;
   //      Boolean supportManagedProfilesExtra =
   //              GetActivity()->GetIntent().GetBooleanExtra(EXTRA_SUPPORT_MANAGED_PROFILES, FALSE);
   //      Boolean mustSupportManagedProfile = HasManagedProfile()
   //              || supportManagedProfilesExtra;
   //      for (Int32 i = 0; i < homeActivities->Size(); i++) {
   //          final ResolveInfo candidate = homeActivities->Get(i);
   //          final ActivityInfo info = candidate.activityInfo;
   //          ComponentName activityName = new ComponentName(info.packageName, info.name);
   //          mHomeComponentSet[i] = activityName;
   //          try {
   //              Drawable icon = info->LoadIcon(mPm);
   //              CharSequence name = info->LoadLabel(mPm);
   //              HomeAppPreference pref;

   //              if (mustSupportManagedProfile && !LauncherHasManagedProfilesFeature(candidate)) {
   //                  pref = new HomeAppPreference(context, activityName, prefIndex,
   //                          icon, name, this, info, FALSE /* not enabled */,
   //                          GetResources()->GetString(R::string::home_work_profile_not_supported));
   //              } else  {
   //                  pref = new HomeAppPreference(context, activityName, prefIndex,
   //                          icon, name, this, info, TRUE /* enabled */, NULL);
   //              }

   //              mPrefs->Add(pref);
   //              mPrefGroup->AddPreference(pref);
   //              if (activityName->Equals(currentDefaultHome)) {
   //                  mCurrentHome = pref;
   //              }
   //              prefIndex++;
   //          } catch (Exception e) {
   //              Logger::V(TAG, "Problem dealing with activity " + activityName, e);
   //          }
   //      }

   //      if (mCurrentHome != NULL) {
   //          if (mCurrentHome->IsEnabled()) {
   //              GetActivity()->SetResult(Activity.RESULT_OK);
   //          }

   //          new Handler()->Post(new Runnable() {
   //             CARAPI Run() {
   //                 mCurrentHome->SetChecked(TRUE);
   //             }
   //          });
   //      }
   //  }

   //  private Boolean HasManagedProfile() {
   //      Context context = GetActivity();
   //      UserManager userManager = (UserManager) GetSystemService(Context.USER_SERVICE);
   //      List<UserInfo> profiles = userManager->GetProfiles(context->GetUserId());
   //      for (UserInfo userInfo : profiles) {
   //          if (userInfo->IsManagedProfile()) return TRUE;
   //      }
   //      return FALSE;
   //  }

   //  private Boolean LauncherHasManagedProfilesFeature(ResolveInfo resolveInfo) {
   //      try {
   //          ApplicationInfo appInfo = GetPackageManager()->GetApplicationInfo(
   //                  resolveInfo.activityInfo.packageName, 0 /* default flags */);
   //          return VersionNumberAtLeastL(appInfo.targetSdkVersion);
   //      } catch (PackageManager.NameNotFoundException e) {
   //          return FALSE;
   //      }
   //  }

   //  private Boolean VersionNumberAtLeastL(Int32 versionNumber) {
   //      return versionNumber >= Build.VERSION_CODES.LOLLIPOP;
   //  }

   //  //@Override
   //  CARAPI OnCreate(Bundle savedInstanceState) {
   //      super->OnCreate(savedInstanceState);
   //      AddPreferencesFromResource(R.xml.home_selection);

   //      mPm = GetPackageManager();
   //      mPrefGroup = (PreferenceGroup) FindPreference("home");

   //      Bundle args = GetArguments();
   //      mShowNotice = (args != NULL) && args->GetBoolean(HOME_SHOW_NOTICE, FALSE);
   //  }

   //  //@Override
   //  CARAPI OnResume() {
   //      super->OnResume();

   //      final IntentFilter filter = new IntentFilter(IIntent::ACTION_PACKAGE_ADDED);
   //      filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
   //      filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
   //      filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
   //      filter->AddDataScheme("package");
   //      GetActivity()->RegisterReceiver(mHomePackageReceiver, filter);

   //      BuildHomeActivitiesList();
   //  }

   //  //@Override
   //  CARAPI OnPause() {
   //      super->OnPause();
   //      GetActivity()->UnregisterReceiver(mHomePackageReceiver);
   //  }

   //  private class HomeAppPreference extends Preference {
   //      ComponentName activityName;
   //      Int32 index;
   //      HomeSettings fragment;
   //      final ColorFilter grayscaleFilter;
   //      Boolean isChecked;

   //      Boolean isSystem;
   //      String uninstallTarget;

   //      public HomeAppPreference(Context context, ComponentName activity,
   //              Int32 i, Drawable icon, CharSequence title, HomeSettings parent, ActivityInfo info,
   //              Boolean enabled, CharSequence summary) {
   //          Super(context);
   //          SetLayoutResource(R.layout.preference_home_app);
   //          SetIcon(icon);
   //          SetTitle(title);
   //          SetEnabled(enabled);
   //          SetSummary(summary);
   //          activityName = activity;
   //          fragment = parent;
   //          index = i;

   //          ColorMatrix colorMatrix = new ColorMatrix();
   //          colorMatrix->SetSaturation(0f);
   //          Float[] matrix = colorMatrix->GetArray();
   //          matrix[18] = 0.5f;
   //          grayscaleFilter = new ColorMatrixColorFilter(colorMatrix);

   //          DetermineTargets(info);
   //      }

   //      // Check whether this activity is bundled on the system, with awareness
   //      // of the META_HOME_ALTERNATE mechanism.
   //      private void DetermineTargets(ActivityInfo info) {
   //          final Bundle meta = info.metaData;
   //          if (meta != NULL) {
   //              final String altHomePackage = meta->GetString(ActivityManager.META_HOME_ALTERNATE);
   //              if (altHomePackage != NULL) {
   //                  try {
   //                      final Int32 match = mPm->CheckSignatures(info.packageName, altHomePackage);
   //                      if (match >= PackageManager.SIGNATURE_MATCH) {
   //                          PackageInfo altInfo = mPm->GetPackageInfo(altHomePackage, 0);
   //                          final Int32 altFlags = altInfo.applicationInfo.flags;
   //                          isSystem = (altFlags & ApplicationInfo.FLAG_SYSTEM) != 0;
   //                          uninstallTarget = altInfo.packageName;
   //                          return;
   //                      }
   //                  } catch (Exception e) {
   //                      // e.g. named alternate package not found during lookup
   //                      Logger::W(TAG, "Unable to compare/resolve alternate", e);
   //                  }
   //              }
   //          }
   //          // No suitable metadata redirect, so use the package's own info
   //          isSystem = (info.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0;
   //          uninstallTarget = info.packageName;
   //      }

   //      //@Override
   //      protected void OnBindView(View view) {
   //          super->OnBindView(view);

   //          RadioButton radio = (RadioButton) view->FindViewById(R.id.home_radio);
   //          radio->SetChecked(isChecked);

   //          Integer indexObj = new Integer(index);

   //          ImageView icon = (ImageView) view->FindViewById(R.id.home_app_uninstall);
   //          if (isSystem) {
   //              icon->SetEnabled(FALSE);
   //              icon->SetColorFilter(grayscaleFilter);
   //          } else {
   //              icon->SetEnabled(TRUE);
   //              icon->SetOnClickListener(mDeleteClickListener);
   //              icon->SetTag(indexObj);
   //          }

   //          View v = view->FindViewById(R.id.home_app_pref);
   //          v->SetTag(indexObj);

   //          v->SetOnClickListener(mHomeClickListener);
   //      }

   //      void SetChecked(Boolean state) {
   //          if (state != isChecked) {
   //              isChecked = state;
   //              NotifyChanged();
   //          }
   //      }
   //  };

   //  /**
   //   * For search
   //   */
   //  public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
   //      new BaseSearchIndexProvider() {
   //          //@Override
   //          public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
   //              final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

   //              final PackageManager pm = context->GetPackageManager();
   //              final ArrayList<ResolveInfo> homeActivities = new ArrayList<ResolveInfo>();
   //              pm->GetHomeActivities(homeActivities);

   //              final SharedPreferences sp = context->GetSharedPreferences(
   //                      HomeSettings.HOME_PREFS, Context.MODE_PRIVATE);
   //              final Boolean doShowHome = sp->GetBoolean(HomeSettings.HOME_PREFS_DO_SHOW, FALSE);

   //              // We index Home Launchers only if there are more than one or if we are showing the
   //              // Home tile into the Dashboard
   //              if (homeActivities->Size() > 1 || doShowHome) {
   //                  final Resources res = context->GetResources();

   //                  // Add fragment title
   //                  SearchIndexableRaw data = new SearchIndexableRaw(context);
   //                  data.title = res->GetString(R::string::home_settings);
   //                  data.screenTitle = res->GetString(R::string::home_settings);
   //                  data.keywords = res->GetString(R::string::keywords_home);
   //                  result->Add(data);

   //                  for (Int32 i = 0; i < homeActivities->Size(); i++) {
   //                      final ResolveInfo resolveInfo = homeActivities->Get(i);
   //                      final ActivityInfo activityInfo = resolveInfo.activityInfo;

   //                      CharSequence name;
   //                      try {
   //                          name = activityInfo->LoadLabel(pm);
   //                          if (TextUtils->IsEmpty(name)) {
   //                              continue;
   //                          }
   //                      } catch (Exception e) {
   //                          Logger::V(TAG, "Problem dealing with Home " + activityInfo.name, e);
   //                          continue;
   //                      }

   //                      data = new SearchIndexableRaw(context);
   //                      data.title = name->ToString();
   //                      data.screenTitle = res->GetString(R::string::home_settings);
   //                      result->Add(data);
   //                  }
   //              }

   //              return result;
   //          }
   //      };

public:
    static const String HOME_PREFS;
    static const String HOME_PREFS_DO_SHOW;
    static const String HOME_SHOW_NOTICE;

    // static const String TAG = "HomeSettings";
    // static const Int32 REQUESTING_UNINSTALL = 10;

private:
    // // Boolean extra, indicates only launchers that support managed profiles should be shown.
    // // Note: must match the constant defined in ManagedProvisioning
    // private static const String EXTRA_SUPPORT_MANAGED_PROFILES = "support_managed_profiles";

    // private PreferenceGroup mPrefGroup;
    // private PackageManager mPm;
    // private ComponentName[] mHomeComponentSet;
    // private ArrayList<HomeAppPreference> mPrefs;
    // private HomeAppPreference mCurrentHome = NULL;
    // private final IntentFilter mHomeFilter;
    // private Boolean mShowNotice;
    // private HomePackageReceiver mHomePackageReceiver = new HomePackageReceiver();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_HOMESETTINGS_H__
