
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/settings/applications/ApplicationsState.h"
#include "elastos/droid/settings/applications/CApplicationsStatePackageStatsObserver.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/regex/Pattern.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::EIID_IIPackageStatsObserver;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::IThread;
using Elastos::IO::CFile;
using Elastos::Text::INormalizer;
using Elastos::Text::CNormalizerHelper;
using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::NormalizerForm_NFD;
using Elastos::Utility::ICollection;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

const AutoPtr<IComparator> ApplicationsState::ALPHA_COMPARATOR = new InitComparator(0);

const AutoPtr<IComparator> ApplicationsState::SIZE_COMPARATOR = new InitComparator(1);

const AutoPtr<IComparator> ApplicationsState::INTERNAL_SIZE_COMPARATOR = new InitComparator(2);

const AutoPtr<IComparator> ApplicationsState::EXTERNAL_SIZE_COMPARATOR = new InitComparator(3);

const AutoPtr<IApplicationsStateAppFilter> ApplicationsState::THIRD_PARTY_FILTER = new InitAppFilter(0);

const AutoPtr<IApplicationsStateAppFilter> ApplicationsState::ON_SD_CARD_FILTER = new InitAppFilter1();

const AutoPtr<IApplicationsStateAppFilter> ApplicationsState::DISABLED_FILTER = new InitAppFilter(1);

const AutoPtr<IApplicationsStateAppFilter> ApplicationsState::ALL_ENABLED_FILTER = new InitAppFilter(2);

const String ApplicationsState::TAG("ApplicationsState");
const Boolean ApplicationsState::DEBUG = FALSE;
const Boolean ApplicationsState::DEBUG_LOCKING = FALSE;

const Int32 ApplicationsState::SIZE_UNKNOWN = -1;
const Int32 ApplicationsState::SIZE_INVALID = -2;

static const AutoPtr<IPattern> InitREMOVE_DIACRITICALS_PATTERN()
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile(String("\\p{InCombiningDiacriticalMarks}+"), (IPattern**)&pattern);
    return pattern;
}

const AutoPtr<IPattern> ApplicationsState::REMOVE_DIACRITICALS_PATTERN = InitREMOVE_DIACRITICALS_PATTERN();

AutoPtr<Object> ApplicationsState::sLock = new Object();
AutoPtr<ApplicationsState> ApplicationsState::sInstance;

const Int32 ApplicationsState::MainHandler::MSG_REBUILD_COMPLETE;
const Int32 ApplicationsState::MainHandler::MSG_PACKAGE_LIST_CHANGED;
const Int32 ApplicationsState::MainHandler::MSG_PACKAGE_ICON_CHANGED;
const Int32 ApplicationsState::MainHandler::MSG_PACKAGE_SIZE_CHANGED;
const Int32 ApplicationsState::MainHandler::MSG_ALL_SIZES_COMPUTED;
const Int32 ApplicationsState::MainHandler::MSG_RUNNING_STATE_CHANGED;

const Int32 ApplicationsState::BackgroundHandler::MSG_REBUILD_LIST;
const Int32 ApplicationsState::BackgroundHandler::MSG_LOAD_ENTRIES;
const Int32 ApplicationsState::BackgroundHandler::MSG_LOAD_ICONS;
const Int32 ApplicationsState::BackgroundHandler::MSG_LOAD_SIZES;

//===============================================================================
//                  CanBeOnSdCardChecker
//===============================================================================

CanBeOnSdCardChecker::CanBeOnSdCardChecker()
    : mInstallLocation(0)
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("package"));
    mPm = IIPackageManager::Probe(obj);
}

void CanBeOnSdCardChecker::Init()
{
    ECode ec = mPm->GetInstallLocation(&mInstallLocation);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E("CanBeOnSdCardChecker", "Is Package Manager running?");
        return;
    }
}

Boolean CanBeOnSdCardChecker::Check(
    /* [in] */ IApplicationInfo* info)
{
    Boolean canBe = FALSE;
    Int32 flags;
    info->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
        canBe = TRUE;
    }
    else {
        if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
            Int32 installLocation;
            info->GetInstallLocation(&installLocation);
            if (installLocation == IPackageInfo::INSTALL_LOCATION_PREFER_EXTERNAL ||
                    installLocation == IPackageInfo::INSTALL_LOCATION_AUTO) {
                canBe = TRUE;
            }
            else if (installLocation
                    == IPackageInfo::INSTALL_LOCATION_UNSPECIFIED) {
                if (mInstallLocation == IPackageHelper::APP_INSTALL_EXTERNAL) {
                    // For apps with no preference and the default value set
                    // to install on sdcard.
                    canBe = TRUE;
                }
            }
        }
    }
    return canBe;
}

//===============================================================================
//                  ApplicationsState::SizeInfo
//===============================================================================

ApplicationsState::SizeInfo::SizeInfo()
    : mCacheSize(0)
    , mCodeSize(0)
    , mDataSize(0)
    , mExternalCodeSize(0)
    , mExternalDataSize(0)
    , mExternalCacheSize(0)
{}

ApplicationsState::SizeInfo::~SizeInfo()
{}

//===============================================================================
//                  ApplicationsState::AppEntry
//===============================================================================

CAR_INTERFACE_IMPL(ApplicationsState::AppEntry, SizeInfo, IApplicationsStateAppEntry)

ApplicationsState::AppEntry::AppEntry(
    /* [in] */ IContext* context,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Int64 id)
    : mId(0)
    , mSize(0)
    , mInternalSize(0)
    , mExternalSize(0)
    , mMounted(FALSE)
    , mSizeStale(FALSE)
    , mSizeLoadStart(0)
{
    String sourceDir;
    info->GetSourceDir(&sourceDir);
    CFile::New(sourceDir, (IFile**)&mApkFile);
    mId = id;
    mInfo = info;
    mSize = SIZE_UNKNOWN;
    mSizeStale = TRUE;
    EnsureLabel(context);
}

ApplicationsState::AppEntry::~AppEntry()
{}

String ApplicationsState::AppEntry::GetNormalizedLabel()
{
    if (!mNormalizedLabel.IsNull()) {
        return mNormalizedLabel;
    }
    mNormalizedLabel = Normalize(mLabel);
    return mNormalizedLabel;
}

