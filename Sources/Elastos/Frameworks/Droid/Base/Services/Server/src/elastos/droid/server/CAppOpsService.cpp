
#include "elastos/droid/server/CAppOpsService.h"
#include "elastos/droid/app/AppOpsManager.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/droid/internal/utility/XmlUtils.h>

#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::App::AppOpsManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::App::IAppOpsManagerOpEntry;
using Elastos::Droid::App::CAppOpsManagerOpEntry;
using Elastos::Droid::App::IAppOpsManagerPackageOps;
using Elastos::Droid::App::CAppOpsManagerPackageOps;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Internal::App::EIID_IIAppOpsService;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CArrayOf;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

//================================================================================
// CAppOpsService::WriteStateTask
//================================================================================
CAppOpsService::WriteStateTask::WriteStateTask(
    /* [in] */ CAppOpsService* host)
    : mHost(host)
{}

//@Override
ECode CAppOpsService::WriteStateTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    mHost->WriteState();
    return NOERROR;
}

//================================================================================
// CAppOpsService::WriteRunner
//================================================================================

CAppOpsService::WriteRunner::WriteRunner(
    /* [in] */ CAppOpsService* host)
    : mHost(host)
{}

ECode CAppOpsService::WriteRunner::Run()
{
    synchronized(mHost) {
        mHost->mWriteScheduled = FALSE;
        AutoPtr<AsyncTask> task = (AsyncTask*)new WriteStateTask(mHost);
        return task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
    }
    return NOERROR;
}

//================================================================================
// CAppOpsService::Op
//================================================================================
CAppOpsService::Op::Op(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 op,
    /* [in] */ Int32 mode)
    : mUid(uid)
    , mPackageName(packageName)
    , mOp(op)
    , mMode(mode)
    , mDuration(0)
    , mTime(0)
    , mRejectTime(0)
    , mNesting(0)
{
}

//================================================================================
// CAppOpsService::Ops
//================================================================================

CAppOpsService::Ops::Ops()
    : mUid(0)
    , mIsPrivileged(FALSE)
{}

ECode CAppOpsService::Ops::constructor(
    /* [in] */ const String& _packageName,
    /* [in] */ Int32 _uid,
    /* [in] */ Boolean _isPrivileged)
{
    SparseArray::constructor();

    mPackageName = _packageName;
    mUid = _uid;
    mIsPrivileged = _isPrivileged;
    return NOERROR;
}

//================================================================================
// CAppOpsService::Callback
//================================================================================

CAR_INTERFACE_IMPL(CAppOpsService::Callback, Object, IProxyDeathRecipient)

CAppOpsService::Callback::Callback(
    /* [in] */ IIAppOpsCallback* callback,
    /* [in] */ CAppOpsService* host)
    : mCallback(callback)
    , mHost(host)
{
    mCallback = callback;

    AutoPtr<IProxy> proxy = (IProxy*)mCallback->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(this, 0);
    }
}

ECode CAppOpsService::Callback::UnlinkToDeath()
{
    AutoPtr<IProxy> proxy = (IProxy*)mCallback->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean bval;
        proxy->UnlinkToDeath(this, 0, &bval);
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::Callback::ProxyDied()
{
    mHost->StopWatchingMode(mCallback);
    return NOERROR;
}

ECode CAppOpsService::Callback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CAppOpsService::Callback";
    return NOERROR;
}

//================================================================================
// CAppOpsService::ClientState
//================================================================================

CAR_INTERFACE_IMPL_2(CAppOpsService::ClientState, Object, IProxyDeathRecipient, IBinder)

CAppOpsService::ClientState::ClientState(
    /* [in] */ IBinder* appToken,
    /* [in] */ CAppOpsService* host)
    : mAppToken(appToken)
    , mHost(host)
{
    mPid = Binder::GetCallingPid();

    AutoPtr<IProxy> proxy = (IProxy*)appToken->Probe(EIID_IProxy);
    if (proxy != NULL) {
        CArrayList::New((IArrayList**)&mStartedOps);
        proxy->LinkToDeath(this, 0);
    }
    else {
        // For local clients, there is no reason to track them.
        mStartedOps = NULL;
    }
}

//@Override
ECode CAppOpsService::ClientState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("ClientState{");
    sb += Object::ToString(mAppToken);
    sb += ",";
    if (mStartedOps != NULL) {
        sb += mPid;
        sb += "}";
    }
    else {
        sb += "local}";
    }
    *str = sb.ToString();
    return NOERROR;
}

//@Override
ECode CAppOpsService::ClientState::ProxyDied()
{
    synchronized(mHost) {
        Int32 size;
        mStartedOps->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mStartedOps->Get(i, (IInterface**)&obj);
            Op* op = (Op*)IObject::Probe(obj);
            mHost->FinishOperationLocked(op);
        }
        mHost->mClients->Remove(mAppToken.Get());
    }
    return NOERROR;
}

//================================================================================
// CAppOpsService::Restriction
//================================================================================
static AutoPtr<IArraySet> InitNO_EXCEPTIONS()
{
    AutoPtr<IArraySet> set;
    CArraySet::New((IArraySet**)&set);
    return set;
}
const AutoPtr<IArraySet> CAppOpsService::Restriction::NO_EXCEPTIONS = InitNO_EXCEPTIONS();

CAppOpsService::Restriction::Restriction()
    : mMode(0)
{
    mExceptionPackages = NO_EXCEPTIONS;
}

//================================================================================
// CAppOpsService
//================================================================================

const String CAppOpsService::TAG("CAppOpsService");
const Boolean CAppOpsService::DEBUG = FALSE;

// Write at most every 30 minutes.
const Int64 CAppOpsService::WRITE_DELAY = DEBUG ? 1000 : 30*60*1000;

static AutoPtr<ArrayOf<Int32> > InitPRIVACY_GUARD_OP_STATES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(5);

    array->Set(0, IAppOpsManager::OP_COARSE_LOCATION);
    array->Set(1, IAppOpsManager::OP_READ_CALL_LOG);
    array->Set(2, IAppOpsManager::OP_READ_CONTACTS);
    array->Set(3, IAppOpsManager::OP_READ_CALENDAR);
    array->Set(4, IAppOpsManager::OP_READ_SMS);

    return array;
}

AutoPtr<ArrayOf<Int32> > CAppOpsService::PRIVACY_GUARD_OP_STATES = InitPRIVACY_GUARD_OP_STATES();

CAR_INTERFACE_IMPL_2(CAppOpsService, Object, IIAppOpsService, IBinder)

CAR_OBJECT_IMPL(CAppOpsService)

CAppOpsService::CAppOpsService()
    : mWriteScheduled(FALSE)
{
}

ECode CAppOpsService::constructor(
    /* [in] */ IFile* storagePath,
    /* [in] */ IHandler* handler)
{
    mWriteRunner = new WriteRunner(this);

    CSparseArray::New((ISparseArray**)&mUidOps);
    CSparseArray::New((ISparseArray**)&mOpRestrictions);
    CSparseArray::New((ISparseArray**)&mOpModeWatchers);
    CArrayMap::New((IArrayMap**)&mPackageModeWatchers);
    CArrayMap::New((IArrayMap**)&mModeWatchers);
    CSparseArray::New((ISparseArray**)&mAudioRestrictions);
    CArrayMap::New((IArrayMap**)&mClients);

    ECode ec = CAtomicFile::New(storagePath, (IAtomicFile**)&mFile);
    if (FAILED(ec)) {
        Slogger::E(TAG, "constructor: failed to create CAtomicFile with path [%s], ec = %08x", TO_CSTR(storagePath), ec);
        assert(mFile != NULL);
        return ec;
    }
    assert(mFile != NULL);
    FAIL_RETURN(ec);

    mHandler = handler;
    ReadState();
    return NOERROR;
}

ECode CAppOpsService::Publish(
    /* [in] */ IContext* context)
{
    mContext = context;
    return ServiceManager::AddService(IContext::APP_OPS_SERVICE, TO_IINTERFACE(this));
}

