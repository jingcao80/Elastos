
#include "elastos/droid/server/CAppOpsService.h"
#include "elastos/droid/server/CAppOpsServiceClientState.h"
#include "elastos/droid/server/AppOpsPolicy.h"
#include "elastos/droid/server/CPermissionDialog.h"
#include "elastos/droid/app/AppOpsManager.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::App::AppOpsManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
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
using Elastos::Droid::Internal::App::EIID_IIAppOpsCallback;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Core::AutoLock;
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
using Elastos::IO::CFile;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

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
    {    AutoLock syncLock(mHost);
        mHost->mWriteScheduled = FALSE;
        AutoPtr<AsyncTask> task = (AsyncTask*)new WriteStateTask(mHost);
        return task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
    }
    return NOERROR;
}

//================================================================================
// CAppOpsService::ChangedRunner
//================================================================================

CAppOpsService::ChangedRunner::ChangedRunner(
    /* [in] */ CAppOpsService* host)
    : mHost(host)
{}

ECode CAppOpsService::ChangedRunner::Run()
{
    AutoPtr<IIntent> p;
    CIntent::New(IAppOpsManager::ACTION_SU_SESSION_CHANGED, (IIntent**)&p);
    AutoPtr<IUserHandleHelper> hlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
    AutoPtr<IUserHandle> hd;
    hlp->GetALL((IUserHandle**)&hd);
    mHost->mContext->SendBroadcastAsUser(p, hd);
    return NOERROR;
}

//================================================================================
// CAppOpsService::AskRunnable
//================================================================================

CAppOpsService::AskRunnable::AskRunnable(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ CAppOpsService::Op* op,
    /* [in] */ PermissionDialogReqQueue::PermissionDialogReq* request,
    /* [in] */ CAppOpsService* host)
    : mCode(code)
    , mUid(uid)
    , mPackageName(packageName)
    , mOp(op)
    , mRequest(request)
    , mHost(host)
{
}

ECode CAppOpsService::AskRunnable::Run()
{
    AutoPtr<IPermissionDialog> permDialog;
    {
        AutoLock syncLock(mHost);

        mOp->mDialogReqQueue->Register(mRequest);
        if (mOp->mDialogReqQueue->GetDialog() == NULL) {
            CPermissionDialog::New(mHost->mContext, (IIAppOpsService*)mHost, mCode, mUid, mPackageName,
                (IPermissionDialog**)&permDialog);
            mOp->mDialogReqQueue->SetDialog(permDialog);
        }
    }

    Logger::I(TAG, "Creating dialog box: %s", TO_CSTR(permDialog));
    if (permDialog != NULL) {
        IDialog::Probe(permDialog)->Show();
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
    , mNoteOpCount(0)
    , mStartOpCount(0)
{
    mDialogReqQueue = new PermissionDialogReqQueue();
    CArrayList::New((IArrayList**)&mClientTokens);
}

ECode CAppOpsService::Op::ToString(
    /* [out] */ String *result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb;
    sb.Append("mUid:");
    sb.Append(mUid);
    sb.Append(";mPackageName:");
    sb.Append(mPackageName);
    sb.Append(";mOp:");
    sb.Append(mOp);
    sb.Append(";mMode:");
    sb.Append(mMode);
    sb.Append(";mDuration:");
    sb.Append(mDuration);
    sb.Append(";mTime:");
    sb.Append(mTime);
    sb.Append(";mRejectTime:");
    sb.Append(mRejectTime);
    sb.Append(";mNesting:");
    sb.Append(mNesting);
    sb.Append(";mNoteOpCount:");
    sb.Append(mNoteOpCount);
    sb.Append(";mStartOpCount:");
    sb.Append(mStartOpCount);
    *result = sb.ToString();
    return NOERROR;
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

//ECode CAppOpsService::Ops::Put(
//    /* [in] */ Int32 key,
//    /* [in] */ IInterface* value)
//{
//    if (mPackageName.Equals(String("Elastos.Droid.TeleService")) && key == 15) {
//        Logger::E("leliang", "line:%d, func:%s, value:%s\n", __LINE__, __func__, TO_CSTR(value));
//    }
//    return SparseArray::Put(key, value);
//}

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

//================================================================================
// CAppOpsService::ClientState
//================================================================================

CAR_INTERFACE_IMPL_2(CAppOpsService::ClientState, Object, IProxyDeathRecipient, IBinder)

ECode CAppOpsService::ClientState::constructor(
    /* [in] */ IBinder* appToken,
    /* [in] */ IIAppOpsService* host)
{
    mAppToken = appToken;
    mHost = (CAppOpsService*)host;
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
    return NOERROR;
}

//@Override
ECode CAppOpsService::ClientState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CAppOpsService::ClientState{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", appToken:";
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
    {
        AutoLock syncLock(mHost);
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

    // We cannot broadcast on the synchronized block above because the broadcast might
    // trigger another appop call that eventually arrives here from a different thread,
    // causing a deadlock.
    Int32 size = 0;
    mStartedOps->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> p;
        mStartedOps->Get(i, (IInterface**)&p);
        Op* pOp = (Op*)IObject::Probe(p);
        mHost->BroadcastOpIfNeeded(pOp->mOp);
    }
    return NOERROR;
}

//================================================================================
// CAppOpsService::Restriction
//================================================================================

const AutoPtr<HashSet<String> > CAppOpsService::Restriction::NO_EXCEPTIONS = new HashSet<String>();

CAppOpsService::Restriction::Restriction()
    : mMode(0)
{
    mExceptionPackages = NO_EXCEPTIONS;
}

//================================================================================
// NativeAppOpsService
//================================================================================

enum {
    CHECK_OPERATION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION,
    NOTE_OPERATION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+1,
    START_OPERATION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+2,
    FINISH_OPERATION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+3,
    START_WATCHING_MODE_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+4,
    STOP_WATCHING_MODE_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+5,
    GET_TOKEN_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION+6,
};

CAR_INTERFACE_IMPL(CAppOpsService::NativeAppOpsService::AppOpsCallback, Object, IIAppOpsCallback)

ECode CAppOpsService::NativeAppOpsService::AppOpsCallback::OpChanged(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName)
{
    mCallback->opChanged(op, android::String16(packageName.string()));
    return NOERROR;
}

UInt32 CAppOpsService::NativeAppOpsService::ClientToken::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CAppOpsService::NativeAppOpsService::ClientToken::Release()
{
    return ElRefBase::Release();
}

PInterface CAppOpsService::NativeAppOpsService::ClientToken::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastos::Droid::Os::IBinder*)this;
    }
    else if (riid == EIID_IBinder) {
        return (Elastos::Droid::Os::IBinder*)this;
    }
    return Object::Probe(riid);
}

