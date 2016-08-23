
#include "elastos/droid/settings/applications/RunningState.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/Formatter.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::IInteger32;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

const String RunningState::TAG("RunningState");
const Boolean RunningState::DEBUG_COMPARE = FALSE;

AutoPtr<Object> RunningState::sGlobalLock = new Object();
AutoPtr<RunningState> RunningState::sInstance;

const Int32 RunningState::MSG_RESET_CONTENTS;
const Int32 RunningState::MSG_UPDATE_CONTENTS;
const Int32 RunningState::MSG_REFRESH_UI;
const Int32 RunningState::MSG_UPDATE_TIME;

const Int64 RunningState::TIME_UPDATE_DELAY = 1000;
const Int64 RunningState::CONTENTS_UPDATE_DELAY = 2000;

const Int32 RunningState::MAX_SERVICES = 100;

//===============================================================================
//                  RunningState::AppProcessInfo
//===============================================================================

RunningState::AppProcessInfo::AppProcessInfo(
    /* [in] */ IActivityManagerRunningAppProcessInfo* _info)
    : mHasServices(FALSE)
    , mHasForegroundServices(FALSE)
{
    mInfo = _info;
}

RunningState::AppProcessInfo::~AppProcessInfo()
{}

//===============================================================================
//                  RunningState::BackgroundHandler
//===============================================================================

RunningState::BackgroundHandler::BackgroundHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ RunningState* host)
    : mHost(host)
{
    Handler::constructor(looper);
}

ECode RunningState::BackgroundHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_RESET_CONTENTS:
            mHost->Reset();
            break;
        case MSG_UPDATE_CONTENTS:
            {
                AutoLock syncLock(mHost->mLock);
                if (!mHost->mResumed) {
                    return NOERROR;
                }
            }
            AutoPtr<IMessage> cmd;
            mHost->mHandler->ObtainMessage(MSG_REFRESH_UI, (IMessage**)&cmd);
            cmd->SetArg1(mHost->Update(mHost->mApplicationContext, mHost->mAm) ? 1 : 0);
            Boolean res;
            mHost->mHandler->SendMessage(cmd, &res);
            RemoveMessages(MSG_UPDATE_CONTENTS);
            msg = NULL;
            ObtainMessage(MSG_UPDATE_CONTENTS, (IMessage**)&msg);
            SendMessageDelayed(msg, CONTENTS_UPDATE_DELAY, &res);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  RunningState::InitComparator
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::InitComparator, Object, IComparator)

RunningState::InitComparator::InitComparator(
    /* [in] */ RunningState* host)
    : mHost(host)
{}

RunningState::InitComparator::~InitComparator()
{}

ECode RunningState::InitComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<MergedItem> lhs = (MergedItem*)IRunningStateMergedItem::Probe(_lhs);
    AutoPtr<MergedItem> rhs = (MergedItem*)IRunningStateMergedItem::Probe(_rhs);

    if (DEBUG_COMPARE) {
        Logger::I(TAG, "Comparing %s with %s", TO_CSTR(lhs), TO_CSTR(rhs));
        Logger::I(TAG, "     Proc %p with %p", TO_CSTR(lhs->mProcess), TO_CSTR(rhs->mProcess));
        Logger::I(TAG, "   UserId %d with %d", lhs->mUserId, rhs->mUserId);
    }
    if (lhs->mUserId != rhs->mUserId) {
        if (lhs->mUserId == mHost->mMyUserId) {
            *result = -1;
            return NOERROR;
        }
        if (rhs->mUserId == mHost->mMyUserId) {
            *result = 1;
            return NOERROR;
        }
        *result = lhs->mUserId < rhs->mUserId ? -1 : 1;
        return NOERROR;
    }
    if (lhs->mProcess == rhs->mProcess) {
        if (lhs->mLabel == rhs->mLabel) {
            *result = 0;
            return NOERROR;
        }
        *result = lhs->mLabel != NULL ? lhs->mLabel.Compare(rhs->mLabel) : -1;
        return NOERROR;
    }
    if (lhs->mProcess == NULL) {
        *result = -1;
        return NOERROR;
    }
    if (rhs->mProcess == NULL) {
        *result = 1;
        return NOERROR;
    }
    if (DEBUG_COMPARE) Logger::I(TAG, "    Label %s  with %s",
            lhs->mProcess->mLabel.string(), rhs->mProcess->mLabel.string());
    AutoPtr<IActivityManagerRunningAppProcessInfo> lhsInfo
            = lhs->mProcess->mRunningProcessInfo;
    AutoPtr<IActivityManagerRunningAppProcessInfo> rhsInfo
            = rhs->mProcess->mRunningProcessInfo;
    Int32 importance;
    const Boolean lhsBg = (lhsInfo->GetImportance(&importance), importance)
            >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
    const Boolean rhsBg = (rhsInfo->GetImportance(&importance), importance)
            >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
    if (DEBUG_COMPARE) Logger::I(TAG, "       Bg %d with %d", lhsBg, rhsBg);
    if (lhsBg != rhsBg) {
        *result = lhsBg ? 1 : -1;
        return NOERROR;
    }
    Int32 flags;
    const Boolean lhsA = ((lhsInfo->GetFlags(&flags), flags)
            & IActivityManagerRunningAppProcessInfo::FLAG_HAS_ACTIVITIES) != 0;
    const Boolean rhsA = ((rhsInfo->GetFlags(&flags), flags)
            & IActivityManagerRunningAppProcessInfo::FLAG_HAS_ACTIVITIES) != 0;
    if (DEBUG_COMPARE) Logger::I(TAG, "      Act %d with %d", lhsA, rhsA);
    if (lhsA != rhsA) {
        *result = lhsA ? -1 : 1;
        return NOERROR;
    }
    Int32 lruL, lruR;
    lhsInfo->GetLru(&lruL);
    rhsInfo->GetLru(&lruR);
    if (DEBUG_COMPARE) Logger::I(TAG, "      Lru %d with %d", lruL, lruR);
    if (lruL != lruR) {
        *result = lruL < lruR ? -1 : 1;
        return NOERROR;
    }
    if (lhs->mProcess->mLabel == rhs->mProcess->mLabel) {
        *result = 0;
        return NOERROR;
    }
    if (lhs->mProcess->mLabel == NULL) {
        *result = 1;
        return NOERROR;
    }
    if (rhs->mProcess->mLabel == NULL) {
        *result = -1;
        return NOERROR;
    }
    *result = lhs->mProcess->mLabel.Compare(rhs->mProcess->mLabel);
    return NOERROR;
}

//===============================================================================
//                  RunningState::InitHandler
//===============================================================================

RunningState::InitHandler::InitHandler(
    /* [in] */ RunningState* host)
    : mNextUpdate(IRunningStateOnRefreshUiListener::REFRESH_TIME)
    , mHost(host)
{
    Handler::constructor();
}

RunningState::InitHandler::~InitHandler()
{}

ECode RunningState::InitHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_REFRESH_UI: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mNextUpdate = arg1 != 0
                    ? IRunningStateOnRefreshUiListener::REFRESH_STRUCTURE
                    : IRunningStateOnRefreshUiListener::REFRESH_DATA;
            break;
        }
        case MSG_UPDATE_TIME:
            {
                AutoLock syncLock(mHost->mLock);
                if (!mHost->mResumed) {
                    return NOERROR;
                }
            }
            RemoveMessages(MSG_UPDATE_TIME);
            AutoPtr<IMessage> m;
            ObtainMessage(MSG_UPDATE_TIME, (IMessage**)&m);
            Boolean res;
            SendMessageDelayed(m, TIME_UPDATE_DELAY, &res);

            if (mHost->mRefreshUiListener != NULL) {
                //Logger::I("foo", "Refresh UI: " + mNextUpdate
                //        + " @ " + SystemClock->UptimeMillis());
                mHost->mRefreshUiListener->OnRefreshUi(mNextUpdate);
                mNextUpdate = IRunningStateOnRefreshUiListener::REFRESH_TIME;
            }
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  RunningState::BaseItem
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::BaseItem, Object, IRunningStateBaseItem)