ECode CAppOpsService::SystemReady()
{
    ECode ec = NOERROR;
    synchronized(this) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        Boolean changed = FALSE;
        Boolean hasNext;
        Int32 curUid;
        Int32 size, pkgsSize;
        mUidOps->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mUidOps->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IHashMap> pkgs = IHashMap::Probe(obj); //HashMap><String, Ops> ;
            AutoPtr<ICollection> values;
            pkgs->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                obj = NULL;
                it->GetNext((IInterface**)&obj);
                AutoPtr<Ops> ops = (Ops*)IObject::Probe(obj);

                ec = pm->GetPackageUid(ops->mPackageName, UserHandle::GetUserId(ops->mUid), &curUid);
                if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                    curUid = -1;
                }

                if (curUid != ops->mUid) {
                    Slogger::I(TAG, "Pruning old package %s/%d: new uid=%d",
                        ops->mPackageName.string(), ops->mUid, curUid);
                    it->Remove();
                    changed = TRUE;
                }
            }

            pkgs->GetSize(&pkgsSize);
            if (pkgsSize <= 0) {
                mUidOps->RemoveAt(i);
            }
        }
        if (changed) {
            ScheduleWriteLocked();
        }
    }
    return NOERROR;
}

ECode CAppOpsService::PackageRemoved(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mUidOps->Get(uid, (IInterface**)&obj);
        if (obj != NULL) {
            AutoPtr<IHashMap> pkgs = IHashMap::Probe(obj); //HashMap><String, Ops> ;
            AutoPtr<ICharSequence> seq = CoreUtils::Convert(packageName);
            obj = NULL;
            pkgs->Remove(seq.Get(), (IInterface**)&obj);
            if (obj != NULL) {
                Int32 size;
                pkgs->GetSize(&size);
                if (size <= 0) {
                    mUidOps->Remove(uid);
                }
                ScheduleWriteLocked();
            }
        }
    }
    return NOERROR;
}

ECode CAppOpsService::UidRemoved(
    /* [in] */ Int32 uid)
{
    synchronized(this) {
        Int32 index;
        mUidOps->IndexOfKey(uid, &index);
        if (index >= 0) {
            mUidOps->Remove(uid);
            ScheduleWriteLocked();
        }
    }
    return NOERROR;
}

ECode CAppOpsService::Shutdown()
{
    Slogger::W(TAG, "Writing app ops before shutdown...");
    Boolean doWrite = FALSE;
    synchronized(this) {
        if (mWriteScheduled) {
            mWriteScheduled = FALSE;
            doWrite = TRUE;
        }
    }
    if (doWrite) {
        WriteState();
    }
    return NOERROR;
}

AutoPtr<IArrayList> CAppOpsService::CollectOps( //ArrayList><IAppOpsManager::OpEntry>
    /* [in] */ CAppOpsService::Ops* pkgOps,
    /* [in] */ ArrayOf<Int32>* ops)
{
    AutoPtr<IArrayList> resOps;//
    if (ops == NULL) {
        CArrayList::New((IArrayList**)&resOps);
        Int32 size;
        pkgOps->GetSize(&size);
        for (Int32 j = 0; j < size; j++) {
            AutoPtr<IInterface> obj;
            pkgOps->ValueAt(j, (IInterface**)&obj);
            Op* curOp = (Op*)IObject::Probe(obj);
            AutoPtr<IAppOpsManagerOpEntry> oe;
            CAppOpsManagerOpEntry::New(curOp->mOp, curOp->mMode, curOp->mTime,
                curOp->mRejectTime, curOp->mDuration, curOp->mAllowedCount,
                curOp->mIgnoredCount, (IAppOpsManagerOpEntry**)&oe);
            resOps->Add(oe.Get());
        }
    }
    else {
        for (Int32 j = 0; j < ops->GetLength(); j++) {
            AutoPtr<IInterface> obj;
            pkgOps->Get((*ops)[j], (IInterface**)&obj);
            if (obj != NULL) {
                Op* curOp = (Op*)IObject::Probe(obj);
                if (resOps == NULL) {
                    CArrayList::New((IArrayList**)&resOps);
                }
                AutoPtr<IAppOpsManagerOpEntry> oe;
                CAppOpsManagerOpEntry::New(curOp->mOp, curOp->mMode, curOp->mTime,
                    curOp->mRejectTime, curOp->mDuration, curOp->mAllowedCount,
                    curOp->mIgnoredCount, (IAppOpsManagerOpEntry**)&oe);
                resOps->Add(oe.Get());
            }
        }
    }
    return resOps;
}

//@Override
ECode CAppOpsService::GetPackagesForOps(
    /* [in] */ ArrayOf<Int32>* ops,
    /* [out] */ IList** result) // List<IAppOpsManager::PackageOps>
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::GET_APP_OPS_STATS,
        Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL)))

    AutoPtr<IArrayList> res;
    synchronized(this) {
        Boolean hasNext;
        Int32 size;
        mUidOps->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mUidOps->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IHashMap> packages = IHashMap::Probe(obj); //HashMap><String, Ops> ;

            AutoPtr<ICollection> values;
            packages->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            while (it->HasNext(&hasNext), hasNext) {
                obj = NULL;
                it->GetNext((IInterface**)&obj);
                AutoPtr<Ops> pkgOps = (Ops*)IObject::Probe(obj);

                AutoPtr<IArrayList> resOps = CollectOps(pkgOps, ops); //<IAppOpsManager::OpEntry>
                if (resOps != NULL) {
                    if (res == NULL) {
                        CArrayList::New((IArrayList**)&res);
                    }

                    AutoPtr<IAppOpsManagerPackageOps> resPackage;
                    CAppOpsManagerPackageOps::New(
                        pkgOps->mPackageName, pkgOps->mUid, IList::Probe(resOps),
                        (IAppOpsManagerPackageOps**)&resPackage);
                    res->Add(resPackage.Get());
                }
            }
        }
    }

    *result = IList::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//@Override
ECode CAppOpsService::GetOpsForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Int32>* ops,
    /* [out] */ IList** result) // List<IAppOpsManager::PackageOps>
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::GET_APP_OPS_STATS,
        Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL)))

    synchronized(this) {
        AutoPtr<Ops> pkgOps = GetOpsLocked(uid, packageName, FALSE);
        if (pkgOps == NULL) {
            return NOERROR;
        }
        AutoPtr<IArrayList> resOps = CollectOps(pkgOps, ops);
        if (resOps == NULL) {
            return NOERROR;
        }

        AutoPtr<IArrayList> res;
        CArrayList::New((IArrayList**)&res);

        AutoPtr<IAppOpsManagerPackageOps> resPackage;
        CAppOpsManagerPackageOps::New(
            pkgOps->mPackageName, pkgOps->mUid, IList::Probe(resOps),
            (IAppOpsManagerPackageOps**)&resPackage);
        res->Add(resPackage.Get());
        *result = IList::Probe(res);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

void CAppOpsService::PruneOp(
    /* [in] */ CAppOpsService::Op* op,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (op->mTime == 0 && op->mRejectTime == 0) {
        AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, FALSE);
        if (ops != NULL) {
            ops->Remove(op->mOp);
            Int32 size;
            ops->GetSize(&size);
            if (size <= 0) {
                AutoPtr<IInterface> obj;
                mUidOps->Get(uid, (IInterface**)&obj);
                if (obj != NULL) {
                    AutoPtr<IHashMap> pkgOps = IHashMap::Probe(obj);
                    AutoPtr<ICharSequence> csq = CoreUtils::Convert(ops->mPackageName);
                    pkgOps->Remove(csq.Get());
                    pkgOps->GetSize(&size);
                    if (size <= 0) {
                        mUidOps->Remove(uid);
                    }
                }
            }
        }
    }
}

//@Override
ECode CAppOpsService::SetMode(
    /* [in] */ Int32 inCode,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    if (Binder::GetCallingPid() != Process::MyPid()) {
        FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::UPDATE_APP_OPS_STATS,
                Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL)))
    }
    Int32 code = inCode;
    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
    FAIL_RETURN(VerifyIncomingOp(code))
    AutoPtr<IArrayList> repCbs; //ArrayList<Callback>
    aom->OpToSwitch(inCode, &code);
    synchronized(this) {
        AutoPtr<Op> op = GetOpLocked(code, uid, packageName, TRUE);
        if (op != NULL) {
            if (op->mMode != mode) {
                op->mMode = mode;

                AutoPtr<IInterface> obj;
                mOpModeWatchers->Get(code, (IInterface**)&obj);
                if (obj != NULL) {
                    if (repCbs == NULL) {
                        CArrayList::New((IArrayList**)&repCbs);
                    }
                    repCbs->AddAll(ICollection::Probe(obj));
                }

                obj = NULL;
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
                mPackageModeWatchers->Get(csq.Get(), (IInterface**)&obj);
                if (obj != NULL) {
                    if (repCbs == NULL) {
                        CArrayList::New((IArrayList**)&repCbs);
                    }
                    repCbs->AddAll(ICollection::Probe(obj));
                }

                // TODO: should be GetDefaultMode
                assert(0 && "TODO:");
                Int32 defaultMode = 0; // = GetDefaultMode(code, uid, packageName);
                if (mode == defaultMode) {
                    // If going into the default mode, prune this op
                    // if there is nothing else interesting in it.
                    PruneOp(op, uid, packageName);
                }
                ScheduleWriteNowLocked();
            }
        }
    }

    if (repCbs != NULL) {
        Int32 size;
        repCbs->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            repCbs->Get(i, (IInterface**)&obj);
            Callback* cb = (Callback*)IObject::Probe(obj);
            cb->mCallback->OpChanged(code, packageName);
        }
    }
    return NOERROR;
}

AutoPtr<IHashMap> CAppOpsService::AddCallbacks(
    /* [in] */ IHashMap* inCallbacks, //HashMap<Callback, ArrayList<Pair<String, Integer>>>
    /* [in] */ const String& packageName,
    /* [in] */ Int32 op,
    /* [in] */ IArrayList* cbs)  //ArrayList<Callback>
{
    AutoPtr<IHashMap> callbacks = inCallbacks;
    if (cbs == NULL) {
        return callbacks;
    }

    if (callbacks == NULL) {
        CHashMap::New((IHashMap**)&callbacks);
    }

    AutoPtr<ICharSequence> p1 = CoreUtils::Convert(packageName);
    AutoPtr<IInteger32> p2 = CoreUtils::Convert(op);
    IInterface* o1 = (IInterface*)p1.Get();
    IInterface* o2 = (IInterface*)p2.Get();

    Int32 size;
    cbs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj, valueObj;
        cbs->Get(i, (IInterface**)&obj);

        callbacks->Get(obj, (IInterface**)&valueObj);
        AutoPtr<IArrayList> reports = IArrayList::Probe(valueObj);
        if (reports == NULL) {
            CArrayList::New((IArrayList**)&reports);
            callbacks->Put(obj, reports.Get());
        }

        AutoPtr<IPair> pair;
        CPair::New(o1, o2, (IPair**)&pair);
        reports->Add(pair.Get());
    }
    return callbacks;
}