void ApplicationsState::AppEntry::EnsureLabel(
    /* [in] */ IContext* context)
{
    if (mLabel.IsNull() || !mMounted) {
        Boolean res;
        if (mApkFile->Exists(&res), res) {
            mMounted = FALSE;
            IPackageItemInfo::Probe(mInfo)->GetPackageName(&mLabel);
        }
        else {
            mMounted = TRUE;
            AutoPtr<IPackageManager> pm;
            context->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<ICharSequence> label;
            IPackageItemInfo::Probe(mInfo)->LoadLabel(pm, (ICharSequence**)&label);
            if (label != NULL) {
                label->ToString(&mLabel);
            }
            else {
                IPackageItemInfo::Probe(mInfo)->GetPackageName(&mLabel);
            }
        }
    }
}

Boolean ApplicationsState::AppEntry::EnsureIconLocked(
    /* [in] */ IContext* context,
    /* [in] */ IPackageManager* pm)
{
    Boolean res;
    if (mIcon == NULL) {
        if (mApkFile->Exists(&res), res) {
            IPackageItemInfo::Probe(mInfo)->LoadIcon(pm, (IDrawable**)&mIcon);
            return TRUE;
        }
        else {
            mMounted = FALSE;
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            mIcon = NULL;
            resources->GetDrawable(
                    Elastos::Droid::R::drawable::sym_app_on_sd_unavailable_icon, (IDrawable**)&mIcon);
        }
    }
    else if (!mMounted) {
        // If the app wasn't mounted but is now mounted, reload
        // its icon.
        if (mApkFile->Exists(&res), res) {
            mMounted = TRUE;
            IPackageItemInfo::Probe(mInfo)->LoadIcon(pm, (IDrawable**)&mIcon);
            return TRUE;
        }
    }
    return FALSE;
}

//===============================================================================
//                  ApplicationsState::InitComparator
//===============================================================================

CAR_INTERFACE_IMPL(ApplicationsState::InitComparator, Object, IComparator)

ApplicationsState::InitComparator::InitComparator(
    /* [in] */ Int32 id)
    : mId(id)
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&sCollator);
}

ApplicationsState::InitComparator::~InitComparator()
{}

