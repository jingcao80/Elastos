#include "elastos/droid/app/LocalActivityManager.h"
#include "elastos/droid/app/CLocalActivityManager.h"
#include "elastos/droid/app/CLocalActivityRecord.h"
#include "elastos/droid/app/CActivityNonConfigurationInstances.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//==========================================================
// LocalActivityManager::LocalActivityRecord
//==========================================================
CAR_INTERFACE_IMPL_2(LocalActivityManager::LocalActivityRecord, Object, ILocalActivityRecord, IBinder)

LocalActivityManager::LocalActivityRecord::LocalActivityRecord()
    : mCurState(LocalActivityManager::RESTORED)
{
}

LocalActivityManager::LocalActivityRecord::~LocalActivityRecord()
{}

ECode LocalActivityManager::LocalActivityRecord::constructor(
    /* [in] */ const String& _id,
    /* [in] */ IIntent* _intent)
{
    mId = _id;
    mIntent = _intent;
    return NOERROR;
}

ECode LocalActivityManager::LocalActivityRecord::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

//==========================================================
// LocalActivityManager
//==========================================================

const Int32 LocalActivityManager::RESTORED = 0;      // State restored, but no StartActivity().
const Int32 LocalActivityManager::INITIALIZING = 1;  // Ready to launch (after StartActivity()).
const Int32 LocalActivityManager::CREATED = 2;       // Created, not started or resumed.
const Int32 LocalActivityManager::STARTED = 3;       // Created and started, not resumed.
const Int32 LocalActivityManager::RESUMED = 4;       // Created started and resumed.
const Int32 LocalActivityManager::DESTROYED = 5;     // No longer with us.

const String LocalActivityManager::TAG("LocalActivityManager");
const Boolean LocalActivityManager::localLOGV = FALSE;

CAR_INTERFACE_IMPL(LocalActivityManager, Object, ILocalActivityManager)

LocalActivityManager::LocalActivityManager()
    : mSingleMode(FALSE)
    , mFinishing(FALSE)
    , mCurState(INITIALIZING)
{}

LocalActivityManager::~LocalActivityManager()
{}

ECode LocalActivityManager::constructor(
    /* [in] */ IActivity* parent,
    /* [in] */ Boolean singleMode)
{
    mActivityThread = CActivityThread::GetCurrentActivityThread();
    mSingleMode = singleMode;

    CHashMap::New((IMap**)&mActivities);
    CArrayList::New((IArrayList**)&mActivityArray);

    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(parent);
    wrs->GetWeakReference((IWeakReference**)&mWeakParent);
    return NOERROR;
}