ECode CAppOpsService::NativeAppOpsService::ClientToken::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IInterface*)(Elastos::Droid::Os::IBinder*)this) {
        *iid = EIID_IBinder;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}


ECode CAppOpsService::NativeAppOpsService::ClientToken::LinkToDeath(
    /* [in] */ IProxyDeathRecipient* recipient,
    /* [in] */ NativeAppOpsService* host)
{
    if (!mClientToken->localBinder()) {
        android::sp<ClientTokenDeathRecipient> ctdr =
            new ClientTokenDeathRecipient(recipient, host);
        android::status_t err = mClientToken->linkToDeath(ctdr, NULL, 0);
        if (err != android::NO_ERROR) {
            Logger::D(TAG, "NativeAppOpsService::ClientToken::LinkToDeath failed");
            return E_FAIL;
        }
    }
    return NOERROR;
}

void CAppOpsService::NativeAppOpsService::ClientTokenDeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    if (mRecipient != NULL) {
        mRecipient->ProxyDied();
        AutoLock lock(mHost->mTokenMapLock);
        mHost->mClientTokenMap.Erase(reinterpret_cast<Int64>(who.promote().get()));
        mHost->mTokenMap.Erase(Elastos::Droid::Os::IBinder::Probe(mRecipient));
    }
}