ECode ApplicationsState::InitComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<AppEntry> object1 = (AppEntry*)IApplicationsStateAppEntry::Probe(lhs);
    AutoPtr<AppEntry> object2 = (AppEntry*)IApplicationsStateAppEntry::Probe(rhs);

    switch (mId) {
        case 0: {
            Boolean enabled1, enabled2;
            object1->mInfo->GetEnabled(&enabled1);
            object2->mInfo->GetEnabled(&enabled2);

            Int32 flags1, flags2;
            object1->mInfo->GetFlags(&flags1);
            object2->mInfo->GetFlags(&flags2);

            const Boolean normal1 = enabled1 && (flags1 & IApplicationInfo::FLAG_INSTALLED) != 0;
            const Boolean normal2 = enabled2 && (flags2 & IApplicationInfo::FLAG_INSTALLED) != 0;
            if (normal1 != normal2) {
                *result = normal1 ? -1 : 1;
                return NOERROR;
            }
            return sCollator->Compare(object1->mLabel, object2->mLabel, result);
        }

        case 1: {
            if (object1->mSize < object2->mSize) {
                *result = 1;
                return NOERROR;
            }
            if (object1->mSize > object2->mSize) {
                *result = -1;
                return NOERROR;
            }
            return sCollator->Compare(object1->mLabel, object2->mLabel, result);
        }

        case 2: {
            if (object1->mInternalSize < object2->mInternalSize) {
                *result = 1;
                return NOERROR;
            }
            if (object1->mInternalSize > object2->mInternalSize) {
                *result = -1;
                return NOERROR;
            }
            return sCollator->Compare(object1->mLabel, object2->mLabel, result);
        }

        case 3: {
            if (object1->mExternalSize < object2->mExternalSize) {
                *result = 1;
                return NOERROR;
            }
            if (object1->mExternalSize > object2->mExternalSize) {
                *result = -1;
                return NOERROR;
            }
            return sCollator->Compare(object1->mLabel, object2->mLabel, result);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::InitAppFilter
//===============================================================================

CAR_INTERFACE_IMPL(ApplicationsState::InitAppFilter, Object, IApplicationsStateAppFilter)

ApplicationsState::InitAppFilter::InitAppFilter(
    /* [in] */ Int32 id)
    : mId(id)
{}

ApplicationsState::InitAppFilter::~InitAppFilter()
{}

ECode ApplicationsState::InitAppFilter::Init()
{
    return NOERROR;
}

ECode ApplicationsState::InitAppFilter::FilterApp(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (mId) {
        case 0: {
            Int32 flags;
            info->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0) {
                *result = TRUE;
                return NOERROR;
            }
            else if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        }

        case 1: {
            Boolean enabled;
            info->GetEnabled(&enabled);
            if (!enabled) {
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        }

        case 2: {
            Boolean enabled;
            info->GetEnabled(&enabled);
            if (enabled) {
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::InitAppFilter1
//===============================================================================

CAR_INTERFACE_IMPL(ApplicationsState::InitAppFilter1, Object, IApplicationsStateAppFilter)

ApplicationsState::InitAppFilter1::InitAppFilter1()
{
    mCanBeOnSdCardChecker = new CanBeOnSdCardChecker();
}

ApplicationsState::InitAppFilter1::~InitAppFilter1()
{}

ECode ApplicationsState::InitAppFilter1::Init()
{
    mCanBeOnSdCardChecker->Init();
    return NOERROR;
}

ECode ApplicationsState::InitAppFilter1::FilterApp(
    /* [in] */ IApplicationInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCanBeOnSdCardChecker->Check(info);
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::Session
//===============================================================================

ApplicationsState::Session::Session(
    /* [in] */ IApplicationsStateCallbacks* callbacks,
    /* [in] */ ApplicationsState* host)
    : mResumed(FALSE)
    , mRebuildRequested(FALSE)
    , mRebuildAsync(FALSE)
    , mHost(host)
{
    mRebuildSync = new Object();
    mCallbacks = callbacks;
}

ECode ApplicationsState::Session::Resume()
{
    if (DEBUG_LOCKING) Logger::V(TAG, "resume about to acquire lock...");
    {
        AutoLock syncLock(mHost->mEntriesMap);
        if (!mResumed) {
            mResumed = TRUE;
            mHost->mSessionsChanged = TRUE;
            mHost->DoResumeIfNeededLocked();
        }
    }
    if (DEBUG_LOCKING) Logger::V(TAG, "...resume releasing lock");
    return NOERROR;
}

ECode ApplicationsState::Session::Pause()
{
    if (DEBUG_LOCKING) Logger::V(TAG, "pause about to acquire lock...");
    {
        AutoLock syncLock(mHost->mEntriesMap);
        if (mResumed) {
            mResumed = FALSE;
            mHost->mSessionsChanged = TRUE;
            mHost->mBackgroundHandler->RemoveMessages(BackgroundHandler::MSG_REBUILD_LIST, (IObject*)this);
            mHost->DoPauseIfNeededLocked();
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "...pause releasing lock");
    }
    return NOERROR;
}

ECode ApplicationsState::Session::ReleaseItem()
{
    Pause();
    {
        AutoLock syncLock(mHost->mEntriesMap);
        mHost->mSessions->Remove((IObject*)this);
    }
    return NOERROR;
}

AutoPtr<IArrayList> ApplicationsState::Session::Rebuild(
    /* [in] */ IApplicationsStateAppFilter* filter,
    /* [in] */ IComparator* comparator)
{
    {
        AutoLock syncLock(mRebuildSync);
        {
            AutoLock syncLock(mHost->mEntriesMap);
            mHost->mRebuildingSessions->Add((IObject*)this);
            mRebuildRequested = TRUE;
            mRebuildAsync = FALSE;
            mRebuildFilter = filter;
            mRebuildComparator = comparator;
            mRebuildResult = NULL;
            Boolean res;
            mHost->mBackgroundHandler->HasMessages(BackgroundHandler::MSG_REBUILD_LIST, &res);
            if (!res) {
                AutoPtr<IMessage> msg;
                mHost->mBackgroundHandler->ObtainMessage(
                        BackgroundHandler::MSG_REBUILD_LIST, (IMessage**)&msg);
                mHost->mBackgroundHandler->SendMessage(msg, &res);
            }
        }

        // We will wait for .25s for the list to be built.
        Int64 waitend = SystemClock::GetUptimeMillis() + 250;

        while (mRebuildResult == NULL) {
            Int64 now = SystemClock::GetUptimeMillis();
            if (now >= waitend) {
                break;
            }
            // try {
                mRebuildSync->Wait(waitend - now);
            // } catch (InterruptedException e) {
            // }
        }

        mRebuildAsync = TRUE;

        return mRebuildResult;
    }
}

void ApplicationsState::Session::HandleRebuildList()
{
    AutoPtr<IApplicationsStateAppFilter> filter;
    AutoPtr<IComparator> comparator;
    {
        AutoLock syncLock(mRebuildSync);
        if (!mRebuildRequested) {
            return;
        }

        filter = mRebuildFilter;
        comparator = mRebuildComparator;
        mRebuildRequested = FALSE;
        mRebuildFilter = NULL;
        mRebuildComparator = NULL;
    }

    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_FOREGROUND);

    if (filter != NULL) {
        filter->Init();
    }

    AutoPtr<IList> apps;
    {
        AutoLock syncLock(mHost->mEntriesMap);
        CArrayList::New(ICollection::Probe(mHost->mApplications), (IList**)&apps);
    }

    AutoPtr<IArrayList> filteredApps;
    CArrayList::New((IArrayList**)&filteredApps);
    if (DEBUG) Logger::I(TAG, "Rebuilding...");
    Int32 size;
    apps->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        apps->Get(i, (IInterface**)&obj);
        IApplicationInfo* info = IApplicationInfo::Probe(obj);
        Boolean res;
        if (filter == NULL || (filter->FilterApp(info, &res), res)) {
            {
                AutoLock syncLock(mHost->mEntriesMap);
                if (DEBUG_LOCKING) Logger::V(TAG, "rebuild acquired lock");
                AutoPtr<AppEntry> entry = mHost->GetEntryLocked(info);
                entry->EnsureLabel(mHost->mContext);
                String packageName;
                IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
                if (DEBUG) Logger::I(TAG, "Using %s: %s", packageName.string(), TO_CSTR(entry));
                filteredApps->Add((IApplicationsStateAppEntry*)entry);
                if (DEBUG_LOCKING) Logger::V(TAG, "rebuild releasing lock");
            }
        }
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(IList::Probe(filteredApps), comparator);

    {
        AutoLock syncLock(mRebuildSync);
        if (!mRebuildRequested) {
            mLastAppList = filteredApps;
            if (!mRebuildAsync) {
                mRebuildResult = filteredApps;
                mRebuildSync->NotifyAll();
            }
            else {
                Boolean res;
                mHost->mMainHandler->HasMessages(MainHandler::MSG_REBUILD_COMPLETE, (IObject*)this, &res);
                if (!res) {
                    AutoPtr<IMessage> msg;
                    mHost->mMainHandler->ObtainMessage(
                            MainHandler::MSG_REBUILD_COMPLETE, (IObject*)this, (IMessage**)&msg);
                    mHost->mMainHandler->SendMessage(msg, &res);
                }
            }
        }
    }

    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
}

//===============================================================================
//                  ApplicationsState::BackgroundHandler
//===============================================================================

ApplicationsState::BackgroundHandler::BackgroundHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ ApplicationsState* host)
    : mRunning(FALSE)
    , mHost(host)
{
    Handler::constructor(looper);
    CApplicationsStatePackageStatsObserver::New(mHost, this, (IIPackageStatsObserver**)&mStatsObserver);
}

ECode ApplicationsState::BackgroundHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // Always try rebuilding list first thing, if needed.
    AutoPtr<IArrayList> rebuildingSessions; // ArrayList<Session>
    Int32 size;
    {
        AutoLock syncLock(mHost->mEntriesMap);
        if ((mHost->mRebuildingSessions->GetSize(&size), size) > 0) {
            CArrayList::New(ICollection::Probe(mHost->mRebuildingSessions), (IArrayList**)&rebuildingSessions);
            mHost->mRebuildingSessions->Clear();
        }
    }
    if (rebuildingSessions != NULL) {
        rebuildingSessions->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            rebuildingSessions->Get(i, (IInterface**)&obj);
            ((Session*)IObject::Probe(obj))->HandleRebuildList();
        }
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_REBUILD_LIST: {
        } break;

        case MSG_LOAD_ENTRIES: {
            Int32 numDone = 0;
            Boolean res;
            {
                AutoLock syncLock(mHost->mEntriesMap);
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ENTRIES acquired lock");
                mHost->mApplications->GetSize(&size);
                for (Int32 i = 0; i < size && numDone < 6; i++) {
                    if (!mRunning) {
                        mRunning = TRUE;
                        AutoPtr<IMessage> m;
                        mHost->mMainHandler->ObtainMessage(
                                MainHandler::MSG_RUNNING_STATE_CHANGED,
                                CoreUtils::Convert(1), (IMessage**)&m);
                        mHost->mMainHandler->SendMessage(m, &res);
                    }
                    AutoPtr<IInterface> obj;
                    mHost->mApplications->Get(i, (IInterface**)&obj);
                    IApplicationInfo* info = IApplicationInfo::Probe(obj);
                    String packageName;
                    IPackageItemInfo::Probe(info)->GetPackageName(&packageName);

                    obj = NULL;
                    mHost->mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
                    if (obj == NULL) {
                        numDone++;
                        mHost->GetEntryLocked(info);
                    }
                }
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ENTRIES releasing lock");
            }

            if (numDone >= 6) {
                SendEmptyMessage(MSG_LOAD_ENTRIES, &res);
            }
            else {
                SendEmptyMessage(MSG_LOAD_ICONS, &res);
            }
        } break;

        case MSG_LOAD_ICONS: {
            Int32 numDone = 0;
            Boolean res;
            {
                AutoLock syncLock(mHost->mEntriesMap);
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ICONS acquired lock");
                mHost->mAppEntries->GetSize(&size);
                for (Int32 i = 0; i < size && numDone < 2; i++) {
                    AutoPtr<IInterface> obj;
                    mHost->mAppEntries->Get(i, (IInterface**)&obj);
                    AppEntry* entry = (AppEntry*) IApplicationsStateAppEntry::Probe(obj);
                    if (entry->mIcon == NULL || !entry->mMounted) {
                        {
                            AutoLock syncLock(entry);
                            if (entry->EnsureIconLocked(mHost->mContext, mHost->mPm)) {
                                if (!mRunning) {
                                    mRunning = TRUE;
                                    AutoPtr<IMessage> m;
                                    mHost->mMainHandler->ObtainMessage(
                                            MainHandler::MSG_RUNNING_STATE_CHANGED,
                                            CoreUtils::Convert(1), (IMessage**)&m);
                                    mHost->mMainHandler->SendMessage(m, &res);
                                }
                                numDone++;
                            }
                        }
                    }
                }
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ICONS releasing lock");
            }
            if (numDone > 0) {
                mHost->mMainHandler->HasMessages(MainHandler::MSG_PACKAGE_ICON_CHANGED, &res);
                if (!res) {
                    mHost->mMainHandler->SendEmptyMessage(MainHandler::MSG_PACKAGE_ICON_CHANGED, &res);
                }
            }
            if (numDone >= 2) {
                SendEmptyMessage(MSG_LOAD_ICONS, &res);
            }
            else {
                SendEmptyMessage(MSG_LOAD_SIZES, &res);
            }
        } break;

        case MSG_LOAD_SIZES: {
            {
                AutoLock syncLock(mHost->mEntriesMap);
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES acquired lock");
                if (!mHost->mCurComputingSizePkg.IsNull()) {
                    if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing: currently computing");
                    return NOERROR;
                }

                Int64 now = SystemClock::GetUptimeMillis();
                Boolean res;
                mHost->mAppEntries->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    mHost->mAppEntries->Get(i, (IInterface**)&obj);
                    AppEntry* entry = (AppEntry*) IApplicationsStateAppEntry::Probe(obj);
                    if (entry->mSize == SIZE_UNKNOWN || entry->mSizeStale) {
                        if (entry->mSizeLoadStart == 0 ||
                                (entry->mSizeLoadStart < (now-20*1000))) {
                            if (!mRunning) {
                                mRunning = TRUE;
                                AutoPtr<IMessage> m;
                                mHost->mMainHandler->ObtainMessage(
                                        MainHandler::MSG_RUNNING_STATE_CHANGED,
                                        CoreUtils::Convert(1), (IMessage**)&m);
                                mHost->mMainHandler->SendMessage(m, &res);
                            }
                            entry->mSizeLoadStart = now;
                            IPackageItemInfo::Probe(entry->mInfo)->GetPackageName(&mHost->mCurComputingSizePkg);
                            mHost->mPm->GetPackageSizeInfo(mHost->mCurComputingSizePkg, mStatsObserver);
                        }
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing: now computing");
                        return NOERROR;
                    }
                }
                mHost->mMainHandler->HasMessages(MainHandler::MSG_ALL_SIZES_COMPUTED, &res);
                if (!res) {
                    mHost->mMainHandler->SendEmptyMessage(MainHandler::MSG_ALL_SIZES_COMPUTED, &res);
                    mRunning = FALSE;
                    AutoPtr<IMessage> m;
                    mHost->mMainHandler->ObtainMessage(
                            MainHandler::MSG_RUNNING_STATE_CHANGED,
                            CoreUtils::Convert(0), (IMessage**)&m);
                    mHost->mMainHandler->SendMessage(m, &res);
                }
                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing lock");
            }
        } break;
    }
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::ApplicationsStatePackageStatsObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(ApplicationsState::ApplicationsStatePackageStatsObserver, Object, IIPackageStatsObserver, IBinder)

ApplicationsState::ApplicationsStatePackageStatsObserver::ApplicationsStatePackageStatsObserver()
{}

ECode ApplicationsState::ApplicationsStatePackageStatsObserver::constructor(
    /* [in] */ IApplicationsState* host,
    /* [in] */ IHandler* owner)
{
    mHost = (ApplicationsState*)host;
    mOwner = (BackgroundHandler*)owner;
    return NOERROR;
}

ApplicationsState::ApplicationsStatePackageStatsObserver::~ApplicationsStatePackageStatsObserver()
{}

ECode ApplicationsState::ApplicationsStatePackageStatsObserver::OnGetStatsCompleted(
    /* [in] */ IPackageStats* stats,
    /* [in] */ Boolean succeeded)
{
    Boolean sizeChanged = FALSE;
    {
        AutoLock syncLock(mHost->mEntriesMap);
        if (DEBUG_LOCKING) Logger::V(TAG, "onGetStatsCompleted acquired lock");
        String packageName;
        stats->GetPackageName(&packageName);
        AutoPtr<IInterface> obj;
        mHost->mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AppEntry* entry = (AppEntry*)IApplicationsStateAppEntry::Probe(obj);
        if (entry != NULL) {
            {
                AutoLock syncLock(entry);
                entry->mSizeStale = FALSE;
                entry->mSizeLoadStart = 0;
                Int64 statsExternalCodeSize, statsExternalObbSize;
                stats->GetExternalCodeSize(&statsExternalCodeSize);
                stats->GetExternalObbSize(&statsExternalObbSize);
                Int64 externalCodeSize = statsExternalCodeSize
                        + statsExternalObbSize;
                Int64 statsExternalDataSize, statsExternalMediaSize;
                stats->GetExternalDataSize(&statsExternalDataSize);
                stats->GetExternalMediaSize(&statsExternalMediaSize);
                Int64 externalDataSize = statsExternalDataSize
                        + statsExternalMediaSize;
                Int64 newSize = externalCodeSize + externalDataSize
                        + mHost->GetTotalInternalSize(stats);
                Int64 cacheSize, codeSize, dataSize, externalCacheSize;
                if (entry->mSize != newSize ||
                        entry->mCacheSize != (stats->GetCacheSize(&cacheSize), cacheSize) ||
                        entry->mCodeSize != (stats->GetCodeSize(&codeSize), codeSize) ||
                        entry->mDataSize != (stats->GetDataSize(&dataSize), dataSize) ||
                        entry->mExternalCodeSize != externalCodeSize ||
                        entry->mExternalDataSize != externalDataSize ||
                        entry->mExternalCacheSize != (stats->GetExternalCacheSize(&externalCacheSize), externalCacheSize)) {
                    entry->mSize = newSize;
                    entry->mCacheSize = (stats->GetCacheSize(&cacheSize), cacheSize);
                    entry->mCodeSize = (stats->GetCodeSize(&codeSize), codeSize);
                    entry->mDataSize = (stats->GetDataSize(&dataSize), dataSize);
                    entry->mExternalCodeSize = externalCodeSize;
                    entry->mExternalDataSize = externalDataSize;
                    entry->mExternalCacheSize = (stats->GetExternalCacheSize(&externalCacheSize), externalCacheSize);
                    entry->mSizeStr = mHost->GetSizeStr(entry->mSize);
                    entry->mInternalSize = mHost->GetTotalInternalSize(stats);
                    entry->mInternalSizeStr = mHost->GetSizeStr(entry->mInternalSize);
                    entry->mExternalSize = mHost->GetTotalExternalSize(stats);
                    entry->mExternalSizeStr = mHost->GetSizeStr(entry->mExternalSize);
                    if (DEBUG) Logger::I(TAG, "Set size of %s %s: %s",
                            entry->mLabel.string(), TO_CSTR(entry), entry->mSizeStr.string());
                    sizeChanged = TRUE;
                }
            }
            if (sizeChanged) {
                AutoPtr<IMessage> msg;
                mHost->mMainHandler->ObtainMessage(
                        MainHandler::MSG_PACKAGE_SIZE_CHANGED, CoreUtils::Convert(packageName), (IMessage**)&msg);
                Boolean res;
                mHost->mMainHandler->SendMessage(msg, &res);
            }
        }

        if (mHost->mCurComputingSizePkg.IsNull()
                || mHost->mCurComputingSizePkg.Equals(packageName)) {
            mHost->mCurComputingSizePkg = String(NULL);
            Boolean res;
            mOwner->SendEmptyMessage(BackgroundHandler::MSG_LOAD_SIZES, &res);
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "onGetStatsCompleted releasing lock");
    }
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::MainHandler
//===============================================================================

ApplicationsState::MainHandler::MainHandler(
    /* [in] */ ApplicationsState* host)
    : mHost(host)
{}

ApplicationsState::MainHandler::~MainHandler()
{}

ECode ApplicationsState::MainHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    mHost->RebuildActiveSessions();
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_REBUILD_COMPLETE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Session* s = (Session*)IObject::Probe(obj);
            Boolean res;
            if (mHost->mActiveSessions->Contains(IObject::Probe(obj), &res), res) {
                s->mCallbacks->OnRebuildComplete(s->mLastAppList);
            }
        } break;

        case MSG_PACKAGE_LIST_CHANGED: {
            Int32 size;
            mHost->mActiveSessions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mActiveSessions->Get(i, (IInterface**)&obj);
                ((Session*)IObject::Probe(obj))->mCallbacks->OnPackageListChanged();
            }
        } break;

        case MSG_PACKAGE_ICON_CHANGED: {
            Int32 size;
            mHost->mActiveSessions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mActiveSessions->Get(i, (IInterface**)&obj);
                ((Session*)IObject::Probe(obj))->mCallbacks->OnPackageIconChanged();
            }
        } break;

        case MSG_PACKAGE_SIZE_CHANGED: {
            Int32 size;
            mHost->mActiveSessions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mActiveSessions->Get(i, (IInterface**)&obj);

                AutoPtr<IInterface> msgObj;
                msg->GetObj((IInterface**)&msgObj);

                ((Session*)IObject::Probe(obj))->mCallbacks->OnPackageSizeChanged(
                        Object::ToString(msgObj));
            }
        } break;

        case MSG_ALL_SIZES_COMPUTED: {
            Int32 size;
            mHost->mActiveSessions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mActiveSessions->Get(i, (IInterface**)&obj);
                ((Session*)IObject::Probe(obj))->mCallbacks->OnAllSizesComputed();
            }
        } break;

        case MSG_RUNNING_STATE_CHANGED: {
            Int32 size;
            mHost->mActiveSessions->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mActiveSessions->Get(i, (IInterface**)&obj);
                Int32 arg1;
                msg->GetArg1(&arg1);
                ((Session*)IObject::Probe(obj))->mCallbacks->OnRunningStateChanged(arg1 != 0);
            }
        } break;
    }
    return NOERROR;
}