ECode LocalActivityManager::MoveToState(
    /* [in] */ ILocalActivityRecord* record,
    /* [in] */ Int32 desiredState)
{
    AutoPtr<IActivity> parent;
    mWeakParent->Resolve(EIID_IActivity, (IInterface**)&parent);
    if (parent == NULL) {
        return NOERROR;
    }

    LocalActivityRecord* r = (LocalActivityRecord*)record;
    if (r->mCurState == RESTORED || r->mCurState == DESTROYED) {
        // StartActivity() has not yet been called, so nothing to do.
        return NOERROR;
    }

    if (r->mCurState == INITIALIZING) {
        // Get the lastNonConfigurationInstance for the activity
        AutoPtr<IHashMap> lastNonConfigurationInstances;
        parent->GetLastNonConfigurationChildInstances((IHashMap**)&lastNonConfigurationInstances);
        AutoPtr<IInterface> instanceObj;
        if (lastNonConfigurationInstances != NULL) {
            AutoPtr<ICharSequence> seq = CoreUtils::Convert(r->mId);
            lastNonConfigurationInstances->Get(seq.Get(), (IInterface**)&instanceObj);
        }
        AutoPtr<IActivityNonConfigurationInstances> instance;
        if (instanceObj != NULL) {
            CActivityNonConfigurationInstances::New((IActivityNonConfigurationInstances**)&instance);
            instance->SetActivity(instanceObj);
        }

        // We need to have always created the activity.
        if (localLOGV) Logger::V(TAG, "%s: starting %s",
            r->mId.string(), TO_CSTR(r->mIntent));

        if (r->mActivityInfo == NULL) {
            mActivityThread->ResolveActivityInfo(r->mIntent, (IActivityInfo**)&r->mActivityInfo);
        }
        mActivityThread->StartActivityNow(
            parent, r->mId, r->mIntent, r->mActivityInfo, r, r->mInstanceState, instance,
            (IActivity**)&r->mActivity);
        if (r->mActivity == NULL) {
            return NOERROR;
        }
        r->mActivity->GetWindow((IWindow**)&r->mWindow);
        r->mInstanceState = NULL;
        r->mCurState = STARTED;

        if (desiredState == RESUMED) {
            if (localLOGV) Logger::V(TAG, "%s: resuming", r->mId.string());
            AutoPtr<IActivityClientRecord> tmp;
            mActivityThread->PerformResumeActivity(IBinder::Probe(record), TRUE, (IActivityClientRecord**)&tmp);
            r->mCurState = RESUMED;
        }

        // Don't do anything more here.  There is an important case:
        // if this is being done as part of onCreate() of the group, then
        // the launching of the activity gets its state a little ahead
        // of our own (it is now STARTED, while we are only CREATED).
        // If we just leave things as-is, we'll deal with it as the
        // group's state catches up.
        return NOERROR;
    }

    switch (r->mCurState) {
        case CREATED:
            if (desiredState == STARTED) {
                if (localLOGV) Logger::V(TAG, "%s: restarting", r->mId.string());
                mActivityThread->PerformRestartActivity(IBinder::Probe(record));
                r->mCurState = STARTED;
            }
            if (desiredState == RESUMED) {
                if (localLOGV) Logger::V(TAG, "%s : restarting and resuming", r->mId.string());
                mActivityThread->PerformRestartActivity(IBinder::Probe(record));
                AutoPtr<IActivityClientRecord> tmp;
                mActivityThread->PerformResumeActivity(IBinder::Probe(record), TRUE, (IActivityClientRecord**)&tmp);
                r->mCurState = RESUMED;
            }
            return NOERROR;

        case STARTED:
            if (desiredState == RESUMED) {
                // Need to resume it...
                if (localLOGV) Logger::V(TAG, "%s : resuming", r->mId.string());
                AutoPtr<IActivityClientRecord> tmp;
                mActivityThread->PerformResumeActivity(IBinder::Probe(record), TRUE, (IActivityClientRecord**)&tmp);
                r->mInstanceState = NULL;
                r->mCurState = RESUMED;
            }
            if (desiredState == CREATED) {
                if (localLOGV) Logger::V(TAG, "%s : stopping", r->mId.string());
                mActivityThread->PerformStopActivity(IBinder::Probe(record), FALSE);
                r->mCurState = CREATED;
            }
            return NOERROR;

        case RESUMED:
            if (desiredState == STARTED) {
                if (localLOGV) Logger::V(TAG, "%s : pausing", r->mId.string());
                PerformPause(r, mFinishing);
                r->mCurState = STARTED;
            }
            if (desiredState == CREATED) {
                if (localLOGV) Logger::V(TAG, "%s : pausing", r->mId.string());
                PerformPause(r, mFinishing);
                if (localLOGV) Logger::V(TAG, "%s : stopping", r->mId.string());
                mActivityThread->PerformStopActivity(IBinder::Probe(record), FALSE);
                r->mCurState = CREATED;
            }
            return NOERROR;
    }
    return NOERROR;
}

ECode LocalActivityManager::PerformPause(
    /* [in] */ ILocalActivityRecord* record,
    /* [in] */ Boolean finishing)
{
    LocalActivityRecord* r = (LocalActivityRecord*)record;
    Boolean needState = r->mInstanceState == NULL;
    AutoPtr<IBundle> instanceState;
    mActivityThread->PerformPauseActivity(IBinder::Probe(record), finishing, needState, (IBundle**)&instanceState);
    if (needState) {
        r->mInstanceState = instanceState;
    }
    return NOERROR;
}