//@Override
ECode CAppOpsService::ResetAllModes()
{
    Int32 callingUid = Binder::GetCallingUid();
    FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::UPDATE_APP_OPS_STATS,
        Binder::GetCallingPid(), callingUid, String(NULL)))

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

    AutoPtr<IHashMap> callbacks;
    synchronized(this) {
        Boolean changed = FALSE;
        Int32 size, pkgOpsSize;
        mUidOps->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mUidOps->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IHashMap> packages = IHashMap::Probe(obj);
            Int32 key;
            mUidOps->KeyAt(i, &key);
            if (UserHandle::GetUserId(callingUid) != UserHandle::GetUserId(key)) {
                // Skip any ops for a different user
                continue;
            }

            AutoPtr<ISet> set;
            packages->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> meObj, keyObj, valueObj;
                it->GetNext((IInterface**)&meObj);
                AutoPtr<IMapEntry> ent = IMapEntry::Probe(meObj);
                ent->GetKey((IInterface**)&keyObj);
                ent->GetValue((IInterface**)&valueObj);

                String packageName = Object::ToString(keyObj);
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
                Ops* pkgOps = (Ops*)IObject::Probe(valueObj);
                pkgOps->GetSize(&pkgOpsSize);
                for (Int32 j = pkgOpsSize - 1; j >= 0; j--) {
                    AutoPtr<IInterface> opObj;
                    pkgOps->ValueAt(j, (IInterface**)&opObj);
                    Op* curOp = (Op*)IObject::Probe(opObj);

                    Boolean allow;
                    Int32 defaultMode = 0;
                    aom->OpAllowsReset(curOp->mOp, &allow);
                    if (allow) {
                        assert(0 && "TODO:");
                        // defaultMode = GetDefaultMode(curOp->mOp, curOp->mUid,
                        //         curOp->mPackageName);
                    }
                    if (allow && curOp->mMode != defaultMode) {
                        curOp->mMode = defaultMode;
                        changed = TRUE;
                        AutoPtr<IInterface> omwObj, pmwObj;
                        mOpModeWatchers->Get(curOp->mOp, (IInterface**)&omwObj);
                        mPackageModeWatchers->Get(csq.Get(), (IInterface**)&pmwObj);
                        callbacks = AddCallbacks(callbacks, packageName, curOp->mOp, IArrayList::Probe(omwObj));
                        callbacks = AddCallbacks(callbacks, packageName, curOp->mOp, IArrayList::Probe(pmwObj));
                        if (curOp->mTime == 0 && curOp->mRejectTime == 0) {
                            pkgOps->RemoveAt(j);
                        }
                    }
                }
                pkgOps->GetSize(&size);
                if (size == 0) {
                    it->Remove();
                }
            }
            packages->GetSize(&size);
            if (size == 0) {
                mUidOps->RemoveAt(i);
            }
        }
        if (changed) {
            ScheduleWriteNowLocked();
        }
    }

    if (callbacks != NULL) {
        Int32 size;
        AutoPtr<ISet> entrySet;
        callbacks->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> meObj, keyObj, valueObj;
            it->GetNext((IInterface**)&meObj);
            AutoPtr<IMapEntry> me = IMapEntry::Probe(meObj);
            me->GetKey((IInterface**)&keyObj);
            me->GetValue((IInterface**)&valueObj);
            Callback* cb = (Callback*)IObject::Probe(keyObj);
            AutoPtr<IArrayList> reports = IArrayList::Probe(valueObj); //ArrayList<Pair<String, Integer>>
            reports->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                reports->Get(i, (IInterface**)&obj);
                AutoPtr<IPair> rep = IPair::Probe(obj);
                AutoPtr<IInterface> first, second;
                rep->GetFirst((IInterface**)&first);
                rep->GetSecond((IInterface**)&second);
                Int32 ival;
                IInteger32::Probe(second)->GetValue(&ival);
                cb->mCallback->OpChanged(ival, Object::ToString(first));

            }
        }
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::StartWatchingMode(
    /* [in] */ Int32 inOp,
    /* [in] */ const String& packageName,
    /* [in] */ IIAppOpsCallback* callback)
{

    synchronized(this) {
        AutoPtr<IAppOpsManagerHelper> aom;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

        Int32 op = inOp;
        aom->OpToSwitch(inOp, &op);
        IBinder* binder = IBinder::Probe(callback);
        AutoPtr<IInterface> obj;
        mModeWatchers->Get(binder, (IInterface**)&obj);
        Callback* cb = (Callback*)IObject::Probe(obj);
        if (cb == NULL) {
            cb = new Callback(callback, this);
            mModeWatchers->Put(binder, TO_IINTERFACE(cb));
        }
        if (op != IAppOpsManager::OP_NONE) {
            AutoPtr<IInterface> omwObj;
            mOpModeWatchers->Get(op, (IInterface**)&omwObj);
            AutoPtr<IArrayList> cbs = IArrayList::Probe(omwObj);
            if (cbs == NULL) {
                CArrayList::New((IArrayList**)&cbs);
                mOpModeWatchers->Put(op, cbs.Get());
            }
            cbs->Add(TO_IINTERFACE(cb));
        }

        if (packageName != NULL) {
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
            AutoPtr<IInterface> pmwObj;
            mPackageModeWatchers->Get(csq.Get(), (IInterface**)&pmwObj);
            AutoPtr<IArrayList> cbs = IArrayList::Probe(pmwObj);
            if (cbs == NULL) {
                CArrayList::New((IArrayList**)&cbs);
                mPackageModeWatchers->Put(csq.Get(), cbs.Get());
            }
            cbs->Add(TO_IINTERFACE(cb));
        }
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::StopWatchingMode(
    /* [in] */ IIAppOpsCallback* callback)
{
    synchronized(this) {
        IBinder* binder = IBinder::Probe(callback);
        AutoPtr<IInterface> obj;
        mModeWatchers->Remove(binder, (IInterface**)&obj);
        if (obj != NULL) {
            AutoPtr<Callback> cb = (Callback*)IObject::Probe(obj);
            cb->UnlinkToDeath();

            Int32 size;
            mOpModeWatchers->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> omwObj;
                mOpModeWatchers->ValueAt(i, (IInterface**)&omwObj);
                AutoPtr<IArrayList> cbs = IArrayList::Probe(omwObj);
                cbs->Remove(obj);
                cbs->GetSize(&size);
                if (size <= 0) {
                    mOpModeWatchers->RemoveAt(i);
                }
            }
            for (Int32 i=mPackageModeWatchers->GetSize(&size)-1; i>=0; i--) {
                AutoPtr<IInterface> omwObj;
                mPackageModeWatchers->GetValueAt(i, (IInterface**)&omwObj);
                AutoPtr<IArrayList> cbs = IArrayList::Probe(omwObj);
                cbs->Remove(obj);
                cbs->GetSize(&size);
                if (size <= 0) {
                    mPackageModeWatchers->RemoveAt(i);
                }
            }
        }
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::GetToken(
    /* [in] */ IBinder* clientToken,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)
    *token = NULL;
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mClients->Get(clientToken, (IInterface**)&obj);
        AutoPtr<ClientState> cs = (ClientState*)IObject::Probe(obj);
        if (cs == NULL) {
            cs = new ClientState(clientToken, this);
            mClients->Put(clientToken, TO_IINTERFACE(cs));
        }
        *token = IBinder::Probe(cs);
        REFCOUNT_ADD(*token)
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::CheckOperation(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IAppOpsManager::MODE_ERRORED;

    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

    synchronized(this) {
        if (IsOpRestricted(uid, code, packageName)) {
            *result = IAppOpsManager::MODE_IGNORED;
            return NOERROR;
        }
        Int32 sw;
        aom->OpToSwitch(code, &sw);
        AutoPtr<Op> op = GetOpLocked(sw, uid, packageName, FALSE);
        if (op == NULL) {
            assert(0 && "TODO:");
            // *result = GetDefaultMode(code, uid, packageName)
            return NOERROR;
        }

        *result = op->mMode;
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::CheckAudioOperation(
    /* [in] */ Int32 code,
    /* [in] */ Int32 usage,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IAppOpsManager::MODE_ERRORED;
    synchronized(this) {
        Int32 mode = CheckRestrictionLocked(code, usage, uid, packageName);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            *result = mode;
            return NOERROR;
        }
    }
    return CheckOperation(code, uid, packageName, result);
}

Int32 CAppOpsService::CheckRestrictionLocked(
    /* [in] */ Int32 code,
    /* [in] */ Int32 usage,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    AutoPtr<IInterface> obj;
    mAudioRestrictions->Get(code, (IInterface**)&obj);
    if (obj != NULL) {
        AutoPtr<ISparseArray> usageRestrictions = ISparseArray::Probe(obj);
        obj = NULL;
        usageRestrictions->Get(usage, (IInterface**)&obj);
        Restriction* r = (Restriction*)IObject::Probe(obj);
        if (r != NULL) {
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
            Boolean contains;
            ISet::Probe(r->mExceptionPackages)->Contains(csq.Get(), &contains);
            if (!contains) {
                return r->mMode;
            }
        }
    }
    return IAppOpsManager::MODE_ALLOWED;
}

//@Override
ECode CAppOpsService::SetAudioRestriction(
    /* [in] */ Int32 code,
    /* [in] */ Int32 usage,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* exceptionPackages)
{
    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mAudioRestrictions->Get(code, (IInterface**)&obj);
        AutoPtr<ISparseArray> usageRestrictions = ISparseArray::Probe(obj);
        if (usageRestrictions != NULL) {
            CSparseArray::New((ISparseArray**)&usageRestrictions);
            mAudioRestrictions->Put(code, usageRestrictions.Get());
        }
        usageRestrictions->Remove(usage);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            AutoPtr<Restriction> r = new Restriction();
            r->mMode = mode;
            if (exceptionPackages != NULL) {
                Int32 N = exceptionPackages->GetLength();
                CArraySet::New((IArraySet**)&r->mExceptionPackages);
                ISet* set = ISet::Probe(r->mExceptionPackages);
                for (Int32 i = 0; i < N; i++) {
                    String pkg = (*exceptionPackages)[i];
                    if (pkg != NULL) {
                        AutoPtr<ICharSequence> csq = CoreUtils::Convert(pkg.Trim());
                        set->Add(csq.Get());
                    }
                }
            }
            usageRestrictions->Put(usage, TO_IINTERFACE(r));
        }
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::CheckPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = IAppOpsManager::MODE_ERRORED;

    synchronized(this) {
        if (GetOpsRawLocked(uid, packageName, TRUE) != NULL) {
            *mode = IAppOpsManager::MODE_ALLOWED;
        }
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::NoteOperation(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = IAppOpsManager::MODE_ERRORED;

    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
    synchronized(this) {
        AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, TRUE);
        if (ops == NULL) {
            if (DEBUG)  {
                Slogger::D(TAG, "noteOperation: no op for code %d uid %d package %s",
                    code, uid, packageName.string());
            }
            return NOERROR;
        }

        AutoPtr<Op> op = GetOpLocked(ops, code, TRUE);
        if (IsOpRestricted(uid, code, packageName)) {
            *mode = IAppOpsManager::MODE_IGNORED;
            return NOERROR;
        }
        if (op->mDuration == -1) {
            Slogger::W(TAG, "Noting op not finished: uid %d pkg %s code %d time=%lld duration=%d",
                uid, packageName.string(), code, op->mTime, op->mDuration);
        }

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        op->mDuration = 0;
        Int32 switchCode;
        aom->OpToSwitch(code, &switchCode);
        AutoPtr<Op> switchOp = switchCode != code ? GetOpLocked(ops, switchCode, TRUE) : op;
        if (switchOp->mMode != IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG) {
                Slogger::D(TAG, "noteOperation: reject #%d for code %d (%d) uid %d package %s",
                    op->mMode, switchCode, code, uid, packageName.string());
            }

            system->GetCurrentTimeMillis(&op->mRejectTime);
            *mode = switchOp->mMode;
            return NOERROR;
        }
        if (DEBUG) {
            Slogger::D(TAG, "noteOperation: allowing code %d uid %d package %s",
                code, uid, packageName.string());
        }
        system->GetCurrentTimeMillis(&op->mTime);
        op->mRejectTime = 0;
        *mode = IAppOpsManager::MODE_ALLOWED;
        return NOERROR;
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::StartOperation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = IAppOpsManager::MODE_ERRORED;

    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
    ClientState* client = (ClientState*)token;
    synchronized(this) {
        AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, TRUE);
        if (ops == NULL) {
            if (DEBUG) {
                Slogger::D(TAG, "startOperation: no op for code %d uid %d package %s",
                    code, uid, packageName.string());
            }
            *mode = IAppOpsManager::MODE_ERRORED;
            return NOERROR;
        }

        AutoPtr<Op> op = GetOpLocked(ops, code, TRUE);
        if (IsOpRestricted(uid, code, packageName)) {
            *mode = IAppOpsManager::MODE_IGNORED;
            return NOERROR;
        }
        Int32 switchCode;
        aom->OpToSwitch(code, &switchCode);
        AutoPtr<Op> switchOp = switchCode != code ? GetOpLocked(ops, switchCode, TRUE) : op;
        if (switchOp->mMode != IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG) {
                Slogger::D(TAG, "startOperation: reject #%d for code %d (%d) uid %d package %s",
                    op->mMode, switchCode, code, uid, packageName.string());
            }
            system->GetCurrentTimeMillis(&op->mRejectTime);
            *mode = switchOp->mMode;
            return NOERROR;
        }
        if (DEBUG) {
            Slogger::D(TAG, "startOperation: allowing code %d, uid %d package %d",
                code, uid, packageName.string());
        }
        if (op->mNesting == 0) {
            system->GetCurrentTimeMillis(&op->mTime);
            op->mRejectTime = 0;
            op->mDuration = -1;
        }
        op->mNesting++;
        if (client->mStartedOps != NULL) {
            client->mStartedOps->Add(TO_IINTERFACE(op));
        }
        *mode = IAppOpsManager::MODE_ALLOWED;
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::FinishOperation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))
    ClientState* client = (ClientState*)token;
    synchronized(this) {
        AutoPtr<Op> op = GetOpLocked(code, uid, packageName, TRUE);
        if (op == NULL) {
            return NOERROR;
        }
        if (client->mStartedOps != NULL) {
            Boolean bval;
            client->mStartedOps->Remove(TO_IINTERFACE(op), &bval);
            if (!bval) {
                Slogger::E(TAG, "Operation not started: uid%d pkg=%s op=%d",
                    op->mUid, op->mPackageName.string(), op->mOp);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
        FinishOperationLocked(op);
    }
    return NOERROR;
}

void CAppOpsService::FinishOperationLocked(
    /* [in] */ CAppOpsService::Op* op)
{
    if (op->mNesting <= 1) {
        if (op->mNesting == 1) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 t;
            system->GetCurrentTimeMillis(&t);
            op->mDuration = (Int32)(t - op->mTime);
            op->mTime += op->mDuration;
        }
        else {
            Slogger::W(TAG, "Finishing op nesting under-run: uid %d pkg %s code %d time=%lld duration=%d nesting=%d",
                op->mUid, op->mPackageName.string(), op->mOp, op->mTime, op->mDuration, op->mNesting);
        }
        op->mNesting = 0;
    }
    else {
        op->mNesting--;
    }
}

ECode CAppOpsService::VerifyIncomingUid(
    /* [in] */ Int32 uid)
{
    if (uid == Binder::GetCallingUid()) {
        return NOERROR;
    }
    if (Binder::GetCallingPid() == Process::MyPid()) {
        return NOERROR;
    }

    FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::UPDATE_APP_OPS_STATS,
        Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL)))
    return NOERROR;
}

ECode CAppOpsService::VerifyIncomingOp(
    /* [in] */ Int32 op)
{
    if (op >= 0 && op < IAppOpsManager::_NUM_OP) {
        return NOERROR;
    }
    Slogger::E(TAG, "Bad operation #%d", op);
    return E_ILLEGAL_STATE_EXCEPTION;
}

AutoPtr<CAppOpsService::Ops> CAppOpsService::GetOpsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean edit)
{
    String pkg(packageName);
    if (uid == 0) {
        pkg = "root";
    }
    else if (uid ==IProcess::SHELL_UID) {
        pkg = "com.android.shell";
    }
    return GetOpsRawLocked(uid, pkg, edit);
}

AutoPtr<CAppOpsService::Ops> CAppOpsService::GetOpsRawLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean edit)
{
    AutoPtr<IInterface> obj;
    mUidOps->Get(uid, (IInterface**)&obj);
    AutoPtr<IHashMap> pkgOps = IHashMap::Probe(obj);
    if (pkgOps == NULL) {
        if (!edit) {
            return NULL;
        }
        CHashMap::New((IHashMap**)&pkgOps);
        mUidOps->Put(uid, pkgOps.Get());
    }

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(packageName);
    obj = NULL;
    pkgOps->Get(csq.Get(), (IInterface**)&obj);
    AutoPtr<Ops> ops = (Ops*)IObject::Probe(obj);
    if (ops == NULL) {
        if (!edit) {
            return NULL;
        }
        Boolean isPrivileged = FALSE;
        // This is the first time we have seen this package name under this uid,
        // so let's make sure it is valid.
        if (uid != 0) {
            Int64 ident = Binder::ClearCallingIdentity();

            Int32 pkgUid = -1;
            AutoPtr<IActivityThreadHelper> ath;
            CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&ath);
            AutoPtr<IIPackageManager> pm;
            ECode ec = ath->GetPackageManager((IIPackageManager**)&pm);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::W(TAG, "Could not contact PackageManager");
            }
            else {
                AutoPtr<IApplicationInfo> appInfo;
                ECode ec = pm->GetApplicationInfo(packageName, 0, UserHandle::GetUserId(uid), (IApplicationInfo**)&appInfo);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    Slogger::W(TAG, "Could not contact PackageManager");
                }
                if (appInfo != NULL) {
                    appInfo->GetUid(&pkgUid);
                    Int32 flags;
                    appInfo->GetFlags(&flags);
                    isPrivileged = (flags & IApplicationInfo::FLAG_PRIVILEGED) != 0;
                }
                else {
                    if (packageName.Equals("media")) {
                        pkgUid =IProcess::MEDIA_UID;
                        isPrivileged = FALSE;
                    }
                }
            }

            if (pkgUid != uid) {
                // Oops!  The package name is not valid for the uid they are calling
                // under.  Abort.
                Slogger::W(TAG, "Bad call: specified package %s under uid %d but it is really %d",
                    packageName.string(), uid, pkgUid);
                return NULL;
            }
            Binder::RestoreCallingIdentity(ident);
        }
        ops = new Ops();
        ops->constructor(packageName, uid, isPrivileged);
        pkgOps->Put(csq.Get(), TO_IINTERFACE(ops));
    }
    return ops;
}

void CAppOpsService::ScheduleWriteLocked()
{
    if (!mWriteScheduled) {
        mWriteScheduled = TRUE;
        Boolean bval;
        mHandler->PostDelayed(mWriteRunner, WRITE_DELAY, &bval);
    }
}

void CAppOpsService::ScheduleWriteNowLocked()
{
    if (!mWriteScheduled) {
        mWriteScheduled = TRUE;
    }
    mHandler->RemoveCallbacks(mWriteRunner);
    Boolean bval;
    mHandler->Post(mWriteRunner, &bval);
}

AutoPtr<CAppOpsService::Op> CAppOpsService::GetOpLocked(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean edit)
{
    AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, edit);
    if (ops == NULL) {
        return NULL;
    }
    return GetOpLocked(ops, code, edit);
}

AutoPtr<CAppOpsService::Op> CAppOpsService::GetOpLocked(
    /* [in] */ CAppOpsService::Ops* ops,
    /* [in] */ Int32 code,
    /* [in] */ Boolean edit)
{
    Int32 mode;
    AutoPtr<IInterface> obj;
    ops->Get(code, (IInterface**)&obj);
    AutoPtr<Op> op = (Op*)IObject::Probe(obj);
    if (op == NULL) {
        if (!edit) {
            return NULL;
        }
        assert(0 && "TODO");
        //  mode = GetDefaultMode(code, ops.uid, ops.packageName);
        op = new Op(ops->mUid, ops->mPackageName, code, mode);
        ops->Put(code, TO_IINTERFACE(op));
    }
    if (edit) {
        ScheduleWriteLocked();
    }
    return op;
}

Boolean CAppOpsService::IsOpRestricted(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 code,
    /* [in] */ const String& packageName)
{
    Int32 userHandle = UserHandle::GetUserId(uid);
    AutoPtr<IInterface> obj;
    mOpRestrictions->Get(userHandle, (IInterface**)&obj);
    AutoPtr<IArrayOf> opRestrictions;//ArrayOf<Boolean>
    if (opRestrictions != NULL) {
        AutoPtr<IInterface> item;
        opRestrictions->Get(code, (IInterface**)&item);
        Boolean bval;
        IBoolean::Probe(item)->GetValue(&bval);
        if (bval) {
            AutoPtr<IAppOpsManagerHelper> aom;
            CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
            aom->OpAllowSystemBypassRestriction(code, &bval);
            if (bval) {
                synchronized(this) {
                    AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, TRUE);
                    if ((ops != NULL) && ops->mIsPrivileged) {
                        return FALSE;
                    }
                }
            }
            return TRUE;
        }
    }

    return FALSE;
}

void CAppOpsService::ReadState()
{
    ISynchronize* synObj = ISynchronize::Probe(mFile);
    synchronized(synObj) {
        synchronized(this) {
            AutoPtr<IFileInputStream> stream;
            ECode ec = mFile->OpenRead((IFileInputStream**)&stream);
            if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
                AutoPtr<IFile> baseFile;
                mFile->GetBaseFile((IFile**)&baseFile);
                Slogger::I(TAG, "No existing app ops %s; starting empty", TO_CSTR(baseFile));
                return;
            }
            Boolean success = FALSE;
            Int32 type, outerDepth, depth;
            String tagName;

            AutoPtr<IXmlPullParser> parser;
            Xml::NewPullParser((IXmlPullParser**)&parser);
            String name;
            parser->GetName(&name);
            ec = parser->SetInput(IInputStream::Probe(stream), String(NULL));
            FAIL_GOTO(ec, _ERROR_)

            ec = parser->Next(&type);
            FAIL_GOTO(ec, _ERROR_)
            while ((type) != IXmlPullParser::START_TAG
                    && type != IXmlPullParser::END_DOCUMENT) {
                ec = parser->Next(&type);
                FAIL_GOTO(ec, _ERROR_)
            }

            if (type != IXmlPullParser::START_TAG) {
                Slogger::E(TAG, "no start tag found");
                ec =  E_ILLEGAL_STATE_EXCEPTION;
                goto _ERROR_;
            }

            ec = parser->GetDepth(&outerDepth);
            FAIL_GOTO(ec, _ERROR_)

            ec = parser->Next(&type);
            FAIL_GOTO(ec, _ERROR_)
            while (type != IXmlPullParser::END_DOCUMENT
                    && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
                if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                    ec = parser->Next(&type);
                    FAIL_GOTO(ec, _ERROR_)
                    continue;
                }

                parser->GetName(&tagName);
                if (tagName.Equals("pkg")) {
                    ReadPackage(parser);
                }
                else {
                    Slogger::W(TAG, "Unknown element under <app-ops>: %s", name.string());
                    XmlUtils::SkipCurrentTag(parser);
                }

                ec = parser->Next(&type);
                FAIL_GOTO(ec, _ERROR_)
            }
            success = TRUE;

            goto _EXIT_;
_ERROR_:
            if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
                Slogger::W(TAG, "Failed parsing E_ILLEGAL_STATE_EXCEPTION");
            }
            else if (ec == (ECode)E_NULL_POINTER_EXCEPTION)  {
                Slogger::W(TAG, "Failed parsing E_NULL_POINTER_EXCEPTION");
            }
            else if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION)  {
                Slogger::W(TAG, "Failed parsing E_NUMBER_FORMAT_EXCEPTION");
            }
            else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION)  {
                Slogger::W(TAG, "Failed parsing E_XML_PULL_PARSER_EXCEPTION");
            }
            else if (ec == (ECode)E_IO_EXCEPTION)  {
                Slogger::W(TAG, "Failed parsing E_IO_EXCEPTION");
            }
            else if (ec == (ECode)E_INDEX_OUT_OF_BOUNDS_EXCEPTION)  {
                Slogger::W(TAG, "Failed parsing E_INDEX_OUT_OF_BOUNDS_EXCEPTION");
            }

_EXIT_:
            if (!success) {
                mUidOps->Clear();
            }
            ICloseable::Probe(stream)->Close();
        }
    }
}