//===============================================================================
//                  ApplicationsState::PackageIntentReceiver
//===============================================================================

ApplicationsState::PackageIntentReceiver::PackageIntentReceiver(
    /* [in] */ ApplicationsState* host)
    : mHost(host)
{}

ApplicationsState::PackageIntentReceiver::~PackageIntentReceiver()
{}

ECode ApplicationsState::PackageIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String actionStr;
    intent->GetAction(&actionStr);
    if (IIntent::ACTION_PACKAGE_ADDED.Equals(actionStr)) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String pkgName;
        data->GetEncodedSchemeSpecificPart(&pkgName);
        mHost->AddPackage(pkgName);
    }
    else if (IIntent::ACTION_PACKAGE_REMOVED.Equals(actionStr)) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String pkgName;
        data->GetEncodedSchemeSpecificPart(&pkgName);
        mHost->RemovePackage(pkgName);
    }
    else if (IIntent::ACTION_PACKAGE_CHANGED.Equals(actionStr)) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        String pkgName;
        data->GetEncodedSchemeSpecificPart(&pkgName);
        mHost->InvalidatePackage(pkgName);
    }
    else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(actionStr) ||
            IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(actionStr)) {
        // When applications become available or unavailable (perhaps because
        // the SD card was inserted or ejected) we need to refresh the
        // AppInfo with new mLabel, icon and size information as appropriate
        // given the newfound (un)availability of the application.
        // A simple way to do that is to treat the refresh as a package
        // removal followed by a package addition.
        AutoPtr< ArrayOf<String> > pkgList;
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
        if (pkgList == NULL || pkgList->GetLength() == 0) {
            // Ignore
            return NOERROR;
        }
        Boolean avail = IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(actionStr);
        if (avail) {
            for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                String pkgName = (*pkgList)[i];
                mHost->InvalidatePackage(pkgName);
            }
        }
    }
    return NOERROR;
}

