

package com.android.settings.notification;

using static com::Android::Settings::Notification::AppNotificationSettings::IEXTRA_HAS_SETTINGS_INTENT;
using static com::Android::Settings::Notification::AppNotificationSettings::IEXTRA_SETTINGS_INTENT;

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::ILauncherActivityInfo;
using Elastos::Droid::Content::Pm::ILauncherApps;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IPinnedHeaderListFragment;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Settings::INotificationAppListActivity;
using Elastos::Droid::Settings::IUserSpinnerAdapter;
using Elastos::Droid::Settings::IUtils;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

/** Just a sectioned list of installed applications, nothing else to index **/
public class NotificationAppList extends PinnedHeaderListFragment
        implements OnItemSelectedListener {
    private static const String TAG = "NotificationAppList";
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    private static const String EMPTY_SUBTITLE = "";
    private static const String SECTION_BEFORE_A = "*";
    private static const String SECTION_AFTER_Z = "**";
    private static const Intent APP_NOTIFICATION_PREFS_CATEGORY_INTENT
            = new Intent(IIntent::ACTION_MAIN)
                .AddCategory(Notification.INTENT_CATEGORY_NOTIFICATION_PREFERENCES);

    private final Handler mHandler = new Handler();
    private final ArrayMap<String, AppRow> mRows = new ArrayMap<String, AppRow>();
    private final ArrayList<AppRow> mSortedRows = new ArrayList<AppRow>();
    private final ArrayList<String> mSections = new ArrayList<String>();

    private Context mContext;
    private LayoutInflater mInflater;
    private NotificationAppAdapter mAdapter;
    private Signature[] mSystemSignature;
    private Parcelable mListViewState;
    private Backend mBackend = new Backend();
    private UserSpinnerAdapter mProfileSpinnerAdapter;

    private PackageManager mPM;
    private UserManager mUM;
    private LauncherApps mLauncherApps;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mContext = GetActivity();
        mInflater = (LayoutInflater) mContext->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mAdapter = new NotificationAppAdapter(mContext);
        mUM = UserManager->Get(mContext);
        mPM = mContext->GetPackageManager();
        mLauncherApps = (LauncherApps) mContext->GetSystemService(Context.LAUNCHER_APPS_SERVICE);
        GetActivity()->SetTitle(R::string::app_notifications_title);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        return inflater->Inflate(R.layout.notification_app_list, container, FALSE);
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);
        mProfileSpinnerAdapter = Utils->CreateUserSpinnerAdapter(mUM, mContext);
        if (mProfileSpinnerAdapter != NULL) {
            Spinner spinner = (Spinner) GetActivity()->GetLayoutInflater().Inflate(
                    R.layout.spinner_view, NULL);
            spinner->SetAdapter(mProfileSpinnerAdapter);
            spinner->SetOnItemSelectedListener(this);
            SetPinnedHeaderView(spinner);
        }
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        RepositionScrollbar();
        GetListView()->SetAdapter(mAdapter);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        if (DEBUG) Logger::D(TAG, "Saving listView state");
        mListViewState = GetListView()->OnSaveInstanceState();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        mListViewState = NULL;  // you're dead to me
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        LoadAppsList();
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        UserHandle selectedUser = mProfileSpinnerAdapter->GetUserHandle(position);
        if (selectedUser->GetIdentifier() != UserHandle->MyUserId()) {
            Intent intent = new Intent(GetActivity(), NotificationAppListActivity.class);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
            mContext->StartActivityAsUser(intent, selectedUser);
        }
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
    }

    CARAPI SetBackend(Backend backend) {
        mBackend = backend;
    }

    private void LoadAppsList() {
        AsyncTask->Execute(mCollectAppsRunnable);
    }

    private String GetSection(CharSequence label) {
        if (label == NULL || label->Length() == 0) return SECTION_BEFORE_A;
        final Char32 c = Character->ToUpperCase(label->CharAt(0));
        if (c < 'A') return SECTION_BEFORE_A;
        if (c > 'Z') return SECTION_AFTER_Z;
        return Character->ToString(c);
    }

    private void RepositionScrollbar() {
        final Int32 sbWidthPx = (Int32) TypedValue->ApplyDimension(TypedValue.COMPLEX_UNIT_DIP,
                GetListView()->GetScrollBarSize(),
                GetResources()->GetDisplayMetrics());
        final View parent = (View)GetView()->GetParent();
        final Int32 eat = Math->Min(sbWidthPx, parent->GetPaddingEnd());
        if (eat <= 0) return;
        if (DEBUG) Logger::D(TAG, String->Format("Eating %dpx into %dpx padding for %dpx scroll, ld=%d",
                eat, parent->GetPaddingEnd(), sbWidthPx, GetListView()->GetLayoutDirection()));
        parent->SetPaddingRelative(parent->GetPaddingStart(), parent->GetPaddingTop(),
                parent->GetPaddingEnd() - eat, parent->GetPaddingBottom());
    }

    private static class ViewHolder {
        ViewGroup row;
        ImageView icon;
        TextView title;
        TextView subtitle;
        View rowDivider;
    }

    private class NotificationAppAdapter extends ArrayAdapter<Row> implements SectionIndexer {
        public NotificationAppAdapter(Context context) {
            Super(context, 0, 0);
        }

        //@Override
        public Boolean HasStableIds() {
            return TRUE;
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        //@Override
        public Int32 GetViewTypeCount() {
            return 2;
        }

        //@Override
        public Int32 GetItemViewType(Int32 position) {
            Row r = GetItem(position);
            return r instanceof AppRow ? 1 : 0;
        }

        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            Row r = GetItem(position);
            View v;
            if (convertView == NULL) {
                v = NewView(parent, r);
            } else {
                v = convertView;
            }
            BindView(v, r, FALSE /*animate*/);
            return v;
        }

        public View NewView(ViewGroup parent, Row r) {
            if (!(r instanceof AppRow)) {
                return mInflater->Inflate(R.layout.notification_app_section, parent, FALSE);
            }
            final View v = mInflater->Inflate(R.layout.notification_app, parent, FALSE);
            final ViewHolder vh = new ViewHolder();
            vh.row = (ViewGroup) v;
            vh.row->SetLayoutTransition(new LayoutTransition());
            vh.row->SetLayoutTransition(new LayoutTransition());
            vh.icon = (ImageView) v->FindViewById(android.R.id.icon);
            vh.title = (TextView) v->FindViewById(android.R.id.title);
            vh.subtitle = (TextView) v->FindViewById(android.R.id.text1);
            vh.rowDivider = v->FindViewById(R.id.row_divider);
            v->SetTag(vh);
            return v;
        }

        private void EnableLayoutTransitions(ViewGroup vg, Boolean enabled) {
            if (enabled) {
                vg->GetLayoutTransition()->EnableTransitionType(LayoutTransition.APPEARING);
                vg->GetLayoutTransition()->EnableTransitionType(LayoutTransition.DISAPPEARING);
            } else {
                vg->GetLayoutTransition()->DisableTransitionType(LayoutTransition.APPEARING);
                vg->GetLayoutTransition()->DisableTransitionType(LayoutTransition.DISAPPEARING);
            }
        }

        CARAPI BindView(final View view, Row r, Boolean animate) {
            if (!(r instanceof AppRow)) {
                // it's a section row
                final TextView tv = (TextView)view->FindViewById(android.R.id.title);
                tv->SetText(r.section);
                return;
            }

            final AppRow row = (AppRow)r;
            final ViewHolder vh = (ViewHolder) view->GetTag();
            EnableLayoutTransitions(vh.row, animate);
            vh.rowDivider->SetVisibility(row.first ? View.GONE : View.VISIBLE);
            vh.row->SetOnClickListener(new OnClickListener() {
                //@Override
                CARAPI OnClick(View v) {
                    mContext->StartActivity(new Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS)
                            .AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP)
                            .PutExtra(Settings.EXTRA_APP_PACKAGE, row.pkg)
                            .PutExtra(Settings.EXTRA_APP_UID, row.uid)
                            .PutExtra(EXTRA_HAS_SETTINGS_INTENT, row.settingsIntent != NULL)
                            .PutExtra(EXTRA_SETTINGS_INTENT, row.settingsIntent));
                }
            });
            EnableLayoutTransitions(vh.row, animate);
            vh.icon->SetImageDrawable(row.icon);
            vh.title->SetText(row.label);
            final String sub = GetSubtitle(row);
            vh.subtitle->SetText(sub);
            vh.subtitle->SetVisibility(!sub->IsEmpty() ? View.VISIBLE : View.GONE);
        }

        private String GetSubtitle(AppRow row) {
            if (row.banned) {
                return mContext->GetString(R::string::app_notification_row_banned);
            }
            if (!row.priority && !row.sensitive) {
                return EMPTY_SUBTITLE;
            }
            final String priString = mContext->GetString(R::string::app_notification_row_priority);
            final String senString = mContext->GetString(R::string::app_notification_row_sensitive);
            if (row.priority != row.sensitive) {
                return row.priority ? priString : senString;
            }
            return priString + mContext->GetString(R::string::summary_divider_text) + senString;
        }

        //@Override
        public Object[] GetSections() {
            return mSections->ToArray(new Object[mSections->Size()]);
        }

        //@Override
        public Int32 GetPositionForSection(Int32 sectionIndex) {
            final String section = mSections->Get(sectionIndex);
            final Int32 n = GetCount();
            for (Int32 i = 0; i < n; i++) {
                final Row r = GetItem(i);
                if (r.section->Equals(section)) {
                    return i;
                }
            }
            return 0;
        }

        //@Override
        public Int32 GetSectionForPosition(Int32 position) {
            Row row = GetItem(position);
            return mSections->IndexOf(row.section);
        }
    }

    private static class Row {
        public String section;
    }

    public static class AppRow extends Row {
        public String pkg;
        public Int32 uid;
        public Drawable icon;
        public CharSequence label;
        public Intent settingsIntent;
        public Boolean banned;
        public Boolean priority;
        public Boolean sensitive;
        public Boolean first;  // first app in section
    }

    private static const Comparator<AppRow> mRowComparator = new Comparator<AppRow>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppRow lhs, AppRow rhs) {
            return sCollator->Compare(lhs.label, rhs.label);
        }
    };


    public static AppRow LoadAppRow(PackageManager pm, ApplicationInfo app,
            Backend backend) {
        final AppRow row = new AppRow();
        row.pkg = app.packageName;
        row.uid = app.uid;
        try {
            row.label = app->LoadLabel(pm);
        } catch (Throwable t) {
            Logger::E(TAG, "Error loading application label for " + row.pkg, t);
            row.label = row.pkg;
        }
        row.icon = app->LoadIcon(pm);
        row.banned = backend->GetNotificationsBanned(row.pkg, row.uid);
        row.priority = backend->GetHighPriority(row.pkg, row.uid);
        row.sensitive = backend->GetSensitive(row.pkg, row.uid);
        return row;
    }

    public static List<ResolveInfo> QueryNotificationConfigActivities(PackageManager pm) {
        if (DEBUG) Logger::D(TAG, "APP_NOTIFICATION_PREFS_CATEGORY_INTENT is "
                + APP_NOTIFICATION_PREFS_CATEGORY_INTENT);
        final List<ResolveInfo> resolveInfos = pm->QueryIntentActivities(
                APP_NOTIFICATION_PREFS_CATEGORY_INTENT,
                0 //PackageManager.MATCH_DEFAULT_ONLY
        );
        return resolveInfos;
    }
    public static void CollectConfigActivities(PackageManager pm, ArrayMap<String, AppRow> rows) {
        final List<ResolveInfo> resolveInfos = QueryNotificationConfigActivities(pm);
        ApplyConfigActivities(pm, rows, resolveInfos);
    }

    public static void ApplyConfigActivities(PackageManager pm, ArrayMap<String, AppRow> rows,
            List<ResolveInfo> resolveInfos) {
        if (DEBUG) Logger::D(TAG, "Found " + resolveInfos->Size() + " preference activities"
                + (resolveInfos->Size() == 0 ? " ;_;" : ""));
        for (ResolveInfo ri : resolveInfos) {
            final ActivityInfo activityInfo = ri.activityInfo;
            final ApplicationInfo appInfo = activityInfo.applicationInfo;
            final AppRow row = rows->Get(appInfo.packageName);
            if (row == NULL) {
                Logger::V(TAG, "Ignoring notification preference activity ("
                        + activityInfo.name + ") for unknown package "
                        + activityInfo.packageName);
                continue;
            }
            if (row.settingsIntent != NULL) {
                Logger::V(TAG, "Ignoring duplicate notification preference activity ("
                        + activityInfo.name + ") for package "
                        + activityInfo.packageName);
                continue;
            }
            row.settingsIntent = new Intent(APP_NOTIFICATION_PREFS_CATEGORY_INTENT)
                    .SetClassName(activityInfo.packageName, activityInfo.name);
        }
    }

    private final Runnable mCollectAppsRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            synchronized(mRows) {
                final Int64 start = SystemClock->UptimeMillis();
                if (DEBUG) Logger::D(TAG, "Collecting apps...");
                mRows->Clear();
                mSortedRows->Clear();

                // collect all launchable apps, plus any packages that have notification settings
                final List<ApplicationInfo> appInfos = new ArrayList<ApplicationInfo>();

                final List<LauncherActivityInfo> lais
                        = mLauncherApps->GetActivityList(NULL /* all */,
                            UserHandle->GetCallingUserHandle());
                if (DEBUG) Logger::D(TAG, "  launchable activities:");
                for (LauncherActivityInfo lai : lais) {
                    if (DEBUG) Logger::D(TAG, "    " + lai->GetComponentName()->ToString());
                    appInfos->Add(lai->GetApplicationInfo());
                }

                final List<ResolveInfo> resolvedConfigActivities
                        = QueryNotificationConfigActivities(mPM);
                if (DEBUG) Logger::D(TAG, "  config activities:");
                for (ResolveInfo ri : resolvedConfigActivities) {
                    if (DEBUG) Logger::D(TAG, "    "
                            + ri.activityInfo.packageName + "/" + ri.activityInfo.name);
                    appInfos->Add(ri.activityInfo.applicationInfo);
                }

                for (ApplicationInfo info : appInfos) {
                    final String key = info.packageName;
                    if (mRows->ContainsKey(key)) {
                        // we already have this app, thanks
                        continue;
                    }

                    final AppRow row = LoadAppRow(mPM, info, mBackend);
                    mRows->Put(key, row);
                }

                // add config activities to the list
                ApplyConfigActivities(mPM, mRows, resolvedConfigActivities);

                // sort rows
                mSortedRows->AddAll(mRows->Values());
                Collections->Sort(mSortedRows, mRowComparator);
                // compute sections
                mSections->Clear();
                String section = NULL;
                for (AppRow r : mSortedRows) {
                    r.section = GetSection(r.label);
                    if (!r.section->Equals(section)) {
                        section = r.section;
                        mSections->Add(section);
                    }
                }
                mHandler->Post(mRefreshAppsListRunnable);
                final Int64 elapsed = SystemClock->UptimeMillis() - start;
                if (DEBUG) Logger::D(TAG, "Collected " + mRows->Size() + " apps in " + elapsed + "ms");
            }
        }
    };

    private void RefreshDisplayedItems() {
        if (DEBUG) Logger::D(TAG, "Refreshing apps...");
        mAdapter->Clear();
        synchronized(mSortedRows) {
            String section = NULL;
            final Int32 N = mSortedRows->Size();
            Boolean first = TRUE;
            for (Int32 i = 0; i < N; i++) {
                final AppRow row = mSortedRows->Get(i);
                if (!row.section->Equals(section)) {
                    section = row.section;
                    Row r = new Row();
                    r.section = section;
                    mAdapter->Add(r);
                    first = TRUE;
                }
                row.first = first;
                mAdapter->Add(row);
                first = FALSE;
            }
        }
        if (mListViewState != NULL) {
            if (DEBUG) Logger::D(TAG, "Restoring listView state");
            GetListView()->OnRestoreInstanceState(mListViewState);
            mListViewState = NULL;
        }
        if (DEBUG) Logger::D(TAG, "Refreshed " + mSortedRows->Size() + " displayed items");
    }

    private final Runnable mRefreshAppsListRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            RefreshDisplayedItems();
        }
    };

    public static class Backend {
        static INotificationManager sINM = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));

        public Boolean SetNotificationsBanned(String pkg, Int32 uid, Boolean banned) {
            try {
                sINM->SetNotificationsEnabledForPackage(pkg, uid, !banned);
                return TRUE;
            } catch (Exception e) {
               Logger::W(TAG, "Error calling NoMan", e);
               return FALSE;
            }
        }

        public Boolean GetNotificationsBanned(String pkg, Int32 uid) {
            try {
                final Boolean enabled = sINM->AreNotificationsEnabledForPackage(pkg, uid);
                return !enabled;
            } catch (Exception e) {
                Logger::W(TAG, "Error calling NoMan", e);
                return FALSE;
            }
        }

        public Boolean GetHighPriority(String pkg, Int32 uid) {
            try {
                return sINM->GetPackagePriority(pkg, uid) == Notification.PRIORITY_MAX;
            } catch (Exception e) {
                Logger::W(TAG, "Error calling NoMan", e);
                return FALSE;
            }
        }

        public Boolean SetHighPriority(String pkg, Int32 uid, Boolean highPriority) {
            try {
                sINM->SetPackagePriority(pkg, uid,
                        highPriority ? Notification.PRIORITY_MAX : Notification.PRIORITY_DEFAULT);
                return TRUE;
            } catch (Exception e) {
                Logger::W(TAG, "Error calling NoMan", e);
                return FALSE;
            }
        }

        public Boolean GetSensitive(String pkg, Int32 uid) {
            try {
                return sINM->GetPackageVisibilityOverride(pkg, uid) == Notification.VISIBILITY_PRIVATE;
            } catch (Exception e) {
                Logger::W(TAG, "Error calling NoMan", e);
                return FALSE;
            }
        }

        public Boolean SetSensitive(String pkg, Int32 uid, Boolean sensitive) {
            try {
                sINM->SetPackageVisibilityOverride(pkg, uid,
                        sensitive ? Notification.VISIBILITY_PRIVATE
                                : NotificationListenerService.Ranking.VISIBILITY_NO_OVERRIDE);
                return TRUE;
            } catch (Exception e) {
                Logger::W(TAG, "Error calling NoMan", e);
                return FALSE;
            }
        }
    }
}