ECode LocalActivityManager::StartActivity(
    /* [in] */ const String& id,
    /* [in] */ IIntent* intent,
    /* [out] */ IWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = NULL;

    if (mCurState == INITIALIZING) {
        Logger::E(TAG, "IllegalStateException: Activities can't be added until the containing group has been created.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Boolean adding = FALSE;
    Boolean sameIntent = FALSE;

    AutoPtr<IActivityInfo> aInfo;
    AutoPtr<ICharSequence> idObj = CoreUtils::Convert(id);
    // Already have information about the new activity id?
    AutoPtr<IInterface> obj;
    mActivities->Get(obj, (IInterface**)&obj);
    ILocalActivityRecord* lar = ILocalActivityRecord::Probe(obj);
    AutoPtr<LocalActivityRecord> r = (LocalActivityRecord*)lar;
    if (lar == NULL) {
        // Need to create it...
        AutoPtr<ILocalActivityRecord> record;
        CLocalActivityRecord::New(id, intent, (ILocalActivityRecord**)&record);
        r = (LocalActivityRecord*)record.Get();
        adding = TRUE;
    }
    else if (r->mIntent != NULL) {
        r->mIntent->FilterEquals(intent, &sameIntent);
        if (sameIntent) {
            // We are starting the same activity.
            aInfo = r->mActivityInfo;
        }
    }
    if (aInfo == NULL) {
        mActivityThread->ResolveActivityInfo(intent, (IActivityInfo**)&aInfo);
    }

    // Pause the currently running activity if there is one and only a single
    // activity is allowed to be running at a time.
    if (mSingleMode) {
        AutoPtr<ILocalActivityRecord> old = mResumed;

        // If there was a previous activity, and it is not the current
        // activity, we need to stop it.
        if (old != NULL && old.Get() != ILocalActivityRecord::Probe(obj) && mCurState == RESUMED) {
            MoveToState(old, STARTED);
        }
    }

    if (adding) {
        // It's a brand new world.
        mActivities->Put(idObj.Get(), TO_IINTERFACE(r));
        mActivityArray->Add(TO_IINTERFACE(r));
    }
    else if (r->mActivityInfo != NULL) {
        // If the new activity is the same as the current one, then
        // we may be able to reuse it.
        String n1, n2, p1, p2;
        IPackageItemInfo::Probe(aInfo)->GetName(&n1);
        IPackageItemInfo::Probe(r->mActivityInfo)->GetName(&n2);
        IPackageItemInfo::Probe(aInfo)->GetPackageName(&p1);
        IPackageItemInfo::Probe(r->mActivityInfo)->GetPackageName(&p2);
        Int32 mode, flags;
        aInfo->GetLaunchMode(&mode);
        aInfo->GetFlags(&flags);

        if (aInfo == r->mActivityInfo || (n1.Equals(n2) && p1.Equals(p2))) {
            if (mode != IActivityInfo::LAUNCH_MULTIPLE
                || (flags & IIntent::FLAG_ACTIVITY_SINGLE_TOP) != 0) {
                // The activity wants onNewIntent() called.
                AutoPtr<IArrayList> intents;
                CArrayList::New((IArrayList**)&intents);
                intents->Add(intent);
                if (localLOGV) Logger::V(TAG, "%s : new intent", r->mId.string());
                mActivityThread->PerformNewIntents(IBinder::Probe(obj), IList::Probe(intents));
                r->mIntent = intent;
                MoveToState(r, mCurState);
                if (mSingleMode) {
                    mResumed = r;
                }
                *window = r->mWindow;
                REFCOUNT_ADD(*window)
                return NOERROR;
            }

            aInfo->GetFlags(&flags);
            if (sameIntent && (flags & IIntent::FLAG_ACTIVITY_CLEAR_TOP) == 0) {
                // We are showing the same thing, so this activity is
                // just resumed and stays as-is.
                r->mIntent = intent;
                MoveToState(r, mCurState);
                if (mSingleMode) {
                    mResumed = r;
                }

                *window = r->mWindow;
                REFCOUNT_ADD(*window)
                return NOERROR;
            }
        }

        // The new activity is different than the current one, or it
        // is a multiple launch activity, so we need to destroy what
        // is currently there.
        PerformDestroy(r, TRUE);
    }

    r->mIntent = intent;
    r->mCurState = INITIALIZING;
    r->mActivityInfo = aInfo;

    MoveToState(r, mCurState);

    // When in single mode keep track of the current activity
    if (mSingleMode) {
        mResumed = r;
    }
    *window = r->mWindow;
    REFCOUNT_ADD(*window)
    return NOERROR;
}

AutoPtr<IWindow> LocalActivityManager::PerformDestroy(
    /* [in] */ ILocalActivityRecord* record,
    /* [in] */ Boolean finish)
{
    LocalActivityRecord* r = (LocalActivityRecord*)record;
    AutoPtr<IWindow> win = r->mWindow;
    if (r->mCurState == RESUMED && !finish) {
        PerformPause(r, finish);
    }
    if (localLOGV) Logger::V(TAG, "%s : destroying", r->mId.string());
    AutoPtr<IActivityClientRecord> tmp;
    mActivityThread->PerformDestroyActivity(IBinder::Probe(record), finish, (IActivityClientRecord**)&tmp);
    r->mActivity = NULL;
    r->mWindow = NULL;
    if (finish) {
        r->mInstanceState = NULL;
    }
    r->mCurState = DESTROYED;
    return win;
}

ECode LocalActivityManager::DestroyActivity(
    /* [in] */ const String& id,
    /* [in] */ Boolean finish,
    /* [out] */ IWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = NULL;

    AutoPtr<ICharSequence> idObj = CoreUtils::Convert(id);
    AutoPtr<IInterface> obj;
    mActivities->Get(idObj, (IInterface**)&obj);
    LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);
    AutoPtr<IWindow> win;
    if (r != NULL) {
        win = PerformDestroy(r, finish);
        if (finish) {
            mActivities->Remove(idObj);
            mActivityArray->Remove(obj);
        }
    }

    *window = win;
    REFCOUNT_ADD(*window)
    return NOERROR;
}

ECode LocalActivityManager::GetCurrentActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;
    if (mResumed != NULL) {
        *activity = ((LocalActivityRecord*)mResumed.Get())->mActivity;
        REFCOUNT_ADD(*activity)
    }
    return NOERROR;
}