void ApplicationsState::PackageIntentReceiver::RegisterReceiver()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_PACKAGE_ADDED, (IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddDataScheme(String("package"));
    AutoPtr<IIntent> stickIntent;
    mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&stickIntent);
    // Register for events related to sdcard installation.
    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New((IIntentFilter**)&sdFilter);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    stickIntent = NULL;
    mHost->mContext->RegisterReceiver(this, sdFilter, (IIntent**)&stickIntent);
}

void ApplicationsState::PackageIntentReceiver::UnregisterReceiver()
{
    mHost->mContext->UnregisterReceiver(this);
}

//===============================================================================
//                  ApplicationsState
//===============================================================================

CAR_INTERFACE_IMPL(ApplicationsState, Object, IApplicationsState);

ApplicationsState::ApplicationsState(
    /* [in] */ IApplication* app)
    : mRetrieveFlags(0)
    , mResumed(FALSE)
    , mHaveDisabledApps(FALSE)
    , mCurId(1)
    , mSessionsChanged(FALSE)
{
    CArrayList::New((IArrayList**)&mSessions);
    CArrayList::New((IArrayList**)&mRebuildingSessions);
    mInterestingConfigChanges = new InterestingConfigChanges();
    CHashMap::New((IHashMap**)&mEntriesMap);
    CArrayList::New((IArrayList**)&mAppEntries);
    CArrayList::New((IList**)&mApplications);

    CArrayList::New((IArrayList**)&mActiveSessions);

    mMainHandler = new MainHandler(this);
    mMainHandler->constructor();

    mContext = IContext::Probe(app);
    mPm = NULL;
    mContext->GetPackageManager((IPackageManager**)&mPm);
    CHandlerThread::New(String("ApplicationsState.Loader"),
            IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&mThread);
    IThread::Probe(mThread)->Start();
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    mBackgroundHandler = new BackgroundHandler(looper, this);

    // Only the owner can see all apps.
    if (UserHandle::GetMyUserId() == 0) {
        mRetrieveFlags = IPackageManager::GET_UNINSTALLED_PACKAGES |
                IPackageManager::GET_DISABLED_COMPONENTS |
                IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS;
    }
    else {
        mRetrieveFlags = IPackageManager::GET_DISABLED_COMPONENTS |
                IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS;
    }

    /**
     * This is a trick to prevent the foreground thread from being delayed.
     * The problem is that Dalvik monitors are initially spin locks, to keep
     * them lightweight.  This leads to unfair contention -- Even though the
     * background thread only holds the lock for a short amount of time, if
     * it keeps running and locking again it can prevent the main thread from
     * acquiring its lock for a Int64 time...  sometimes even > 5 seconds
     * (leading to an ANR).
     *
     * Dalvik will promote a monitor to a "real" lock if it detects enough
     * contention on it.  It doesn't figure this out fast enough for us
     * here, though, so this little trick will force it to turn into a real
     * lock immediately.
     */
    {
        AutoLock syncLock(mEntriesMap);
        // try {
        ISynchronize::Probe(mEntriesMap)->Wait(1);
        // } catch (InterruptedException e) {
        // }
    }
}