ECode CAppOpsService::ReadPackage(
    /* [in] */ IXmlPullParser* parser)
{
    String pkgName;
    parser->GetAttributeValue(String(NULL), String("n"), &pkgName);
    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("uid")) {
            ReadUid(parser, pkgName);
        }
        else {
            Slogger::W(TAG, "Unknown element under <pkg>: %s", tagName.string());
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

ECode CAppOpsService::ReadUid(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& pkgName)
{
    String str;
    parser->GetAttributeValue(String(NULL), String("n"), &str);
    Int32 uid;
    StringUtils::Parse(str, &uid);
    String isPrivilegedString;
    parser->GetAttributeValue(String(NULL), String("p"), &isPrivilegedString);
    Boolean isPrivileged = FALSE;
    if (isPrivilegedString == NULL) {
        AutoPtr<IActivityThreadHelper> ath;
        CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&ath);
        AutoPtr<IIPackageManager> packageManager;
        ECode ec = ath->GetPackageManager((IIPackageManager**)&packageManager);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "Could not contact PackageManager");
        }

        if (packageManager != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            packageManager->GetApplicationInfo(pkgName, 0, UserHandle::GetUserId(uid), (IApplicationInfo**)&appInfo);
            if (appInfo != NULL) {
                Int32 flags;
                appInfo->GetFlags(&flags);
                isPrivileged = (flags & IApplicationInfo::FLAG_PRIVILEGED) != 0;
            }
        } else {
            // Could not load data, don't add to cache so it will be loaded later.
            return NOERROR;
        }
    }
    else {
        isPrivileged = StringUtils::ParseBoolean(isPrivilegedString);
    }

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(pkgName);
    IInterface* pkgNameObj = (IInterface*)csq.Get();
    Int32 outerDepth, depth, mode;
    parser->GetDepth(&outerDepth);
    Int32 type;
    String nullStr;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("op")) {
            parser->GetAttributeValue(nullStr, String("n"), &str);
            AutoPtr<Op> op = new Op(uid, pkgName, StringUtils::ParseInt32(str),
                    IAppOpsManager::MODE_ERRORED);

            parser->GetAttributeValue(nullStr, String("m"), &str);
            if (str != NULL) {
                op->mMode = StringUtils::ParseInt32(str);
            }
            parser->GetAttributeValue(nullStr, String("t"), &str);
            if (str != NULL) {
                op->mTime = StringUtils::ParseInt64(str);
            }
            parser->GetAttributeValue(nullStr, String("r"), &str);
            if (str != NULL) {
                op->mRejectTime = StringUtils::ParseInt64(str);
            }
            parser->GetAttributeValue(nullStr, String("d"), &str);
            if (str != NULL) {
                op->mDuration = StringUtils::ParseInt32(str);
            }

            AutoPtr<IInterface> obj;
            mUidOps->Get(uid, (IInterface**)&obj);
            AutoPtr<IHashMap> pkgOps = IHashMap::Probe(obj);
            if (pkgOps == NULL) {
                CHashMap::New((IHashMap**)&pkgOps);
                mUidOps->Put(uid, pkgOps.Get());
            }
            obj = NULL;
            pkgOps->Get(pkgNameObj, (IInterface**)&obj);
            AutoPtr<Ops> ops = (Ops*)IObject::Probe(obj);
            if (ops == NULL) {
                ops = new Ops();
                ops->constructor(pkgName, uid, isPrivileged);
                pkgOps->Put(pkgNameObj, TO_IINTERFACE(ops));
            }
            ops->Put(op->mOp, TO_IINTERFACE(op));
        }
        else {
            Slogger::W(TAG, "Unknown element under <pkg>: %s", tagName.string());
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

ECode CAppOpsService::WriteState()
{
    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

    ECode ec = NOERROR;

    ISynchronize* synObj = ISynchronize::Probe(mFile);
    synchronized(synObj) {
        AutoPtr<IList> allOps; //<IAppOpsManagerPackageOps>
        GetPackagesForOps(NULL, (IList**)&allOps);

        AutoPtr<IFileOutputStream> stream;
        // try {
        ec = mFile->StartWrite((IFileOutputStream**)&stream);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W(TAG, "Failed to write state: E_IO_EXCEPTION");
            return ec;
        }

        String lastPkg, pkgName;
        Int64 time;
        Int32 size, opsSize, uid, dur, ival, mode, defaultMode;
        AutoPtr<IInterface> obj;
        AutoPtr<IAppOpsManagerPackageOps> pkg;
        AutoPtr<IList> ops;
        String nullStr;

        AutoPtr<IXmlSerializer> out;
        CFastXmlSerializer::New((IXmlSerializer**)&out);
        FAIL_GOTO(out->SetOutput(IOutputStream::Probe(stream), String("utf-8")), _ERROR_)
        FAIL_GOTO(out->StartDocument(nullStr, TRUE), _ERROR_)
        FAIL_GOTO(out->WriteStartTag(nullStr, String("app-ops")), _ERROR_)

        if (allOps != NULL) {
            allOps->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                obj = NULL;
                allOps->Get(i, (IInterface**)&obj);
                pkg = IAppOpsManagerPackageOps::Probe(obj);
                pkg->GetPackageName(&pkgName);
                if (!pkgName.Equals(lastPkg)) {
                    if (lastPkg != NULL) {
                        FAIL_GOTO(out->WriteEndTag(nullStr, String("pkg")), _ERROR_)
                    }
                    lastPkg = pkgName;
                    FAIL_GOTO(out->WriteStartTag(nullStr, String("pkg")), _ERROR_)
                    FAIL_GOTO(out->WriteAttribute(nullStr, String("n"), lastPkg), _ERROR_)
                }
                FAIL_GOTO(out->WriteStartTag(nullStr, String("uid")), _ERROR_)
                pkg->GetUid(&uid);
                FAIL_GOTO(out->WriteAttribute(nullStr, String("n"), StringUtils::ToString(uid)), _ERROR_)

                synchronized(this) {
                    AutoPtr<Ops> ops = GetOpsLocked(uid, pkgName, FALSE);
                    // Should always be present as the list of PackageOps is generated
                    // from Ops.
                    if (ops != NULL) {
                        FAIL_GOTO(out->WriteAttribute(nullStr, String("p"),
                            StringUtils::BooleanToString(ops->mIsPrivileged)), _ERROR_)

                    }
                    else {
                        FAIL_GOTO(out->WriteAttribute(nullStr, String("p"),
                            StringUtils::BooleanToString(FALSE)), _ERROR_)
                    }
                }
                ops = NULL;
                pkg->GetOps((IList**)&ops);
                ops->GetSize(&opsSize);
                for (Int32 j = 0; j < opsSize; j++) {
                    AutoPtr<IInterface> opObj;
                    ops->Get(j, (IInterface**)&opObj);
                    IAppOpsManagerOpEntry* op = IAppOpsManagerOpEntry::Probe(opObj);
                    out->WriteStartTag(nullStr, String("op"));
                    op->GetOp(&ival);
                    assert(0 && "TODO:");
                    // defaultMode = GetDefaultMode(op.getOp(),
                    //                 pkg.getUid(), pkg.getPackageName());
                    out->WriteAttribute(nullStr, String("n"), StringUtils::ToString(ival));
                    op->GetMode(&mode);
                    if (mode != defaultMode) {
                        out->WriteAttribute(nullStr, String("m"), StringUtils::ToString(mode));
                    }

                    op->GetTime(&time);
                    if (time != 0) {
                        out->WriteAttribute(nullStr, String("t"), StringUtils::ToString(time));
                    }
                    op->GetRejectTime(&time);
                    if (time != 0) {
                        out->WriteAttribute(nullStr, String("r"), StringUtils::ToString(time));
                    }

                    op->GetDuration(&dur);
                    if (dur != 0) {
                        out->WriteAttribute(nullStr, String("d"), StringUtils::ToString(dur));
                    }
                    FAIL_GOTO(out->WriteEndTag(nullStr, String("op")), _ERROR_)
                }
                FAIL_GOTO(out->WriteEndTag(nullStr, String("uid")), _ERROR_)
            }
            if (lastPkg != NULL) {
                FAIL_GOTO(out->WriteEndTag(nullStr, String("pkg")), _ERROR_)
            }
        }

        FAIL_GOTO(out->WriteEndTag(nullStr, String("app-ops")), _ERROR_)
        FAIL_GOTO(out->EndDocument(), _ERROR_)
        FAIL_GOTO(mFile->FinishWrite(stream), _ERROR_)

        return NOERROR;
_ERROR_:
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W(TAG, "Failed to write state, restoring backup.");
            mFile->FailWrite(stream);
        }
    }
    return ec;
}