RunningState::BaseItem::BaseItem(
    /* [in] */ Boolean isProcess,
    /* [in] */ Int32 userId)
    : mIsProcess(FALSE)
    , mUserId(0)
    , mCurSeq(0)
    , mActiveSince(0)
    , mSize(0)
    , mNeedDivider(FALSE)
    , mBackground(FALSE)
{
    mIsProcess = isProcess;
    mUserId = userId;
}

RunningState::BaseItem::~BaseItem()
{}

ECode RunningState::BaseItem::LoadIcon(
    /* [in] */ IContext* context,
    /* [in] */ RunningState* state,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mPackageInfo != NULL) {
        return mPackageInfo->LoadIcon(state->mPm, result);
    }
    return NOERROR;
}

//===============================================================================
//                  RunningState::ServiceItem
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::ServiceItem, BaseItem, IRunningStateServiceItem)

RunningState::ServiceItem::ServiceItem(
    /* [in] */ Int32 userId)
    : BaseItem::BaseItem(FALSE, userId)
    , mShownAsStarted(FALSE)
{
}

//===============================================================================
//                  RunningState::ProcessItem
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::ProcessItem, BaseItem, IRunningStateProcessItem)

RunningState::ProcessItem::ProcessItem(
    /* [in] */ IContext* context,
    /* [in] */ Int32 uid,
    /* [in] */ const String& processName)
    : BaseItem::BaseItem(TRUE, UserHandle::GetUserId(uid))
    , mUid(0)
    , mPid(0)
    , mLastNumDependentProcesses(0)
    , mRunningSeq(0)
    , mInteresting(FALSE)
    , mIsSystem(FALSE)
    , mIsStarted(FALSE)
    , mActiveSince(0)
{
    CHashMap::New((IHashMap**)&mServices);
    CSparseArray::New((ISparseArray**)&mDependentProcesses);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(processName));
    resources->GetString(R::string::service_process_name, args, &mDescription);
    mUid = uid;
    mProcessName = processName;
}

void RunningState::ProcessItem::EnsureLabel(
    /* [in] */ IPackageManager* pm)
{
    if (!mLabel.IsNull()) {
        return;
    }

    // try {
    AutoPtr<IApplicationInfo> ai;
    pm->GetApplicationInfo(mProcessName,
            IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&ai);
    Int32 uid;
    ai->GetUid(&uid);
    if (uid == mUid) {
        mDisplayLabel = NULL;
        IPackageItemInfo::Probe(ai)->LoadLabel(pm, (ICharSequence**)&mDisplayLabel);
        mDisplayLabel->ToString(&mLabel);
        mPackageInfo = IPackageItemInfo::Probe(ai);
        return;
    }
    // } catch (PackageManager.NameNotFoundException e) {
    // }

    // If we couldn't get information about the overall
    // process, try to find something about the uid.
    AutoPtr< ArrayOf<String> > pkgs;
    pm->GetPackagesForUid(mUid, (ArrayOf<String>**)&pkgs);

    // If there is one package with this uid, that is what we want.
    if (pkgs->GetLength() == 1) {
        // try {
        AutoPtr<IApplicationInfo> ai;
        pm->GetApplicationInfo((*pkgs)[0],
                IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&ai);
        mDisplayLabel = NULL;
        IPackageItemInfo::Probe(ai)->LoadLabel(pm, (ICharSequence**)&mDisplayLabel);
        mDisplayLabel->ToString(&mLabel);
        mPackageInfo = IPackageItemInfo::Probe(ai);
        return;
        // } catch (PackageManager.NameNotFoundException e) {
        // }
    }

    // If there are multiple, see if one gives us the official name
    // for this uid.
    for (Int32 i = 0; i < pkgs->GetLength(); ++i) {
        String name = (*pkgs)[i];
        // try {
        AutoPtr<IPackageInfo> pi;
        pm->GetPackageInfo(name, 0, (IPackageInfo**)&pi);
        Int32 sharedUserLabel;
        pi->GetSharedUserLabel(&sharedUserLabel);
        if (sharedUserLabel != 0) {
            AutoPtr<IApplicationInfo> applicationInfo;
            pi->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            AutoPtr<ICharSequence> nm;
            pm->GetText(name, sharedUserLabel,
                    applicationInfo, (ICharSequence**)&nm);
            if (nm != NULL) {
                mDisplayLabel = nm;
                nm->ToString(&mLabel);
                mPackageInfo = IPackageItemInfo::Probe(applicationInfo);
                return;
            }
        }
        // } catch (PackageManager.NameNotFoundException e) {
        // }
    }

    // If still don't have anything to display, just use the
    // service info.
    Int32 size;
    mServices->GetSize(&size);
    if (size > 0) {
        AutoPtr<ICollection> values;
        mServices->GetValues((ICollection**)&values);
        AutoPtr<IIterator> iterator;
        values->GetIterator((IIterator**)&iterator);
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        IRunningStateServiceItem* si = IRunningStateServiceItem::Probe(obj);
        AutoPtr<IApplicationInfo> ai;
        IComponentInfo::Probe(((ServiceItem*)si)->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&ai);
        mPackageInfo = IPackageItemInfo::Probe(ai);
        mDisplayLabel = NULL;
        mPackageInfo->LoadLabel(pm, (ICharSequence**)&mDisplayLabel);
        mDisplayLabel->ToString(&mLabel);
        return;
    }

    // Finally... whatever, just pick the first package's name.
    // try {
    AutoPtr<IApplicationInfo> _ai;
    pm->GetApplicationInfo((*pkgs)[0],
            IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&_ai);
    mDisplayLabel = NULL;
    IPackageItemInfo::Probe(_ai)->LoadLabel(pm, (ICharSequence**)&mDisplayLabel);
    mDisplayLabel->ToString(&mLabel);
    mPackageInfo = IPackageItemInfo::Probe(_ai);
    return;
    // } catch (PackageManager.NameNotFoundException e) {
    // }
}

Boolean RunningState::ProcessItem::UpdateService(
    /* [in] */ IContext* context,
    /* [in] */ IActivityManagerRunningServiceInfo* service)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);

    AutoPtr<IComponentName> comService;
    service->GetService((IComponentName**)&comService);
    Boolean changed = FALSE;
    AutoPtr<IInterface> obj;
    mServices->Get(comService, (IInterface**)&obj);
    ServiceItem* si = (ServiceItem*)IRunningStateServiceItem::Probe(obj);
    if (si == NULL) {
        changed = TRUE;
        si = new ServiceItem(mUserId);
        si->mRunningService = service;
        // try {
            AutoPtr<IActivityThreadHelper> helper;
            CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
            AutoPtr<IIPackageManager> pkgManager;
            helper->GetPackageManager((IIPackageManager**)&pkgManager);
            Int32 uid;
            service->GetUid(&uid);
            pkgManager->GetServiceInfo(
                    comService, IPackageManager::GET_UNINSTALLED_PACKAGES,
                    UserHandle::GetUserId(uid), (IServiceInfo**)&si->mServiceInfo);

            if (si->mServiceInfo == NULL) {
                Logger::D("RunningService", "getServiceInfo returned NULL for: %s", TO_CSTR(comService));
                return FALSE;
            }
        // } catch (RemoteException e) {
        // }
        AutoPtr<IComponentName> comName;
        si->mRunningService->GetService((IComponentName**)&comName);
        String className;
        comName->GetClassName(&className);
        si->mDisplayLabel = MakeLabel(pm, className, IPackageItemInfo::Probe(si->mServiceInfo));
        mLabel = String(NULL);
        if (mDisplayLabel != NULL) {
            mDisplayLabel->ToString(&mLabel);
        }
        IComponentInfo::Probe(si->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&si->mPackageInfo);
        mServices->Put(comService, (IRunningStateServiceItem*)si);
    }
    si->mCurSeq = mCurSeq;
    si->mRunningService = service;
    Int64 restarting;
    service->GetRestarting(&restarting);
    Int64 activeSince = -1;
    if (restarting == 0) {
        service->GetActiveSince(&activeSince);
    }
    if (si->mActiveSince != activeSince) {
        si->mActiveSince = activeSince;
        changed = TRUE;
    }
    String clientPackage;
    service->GetClientPackage(&clientPackage);
    Int32 clientLabel;
    if (!clientPackage.IsNull() && (service->GetClientLabel(&clientLabel), clientLabel) != 0) {
        if (si->mShownAsStarted) {
            si->mShownAsStarted = FALSE;
            changed = TRUE;
        }
        // try {
            AutoPtr<IResources> clientr;
            ECode ec = pm->GetResourcesForApplication(clientPackage, (IResources**)&clientr);
            if (SUCCEEDED(ec)) {
                String label;
                clientr->GetString(clientLabel, &label);
                AutoPtr<IResources> resources;
                context->GetResources((IResources**)&resources);
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(label));
                resources->GetString(
                        R::string::service_client_name, args, &si->mDescription);
            }
            else {
                si->mDescription = String(NULL);
            }
        // } catch (PackageManager.NameNotFoundException e) {
        //     si.mDescription = NULL;
        // }
    }
    else {
        if (!si->mShownAsStarted) {
            si->mShownAsStarted = TRUE;
            changed = TRUE;
        }
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetString(
                R::string::service_started_by_app, &si->mDescription);
    }

    return changed;
}