AutoPtr<ApplicationsState> ApplicationsState::GetInstance(
    /* [in] */ IApplication* app)
{
    {
        AutoLock syncLock(sLock);
        if (sInstance == NULL) {
            sInstance = new ApplicationsState(app);
        }
        return sInstance;
    }
}

String ApplicationsState::Normalize(
    /* [in] */ const String& str)
{
    AutoPtr<INormalizer> helper;
    CNormalizerHelper::AcquireSingleton((INormalizer**)&helper);
    String tmp;
    helper->Normalize(CoreUtils::Convert(str), NormalizerForm_NFD, &tmp);
    AutoPtr<IMatcher> matcher;
    REMOVE_DIACRITICALS_PATTERN->Matcher(tmp, (IMatcher**)&matcher);
    String result;
    matcher->ReplaceAll(String(""), &result);
    return result.ToLowerCase();
}

void ApplicationsState::RebuildActiveSessions()
{
    {
        AutoLock syncLock(mEntriesMap);
        if (!mSessionsChanged) {
            return;
        }
        mActiveSessions->Clear();
        Int32 size;
        mSessions->GetSize(&size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> obj;
            mSessions->Get(i, (IInterface**)&obj);
            Session* s = (Session*)IObject::Probe(obj);
            if (s->mResumed) {
                mActiveSessions->Add((IObject*)s);
            }
        }
    }
}

AutoPtr<ApplicationsState::Session> ApplicationsState::NewSession(
    /* [in] */ IApplicationsStateCallbacks* callbacks)
{
    AutoPtr<Session> s = new Session(callbacks, this);
    {
        AutoLock syncLock(mEntriesMap);
        mSessions->Add((IObject*)s);
    }
    return s;
}

void ApplicationsState::DoResumeIfNeededLocked()
{
    if (mResumed) {
        return;
    }
    mResumed = TRUE;
    if (mPackageIntentReceiver == NULL) {
        mPackageIntentReceiver = new PackageIntentReceiver(this);
        mPackageIntentReceiver->RegisterReceiver();
    }
    mApplications = NULL;
    mPm->GetInstalledApplications(mRetrieveFlags, (IList**)&mApplications);
    if (mApplications == NULL) {
        CArrayList::New((IList**)&mApplications);
    }

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 size;
    if (mInterestingConfigChanges->ApplyNewConfig(resources)) {
        // If an interesting part of the configuration has changed, we
        // should completely reload the app entries.
        mEntriesMap->Clear();
        mAppEntries->Clear();
    }
    else {
        mAppEntries->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mAppEntries->Get(i, (IInterface**)&obj);
            ((AppEntry*)IApplicationsStateAppEntry::Probe(obj))->mSizeStale = TRUE;
        }
    }

    mHaveDisabledApps = FALSE;
    mApplications->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mApplications->Get(i, (IInterface**)&obj);
        IApplicationInfo* info = IApplicationInfo::Probe(obj);
        // Need to trim out any applications that are disabled by
        // something different than the user.
        Boolean enabled;
        info->GetEnabled(&enabled);
        if (!enabled) {
            Int32 enabledSetting;
            info->GetEnabledSetting(&enabledSetting);
            if (enabledSetting != IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER) {
                mApplications->Remove(i);
                i--;
                continue;
            }
            mHaveDisabledApps = TRUE;
        }
        String packageName;
        IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
        AutoPtr<IInterface> entryObj;
        mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&entryObj);
        AppEntry* entry = (AppEntry*)IApplicationsStateAppEntry::Probe(entryObj);
        if (entry != NULL) {
            entry->mInfo = info;
        }
    }
    mCurComputingSizePkg = String(NULL);
    Boolean res;
    mBackgroundHandler->HasMessages(BackgroundHandler::MSG_LOAD_ENTRIES, &res);
    if (!res) {
        mBackgroundHandler->SendEmptyMessage(BackgroundHandler::MSG_LOAD_ENTRIES, &res);
    }
}

Boolean ApplicationsState::HaveDisabledApps()
{
    return mHaveDisabledApps;
}

void ApplicationsState::DoPauseIfNeededLocked()
{
    if (!mResumed) {
        return;
    }
    Int32 size;
    mSessions->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mSessions->Get(i, (IInterface**)&obj);
        if (((Session*)IObject::Probe(obj))->mResumed) {
            return;
        }
    }
    mResumed = FALSE;
    if (mPackageIntentReceiver != NULL) {
        mPackageIntentReceiver->UnregisterReceiver();
        mPackageIntentReceiver = NULL;
    }
}

AutoPtr<ApplicationsState::AppEntry> ApplicationsState::GetEntry(
    /* [in] */ const String& packageName)
{
    if (DEBUG_LOCKING) Logger::V(TAG, "getEntry about to acquire lock...");
    {
        AutoLock syncLock(mEntriesMap);
        AutoPtr<IInterface> obj;
        mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<AppEntry> entry = (AppEntry*)IApplicationsStateAppEntry::Probe(obj);
        if (entry == NULL) {
            Int32 size;
            mApplications->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mApplications->Get(i, (IInterface**)&obj);
                IApplicationInfo* info = IApplicationInfo::Probe(obj);
                String infoPackageName;
                IPackageItemInfo::Probe(info)->GetPackageName(&infoPackageName);
                if (packageName.Equals(infoPackageName)) {
                    entry = GetEntryLocked(info);
                    break;
                }
            }
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "...getEntry releasing lock");
        return entry;
    }
}

void ApplicationsState::EnsureIcon(
    /* [in] */ AppEntry* entry)
{
    if (entry->mIcon != NULL) {
        return;
    }
    {
        AutoLock syncLock(entry);
        entry->EnsureIconLocked(mContext, mPm);
    }
}

void ApplicationsState::RequestSize(
    /* [in] */ const String& packageName)
{
    if (DEBUG_LOCKING) Logger::V(TAG, "requestSize about to acquire lock...");
    {
        AutoLock syncLock(mEntriesMap);
        AutoPtr<IInterface> obj;
        mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        IApplicationsStateAppEntry* entry = IApplicationsStateAppEntry::Probe(obj);
        if (entry != NULL) {
            mPm->GetPackageSizeInfo(packageName, mBackgroundHandler->mStatsObserver);
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "...requestSize releasing lock");
    }
}

Int64 ApplicationsState::SumCacheSizes()
{
    Int64 sum = 0;
    if (DEBUG_LOCKING) Logger::V(TAG, "sumCacheSizes about to acquire lock...");
    {
        AutoLock syncLock(mEntriesMap);
        if (DEBUG_LOCKING) Logger::V(TAG, "-> sumCacheSizes now has lock");
        Int32 size;
        mAppEntries->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mAppEntries->Get(i, (IInterface**)&obj);
            sum += ((AppEntry*)IApplicationsStateAppEntry::Probe(obj))->mCacheSize;
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "...sumCacheSizes releasing lock");
    }
    return sum;
}

Int32 ApplicationsState::IndexOfApplicationInfoLocked(
    /* [in] */ const String& pkgName)
{
    Int32 size;
    mApplications->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mApplications->Get(i, (IInterface**)&obj);
        IApplicationInfo* info = IApplicationInfo::Probe(obj);
        String packageName;
        IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
        if (packageName.Equals(pkgName)) {
            return i;
        }
    }
    return -1;
}