android::status_t CAppOpsService::NativeAppOpsService::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [in] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    android::String16 interfaceName("com.android.internal.app.IAppOpsService");
    switch(code) {
        case CHECK_OPERATION_TRANSACTION: {
            data.enforceInterface(interfaceName);
            Int32 code = data.readInt32();
            Int32 uid = data.readInt32();
            android::String8 packageName(data.readString16());
            Int32 res;
            mHost->CheckOperation(code, uid, String(packageName.string()), &res);
            reply->writeNoException();
            reply->writeInt32(res);
            return android::NO_ERROR;
        } break;
        case NOTE_OPERATION_TRANSACTION: {
            data.enforceInterface(interfaceName);
            Int32 code = data.readInt32();
            Int32 uid = data.readInt32();
            android::String8 packageName(data.readString16());
            Int32 res;
            mHost->NoteOperation(code, uid, String(packageName.string()), &res);
            reply->writeNoException();
            reply->writeInt32(res);
            return android::NO_ERROR;
        } break;
        case START_OPERATION_TRANSACTION: {
            data.enforceInterface(interfaceName);
            android::sp<IBinder> token = data.readStrongBinder();
            Int32 code = data.readInt32();
            Int32 uid = data.readInt32();
            android::String8 packageName(data.readString16());
            Int32 res;
            mHost->StartOperation(((Token*)token.get())->mToken, code, uid, String(packageName.string()), &res);
            reply->writeNoException();
            reply->writeInt32(res);
            return android::NO_ERROR;
        } break;
        case FINISH_OPERATION_TRANSACTION: {
            data.enforceInterface(interfaceName);
            android::sp<IBinder> token = data.readStrongBinder();
            Int32 code = data.readInt32();
            Int32 uid = data.readInt32();
            android::String8 packageName(data.readString16());
            mHost->FinishOperation(((Token*)token.get())->mToken, code, uid, String(packageName.string()));
            reply->writeNoException();
            return android::NO_ERROR;
        } break;
        case START_WATCHING_MODE_TRANSACTION: {
            data.enforceInterface(interfaceName);
            Int32 op = data.readInt32();
            android::String8 packageName(data.readString16());
            android::sp<IBinder> callback = data.readStrongBinder();
            AutoPtr<AppOpsCallback> elCallback = new AppOpsCallback(
                android::interface_cast<android::IAppOpsCallback>(callback).get());
            mCallbackMap[reinterpret_cast<Int64>(callback.get())] = elCallback;
            mHost->StartWatchingMode(op, String(packageName.string()), elCallback);
            reply->writeNoException();
            return android::NO_ERROR;
        } break;
        case STOP_WATCHING_MODE_TRANSACTION: {
            data.enforceInterface(interfaceName);
            android::sp<IBinder> callback = data.readStrongBinder();
            if (callback != NULL) {
                AutoPtr<AppOpsCallback> elCallback = mCallbackMap[reinterpret_cast<Int64>(callback.get())];
                assert(elCallback != NULL);
                mCallbackMap.Erase(reinterpret_cast<Int64>(callback.get()));
                mHost->StopWatchingMode(elCallback);
            }
            reply->writeNoException();
            return android::NO_ERROR;
        } break;
        case GET_TOKEN_TRANSACTION: {
            data.enforceInterface(interfaceName);
            android::sp<IBinder> clientToken = data.readStrongBinder();
            AutoPtr<ClientToken> elClientToken;
            {
                AutoLock lock(mTokenMapLock);
                elClientToken = mClientTokenMap[reinterpret_cast<Int64>(clientToken.get())];
                if (elClientToken == NULL) {
                    elClientToken = new ClientToken(clientToken.get());
                    mClientTokenMap[reinterpret_cast<Int64>(clientToken.get())] = elClientToken;
                }
            }
            AutoPtr<Elastos::Droid::Os::IBinder> elToken;
            mHost->GetToken(elClientToken, (Elastos::Droid::Os::IBinder**)&elToken);

            android::sp<android::IBinder> token;
            {
                AutoLock lock(mTokenMapLock);
                HashMap<AutoPtr<Elastos::Droid::Os::IBinder>, android::sp<android::IBinder> >::Iterator it = mTokenMap.Find(elToken);
                if (it != mTokenMap.End()) {
                    token = it->mSecond;
                }
                else {
                    token = new Token(elToken);
                    mTokenMap[elToken] = token;
                    elClientToken->LinkToDeath(IProxyDeathRecipient::Probe(elToken), this);
                }
            }

            reply->writeNoException();
            reply->writeStrongBinder(token);
            return android::NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
    return android::NO_ERROR;
}

//================================================================================
// CAppOpsService
//================================================================================

const String CAppOpsService::TAG("CAppOpsService");
const Boolean CAppOpsService::DEBUG = FALSE;
const String CAppOpsService::DEFAULT_POLICY_FILE("/system/etc/appops_policy.xml");

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

CAR_OBJECT_IMPL(CAppOpsServiceClientState)

CAppOpsService::CAppOpsService()
    : mWriteScheduled(FALSE)
{
}

ECode CAppOpsService::constructor(
    /* [in] */ IFile* storagePath,
    /* [in] */ IHandler* handler)
{
    mWriteRunner = new WriteRunner(this);

    mSuSessionChangedRunner = new ChangedRunner(this);

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
        return ec;
    }

    mHandler = handler;
    AutoPtr<ILooperHelper> hlp;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
    hlp->GetMyLooper((ILooper**)&mLooper);
    AutoPtr<IAppOpsManagerHelper> mhlp;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&mhlp);
    mhlp->IsStrictEnable(&mStrictEnable);
    ReadState();
    return NOERROR;
}

ECode CAppOpsService::Publish(
    /* [in] */ IContext* context)
{
    mContext = context;
    ReadPolicy();
    FAIL_RETURN(ServiceManager::AddService(IContext::APP_OPS_SERVICE, (IBinder*)this))
    if (mNative == NULL) {
        mNative = new NativeAppOpsService(this);
        android::sp<android::IServiceManager> sm = android::defaultServiceManager();
        int res = sm->addService(android::String16("appops"), mNative);
        if (res != 0) {
            Slogger::E(TAG, "add service appops failed");
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CAppOpsService::SystemReady()
{
    ECode ec = NOERROR;
    {    AutoLock syncLock(this);
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
                mUidOps->GetSize(&size);
            }
        }

        AutoPtr<IActivityThreadHelper> hlp;
        CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&hlp);
        AutoPtr<IIPackageManager> packageManager;
        hlp->GetPackageManager((IIPackageManager**)&packageManager);
        if (mLoadPrivLaterPkgs != NULL && packageManager != NULL) {
            Int32 size = 0;
            mLoadPrivLaterPkgs->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                Int32 uid = 0;
                mLoadPrivLaterPkgs->KeyAt(i, &uid);
                AutoPtr<IInterface> pPkg;
                mLoadPrivLaterPkgs->ValueAt(i, (IInterface**)&pPkg);
                String pkg;
                ICharSequence::Probe(pPkg)->ToString(&pkg);
                AutoPtr<IInterface> pUid;
                mUidOps->Get(uid, (IInterface**)&pUid);
                AutoPtr<IHashMap> pkgs = IHashMap::Probe(pUid);
                if (pkgs == NULL) {
                    continue;
                }
                AutoPtr<IInterface> pOps;
                pkgs->Get(pPkg, (IInterface**)&pOps);
                AutoPtr<Ops> ops = (Ops*)IObject::Probe(pOps);
                if (ops == NULL) {
                    continue;
                }
                // try {
                AutoPtr<IUserHandleHelper> uhlp;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhlp);
                Int32 id = 0;
                uhlp->GetUserId(uid, &id);
                AutoPtr<IApplicationInfo> appInfo;
                packageManager->GetApplicationInfo(
                        pkg, 0, id, (IApplicationInfo**)&appInfo);
                Int32 flags = 0;
                appInfo->GetFlags(&flags);
                if (appInfo != NULL
                        && (flags & IApplicationInfo::FLAG_PRIVILEGED) != 0) {
                    Slogger::I(TAG, "Privileged package %s", pkg.string());
                    AutoPtr<Ops> newOps = new Ops();
                    newOps->constructor(pkg, uid, TRUE);
                    Int32 s = 0;
                    ops->GetSize(&s);
                    for (Int32 j = 0; j < s; j++) {
                        Int32 key = 0;
                        ops->KeyAt(j, &key);
                        AutoPtr<IInterface> val;
                        ops->ValueAt(j, (IInterface**)&val);
                        newOps->Put(key, val);
                    }
                    pkgs->Put(CoreUtils::Convert(pkg), ISparseArray::Probe(newOps));
                    changed = TRUE;
                }
                // } catch (RemoteException e) {
                //     Slog.w(TAG, "Could not contact PackageManager", e);
                // }
            }
            mLoadPrivLaterPkgs = NULL;
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
    {    AutoLock syncLock(this);
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
    {    AutoLock syncLock(this);
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
    {    AutoLock syncLock(this);
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
    {    AutoLock syncLock(this);
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
            values->GetIterator((IIterator**)&it);
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
                    res->Add(resPackage);
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

    {    AutoLock syncLock(this);
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
    {    AutoLock syncLock(this);
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
                Int32 defaultMode = GetDefaultMode(code, uid, packageName);
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
    {    AutoLock syncLock(this);
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
                        defaultMode = GetDefaultMode(curOp->mOp, curOp->mUid,
                                curOp->mPackageName);
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
    AutoLock syncLock(this);
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

    return NOERROR;
}

//@Override
ECode CAppOpsService::StopWatchingMode(
    /* [in] */ IIAppOpsCallback* callback)
{
    AutoLock syncLock(this);
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
    return NOERROR;
}

//@Override
ECode CAppOpsService::GetToken(
    /* [in] */ IBinder* clientToken,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)

    AutoLock syncLock(this);
    AutoPtr<IInterface> obj;
    mClients->Get(clientToken, (IInterface**)&obj);
    AutoPtr<ClientState> cs = (ClientState*)IObject::Probe(obj);
    if (cs == NULL) {
        AutoPtr<IBinder> _cs;
        CAppOpsServiceClientState::New(clientToken, this, (IBinder**)&_cs);
        cs = (ClientState*)_cs.Get();
        mClients->Put(clientToken, TO_IINTERFACE(cs));
    }
    *token = IBinder::Probe(cs);
    REFCOUNT_ADD(*token)

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

    {
        AutoLock syncLock(this);
        if (IsOpRestricted(uid, code, packageName)) {
            *result = IAppOpsManager::MODE_IGNORED;
            return NOERROR;
        }
        Int32 sw;
        aom->OpToSwitch(code, &sw);
        AutoPtr<Op> op = GetOpLocked(sw, uid, packageName, FALSE);
        if (op == NULL) {
            *result = GetDefaultMode(code, uid, packageName);
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
    {    AutoLock syncLock(this);
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
    Int32 mode = IAppOpsManager::MODE_ALLOWED;
    AutoPtr<IInterface> obj;
    mAudioRestrictions->Get(code, (IInterface**)&obj);
    if (obj != NULL) {
        AutoPtr<ISparseArray> usageRestrictions = ISparseArray::Probe(obj);
        obj = NULL;
        usageRestrictions->Get(usage, (IInterface**)&obj);
        Restriction* r = (Restriction*)IObject::Probe(obj);
        if (r != NULL) {
            HashSet<String>::Iterator it = r->mExceptionPackages->Find(packageName);
            Boolean contains = (it != r->mExceptionPackages->End());
            if (!contains) {
                mode = r->mMode;
            }
        }
    }

    if (DEBUG) {
        Logger::D(TAG, "CheckRestrictionLocked: code:%d, usage:%d, uid:%d, packageName:%s, result:%d, ALLOWED:%d",
            code, usage, uid, packageName.string(), mode, (mode == IAppOpsManager::MODE_ALLOWED));
    }
    return mode;
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
    {    AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mAudioRestrictions->Get(code, (IInterface**)&obj);
        AutoPtr<ISparseArray> usageRestrictions = ISparseArray::Probe(obj);
        if (usageRestrictions == NULL) {
            CSparseArray::New((ISparseArray**)&usageRestrictions);
            mAudioRestrictions->Put(code, usageRestrictions.Get());
        }
        usageRestrictions->Remove(usage);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            AutoPtr<Restriction> r = new Restriction();
            r->mMode = mode;
            if (exceptionPackages != NULL) {
                Int32 N = exceptionPackages->GetLength();
                r->mExceptionPackages = new HashSet<String>();
                for (Int32 i = 0; i < N; i++) {
                    String pkg = (*exceptionPackages)[i];
                    if (pkg != NULL) {
                        r->mExceptionPackages->Insert(pkg.Trim());
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

    AutoLock syncLock(this);
    if (GetOpsRawLocked(uid, packageName, TRUE) != NULL) {
        *mode = IAppOpsManager::MODE_ALLOWED;
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

    AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> req;
    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
    {    AutoLock syncLock(this);
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
            op->mIgnoredCount++;
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
        Int32 switchCode = 0;
        aom->OpToSwitch(code, &switchCode);
        AutoPtr<Op> switchOp = switchCode != code ? GetOpLocked(ops, switchCode, TRUE) : op;
        if (switchOp->mMode != IAppOpsManager::MODE_ALLOWED
            && switchOp->mMode != IAppOpsManager::MODE_ASK) {
            if (DEBUG) {
                Slogger::D(TAG, "noteOperation: reject #%d for code %d (%d) uid %d package %s",
                    op->mMode, switchCode, code, uid, packageName.string());
            }

            system->GetCurrentTimeMillis(&(op->mRejectTime));
            op->mIgnoredCount++;
            *mode = switchOp->mMode;
            return NOERROR;
        }
        else if (switchOp->mMode == IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG) {
                Logger::D(TAG, "noteOperation: allowing code %d uid %d package %s",
                    code, uid, packageName.string());
            }
            system->GetCurrentTimeMillis(&(op->mTime));
            op->mRejectTime = 0;
            op->mAllowedCount++;
            BroadcastOpIfNeeded(code);
            *mode = IAppOpsManager::MODE_ALLOWED;
            return NOERROR;
        }
        else {
            AutoPtr<ILooperHelper> hlp;
            CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
            AutoPtr<ILooper> lp;
            hlp->GetMyLooper((ILooper**)&lp);
            if (lp == mLooper) {
                Logger::E(TAG,
                    "noteOperation: This method will deadlock "
                    "if called from the main thread. (Code: %d uid: %d package: %s)",
                    code, uid, packageName.string());
                *mode = switchOp->mMode;
                return NOERROR;
            }
            op->mNoteOpCount++;
            req = AskOperationLocked(code, uid, packageName, switchOp);
        }
    }

    Int32 result = req->Get();
    BroadcastOpIfNeeded(code);
    *mode = result;
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

    AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> req;
    FAIL_RETURN(VerifyIncomingUid(uid))
    FAIL_RETURN(VerifyIncomingOp(code))

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    AutoPtr<IAppOpsManagerHelper> aom;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
    ClientState* client = (ClientState*)token;
    {
        AutoLock syncLock(this);
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
            if (DEBUG) {
                Slogger::D(TAG, "startOperation: IsOpRestricted for code %d uid %d package %s",
                    code, uid, packageName.string());
            }
            op->mIgnoredCount++;
            *mode = IAppOpsManager::MODE_IGNORED;
            return NOERROR;
        }
        Int32 switchCode;
        aom->OpToSwitch(code, &switchCode);
        AutoPtr<Op> switchOp = switchCode != code ? GetOpLocked(ops, switchCode, TRUE) : op;
        if (switchOp->mMode != IAppOpsManager::MODE_ALLOWED
            && switchOp->mMode != IAppOpsManager::MODE_ASK) {
            if (DEBUG) {
                Slogger::D(TAG, "startOperation: reject #%d for code %d (%d) uid %d package %s",
                    op->mMode, switchCode, code, uid, packageName.string());
            }
            system->GetCurrentTimeMillis(&op->mRejectTime);
            *mode = switchOp->mMode;
            return NOERROR;
        }
        else if (switchOp->mMode == IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG) {
                Logger::D(TAG, "startOperation: allowing code %d uid %d package %s",
                    code, uid, packageName.string());
            }
            if (op->mNesting == 0) {
                system->GetCurrentTimeMillis(&op->mTime);
                op->mRejectTime = 0;
                op->mDuration = -1;
                op->mAllowedCount++;
            }
            op->mNesting++;
            if (client->mStartedOps != NULL) {
                client->mStartedOps->Add(ISynchronize::Probe(op));
            }
            BroadcastOpIfNeeded(code);
            *mode = IAppOpsManager::MODE_ALLOWED;
            return NOERROR;
        }
        else {
            AutoPtr<ILooperHelper> hlp;
            CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
            AutoPtr<ILooper> lp;
            hlp->GetMyLooper((ILooper**)&lp);
            if (lp == mLooper) {
                Logger::E(TAG, "startOperation: This method will deadlock"
                    " if called from the main thread. (Code: %d uid: %d package: %s)",
                    code, uid, packageName.string());
                *mode = switchOp->mMode;
                return NOERROR;
            }
            op->mStartOpCount++;
            AutoPtr<IBinder> clientToken = client->mAppToken;
            op->mClientTokens->Add(clientToken);
            req = AskOperationLocked(code, uid, packageName, switchOp);
        }
    }
    Int32 result = req->Get();
    BroadcastOpIfNeeded(code);
    *mode = result;
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
    {    AutoLock syncLock(this);
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
    BroadcastOpIfNeeded(code);
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
    if (Binder::GetCallingUid() == 0) {
        // Allow root to delegate uid operations.
        return NOERROR;
    }
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
    else if (uid == IProcess::SYSTEM_UID) {
        if (packageName.IsNull()) {
            pkg = "android";
        }
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
                Binder::RestoreCallingIdentity(ident);
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
    Int32 mode = 0;
    AutoPtr<IInterface> obj;
    ops->Get(code, (IInterface**)&obj);
    AutoPtr<Op> op = (Op*)IObject::Probe(obj);
    if (op == NULL) {
        if (!edit) {
            return NULL;
        }
        mode = GetDefaultMode(code, ops->mUid, ops->mPackageName);
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
    AutoPtr<IArrayOf> opRestrictions = IArrayOf::Probe(obj);//ArrayOf<Boolean>
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
                AutoLock syncLock(this);
                AutoPtr<Ops> ops = GetOpsLocked(uid, packageName, TRUE);
                if ((ops != NULL) && ops->mIsPrivileged) {
                    return FALSE;
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
    {    AutoLock syncLock(synObj);
        {    AutoLock syncLock(this);
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
            else if (FAILED(ec)) {
                Slogger::W(TAG, "Failed parsing, ec=%08x", ec);
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
        if (mLoadPrivLaterPkgs == NULL) {
            CSparseArray::New((ISparseArray**)&mLoadPrivLaterPkgs);
        }
        mLoadPrivLaterPkgs->Put(uid, CoreUtils::Convert(pkgName));
    }
    else {
        isPrivileged = StringUtils::ParseBoolean(isPrivilegedString);
    }

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(pkgName);
    IInterface* pkgNameObj = (IInterface*)csq.Get();
    Int32 outerDepth, depth;
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
            Int32 code = StringUtils::ParseInt32(str);
            // use op name string if it exists
            String codeNameStr;
            parser->GetAttributeValue(nullStr, String("ns"), &codeNameStr);
            if (!codeNameStr.IsNull()) {
                // returns OP_NONE if it could not be mapped
                AutoPtr<IAppOpsManagerHelper> aom;
                CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&aom);
                aom->NameToOp(codeNameStr, &code);
            }
            // skip op codes that are out of bounds
            if (code == IAppOpsManager::OP_NONE
                || code >= IAppOpsManager::_NUM_OP) {
                continue;
            }
            AutoPtr<Op> op = new Op(uid, pkgName, code, IAppOpsManager::MODE_ERRORED);

            String mode;
            parser->GetAttributeValue(nullStr, String("m"), &mode);
            if (!mode.IsNull()) {
                op->mMode = StringUtils::ParseInt32(mode);
            }
            else {
                String sDefualtMode;
                parser->GetAttributeValue(nullStr, String("dm"), &sDefualtMode);
                Int32 defaultMode = 0;
                if (!sDefualtMode.IsNull()) {
                    defaultMode = StringUtils::ParseInt32(sDefualtMode);
                }
                else {
                    defaultMode = GetDefaultMode(code, uid, pkgName);
                }
                op->mMode = defaultMode;
            }

            parser->GetAttributeValue(nullStr, String("t"), &str);
            if (!str.IsNull()) {
                op->mTime = StringUtils::ParseInt64(str);
            }
            parser->GetAttributeValue(nullStr, String("r"), &str);
            if (!str.IsNull()) {
                op->mRejectTime = StringUtils::ParseInt64(str);
            }
            parser->GetAttributeValue(nullStr, String("d"), &str);
            if (!str.IsNull()) {
                op->mDuration = StringUtils::ParseInt32(str);
            }
            String allowed;
            parser->GetAttributeValue(nullStr, String("ac"), &allowed);
            if (!allowed.IsNull()) {
                op->mAllowedCount = StringUtils::ParseInt32(allowed);
            }
            String ignored;
            parser->GetAttributeValue(nullStr, String("ic"), &ignored);
            if (!ignored.IsNull()) {
                op->mIgnoredCount = StringUtils::ParseInt32(ignored);
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
    {    AutoLock syncLock(synObj);
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
        Int32 size, opsSize, uid, dur, ival, mode, defaultMode, allowed, ignored;
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

                {
                    AutoLock lock(this);
                    AutoPtr<Ops> op = GetOpsLocked(uid, pkgName, FALSE);
                    // Should always be present as the list of PackageOps is generated
                    // from Ops.
                    if (op != NULL) {
                        FAIL_GOTO(out->WriteAttribute(nullStr, String("p"),
                            StringUtils::BooleanToString(op->mIsPrivileged)), _ERROR_)

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
                    defaultMode = GetDefaultMode(ival, uid, pkgName);
                    out->WriteAttribute(nullStr, String("n"), StringUtils::ToString(ival));
                    String name;
                    aom->OpToName(ival, &name);
                    out->WriteAttribute(nullStr, String("ns"), name);
                    op->GetMode(&mode);
                    if (mode != defaultMode) {
                        out->WriteAttribute(nullStr, String("m"), StringUtils::ToString(mode));
                    }
                    else {
                        out->WriteAttribute(nullStr, String("dm"), StringUtils::ToString(defaultMode));
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

                    op->GetAllowedCount(&allowed);
                    if (allowed != 0) {
                        out->WriteAttribute(nullStr, String("ac"), StringUtils::ToString(allowed));
                    }
                    op->GetIgnoredCount(&ignored);
                    if (ignored != 0) {
                        out->WriteAttribute(nullStr, String("ic"), StringUtils::ToString(ignored));
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

    // {    AutoLock syncLock(this);
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
    AutoPtr<IArrayOf> opRestrictions = IArrayOf::Probe(obj);
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
    *result = TRUE;

    if (mPolicy != NULL) {
        return mPolicy->IsControlAllowed(code, packageName, result);
    }
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

    {    AutoLock syncLock(this);
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

Boolean CAppOpsService::IsStrict(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 code,
    /* [in] */ const String& packageName)
{
    if (!mStrictEnable)
        return FALSE;

    return UserHandle::IsApp(uid);
}

Int32 CAppOpsService::GetDefaultMode(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    AutoPtr<IAppOpsManagerHelper> helper;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&helper);
    Int32 mode;
    helper->OpToDefaultMode(code, IsStrict(code, uid, packageName), &mode);

    Boolean bval;
    helper->IsStrictOp(code, &bval);
    if (bval && mPolicy != NULL) {
        Int32 policyMode;
        mPolicy->GetDefualtMode(code, packageName, &policyMode);
        if (policyMode != IAppOpsManager::MODE_ERRORED) {
            mode = policyMode;
        }
    }
    return mode;
}

AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> CAppOpsService::AskOperationLocked(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ CAppOpsService::Op* op)
{
    AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> request;
    request = new PermissionDialogReqQueue::PermissionDialogReq();
    AutoPtr<IRunnable> runnable = new AskRunnable(code, uid, packageName, op, request, this);
    Boolean bval;
    mHandler->Post(runnable, &bval);
    return request;
}

void CAppOpsService::PrintOperationLocked(
    /* [in] */ Op* op,
    /* [in] */ Int32 mode,
    /* [in] */ const String& operation)
{
    if(op != NULL) {
        AutoPtr<IAppOpsManagerHelper> helper;
        CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&helper);
        Int32 switchCode = 0;
        helper->OpToSwitch(op->mOp, &switchCode);
        if (mode == IAppOpsManager::MODE_IGNORED) {
            if (DEBUG) {
                Logger::D(TAG, "%s: reject #%d for code %d (%d) uid %d package %s",
                    operation.string(), mode, switchCode, op->mOp,
                    op->mUid, op->mPackageName.string());
            }
        }
        else if (mode == IAppOpsManager::MODE_ALLOWED) {
            if (DEBUG) {
                Logger::D(TAG, "%s: allowing code %d uid %d package %s",
                    operation.string(), op->mOp, op->mUid, op->mPackageName.string());
            }
        }
    }
}

void CAppOpsService::RecordOperationLocked(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    AutoPtr<Op> op = GetOpLocked(code, uid, packageName, FALSE);
    if(op != NULL) {
        if(op->mNoteOpCount != 0) {
            PrintOperationLocked(op, mode, String("noteOperartion"));
        }
        if(op->mStartOpCount != 0) {
            PrintOperationLocked(op, mode, String("startOperation"));
        }
        if (mode == IAppOpsManager::MODE_IGNORED) {
            sys->GetCurrentTimeMillis(&(op->mRejectTime));
        }
        else if (mode == IAppOpsManager::MODE_ALLOWED) {
            if(op->mNoteOpCount != 0) {
                sys->GetCurrentTimeMillis(&(op->mTime));
                op->mRejectTime = 0;
            }
            if(op->mStartOpCount != 0) {
                if(op->mNesting == 0) {
                    sys->GetCurrentTimeMillis(&(op->mTime));
                    op->mRejectTime = 0;
                    op->mDuration = -1;
                }
                op->mNesting = op->mNesting + op->mStartOpCount;
                Int32 tokSize = 0;
                op->mClientTokens->GetSize(&tokSize);
                while(tokSize != 0) {
                    AutoPtr<IInterface> clientToken;
                    op->mClientTokens->Get(0, (IInterface**)&clientToken);
                    AutoPtr<IInterface> pC;
                    mClients->Get(clientToken, (IInterface**)&pC);
                    AutoPtr<ClientState> client = (ClientState*)IProxyDeathRecipient::Probe(pC);
                    if (client != NULL) {
                        if (client->mStartedOps != NULL) {
                            client->mStartedOps->Add(ISynchronize::Probe(op));
                        }
                    }
                    op->mClientTokens->Remove(0);
                }
            }
        }
        op->mClientTokens->Clear();
        op->mStartOpCount = 0;
        op->mNoteOpCount = 0;
    }
}

ECode CAppOpsService::NotifyOperation(
    /* [in] */ Int32 code,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode,
    /* [in] */ Boolean remember)
{
    FAIL_RETURN(VerifyIncomingUid(uid));
    FAIL_RETURN(VerifyIncomingOp(code));
    AutoPtr<IAppOpsManagerHelper> helper;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&helper);
    AutoPtr<IArrayList> repCbs;
    Int32 switchCode = 0;
    helper->OpToSwitch(code, &switchCode);
    {    AutoLock syncLock(this);
        RecordOperationLocked(code, uid, packageName, mode);
        AutoPtr<Op> op = GetOpLocked(switchCode, uid, packageName, TRUE);
        if (op != NULL) {
            // Send result to all waiting client
            if (op->mDialogReqQueue->GetDialog() != NULL) {
                op->mDialogReqQueue->NotifyAll(mode);
                op->mDialogReqQueue->SetDialog(NULL);
            }
            if (remember && op->mMode != mode) {
                op->mMode = mode;
                AutoPtr<IInterface> pCbs;
                mOpModeWatchers->Get(switchCode, (IInterface**)&pCbs);
                AutoPtr<IArrayList> cbs = IArrayList::Probe(pCbs);
                if (cbs != NULL) {
                    if (repCbs == NULL) {
                        CArrayList::New((IArrayList**)&repCbs);
                    }
                    repCbs->AddAll(ICollection::Probe(cbs));
                }
                pCbs = NULL;
                mPackageModeWatchers->Get(CoreUtils::Convert(packageName), (IInterface**)&pCbs);
                cbs = IArrayList::Probe(pCbs);
                if (cbs != NULL) {
                    if (repCbs == NULL) {
                        CArrayList::New((IArrayList**)&repCbs);
                    }
                    repCbs->AddAll(ICollection::Probe(cbs));
                }
                if (mode == GetDefaultMode(op->mOp, op->mUid, op->mPackageName)) {
                    // If going into the default mode, prune this op
                    // if there is nothing else interesting in it.
                    PruneOp(op, uid, packageName);
                }
                ScheduleWriteNowLocked();
            }
        }
    }
    if (repCbs != NULL) {
        Int32 s = 0;
        repCbs->GetSize(&s);
        for (Int32 i = 0; i < s; i++) {
            AutoPtr<IInterface> p;
            repCbs->Get(i, (IInterface**)&p);
            AutoPtr<Callback> pCB = (Callback*)IProxyDeathRecipient::Probe(p);
            pCB->mCallback->OpChanged(switchCode, packageName);
        }
    }
    return NOERROR;
}

void CAppOpsService::BroadcastOpIfNeeded(
    /* [in] */ Int32 op)
{
    Boolean b = FALSE;
    switch (op) {
        case IAppOpsManager::OP_SU:
            mHandler->Post(mSuSessionChangedRunner, &b);
            break;
        default:
            break;
    }
}

void CAppOpsService::ReadPolicy()
{
    if (mStrictEnable) {
        AutoPtr<IFile> f;
        CFile::New(DEFAULT_POLICY_FILE, (IFile**)&f);
        AutoPtr<AppOpsPolicy> aop = new AppOpsPolicy();
        aop->constructor(f, mContext);
        aop->ReadPolicy();
        aop->DebugPoilcy();
        mPolicy = (IAppOpsPolicy*)aop.Get();
    }
    else {
        mPolicy = NULL;
    }
}

} // Server
} // Droid
} // Elastos