ECode LocalActivityManager::GetCurrentId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)
    *id = NULL;
    if (mResumed != NULL) {
        *id = ((LocalActivityRecord*)mResumed.Get())->mId;
    }
    return NOERROR;
}

ECode LocalActivityManager::GetActivity(
    /* [in] */ const String& id,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;

    AutoPtr<ICharSequence> idObj = CoreUtils::Convert(id);
    AutoPtr<IInterface> obj;
    mActivities->Get(idObj, (IInterface**)&obj);

    if (obj != NULL) {
        LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);
        *activity = r->mActivity;
        REFCOUNT_ADD(*activity)
    }
    return NOERROR;
}

ECode LocalActivityManager::DispatchCreate(
    /* [in] */ IBundle* state)
{
    if (state != NULL) {
        AutoPtr<ISet> keyset;
        state->GetKeySet((ISet**)&keyset);
        AutoPtr<IIterator> it;
        keyset->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> idObj;
            it->GetNext((IInterface**)&idObj);
            String id = Object::ToString(idObj);
            // try {
                AutoPtr<IBundle> astate;
                state->GetBundle(id, (IBundle**)&astate);

                AutoPtr<IInterface> obj;
                mActivities->Get(idObj, (IInterface**)&obj);
                LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);
                if (r != NULL) {
                    r->mInstanceState = astate;
                }
                else {
                    AutoPtr<ILocalActivityRecord> record;
                    CLocalActivityRecord::New(id, NULL, (ILocalActivityRecord**)&record);
                    r = (LocalActivityRecord*)record.Get();
                    r->mInstanceState = astate;
                    mActivities->Put(idObj, record.Get());
                    mActivityArray->Add(idObj);
                }
            // } catch (Exception e) {
            //     // Recover from -all- app errors.
            //     Logger::E(TAG, "Exception thrown when restoring LocalActivityManager state", e);
            // }
        }
    }

    mCurState = CREATED;
    return NOERROR;
}