Boolean RunningState::ProcessItem::UpdateSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 pss,
    /* [in] */ Int32 curSeq)
{
    mSize = pss * 1024;
    if (mCurSeq == curSeq) {
        String sizeStr = Formatter::FormatShortFileSize(
                context, mSize);
        if (!sizeStr.Equals(mSizeStr)){
            mSizeStr = sizeStr;
            // We update this on the second tick where we update just
            // the text in the current items, so no need to say we
            // changed here.
            return FALSE;
        }
    }
    return FALSE;
}

Boolean RunningState::ProcessItem::BuildDependencyChain(
    /* [in] */ IContext* context,
    /* [in] */ IPackageManager* pm,
    /* [in] */ Int32 curSeq)
{
    Int32 NP;
    mDependentProcesses->GetSize(&NP);
    Boolean changed = FALSE;
    for (Int32 i = 0; i < NP; i++) {
        AutoPtr<IInterface> obj;
        mDependentProcesses->ValueAt(i, (IInterface**)&obj);
        ProcessItem* proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        if (proc->mClient != this) {
            changed = TRUE;
            proc->mClient = this;
        }
        proc->mCurSeq = curSeq;
        proc->EnsureLabel(pm);
        changed |= proc->BuildDependencyChain(context, pm, curSeq);
    }

    Int32 size;
    if (mLastNumDependentProcesses != (mDependentProcesses->GetSize(&size), size)) {
        changed = TRUE;
        mLastNumDependentProcesses = size;
    }

    return changed;
}

void RunningState::ProcessItem::AddDependentProcesses(
    /* [in] */ IArrayList* dest,//ArrayList<BaseItem>
    /* [in] */ IArrayList* destProc)//ArrayList<ProcessItem>
{
    Int32 NP;
    mDependentProcesses->GetSize(&NP);
    for (Int32 i = 0; i < NP; i++) {
        AutoPtr<IInterface> obj;
        mDependentProcesses->ValueAt(i, (IInterface**)&obj);
        IRunningStateProcessItem* proc = IRunningStateProcessItem::Probe(obj);

        ((ProcessItem*)proc)->AddDependentProcesses(dest, destProc);
        dest->Add(proc);
        if (((ProcessItem*)proc)->mPid > 0) {
            destProc->Add(proc);
        }
    }
}

//===============================================================================
//                  RunningState::MergedItem
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::MergedItem, BaseItem, IRunningStateMergedItem)

RunningState::MergedItem::MergedItem(
    /* [in] */ Int32 userId)
    : BaseItem::BaseItem(FALSE, userId)
    , mLastNumProcesses(-1)
    , mLastNumServices(-1)
{
    CArrayList::New((IArrayList**)&mOtherProcesses);
    CArrayList::New((IArrayList**)&mServices);
    CArrayList::New((IArrayList**)&mChildren);
}

void RunningState::MergedItem::SetDescription(
    /* [in] */ IContext* context,
    /* [in] */ Int32 numProcesses,
    /* [in] */ Int32 numServices)
{
    if (mLastNumProcesses != numProcesses || mLastNumServices != numServices) {
        mLastNumProcesses = numProcesses;
        mLastNumServices = numServices;
        Int32 resid = R::string::running_processes_item_description_s_s;
        if (numProcesses != 1) {
            resid = numServices != 1
                    ? R::string::running_processes_item_description_p_p
                    : R::string::running_processes_item_description_p_s;
        }
        else if (numServices != 1) {
            resid = R::string::running_processes_item_description_s_p;
        }
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, CoreUtils::Convert(numProcesses));
        args->Set(1, CoreUtils::Convert(numServices));
        resources->GetString(resid, args, &mDescription);
    }
}

Boolean RunningState::MergedItem::Update(
    /* [in] */ IContext* context,
    /* [in] */ Boolean background)
{
    mBackground = background;

    Int32 size;
    if (mUser != NULL) {
        // This is a merged item that contains a child collection
        // of items...  that is, it is an entire user, containing
        // everything associated with that user.  So set it up as such.
        // For concrete stuff we need about the process of this item,
        // we will just use the info from the first child.
        AutoPtr<IInterface> obj;
        mChildren->Get(0, (IInterface**)&obj);
        MergedItem* child0 = (MergedItem*)IRunningStateMergedItem::Probe(obj);
        mPackageInfo = child0->mProcess->mPackageInfo;
        mLabel = mUser != NULL ? mUser->mLabel : String(NULL);
        mDisplayLabel = CoreUtils::Convert(mLabel);
        Int32 numProcesses = 0;
        Int32 numServices = 0;
        mActiveSince = -1;
        mChildren->GetSize(&size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> obj;
            mChildren->Get(i, (IInterface**)&obj);
            MergedItem* child = (MergedItem*)IRunningStateMergedItem::Probe(obj);
            numProcesses += child->mLastNumProcesses;
            numServices += child->mLastNumServices;
            if (child->mActiveSince >= 0 && mActiveSince < child->mActiveSince) {
                mActiveSince = child->mActiveSince;
            }
        }
        if (!mBackground) {
            SetDescription(context, numProcesses, numServices);
        }
    }
    else {
        mPackageInfo = mProcess->mPackageInfo;
        mDisplayLabel = mProcess->mDisplayLabel;
        mLabel = mProcess->mLabel;

        if (!mBackground) {
            Int32 size1, size2;
            SetDescription(context,
                    (mProcess->mPid > 0 ? 1 : 0) + (mOtherProcesses->GetSize(&size1), size1),
                    (mServices->GetSize(&size2), size2));
        }

        mActiveSince = -1;
        mServices->GetSize(&size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            ServiceItem* si = (ServiceItem*)IRunningStateServiceItem::Probe(obj);
            if (si->mActiveSince >= 0 && mActiveSince < si->mActiveSince) {
                mActiveSince = si->mActiveSince;
            }
        }
    }

    return FALSE;
}

Boolean RunningState::MergedItem::UpdateSize(
    /* [in] */ IContext* context)
{
    Int32 size;
    if (mUser != NULL) {
        mSize = 0;
        mChildren->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mChildren->Get(i, (IInterface**)&obj);
            MergedItem* child = (MergedItem*)IRunningStateMergedItem::Probe(obj);
            child->UpdateSize(context);
            mSize += child->mSize;
        }
    }
    else {
        mSize = mProcess->mSize;
        mOtherProcesses->GetSize(&size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> obj;
            mOtherProcesses->Get(i, (IInterface**)&obj);
            mSize += ((ProcessItem*)IRunningStateProcessItem::Probe(obj))->mSize;
        }
    }

    String sizeStr = Formatter::FormatShortFileSize(
            context, mSize);
    if (!sizeStr.Equals(mSizeStr)){
        mSizeStr = sizeStr;
        // We update this on the second tick where we update just
        // the text in the current items, so no need to say we
        // changed here.
        return FALSE;
    }
    return FALSE;
}

ECode RunningState::MergedItem::LoadIcon(
    /* [in] */ IContext* context,
    /* [in] */ RunningState* state,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)

    if (mUser == NULL) {
        return BaseItem::LoadIcon(context, state, result);
    }
    if (mUser->mIcon != NULL) {
        AutoPtr<IDrawableConstantState> constState;
        mUser->mIcon->GetConstantState((IDrawableConstantState**)&constState);
        if (constState == NULL) {
            *result = mUser->mIcon;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            return constState->NewDrawable(result);
        }
    }
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    return resources->GetDrawable(
            Elastos::Droid::R::drawable::ic_menu_cc, result);
}

