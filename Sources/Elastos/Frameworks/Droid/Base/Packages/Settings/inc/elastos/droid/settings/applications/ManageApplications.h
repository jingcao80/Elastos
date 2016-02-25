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

package com.android.settings.applications;

using static android::Net::NetworkPolicyManager::IPOLICY_NONE;
using static android::Net::NetworkPolicyManager::IPOLICY_REJECT_METERED_BACKGROUND;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Support::V4::View::IPagerAdapter;
using Elastos::Droid::Support::V4::View::IPagerTabStrip;
using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemClickListener;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IFilter;
using Elastos::Droid::Widget::IFilterable;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ISpinner;

using Elastos::Droid::Internal::App::IIMediaContainerService;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUserSpinnerAdapter;
using Elastos::Droid::Settings::Settings::IRunningServicesActivity;
using Elastos::Droid::Settings::Settings::IStorageUseActivity;
using Elastos::Droid::Settings::Applications::ApplicationsState::IAppEntry;
using Elastos::Droid::Settings::Deviceinfo::IStorageMeasurement;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

final class CanBeOnSdCardChecker {
    final IPackageManager mPm;
    Int32 mInstallLocation;
    
    CanBeOnSdCardChecker() {
        mPm = IPackageManager.Stub->AsInterface(
                ServiceManager->GetService("package"));
    }
    
    void Init() {
        try {
            mInstallLocation = mPm->GetInstallLocation();
        } catch (RemoteException e) {
            Logger::E("CanBeOnSdCardChecker", "Is Package Manager running?");
            return;
        }
    }
    
