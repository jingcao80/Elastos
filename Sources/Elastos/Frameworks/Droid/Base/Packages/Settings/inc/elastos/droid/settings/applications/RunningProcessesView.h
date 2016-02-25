/*
 * Copyright (C) 2010 The Android Open Source Project
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

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Settings::IR;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::AbsListView::IRecyclerListener;
using Elastos::Droid::Settings::ISettingsActivity;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;

public class RunningProcessesView extends FrameLayout
        implements AdapterView.OnItemClickListener, RecyclerListener,
        RunningState.OnRefreshUiListener {
    
    final Int32 mMyUserId;

    Int64 SECONDARY_SERVER_MEM;
    
    final HashMap<View, ActiveItem> mActiveItems = new HashMap<View, ActiveItem>();

    ActivityManager mAm;
    
    RunningState mState;
    
    Fragment mOwner;
    
    Runnable mDataAvail;

    StringBuilder mBuilder = new StringBuilder(128);
    
    RunningState.BaseItem mCurSelected;
    
    ListView mListView;
    View mHeader;
    ServiceListAdapter mAdapter;
    LinearColorBar mColorBar;
    TextView mBackgroundProcessPrefix;
    TextView mAppsProcessPrefix;
    TextView mForegroundProcessPrefix;
    TextView mBackgroundProcessText;
    TextView mAppsProcessText;
    TextView mForegroundProcessText;

    Int64 mCurTotalRam = -1;
    Int64 mCurHighRam = -1;      // "System" or "Used"
    Int64 mCurMedRam = -1;       // "Apps" or "Cached"
    Int64 mCurLowRam = -1;       // "Free"
    Boolean mCurShowCached = FALSE;

    Dialog mCurDialog;

    MemInfoReader mMemInfoReader = new MemInfoReader();

    public static class ActiveItem {
        View mRootView;
        RunningState.BaseItem mItem;
        ActivityManager.RunningServiceInfo mService;
        ViewHolder mHolder;
        Int64 mFirstRunTime;
        Boolean mSetBackground;

        void UpdateTime(Context context, StringBuilder builder) {
            TextView uptimeView = NULL;
            
            if (mItem instanceof RunningState.ServiceItem) {
                // If we are displaying a service, then the service
                // uptime goes at the top.
                uptimeView = mHolder.size;
                
            } else {
                String size = mItem.mSizeStr != NULL ? mItem.mSizeStr : "";
                if (!size->Equals(mItem.mCurSizeStr)) {
                    mItem.mCurSizeStr = size;
                    mHolder.size->SetText(size);
                }
                
                if (mItem.mBackground) {
                    // This is a background process; no uptime.
                    if (!mSetBackground) {
                        mSetBackground = TRUE;
                        mHolder.uptime->SetText("");
                    }
                } else if (mItem instanceof RunningState.MergedItem) {
                    // This item represents both services and processes,
                    // so show the service uptime below.
                    uptimeView = mHolder.uptime;
                }
            }

            if (uptimeView != NULL) {
                mSetBackground = FALSE;
                if (mFirstRunTime >= 0) {
                    //Logger::I("foo", "Time for " + mItem.mDisplayLabel
                    //        + ": " + (SystemClock->UptimeMillis()-mFirstRunTime));
                    uptimeView->SetText(DateUtils->FormatElapsedTime(builder,
                            (SystemClock->ElapsedRealtime()-mFirstRunTime)/1000));
                } else {
                    Boolean isService = FALSE;
                    if (mItem instanceof RunningState.MergedItem) {
                        isService = ((RunningState.MergedItem)mItem).mServices->Size() > 0;
                    }
                    if (isService) {
                        uptimeView->SetText(context->GetResources()->GetText(
                                R::string::service_restarting));
                    } else {
                        uptimeView->SetText("");
                    }
                }
            }
        }
    }
    
    public static class ViewHolder {
        public View rootView;
        public ImageView icon;
        public TextView name;
        public TextView description;
        public TextView size;
        public TextView uptime;
        
        public ViewHolder(View v) {
            rootView = v;
            icon = (ImageView)v->FindViewById(R.id.icon);
            name = (TextView)v->FindViewById(R.id.name);
            description = (TextView)v->FindViewById(R.id.description);
            size = (TextView)v->FindViewById(R.id.size);
            uptime = (TextView)v->FindViewById(R.id.uptime);
            v->SetTag(this);
        }
        
        public ActiveItem Bind(RunningState state, RunningState.BaseItem item,
                StringBuilder builder) {
            synchronized(state.mLock) {
                PackageManager pm = rootView->GetContext()->GetPackageManager();
                if (item.mPackageInfo == NULL && item instanceof RunningState.MergedItem) {
                    // Items for background processes don't normally load
                    // their labels for performance reasons.  Do it now.
                    RunningState.MergedItem mergedItem = (RunningState.MergedItem)item;
                    if (mergedItem.mProcess != NULL) {
                        ((RunningState.MergedItem)item).mProcess->EnsureLabel(pm);
                        item.mPackageInfo = ((RunningState.MergedItem)item).mProcess.mPackageInfo;
                        item.mDisplayLabel = ((RunningState.MergedItem)item).mProcess.mDisplayLabel;
                    }
                }
                name->SetText(item.mDisplayLabel);
                ActiveItem ai = new ActiveItem();
                ai.mRootView = rootView;
                ai.mItem = item;
                ai.mHolder = this;
                ai.mFirstRunTime = item.mActiveSince;
                if (item.mBackground) {
                    description->SetText(rootView->GetContext()->GetText(R::string::cached));
                } else {
                    description->SetText(item.mDescription);
                }
                item.mCurSizeStr = NULL;
                icon->SetImageDrawable(item->LoadIcon(rootView->GetContext(), state));
                icon->SetVisibility(View.VISIBLE);
                ai->UpdateTime(rootView->GetContext(), builder);
                return ai;
            }
        }
    }
    
    static class TimeTicker extends TextView {
        public TimeTicker(Context context, AttributeSet attrs) {
            Super(context, attrs);
        }
    }
    
    class ServiceListAdapter extends BaseAdapter {
        final RunningState mState;
        final LayoutInflater mInflater;
        Boolean mShowBackground;
        ArrayList<RunningState.MergedItem> mOrigItems;
        final ArrayList<RunningState.MergedItem> mItems
                = new ArrayList<RunningState.MergedItem>();
        
        ServiceListAdapter(RunningState state) {
            mState = state;
            mInflater = (LayoutInflater)GetContext()->GetSystemService(
                    Context.LAYOUT_INFLATER_SERVICE);
            RefreshItems();
        }

        void SetShowBackground(Boolean showBackground) {
            if (mShowBackground != showBackground) {
                mShowBackground = showBackground;
                mState->SetWatchingBackgroundItems(showBackground);
                RefreshItems();
                RefreshUi(TRUE);
            }
        }

        Boolean GetShowBackground() {
            return mShowBackground;
        }

        void RefreshItems() {
            ArrayList<RunningState.MergedItem> newItems =
                mShowBackground ? mState->GetCurrentBackgroundItems()
                        : mState->GetCurrentMergedItems();
            if (mOrigItems != newItems) {
                mOrigItems = newItems;
                if (newItems == NULL) {
                    mItems->Clear();
                } else {
                    mItems->Clear();
                    mItems->AddAll(newItems);
                    if (mShowBackground) {
                        Collections->Sort(mItems, mState.mBackgroundComparator);
                    }
                }
            }
        }
        
        public Boolean HasStableIds() {
            return TRUE;
        }
        
        public Int32 GetCount() {
            return mItems->Size();
        }

        //@Override
        public Boolean IsEmpty() {
            return mState->HasData() && mItems->Size() == 0;
        }

        public Object GetItem(Int32 position) {
            return mItems->Get(position);
        }

        public Int64 GetItemId(Int32 position) {
            return mItems->Get(position).HashCode();
        }

        public Boolean AreAllItemsEnabled() {
            return FALSE;
        }

        public Boolean IsEnabled(Int32 position) {
            return !mItems->Get(position).mIsProcess;
        }

        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            View v;
            if (convertView == NULL) {
                v = NewView(parent);
            } else {
                v = convertView;
            }
            BindView(v, position);
            return v;
        }
        
        public View NewView(ViewGroup parent) {
            View v = mInflater->Inflate(R.layout.running_processes_item, parent, FALSE);
            new ViewHolder(v);
            return v;
        }
        
        CARAPI BindView(View view, Int32 position) {
            synchronized(mState.mLock) {
                if (position >= mItems->Size()) {
                    // List must have changed since we last reported its
                    // size...  ignore here, we will be doing a data changed
                    // to refresh the entire list.
                    return;
                }
                ViewHolder vh = (ViewHolder) view->GetTag();
                RunningState.MergedItem item = mItems->Get(position);
                ActiveItem ai = vh->Bind(mState, item, mBuilder);
                mActiveItems->Put(view, ai);
            }
        }
    }

    void RefreshUi(Boolean dataChanged) {
        if (dataChanged) {
            ServiceListAdapter adapter = mAdapter;
            adapter->RefreshItems();
            adapter->NotifyDataSetChanged();
        }
        
        if (mDataAvail != NULL) {
            mDataAvail->Run();
            mDataAvail = NULL;
        }

        mMemInfoReader->ReadMemInfo();

        /*
        // This is the amount of available memory until we start killing
        // background services.
        Int64 availMem = mMemInfoReader->GetFreeSize() + mMemInfoReader->GetCachedSize()
                - SECONDARY_SERVER_MEM;
        if (availMem < 0) {
            availMem = 0;
        }
        */

        synchronized(mState.mLock) {
            if (mCurShowCached != mAdapter.mShowBackground) {
                mCurShowCached = mAdapter.mShowBackground;
                if (mCurShowCached) {
                    mForegroundProcessPrefix->SetText(GetResources()->GetText(
                            R::string::running_processes_header_used_prefix));
                    mAppsProcessPrefix->SetText(GetResources()->GetText(
                            R::string::running_processes_header_cached_prefix));
                } else {
                    mForegroundProcessPrefix->SetText(GetResources()->GetText(
                            R::string::running_processes_header_system_prefix));
                    mAppsProcessPrefix->SetText(GetResources()->GetText(
                            R::string::running_processes_header_apps_prefix));
                }
            }

            final Int64 totalRam = mMemInfoReader->GetTotalSize();
            final Int64 medRam;
            final Int64 lowRam;
            if (mCurShowCached) {
                lowRam = mMemInfoReader->GetFreeSize() + mMemInfoReader->GetCachedSize();
                medRam = mState.mBackgroundProcessMemory;
            } else {
                lowRam = mMemInfoReader->GetFreeSize() + mMemInfoReader->GetCachedSize()
                        + mState.mBackgroundProcessMemory;
                medRam = mState.mServiceProcessMemory;

            }
            final Int64 highRam = totalRam - medRam - lowRam;

            if (mCurTotalRam != totalRam || mCurHighRam != highRam || mCurMedRam != medRam
                    || mCurLowRam != lowRam) {
                mCurTotalRam = totalRam;
                mCurHighRam = highRam;
                mCurMedRam = medRam;
                mCurLowRam = lowRam;
                BidiFormatter bidiFormatter = BidiFormatter->GetInstance();
                String sizeStr = bidiFormatter->UnicodeWrap(
                        Formatter->FormatShortFileSize(GetContext(), lowRam));
                mBackgroundProcessText->SetText(GetResources()->GetString(
                        R::string::running_processes_header_ram, sizeStr));
                sizeStr = bidiFormatter->UnicodeWrap(
                        Formatter->FormatShortFileSize(GetContext(), medRam));
                mAppsProcessText->SetText(GetResources()->GetString(
                        R::string::running_processes_header_ram, sizeStr));
                sizeStr = bidiFormatter->UnicodeWrap(
                        Formatter->FormatShortFileSize(GetContext(), highRam));
                mForegroundProcessText->SetText(GetResources()->GetString(
                        R::string::running_processes_header_ram, sizeStr));
                mColorBar->SetRatios(highRam/(Float)totalRam,
                        medRam/(Float)totalRam,
                        lowRam/(Float)totalRam);
            }
        }
    }
    
    CARAPI OnItemClick(AdapterView<?> parent, View v, Int32 position, Int64 id) {
        ListView l = (ListView)parent;
        RunningState.MergedItem mi = (RunningState.MergedItem)l->GetAdapter()->GetItem(position);
        mCurSelected = mi;
        StartServiceDetailsActivity(mi);
    }

    // utility method used to start sub activity
    private void StartServiceDetailsActivity(RunningState.MergedItem mi) {
        if (mOwner != NULL) {
            // start new fragment to display extended information
            Bundle args = new Bundle();
            if (mi.mProcess != NULL) {
                args->PutInt(RunningServiceDetails.KEY_UID, mi.mProcess.mUid);
                args->PutString(RunningServiceDetails.KEY_PROCESS, mi.mProcess.mProcessName);
            }
            args->PutInt(RunningServiceDetails.KEY_USER_ID, mi.mUserId);
            args->PutBoolean(RunningServiceDetails.KEY_BACKGROUND, mAdapter.mShowBackground);

            SettingsActivity sa = (SettingsActivity) mOwner->GetActivity();
            sa->StartPreferencePanel(RunningServiceDetails.class->GetName(), args,
                    R::string::runningservicedetails_settings_title, NULL, NULL, 0);
        }
    }
    
    CARAPI OnMovedToScrapHeap(View view) {
        mActiveItems->Remove(view);
    }

    public RunningProcessesView(Context context, AttributeSet attrs) {
        Super(context, attrs);
        mMyUserId = UserHandle->MyUserId();
    }
    
    CARAPI DoCreate(Bundle savedInstanceState) {
        mAm = (ActivityManager)GetContext()->GetSystemService(Context.ACTIVITY_SERVICE);
        mState = RunningState->GetInstance(GetContext());
        LayoutInflater inflater = (LayoutInflater)GetContext()->GetSystemService(
                Context.LAYOUT_INFLATER_SERVICE);
        inflater->Inflate(R.layout.running_processes_view, this);
        mListView = (ListView)FindViewById(android.R.id.list);
        View emptyView = FindViewById(R.id.empty);
        if (emptyView != NULL) {
            mListView->SetEmptyView(emptyView);
        }
        mListView->SetOnItemClickListener(this);
        mListView->SetRecyclerListener(this);
        mAdapter = new ServiceListAdapter(mState);
        mListView->SetAdapter(mAdapter);
        mHeader = inflater->Inflate(R.layout.running_processes_header, NULL);
        mListView->AddHeaderView(mHeader, NULL, FALSE /* set as not selectable */);
        mColorBar = (LinearColorBar)mHeader->FindViewById(R.id.color_bar);
        Resources res = GetResources();
        mColorBar->SetColors(res->GetColor(R::Color::running_processes_system_ram),
                res->GetColor(R::Color::running_processes_apps_ram),
                res->GetColor(R::Color::running_processes_free_ram));
        mBackgroundProcessPrefix = (TextView)mHeader->FindViewById(R.id.freeSizePrefix);
        mAppsProcessPrefix = (TextView)mHeader->FindViewById(R.id.appsSizePrefix);
        mForegroundProcessPrefix = (TextView)mHeader->FindViewById(R.id.systemSizePrefix);
        mBackgroundProcessText = (TextView)mHeader->FindViewById(R.id.freeSize);
        mAppsProcessText = (TextView)mHeader->FindViewById(R.id.appsSize);
        mForegroundProcessText = (TextView)mHeader->FindViewById(R.id.systemSize);

        ActivityManager.MemoryInfo memInfo = new ActivityManager->MemoryInfo();
        mAm->GetMemoryInfo(memInfo);
        SECONDARY_SERVER_MEM = memInfo.secondaryServerThreshold;
    }
    
    CARAPI DoPause() {
        mState->Pause();
        mDataAvail = NULL;
        mOwner = NULL;
    }

    public Boolean DoResume(Fragment owner, Runnable dataAvail) {
        mOwner = owner;
        mState->Resume(this);
        if (mState->HasData()) {
            // If the state already has its data, then let's populate our
            // list right now to avoid flicker.
            RefreshUi(TRUE);
            return TRUE;
        }
        mDataAvail = dataAvail;
        return FALSE;
    }

    void UpdateTimes() {
        Iterator<ActiveItem> it = mActiveItems->Values()->Iterator();
        while (it->HasNext()) {
            ActiveItem ai = it->Next();
            if (ai.mRootView->GetWindowToken() == NULL) {
                // Clean out any dead views, just in case.
                it->Remove();
                continue;
            }
            ai->UpdateTime(GetContext(), mBuilder);
        }
    }

    //@Override
    CARAPI OnRefreshUi(Int32 what) {
        switch (what) {
            case REFRESH_TIME:
                UpdateTimes();
                break;
            case REFRESH_DATA:
                RefreshUi(FALSE);
                UpdateTimes();
                break;
            case REFRESH_STRUCTURE:
                RefreshUi(TRUE);
                UpdateTimes();
                break;
        }
    }
}