//===============================================================================
//                  RunningState::ServiceProcessComparator
//===============================================================================

CAR_INTERFACE_IMPL(RunningState::ServiceProcessComparator, Object, IComparator)

RunningState::ServiceProcessComparator::ServiceProcessComparator(
    /* [in] */ RunningState* host)
    : mHost(host)
{}

RunningState::ServiceProcessComparator::~ServiceProcessComparator()
{}

ECode RunningState::ServiceProcessComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<ProcessItem> object1 = (ProcessItem*)IRunningStateProcessItem::Probe(lhs);
    AutoPtr<ProcessItem> object2= (ProcessItem*)IRunningStateProcessItem::Probe(rhs);

    if (object1->mUserId != object2->mUserId) {
        if (object1->mUserId == mHost->mMyUserId) {
            *result = -1;
            return NOERROR;
        }
        if (object2->mUserId == mHost->mMyUserId) {
            *result = 1;
            return NOERROR;
        }
        *result = object1->mUserId < object2->mUserId ? -1 : 1;
        return NOERROR;
    }
    if (object1->mIsStarted != object2->mIsStarted) {
        // Non-started processes go last.
        *result = object1->mIsStarted ? -1 : 1;
        return NOERROR;
    }
    if (object1->mIsSystem != object2->mIsSystem) {
        // System processes go below non-system.
        *result = object1->mIsSystem ? 1 : -1;
        return NOERROR;
    }
    if (object1->mActiveSince != object2->mActiveSince) {
        // Remaining ones are sorted with the longest running
        // services last.
        *result = (object1->mActiveSince > object2->mActiveSince) ? -1 : 1;
        return NOERROR;
    }
    return NOERROR;
}

//===============================================================================
//                  RunningState
//===============================================================================

AutoPtr<ICharSequence> RunningState::MakeLabel(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& className,
    /* [in] */ IPackageItemInfo* item)
{
    Int32 labelRes;
    AutoPtr<ICharSequence> nonLocalizedLabel;
    if (item != NULL && ((item->GetLabelRes(&labelRes), labelRes) != 0
            || (item->GetNonLocalizedLabel((ICharSequence**)&nonLocalizedLabel),
                nonLocalizedLabel) != NULL)) {
        AutoPtr<ICharSequence> label;
        item->LoadLabel(pm, (ICharSequence**)&label);
        if (label != NULL) {
            return label;
        }
    }

    String label = className;
    Int32 tail = label.LastIndexOf('.');
    if (tail >= 0) {
        label = label.Substring(tail + 1, label.GetLength());
    }

    AutoPtr<ICharSequence> cs = CoreUtils::Convert(label);
    return cs;
}

AutoPtr<RunningState> RunningState::GetInstance(
    /* [in] */ IContext* context)
{
    {
        AutoLock syncLock(sGlobalLock);
        if (sInstance == NULL) {
            sInstance = new RunningState(context);
        }
        return sInstance;
    }
}

RunningState::RunningState(
    /* [in] */ IContext* context)
    : mMyUserId(0)
    , mSequence(0)
    , mResumed(FALSE)
    , mHaveData(FALSE)
    , mWatchingBackgroundItems(FALSE)
    , mNumBackgroundProcesses(0)
    , mBackgroundProcessMemory(0)
    , mNumForegroundProcesses(0)
    , mForegroundProcessMemory(0)
    , mNumServiceProcesses(0)
    , mServiceProcessMemory(0)
{
    context->GetApplicationContext((IContext**)&mApplicationContext);
    AutoPtr<IInterface> obj;
    mApplicationContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    mAm = IActivityManager::Probe(obj);
    mApplicationContext->GetPackageManager((IPackageManager**)&mPm);
    obj = NULL;
    mApplicationContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IUserManager::Probe(obj);
    mMyUserId = UserHandle::GetMyUserId();
    mResumed = FALSE;
    CHandlerThread::New(String("RunningState:Background"), (IHandlerThread**)&mBackgroundThread);
    IThread::Probe(mBackgroundThread)->Start();
    AutoPtr<ILooper> looper;
    mBackgroundThread->GetLooper((ILooper**)&looper);
    mBackgroundHandler = new BackgroundHandler(looper, this);

    mInterestingConfigChanges = new InterestingConfigChanges();
    CSparseArray::New((ISparseArray**)&mServiceProcessesByName);
    CSparseArray::New((ISparseArray**)&mServiceProcessesByPid);
    mServiceProcessComparator = new ServiceProcessComparator(this);
    CArrayList::New((IArrayList**)&mInterestingProcesses);
    CSparseArray::New((ISparseArray**)&mRunningProcesses);
    CArrayList::New((IArrayList**)&mProcessItems);
    CArrayList::New((IArrayList**)&mAllProcessItems);

    CSparseArray::New((ISparseArray**)&mOtherUserMergedItems);
    CSparseArray::New((ISparseArray**)&mOtherUserBackgroundItems);
    CSparseArray::New((ISparseArray**)&mUsers);
    CSparseArray::New((ISparseArray**)&mTmpAppProcesses);

    mBackgroundComparator = new InitComparator(this);

    mLock = new Object();
    CArrayList::New((IArrayList**)&mItems);
    CArrayList::New((IArrayList**)&mMergedItems);
    CArrayList::New((IArrayList**)&mBackgroundItems);
    CArrayList::New((IArrayList**)&mUserBackgroundItems);

    mHandler = new InitHandler(this);
}

void RunningState::Resume(
    /* [in] */ IRunningStateOnRefreshUiListener* listener)
{
    {
        AutoLock syncLock(mLock);
        mResumed = TRUE;
        mRefreshUiListener = listener;
        AutoPtr<IResources> resources;
        mApplicationContext->GetResources((IResources**)&resources);
        Boolean res;
        if (mInterestingConfigChanges->ApplyNewConfig(resources)) {
            mHaveData = FALSE;
            mBackgroundHandler->RemoveMessages(MSG_RESET_CONTENTS);
            mBackgroundHandler->RemoveMessages(MSG_UPDATE_CONTENTS);
            mBackgroundHandler->SendEmptyMessage(MSG_RESET_CONTENTS, &res);
        }
        if (mBackgroundHandler->HasMessages(MSG_UPDATE_CONTENTS, &res), !res) {
            mBackgroundHandler->SendEmptyMessage(MSG_UPDATE_CONTENTS, &res);
        }
        mHandler->SendEmptyMessage(MSG_UPDATE_TIME, &res);
    }
}

void RunningState::UpdateNow()
{
    {
        AutoLock syncLock(mLock);
        mBackgroundHandler->RemoveMessages(MSG_UPDATE_CONTENTS);
        Boolean res;
        mBackgroundHandler->SendEmptyMessage(MSG_UPDATE_CONTENTS, &res);
    }
}

Boolean RunningState::HasData()
{
    {
        AutoLock syncLock(mLock);
        return mHaveData;
    }
}

void RunningState::WaitForData()
{
    {
        AutoLock syncLock(mLock);
        while (!mHaveData) {
            // try {
                mLock->Wait(0);
            // } catch (InterruptedException e) {
            // }
        }
    }
}

void RunningState::Pause()
{
    {
        AutoLock syncLock(mLock);
        mResumed = FALSE;
        mRefreshUiListener = NULL;
        mHandler->RemoveMessages(MSG_UPDATE_TIME);
    }
}

Boolean RunningState::IsInterestingProcess(
    /* [in] */ IActivityManagerRunningAppProcessInfo* pi)
{
    Int32 flags;
    pi->GetFlags(&flags);
    if ((flags & IActivityManagerRunningAppProcessInfo::FLAG_CANT_SAVE_STATE) != 0) {
        return TRUE;
    }
    Int32 importance, importanceReasonCode;
    if ((flags & IActivityManagerRunningAppProcessInfo::FLAG_PERSISTENT) == 0
            && (pi->GetImportance(&importance), importance) >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND
            && importance < IActivityManagerRunningAppProcessInfo::IMPORTANCE_CANT_SAVE_STATE
            && (pi->GetImportanceReasonCode(&importanceReasonCode), importanceReasonCode)
                    == IActivityManagerRunningAppProcessInfo::REASON_UNKNOWN) {
        return TRUE;
    }
    return FALSE;
}