    Boolean Check(ApplicationInfo info) {
        Boolean canBe = FALSE;
        if ((info.flags & ApplicationInfo.FLAG_EXTERNAL_STORAGE) != 0) {
            canBe = TRUE;
        } else {
            if ((info.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
                if (info.installLocation == PackageInfo.INSTALL_LOCATION_PREFER_EXTERNAL ||
                        info.installLocation == PackageInfo.INSTALL_LOCATION_AUTO) {
                    canBe = TRUE;
                } else if (info.installLocation
                        == PackageInfo.INSTALL_LOCATION_UNSPECIFIED) {
                    if (mInstallLocation == PackageHelper.APP_INSTALL_EXTERNAL) {
                        // For apps with no preference and the default value set
                        // to install on sdcard.
                        canBe = TRUE;
                    }
                }
            }
        }
        return canBe;
    }
}

interface AppClickListener {
    void OnItemClick(ManageApplications.TabInfo tab, AdapterView<?> parent,
            View view, Int32 position, Int64 id);
}

/**
 * Activity to pick an application that will be used to display installation information and
 * options to uninstall/delete user data for system applications. This activity
 * can be launched through Settings or via the ACTION_MANAGE_PACKAGE_STORAGE
 * intent.
 */
public class ManageApplications extends Fragment implements
        AppClickListener, DialogInterface.OnClickListener,
        DialogInterface.OnDismissListener, OnItemSelectedListener  {

    static const String TAG = "ManageApplications";
    static const Boolean DEBUG = FALSE;

    private static const String EXTRA_LIST_TYPE = "currentListType";
    private static const String EXTRA_SORT_ORDER = "sortOrder";
    private static const String EXTRA_SHOW_BACKGROUND = "showBackground";
    private static const String EXTRA_DEFAULT_LIST_TYPE = "defaultListType";
    private static const String EXTRA_RESET_DIALOG = "resetDialog";

    // attributes used as keys when passing values to InstalledAppDetails activity
    public static const String APP_CHG = "chg";

    // constant value that can be used to check return code from sub activity.
    private static const Int32 INSTALLED_APP_DETAILS = 1;

    public static const Int32 SIZE_TOTAL = 0;
    public static const Int32 SIZE_INTERNAL = 1;
    public static const Int32 SIZE_EXTERNAL = 2;

    // sort order that can be changed through the menu can be sorted alphabetically
    // or Size(descending)
    private static const Int32 MENU_OPTIONS_BASE = 0;
    // Filter options used for displayed list of applications
    public static const Int32 FILTER_APPS_ALL = MENU_OPTIONS_BASE + 0;
    public static const Int32 FILTER_APPS_THIRD_PARTY = MENU_OPTIONS_BASE + 1;
    public static const Int32 FILTER_APPS_SDCARD = MENU_OPTIONS_BASE + 2;
    public static const Int32 FILTER_APPS_DISABLED = MENU_OPTIONS_BASE + 3;

    public static const Int32 SORT_ORDER_ALPHA = MENU_OPTIONS_BASE + 4;
    public static const Int32 SORT_ORDER_SIZE = MENU_OPTIONS_BASE + 5;
    public static const Int32 SHOW_RUNNING_SERVICES = MENU_OPTIONS_BASE + 6;
    public static const Int32 SHOW_BACKGROUND_PROCESSES = MENU_OPTIONS_BASE + 7;
    public static const Int32 RESET_APP_PREFERENCES = MENU_OPTIONS_BASE + 8;
    // sort order
    private Int32 mSortOrder = SORT_ORDER_ALPHA;
    
    private ApplicationsState mApplicationsState;

    public static class TabInfo implements OnItemClickListener {
        public final ManageApplications mOwner;
        public final ApplicationsState mApplicationsState;
        public final CharSequence mLabel;
        public final Int32 mListType;
        public final Int32 mFilter;
        public final AppClickListener mClickListener;
        public final CharSequence mInvalidSizeStr;
        public final CharSequence mComputingSizeStr;
        private final Bundle mSavedInstanceState;

        public ApplicationsAdapter mApplications;
        public LayoutInflater mInflater;
        public View mRootView;

        private IMediaContainerService mContainerService;

        private View mLoadingContainer;

        private View mListContainer;

        private ViewGroup mPinnedHeader;

        // ListView used to display list
        private ListView mListView;
        // Custom view used to display running processes
        private RunningProcessesView mRunningProcessesView;
        
        //private LinearColorBar mColorBar;
        //private TextView mStorageChartLabel;
        //private TextView mUsedStorageText;
        //private TextView mFreeStorageText;
        private Int64 mFreeStorage = 0, mAppStorage = 0, mTotalStorage = 0;
        private Int64 mLastUsedStorage, mLastAppStorage, mLastFreeStorage;

        final Runnable mRunningProcessesAvail = new Runnable() {
            CARAPI Run() {
                HandleRunningProcessesAvail();
            }
        };

        public TabInfo(ManageApplications owner, ApplicationsState apps,
                CharSequence label, Int32 listType, AppClickListener clickListener,
                Bundle savedInstanceState) {
            mOwner = owner;
            mApplicationsState = apps;
            mLabel = label;
            mListType = listType;
            switch (listType) {
                case LIST_TYPE_DOWNLOADED: mFilter = FILTER_APPS_THIRD_PARTY; break;
                case LIST_TYPE_SDCARD: mFilter = FILTER_APPS_SDCARD; break;
                case LIST_TYPE_DISABLED: mFilter = FILTER_APPS_DISABLED; break;
                default: mFilter = FILTER_APPS_ALL; break;
            }
            mClickListener = clickListener;
            mInvalidSizeStr = owner->GetActivity()->GetText(R::string::invalid_size_value);
            mComputingSizeStr = owner->GetActivity()->GetText(R::string::computing_size);
            mSavedInstanceState = savedInstanceState;
        }

        CARAPI SetContainerService(IMediaContainerService containerService) {
            mContainerService = containerService;
            UpdateStorageUsage();
        }

        public View Build(LayoutInflater inflater, ViewGroup contentParent, View contentChild) {
            if (mRootView != NULL) {
                return mRootView;
            }

            mInflater = inflater;
            mRootView = inflater->Inflate(mListType == LIST_TYPE_RUNNING
                    ? R.layout.manage_applications_running
                    : R.layout.manage_applications_apps, NULL);
            mPinnedHeader = (ViewGroup) mRootView->FindViewById(R.id.pinned_header);
            if (mOwner.mProfileSpinnerAdapter != NULL) {
                Spinner spinner = (Spinner) inflater->Inflate(R.layout.spinner_view, NULL);
                spinner->SetAdapter(mOwner.mProfileSpinnerAdapter);
                spinner->SetOnItemSelectedListener(mOwner);
                mPinnedHeader->AddView(spinner);
                mPinnedHeader->SetVisibility(View.VISIBLE);
            }
            mLoadingContainer = mRootView->FindViewById(R.id.loading_container);
            mLoadingContainer->SetVisibility(View.VISIBLE);
            mListContainer = mRootView->FindViewById(R.id.list_container);
            if (mListContainer != NULL) {
                // Create adapter and list view here
                View emptyView = mListContainer->FindViewById(R.id.empty);
                ListView lv = (ListView) mListContainer->FindViewById(android.R.id.list);
                if (emptyView != NULL) {
                    lv->SetEmptyView(emptyView);
                }
                lv->SetOnItemClickListener(this);
                lv->SetSaveEnabled(TRUE);
                lv->SetItemsCanFocus(TRUE);
                lv->SetTextFilterEnabled(TRUE);
                mListView = lv;
                mApplications = new ApplicationsAdapter(mApplicationsState, this, mFilter);
                mListView->SetAdapter(mApplications);
                mListView->SetRecyclerListener(mApplications);
                //mColorBar = (LinearColorBar)mListContainer->FindViewById(R.id.storage_color_bar);
                //mStorageChartLabel = (TextView)mListContainer->FindViewById(R.id.storageChartLabel);
                //mUsedStorageText = (TextView)mListContainer->FindViewById(R.id.usedStorageText);
                //mFreeStorageText = (TextView)mListContainer->FindViewById(R.id.freeStorageText);
                Utils->PrepareCustomPreferencesList(contentParent, contentChild, mListView, FALSE);
                if (mFilter == FILTER_APPS_SDCARD) {
                    //mStorageChartLabel->SetText(mOwner->GetActivity()->GetText(
                    //        R::string::sd_card_storage));
                } else {
                    //mStorageChartLabel->SetText(mOwner->GetActivity()->GetText(
                    //        R::string::internal_storage));
                }
                ApplyCurrentStorage();
            }
            mRunningProcessesView = (RunningProcessesView)mRootView->FindViewById(
                    R.id.running_processes);
            if (mRunningProcessesView != NULL) {
                mRunningProcessesView->DoCreate(mSavedInstanceState);
            }

            return mRootView;
        }

        CARAPI DetachView() {
            if (mRootView != NULL) {
                ViewGroup group = (ViewGroup)mRootView->GetParent();
                if (group != NULL) {
                    group->RemoveView(mRootView);
                }
            }
        }

        CARAPI Resume(Int32 sortOrder) {
            if (mApplications != NULL) {
                mApplications->Resume(sortOrder);
            }
            if (mRunningProcessesView != NULL) {
                Boolean haveData = mRunningProcessesView->DoResume(mOwner, mRunningProcessesAvail);
                if (haveData) {
                    mRunningProcessesView->SetVisibility(View.VISIBLE);
                    mLoadingContainer->SetVisibility(View.INVISIBLE);
                } else {
                    mLoadingContainer->SetVisibility(View.VISIBLE);
                }
            }
        }

        CARAPI Pause() {
            if (mApplications != NULL) {
                mApplications->Pause();
            }
            if (mRunningProcessesView != NULL) {
                mRunningProcessesView->DoPause();
            }
        }

        CARAPI Release() {
            if (mApplications != NULL) {
                mApplications->Release();
            }
        }

        void UpdateStorageUsage() {
            // Make sure a callback didn't come at an inopportune time.
            if (mOwner->GetActivity() == NULL) return;
            // Doesn't make sense for stuff that is not an app list.
            if (mApplications == NULL) return;

            mFreeStorage = 0;
            mAppStorage = 0;
            mTotalStorage = 0;

            if (mFilter == FILTER_APPS_SDCARD) {
                if (mContainerService != NULL) {
                    try {
                        final Int64[] stats = mContainerService->GetFileSystemStats(
                                Environment->GetExternalStorageDirectory()->GetPath());
                        mTotalStorage = stats[0];
                        mFreeStorage = stats[1];
                    } catch (RemoteException e) {
                        Logger::W(TAG, "Problem in container service", e);
                    }
                }

                if (mApplications != NULL) {
                    final Int32 N = mApplications->GetCount();
                    for (Int32 i=0; i<N; i++) {
                        ApplicationsState.AppEntry ae = mApplications->GetAppEntry(i);
                        mAppStorage += ae.externalCodeSize + ae.externalDataSize
                                + ae.externalCacheSize;
                    }
                }
            } else {
                if (mContainerService != NULL) {
                    try {
                        final Int64[] stats = mContainerService->GetFileSystemStats(
                                Environment->GetDataDirectory()->GetPath());
                        mTotalStorage = stats[0];
                        mFreeStorage = stats[1];
                    } catch (RemoteException e) {
                        Logger::W(TAG, "Problem in container service", e);
                    }
                }

                final Boolean emulatedStorage = Environment->IsExternalStorageEmulated();
                if (mApplications != NULL) {
                    final Int32 N = mApplications->GetCount();
                    for (Int32 i=0; i<N; i++) {
                        ApplicationsState.AppEntry ae = mApplications->GetAppEntry(i);
                        mAppStorage += ae.codeSize + ae.dataSize;
                        if (emulatedStorage) {
                            mAppStorage += ae.externalCodeSize + ae.externalDataSize;
                        }
                    }
                }
                mFreeStorage += mApplicationsState->SumCacheSizes();
            }

            ApplyCurrentStorage();
        }

        void ApplyCurrentStorage() {
            // If view hierarchy is not yet created, no views to update.
            if (mRootView == NULL) {
                return;
            }
            /*
            if (mTotalStorage > 0) {
                BidiFormatter bidiFormatter = BidiFormatter->GetInstance();
                mColorBar->SetRatios((mTotalStorage-mFreeStorage-mAppStorage)/(Float)mTotalStorage,
                        mAppStorage/(Float)mTotalStorage, mFreeStorage/(Float)mTotalStorage);
                Int64 usedStorage = mTotalStorage - mFreeStorage;
                if (mLastUsedStorage != usedStorage) {
                    mLastUsedStorage = usedStorage;
                    String sizeStr = bidiFormatter->UnicodeWrap(
                            Formatter->FormatShortFileSize(mOwner->GetActivity(), usedStorage));
                    mUsedStorageText->SetText(mOwner->GetActivity()->GetResources().GetString(
                            R::string::service_foreground_processes, sizeStr));
                }
                if (mLastFreeStorage != mFreeStorage) {
                    mLastFreeStorage = mFreeStorage;
                    String sizeStr = bidiFormatter->UnicodeWrap(
                            Formatter->FormatShortFileSize(mOwner->GetActivity(), mFreeStorage));
                    mFreeStorageText->SetText(mOwner->GetActivity()->GetResources().GetString(
                            R::string::service_background_processes, sizeStr));
                }
            } else {
                mColorBar->SetRatios(0, 0, 0);
                if (mLastUsedStorage != -1) {
                    mLastUsedStorage = -1;
                    mUsedStorageText->SetText("");
                }
                if (mLastFreeStorage != -1) {
                    mLastFreeStorage = -1;
                    mFreeStorageText->SetText("");
                }
            }
            */
        }

        //@Override
        CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
            mClickListener->OnItemClick(this, parent, view, position, id);
        }

        void HandleRunningProcessesAvail() {
            mLoadingContainer->StartAnimation(AnimationUtils->LoadAnimation(
                    mOwner->GetActivity(), android.R.anim.fade_out));
            mRunningProcessesView->StartAnimation(AnimationUtils->LoadAnimation(
                    mOwner->GetActivity(), android.R.anim.fade_in));
            mRunningProcessesView->SetVisibility(View.VISIBLE);
            mLoadingContainer->SetVisibility(View.GONE);
        }
    }
    private final ArrayList<TabInfo> mTabs = new ArrayList<TabInfo>();
    private Int32 mNumTabs;
    TabInfo mCurTab = NULL;

    // Size resource used for packages whose size computation failed for some reason
    CharSequence mInvalidSizeStr;
    private CharSequence mComputingSizeStr;
    
    // layout inflater object used to inflate views
    private LayoutInflater mInflater;
    
    private String mCurrentPkgName;
    
    private Menu mOptionsMenu;

    // These are for keeping track of activity and spinner switch state.
    private Boolean mActivityResumed;

    private static const Int32 LIST_TYPE_MISSING = -1;
    static const Int32 LIST_TYPE_DOWNLOADED = 0;
    static const Int32 LIST_TYPE_RUNNING = 1;
    static const Int32 LIST_TYPE_SDCARD = 2;
    static const Int32 LIST_TYPE_ALL = 3;
    static const Int32 LIST_TYPE_DISABLED = 4;

    private Boolean mShowBackground = FALSE;
    
    private Int32 mDefaultListType = -1;

    private ViewGroup mContentContainer;
    private View mRootView;
    private ViewPager mViewPager;
    private UserSpinnerAdapter mProfileSpinnerAdapter;
    private Context mContext;

    AlertDialog mResetDialog;

    class MyPagerAdapter extends PagerAdapter
            implements ViewPager.OnPageChangeListener {
        Int32 mCurPos = 0;

        //@Override
        public Int32 GetCount() {
            return mNumTabs;
        }
        
        //@Override
        public Object InstantiateItem(ViewGroup container, Int32 position) {
            TabInfo tab = mTabs->Get(position);
            View root = tab->Build(mInflater, mContentContainer, mRootView);
            container->AddView(root);
            root->SetTag(R.id.name, tab);
            return root;
        }

        //@Override
        CARAPI DestroyItem(ViewGroup container, Int32 position, Object object) {
            container->RemoveView((View)object);
        }

        //@Override
        public Boolean IsViewFromObject(View view, Object object) {
            return view == object;
        }

        //@Override
        public Int32 GetItemPosition(Object object) {
            return super->GetItemPosition(object);
            //return ((TabInfo)((View)object).GetTag(R.id.name)).mListType;
        }

        //@Override
        public CharSequence GetPageTitle(Int32 position) {
            return mTabs->Get(position).mLabel;
        }

        //@Override
        CARAPI OnPageScrolled(Int32 position, Float positionOffset, Int32 positionOffsetPixels) {
        }

        //@Override
        CARAPI OnPageSelected(Int32 position) {
            mCurPos = position;
        }

        //@Override
        CARAPI OnPageScrollStateChanged(Int32 state) {
            if (state == ViewPager.SCROLL_STATE_IDLE) {
                UpdateCurrentTab(mCurPos);
            }
        }
    }

    /*
     * Custom adapter implementation for the ListView
     * This adapter maintains a map for each displayed application and its properties
     * An index value on each AppInfo object indicates the correct position or index
     * in the list. If the list gets updated dynamically when the user is viewing the list of
     * applications, we need to return the correct index of position. This is done by mapping
     * the getId methods via the package name into the internal maps and indices.
     * The order of applications in the list is mirrored in mAppLocalList
     */
    static class ApplicationsAdapter extends BaseAdapter implements Filterable,
            ApplicationsState.Callbacks, AbsListView.RecyclerListener {
        private final ApplicationsState mState;
        private final ApplicationsState.Session mSession;
        private final TabInfo mTab;
        private final Context mContext;
        private final ArrayList<View> mActive = new ArrayList<View>();
        private final Int32 mFilterMode;
        private ArrayList<ApplicationsState.AppEntry> mBaseEntries;
        private ArrayList<ApplicationsState.AppEntry> mEntries;
        private Boolean mResumed;
        private Int32 mLastSortMode=-1;
        private Boolean mWaitingForData;
        private Int32 mWhichSize = SIZE_TOTAL;
        CharSequence mCurFilterPrefix;

        private Filter mFilter = new Filter() {
            //@Override
            protected FilterResults PerformFiltering(CharSequence constraint) {
                ArrayList<ApplicationsState.AppEntry> entries
                        = ApplyPrefixFilter(constraint, mBaseEntries);
                FilterResults fr = new FilterResults();
                fr.values = entries;
                fr.count = entries->Size();
                return fr;
            }

            //@Override
            protected void PublishResults(CharSequence constraint, FilterResults results) {
                mCurFilterPrefix = constraint;
                mEntries = (ArrayList<ApplicationsState.AppEntry>)results.values;
                NotifyDataSetChanged();
                mTab->UpdateStorageUsage();
            }
        };

        public ApplicationsAdapter(ApplicationsState state, TabInfo tab, Int32 filterMode) {
            mState = state;
            mSession = state->NewSession(this);
            mTab = tab;
            mContext = tab.mOwner->GetActivity();
            mFilterMode = filterMode;
        }

        CARAPI Resume(Int32 sort) {
            if (DEBUG) Logger::I(TAG, "Resume!  mResumed=" + mResumed);
            if (!mResumed) {
                mResumed = TRUE;
                mSession->Resume();
                mLastSortMode = sort;
                Rebuild(TRUE);
            } else {
                Rebuild(sort);
            }
        }

        CARAPI Pause() {
            if (mResumed) {
                mResumed = FALSE;
                mSession->Pause();
            }
        }

        CARAPI Release() {
            mSession->Release();
        }

        CARAPI Rebuild(Int32 sort) {
            if (sort == mLastSortMode) {
                return;
            }
            mLastSortMode = sort;
            Rebuild(TRUE);
        }
        
        CARAPI Rebuild(Boolean eraseold) {
            if (DEBUG) Logger::I(TAG, "Rebuilding app list...");
            ApplicationsState.AppFilter filterObj;
            Comparator<AppEntry> comparatorObj;
            Boolean emulated = Environment->IsExternalStorageEmulated();
            if (emulated) {
                mWhichSize = SIZE_TOTAL;
            } else {
                mWhichSize = SIZE_INTERNAL;
            }
            switch (mFilterMode) {
                case FILTER_APPS_THIRD_PARTY:
                    filterObj = ApplicationsState.THIRD_PARTY_FILTER;
                    break;
                case FILTER_APPS_SDCARD:
                    filterObj = ApplicationsState.ON_SD_CARD_FILTER;
                    if (!emulated) {
                        mWhichSize = SIZE_EXTERNAL;
                    }
                    break;
                case FILTER_APPS_DISABLED:
                    filterObj = ApplicationsState.DISABLED_FILTER;
                    break;
                default:
                    filterObj = ApplicationsState.ALL_ENABLED_FILTER;
                    break;
            }
            switch (mLastSortMode) {
                case SORT_ORDER_SIZE:
                    switch (mWhichSize) {
                        case SIZE_INTERNAL:
                            comparatorObj = ApplicationsState.INTERNAL_SIZE_COMPARATOR;
                            break;
                        case SIZE_EXTERNAL:
                            comparatorObj = ApplicationsState.EXTERNAL_SIZE_COMPARATOR;
                            break;
                        default:
                            comparatorObj = ApplicationsState.SIZE_COMPARATOR;
                            break;
                    }
                    break;
                default:
                    comparatorObj = ApplicationsState.ALPHA_COMPARATOR;
                    break;
            }
            ArrayList<ApplicationsState.AppEntry> entries
                    = mSession->Rebuild(filterObj, comparatorObj);
            if (entries == NULL && !eraseold) {
                // Don't have new list yet, but can continue using the old one.
                return;
            }
            mBaseEntries = entries;
            if (mBaseEntries != NULL) {
                mEntries = ApplyPrefixFilter(mCurFilterPrefix, mBaseEntries);
            } else {
                mEntries = NULL;
            }
            NotifyDataSetChanged();
            mTab->UpdateStorageUsage();

            if (entries == NULL) {
                mWaitingForData = TRUE;
                mTab.mListContainer->SetVisibility(View.INVISIBLE);
                mTab.mLoadingContainer->SetVisibility(View.VISIBLE);
            } else {
                mTab.mListContainer->SetVisibility(View.VISIBLE);
                mTab.mLoadingContainer->SetVisibility(View.GONE);
            }
        }

        ArrayList<ApplicationsState.AppEntry> ApplyPrefixFilter(CharSequence prefix,
                ArrayList<ApplicationsState.AppEntry> origEntries) {
            if (prefix == NULL || prefix->Length() == 0) {
                return origEntries;
            } else {
                String prefixStr = ApplicationsState->Normalize(prefix->ToString());
                final String spacePrefixStr = " " + prefixStr;
                ArrayList<ApplicationsState.AppEntry> newEntries
                        = new ArrayList<ApplicationsState.AppEntry>();
                for (Int32 i=0; i<origEntries->Size(); i++) {
                    ApplicationsState.AppEntry entry = origEntries->Get(i);
                    String nlabel = entry->GetNormalizedLabel();
                    if (nlabel->StartsWith(prefixStr) || nlabel->IndexOf(spacePrefixStr) != -1) {
                        newEntries->Add(entry);
                    }
                }
                return newEntries;
            }
        }

        //@Override
        CARAPI OnRunningStateChanged(Boolean running) {
            mTab.mOwner->GetActivity()->SetProgressBarIndeterminateVisibility(running);
        }

        //@Override
        CARAPI OnRebuildComplete(ArrayList<AppEntry> apps) {
            if (mTab.mLoadingContainer->GetVisibility() == View.VISIBLE) {
                mTab.mLoadingContainer->StartAnimation(AnimationUtils->LoadAnimation(
                        mContext, android.R.anim.fade_out));
                mTab.mListContainer->StartAnimation(AnimationUtils->LoadAnimation(
                        mContext, android.R.anim.fade_in));
            }
            mTab.mListContainer->SetVisibility(View.VISIBLE);
            mTab.mLoadingContainer->SetVisibility(View.GONE);
            mWaitingForData = FALSE;
            mBaseEntries = apps;
            mEntries = ApplyPrefixFilter(mCurFilterPrefix, mBaseEntries);
            NotifyDataSetChanged();
            mTab->UpdateStorageUsage();
        }

        //@Override
        CARAPI OnPackageListChanged() {
            Rebuild(FALSE);
        }

        //@Override
        CARAPI OnPackageIconChanged() {
            // We ensure icons are loaded when their item is displayed, so
            // don't care about icons loaded in the background.
        }

        //@Override
        CARAPI OnPackageSizeChanged(String packageName) {
            for (Int32 i=0; i<mActive->Size(); i++) {
                AppViewHolder holder = (AppViewHolder)mActive->Get(i).GetTag();
                if (holder.entry.info.packageName->Equals(packageName)) {
                    synchronized(holder.entry) {
                        holder->UpdateSizeText(mTab.mInvalidSizeStr, mWhichSize);
                    }
                    if (holder.entry.info.packageName->Equals(mTab.mOwner.mCurrentPkgName)
                            && mLastSortMode == SORT_ORDER_SIZE) {
                        // We got the size information for the last app the
                        // user viewed, and are sorting by size...  they may
                        // have cleared data, so we immediately want to resort
                        // the list with the new size to reflect it to the user.
                        Rebuild(FALSE);
                    }
                    mTab->UpdateStorageUsage();
                    return;
                }
            }
        }

        //@Override
        CARAPI OnAllSizesComputed() {
            if (mLastSortMode == SORT_ORDER_SIZE) {
                Rebuild(FALSE);
            }
            mTab->UpdateStorageUsage();
        }
        
        public Int32 GetCount() {
            return mEntries != NULL ? mEntries->Size() : 0;
        }
        
        public Object GetItem(Int32 position) {
            return mEntries->Get(position);
        }
        
        public ApplicationsState.AppEntry GetAppEntry(Int32 position) {
            return mEntries->Get(position);
        }

        public Int64 GetItemId(Int32 position) {
            return mEntries->Get(position).id;
        }
        
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            // A ViewHolder keeps references to children views to avoid unnecessary calls
            // to FindViewById() on each row.
            AppViewHolder holder = AppViewHolder->CreateOrRecycle(mTab.mInflater, convertView);
            convertView = holder.rootView;

            // Bind the data efficiently with the holder
            ApplicationsState.AppEntry entry = mEntries->Get(position);
            synchronized(entry) {
                holder.entry = entry;
                if (entry.label != NULL) {
                    holder.appName->SetText(entry.label);
                }
                mState->EnsureIcon(entry);
                if (entry.icon != NULL) {
                    holder.appIcon->SetImageDrawable(entry.icon);
                }
                holder->UpdateSizeText(mTab.mInvalidSizeStr, mWhichSize);
                if ((entry.info.flags&ApplicationInfo.FLAG_INSTALLED) == 0) {
                    holder.disabled->SetVisibility(View.VISIBLE);
                    holder.disabled->SetText(R::string::not_installed);
                } else if (!entry.info.enabled) {
                    holder.disabled->SetVisibility(View.VISIBLE);
                    holder.disabled->SetText(R::string::disabled);
                } else {
                    holder.disabled->SetVisibility(View.GONE);
                }
                if (mFilterMode == FILTER_APPS_SDCARD) {
                    holder.checkBox->SetVisibility(View.VISIBLE);
                    holder.checkBox->SetChecked((entry.info.flags
                            & ApplicationInfo.FLAG_EXTERNAL_STORAGE) != 0);
                } else {
                    holder.checkBox->SetVisibility(View.GONE);
                }
            }
            mActive->Remove(convertView);
            mActive->Add(convertView);
            return convertView;
        }

        //@Override
        public Filter GetFilter() {
            return mFilter;
        }

        //@Override
        CARAPI OnMovedToScrapHeap(View view) {
            mActive->Remove(view);
        }
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        SetHasOptionsMenu(TRUE);

        mContext = GetActivity();
        mApplicationsState = ApplicationsState->GetInstance(GetActivity()->GetApplication());
        Intent intent = GetActivity()->GetIntent();
        String action = intent->GetAction();
        Int32 defaultListType = LIST_TYPE_DOWNLOADED;
        String className = GetArguments() != NULL
                ? GetArguments()->GetString("classname") : NULL;
        if (className == NULL) {
            className = intent->GetComponent()->GetClassName();
        }
        if (className->Equals(RunningServicesActivity.class->GetName())
                || className->EndsWith(".RunningServices")) {
            defaultListType = LIST_TYPE_RUNNING;
        } else if (className->Equals(StorageUseActivity.class->GetName())
                || IIntent::ACTION_MANAGE_PACKAGE_STORAGE->Equals(action)
                || className->EndsWith(".StorageUse")) {
            mSortOrder = SORT_ORDER_SIZE;
            defaultListType = LIST_TYPE_ALL;
        } else if (android.provider.Settings.ACTION_MANAGE_ALL_APPLICATIONS_SETTINGS->Equals(action)) {
            // Select the all-apps list, with the default sorting
            defaultListType = LIST_TYPE_ALL;
        }

        if (savedInstanceState != NULL) {
            mSortOrder = savedInstanceState->GetInt(EXTRA_SORT_ORDER, mSortOrder);
            Int32 tmp = savedInstanceState->GetInt(EXTRA_DEFAULT_LIST_TYPE, -1);
            if (tmp != -1) defaultListType = tmp;
            mShowBackground = savedInstanceState->GetBoolean(EXTRA_SHOW_BACKGROUND, FALSE);
        }

        mDefaultListType = defaultListType;

        final Intent containerIntent = new Intent()->SetComponent(
                StorageMeasurement.DEFAULT_CONTAINER_COMPONENT);
        GetActivity()->BindService(containerIntent, mContainerConnection, Context.BIND_AUTO_CREATE);

        mInvalidSizeStr = GetActivity()->GetText(R::string::invalid_size_value);
        mComputingSizeStr = GetActivity()->GetText(R::string::computing_size);

        TabInfo tab = new TabInfo(this, mApplicationsState,
                GetActivity()->GetString(R::string::filter_apps_third_party),
                LIST_TYPE_DOWNLOADED, this, savedInstanceState);
        mTabs->Add(tab);

        if (!Environment->IsExternalStorageEmulated()) {
            tab = new TabInfo(this, mApplicationsState,
                    GetActivity()->GetString(R::string::filter_apps_onsdcard),
                    LIST_TYPE_SDCARD, this, savedInstanceState);
            mTabs->Add(tab);
        }

        tab = new TabInfo(this, mApplicationsState,
                GetActivity()->GetString(R::string::filter_apps_running),
                LIST_TYPE_RUNNING, this, savedInstanceState);
        mTabs->Add(tab);

        tab = new TabInfo(this, mApplicationsState,
                GetActivity()->GetString(R::string::filter_apps_all),
                LIST_TYPE_ALL, this, savedInstanceState);
        mTabs->Add(tab);

        tab = new TabInfo(this, mApplicationsState,
                GetActivity()->GetString(R::string::filter_apps_disabled),
                LIST_TYPE_DISABLED, this, savedInstanceState);
        mTabs->Add(tab);

        mNumTabs = mTabs->Size();

        final UserManager um = (UserManager) mContext->GetSystemService(Context.USER_SERVICE);
        mProfileSpinnerAdapter = Utils->CreateUserSpinnerAdapter(um, mContext);
    }


    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // initialize the inflater
        mInflater = inflater;

        View rootView = mInflater->Inflate(R.layout.manage_applications_content,
                container, FALSE);
        mContentContainer = container;
        mRootView = rootView;

        mViewPager = (ViewPager) rootView->FindViewById(R.id.pager);
        MyPagerAdapter adapter = new MyPagerAdapter();
        mViewPager->SetAdapter(adapter);
        mViewPager->SetOnPageChangeListener(adapter);
        PagerTabStrip tabs = (PagerTabStrip) rootView->FindViewById(R.id.tabs);
        tabs->SetTabIndicatorColorResource(R::Color::theme_accent);

        // We have to do this now because PreferenceFrameLayout looks at it
        // only when the view is added.
        if (container instanceof PreferenceFrameLayout) {
            ((PreferenceFrameLayout.LayoutParams) rootView->GetLayoutParams()).removeBorders = TRUE;
        }

        if (savedInstanceState != NULL && savedInstanceState->GetBoolean(EXTRA_RESET_DIALOG)) {
            BuildResetDialog();
        }

        if (savedInstanceState == NULL) {
            // First time init: make sure view pager is showing the correct tab.
            Int32 extraCurrentListType = GetActivity()->GetIntent().GetIntExtra(EXTRA_LIST_TYPE,
                    LIST_TYPE_MISSING);
            Int32 currentListType = (extraCurrentListType != LIST_TYPE_MISSING)
                    ? extraCurrentListType : mDefaultListType;
            for (Int32 i = 0; i < mNumTabs; i++) {
                TabInfo tab = mTabs->Get(i);
                if (tab.mListType == currentListType) {
                    mViewPager->SetCurrentItem(i);
                    break;
                }
            }
        }

        return rootView;
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mActivityResumed = TRUE;
        UpdateCurrentTab(mViewPager->GetCurrentItem());
        UpdateNumTabs();
        UpdateOptionsMenu();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutInt(EXTRA_SORT_ORDER, mSortOrder);
        if (mDefaultListType != -1) {
            outState->PutInt(EXTRA_DEFAULT_LIST_TYPE, mDefaultListType);
        }
        outState->PutBoolean(EXTRA_SHOW_BACKGROUND, mShowBackground);
        if (mResetDialog != NULL) {
            outState->PutBoolean(EXTRA_RESET_DIALOG, TRUE);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mActivityResumed = FALSE;
        for (Int32 i=0; i<mTabs->Size(); i++) {
            mTabs->Get(i).Pause();
        }
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        if (mResetDialog != NULL) {
            mResetDialog->Dismiss();
            mResetDialog = NULL;
        }
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        // We are going to keep the tab data structures around, but they
        // are no longer attached to their view hierarchy.
        for (Int32 i=0; i<mTabs->Size(); i++) {
            mTabs->Get(i).DetachView();
            mTabs->Get(i).Release();
        }
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == INSTALLED_APP_DETAILS && mCurrentPkgName != NULL) {
            mApplicationsState->RequestSize(mCurrentPkgName);
        }
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        UserHandle selectedUser = mProfileSpinnerAdapter->GetUserHandle(position);
        if (selectedUser->GetIdentifier() != UserHandle->MyUserId()) {
            Intent intent = new Intent(Settings.ACTION_APPLICATION_SETTINGS);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
            Int32 currentTab = mViewPager->GetCurrentItem();
            intent->PutExtra(EXTRA_LIST_TYPE, mTabs->Get(currentTab).mListType);
            mContext->StartActivityAsUser(intent, selectedUser);
        }
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
        // Nothing to do
    }

    private void UpdateNumTabs() {
        Int32 newNum = mApplicationsState->HaveDisabledApps() ? mTabs->Size() : (mTabs->Size()-1);
        if (newNum != mNumTabs) {
            mNumTabs = newNum;
            if (mViewPager != NULL) {
                mViewPager->GetAdapter()->NotifyDataSetChanged();
            }
        }
    }

    TabInfo TabForType(Int32 type) {
        for (Int32 i = 0; i < mTabs->Size(); i++) {
            TabInfo tab = mTabs->Get(i);
            if (tab.mListType == type) {
                return tab;
            }
        }
        return NULL;
    }

    // utility method used to start sub activity
    private void StartApplicationDetailsActivity() {
        // start new fragment to display extended information
        Bundle args = new Bundle();
        args->PutString(InstalledAppDetails.ARG_PACKAGE_NAME, mCurrentPkgName);

        SettingsActivity sa = (SettingsActivity) GetActivity();
        sa->StartPreferencePanel(InstalledAppDetails.class->GetName(), args,
                R::string::application_info_label, NULL, this, INSTALLED_APP_DETAILS);
    }
    
    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        mOptionsMenu = menu;
        // note: icons removed for now because the cause the new action
        // bar UI to be very confusing.
        menu->Add(0, SORT_ORDER_ALPHA, 1, R::string::sort_order_alpha)
                //.SetIcon(android.R.drawable.ic_menu_sort_alphabetically)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        menu->Add(0, SORT_ORDER_SIZE, 2, R::string::sort_order_size)
                //.SetIcon(android.R.drawable.ic_menu_sort_by_size)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        menu->Add(0, SHOW_RUNNING_SERVICES, 3, R::string::show_running_services)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        menu->Add(0, SHOW_BACKGROUND_PROCESSES, 3, R::string::show_background_processes)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        menu->Add(0, RESET_APP_PREFERENCES, 4, R::string::reset_app_preferences)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        UpdateOptionsMenu();
    }
    
    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        UpdateOptionsMenu();
    }
    
    //@Override
    CARAPI OnDestroyOptionsMenu() {
        mOptionsMenu = NULL;
    }

    //@Override
    CARAPI OnDestroy() {
        GetActivity()->UnbindService(mContainerConnection);
        super->OnDestroy();
    }

    void UpdateOptionsMenu() {
        if (mOptionsMenu == NULL) {
            return;
        }
        
        /*
         * The running processes screen doesn't use the mApplicationsAdapter
         * so bringing up this menu in that case doesn't make any sense.
         */
        if (mCurTab != NULL && mCurTab.mListType == LIST_TYPE_RUNNING) {
            TabInfo tab = TabForType(LIST_TYPE_RUNNING);
            Boolean showingBackground = tab != NULL && tab.mRunningProcessesView != NULL
                    ? tab.mRunningProcessesView.mAdapter->GetShowBackground() : FALSE;
            mOptionsMenu->FindItem(SORT_ORDER_ALPHA).SetVisible(FALSE);
            mOptionsMenu->FindItem(SORT_ORDER_SIZE).SetVisible(FALSE);
            mOptionsMenu->FindItem(SHOW_RUNNING_SERVICES).SetVisible(showingBackground);
            mOptionsMenu->FindItem(SHOW_BACKGROUND_PROCESSES).SetVisible(!showingBackground);
            mOptionsMenu->FindItem(RESET_APP_PREFERENCES).SetVisible(FALSE);
            mShowBackground = showingBackground;
        } else {
            mOptionsMenu->FindItem(SORT_ORDER_ALPHA).SetVisible(mSortOrder != SORT_ORDER_ALPHA);
            mOptionsMenu->FindItem(SORT_ORDER_SIZE).SetVisible(mSortOrder != SORT_ORDER_SIZE);
            mOptionsMenu->FindItem(SHOW_RUNNING_SERVICES).SetVisible(FALSE);
            mOptionsMenu->FindItem(SHOW_BACKGROUND_PROCESSES).SetVisible(FALSE);
            mOptionsMenu->FindItem(RESET_APP_PREFERENCES).SetVisible(TRUE);
        }
    }

    void BuildResetDialog() {
        if (mResetDialog == NULL) {
            AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
            builder->SetTitle(R::string::reset_app_preferences_title);
            builder->SetMessage(R::string::reset_app_preferences_desc);
            builder->SetPositiveButton(R::string::reset_app_preferences_button, this);
            builder->SetNegativeButton(R::string::cancel, NULL);
            mResetDialog = builder->Show();
            mResetDialog->SetOnDismissListener(this);
        }
    }

    //@Override
    CARAPI OnDismiss(DialogInterface dialog) {
        if (mResetDialog == dialog) {
            mResetDialog = NULL;
        }
    }


    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (mResetDialog == dialog) {
            final PackageManager pm = GetActivity()->GetPackageManager();
            final IPackageManager mIPm = IPackageManager.Stub->AsInterface(
                    ServiceManager->GetService("package"));
            final INotificationManager nm = INotificationManager.Stub->AsInterface(
                    ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
            final NetworkPolicyManager npm = NetworkPolicyManager->From(GetActivity());
            final AppOpsManager aom = (AppOpsManager)GetActivity()->GetSystemService(
                    Context.APP_OPS_SERVICE);
            final Handler handler = new Handler(GetActivity()->GetMainLooper());
            (new AsyncTask<Void, Void, Void>() {
                //@Override protected Void DoInBackground(Void... params) {
                    List<ApplicationInfo> apps = pm->GetInstalledApplications(
                            PackageManager.GET_DISABLED_COMPONENTS);
                    for (Int32 i=0; i<apps->Size(); i++) {
                        ApplicationInfo app = apps->Get(i);
                        try {
                            if (DEBUG) Logger::V(TAG, "Enabling notifications: " + app.packageName);
                            nm->SetNotificationsEnabledForPackage(app.packageName, app.uid, TRUE);
                        } catch (android.os.RemoteException ex) {
                        }
                        if (!app.enabled) {
                            if (DEBUG) Logger::V(TAG, "Enabling app: " + app.packageName);
                            if (pm->GetApplicationEnabledSetting(app.packageName)
                                    == PackageManager.COMPONENT_ENABLED_STATE_DISABLED_USER) {
                                pm->SetApplicationEnabledSetting(app.packageName,
                                        PackageManager.COMPONENT_ENABLED_STATE_DEFAULT,
                                        PackageManager.DONT_KILL_APP);
                            }
                        }
                    }
                    try {
                        mIPm->ResetPreferredActivities(UserHandle->MyUserId());
                    } catch (RemoteException e) {
                    }
                    aom->ResetAllModes();
                    final Int32[] restrictedUids = npm->GetUidsWithPolicy(
                            POLICY_REJECT_METERED_BACKGROUND);
                    final Int32 currentUserId = ActivityManager->GetCurrentUser();
                    for (Int32 uid : restrictedUids) {
                        // Only reset for current user
                        if (UserHandle->GetUserId(uid) == currentUserId) {
                            if (DEBUG) Logger::V(TAG, "Clearing data policy: " + uid);
                            npm->SetUidPolicy(uid, POLICY_NONE);
                        }
                    }
                    handler->Post(new Runnable() {
                        //@Override CARAPI Run() {
                            if (DEBUG) Logger::V(TAG, "Done clearing");
                            if (GetActivity() != NULL && mActivityResumed) {
                                if (DEBUG) Logger::V(TAG, "Updating UI!");
                                for (Int32 i=0; i<mTabs->Size(); i++) {
                                    TabInfo tab = mTabs->Get(i);
                                    if (tab.mApplications != NULL) {
                                        tab.mApplications->Pause();
                                    }
                                }
                                if (mCurTab != NULL) {
                                    mCurTab->Resume(mSortOrder);
                                }
                            }
                        }
                    });
                    return NULL;
                }
            }).Execute();
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        Int32 menuId = item->GetItemId();
        if ((menuId == SORT_ORDER_ALPHA) || (menuId == SORT_ORDER_SIZE)) {
            mSortOrder = menuId;
            if (mCurTab != NULL && mCurTab.mApplications != NULL) {
                mCurTab.mApplications->Rebuild(mSortOrder);
            }
        } else if (menuId == SHOW_RUNNING_SERVICES) {
            mShowBackground = FALSE;
            if (mCurTab != NULL && mCurTab.mRunningProcessesView != NULL) {
                mCurTab.mRunningProcessesView.mAdapter->SetShowBackground(FALSE);
            }
        } else if (menuId == SHOW_BACKGROUND_PROCESSES) {
            mShowBackground = TRUE;
            if (mCurTab != NULL && mCurTab.mRunningProcessesView != NULL) {
                mCurTab.mRunningProcessesView.mAdapter->SetShowBackground(TRUE);
            }
        } else if (menuId == RESET_APP_PREFERENCES) {
            BuildResetDialog();
        } else {
            // Handle the home button
            return FALSE;
        }
        UpdateOptionsMenu();
        return TRUE;
    }
    
    CARAPI OnItemClick(TabInfo tab, AdapterView<?> parent, View view, Int32 position,
            Int64 id) {
        if (tab.mApplications != NULL && tab.mApplications->GetCount() > position) {
            ApplicationsState.AppEntry entry = tab.mApplications->GetAppEntry(position);
            mCurrentPkgName = entry.info.packageName;
            StartApplicationDetailsActivity();
        }
    }

    CARAPI UpdateCurrentTab(Int32 position) {
        TabInfo tab = mTabs->Get(position);
        mCurTab = tab;

        // Put things in the correct paused/resumed state.
        if (mActivityResumed) {
            mCurTab->Build(mInflater, mContentContainer, mRootView);
            mCurTab->Resume(mSortOrder);
        } else {
            mCurTab->Pause();
        }
        for (Int32 i=0; i<mTabs->Size(); i++) {
            TabInfo t = mTabs->Get(i);
            if (t != mCurTab) {
                t->Pause();
            }
        }

        mCurTab->UpdateStorageUsage();
        UpdateOptionsMenu();
        final Activity host = GetActivity();
        if (host != NULL) {
            host->InvalidateOptionsMenu();
        }
    }

    private volatile IMediaContainerService mContainerService;

    private final ServiceConnection mContainerConnection = new ServiceConnection() {
        //@Override
        CARAPI OnServiceConnected(ComponentName name, IBinder service) {
            mContainerService = IMediaContainerService.Stub->AsInterface(service);
            for (Int32 i=0; i<mTabs->Size(); i++) {
                mTabs->Get(i).SetContainerService(mContainerService);
            }
        }

        //@Override
        CARAPI OnServiceDisconnected(ComponentName name) {
            mContainerService = NULL;
        }
    };
}