ECode LocalActivityManager::SaveInstanceState(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;

    AutoPtr<IBundle> state;

    // FIXME: child activities will freeze as part of onPaused. Do we
    // need to do this here?
    Int32 N;
    mActivityArray->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActivityArray->Get(i, (IInterface**)&obj);
        LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);
        if (state == NULL) {
            CBundle::New((IBundle**)&state);
        }
        if ((r->mInstanceState != NULL || r->mCurState == RESUMED)
                && r->mActivity != NULL) {
            // We need to save the state now, if we don't currently
            // already have it or the activity is currently resumed.
            AutoPtr<IBundle> childState;
            CBundle::New((IBundle**)&childState);
            r->mActivity->PerformSaveInstanceState(childState);
            r->mInstanceState = childState;
        }
        if (r->mInstanceState != NULL) {
            state->PutBundle(r->mId, r->mInstanceState);
        }
    }

    *bundle = state;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode LocalActivityManager::DispatchResume()
{
    mCurState = RESUMED;
    if (mSingleMode) {
        if (mResumed != NULL) {
            MoveToState(mResumed, RESUMED);
        }
    }
    else {
        Int32 N;
        mActivityArray->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mActivityArray->Get(i, (IInterface**)&obj);
            MoveToState(ILocalActivityRecord::Probe(obj), RESUMED);
        }
    }
    return NOERROR;
}

ECode LocalActivityManager::DispatchPause(
    /* [in] */ Boolean finishing)
{
    if (finishing) {
        mFinishing = TRUE;
    }
    mCurState = STARTED;
    if (mSingleMode) {
        if (mResumed != NULL) {
            MoveToState(mResumed, STARTED);
        }
    }
    else {
        Int32 N;
        mActivityArray->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mActivityArray->Get(i, (IInterface**)&obj);
            ILocalActivityRecord* record = ILocalActivityRecord::Probe(obj);
            LocalActivityRecord* r = (LocalActivityRecord*)record;
            MoveToState(ILocalActivityRecord::Probe(obj), RESUMED);
            if (r->mCurState == RESUMED) {
                MoveToState(record, STARTED);
            }
        }
    }
    return NOERROR;
}

ECode LocalActivityManager::DispatchStop()
{
    mCurState = CREATED;
    Int32 N;
    mActivityArray->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActivityArray->Get(i, (IInterface**)&obj);
        MoveToState(ILocalActivityRecord::Probe(obj), CREATED);
    }
    return NOERROR;
}

ECode LocalActivityManager::DispatchRetainNonConfigurationInstance(
    /* [out] */ IHashMap** map)
{
    VALIDATE_NOT_NULL(map)
    *map = NULL;

    AutoPtr<IHashMap> instanceMap;

    Int32 N;
    mActivityArray->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActivityArray->Get(i, (IInterface**)&obj);
        LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);

        if ((r != NULL) && (r->mActivity != NULL)) {
            AutoPtr<IInterface> instance;
            r->mActivity->OnRetainNonConfigurationInstance((IInterface**)&instance);
            if (instance != NULL) {
                if (instanceMap == NULL) {
                    CHashMap::New((IHashMap**)&instanceMap);
                }
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(r->mId);
                instanceMap->Put(csq, instance.Get());
            }
        }
    }

    *map = instanceMap;
    REFCOUNT_ADD(*map)
    return NOERROR;
}

ECode LocalActivityManager::RemoveAllActivities()
{
    return DispatchDestroy(TRUE);
}

ECode LocalActivityManager::DispatchDestroy(
    /* [in] */ Boolean finishing)
{
    Int32 N;
    mActivityArray->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActivityArray->Get(i, (IInterface**)&obj);
        LocalActivityRecord* r = (LocalActivityRecord*)ILocalActivityRecord::Probe(obj);

        if (localLOGV) Logger::V(TAG, "%s : destroying", r->mId.string());
        AutoPtr<IActivityClientRecord> tmp;
        mActivityThread->PerformDestroyActivity(IBinder::Probe(obj), finishing, (IActivityClientRecord**)&tmp);
    }
    mActivities->Clear();
    mActivityArray->Clear();
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