void RunningState::Reset()
{
    mServiceProcessesByName->Clear();
    mServiceProcessesByPid->Clear();
    mInterestingProcesses->Clear();
    mRunningProcesses->Clear();
    mProcessItems->Clear();
    mAllProcessItems->Clear();
    mUsers->Clear();
}

void RunningState::AddOtherUserItem(
    /* [in] */ IContext* context,
    /* [in] */ IArrayList* newMergedItems,//ArrayList<MergedItem>
    /* [in] */ ISparseArray* userItems, //SparseArray<MergedItem>
    /* [in] */ MergedItem* newItem)
{
    AutoPtr<IInterface> obj;
    userItems->Get(newItem->mUserId, (IInterface**)&obj);
    AutoPtr<MergedItem> userItem = (MergedItem*)IRunningStateMergedItem::Probe(obj);
    Boolean first = userItem == NULL || userItem->mCurSeq != mSequence;
    if (first) {
        if (userItem == NULL) {
            userItem = new MergedItem(newItem->mUserId);
            userItems->Put(newItem->mUserId, (IRunningStateMergedItem*)userItem);
        }
        else {
            userItem->mChildren->Clear();
        }
        userItem->mCurSeq = mSequence;
        obj = NULL;
        mUsers->Get(newItem->mUserId, (IInterface**)&obj);
        if ((userItem->mUser = (UserState*)IObject::Probe(obj)) == NULL) {
            userItem->mUser = new UserState();
            AutoPtr<IUserInfo> info;
            mUm->GetUserInfo(newItem->mUserId, (IUserInfo**)&info);
            userItem->mUser->mInfo = info;
            if (info != NULL) {
                userItem->mUser->mIcon = Utils::GetUserIcon(context, mUm, info);
            }
            String name;
            if (info != NULL) {
                info->GetName(&name);
            }
            if (name.IsNull() && info != NULL) {
                Int32 id;
                info->GetId(&id);
                name = StringUtils::ToString(id);
            }
            else if (info == NULL) {
                context->GetString(R::string::unknown, &name);
            }
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(name));
            resources->GetString(
                    R::string::running_process_item_user_label, args, &userItem->mUser->mLabel);
        }
        newMergedItems->Add((IRunningStateMergedItem*)userItem);
    }
    userItem->mChildren->Add((IRunningStateMergedItem*)newItem);
}

