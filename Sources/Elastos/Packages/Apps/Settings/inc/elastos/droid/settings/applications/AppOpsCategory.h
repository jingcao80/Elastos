/**
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.settings.applications;

using Elastos::Droid::App::IListFragment;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Content::IAsyncTaskLoader;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IList;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Applications::AppOpsState::IAppOpEntry;

public class AppOpsCategory extends ListFragment implements
        LoaderManager.LoaderCallbacks<List<AppOpEntry>> {

    private static const Int32 RESULT_APP_DETAILS = 1;

    AppOpsState mState;

    // This is the Adapter being used to display the list's data.
    AppListAdapter mAdapter;

    String mCurrentPkgName;

    public AppOpsCategory() {
    }

    public AppOpsCategory(AppOpsState.OpsTemplate template) {
        Bundle args = new Bundle();
        args->PutParcelable("template", template);
        SetArguments(args);
    }

    /**
     * Helper for determining if the configuration has changed in an interesting
     * way so we need to rebuild the app list.
     */
    public static class InterestingConfigChanges {
        final Configuration mLastConfiguration = new Configuration();
        Int32 mLastDensity;

        Boolean ApplyNewConfig(Resources res) {
            Int32 configChanges = mLastConfiguration->UpdateFrom(res->GetConfiguration());
            Boolean densityChanged = mLastDensity != res->GetDisplayMetrics().densityDpi;
            if (densityChanged || (configChanges&(ActivityInfo.CONFIG_LOCALE
                    |ActivityInfo.CONFIG_UI_MODE|ActivityInfo.CONFIG_SCREEN_LAYOUT)) != 0) {
                mLastDensity = res->GetDisplayMetrics().densityDpi;
                return TRUE;
            }
            return FALSE;
        }
    }

    /**
     * Helper class to look for interesting changes to the installed apps
     * so that the loader can be updated.
     */
    public static class PackageIntentReceiver extends BroadcastReceiver {
        final AppListLoader mLoader;

        public PackageIntentReceiver(AppListLoader loader) {
            mLoader = loader;
            IntentFilter filter = new IntentFilter(IIntent::ACTION_PACKAGE_ADDED);
            filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
            filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
            filter->AddDataScheme("package");
            mLoader->GetContext()->RegisterReceiver(this, filter);
            // Register for events related to sdcard installation.
            IntentFilter sdFilter = new IntentFilter();
            sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
            sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
            mLoader->GetContext()->RegisterReceiver(this, sdFilter);
        }

        //@Override CARAPI OnReceive(Context context, Intent intent) {
            // Tell the loader about the change.
            mLoader->OnContentChanged();
        }
    }

    /**
     * A custom Loader that loads all of the installed applications.
     */
    public static class AppListLoader extends AsyncTaskLoader<List<AppOpEntry>> {
        final InterestingConfigChanges mLastConfig = new InterestingConfigChanges();
        final AppOpsState mState;
        final AppOpsState.OpsTemplate mTemplate;

        List<AppOpEntry> mApps;
        PackageIntentReceiver mPackageObserver;

        public AppListLoader(Context context, AppOpsState state, AppOpsState.OpsTemplate template) {
            Super(context);
            mState = state;
            mTemplate = template;
        }

        //@Override public List<AppOpEntry> LoadInBackground() {
            return mState->BuildState(mTemplate);
        }

        /**
         * Called when there is new data to deliver to the client.  The
         * super class will take care of delivering it; the implementation
         * here just adds a little more logic.
         */
        //@Override CARAPI DeliverResult(List<AppOpEntry> apps) {
            if (IsReset()) {
                // An async query came in while the loader is stopped.  We
                // don't need the result.
                if (apps != NULL) {
                    OnReleaseResources(apps);
                }
            }
            List<AppOpEntry> oldApps = apps;
            mApps = apps;

            if (IsStarted()) {
                // If the Loader is currently started, we can immediately
                // deliver its results.
                super->DeliverResult(apps);
            }

            // At this point we can release the resources associated with
            // 'oldApps' if needed; now that the new result is delivered we
            // know that it is no longer in use.
            if (oldApps != NULL) {
                OnReleaseResources(oldApps);
            }
        }

        /**
         * Handles a request to start the Loader.
         */
        //@Override protected void OnStartLoading() {
            // We don't monitor changed when loading is stopped, so need
            // to always reload at this point.
            OnContentChanged();

            if (mApps != NULL) {
                // If we currently have a result available, deliver it
                // immediately.
                DeliverResult(mApps);
            }

            // Start watching for changes in the app data.
            if (mPackageObserver == NULL) {
                mPackageObserver = new PackageIntentReceiver(this);
            }

            // Has something interesting in the configuration changed since we
            // last built the app list?
            Boolean configChange = mLastConfig->ApplyNewConfig(GetContext()->GetResources());

            if (TakeContentChanged() || mApps == NULL || configChange) {
                // If the data has changed since the last time it was loaded
                // or is not currently available, start a load.
                ForceLoad();
            }
        }

        /**
         * Handles a request to stop the Loader.
         */
        //@Override protected void OnStopLoading() {
            // Attempt to cancel the current load task if possible.
            CancelLoad();
        }

        /**
         * Handles a request to cancel a load.
         */
        //@Override CARAPI OnCanceled(List<AppOpEntry> apps) {
            super->OnCanceled(apps);

            // At this point we can release the resources associated with 'apps'
            // if needed.
            OnReleaseResources(apps);
        }

        /**
         * Handles a request to completely reset the Loader.
         */
        //@Override protected void OnReset() {
            super->OnReset();

            // Ensure the loader is stopped
            OnStopLoading();

            // At this point we can release the resources associated with 'apps'
            // if needed.
            if (mApps != NULL) {
                OnReleaseResources(mApps);
                mApps = NULL;
            }

            // Stop monitoring for changes.
            if (mPackageObserver != NULL) {
                GetContext()->UnregisterReceiver(mPackageObserver);
                mPackageObserver = NULL;
            }
        }

        /**
         * Helper function to take care of releasing resources associated
         * with an actively loaded data set.
         */
        protected void OnReleaseResources(List<AppOpEntry> apps) {
            // For a simple List<> there is nothing to do.  For something
            // like a Cursor, we would close it here.
        }
    }

    public static class AppListAdapter extends BaseAdapter {
        private final Resources mResources;
        private final LayoutInflater mInflater;
        private final AppOpsState mState;

        List<AppOpEntry> mList;

        public AppListAdapter(Context context, AppOpsState state) {
            mResources = context->GetResources();
            mInflater = (LayoutInflater)context->GetSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mState = state;
        }

        CARAPI SetData(List<AppOpEntry> data) {
            mList = data;
            NotifyDataSetChanged();
        }

        //@Override
        public Int32 GetCount() {
            return mList != NULL ? mList->Size() : 0;
        }

        //@Override
        public AppOpEntry GetItem(Int32 position) {
            return mList->Get(position);
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return position;
        }

        /**
         * Populate new items in the list.
         */
        //@Override public View GetView(Int32 position, View convertView, ViewGroup parent) {
            View view;

            if (convertView == NULL) {
                view = mInflater->Inflate(R.layout.app_ops_item, parent, FALSE);
            } else {
                view = convertView;
            }

            AppOpEntry item = GetItem(position);
            ((ImageView)view->FindViewById(R.id.app_icon)).SetImageDrawable(
                    item->GetAppEntry()->GetIcon());
            ((TextView)view->FindViewById(R.id.app_name)).SetText(item->GetAppEntry()->GetLabel());
            ((TextView)view->FindViewById(R.id.op_name)).SetText(item->GetSummaryText(mState));
            ((TextView)view->FindViewById(R.id.op_time)).SetText(
                    item->GetTimeText(mResources, FALSE));

            return view;
        }
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mState = new AppOpsState(GetActivity());
    }

    //@Override CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        // Give some text to display if there is no data.  In a real
        // application this would come from a resource.
        SetEmptyText("No applications");

        // We have a menu item to show in action bar.
        SetHasOptionsMenu(TRUE);

        // Create an empty adapter we will use to display the loaded data.
        mAdapter = new AppListAdapter(GetActivity(), mState);
        SetListAdapter(mAdapter);

        // Start out with a progress indicator.
        SetListShown(FALSE);

        // Prepare the loader.
        GetLoaderManager()->InitLoader(0, NULL, this);
    }

    // utility method used to start sub activity
    private void StartApplicationDetailsActivity() {
        // start new fragment to display extended information
        Bundle args = new Bundle();
        args->PutString(AppOpsDetails.ARG_PACKAGE_NAME, mCurrentPkgName);

        SettingsActivity sa = (SettingsActivity) GetActivity();
        sa->StartPreferencePanel(AppOpsDetails.class->GetName(), args,
                R::string::app_ops_settings, NULL, this, RESULT_APP_DETAILS);
    }
    
    //@Override CARAPI OnListItemClick(ListView l, View v, Int32 position, Int64 id) {
        AppOpEntry entry = mAdapter->GetItem(position);
        if (entry != NULL) {
            mCurrentPkgName = entry->GetAppEntry()->GetApplicationInfo().packageName;
            StartApplicationDetailsActivity();
        }
    }

    //@Override public Loader<List<AppOpEntry>> OnCreateLoader(Int32 id, Bundle args) {
        Bundle fargs = GetArguments();
        AppOpsState.OpsTemplate template = NULL;
        if (fargs != NULL) {
            template = (AppOpsState.OpsTemplate)fargs->GetParcelable("template");
        }
        return new AppListLoader(GetActivity(), mState, template);
    }

    //@Override CARAPI OnLoadFinished(Loader<List<AppOpEntry>> loader, List<AppOpEntry> data) {
        // Set the new data in the adapter.
        mAdapter->SetData(data);

        // The list should now be shown.
        if (IsResumed()) {
            SetListShown(TRUE);
        } else {
            SetListShownNoAnimation(TRUE);
        }
    }

    //@Override CARAPI OnLoaderReset(Loader<List<AppOpEntry>> loader) {
        // Clear the data in the adapter.
        mAdapter->SetData(NULL);
    }
}