//@Override
ECode CAppOpsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // if (mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw->Println("Permission Denial: can't dump ApOps service from from pid="
    //             + Binder::GetCallingPid()
    //             + ", uid=" + Binder::GetCallingUid());
    //     return;
    // }

    // AutoPtr<IAppOpsManagerHelper> aom;
    // CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

    // synchronized(this) {
    //     pw->Println("Current AppOps Service state:");
    //     Int64 now = System->GetCurrentTimeMillis();
    //     Boolean needSep = FALSE;
    //     if (mOpModeWatchers->GetSize(&size) > 0) {
    //         needSep = TRUE;
    //         pw->Println("  Op mode watchers:");
    //         for (Int32 i=0; i<mOpModeWatchers->GetSize(&size); i++) {
    //             pw->Print("    Op "); pw->Print(aom->OpToName(mOpModeWatchers->KeyAt(i)));
    //             pw->Println(":");
    //             ArrayList<Callback> callbacks = mOpModeWatchers->ValueAt(i);
    //             for (Int32 j=0; j<callbacks->GetSize(&size); j++) {
    //                 pw->Print("      #"); pw->Print(j); pw->Print(": ");
    //                 pw->Println(callbacks->Get(j));
    //             }
    //         }
    //     }
    //     if (mPackageModeWatchers->GetSize(&size) > 0) {
    //         needSep = TRUE;
    //         pw->Println("  Package mode watchers:");
    //         for (Int32 i=0; i<mPackageModeWatchers->GetSize(&size); i++) {
    //             pw->Print("    Pkg "); pw->Print(mPackageModeWatchers->KeyAt(i));
    //             pw->Println(":");
    //             ArrayList<Callback> callbacks = mPackageModeWatchers->ValueAt(i);
    //             for (Int32 j=0; j<callbacks->GetSize(&size); j++) {
    //                 pw->Print("      #"); pw->Print(j); pw->Print(": ");
    //                 pw->Println(callbacks->Get(j));
    //             }
    //         }
    //     }
    //     if (mModeWatchers->GetSize(&size) > 0) {
    //         needSep = TRUE;
    //         pw->Println("  All mode watchers:");
    //         for (Int32 i=0; i<mModeWatchers->GetSize(&size); i++) {
    //             pw->Print("    "); pw->Print(mModeWatchers->KeyAt(i));
    //             pw->Print(" -> "); pw->Println(mModeWatchers->ValueAt(i));
    //         }
    //     }
    //     if (mClients->GetSize(&size) > 0) {
    //         needSep = TRUE;
    //         pw->Println("  Clients:");
    //         for (Int32 i=0; i<mClients->GetSize(&size); i++) {
    //             pw->Print("    "); pw->Print(mClients->KeyAt(i)); pw->Println(":");
    //             ClientState cs = mClients->ValueAt(i);
    //             pw->Print("      "); pw->Println(cs);
    //             if (cs->mStartedOps != NULL && cs->mStartedOps->GetSize(&size) > 0) {
    //                 pw->Println("      Started ops:");
    //                 for (Int32 j=0; j<cs->mStartedOps->GetSize(&size); j++) {
    //                     Op op = cs->mStartedOps->Get(j);
    //                     pw->Print("        "); pw->Print("uid="); pw->Print(op->mUid);
    //                     pw->Print(" pkg="); pw->Print(op->mPackageName);
    //                     pw->Print(" op="); pw->Println(aom->OpToName(op->mOp));
    //                 }
    //             }
    //         }
    //     }
    //     if (mAudioRestrictions->GetSize(&size) > 0) {
    //         Boolean printedHeader = FALSE;
    //         for (Int32 o=0; o<mAudioRestrictions->GetSize(&size); o++) {
    //             String op = aom->OpToName(mAudioRestrictions->KeyAt(o));
    //             SparseArray<Restriction> restrictions = mAudioRestrictions->ValueAt(o);
    //             for (Int32 i=0; i<restrictions->GetSize(&size); i++) {
    //                 if (!printedHeader){
    //                     pw->Println("  Audio Restrictions:");
    //                     printedHeader = TRUE;
    //                     needSep = TRUE;
    //                 }
    //                 Int32 usage = restrictions->KeyAt(i);
    //                 pw->Print("    "); pw->Print(op);
    //                 pw->Print(" usage="); pw->Print(AudioAttributes->UsageToString(usage));
    //                 Restriction r = restrictions->ValueAt(i);
    //                 pw->Print(": mode="); pw->Println(r->mMode);
    //                 if (!r->mExceptionPackages->IsEmpty()) {
    //                     pw->Println("      Exceptions:");
    //                     for (Int32 j=0; j<r->mExceptionPackages->GetSize(&size); j++) {
    //                         pw->Print("        "); pw->Println(r->mExceptionPackages->ValueAt(j));
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     if (needSep) {
    //         pw->Println();
    //     }
    //     for (Int32 i=0; i<mUidOps->GetSize(&size); i++) {
    //         pw->Print("  Uid "); UserHandle::FormatUid(pw, mUidOps->KeyAt(i)); pw->Println(":");
    //         HashMap<String, Ops> pkgOps = mUidOps->ValueAt(i);
    //         for (Ops ops : pkgOps->Values()) {
    //             pw->Print("    Package "); pw->Print(ops->mPackageName); pw->Println(":");
    //             for (Int32 j=0; j<ops->GetSize(&size); j++) {
    //                 Op op = ops->ValueAt(j);
    //                 pw->Print("      "); pw->Print(aom->OpToName(op->mOp));
    //                 pw->Print(": mode="); pw->Print(op->mMode);
    //                 if (op->mTime != 0) {
    //                     pw->Print("; time="); TimeUtils->FormatDuration(now-op->mTime, pw);
    //                     pw->Print(" ago");
    //                 }
    //                 if (op->mRejectTime != 0) {
    //                     pw->Print("; rejectTime="); TimeUtils->FormatDuration(now-op->mRejectTime, pw);
    //                     pw->Print(" ago");
    //                 }
    //                 if (op->mDuration == -1) {
    //                     pw->Println(" (running)");
    //                 } else {
    //                     pw->Print("; duration=");
    //                             TimeUtils->FormatDuration(op->mDuration, pw);
    //                             pw->Println();
    //                 }
    //             }
    //         }
    //     }
    // }
    return NOERROR;
}

//@Override
ECode CAppOpsService::SetUserRestrictions(
    /* [in] */ IBundle* restrictions,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(CheckSystemUid(String("SetUserRestrictions")))

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);

    AutoPtr<IInterface> obj;
    mOpRestrictions->Get(userHandle, (IInterface**)&obj);
    AutoPtr<IArrayOf> opRestrictions;//ArrayOf<Boolean>

    if (opRestrictions == NULL) {
        CArrayOf::New(EIID_IBoolean, IAppOpsManager::_NUM_OP, (IArrayOf**)&opRestrictions);
        mOpRestrictions->Put(userHandle, opRestrictions.Get());
    }
    Int32 length;
    opRestrictions->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        Boolean bval = FALSE;

        String restriction;
        aom->OpToRestriction(i, &restriction);
        if (restriction != NULL) {
            restrictions->GetBoolean(restriction, FALSE, &bval);
        }

        AutoPtr<IBoolean> bobj = CoreUtils::Convert(bval);
        opRestrictions->Set(i, bobj.Get());
    }
    return NOERROR;
}

//@Override
ECode CAppOpsService::RemoveUser(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(CheckSystemUid(String("RemoveUser")))
    mOpRestrictions->Remove(userHandle);
    return NOERROR;
}

ECode CAppOpsService::IsControlAllowed(
    /* [in] */ Int32 code,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

// TODO: Need mPolicy
    // Boolean isShow = TRUE;
    // if (mPolicy != NULL) {
    //     mPolicy->IsControlAllowed(code, packageName, &isShow);
    // }
    // *result = isShow;
    return NOERROR;
}

ECode CAppOpsService::GetPrivacyGuardSettingForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < PRIVACY_GUARD_OP_STATES->GetLength(); i++) {
        Int32 op = (*PRIVACY_GUARD_OP_STATES)[i];
        AutoPtr<IAppOpsManagerHelper> aom;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
        Int32 switchOp;
        aom->OpToSwitch(op, &switchOp);
        Int32 mode;
        CheckOperation(op, uid, packageName, &mode);
        if (mode != IAppOpsManager::MODE_ALLOWED && !IsOpRestricted(uid, op, packageName)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CAppOpsService::SetPrivacyGuardSettingForPackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean state)
{
    for (Int32 i = 0; i < PRIVACY_GUARD_OP_STATES->GetLength(); i++) {
        Int32 op = (*PRIVACY_GUARD_OP_STATES)[i];
        AutoPtr<IAppOpsManagerHelper> aom;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
        Int32 switchOp;
        aom->OpToSwitch(op, &switchOp);
        return SetMode(switchOp, uid, packageName, state
                ? IAppOpsManager::MODE_ASK : IAppOpsManager::MODE_ALLOWED);
    }
    return NOERROR;
}

ECode CAppOpsService::ResetCounters()
{
    FAIL_RETURN(mContext->EnforcePermission(Manifest::permission::UPDATE_APP_OPS_STATS,
        Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL)))

    synchronized(this) {
        Int32 size;
        mUidOps->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mUidOps->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IHashMap> packages = IHashMap::Probe(obj);

            AutoPtr<ISet> set;
            packages->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean b;
            while (it->HasNext(&b), b) {
                obj = NULL;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
                AutoPtr<IInterface> iKey;
                entry->GetKey((IInterface**)&iKey);
                String packageName;
                ICharSequence::Probe(iKey)->ToString(&packageName);
                AutoPtr<IInterface> iValue;
                entry->GetValue((IInterface**)&iValue);
                AutoPtr<Ops> pkgOps = (Ops*)(IObject*)iValue.Get();

                Int32 s;
                pkgOps->GetSize(&s);
                for (Int32 j = 0; j < s; j++) {
                    obj = NULL;
                    pkgOps->ValueAt(j, (IInterface**)&obj);
                    AutoPtr<Op> curOp = (Op*)(IObject*)obj.Get();
                    curOp->mAllowedCount = 0;
                    curOp->mIgnoredCount = 0;
                }
            }
        }
        // ensure the counter reset persists
        ScheduleWriteNowLocked();
    }
    return NOERROR;
}

ECode CAppOpsService::CheckSystemUid(
    /* [in] */ const String& function)
{
    Int32 uid = Binder::GetCallingUid();
    if (uid != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "%s must by called by the system. SecurityException", function.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CAppOpsService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}


} // Server
} // Droid
} // Elastos