Boolean RunningState::Update(
    /* [in] */ IContext* context,
    /* [in] */ IActivityManager* am)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);

    mSequence++;

    Boolean changed = FALSE;

    // Retrieve list of services, filtering out anything that definitely
    // won't be shown in the UI.
    AutoPtr<IList> services; // List<ActivityManager.RunningServiceInfo>
    am->GetRunningServices(MAX_SERVICES, (IList**)&services);
    Int32 NS = 0;
    if (services != NULL) {
        services->GetSize(&NS);
    }
    for (Int32 i = 0; i < NS; i++) {
        AutoPtr<IInterface> obj;
        services->Get(i, (IInterface**)&obj);
        IActivityManagerRunningServiceInfo* si = IActivityManagerRunningServiceInfo::Probe(obj);
        // We are not interested in services that have not been started
        // and don't have a known client, because
        // there is nothing the user can do about them.
        Boolean started;
        si->IsStarted(&started);
        Int32 clientLabel;
        if (!started && (si->GetClientLabel(&clientLabel), clientLabel) == 0) {
            services->Remove(i);
            i--;
            NS--;
            continue;
        }
        // We likewise don't care about services running in a
        // persistent process like the system or phone.
        Int32 flags;
        si->GetFlags(&flags);
        if ((flags & IActivityManagerRunningServiceInfo::FLAG_PERSISTENT_PROCESS) != 0) {
            services->Remove(i);
            i--;
            NS--;
            continue;
        }
    }

    // Retrieve list of running processes, organizing them into a sparse
    // array for easy retrieval.
    AutoPtr<IList> processes;// List<ActivityManager.RunningAppProcessInfo>
    am->GetRunningAppProcesses((IList**)&processes);
    Int32 NP = 0;
    if (processes != NULL) {
        processes->GetSize(&NP);
    }
    mTmpAppProcesses->Clear();
    for (Int32 i = 0; i < NP; i++) {
        AutoPtr<IInterface> obj;
        processes->Get(i, (IInterface**)&obj);
        IActivityManagerRunningAppProcessInfo* pi = IActivityManagerRunningAppProcessInfo::Probe(obj);
        Int32 pid;
        pi->GetPid(&pid);
        mTmpAppProcesses->Put(pid, (IObject*)new AppProcessInfo(pi));
    }

    // Initial iteration through running services to collect per-process
    // info about them.
    for (Int32 i = 0; i < NS; i++) {
        AutoPtr<IInterface> obj;
        services->Get(i, (IInterface**)&obj);
        IActivityManagerRunningServiceInfo* si = IActivityManagerRunningServiceInfo::Probe(obj);
        Int64 restarting;
        si->GetRestarting(&restarting);
        Int32 pid;
        if (restarting == 0 && (si->GetPid(&pid), pid) > 0) {
            AutoPtr<IInterface> obj;
            mTmpAppProcesses->Get(pid, (IInterface**)&obj);
            AppProcessInfo* ainfo = (AppProcessInfo*)IObject::Probe(obj);
            if (ainfo != NULL) {
                ainfo->mHasServices = TRUE;
                Boolean foreground;
                si->GetForeground(&foreground);
                if (foreground) {
                    ainfo->mHasForegroundServices = TRUE;
                }
            }
        }
    }

    // Update state we are maintaining about process that are running services.
    for (Int32 i = 0; i < NS; i++) {
        AutoPtr<IInterface> obj;
        services->Get(i, (IInterface**)&obj);
        IActivityManagerRunningServiceInfo* si = IActivityManagerRunningServiceInfo::Probe(obj);

        // If this service's process is in use at a higher importance
        // due to another process bound to one of its services, then we
        // won't put it in the top-level list of services.  Instead we
        // want it to be included in the set of processes that the other
        // process needs.
        Int64 restarting;
        si->GetRestarting(&restarting);
        Int32 pid;
        if (restarting == 0 && (si->GetPid(&pid), pid) > 0) {
            AutoPtr<IInterface> obj;
            mTmpAppProcesses->Get(pid, (IInterface**)&obj);
            AppProcessInfo* ainfo = (AppProcessInfo*)IObject::Probe(obj);
            if (ainfo != NULL && !ainfo->mHasForegroundServices) {
                // This process does not have any foreground services.
                // If its importance is greater than the service importance
                // then there is something else more significant that is
                // keeping it around that it should possibly be included as
                // a part of instead of being shown by itself.
                Int32 importance;
                ainfo->mInfo->GetImportance(&importance);
                if (importance < IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE) {
                    // Follow process chain to see if there is something
                    // else that could be shown
                    Boolean skip = FALSE;
                    Int32 importanceReasonPid;
                    ainfo->mInfo->GetImportanceReasonPid(&importanceReasonPid);
                    obj = NULL;
                    mTmpAppProcesses->Get(importanceReasonPid, (IInterface**)&obj);
                    ainfo = (AppProcessInfo*)IObject::Probe(obj);
                    while (ainfo != NULL) {
                        if (ainfo->mHasServices || IsInterestingProcess(ainfo->mInfo)) {
                            skip = TRUE;
                            break;
                        }
                        ainfo->mInfo->GetImportanceReasonPid(&importanceReasonPid);
                        obj = NULL;
                        mTmpAppProcesses->Get(importanceReasonPid, (IInterface**)&obj);
                        ainfo = (AppProcessInfo*)IObject::Probe(obj);
                    }
                    if (skip) {
                        continue;
                    }
                }
            }
        }

        Int32 uid;
        si->GetUid(&uid);
        obj = NULL;
        mServiceProcessesByName->Get(uid, (IInterface**)&obj);
        AutoPtr<IHashMap> procs = IHashMap::Probe(obj);//HashMap<String, ProcessItem>
        if (procs == NULL) {
            CHashMap::New((IHashMap**)&procs);
            mServiceProcessesByName->Put(uid, procs);
        }
        String process;
        si->GetProcess(&process);
        obj = NULL;
        procs->Get(CoreUtils::Convert(process), (IInterface**)&obj);
        AutoPtr<ProcessItem> proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        if (proc == NULL) {
            changed = TRUE;
            proc = new ProcessItem(context, uid, process);
            procs->Put(CoreUtils::Convert(process), (IRunningStateProcessItem*)proc);
        }

        if (proc->mCurSeq != mSequence) {
            Int32 pid = 0;
            if (restarting == 0) {
                si->GetPid(&pid);
            }
            if (pid != proc->mPid) {
                changed = TRUE;
                if (proc->mPid != pid) {
                    if (proc->mPid != 0) {
                        mServiceProcessesByPid->Remove(proc->mPid);
                    }
                    if (pid != 0) {
                        mServiceProcessesByPid->Put(pid, (IRunningStateProcessItem*)proc);
                    }
                    proc->mPid = pid;
                }
            }
            proc->mDependentProcesses->Clear();
            proc->mCurSeq = mSequence;
        }
        changed |= proc->UpdateService(context, si);
    }

    // Now update the map of other processes that are running (but
    // don't have services actively running inside them).
    for (Int32 i = 0; i < NP; i++) {
        AutoPtr<IInterface> obj;
        processes->Get(i, (IInterface**)&obj);
        AutoPtr<IActivityManagerRunningAppProcessInfo> pi = IActivityManagerRunningAppProcessInfo::Probe(obj);
        Int32 pid;
        pi->GetPid(&pid);
        obj = NULL;
        mServiceProcessesByPid->Get(pid, (IInterface**)&obj);
        AutoPtr<ProcessItem> proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        if (proc == NULL) {
            // This process is not one that is a direct container
            // of a service, so look for it in the secondary
            // running list.
            obj = NULL;
            mRunningProcesses->Get(pid, (IInterface**)&obj);
            proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
            if (proc == NULL) {
                changed = TRUE;
                Int32 uid;
                pi->GetUid(&uid);
                String processName;
                pi->GetProcessName(&processName);
                proc = new ProcessItem(context, uid, processName);
                proc->mPid = pid;
                mRunningProcesses->Put(pid, (IRunningStateProcessItem*)proc);
            }
            proc->mDependentProcesses->Clear();
        }

        if (IsInterestingProcess(pi)) {
            Boolean res;
            mInterestingProcesses->Contains((IRunningStateProcessItem*)proc, &res);
            if (!res) {
                changed = TRUE;
                mInterestingProcesses->Add((IRunningStateProcessItem*)proc);
            }
            proc->mCurSeq = mSequence;
            proc->mInteresting = TRUE;
            proc->EnsureLabel(pm);
        }
        else {
            proc->mInteresting = FALSE;
        }

        proc->mRunningSeq = mSequence;
        proc->mRunningProcessInfo = pi;
    }

    // Build the chains from client processes to the process they are
    // dependent on; also remove any old running processes.
    Int32 NRP;
    mRunningProcesses->GetSize(&NRP);
    for (Int32 i = 0; i < NRP;) {
        AutoPtr<IInterface> obj;
        mRunningProcesses->ValueAt(i, (IInterface**)&obj);
        AutoPtr<ProcessItem> proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        if (proc->mRunningSeq == mSequence) {
            Int32 clientPid;
            proc->mRunningProcessInfo->GetImportanceReasonPid(&clientPid);
            if (clientPid != 0) {
                obj = NULL;
                mServiceProcessesByPid->Get(clientPid, (IInterface**)&obj);
                AutoPtr<ProcessItem> client = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
                if (client == NULL) {
                    obj = NULL;
                    mRunningProcesses->Get(clientPid, (IInterface**)&obj);
                    client = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
                }
                if (client != NULL) {
                    client->mDependentProcesses->Put(proc->mPid, (IRunningStateProcessItem*)proc);
                }
            }
            else {
                // In this pass the process doesn't have a client.
                // Clear to make sure that, if it later gets the same one,
                // we will detect the change.
                proc->mClient = NULL;
            }
            i++;
        }
        else {
            changed = TRUE;
            Int32 value;
            mRunningProcesses->KeyAt(i, &value);
            mRunningProcesses->Remove(value);
            NRP--;
        }
    }

    // Remove any old interesting processes.
    Int32 NHP;
    mInterestingProcesses->GetSize(&NHP);
    for (Int32 i = 0; i < NHP; i++) {
        AutoPtr<IInterface> obj;
        mInterestingProcesses->Get(i, (IInterface**)&obj);
        ProcessItem* proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        obj = NULL;
        if (!proc->mInteresting || (mRunningProcesses->Get(proc->mPid, (IInterface**)&obj), obj) == NULL) {
            changed = TRUE;
            mInterestingProcesses->Remove(i);
            i--;
            NHP--;
        }
    }

    // Follow the tree from all primary service processes to all
    // processes they are dependent on, marking these processes as
    // still being active and determining if anything has changed.
    Int32 NAP;
    mServiceProcessesByPid->GetSize(&NAP);
    for (Int32 i = 0; i < NAP; i++) {
        AutoPtr<IInterface> obj;
        mServiceProcessesByPid->ValueAt(i, (IInterface**)&obj);
        ProcessItem* proc = (ProcessItem*) IRunningStateProcessItem::Probe(obj);
        if (proc->mCurSeq == mSequence) {
            changed |= proc->BuildDependencyChain(context, pm, mSequence);
        }
    }

    // Look for services and their primary processes that no longer exist...
    AutoPtr<IArrayList> uidToDelete; // ArrayList<Integer>
    Int32 size;
    mServiceProcessesByName->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServiceProcessesByName->ValueAt(i, (IInterface**)&obj);
        IHashMap* procs = IHashMap::Probe(obj); //HashMap<String, ProcessItem>
        AutoPtr<ICollection> collection;
        procs->GetValues((ICollection**)&collection);
        AutoPtr<IIterator> pit; //Iterator<ProcessItem>
        collection->GetIterator((IIterator**)&pit);
        Boolean res;
        while (pit->HasNext(&res), res) {
            obj = NULL;
            pit->GetNext((IInterface**)&obj);
            ProcessItem* pi = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
            if (pi->mCurSeq == mSequence) {
                pi->EnsureLabel(pm);
                if (pi->mPid == 0) {
                    // Sanity: a non-process can't be dependent on
                    // anything.
                    pi->mDependentProcesses->Clear();
                }
            }
            else {
                changed = TRUE;
                pit->Remove();
                if ((procs->GetSize(&size), size) == 0) {
                    if (uidToDelete == NULL) {
                        CArrayList::New((IArrayList**)&uidToDelete);
                    }
                    Int32 key;
                    mServiceProcessesByName->KeyAt(i, &key);
                    uidToDelete->Add(CoreUtils::Convert(key));
                }
                if (pi->mPid != 0) {
                    mServiceProcessesByPid->Remove(pi->mPid);
                }
                continue;
            }

            AutoPtr<ICollection> coll;
            pi->mServices->GetValues((ICollection**)&coll);
            AutoPtr<IIterator> sit; //Iterator<ServiceItem>
            coll->GetIterator((IIterator**)&pit);
            Boolean result;
            while (sit->HasNext(&result), result) {
                obj = NULL;
                sit->GetNext((IInterface**)&obj);
                ServiceItem* si = (ServiceItem*)IRunningStateServiceItem::Probe(obj);
                if (si->mCurSeq != mSequence) {
                    changed = TRUE;
                    sit->Remove();
                }
            }
        }
    }

    if (uidToDelete != NULL) {
        uidToDelete->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            uidToDelete->Get(i, (IInterface**)&obj);
            Int32 uid;
            IInteger32::Probe(obj)->GetValue(&uid);
            mServiceProcessesByName->Remove(uid);
        }
    }

    if (changed) {
        // First determine an order for the services.
        AutoPtr<IArrayList> sortedProcesses; // ArrayList<ProcessItem>
        CArrayList::New((IArrayList**)&sortedProcesses);
        mServiceProcessesByName->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mServiceProcessesByName->ValueAt(i, (IInterface**)&obj);
            AutoPtr<ICollection> values;
            IHashMap::Probe(obj)->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while(it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> piObj;
                it->GetNext((IInterface**)&piObj);
                ProcessItem* pi = (ProcessItem*)IRunningStateProcessItem::Probe(piObj);

                pi->mIsSystem = FALSE;
                pi->mIsStarted = TRUE;
                pi->mActiveSince = Elastos::Core::Math::INT64_MAX_VALUE;
                AutoPtr<ICollection> servicesValues;
                pi->mServices->GetValues((ICollection**)&servicesValues);
                AutoPtr<IIterator> servicesIterator;
                servicesValues->GetIterator((IIterator**)&servicesIterator);
                while(servicesIterator->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> siObj;
                    servicesIterator->GetNext((IInterface**)&siObj);
                    ServiceItem* si = (ServiceItem*)IRunningStateServiceItem::Probe(siObj);

                    if (si->mServiceInfo != NULL) {
                        AutoPtr<IApplicationInfo> info;
                        IComponentInfo::Probe(si->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&info);
                        Int32 flags;
                        info->GetFlags(&flags);
                        if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                            pi->mIsSystem = TRUE;
                        }
                    }
                    Int32 clientLabel;
                    if (si->mRunningService != NULL
                            && (si->mRunningService->GetClientLabel(&clientLabel), clientLabel) != 0) {
                        pi->mIsStarted = FALSE;
                        Int64 activeSince;
                        si->mRunningService->GetActiveSince(&activeSince);
                        if (pi->mActiveSince > activeSince) {
                            pi->mActiveSince = activeSince;
                        }
                    }
                }
                sortedProcesses->Add((IRunningStateProcessItem*)pi);
            }
        }

        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(IList::Probe(sortedProcesses), mServiceProcessComparator);

        AutoPtr<IArrayList> newItems; // ArrayList<BaseItem>
        CArrayList::New((IArrayList**)&newItems);
        AutoPtr<IArrayList> newMergedItems; // ArrayList<MergedItem>
        CArrayList::New((IArrayList**)&newMergedItems);
        AutoPtr<ISparseArray> otherUsers; // SparseArray<MergedItem>
        mProcessItems->Clear();
        sortedProcesses->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            sortedProcesses->Get(i, (IInterface**)&obj);
            ProcessItem* pi = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
            pi->mNeedDivider = FALSE;

            Int32 firstProc;
            mProcessItems->GetSize(&firstProc);
            // First add processes we are dependent on.
            pi->AddDependentProcesses(newItems, mProcessItems);
            // And add the process itself.
            newItems->Add((IRunningStateProcessItem*)pi);
            if (pi->mPid > 0) {
                mProcessItems->Add((IRunningStateProcessItem*)pi);
            }

            // Now add the services running in it.
            AutoPtr<MergedItem> mergedItem;
            Boolean haveAllMerged = FALSE;
            Boolean needDivider = FALSE;

            AutoPtr<ICollection> servicesValues;
            pi->mServices->GetValues((ICollection**)&servicesValues);
            AutoPtr<IIterator> iterator;
            servicesValues->GetIterator((IIterator**)&iterator);
            Boolean hasNext;
            while (iterator->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                iterator->GetNext((IInterface**)&tmp);
                ServiceItem* si = (ServiceItem*)IRunningStateServiceItem::Probe(tmp);

                si->mNeedDivider = needDivider;
                needDivider = TRUE;
                newItems->Add((IRunningStateServiceItem*)si);
                if (si->mMergedItem != NULL) {
                    if (mergedItem != NULL && mergedItem.Get() != (MergedItem*)si->mMergedItem.Get()) {
                        haveAllMerged = FALSE;
                    }
                    mergedItem = (MergedItem*)si->mMergedItem.Get();
                }
                else {
                    haveAllMerged = FALSE;
                }
            }

            Int32 size1, size2;
            if (!haveAllMerged || mergedItem == NULL
                    || (mergedItem->mServices->GetSize(&size1), size1)
                    != (pi->mServices->GetSize(&size2), size2)) {
                // Whoops, we need to build a new MergedItem!
                mergedItem = new MergedItem(pi->mUserId);

                servicesValues = NULL;
                pi->mServices->GetValues((ICollection**)&servicesValues);
                iterator = NULL;
                servicesValues->GetIterator((IIterator**)&iterator);
                while (iterator->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> tmp;
                    iterator->GetNext((IInterface**)&tmp);
                    IRunningStateServiceItem* si = IRunningStateServiceItem::Probe(tmp);
                    mergedItem->mServices->Add(si);
                    ((ServiceItem*)si)->mMergedItem = mergedItem;
                }
                mergedItem->mProcess = pi;
                mergedItem->mOtherProcesses->Clear();
                Int32 count;
                mProcessItems->GetSize(&count);
                for (Int32 mpi = firstProc; mpi < (count - 1); mpi++) {
                    AutoPtr<IInterface> tmp;
                    mProcessItems->Get(mpi, (IInterface**)&tmp);
                    mergedItem->mOtherProcesses->Add(tmp);
                }
            }

            mergedItem->Update(context, FALSE);
            if (mergedItem->mUserId != mMyUserId) {
                AddOtherUserItem(context, newMergedItems, mOtherUserMergedItems, mergedItem);
            }
            else {
                newMergedItems->Add((IRunningStateMergedItem*)mergedItem);
            }
        }

        // Finally, interesting processes need to be shown and will
        // go at the top.
        mInterestingProcesses->GetSize(&NHP);
        for (Int32 i = 0; i < NHP; i++) {
            AutoPtr<IInterface> tmp;
            mInterestingProcesses->Get(i, (IInterface**)&tmp);
            AutoPtr<ProcessItem> proc = (ProcessItem*)IRunningStateProcessItem::Probe(tmp);
            if (proc->mClient == NULL && (proc->mServices->GetSize(&size), size) <= 0) {
                if (proc->mMergedItem == NULL) {
                    proc->mMergedItem = new MergedItem(proc->mUserId);
                    ((MergedItem*)proc->mMergedItem.Get())->mProcess = proc;
                }

                MergedItem* _proc = (MergedItem*)proc->mMergedItem.Get();
                _proc->Update(context, FALSE);
                if (_proc->mUserId != mMyUserId) {
                    AddOtherUserItem(context, newMergedItems, mOtherUserMergedItems,
                            _proc);
                }
                else {
                    newMergedItems->Add(0, (IRunningStateMergedItem*)proc->mMergedItem);
                }
                mProcessItems->Add((IRunningStateProcessItem*)proc);
            }
        }

        // Finally finally, user aggregated merged items need to be
        // updated now that they have all of their children.
        Int32 NU;
        mOtherUserMergedItems->GetSize(&NU);
        for (Int32 i = 0; i < NU; i++) {
            AutoPtr<IInterface> tmp;
            mOtherUserMergedItems->ValueAt(i, (IInterface**)&tmp);
            MergedItem* user = (MergedItem*)IRunningStateMergedItem::Probe(tmp);
            if (user->mCurSeq == mSequence) {
                user->Update(context, FALSE);
            }
        }

        {
            AutoLock syncLock(mLock);
            mItems = newItems;
            mMergedItems = newMergedItems;
        }
    }

    // Count number of interesting other (non-active) processes, and
    // build a list of all processes we will retrieve memory for.
    mAllProcessItems->Clear();
    mAllProcessItems->AddAll(ICollection::Probe(mProcessItems));
    Int32 numBackgroundProcesses = 0;
    Int32 numForegroundProcesses = 0;
    Int32 numServiceProcesses = 0;
    mRunningProcesses->GetSize(&NRP);
    for (Int32 i = 0; i < NRP; i++) {
        AutoPtr<IInterface> obj;
        mRunningProcesses->ValueAt(i, (IInterface**)&obj);
        AutoPtr<ProcessItem> proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
        if (proc->mCurSeq != mSequence) {
            // We didn't hit this process as a dependency on one
            // of our active ones, so add it up if needed.
            Int32 importance;
            proc->mRunningProcessInfo->GetImportance(&importance);
            if (importance >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND) {
                numBackgroundProcesses++;
                mAllProcessItems->Add((IRunningStateProcessItem*)proc);
            }
            else if (importance <= IActivityManagerRunningAppProcessInfo::IMPORTANCE_VISIBLE) {
                numForegroundProcesses++;
                mAllProcessItems->Add((IRunningStateProcessItem*)proc);
            }
            else {
                Logger::I("RunningState", "Unknown non-service process: %s #%d", proc->mProcessName.string(), proc->mPid);
            }
        }
        else {
            numServiceProcesses++;
        }
    }

    Int64 backgroundProcessMemory = 0;
    Int64 foregroundProcessMemory = 0;
    Int64 serviceProcessMemory = 0;
    AutoPtr<IArrayList> newBackgroundItems; //ArrayList<MergedItem>
    AutoPtr<IArrayList> newUserBackgroundItems; //ArrayList<MergedItem>
    Boolean diffUsers = FALSE;
    // try {
        Int32 numProc;
        mAllProcessItems->GetSize(&numProc);
        AutoPtr< ArrayOf<Int32> > pids = ArrayOf<Int32>::Alloc(numProc);
        for (Int32 i = 0; i < numProc; i++) {
            AutoPtr<IInterface> obj;
            mAllProcessItems->Get(i, (IInterface**)&obj);
            (*pids)[i] = ((ProcessItem*)IRunningStateProcessItem::Probe(obj))->mPid;
        }
        AutoPtr< ArrayOf<Int64> > pss;
        ActivityManagerNative::GetDefault()->GetProcessPss(pids, (ArrayOf<Int64>**)&pss);
        Int32 bgIndex = 0;
        for (Int32 i = 0; i < pids->GetLength(); i++) {
            AutoPtr<IInterface> obj;
            mAllProcessItems->Get(i, (IInterface**)&obj);
            ProcessItem* proc = (ProcessItem*)IRunningStateProcessItem::Probe(obj);
            changed |= proc->UpdateSize(context, (*pss)[i], mSequence);
            Int32 importance;
            if (proc->mCurSeq == mSequence) {
                serviceProcessMemory += proc->mSize;
            }
            else if ((proc->mRunningProcessInfo->GetImportance(&importance), importance) >=
                    IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND) {
                backgroundProcessMemory += proc->mSize;
                AutoPtr<MergedItem> mergedItem;
                if (newBackgroundItems != NULL) {
                    proc->mMergedItem = new MergedItem(proc->mUserId);
                    mergedItem = (MergedItem*)proc->mMergedItem.Get();
                    ((MergedItem*)proc->mMergedItem.Get())->mProcess = proc;
                    diffUsers |= mergedItem->mUserId != mMyUserId;
                    newBackgroundItems->Add((IRunningStateMergedItem*)mergedItem);
                }
                else {
                    AutoPtr<IInterface> tmp;
                    if (bgIndex >= (mBackgroundItems->GetSize(&size), size)
                            || (mBackgroundItems->Get(bgIndex, (IInterface**)&tmp),
                                ((MergedItem*)IRunningStateMergedItem::Probe(tmp))->mProcess).Get() != proc) {
                        CArrayList::New(numBackgroundProcesses, (IArrayList**)&newBackgroundItems);
                        for (Int32 bgi = 0; bgi < bgIndex; bgi++) {
                            AutoPtr<IInterface> tmpObj;
                            mBackgroundItems->Get(bgi, (IInterface**)&tmpObj);
                            mergedItem = (MergedItem*)IRunningStateMergedItem::Probe(tmpObj);
                            diffUsers |= mergedItem->mUserId != mMyUserId;
                            newBackgroundItems->Add((IRunningStateMergedItem*)mergedItem);
                        }
                        proc->mMergedItem = new MergedItem(proc->mUserId);
                        mergedItem = (MergedItem*)proc->mMergedItem.Get();
                        ((MergedItem*)proc->mMergedItem.Get())->mProcess = proc;
                        diffUsers |= mergedItem->mUserId != mMyUserId;
                        newBackgroundItems->Add((IRunningStateMergedItem*)mergedItem);
                    }
                    else {
                        AutoPtr<IInterface> tmpObj;
                        mBackgroundItems->Get(bgIndex, (IInterface**)&tmpObj);
                        mergedItem = (MergedItem*)IRunningStateMergedItem::Probe(tmpObj);
                    }
                }
                mergedItem->Update(context, TRUE);
                mergedItem->UpdateSize(context);
                bgIndex++;
            }
            else if ((proc->mRunningProcessInfo->GetImportance(&importance), importance) <=
                    IActivityManagerRunningAppProcessInfo::IMPORTANCE_VISIBLE) {
                foregroundProcessMemory += proc->mSize;
            }
        }
    // } catch (RemoteException e) {
    // }

    if (newBackgroundItems == NULL) {
        // One or more at the bottom may no longer exist.
        if ((mBackgroundItems->GetSize(&size), size) > numBackgroundProcesses) {
            CArrayList::New(numBackgroundProcesses, (IArrayList**)&newBackgroundItems);
            for (Int32 bgi = 0; bgi < numBackgroundProcesses; bgi++) {
                AutoPtr<IInterface> obj;
                mBackgroundItems->Get(bgi, (IInterface**)&obj);
                MergedItem* mergedItem = (MergedItem*)IRunningStateMergedItem::Probe(obj);
                diffUsers |= mergedItem->mUserId != mMyUserId;
                newBackgroundItems->Add((IRunningStateMergedItem*)mergedItem);
            }
        }
    }

    if (newBackgroundItems != NULL) {
        // The background items have changed; we need to re-build the
        // per-user items.
        if (!diffUsers) {
            // Easy: there are no other users, we can just use the same array.
            newUserBackgroundItems = newBackgroundItems;
        }
        else {
            // We now need to re-build the per-user list so that background
            // items for users are collapsed together.
            newUserBackgroundItems = NULL;
            CArrayList::New((IArrayList**)&newUserBackgroundItems);
            Int32 NB;
            newBackgroundItems->GetSize(&NB);
            for (Int32 i = 0; i < NB; i++) {
                AutoPtr<IInterface> obj;
                newBackgroundItems->Get(i, (IInterface**)&obj);
                MergedItem* mergedItem = (MergedItem*)IRunningStateMergedItem::Probe(obj);
                if (mergedItem->mUserId != mMyUserId) {
                    AddOtherUserItem(context, newUserBackgroundItems,
                            mOtherUserBackgroundItems, mergedItem);
                }
                else {
                    newUserBackgroundItems->Add((IRunningStateMergedItem*)mergedItem);
                }
            }
            // And user aggregated merged items need to be
            // updated now that they have all of their children.
            Int32 NU;
            mOtherUserBackgroundItems->GetSize(&NU);
            for (Int32 i = 0; i < NU; i++) {
                AutoPtr<IInterface> obj;
                mOtherUserBackgroundItems->ValueAt(i, (IInterface**)&obj);
                MergedItem* user = (MergedItem*)IRunningStateMergedItem::Probe(obj);
                if (user->mCurSeq == mSequence) {
                    user->Update(context, TRUE);
                    user->UpdateSize(context);
                }
            }
        }
    }

    mMergedItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mMergedItems->Get(i, (IInterface**)&obj);
        ((MergedItem*)IRunningStateMergedItem::Probe(obj))->UpdateSize(context);
    }

    {
        AutoLock syncLock(mLock);
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

AutoPtr<IArrayList> /*ArrayList<BaseItem>*/ RunningState::GetCurrentItems()
{
    {
        AutoLock syncLock(mLock);
        return mItems;
    }
}

void RunningState::SetWatchingBackgroundItems(
    /* [in] */ Boolean watching)
{
    {
        AutoLock syncLock(mLock);
        mWatchingBackgroundItems = watching;
    }
}

AutoPtr<IArrayList> /*ArrayList<MergedItem>*/ RunningState::GetCurrentMergedItems()
{
    {
        AutoLock syncLock(mLock);
        return mMergedItems;
    }
}

AutoPtr<IArrayList> /*ArrayList<MergedItem>*/ RunningState::GetCurrentBackgroundItems()
{
    {
        AutoLock syncLock(mLock);
        return mUserBackgroundItems;
    }
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos