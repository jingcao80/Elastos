/**
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::Usage::IUsageStats;
using Elastos::Droid::App::Usage::IUsageStatsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;

using Elastos::Text::IDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;

/**
 * Activity to display package usage statistics.
 */
public class UsageStatsActivity extends Activity implements OnItemSelectedListener {
    private static const String TAG = "UsageStatsActivity";
    private static const Boolean localLOGV = FALSE;
    private UsageStatsManager mUsageStatsManager;
    private LayoutInflater mInflater;
    private UsageStatsAdapter mAdapter;
    private PackageManager mPm;

    public static class AppNameComparator implements Comparator<UsageStats> {
        private Map<String, String> mAppLabelList;

        AppNameComparator(Map<String, String> appList) {
            mAppLabelList = appList;
        }

        //@Override
        public final Int32 Compare(UsageStats a, UsageStats b) {
            String alabel = mAppLabelList->Get(a->GetPackageName());
            String blabel = mAppLabelList->Get(b->GetPackageName());
            return alabel->CompareTo(blabel);
        }
    }

    public static class LastTimeUsedComparator implements Comparator<UsageStats> {
        //@Override
        public final Int32 Compare(UsageStats a, UsageStats b) {
            // return by descending order
            return (Int32)(b->GetLastTimeUsed() - a->GetLastTimeUsed());
        }
    }

    public static class UsageTimeComparator implements Comparator<UsageStats> {
        //@Override
        public final Int32 Compare(UsageStats a, UsageStats b) {
            return (Int32)(b->GetTotalTimeInForeground() - a->GetTotalTimeInForeground());
        }
    }

    // View Holder used when displaying views
    static class AppViewHolder {
        TextView pkgName;
        TextView lastTimeUsed;
        TextView usageTime;
    }

    class UsageStatsAdapter extends BaseAdapter {
         // Constants defining order for display order
        private static const Int32 _DISPLAY_ORDER_USAGE_TIME = 0;
        private static const Int32 _DISPLAY_ORDER_LAST_TIME_USED = 1;
        private static const Int32 _DISPLAY_ORDER_APP_NAME = 2;

        private Int32 mDisplayOrder = _DISPLAY_ORDER_USAGE_TIME;
        private LastTimeUsedComparator mLastTimeUsedComparator = new LastTimeUsedComparator();
        private UsageTimeComparator mUsageTimeComparator = new UsageTimeComparator();
        private AppNameComparator mAppLabelComparator;
        private final ArrayMap<String, String> mAppLabelMap = new ArrayMap<>();
        private final ArrayList<UsageStats> mPackageStats = new ArrayList<>();

        UsageStatsAdapter() {
            Calendar cal = Calendar->GetInstance();
            cal->Add(Calendar.DAY_OF_YEAR, -5);

            final List<UsageStats> stats =
                    mUsageStatsManager->QueryUsageStats(UsageStatsManager.INTERVAL_BEST,
                            cal->GetTimeInMillis(), System->CurrentTimeMillis());
            if (stats == NULL) {
                return;
            }

            ArrayMap<String, UsageStats> map = new ArrayMap<>();
            final Int32 statCount = stats->Size();
            for (Int32 i = 0; i < statCount; i++) {
                final android.app.usage.UsageStats pkgStats = stats->Get(i);

                // load application labels for each application
                try {
                    ApplicationInfo appInfo = mPm->GetApplicationInfo(pkgStats->GetPackageName(), 0);
                    String label = appInfo->LoadLabel(mPm).ToString();
                    mAppLabelMap->Put(pkgStats->GetPackageName(), label);

                    UsageStats existingStats =
                            map->Get(pkgStats->GetPackageName());
                    if (existingStats == NULL) {
                        map->Put(pkgStats->GetPackageName(), pkgStats);
                    } else {
                        existingStats->Add(pkgStats);
                    }

                } catch (NameNotFoundException e) {
                    // This package may be gone.
                }
            }
            mPackageStats->AddAll(map->Values());

            // Sort list
            mAppLabelComparator = new AppNameComparator(mAppLabelMap);
            SortList();
        }

        //@Override
        public Int32 GetCount() {
            return mPackageStats->Size();
        }

        //@Override
        public Object GetItem(Int32 position) {
            return mPackageStats->Get(position);
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            // A ViewHolder keeps references to children views to avoid unneccessary calls
            // to FindViewById() on each row.
            AppViewHolder holder;

            // When convertView is not NULL, we can reuse it directly, there is no need
            // to reinflate it. We only inflate a new View when the convertView supplied
            // by ListView is NULL.
            if (convertView == NULL) {
                convertView = mInflater->Inflate(R.layout.usage_stats_item, NULL);

                // Creates a ViewHolder and store references to the two children views
                // we want to bind data to.
                holder = new AppViewHolder();
                holder.pkgName = (TextView) convertView->FindViewById(R.id.package_name);
                holder.lastTimeUsed = (TextView) convertView->FindViewById(R.id.last_time_used);
                holder.usageTime = (TextView) convertView->FindViewById(R.id.usage_time);
                convertView->SetTag(holder);
            } else {
                // Get the ViewHolder back to get fast access to the TextView
                // and the ImageView.
                holder = (AppViewHolder) convertView->GetTag();
            }

            // Bind the data efficiently with the holder
            UsageStats pkgStats = mPackageStats->Get(position);
            if (pkgStats != NULL) {
                String label = mAppLabelMap->Get(pkgStats->GetPackageName());
                holder.pkgName->SetText(label);
                holder.lastTimeUsed->SetText(DateUtils->FormatSameDayTime(pkgStats->GetLastTimeUsed(),
                        System->CurrentTimeMillis(), DateFormat.MEDIUM, DateFormat.MEDIUM));
                holder.usageTime->SetText(
                        DateUtils->FormatElapsedTime(pkgStats->GetTotalTimeInForeground() / 1000));
            } else {
                Logger::W(TAG, "No usage stats info for package:" + position);
            }
            return convertView;
        }

        void SortList(Int32 sortOrder) {
            if (mDisplayOrder == sortOrder) {
                // do nothing
                return;
            }
            mDisplayOrder= sortOrder;
            SortList();
        }
        private void SortList() {
            if (mDisplayOrder == _DISPLAY_ORDER_USAGE_TIME) {
                if (localLOGV) Logger::I(TAG, "Sorting by usage time");
                Collections->Sort(mPackageStats, mUsageTimeComparator);
            } else if (mDisplayOrder == _DISPLAY_ORDER_LAST_TIME_USED) {
                if (localLOGV) Logger::I(TAG, "Sorting by last time used");
                Collections->Sort(mPackageStats, mLastTimeUsedComparator);
            } else if (mDisplayOrder == _DISPLAY_ORDER_APP_NAME) {
                if (localLOGV) Logger::I(TAG, "Sorting by application name");
                Collections->Sort(mPackageStats, mAppLabelComparator);
            }
            NotifyDataSetChanged();
        }
    }

    /** Called when the activity is first created. */
    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        SetContentView(R.layout.usage_stats);

        mUsageStatsManager = (UsageStatsManager) GetSystemService(Context.USAGE_STATS_SERVICE);
        mInflater = (LayoutInflater)GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mPm = GetPackageManager();

        Spinner typeSpinner = (Spinner) FindViewById(R.id.typeSpinner);
        typeSpinner->SetOnItemSelectedListener(this);

        ListView listView = (ListView) FindViewById(R.id.pkg_list);
        mAdapter = new UsageStatsAdapter();
        listView->SetAdapter(mAdapter);
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        mAdapter->SortList(position);
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
        // do nothing
    }
}