void ApplicationsState::AddPackage(
    /* [in] */ const String& pkgName)
{
    // try {
    {
        AutoLock syncLock(mEntriesMap);
        if (DEBUG_LOCKING) Logger::V(TAG, "addPackage acquired lock");
        if (DEBUG) Logger::I(TAG, "Adding package %s", pkgName.string());
        if (!mResumed) {
            // If we are not resumed, we will do a full query the
            // next time we resume, so there is no reason to do work
            // here.
            if (DEBUG_LOCKING) Logger::V(TAG, "addPackage release lock: not resumed");
            return;
        }
        if (IndexOfApplicationInfoLocked(pkgName) >= 0) {
            if (DEBUG) Logger::I(TAG, "Package already exists!");
            if (DEBUG_LOCKING) Logger::V(TAG, "addPackage release lock: already exists");
            return;
        }
        AutoPtr<IApplicationInfo> info;
        mPm->GetApplicationInfo(pkgName, mRetrieveFlags, (IApplicationInfo**)&info);
        Boolean enabled;
        info->GetEnabled(&enabled);
        if (!enabled) {
            Int32 enabledSetting;
            info->GetEnabledSetting(&enabledSetting);
            if (enabledSetting
                    != IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER) {
                return;
            }
            mHaveDisabledApps = TRUE;
        }
        mApplications->Add(info);
        Boolean res;
        mBackgroundHandler->HasMessages(BackgroundHandler::MSG_LOAD_ENTRIES, &res);
        if (!res) {
            mBackgroundHandler->SendEmptyMessage(BackgroundHandler::MSG_LOAD_ENTRIES, &res);
        }
        mMainHandler->HasMessages(MainHandler::MSG_PACKAGE_LIST_CHANGED, &res);
        if (!res) {
            mMainHandler->SendEmptyMessage(MainHandler::MSG_PACKAGE_LIST_CHANGED, &res);
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "addPackage releasing lock");
    }
    // } catch (NameNotFoundException e) {
    // }
}

void ApplicationsState::RemovePackage(
    /* [in] */ const String& pkgName)
{
    {
        AutoLock syncLock(mEntriesMap);
        if (DEBUG_LOCKING) Logger::V(TAG, "removePackage acquired lock");
        Int32 idx = IndexOfApplicationInfoLocked(pkgName);
        if (DEBUG) Logger::I(TAG, "removePackage: %s @ %d", pkgName.string(), idx);
        if (idx >= 0) {
            AutoPtr<IInterface> obj;
            mEntriesMap->Get(CoreUtils::Convert(pkgName), (IInterface**)&obj);
            AutoPtr<IApplicationsStateAppEntry> entry = IApplicationsStateAppEntry::Probe(obj);
            if (DEBUG) Logger::I(TAG, "removePackage: %s", TO_CSTR(entry));
            if (entry != NULL) {
                mEntriesMap->Remove(CoreUtils::Convert(pkgName));
                mAppEntries->Remove(entry);
            }
            obj = NULL;
            mApplications->Get(idx, (IInterface**)&obj);
            AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
            mApplications->Remove(idx);
            Boolean enabled;
            info->GetEnabled(&enabled);
            if (!enabled) {
                mHaveDisabledApps = FALSE;
                Int32 size;
                mApplications->GetSize(&size);
                for (Int32 i = 0; i < size; i++) {
                    obj = NULL;
                    mApplications->Get(i, (IInterface**)&obj);
                    AutoPtr<IApplicationInfo> applicationInfo = IApplicationInfo::Probe(obj);
                    Boolean infoEnabled;
                    applicationInfo->GetEnabled(&infoEnabled);
                    if (!infoEnabled) {
                        mHaveDisabledApps = TRUE;
                        break;
                    }
                }
            }
            Boolean res;
            mMainHandler->HasMessages(MainHandler::MSG_PACKAGE_LIST_CHANGED, &res);
            if (!res) {
                mMainHandler->SendEmptyMessage(MainHandler::MSG_PACKAGE_LIST_CHANGED, &res);
            }
        }
        if (DEBUG_LOCKING) Logger::V(TAG, "removePackage releasing lock");
    }
}

void ApplicationsState::InvalidatePackage(
    /* [in] */ const String& pkgName)
{
    RemovePackage(pkgName);
    AddPackage(pkgName);
}

AutoPtr<ApplicationsState::AppEntry> ApplicationsState::GetEntryLocked(
    /* [in] */ IApplicationInfo* info)
{
    String packageName;
    IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
    AutoPtr<IInterface> obj;
    mEntriesMap->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
    AutoPtr<AppEntry> entry = (AppEntry*) IApplicationsStateAppEntry::Probe(obj);

    if (DEBUG) Logger::I(TAG, "Looking up entry of pkg %s: %s", packageName.string(), TO_CSTR(entry));
    if (entry == NULL) {
        if (DEBUG) Logger::I(TAG, "Creating AppEntry for %s", packageName.string());
        entry = new AppEntry(mContext, info, mCurId++);
        mEntriesMap->Put(CoreUtils::Convert(packageName), (IApplicationsStateAppEntry*)entry);
        mAppEntries->Add((IApplicationsStateAppEntry*)entry);
    }
    else if (entry->mInfo.Get() != info) {
        entry->mInfo = info;
    }
    return entry;
}

Int64 ApplicationsState::GetTotalInternalSize(
    /* [in] */ IPackageStats* ps)
{
    if (ps != NULL) {
        Int64 codeSize, dataSize;
        ps->GetCodeSize(&codeSize);
        ps->GetDataSize(&dataSize);
        return codeSize + dataSize;
    }
    return SIZE_INVALID;
}

Int64 ApplicationsState::GetTotalExternalSize(
    /* [in] */ IPackageStats* ps)
{
    if (ps != NULL) {
        // We also include the cache size here because for non-emulated
        // we don't automtically clean cache files.
        Int64 externalCodeSize, externalDataSize, externalCacheSize, externalMediaSize, externalObbSize;
        ps->GetExternalCodeSize(&externalCodeSize);
        ps->GetExternalDataSize(&externalDataSize);
        ps->GetExternalCacheSize(&externalCacheSize);
        ps->GetExternalMediaSize(&externalMediaSize);
        ps->GetExternalObbSize(&externalObbSize);
        return externalCodeSize + externalDataSize
                + externalCacheSize
                + externalMediaSize + externalObbSize;
    }
    return SIZE_INVALID;
}

String ApplicationsState::GetSizeStr(
    /* [in] */ Int64 size)
{
    if (size >= 0) {
        return Formatter::FormatFileSize(mContext, size);
    }
    return String(NULL);
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos