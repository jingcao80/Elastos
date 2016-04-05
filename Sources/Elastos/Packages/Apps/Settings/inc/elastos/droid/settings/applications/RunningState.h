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

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::Drawable::IConstantState;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

/**
 * Singleton for retrieving and monitoring the state about all running
 * applications/processes/services.
 */
public class RunningState {
    static const String TAG = "RunningState";
    static const Boolean DEBUG_COMPARE = FALSE;

    static Object sGlobalLock = new Object();
    static RunningState sInstance;

    static const Int32 MSG_RESET_CONTENTS = 1;
    static const Int32 MSG_UPDATE_CONTENTS = 2;
    static const Int32 MSG_REFRESH_UI = 3;
    static const Int32 MSG_UPDATE_TIME = 4;

    static const Int64 TIME_UPDATE_DELAY = 1000;
    static const Int64 CONTENTS_UPDATE_DELAY = 2000;

    static const Int32 MAX_SERVICES = 100;

    final Context mApplicationContext;
    final ActivityManager mAm;
    final PackageManager mPm;
    final UserManager mUm;
    final Int32 mMyUserId;

    OnRefreshUiListener mRefreshUiListener;

    final InterestingConfigChanges mInterestingConfigChanges = new InterestingConfigChanges();

    // Processes that are hosting a service we are interested in, organized
    // by uid and name.  Note that this mapping does not change even across
    // service restarts, and during a restart there will still be a process
    // entry.
    final SparseArray<HashMap<String, ProcessItem>> mServiceProcessesByName
            = new SparseArray<HashMap<String, ProcessItem>>();
    
    // Processes that are hosting a service we are interested in, organized
    // by their pid.  These disappear and re-appear as services are restarted.
    final SparseArray<ProcessItem> mServiceProcessesByPid
            = new SparseArray<ProcessItem>();
    
    // Used to sort the interesting processes.
    final ServiceProcessComparator mServiceProcessComparator
            = new ServiceProcessComparator();
    
    // Additional interesting processes to be shown to the user, even if
    // there is no service running in them.
    final ArrayList<ProcessItem> mInterestingProcesses = new ArrayList<ProcessItem>();
    
    // All currently running processes, for finding dependencies etc.
    final SparseArray<ProcessItem> mRunningProcesses
            = new SparseArray<ProcessItem>();
    
    // The processes associated with services, in sorted order.
    final ArrayList<ProcessItem> mProcessItems = new ArrayList<ProcessItem>();
    
    // All processes, used for retrieving memory information.
    final ArrayList<ProcessItem> mAllProcessItems = new ArrayList<ProcessItem>();

    // If there are other users on the device, these are the merged items
    // representing all items that would be put in mMergedItems for that user.
    final SparseArray<MergedItem> mOtherUserMergedItems = new SparseArray<MergedItem>();

    // If there are other users on the device, these are the merged items
    // representing all items that would be put in mUserBackgroundItems for that user.
    final SparseArray<MergedItem> mOtherUserBackgroundItems = new SparseArray<MergedItem>();

    // Tracking of information about users.
    final SparseArray<UserState> mUsers = new SparseArray<UserState>();

    static class AppProcessInfo {
        final ActivityManager.RunningAppProcessInfo info;
        Boolean hasServices;
        Boolean hasForegroundServices;

        AppProcessInfo(ActivityManager.RunningAppProcessInfo _info) {
            info = _info;
        }
    }

    // Temporary structure used when updating above information.
    final SparseArray<AppProcessInfo> mTmpAppProcesses = new SparseArray<AppProcessInfo>();

    Int32 mSequence = 0;

    final Comparator<RunningState.MergedItem> mBackgroundComparator
        = new Comparator<RunningState.MergedItem>() {
            //@Override
            public Int32 Compare(MergedItem lhs, MergedItem rhs) {
                if (DEBUG_COMPARE) {
                    Logger::I(TAG, "Comparing " + lhs + " with " + rhs);
                    Logger::I(TAG, "     Proc " + lhs.mProcess + " with " + rhs.mProcess);
                    Logger::I(TAG, "   UserId " + lhs.mUserId + " with " + rhs.mUserId);
                }
                if (lhs.mUserId != rhs.mUserId) {
                    if (lhs.mUserId == mMyUserId) return -1;
                    if (rhs.mUserId == mMyUserId) return 1;
                    return lhs.mUserId < rhs.mUserId ? -1 : 1;
                }
                if (lhs.mProcess == rhs.mProcess) {
                    if (lhs.mLabel == rhs.mLabel) {
                        return 0;
                    }
                    return lhs.mLabel != NULL ? lhs.mLabel->CompareTo(rhs.mLabel) : -1;
                }
                if (lhs.mProcess == NULL) return -1;
                if (rhs.mProcess == NULL) return 1;
                if (DEBUG_COMPARE) Logger::I(TAG, "    Label " + lhs.mProcess.mLabel
                        + " with " + rhs.mProcess.mLabel);
                final ActivityManager.RunningAppProcessInfo lhsInfo
                        = lhs.mProcess.mRunningProcessInfo;
                final ActivityManager.RunningAppProcessInfo rhsInfo
                        = rhs.mProcess.mRunningProcessInfo;
                final Boolean lhsBg = lhsInfo.importance
                        >= ActivityManager.RunningAppProcessInfo.IMPORTANCE_BACKGROUND;
                final Boolean rhsBg = rhsInfo.importance
                        >= ActivityManager.RunningAppProcessInfo.IMPORTANCE_BACKGROUND;
                        if (DEBUG_COMPARE) Logger::I(TAG, "       Bg " + lhsBg + " with " + rhsBg);
                if (lhsBg != rhsBg) {
                    return lhsBg ? 1 : -1;
                }
                final Boolean lhsA = (lhsInfo.flags
                        & ActivityManager.RunningAppProcessInfo.FLAG_HAS_ACTIVITIES) != 0;
                final Boolean rhsA = (rhsInfo.flags
                        & ActivityManager.RunningAppProcessInfo.FLAG_HAS_ACTIVITIES) != 0;
                if (DEBUG_COMPARE) Logger::I(TAG, "      Act " + lhsA + " with " + rhsA);
                if (lhsA != rhsA) {
                    return lhsA ? -1 : 1;
                }
                if (DEBUG_COMPARE) Logger::I(TAG, "      Lru " + lhsInfo.lru + " with " + rhsInfo.lru);
                if (lhsInfo.lru != rhsInfo.lru) {
                    return lhsInfo.lru < rhsInfo.lru ? -1 : 1;
                }
                if (lhs.mProcess.mLabel == rhs.mProcess.mLabel) {
                    return 0;
                }
                if (lhs.mProcess.mLabel == NULL) return 1;
                if (rhs.mProcess.mLabel == NULL) return -1;
                return lhs.mProcess.mLabel->CompareTo(rhs.mProcess.mLabel);
            }
    };

    // ----- following protected by mLock -----
    
    // Lock for protecting the state that will be shared between the
    // background update thread and the UI thread.
    final Object mLock = new Object();
    
    Boolean mResumed;
    Boolean mHaveData;
    Boolean mWatchingBackgroundItems;

    ArrayList<BaseItem> mItems = new ArrayList<BaseItem>();
    ArrayList<MergedItem> mMergedItems = new ArrayList<MergedItem>();
    ArrayList<MergedItem> mBackgroundItems = new ArrayList<MergedItem>();
    ArrayList<MergedItem> mUserBackgroundItems = new ArrayList<MergedItem>();
    
    Int32 mNumBackgroundProcesses;
    Int64 mBackgroundProcessMemory;
    Int32 mNumForegroundProcesses;
    Int64 mForegroundProcessMemory;
    Int32 mNumServiceProcesses;
    Int64 mServiceProcessMemory;

    // ----- BACKGROUND MONITORING THREAD -----

    final HandlerThread mBackgroundThread;
    final class BackgroundHandler extends Handler {
        public BackgroundHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_RESET_CONTENTS:
                    Reset();
                    break;
                case MSG_UPDATE_CONTENTS:
                    synchronized(mLock) {
                        if (!mResumed) {
                            return;
                        }
                    }
                    Message cmd = mHandler->ObtainMessage(MSG_REFRESH_UI);
                    cmd.arg1 = Update(mApplicationContext, mAm) ? 1 : 0;
                    mHandler->SendMessage(cmd);
                    RemoveMessages(MSG_UPDATE_CONTENTS);
                    msg = ObtainMessage(MSG_UPDATE_CONTENTS);
                    SendMessageDelayed(msg, CONTENTS_UPDATE_DELAY);
                    break;
            }
        }
    };

    final BackgroundHandler mBackgroundHandler;

    final Handler mHandler = new Handler() {
        Int32 mNextUpdate = OnRefreshUiListener.REFRESH_TIME;

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_REFRESH_UI:
                    mNextUpdate = msg.arg1 != 0
                            ? OnRefreshUiListener.REFRESH_STRUCTURE
                            : OnRefreshUiListener.REFRESH_DATA;
                    break;
                case MSG_UPDATE_TIME:
                    synchronized(mLock) {
                        if (!mResumed) {
                            return;
                        }
                    }
                    RemoveMessages(MSG_UPDATE_TIME);
                    Message m = ObtainMessage(MSG_UPDATE_TIME);
                    SendMessageDelayed(m, TIME_UPDATE_DELAY);

                    if (mRefreshUiListener != NULL) {
                        //Logger::I("foo", "Refresh UI: " + mNextUpdate
                        //        + " @ " + SystemClock->UptimeMillis());
                        mRefreshUiListener->OnRefreshUi(mNextUpdate);
                        mNextUpdate = OnRefreshUiListener.REFRESH_TIME;
                    }
                    break;
            }
        }
    };

    // ----- DATA STRUCTURES -----

    static interface OnRefreshUiListener {
        public static const Int32 REFRESH_TIME = 0;
        public static const Int32 REFRESH_DATA = 1;
        public static const Int32 REFRESH_STRUCTURE = 2;

        CARAPI OnRefreshUi(Int32 what);
    }

    static class UserState {
        UserInfo mInfo;
        String mLabel;
        Drawable mIcon;
    }

    static class BaseItem {
        final Boolean mIsProcess;
        final Int32 mUserId;

        PackageItemInfo mPackageInfo;
        CharSequence mDisplayLabel;
        String mLabel;
        String mDescription;

        Int32 mCurSeq;

        Int64 mActiveSince;
        Int64 mSize;
        String mSizeStr;
        String mCurSizeStr;
        Boolean mNeedDivider;
        Boolean mBackground;

        public BaseItem(Boolean isProcess, Int32 userId) {
            mIsProcess = isProcess;
            mUserId = userId;
        }

        public Drawable LoadIcon(Context context, RunningState state) {
            if (mPackageInfo != NULL) {
                return mPackageInfo->LoadIcon(state.mPm);
            }
            return NULL;
        }
    }

    static class ServiceItem extends BaseItem {
        ActivityManager.RunningServiceInfo mRunningService;
        ServiceInfo mServiceInfo;
        Boolean mShownAsStarted;
        
        MergedItem mMergedItem;
        
        public ServiceItem(Int32 userId) {
            Super(FALSE, userId);
        }
    }

    static class ProcessItem extends BaseItem {
        final HashMap<ComponentName, ServiceItem> mServices
                = new HashMap<ComponentName, ServiceItem>();
        final SparseArray<ProcessItem> mDependentProcesses
                = new SparseArray<ProcessItem>();
        
        final Int32 mUid;
        final String mProcessName;
        Int32 mPid;
        
        ProcessItem mClient;
        Int32 mLastNumDependentProcesses;
        
        Int32 mRunningSeq;
        ActivityManager.RunningAppProcessInfo mRunningProcessInfo;
        
        MergedItem mMergedItem;

        Boolean mInteresting;

        // Purely for sorting.
        Boolean mIsSystem;
        Boolean mIsStarted;
        Int64 mActiveSince;
        
        public ProcessItem(Context context, Int32 uid, String processName) {
            Super(TRUE, UserHandle->GetUserId(uid));
            mDescription = context->GetResources()->GetString(
                    R::string::service_process_name, processName);
            mUid = uid;
            mProcessName = processName;
        }
        
        void EnsureLabel(PackageManager pm) {
            if (mLabel != NULL) {
                return;
            }
            
            try {
                ApplicationInfo ai = pm->GetApplicationInfo(mProcessName,
                        PackageManager.GET_UNINSTALLED_PACKAGES);
                if (ai.uid == mUid) {
                    mDisplayLabel = ai->LoadLabel(pm);
                    mLabel = mDisplayLabel->ToString();
                    mPackageInfo = ai;
                    return;
                }
            } catch (PackageManager.NameNotFoundException e) {
            }
            
            // If we couldn't get information about the overall
            // process, try to find something about the uid.
            String[] pkgs = pm->GetPackagesForUid(mUid);
            
            // If there is one package with this uid, that is what we want.
            if (pkgs.length == 1) {
                try {
                    ApplicationInfo ai = pm->GetApplicationInfo(pkgs[0],
                            PackageManager.GET_UNINSTALLED_PACKAGES);
                    mDisplayLabel = ai->LoadLabel(pm);
                    mLabel = mDisplayLabel->ToString();
                    mPackageInfo = ai;
                    return;
                } catch (PackageManager.NameNotFoundException e) {
                }
            }
            
            // If there are multiple, see if one gives us the official name
            // for this uid.
            for (String name : pkgs) {
                try {
                    PackageInfo pi = pm->GetPackageInfo(name, 0);
                    if (pi.sharedUserLabel != 0) {
                        CharSequence nm = pm->GetText(name,
                                pi.sharedUserLabel, pi.applicationInfo);
                        if (nm != NULL) {
                            mDisplayLabel = nm;
                            mLabel = nm->ToString();
                            mPackageInfo = pi.applicationInfo;
                            return;
                        }
                    }
                } catch (PackageManager.NameNotFoundException e) {
                }
            }
            
            // If still don't have anything to display, just use the
            // service info.
            if (mServices->Size() > 0) {
                ApplicationInfo ai = mServices->Values()->Iterator().Next()
                        .mServiceInfo.applicationInfo;
                mPackageInfo = ai;
                mDisplayLabel = mPackageInfo->LoadLabel(pm);
                mLabel = mDisplayLabel->ToString();
                return;
            }
            
            // Finally... whatever, just pick the first package's name.
            try {
                ApplicationInfo ai = pm->GetApplicationInfo(pkgs[0],
                        PackageManager.GET_UNINSTALLED_PACKAGES);
                mDisplayLabel = ai->LoadLabel(pm);
                mLabel = mDisplayLabel->ToString();
                mPackageInfo = ai;
                return;
            } catch (PackageManager.NameNotFoundException e) {
            }
        }

        Boolean UpdateService(Context context, ActivityManager.RunningServiceInfo service) {
            final PackageManager pm = context->GetPackageManager();

            Boolean changed = FALSE;
            ServiceItem si = mServices->Get(service.service);
            if (si == NULL) {
                changed = TRUE;
                si = new ServiceItem(mUserId);
                si.mRunningService = service;
                try {
                    si.mServiceInfo = ActivityThread->GetPackageManager()->GetServiceInfo(
                            service.service, PackageManager.GET_UNINSTALLED_PACKAGES,
                            UserHandle->GetUserId(service.uid));

                    if (si.mServiceInfo == NULL) {
                        Logger::D("RunningService", "getServiceInfo returned NULL for: "
                                + service.service);
                        return FALSE;
                    }
                } catch (RemoteException e) {
                }
                si.mDisplayLabel = MakeLabel(pm,
                        si.mRunningService.service->GetClassName(), si.mServiceInfo);
                mLabel = mDisplayLabel != NULL ? mDisplayLabel->ToString() : NULL;
                si.mPackageInfo = si.mServiceInfo.applicationInfo;
                mServices->Put(service.service, si);
            }
            si.mCurSeq = mCurSeq;
            si.mRunningService = service;
            Int64 activeSince = service.restarting == 0 ? service.activeSince : -1;
            if (si.mActiveSince != activeSince) {
                si.mActiveSince = activeSince;
                changed = TRUE;
            }
            if (service.clientPackage != NULL && service.clientLabel != 0) {
                if (si.mShownAsStarted) {
                    si.mShownAsStarted = FALSE;
                    changed = TRUE;
                }
                try {
                    Resources clientr = pm->GetResourcesForApplication(service.clientPackage);
                    String label = clientr->GetString(service.clientLabel);
                    si.mDescription = context->GetResources()->GetString(
                            R::string::service_client_name, label);
                } catch (PackageManager.NameNotFoundException e) {
                    si.mDescription = NULL;
                }
            } else {
                if (!si.mShownAsStarted) {
                    si.mShownAsStarted = TRUE;
                    changed = TRUE;
                }
                si.mDescription = context->GetResources()->GetString(
                        R::string::service_started_by_app);
            }
            
            return changed;
        }
        
        Boolean UpdateSize(Context context, Int64 pss, Int32 curSeq) {
            mSize = pss * 1024;
            if (mCurSeq == curSeq) {
                String sizeStr = Formatter->FormatShortFileSize(
                        context, mSize);
                if (!sizeStr->Equals(mSizeStr)){
                    mSizeStr = sizeStr;
                    // We update this on the second tick where we update just
                    // the text in the current items, so no need to say we
                    // changed here.
                    return FALSE;
                }
            }
            return FALSE;
        }
        
        Boolean BuildDependencyChain(Context context, PackageManager pm, Int32 curSeq) {
            final Int32 NP = mDependentProcesses->Size();
            Boolean changed = FALSE;
            for (Int32 i=0; i<NP; i++) {
                ProcessItem proc = mDependentProcesses->ValueAt(i);
                if (proc.mClient != this) {
                    changed = TRUE;
                    proc.mClient = this;
                }
                proc.mCurSeq = curSeq;
                proc->EnsureLabel(pm);
                changed |= proc->BuildDependencyChain(context, pm, curSeq);
            }
            
            if (mLastNumDependentProcesses != mDependentProcesses->Size()) {
                changed = TRUE;
                mLastNumDependentProcesses = mDependentProcesses->Size();
            }
            
            return changed;
        }
        
        void AddDependentProcesses(ArrayList<BaseItem> dest,
                ArrayList<ProcessItem> destProc) {
            final Int32 NP = mDependentProcesses->Size();
            for (Int32 i=0; i<NP; i++) {
                ProcessItem proc = mDependentProcesses->ValueAt(i);
                proc->AddDependentProcesses(dest, destProc);
                dest->Add(proc);
                if (proc.mPid > 0) {
                    destProc->Add(proc);
                }
            }
        }
    }

    static class MergedItem extends BaseItem {
        ProcessItem mProcess;
        UserState mUser;
        final ArrayList<ProcessItem> mOtherProcesses = new ArrayList<ProcessItem>();
        final ArrayList<ServiceItem> mServices = new ArrayList<ServiceItem>();
        final ArrayList<MergedItem> mChildren = new ArrayList<MergedItem>();
        
        private Int32 mLastNumProcesses = -1, mLastNumServices = -1;

        MergedItem(Int32 userId) {
            Super(FALSE, userId);
        }

        private void SetDescription(Context context, Int32 numProcesses, Int32 numServices) {
            if (mLastNumProcesses != numProcesses || mLastNumServices != numServices) {
                mLastNumProcesses = numProcesses;
                mLastNumServices = numServices;
                Int32 resid = R::string::running_processes_item_description_s_s;
                if (numProcesses != 1) {
                    resid = numServices != 1
                            ? R::string::running_processes_item_description_p_p
                            : R::string::running_processes_item_description_p_s;
                } else if (numServices != 1) {
                    resid = R::string::running_processes_item_description_s_p;
                }
                mDescription = context->GetResources()->GetString(resid, numProcesses,
                        numServices);
            }
        }

        Boolean Update(Context context, Boolean background) {
            mBackground = background;

            if (mUser != NULL) {
                // This is a merged item that contains a child collection
                // of items...  that is, it is an entire user, containing
                // everything associated with that user.  So set it up as such.
                // For concrete stuff we need about the process of this item,
                // we will just use the info from the first child.
                MergedItem child0 = mChildren->Get(0);
                mPackageInfo = child0.mProcess.mPackageInfo;
                mLabel = mUser != NULL ? mUser.mLabel : NULL;
                mDisplayLabel = mLabel;
                Int32 numProcesses = 0;
                Int32 numServices = 0;
                mActiveSince = -1;
                for (Int32 i=0; i<mChildren->Size(); i++) {
                    MergedItem child = mChildren->Get(i);
                    numProcesses += child.mLastNumProcesses;
                    numServices += child.mLastNumServices;
                    if (child.mActiveSince >= 0 && mActiveSince < child.mActiveSince) {
                        mActiveSince = child.mActiveSince;
                    }
                }
                if (!mBackground) {
                    SetDescription(context, numProcesses, numServices);
                }
            } else {
                mPackageInfo = mProcess.mPackageInfo;
                mDisplayLabel = mProcess.mDisplayLabel;
                mLabel = mProcess.mLabel;
                
                if (!mBackground) {
                    SetDescription(context, (mProcess.mPid > 0 ? 1 : 0) + mOtherProcesses->Size(),
                            mServices->Size());
                }
                
                mActiveSince = -1;
                for (Int32 i=0; i<mServices->Size(); i++) {
                    ServiceItem si = mServices->Get(i);
                    if (si.mActiveSince >= 0 && mActiveSince < si.mActiveSince) {
                        mActiveSince = si.mActiveSince;
                    }
                }
            }

            return FALSE;
        }
        
        Boolean UpdateSize(Context context) {
            if (mUser != NULL) {
                mSize = 0;
                for (Int32 i=0; i<mChildren->Size(); i++) {
                    MergedItem child = mChildren->Get(i);
                    child->UpdateSize(context);
                    mSize += child.mSize;
                }
            } else {
                mSize = mProcess.mSize;
                for (Int32 i=0; i<mOtherProcesses->Size(); i++) {
                    mSize += mOtherProcesses->Get(i).mSize;
                }
            }
            
            String sizeStr = Formatter->FormatShortFileSize(
                    context, mSize);
            if (!sizeStr->Equals(mSizeStr)){
                mSizeStr = sizeStr;
                // We update this on the second tick where we update just
                // the text in the current items, so no need to say we
                // changed here.
                return FALSE;
            }
            return FALSE;
        }

        public Drawable LoadIcon(Context context, RunningState state) {
            if (mUser == NULL) {
                return super->LoadIcon(context, state);
            }
            if (mUser.mIcon != NULL) {
                ConstantState constState = mUser.mIcon->GetConstantState();
                if (constState == NULL) {
                    return mUser.mIcon;
                } else {
                    return constState->NewDrawable();
                }
            }
            return context->GetResources()->GetDrawable(
                    R.drawable.ic_menu_cc);
        }
    }

    class ServiceProcessComparator implements Comparator<ProcessItem> {
        public Int32 Compare(ProcessItem object1, ProcessItem object2) {
            if (object1.mUserId != object2.mUserId) {
                if (object1.mUserId == mMyUserId) return -1;
                if (object2.mUserId == mMyUserId) return 1;
                return object1.mUserId < object2.mUserId ? -1 : 1;
            }
            if (object1.mIsStarted != object2.mIsStarted) {
                // Non-started processes go last.
                return object1.mIsStarted ? -1 : 1;
            }
            if (object1.mIsSystem != object2.mIsSystem) {
                // System processes go below non-system.
                return object1.mIsSystem ? 1 : -1;
            }
            if (object1.mActiveSince != object2.mActiveSince) {
                // Remaining ones are sorted with the longest running
                // services last.
                return (object1.mActiveSince > object2.mActiveSince) ? -1 : 1;
            }
            return 0;
        }
    }
    
    static CharSequence MakeLabel(PackageManager pm,
            String className, PackageItemInfo item) {
        if (item != NULL && (item.labelRes != 0
                || item.nonLocalizedLabel != NULL)) {
            CharSequence label = item->LoadLabel(pm);
            if (label != NULL) {
                return label;
            }
        }
        
        String label = className;
        Int32 tail = label->LastIndexOf('.');
        if (tail >= 0) {
            label = label->Substring(tail+1, label->Length());
        }
        return label;
    }
    
    static RunningState GetInstance(Context context) {
        synchronized(sGlobalLock) {
            if (sInstance == NULL) {
                sInstance = new RunningState(context);
            }
            return sInstance;
        }
    }

    private RunningState(Context context) {
        mApplicationContext = context->GetApplicationContext();
        mAm = (ActivityManager)mApplicationContext->GetSystemService(Context.ACTIVITY_SERVICE);
        mPm = mApplicationContext->GetPackageManager();
        mUm = (UserManager)mApplicationContext->GetSystemService(Context.USER_SERVICE);
        mMyUserId = UserHandle->MyUserId();
        mResumed = FALSE;
        mBackgroundThread = new HandlerThread("RunningState:Background");
        mBackgroundThread->Start();
        mBackgroundHandler = new BackgroundHandler(mBackgroundThread->GetLooper());
    }

    void Resume(OnRefreshUiListener listener) {
        synchronized(mLock) {
            mResumed = TRUE;
            mRefreshUiListener = listener;
            if (mInterestingConfigChanges->ApplyNewConfig(mApplicationContext->GetResources())) {
                mHaveData = FALSE;
                mBackgroundHandler->RemoveMessages(MSG_RESET_CONTENTS);
                mBackgroundHandler->RemoveMessages(MSG_UPDATE_CONTENTS);
                mBackgroundHandler->SendEmptyMessage(MSG_RESET_CONTENTS);
            }
            if (!mBackgroundHandler->HasMessages(MSG_UPDATE_CONTENTS)) {
                mBackgroundHandler->SendEmptyMessage(MSG_UPDATE_CONTENTS);
            }
            mHandler->SendEmptyMessage(MSG_UPDATE_TIME);
        }
    }

    void UpdateNow() {
        synchronized(mLock) {
            mBackgroundHandler->RemoveMessages(MSG_UPDATE_CONTENTS);
            mBackgroundHandler->SendEmptyMessage(MSG_UPDATE_CONTENTS);
        }
    }

    Boolean HasData() {
        synchronized(mLock) {
            return mHaveData;
        }
    }

    void WaitForData() {
        synchronized(mLock) {
            while (!mHaveData) {
                try {
                    mLock->Wait(0);
                } catch (InterruptedException e) {
                }
            }
        }
    }

    void Pause() {
        synchronized(mLock) {
            mResumed = FALSE;
            mRefreshUiListener = NULL;
            mHandler->RemoveMessages(MSG_UPDATE_TIME);
        }
    }

    private Boolean IsInterestingProcess(ActivityManager.RunningAppProcessInfo pi) {
        if ((pi.flags&ActivityManager.RunningAppProcessInfo.FLAG_CANT_SAVE_STATE) != 0) {
            return TRUE;
        }
        if ((pi.flags&ActivityManager.RunningAppProcessInfo.FLAG_PERSISTENT) == 0
                && pi.importance >= ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND
                && pi.importance < ActivityManager.RunningAppProcessInfo.IMPORTANCE_CANT_SAVE_STATE
                && pi.importanceReasonCode
                        == ActivityManager.RunningAppProcessInfo.REASON_UNKNOWN) {
            return TRUE;
        }
        return FALSE;
    }

    private void Reset() {
        mServiceProcessesByName->Clear();
        mServiceProcessesByPid->Clear();
        mInterestingProcesses->Clear();
        mRunningProcesses->Clear();
        mProcessItems->Clear();
        mAllProcessItems->Clear();
        mUsers->Clear();
    }

    private void AddOtherUserItem(Context context, ArrayList<MergedItem> newMergedItems,
            SparseArray<MergedItem> userItems, MergedItem newItem) {
        MergedItem userItem = userItems->Get(newItem.mUserId);
        Boolean first = userItem == NULL || userItem.mCurSeq != mSequence;
        if (first) {
            if (userItem == NULL) {
                userItem = new MergedItem(newItem.mUserId);
                userItems->Put(newItem.mUserId, userItem);
            } else {
                userItem.mChildren->Clear();
            }
            userItem.mCurSeq = mSequence;
            if ((userItem.mUser=mUsers->Get(newItem.mUserId)) == NULL) {
                userItem.mUser = new UserState();
                UserInfo info = mUm->GetUserInfo(newItem.mUserId);
                userItem.mUser.mInfo = info;
                if (info != NULL) {
                    userItem.mUser.mIcon = Utils->GetUserIcon(context, mUm, info);
                }
                String name = info != NULL ? info.name : NULL;
                if (name == NULL && info != NULL) {
                    name = Integer->ToString(info.id);
                } else if (info == NULL) {
                    name = context->GetString(R::string::unknown);
                }
                userItem.mUser.mLabel = context->GetResources()->GetString(
                        R::string::running_process_item_user_label, name);
            }
            newMergedItems->Add(userItem);
        }
        userItem.mChildren->Add(newItem);
    }

    private Boolean Update(Context context, ActivityManager am) {
        final PackageManager pm = context->GetPackageManager();
        
        mSequence++;
        
        Boolean changed = FALSE;

        // Retrieve list of services, filtering out anything that definitely
        // won't be shown in the UI.
        List<ActivityManager.RunningServiceInfo> services 
                = am->GetRunningServices(MAX_SERVICES);
        Int32 NS = services != NULL ? services->Size() : 0;
        for (Int32 i=0; i<NS; i++) {
            ActivityManager.RunningServiceInfo si = services->Get(i);
            // We are not interested in services that have not been started
            // and don't have a known client, because
            // there is nothing the user can do about them.
            if (!si.started && si.clientLabel == 0) {
                services->Remove(i);
                i--;
                NS--;
                continue;
            }
            // We likewise don't care about services running in a
            // persistent process like the system or phone.
            if ((si.flags&ActivityManager.RunningServiceInfo.FLAG_PERSISTENT_PROCESS)
                    != 0) {
                services->Remove(i);
                i--;
                NS--;
                continue;
            }
        }

        // Retrieve list of running processes, organizing them into a sparse
        // array for easy retrieval.
        List<ActivityManager.RunningAppProcessInfo> processes
                = am->GetRunningAppProcesses();
        final Int32 NP = processes != NULL ? processes->Size() : 0;
        mTmpAppProcesses->Clear();
        for (Int32 i=0; i<NP; i++) {
            ActivityManager.RunningAppProcessInfo pi = processes->Get(i);
            mTmpAppProcesses->Put(pi.pid, new AppProcessInfo(pi));
        }

        // Initial iteration through running services to collect per-process
        // info about them.
        for (Int32 i=0; i<NS; i++) {
            ActivityManager.RunningServiceInfo si = services->Get(i);
            if (si.restarting == 0 && si.pid > 0) {
                AppProcessInfo ainfo = mTmpAppProcesses->Get(si.pid);
                if (ainfo != NULL) {
                    ainfo.hasServices = TRUE;
                    if (si.foreground) {
                        ainfo.hasForegroundServices = TRUE;
                    }
                }
            }
        }

        // Update state we are maintaining about process that are running services.
        for (Int32 i=0; i<NS; i++) {
            ActivityManager.RunningServiceInfo si = services->Get(i);

            // If this service's process is in use at a higher importance
            // due to another process bound to one of its services, then we
            // won't put it in the top-level list of services.  Instead we
            // want it to be included in the set of processes that the other
            // process needs.
            if (si.restarting == 0 && si.pid > 0) {
                AppProcessInfo ainfo = mTmpAppProcesses->Get(si.pid);
                if (ainfo != NULL && !ainfo.hasForegroundServices) {
                    // This process does not have any foreground services.
                    // If its importance is greater than the service importance
                    // then there is something else more significant that is
                    // keeping it around that it should possibly be included as
                    // a part of instead of being shown by itself.
                    if (ainfo.info.importance
                            < ActivityManager.RunningAppProcessInfo.IMPORTANCE_SERVICE) {
                        // Follow process chain to see if there is something
                        // else that could be shown
                        Boolean skip = FALSE;
                        ainfo = mTmpAppProcesses->Get(ainfo.info.importanceReasonPid);
                        while (ainfo != NULL) {
                            if (ainfo.hasServices || IsInterestingProcess(ainfo.info)) {
                                skip = TRUE;
                                break;
                            }
                            ainfo = mTmpAppProcesses->Get(ainfo.info.importanceReasonPid);
                        }
                        if (skip) {
                            continue;
                        }
                    }
                }
            }

            HashMap<String, ProcessItem> procs = mServiceProcessesByName->Get(si.uid);
            if (procs == NULL) {
                procs = new HashMap<String, ProcessItem>();
                mServiceProcessesByName->Put(si.uid, procs);
            }
            ProcessItem proc = procs->Get(si.process);
            if (proc == NULL) {
                changed = TRUE;
                proc = new ProcessItem(context, si.uid, si.process);
                procs->Put(si.process, proc);
            }
            
            if (proc.mCurSeq != mSequence) {
                Int32 pid = si.restarting == 0 ? si.pid : 0;
                if (pid != proc.mPid) {
                    changed = TRUE;
                    if (proc.mPid != pid) {
                        if (proc.mPid != 0) {
                            mServiceProcessesByPid->Remove(proc.mPid);
                        }
                        if (pid != 0) {
                            mServiceProcessesByPid->Put(pid, proc);
                        }
                        proc.mPid = pid;
                    }
                }
                proc.mDependentProcesses->Clear();
                proc.mCurSeq = mSequence;
            }
            changed |= proc->UpdateService(context, si);
        }
        
        // Now update the map of other processes that are running (but
        // don't have services actively running inside them).
        for (Int32 i=0; i<NP; i++) {
            ActivityManager.RunningAppProcessInfo pi = processes->Get(i);
            ProcessItem proc = mServiceProcessesByPid->Get(pi.pid);
            if (proc == NULL) {
                // This process is not one that is a direct container
                // of a service, so look for it in the secondary
                // running list.
                proc = mRunningProcesses->Get(pi.pid);
                if (proc == NULL) {
                    changed = TRUE;
                    proc = new ProcessItem(context, pi.uid, pi.processName);
                    proc.mPid = pi.pid;
                    mRunningProcesses->Put(pi.pid, proc);
                }
                proc.mDependentProcesses->Clear();
            }
            
            if (IsInterestingProcess(pi)) {
                if (!mInterestingProcesses->Contains(proc)) {
                    changed = TRUE;
                    mInterestingProcesses->Add(proc);
                }
                proc.mCurSeq = mSequence;
                proc.mInteresting = TRUE;
                proc->EnsureLabel(pm);
            } else {
                proc.mInteresting = FALSE;
            }
            
            proc.mRunningSeq = mSequence;
            proc.mRunningProcessInfo = pi;
        }

        // Build the chains from client processes to the process they are
        // dependent on; also remove any old running processes.
        Int32 NRP = mRunningProcesses->Size();
        for (Int32 i = 0; i < NRP;) {
            ProcessItem proc = mRunningProcesses->ValueAt(i);
            if (proc.mRunningSeq == mSequence) {
                Int32 clientPid = proc.mRunningProcessInfo.importanceReasonPid;
                if (clientPid != 0) {
                    ProcessItem client = mServiceProcessesByPid->Get(clientPid);
                    if (client == NULL) {
                        client = mRunningProcesses->Get(clientPid);
                    }
                    if (client != NULL) {
                        client.mDependentProcesses->Put(proc.mPid, proc);
                    }
                } else {
                    // In this pass the process doesn't have a client.
                    // Clear to make sure that, if it later gets the same one,
                    // we will detect the change.
                    proc.mClient = NULL;
                }
                i++;
            } else {
                changed = TRUE;
                mRunningProcesses->Remove(mRunningProcesses->KeyAt(i));
                NRP--;
            }
        }
        
        // Remove any old interesting processes.
        Int32 NHP = mInterestingProcesses->Size();
        for (Int32 i=0; i<NHP; i++) {
            ProcessItem proc = mInterestingProcesses->Get(i);
            if (!proc.mInteresting || mRunningProcesses->Get(proc.mPid) == NULL) {
                changed = TRUE;
                mInterestingProcesses->Remove(i);
                i--;
                NHP--;
            }
        }
        
        // Follow the tree from all primary service processes to all
        // processes they are dependent on, marking these processes as
        // still being active and determining if anything has changed.
        final Int32 NAP = mServiceProcessesByPid->Size();
        for (Int32 i=0; i<NAP; i++) {
            ProcessItem proc = mServiceProcessesByPid->ValueAt(i);
            if (proc.mCurSeq == mSequence) {
                changed |= proc->BuildDependencyChain(context, pm, mSequence);
            }
        }
        
        // Look for services and their primary processes that no longer exist...
        ArrayList<Integer> uidToDelete = NULL;
        for (Int32 i=0; i<mServiceProcessesByName->Size(); i++) {
            HashMap<String, ProcessItem> procs = mServiceProcessesByName->ValueAt(i);
            Iterator<ProcessItem> pit = procs->Values()->Iterator();
            while (pit->HasNext()) {
                ProcessItem pi = pit->Next();
                if (pi.mCurSeq == mSequence) {
                    pi->EnsureLabel(pm);
                    if (pi.mPid == 0) {
                        // Sanity: a non-process can't be dependent on
                        // anything.
                        pi.mDependentProcesses->Clear();
                    }
                } else {
                    changed = TRUE;
                    pit->Remove();
                    if (procs->Size() == 0) {
                        if (uidToDelete == NULL) {
                            uidToDelete = new ArrayList<Integer>();
                        }
                        uidToDelete->Add(mServiceProcessesByName->KeyAt(i));
                    }
                    if (pi.mPid != 0) {
                        mServiceProcessesByPid->Remove(pi.mPid);
                    }
                    continue;
                }
                Iterator<ServiceItem> sit = pi.mServices->Values()->Iterator();
                while (sit->HasNext()) {
                    ServiceItem si = sit->Next();
                    if (si.mCurSeq != mSequence) {
                        changed = TRUE;
                        sit->Remove();
                    }
                }
            }
        }
        
        if (uidToDelete != NULL) {
            for (Int32 i = 0; i < uidToDelete->Size(); i++) {
                Int32 uid = uidToDelete->Get(i);
                mServiceProcessesByName->Remove(uid);
            }
        }

        if (changed) {
            // First determine an order for the services.
            ArrayList<ProcessItem> sortedProcesses = new ArrayList<ProcessItem>();
            for (Int32 i=0; i<mServiceProcessesByName->Size(); i++) {
                for (ProcessItem pi : mServiceProcessesByName->ValueAt(i).Values()) {
                    pi.mIsSystem = FALSE;
                    pi.mIsStarted = TRUE;
                    pi.mActiveSince = Long.MAX_VALUE;
                    for (ServiceItem si : pi.mServices->Values()) {
                        if (si.mServiceInfo != NULL
                                && (si.mServiceInfo.applicationInfo.flags
                                        & ApplicationInfo.FLAG_SYSTEM) != 0) {
                            pi.mIsSystem = TRUE;
                        }
                        if (si.mRunningService != NULL
                                && si.mRunningService.clientLabel != 0) {
                            pi.mIsStarted = FALSE;
                            if (pi.mActiveSince > si.mRunningService.activeSince) {
                                pi.mActiveSince = si.mRunningService.activeSince;
                            }
                        }
                    }
                    sortedProcesses->Add(pi);
                }
            }
            
            Collections->Sort(sortedProcesses, mServiceProcessComparator);
            
            ArrayList<BaseItem> newItems = new ArrayList<BaseItem>();
            ArrayList<MergedItem> newMergedItems = new ArrayList<MergedItem>();
            SparseArray<MergedItem> otherUsers = NULL;
            mProcessItems->Clear();
            for (Int32 i=0; i<sortedProcesses->Size(); i++) {
                ProcessItem pi = sortedProcesses->Get(i);
                pi.mNeedDivider = FALSE;
                
                Int32 firstProc = mProcessItems->Size();
                // First add processes we are dependent on.
                pi->AddDependentProcesses(newItems, mProcessItems);
                // And add the process itself.
                newItems->Add(pi);
                if (pi.mPid > 0) {
                    mProcessItems->Add(pi);
                }
                
                // Now add the services running in it.
                MergedItem mergedItem = NULL;
                Boolean haveAllMerged = FALSE;
                Boolean needDivider = FALSE;
                for (ServiceItem si : pi.mServices->Values()) {
                    si.mNeedDivider = needDivider;
                    needDivider = TRUE;
                    newItems->Add(si);
                    if (si.mMergedItem != NULL) {
                        if (mergedItem != NULL && mergedItem != si.mMergedItem) {
                            haveAllMerged = FALSE;
                        }
                        mergedItem = si.mMergedItem;
                    } else {
                        haveAllMerged = FALSE;
                    }
                }
                
                if (!haveAllMerged || mergedItem == NULL
                        || mergedItem.mServices->Size() != pi.mServices->Size()) {
                    // Whoops, we need to build a new MergedItem!
                    mergedItem = new MergedItem(pi.mUserId);
                    for (ServiceItem si : pi.mServices->Values()) {
                        mergedItem.mServices->Add(si);
                        si.mMergedItem = mergedItem;
                    }
                    mergedItem.mProcess = pi;
                    mergedItem.mOtherProcesses->Clear();
                    for (Int32 mpi=firstProc; mpi<(mProcessItems->Size()-1); mpi++) {
                        mergedItem.mOtherProcesses->Add(mProcessItems->Get(mpi));
                    }
                }
                
                mergedItem->Update(context, FALSE);
                if (mergedItem.mUserId != mMyUserId) {
                    AddOtherUserItem(context, newMergedItems, mOtherUserMergedItems, mergedItem);
                } else {
                    newMergedItems->Add(mergedItem);
                }
            }

            // Finally, interesting processes need to be shown and will
            // go at the top.
            NHP = mInterestingProcesses->Size();
            for (Int32 i=0; i<NHP; i++) {
                ProcessItem proc = mInterestingProcesses->Get(i);
                if (proc.mClient == NULL && proc.mServices->Size() <= 0) {
                    if (proc.mMergedItem == NULL) {
                        proc.mMergedItem = new MergedItem(proc.mUserId);
                        proc.mMergedItem.mProcess = proc;
                    }
                    proc.mMergedItem->Update(context, FALSE);
                    if (proc.mMergedItem.mUserId != mMyUserId) {
                        AddOtherUserItem(context, newMergedItems, mOtherUserMergedItems,
                                proc.mMergedItem);
                    } else {
                        newMergedItems->Add(0, proc.mMergedItem);
                    }
                    mProcessItems->Add(proc);
                }
            }

            // Finally finally, user aggregated merged items need to be
            // updated now that they have all of their children.
            final Int32 NU = mOtherUserMergedItems->Size();
            for (Int32 i=0; i<NU; i++) {
                MergedItem user = mOtherUserMergedItems->ValueAt(i);
                if (user.mCurSeq == mSequence) {
                    user->Update(context, FALSE);
                }
            }

            synchronized(mLock) {
                mItems = newItems;
                mMergedItems = newMergedItems;
            }
        }
        
        // Count number of interesting other (non-active) processes, and
        // build a list of all processes we will retrieve memory for.
        mAllProcessItems->Clear();
        mAllProcessItems->AddAll(mProcessItems);
        Int32 numBackgroundProcesses = 0;
        Int32 numForegroundProcesses = 0;
        Int32 numServiceProcesses = 0;
        NRP = mRunningProcesses->Size();
        for (Int32 i=0; i<NRP; i++) {
            ProcessItem proc = mRunningProcesses->ValueAt(i);
            if (proc.mCurSeq != mSequence) {
                // We didn't hit this process as a dependency on one
                // of our active ones, so add it up if needed.
                if (proc.mRunningProcessInfo.importance >=
                        ActivityManager.RunningAppProcessInfo.IMPORTANCE_BACKGROUND) {
                    numBackgroundProcesses++;
                    mAllProcessItems->Add(proc);
                } else if (proc.mRunningProcessInfo.importance <=
                        ActivityManager.RunningAppProcessInfo.IMPORTANCE_VISIBLE) {
                    numForegroundProcesses++;
                    mAllProcessItems->Add(proc);
                } else {
                    Logger::I("RunningState", "Unknown non-service process: "
                            + proc.mProcessName + " #" + proc.mPid);
                }
            } else {
                numServiceProcesses++;
            }
        }
        
        Int64 backgroundProcessMemory = 0;
        Int64 foregroundProcessMemory = 0;
        Int64 serviceProcessMemory = 0;
        ArrayList<MergedItem> newBackgroundItems = NULL;
        ArrayList<MergedItem> newUserBackgroundItems = NULL;
        Boolean diffUsers = FALSE;
        try {
            final Int32 numProc = mAllProcessItems->Size();
            Int32[] pids = new Int32[numProc];
            for (Int32 i=0; i<numProc; i++) {
                pids[i] = mAllProcessItems->Get(i).mPid;
            }
            Int64[] pss = ActivityManagerNative->GetDefault()
                    .GetProcessPss(pids);
            Int32 bgIndex = 0;
            for (Int32 i=0; i<pids.length; i++) {
                ProcessItem proc = mAllProcessItems->Get(i);
                changed |= proc->UpdateSize(context, pss[i], mSequence);
                if (proc.mCurSeq == mSequence) {
                    serviceProcessMemory += proc.mSize;
                } else if (proc.mRunningProcessInfo.importance >=
                        ActivityManager.RunningAppProcessInfo.IMPORTANCE_BACKGROUND) {
                    backgroundProcessMemory += proc.mSize;
                    MergedItem mergedItem;
                    if (newBackgroundItems != NULL) {
                        mergedItem = proc.mMergedItem = new MergedItem(proc.mUserId);
                        proc.mMergedItem.mProcess = proc;
                        diffUsers |= mergedItem.mUserId != mMyUserId;
                        newBackgroundItems->Add(mergedItem);
                    } else {
                        if (bgIndex >= mBackgroundItems->Size()
                                || mBackgroundItems->Get(bgIndex).mProcess != proc) {
                            newBackgroundItems = new ArrayList<MergedItem>(numBackgroundProcesses);
                            for (Int32 bgi=0; bgi<bgIndex; bgi++) {
                                mergedItem = mBackgroundItems->Get(bgi);
                                diffUsers |= mergedItem.mUserId != mMyUserId;
                                newBackgroundItems->Add(mergedItem);
                            }
                            mergedItem = proc.mMergedItem = new MergedItem(proc.mUserId);
                            proc.mMergedItem.mProcess = proc;
                            diffUsers |= mergedItem.mUserId != mMyUserId;
                            newBackgroundItems->Add(mergedItem);
                        } else {
                            mergedItem = mBackgroundItems->Get(bgIndex);
                        }
                    }
                    mergedItem->Update(context, TRUE);
                    mergedItem->UpdateSize(context);
                    bgIndex++;
                } else if (proc.mRunningProcessInfo.importance <=
                        ActivityManager.RunningAppProcessInfo.IMPORTANCE_VISIBLE) {
                    foregroundProcessMemory += proc.mSize;
                }
            }
        } catch (RemoteException e) {
        }
        
        if (newBackgroundItems == NULL) {
            // One or more at the bottom may no longer exist.
            if (mBackgroundItems->Size() > numBackgroundProcesses) {
                newBackgroundItems = new ArrayList<MergedItem>(numBackgroundProcesses);
                for (Int32 bgi=0; bgi<numBackgroundProcesses; bgi++) {
                    MergedItem mergedItem = mBackgroundItems->Get(bgi);
                    diffUsers |= mergedItem.mUserId != mMyUserId;
                    newBackgroundItems->Add(mergedItem);
                }
            }
        }

        if (newBackgroundItems != NULL) {
            // The background items have changed; we need to re-build the
            // per-user items.
            if (!diffUsers) {
                // Easy: there are no other users, we can just use the same array.
                newUserBackgroundItems = newBackgroundItems;
            } else {
                // We now need to re-build the per-user list so that background
                // items for users are collapsed together.
                newUserBackgroundItems = new ArrayList<MergedItem>();
                final Int32 NB = newBackgroundItems->Size();
                for (Int32 i=0; i<NB; i++) {
                    MergedItem mergedItem = newBackgroundItems->Get(i);
                    if (mergedItem.mUserId != mMyUserId) {
                        AddOtherUserItem(context, newUserBackgroundItems,
                                mOtherUserBackgroundItems, mergedItem);
                    } else {
                        newUserBackgroundItems->Add(mergedItem);
                    }
                }
                // And user aggregated merged items need to be
                // updated now that they have all of their children.
                final Int32 NU = mOtherUserBackgroundItems->Size();
                for (Int32 i=0; i<NU; i++) {
                    MergedItem user = mOtherUserBackgroundItems->ValueAt(i);
                    if (user.mCurSeq == mSequence) {
                        user->Update(context, TRUE);
                        user->UpdateSize(context);
                    }
                }
            }
        }

        for (Int32 i=0; i<mMergedItems->Size(); i++) {
            mMergedItems->Get(i).UpdateSize(context);
        }
        
        synchronized(mLock) {
            mNumBackgroundProcesses = numBackgroundProcesses;
            mNumForegroundProcesses = numForegroundProcesses;
            mNumServiceProcesses = numServiceProcesses;
            mBackgroundProcessMemory = backgroundProcessMemory;
            mForegroundProcessMemory = foregroundProcessMemory;
            mServiceProcessMemory = serviceProcessMemory;
            if (newBackgroundItems != NULL) {
                mBackgroundItems = newBackgroundItems;
                mUserBackgroundItems = newUserBackgroundItems;
                if (mWatchingBackgroundItems) {
                    changed = TRUE;
                }
            }
            if (!mHaveData) {
                mHaveData = TRUE;
                mLock->NotifyAll();
            }
        }
        
        return changed;
    }
    
    ArrayList<BaseItem> GetCurrentItems() {
        synchronized(mLock) {
            return mItems;
        }
    }
    
    void SetWatchingBackgroundItems(Boolean watching) {
        synchronized(mLock) {
            mWatchingBackgroundItems = watching;
        }
    }

    ArrayList<MergedItem> GetCurrentMergedItems() {
        synchronized(mLock) {
            return mMergedItems;
        }
    }

    ArrayList<MergedItem> GetCurrentBackgroundItems() {
        synchronized(mLock) {
            return mUserBackgroundItems;
        }
    }
}
