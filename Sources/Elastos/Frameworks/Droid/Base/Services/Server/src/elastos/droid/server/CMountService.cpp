
#include "CMountService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/storage/StorageResultCode.h"
#include "pm/CPackageManagerService.h"
#include "util/Xml.h"
#include "util/XmlUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <unistd.h>

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IThread;
using Elastos::IO::CFile;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IObbScanner;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IUserEnvironment;
using Elastos::Droid::Os::CUserEnvironment;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Storage::CStorageVolume;
using Elastos::Droid::Os::Storage::IStorageVolumeHelper;
using Elastos::Droid::Os::Storage::CStorageVolumeHelper;
using Elastos::Droid::Os::Storage::StorageResultCode;
using Elastos::Droid::Os::Storage::IOnObbStateChangeListener;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Server::Pm::CPackageManagerService;
using Elastos::Droid::Server::Pm::CUserManagerService;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Droid::Internal::App::EIID_IMediaContainerService;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {

const Int32 CMountService::VolumeState::Init;
const Int32 CMountService::VolumeState::NoMedia;
const Int32 CMountService::VolumeState::Idle;
const Int32 CMountService::VolumeState::Pending;
const Int32 CMountService::VolumeState::Checking;
const Int32 CMountService::VolumeState::Mounted;
const Int32 CMountService::VolumeState::Unmounting;
const Int32 CMountService::VolumeState::Formatting;
const Int32 CMountService::VolumeState::Shared;
const Int32 CMountService::VolumeState::SharedMnt;

const Int32 CMountService::VoldResponseCode::VolumeListResult;
const Int32 CMountService::VoldResponseCode::AsecListResult;
const Int32 CMountService::VoldResponseCode::StorageUsersListResult;
const Int32 CMountService::VoldResponseCode::ShareStatusResult;
const Int32 CMountService::VoldResponseCode::AsecPathResult;
const Int32 CMountService::VoldResponseCode::ShareEnabledResult;
const Int32 CMountService::VoldResponseCode::OpFailedNoMedia;
const Int32 CMountService::VoldResponseCode::OpFailedMediaBlank;
const Int32 CMountService::VoldResponseCode::OpFailedMediaCorrupt;
const Int32 CMountService::VoldResponseCode::OpFailedVolNotMounted;
const Int32 CMountService::VoldResponseCode::OpFailedStorageBusy;
const Int32 CMountService::VoldResponseCode::OpFailedStorageNotFound;
const Int32 CMountService::VoldResponseCode::VolumeStateChange;
const Int32 CMountService::VoldResponseCode::VolumeDiskInserted;
const Int32 CMountService::VoldResponseCode::VolumeDiskRemoved;
const Int32 CMountService::VoldResponseCode::VolumeBadRemoval;

const Int32 CMountService::H_UNMOUNT_PM_UPDATE;
const Int32 CMountService::H_UNMOUNT_PM_DONE;
const Int32 CMountService::H_UNMOUNT_MS;
const Int32 CMountService::H_SYSTEM_READY;

const Int32 CMountService::OBB_RUN_ACTION;
const Int32 CMountService::OBB_MCS_BOUND;
const Int32 CMountService::OBB_MCS_UNBIND;
const Int32 CMountService::OBB_MCS_RECONNECT;
const Int32 CMountService::OBB_FLUSH_MOUNT_STATE;

//=================================================================
// CMountService::MountServiceHandler
//=================================================================
CMountService::MountServiceHandler::MountServiceHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CMountService* service)
    : HandlerBase(l)
    , mHost(service)
{}

ECode CMountService::MountServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMountService::H_UNMOUNT_PM_UPDATE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            UnmountCallBack* cb = (UnmountCallBack*)obj.Get();
            mHost->HandleUnmountPmUpdate(cb);
            break;
        }
        case CMountService::H_UNMOUNT_PM_DONE: {
            mHost->HandleUnmountPmDone();
            break;
        }
        case CMountService::H_UNMOUNT_MS: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            UnmountCallBack* cb = (UnmountCallBack*)obj.Get();
            mHost->HandleUnmountMs(cb);
            break;
        }
        case CMountService::H_SYSTEM_READY: {
            mHost->HandleSystemReady();
            break;
        }
    }
    return NOERROR;
}

//=================================================================
// CMountService::ObbActionHandler
//=================================================================
CMountService::ObbActionHandler::ObbActionHandler(
    /* [in] */ ILooper* l,
    /* [in] */ CMountService* service)
    : HandlerBase(l)
    , mHost(service)
{}

ECode CMountService::ObbActionHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMountService::OBB_RUN_ACTION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ObbAction* action = (ObbAction*)obj.Get();
            mHost->HandleObbRunAction(action);
            break;
        }
        case CMountService::OBB_MCS_BOUND: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMediaContainerService* service = IMediaContainerService::Probe(obj);
            mHost->HandleObbMcsBound(service);
            break;
        }
        case CMountService::OBB_MCS_RECONNECT: {
            mHost->HandleObbMcsReconnect();
            break;
        }
        case CMountService::OBB_MCS_UNBIND: {
            mHost->HandleObbMcsUnbind();
            break;
        }
        case CMountService::OBB_FLUSH_MOUNT_STATE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ICharSequence* seq = ICharSequence::Probe(obj);
            String path;
            seq->ToString(&path);
            mHost->HandleObbFlushMountState(path);
            break;
        }
    }
    return NOERROR;
}


//=================================================================
// CMountService::ObbState
//=================================================================
CMountService::ObbState::ObbState(
    /* [in] */ const String& rawPath,
    /* [in] */ const String& canonicalPath,
    /* [in] */ Int32 callingUid,
    /* [in] */ IIObbActionListener* token,
    /* [in] */ Int32 nonce,
    /* [in] */ CMountService* host)
    : mRawPath(rawPath)
    , mCanonicalPath(canonicalPath)
    , mToken(token)
    , mNonce(nonce)
    , mHost(host)
{
    Int32 userId = UserHandle::GetUserId(callingUid);
    mOwnerPath = BuildObbPath(canonicalPath, userId, FALSE);
    mVoldPath = BuildObbPath(canonicalPath, userId, TRUE);

    mOwnerGid = UserHandle::GetSharedAppGid(callingUid);
}

CAR_INTERFACE_IMPL(CMountService::ObbState, IProxyDeathRecipient)

AutoPtr<IBinder> CMountService::ObbState::GetBinder()
{
    return IBinder::Probe(mToken);
}

ECode CMountService::ObbState::ProxyDied()
{
    AutoPtr<ObbAction> action = new UnmountObbAction(this, TRUE, mHost);
    AutoPtr<IMessage> msg;
    mHost->mObbActionHandler->ObtainMessage(CMountService::OBB_RUN_ACTION, (IMessage**)&msg);
    msg->SetObj(action);
    Boolean result;
    return mHost->mObbActionHandler->SendMessage(msg, &result);
}

ECode CMountService::ObbState::Link()
{
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    if (proxy != NULL) proxy->LinkToDeath(this, 0);
    return NOERROR;
}

ECode CMountService::ObbState::Unlink()
{
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
    return NOERROR;
}

String CMountService::ObbState::ToString()
{
    StringBuilder sb("ObbState{");
    sb.Append("rawPath=");
    sb.Append(mRawPath);
    sb.Append(",canonicalPath=");
    sb.Append(mCanonicalPath);
    sb.Append(",ownerPath=");
    sb.Append(mOwnerPath);
    sb.Append(",voldPath=");
    sb.Append(mVoldPath);
    sb.Append(",ownerGid=");
    sb.Append(mOwnerGid);
    sb.Append(",token=");
    sb.AppendObject(mToken);
    sb.Append(",binder=");
    sb.AppendObject(GetBinder());
    sb.Append("}");
    return sb.ToString();
}


CMountService::DefaultContainerConnection::DefaultContainerConnection(
    /* [in] */ CMountService* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CMountService::DefaultContainerConnection, IServiceConnection);

ECode CMountService::DefaultContainerConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DEBUG_OBB)
        Slogger::I(CMountService::TAG, "onServiceConnected");

    AutoPtr<IMediaContainerService> imcs = IMediaContainerService::Probe(service);
    AutoPtr<IMessage> msg;
    mHost->mObbActionHandler->ObtainMessage(CMountService::OBB_MCS_BOUND, (IMessage**)&msg);
    msg->SetObj(imcs);
    Boolean result;
    return mHost->mObbActionHandler->SendMessage(msg, &result);
}

ECode CMountService::DefaultContainerConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DEBUG_OBB)
        Slogger::I(CMountService::TAG, "onServiceDisconnected");
    return NOERROR;
}


CAR_INTERFACE_IMPL(CMountService::UnmountCallBack, IInterface)

CMountService::UnmountCallBack::UnmountCallBack(
    /* [in] */ const String& path,
    /* [in] */ Boolean force,
    /* [in] */ Boolean removeEncryption,
    /* [in] */ CMountService* host)
    : mPath(path)
    , mForce(force)
    , mRemoveEncryption(removeEncryption)
    , mRetries(0)
    , mHost(host)
{}

void CMountService::UnmountCallBack::HandleFinished()
{
    if (DEBUG_UNMOUNT)
        Slogger::D(CMountService::TAG, "Unmounting %s", mPath.string());
    mHost->DoUnmountVolume(mPath, TRUE, mRemoveEncryption);
}


CMountService::UmsEnableCallBack::UmsEnableCallBack(
    /* [in] */ const String& path,
    /* [in] */ const String& method,
    /* [in] */ Boolean force,
    /* [in] */ CMountService* host)
    : UnmountCallBack(path, force, FALSE, host)
    , mMethod(method)
{}

void CMountService::UmsEnableCallBack::HandleFinished()
{
    UnmountCallBack::HandleFinished();
    mHost->DoShareUnshareVolume(mPath, mMethod, TRUE);
}


CMountService::ShutdownCallBack::ShutdownCallBack(
    /* [in] */ const String& path,
    /* [in] */ IIMountShutdownObserver* observer,
    /* [in] */ CMountService* host)
    : UnmountCallBack(path, TRUE, FALSE, host)
    , mObserver(observer)
{}

void CMountService::ShutdownCallBack::HandleFinished()
{
    Int32 ret = mHost->DoUnmountVolume(mPath, TRUE, mRemoveEncryption);
    if (mObserver != NULL) {
        // try {
        if (FAILED(mObserver->OnShutDownComplete(ret))) {
            Slogger::W(CMountService::TAG, "RemoteException when shutting down");
        }
        // } catch (RemoteException e) {
        //     Slog.w(TAG, "RemoteException when shutting down");
        // }
    }
}


CMountService::UserBroadcastReceiver::UserBroadcastReceiver(
    /* [in] */ CMountService* host)
    : mHost(host)
{}

ECode CMountService::UserBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userId);
    if (userId == -1) {
        return NOERROR;
    }
    AutoPtr<IUserHandle> user;
    FAIL_RETURN(CUserHandle::New(userId, (IUserHandle**)&user));

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_ADDED.Equals(action)) {
        AutoLock lock(mHost->mVolumesLock);
        mHost->CreateEmulatedVolumeForUserLocked(user);
    }
    else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        AutoLock lock(mHost->mVolumesLock);
        List<AutoPtr<IStorageVolume> > toRemove;
        List<AutoPtr<IStorageVolume> >::Iterator iter = mHost->mVolumes.Begin();
        for(; iter != mHost->mVolumes.End(); iter++) {
            AutoPtr<IStorageVolume> volume = *iter;
            AutoPtr<IUserHandle> owner;
            volume->GetOwner((IUserHandle**)&owner);
            Boolean equals;
            if(user->Equals(owner, &equals), equals) {
                toRemove.PushBack(volume);
            }
        }
        List<AutoPtr<IStorageVolume> >::Iterator iter2 = toRemove.Begin();
        for(; iter2 != toRemove.End(); iter2++) {
            mHost->RemoveVolumeLocked(*iter2);
        }
    }
    return NOERROR;
}


CMountService::UsbBroadcastReceiver::UsbBroadcastReceiver(
    /* [in] */ CMountService* host)
    : mHost(host)
{}

ECode CMountService::UsbBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean usbConnected;
    intent->GetBooleanExtra(IUsbManager::USB_CONNECTED, FALSE, &usbConnected);
    Boolean usbMassStorage;
    intent->GetBooleanExtra(IUsbManager::USB_FUNCTION_MASS_STORAGE, FALSE, &usbMassStorage);
    Boolean available = usbConnected && usbMassStorage;
    mHost->NotifyShareAvailabilityChange(available);
    return NOERROR;
}


CMountService::MountServiceBinderListener::MountServiceBinderListener(
    /* [in] */ IIMountServiceListener* listener,
    /* [in] */ CMountService* host)
    : mListener(listener)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(CMountService::MountServiceBinderListener, IProxyDeathRecipient)

ECode CMountService::MountServiceBinderListener::ProxyDied()
{
    if (LOCAL_LOGD) {
        Slogger::D(TAG, "An IIMountServiceListener has died!");
    }
    AutoLock lock(mHost->mListenersLock);
    AutoPtr<IProxy> proxy = (IProxy*)mListener->Probe(EIID_IProxy);
    assert(proxy != NULL);
    Boolean result;
    proxy->UnlinkToDeath(this, 0, &result);
    mHost->mListeners.Remove(this);
    return NOERROR;
}


CMountService::OnDaemonConnectedThread::OnDaemonConnectedThread(
    /* [in] */ const String& threadName,
    /* [in] */ CMountService* host)
    : mHost(host)
{
    Thread::constructor(threadName);
}

ECode CMountService::OnDaemonConnectedThread::Run()
{
    /**
     * Determine media state and UMS detection status
     */
    String path, state, volstr;
    AutoPtr< ArrayOf<String> > vols;
    AutoPtr< ArrayOf<String> > tok;
    Int32 st, i;
    AutoPtr<IStorageVolume> volume;

    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String("list"), (ICharSequence**)&cs);
    args->Set(0, cs);
    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    ECode ec = mHost->mConnector->ExecuteForList(String("volume"), args, (ArrayOf<NativeDaemonEvent*>**)&events);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error processing initial volume state 0x%08x", ec);
        AutoPtr<IStorageVolume> primary = mHost->GetPrimaryPhysicalVolume();
        if (primary != NULL) {
            mHost->UpdatePublicVolumeState(primary, IEnvironment::MEDIA_REMOVED);
        }
        goto _EXIT_;
    }

    vols = NativeDaemonEvent::FilterMessageList(
            *events, VoldResponseCode::VolumeListResult);
    for (i = 0; i < vols->GetLength(); i++) {
        volstr = (*vols)[i];
        tok = NULL;
        StringUtils::Split(volstr, String(" "), (ArrayOf<String>**)&tok);
        assert(tok != NULL);

        // FMT: <label> <mountpoint> <state>
        path = (*tok)[1];
        state = IEnvironment::MEDIA_REMOVED;

        {
            AutoLock lock(mHost->mVolumesLock);
            HashMap<String, AutoPtr<IStorageVolume> >::Iterator it = mHost->mVolumesByPath.Find(path);
            if (it != mHost->mVolumesByPath.End()) {
                volume = it->mSecond;
            }
        }

        st = StringUtils::ParseInt32((*tok)[2]);
        if (st == VolumeState::NoMedia) {
            state = IEnvironment::MEDIA_REMOVED;
        }
        else if (st == VolumeState::Idle) {
            state = IEnvironment::MEDIA_UNMOUNTED;
        }
        else if (st == VolumeState::Mounted) {
            state = IEnvironment::MEDIA_MOUNTED;
            Slogger::I(CMountService::TAG, String("Media already mounted on daemon connection"));
        }
        else if (st == VolumeState::Shared) {
            state = IEnvironment::MEDIA_SHARED;
            Slogger::I(CMountService::TAG, String("Media shared on daemon connection"));
        } else {
            // throw new Exception(String.format("Unexpected state %d", st));
            Slogger::E(CMountService::TAG, "Unexpected state %d", st);
            Slogger::E(TAG, "Error processing initial volume state 0x%08x", ec);
            AutoPtr<IStorageVolume> primary = mHost->GetPrimaryPhysicalVolume();
            if (primary != NULL) {
                mHost->UpdatePublicVolumeState(primary, IEnvironment::MEDIA_REMOVED);
            }
            goto _EXIT_;
        }

        if (!state.IsNull()) {
            if (DEBUG_EVENTS) Slogger::I(CMountService::TAG, "Updating valid state %s", (const char*)state);
            mHost->UpdatePublicVolumeState(volume, state);
        }
    }
    // } catch (Exception e) {
    //     Slog.e(TAG, "Error processing initial volume state", e);
    //     if (path != NULL)
    //         UpdatePublicVolumeState(path, IEnvironment::MEDIA_REMOVED);
    // }

_EXIT_:
    /*
     * Now that we've done our initialization, release
     * the hounds!
     */
    mHost->mConnectedSignal->CountDown();

    // Let package manager load internal ASECs.
    mHost->mPms->ScanAvailableAsecs();

    // Notify people waiting for ASECs to be scanned that it's done.
    mHost->mAsecsScanned->CountDown();

    return NOERROR;
}

CMountService::OnEventThread::OnEventThread(
    /* [in] */ const String& path,
    /* [in] */ CMountService* host)
    : mPath(path)
    , mHost(host)
{
    Thread::constructor();
}


ECode CMountService::OnEventThread::Run()
{
    // try {
    Int32 rc;
    if ((rc = mHost->DoMountVolume(mPath)) != StorageResultCode::OperationSucceeded) {
        Slogger::W(TAG, "Insertion mount failed (%d)", rc);
    }
    // } catch (Exception ex) {
    //     Slog.w(TAG, "Failed to mount media on insertion", ex);
    // }
    return NOERROR;
}


CMountService::UsbMassStorageThread::UsbMassStorageThread(
    /* [in] */ const String& path,
    /* [in] */ CMountService* host)
    : mPath(path)
    , mHost(host)
{
    Thread::constructor();
}

ECode CMountService::UsbMassStorageThread::Run()
{
    // try {
    Int32 rc = 0;
    Slogger::W(TAG, "Disabling UMS after cable disconnect");
    mHost->DoShareUnshareVolume(mPath, String("ums"), FALSE);
    if ((rc = mHost->DoMountVolume(mPath)) != StorageResultCode::OperationSucceeded) {
        Slogger::E(TAG, "Failed to remount {%s} on UMS enabled-disconnect (%d)", mPath.string(), rc);
    }
    // } catch (Exception ex) {
    //     Slog.w(TAG, "Failed to mount media on UMS enabled-disconnect", ex);
    // }

    return NOERROR;
}

const Int32 CMountService::ObbAction::MAX_RETRIES;

CAR_INTERFACE_IMPL(CMountService::ObbAction, IInterface)

CMountService::ObbAction::ObbAction(
    /* [in] */ ObbState* obbState,
    /* [in] */ CMountService* host)
    : mObbState(obbState)
    , mHost(host)
    , mRetries(0)
{}

void CMountService::ObbAction::Execute(
    /* [in] */ IHandler* handler)
{
    // try {
    if (DEBUG_OBB)
        Slogger::I(TAG, "Starting to execute action: %s", ToString().string());
    mRetries++;
    if (mRetries > MAX_RETRIES) {
        Slogger::W(TAG, "Failed to invoke remote methods on default container service. Giving up");
        Boolean result;
        mHost->mObbActionHandler->SendEmptyMessage(CMountService::OBB_MCS_UNBIND, &result);
        HandleError();
        return;
    }
    else {
        ECode ec = HandleExecute();
        if (FAILED(ec)) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Boolean result;
                mHost->mObbActionHandler->SendEmptyMessage(CMountService::OBB_MCS_RECONNECT, &result);
            }
            else {
                HandleError();
                Boolean result;
                mHost->mObbActionHandler->SendEmptyMessage(CMountService::OBB_MCS_UNBIND, &result);
            }
        }
        else {
            Boolean result;
            mHost->mObbActionHandler->SendEmptyMessage(CMountService::OBB_MCS_UNBIND, &result);
        }
    }
    // } catch (RemoteException e) {
    //     if (DEBUG_OBB)
    //         Slog.i(TAG, "Posting install MCS_RECONNECT");
    //     mObbActionHandler.sendEmptyMessage(OBB_MCS_RECONNECT);
    // } catch (Exception e) {
    //     if (DEBUG_OBB)
    //         Slog.d(TAG, "Error handling OBB action", e);
    //     handleError();
    //     mObbActionHandler.sendEmptyMessage(OBB_MCS_UNBIND);
    // }
}

ECode CMountService::ObbAction::GetObbInfo(
    /* [out] */ IObbInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoPtr<IObbInfo> obbInfo;
    // try {
    if (FAILED(mHost->mContainerService->GetObbInfo(mObbState->mOwnerPath, (IObbInfo**)&obbInfo))) {
        Slogger::D(TAG, "Couldn't call DefaultContainerService to fetch OBB info for %s"
                , mObbState->mOwnerPath.string());
        obbInfo = NULL;
    }
    // } catch (RemoteException e) {
    //     Slog.d(TAG, "Couldn't call DefaultContainerService to fetch OBB info for "
    //             + mObbState.ownerPath);
    //     obbInfo = null;
    // }
    if (obbInfo == NULL) {
        Slogger::E(TAG, "Couldn't read OBB file: %s", mObbState->mOwnerPath.string());
        return E_IO_EXCEPTION;
        // throw new IOException("Couldn't read OBB file: " + mObbState.ownerPath);
    }
    *info = obbInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CMountService::ObbAction::SendNewStatusOrIgnore(
    /* [in] */ Int32 status)
{
    if (mObbState == NULL || mObbState->mToken == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = mObbState->mToken->OnObbResult(mObbState->mRawPath, mObbState->mNonce, status);
    if (FAILED(ec)) {
        Slogger::W(TAG, "MountServiceListener went away while calling onObbStateChanged");
        return ec;
    }
    // } catch (RemoteException e) {
    //     Slog.w(TAG, "MountServiceListener went away while calling onObbStateChanged");
    // }
    return NOERROR;
}


CMountService::MountObbAction::MountObbAction(
    /* [in] */ ObbState* obbState,
    /* [in] */ const String& key,
    /* [in] */ Int32 callingUid,
    /* [in] */ CMountService* host)
   : ObbAction(obbState, host)
   , mKey(key)
   , mCallingUid(callingUid)
{}

ECode CMountService::MountObbAction::HandleExecute()
{
    mHost->WaitForReady();
    mHost->WarnOnNotMounted();

    AutoPtr<IObbInfo> obbInfo;
    FAIL_RETURN(GetObbInfo((IObbInfo**)&obbInfo));

    String fileName, packageName;
    obbInfo->GetFilename(&fileName);
    obbInfo->GetPackageName(&packageName);
    if (!mHost->IsUidOwnerOfPackageOrSystem(packageName, mCallingUid)) {
        Slogger::W(TAG, "Denied attempt to mount OBB %s which is owned by %s",
                fileName.string(), packageName.string());
        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_PERMISSION_DENIED);
    }

    Boolean isMounted;
    {
        AutoLock lock(mHost->mObbMountsLock);
        HashMap<String, AutoPtr<ObbState> >::Iterator iter = mHost->mObbPathToStateMap.Find(mObbState->mRawPath);
        isMounted = iter != mHost->mObbPathToStateMap.End();
    }
    if (isMounted) {
        Slogger::W(TAG, "Attempt to mount OBB which is already mounted: ", (const char*)fileName);
        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_ALREADY_MOUNTED);
    }

    String hashedKey;
    if (mKey.IsNull()) {
        hashedKey = String("none");
    }
    else {
        assert(0);
        // try {
        // SecretKeyFactory factory = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA1");

        // KeySpec ks = new PBEKeySpec(mKey.toCharArray(), obbInfo.salt,
        //         PBKDF2_HASH_ROUNDS, CRYPTO_ALGORITHM_KEY_SIZE);
        // SecretKey key = factory.generateSecret(ks);
        // BigInteger bi = new BigInteger(key.getEncoded());
        // hashedKey = bi.toString(16);
        // } catch (NoSuchAlgorithmException e) {
        //     Slog.e(TAG, "Could not load PBKDF2 algorithm", e);
        //     sendNewStatusOrIgnore(OnObbStateChangeListener.ERROR_INTERNAL);
        //     return;
        // } catch (InvalidKeySpecException e) {
        //     Slog.e(TAG, "Invalid key spec when loading PBKDF2 algorithm", e);
        //     sendNewStatusOrIgnore(OnObbStateChangeListener.ERROR_INTERNAL);
        //     return;
        // }
    }

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    AutoPtr<ICharSequence> cs1, cs2, cs3, cs4;
    CString::New(String("mount"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(mObbState->mVoldPath, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    CString::New(hashedKey, (ICharSequence**)&cs3);
    args->Set(2, cs3);
    CString::New(StringUtils::Int32ToString(mObbState->mOwnerGid), (ICharSequence**)&cs4);
    args->Set(3, cs4);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mHost->mConnector->Execute(String("obb"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code != VoldResponseCode::OpFailedStorageBusy) {
            rc = StorageResultCode::OperationFailedInternalError;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code != VoldResponseCode.OpFailedStorageBusy) {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        if (DEBUG_OBB)
            Slogger::D(TAG, "Successfully mounted OBB %s", (const char*)mObbState->mVoldPath);

        {
            AutoLock lock(mHost->mObbMountsLock);
            mHost->AddObbStateLocked(mObbState);
        }

        return SendNewStatusOrIgnore(IOnObbStateChangeListener::MOUNTED);
    }
    else {
        Slogger::E(TAG, "Couldn't mount OBB file: %d", rc);

        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_COULD_NOT_MOUNT);
    }
}

void CMountService::MountObbAction::HandleError()
{
    SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_INTERNAL);
}

String CMountService::MountObbAction::ToString()
{
    StringBuilder sb;
    sb.Append("MountObbAction{");
    sb.Append(mObbState->ToString());
    sb.AppendChar('}');
    return sb.ToString();
}


CMountService::UnmountObbAction::UnmountObbAction(
    /* [in] */ ObbState* obbState,
    /* [in] */ Boolean force,
    /* [in] */ CMountService* host)
    : ObbAction(obbState, host)
    , mForceUnmount(force)
{}

ECode CMountService::UnmountObbAction::HandleExecute()
{
    mHost->WaitForReady();
    mHost->WarnOnNotMounted();

    AutoPtr<IObbInfo> obbInfo;
    FAIL_RETURN(GetObbInfo((IObbInfo**)&obbInfo));

    AutoPtr<ObbState> existingState;
    {
        AutoLock lock(mHost->mObbMountsLock);
        HashMap<String, AutoPtr<ObbState> >::Iterator iter = mHost->mObbPathToStateMap.Find(mObbState->mRawPath);
        if(iter != mHost->mObbPathToStateMap.End()) {
            existingState = iter->mSecond;
        }
    }

    if (existingState == NULL) {
        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_NOT_MOUNTED);
    }

    if (existingState->mOwnerGid != mObbState->mOwnerGid) {
        Slogger::W(TAG, "Permission denied attempting to unmount OBB %s (owned by GID %d)",
                existingState->mRawPath.string(), existingState->mOwnerGid);
        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_PERMISSION_DENIED);
    }

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("unmount"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(mObbState->mVoldPath, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(String("obb"), args);
    if (mForceUnmount) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("force"), (ICharSequence**)&cs);
        cmd->AppendArg(cs);
    }
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mHost->mConnector->Execute(cmd, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageBusy) {
            rc = StorageResultCode::OperationFailedStorageBusy;
        }
        else if (code == VoldResponseCode::OpFailedStorageNotFound) {
            // If it's not mounted then we've already won.
            rc = StorageResultCode::OperationSucceeded;
        }
        else {
            rc = StorageResultCode::OperationFailedInternalError;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageBusy) {
    //         rc = StorageResultCode.OperationFailedStorageBusy;
    //     } else if (code == VoldResponseCode.OpFailedStorageNotFound) {
    //         // If it's not mounted then we've already won.
    //         rc = StorageResultCode.OperationSucceeded;
    //     } else {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        {
            AutoLock lock(mHost->mObbMountsLock);
            mHost->RemoveObbStateLocked(existingState);
        }

        return SendNewStatusOrIgnore(IOnObbStateChangeListener::UNMOUNTED);
    }
    else {
        Slogger::W(TAG, "Could not unmount OBB: %p", existingState.Get());
        return SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_COULD_NOT_UNMOUNT);
    }
}

void CMountService::UnmountObbAction::HandleError()
{
    SendNewStatusOrIgnore(IOnObbStateChangeListener::ERROR_INTERNAL);
}

String CMountService::UnmountObbAction::ToString()
{
    StringBuilder sb("UnmountObbAction{");
    sb.Append(mObbState->ToString());
    sb.Append(",force=");
    sb.AppendBoolean(mForceUnmount);
    sb.AppendChar('}');
    return sb.ToString();
}

static AutoPtr<IComponentName> InitDefaultComponet()
{
    AutoPtr<IComponentName> cn;
    CComponentName::New(
            String("com.android.defcontainer"),
            String("com.android.defcontainer.DefaultContainerService"),
            (IComponentName**)&cn);

    return cn;
}

const Boolean CMountService::LOCAL_LOGD = FALSE;
const Boolean CMountService::DEBUG_UNMOUNT = FALSE;
const Boolean CMountService::DEBUG_EVENTS = FALSE;
const Boolean CMountService::DEBUG_OBB = FALSE;
const Boolean CMountService::WATCHDOG_ENABLE = FALSE;
const String CMountService::TAG("MountService");
const String CMountService::VOLD_TAG("VoldConnector");
const Int32 CMountService::MAX_CONTAINERS;
const Int32 CMountService::CRYPTO_ALGORITHM_KEY_SIZE;
const Int32 CMountService::PBKDF2_HASH_ROUNDS;
const AutoPtr<IComponentName> CMountService::DEFAULT_CONTAINER_COMPONENT = InitDefaultComponet();
const Int32 CMountService::RETRY_UNMOUNT_DELAY; // in ms
const Int32 CMountService::MAX_UNMOUNT_RETRIES;
const String CMountService::TAG_STORAGE_LIST("StorageList");
const String CMountService::TAG_STORAGE("storage");

CMountService::CMountService()
    : mSystemReady(FALSE)
    , mUmsEnabling(FALSE)
    , mUmsAvailable(FALSE)
    , mSendUmsConnectedOnBoot(FALSE)
    , mUpdatingStatus(FALSE)
    , mBound(FALSE)
{
    ASSERT_SUCCEEDED(CCountDownLatch::New(1, (ICountDownLatch**)&mConnectedSignal));
    ASSERT_SUCCEEDED(CCountDownLatch::New(1, (ICountDownLatch**)&mAsecsScanned));

    mDefContainerConn = new DefaultContainerConnection(this);

    mUserReceiver = (IBroadcastReceiver*)new UserBroadcastReceiver(this);
    mUsbReceiver = (IBroadcastReceiver*)new UsbBroadcastReceiver(this);
}

CMountService::~CMountService()
{
    mObbMounts.Clear();

    mVolumesByPath.Clear();
    mVolumeStates.Clear();
    mListeners.Clear();
    mAsecMountSet.Clear();
    mObbPathToStateMap.Clear();
    mForceUnmounts.Clear();
    mActions.Clear();
}

// for MountServiceHandler
void CMountService::HandleUnmountPmUpdate(
    /* [in] */ UnmountCallBack* ucb)
{
    if (DEBUG_UNMOUNT)
        Slogger::I(TAG, "H_UNMOUNT_PM_UPDATE");

    mForceUnmounts.PushBack(ucb);
    if (DEBUG_UNMOUNT)
        Slogger::I(TAG, " registered = %d", mUpdatingStatus);
    // Register only if needed.
    if (!mUpdatingStatus) {
        if (DEBUG_UNMOUNT)
            Slogger::I(TAG, "Updating external media status on PackageManager");
        mUpdatingStatus = TRUE;
        mPms->UpdateExternalMediaStatus(FALSE, TRUE);
    }
}

void CMountService::HandleUnmountPmDone()
{
    if (DEBUG_UNMOUNT) Slogger::I(TAG, "H_UNMOUNT_PM_DONE");
    if (DEBUG_UNMOUNT) Slogger::I(TAG, "Updated status. Processing requests");
    mUpdatingStatus = FALSE;

    Int32 size = mForceUnmounts.GetSize();
    AutoPtr< ArrayOf<List<AutoPtr<UnmountCallBack> >::Iterator> > sizeArr
            = ArrayOf<List<AutoPtr<UnmountCallBack> >::Iterator>::Alloc(size);
    Int32 sizeArrN = 0;
    // Kill processes holding references first
    AutoPtr<IIActivityManager> ams = IIActivityManager::Probe(ServiceManager::GetService(String("activity")));
    List<AutoPtr<UnmountCallBack> >::Iterator iter;
    for (iter = mForceUnmounts.Begin(); iter != mForceUnmounts.End(); ++iter) {
        AutoPtr<UnmountCallBack> ucb = *iter;
        String path = ucb->mPath;
        Boolean done = FALSE;
        if (!ucb->mForce) {
            done = TRUE;
        }
        else {
            AutoPtr< ArrayOf<Int32> > pids;
            GetStorageUsers(path, (ArrayOf<Int32>**)&pids);
            if (pids == NULL || pids->GetLength() == 0) {
                done = TRUE;
            }
            else {
                // Eliminate system process here?
                Boolean result;
                if(ams) {
                    ams->KillPids(pids, String("unmount media"), TRUE, &result);
                }
                // Confirm if file references have been freed.
                pids = NULL;
                GetStorageUsers(path, (ArrayOf<Int32>**)&pids);
                if (pids == NULL || pids->GetLength() == 0) {
                    done = TRUE;
                }
            }
        }
        if (!done && (ucb->mRetries < MAX_UNMOUNT_RETRIES)) {
            // Retry again
            Slogger::I(TAG, "Retrying to kill storage users again");
            ucb->mRetries++;

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(H_UNMOUNT_PM_DONE, (IMessage**)&msg);
            msg->SetArg1(ucb->mRetries++);
            Boolean result;
            mHandler->SendMessageDelayed(msg, RETRY_UNMOUNT_DELAY, &result);
        }
        else {
            if (ucb->mRetries >= MAX_UNMOUNT_RETRIES) {
                Slogger::I(TAG, "Failed to unmount media inspite of %d retries. Forcibly killing processes now",
                        MAX_UNMOUNT_RETRIES);
            }
            (*sizeArr)[sizeArrN++] = iter;

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(H_UNMOUNT_MS, (IMessage**)&msg);
            msg->SetObj(ucb);
            Boolean result;
            mHandler->SendMessage(msg, &result);
        }
    }
    // Remove already processed elements from list.
    for (Int32 i = sizeArrN - 1; i >= 0; i--) {
        mForceUnmounts.Erase((*sizeArr)[i]);
    }
}

void CMountService::HandleUnmountMs(
    /* [in] */ UnmountCallBack* ucb)
{
    if (DEBUG_UNMOUNT) Slogger::I(TAG, "H_UNMOUNT_MS %s", ucb != NULL ? ucb->mPath.string() : "?");
    ucb->HandleFinished();
}

void CMountService::WaitForAsecScan()
{
    WaitForLatch(mAsecsScanned);
}

void CMountService::WaitForReady()
{
    WaitForLatch(mConnectedSignal);
}

void CMountService::WaitForLatch(
    /* [in] */ ICountDownLatch* latch)
{
    PFL_EX("TODO: WaitForLatch need CCountDownLatch::AwaitEx")
    return;

    for (;;) {
        // try {
        Boolean res = FALSE;
        latch->Await(5000, NULL/*TimeUnit::MILLISECONDS*/, &res);
        if (res) {
            return;
        }
        else {
            String name;
            Thread::GetCurrentThread()->GetName(&name);
            Slogger::W(TAG, "Thread %s still waiting for MountService ready...", (const char*)name);
        }
        // } catch (InterruptedException e) {
        //     Slog.w(TAG, "Interrupt while waiting for MountService to be ready.");
        // }
    }
}

void CMountService::HandleSystemReady()
{
    // Snapshot current volume states since it's not safe to call into vold
    // while holding locks.
    HashMap<String, String> snapshot(30);
    {
        AutoLock lock(mVolumesLock);
        HashMap<String, String>::Iterator it = mVolumeStates.Begin();
        for(; it != mVolumeStates.End(); ++it) {
            snapshot[it->mFirst] = it->mSecond;
        }
    }

    HashMap<String, String>::Iterator iter = snapshot.Begin();
    for (; iter != snapshot.End(); ++iter) {
        String path = iter->mFirst;
        String state = iter->mSecond;

        if (state.Equals(IEnvironment::MEDIA_UNMOUNTED)) {
            Int32 rc = DoMountVolume(path);
            if (rc != StorageResultCode::OperationSucceeded) {
                Slogger::E(TAG, "Boot-time mount failed (%d)", rc);
            }
        }
        else if (state.Equals(IEnvironment::MEDIA_SHARED)) {
            /*
             * Bootstrap UMS enabled state since vold indicates
             * the volume is shared (runtime restart while ums enabled)
             */
            NotifyVolumeStateChange(String(NULL), path, VolumeState::NoMedia,
                    VolumeState::Shared);
        }
    }

    // Push mounted state for all emulated storage
    {
        AutoLock lock(mVolumesLock);
        List<AutoPtr<IStorageVolume> >::Iterator iter;
        for (iter = mVolumes.Begin(); iter != mVolumes.End(); ++iter) {
            AutoPtr<IStorageVolume> volume = *iter;
            Boolean isEmulated;
            if (volume->IsEmulated(&isEmulated), isEmulated) {
                UpdatePublicVolumeState(volume, IEnvironment::MEDIA_MOUNTED);
            }
        }
    }

    /*
     * If UMS was connected on boot, send the connected event
     * now that we're up.
     */
    if (mSendUmsConnectedOnBoot) {
        SendUmsIntent(TRUE);
        mSendUmsConnectedOnBoot = FALSE;
    }
}

ECode CMountService::DoShareUnshareVolume(
    /* [in] */ const String& path,
    /* [in] */ const String& method,
    /* [in] */ Boolean enable)
{
    // TODO: Add support for multiple share methods
    if (!method.Equals("ums")) {
        Slogger::E(TAG, "Method %s not supported", method.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> cs1, cs2, cs3;
    CString::New(enable ? String("share") : String("unshare"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    CString::New(method, (ICharSequence**)&cs3);
    args->Set(2, (IInterface*)cs3);
    AutoPtr<NativeDaemonEvent> event;
    ECode ec = mConnector->Execute(String("volume"), args, (NativeDaemonEvent**)&event);
    if (ec == (ECode)E_NATIVE_DAEMON_CONNECTOR_EXCEPTION) {
       Slogger::E(TAG, "Failed to share/unshare");
    }

    return NOERROR;
}

void CMountService::UpdatePublicVolumeState(
    /* [in] */ IStorageVolume* volume,
    /* [in] */ const String& state)
{
    String path;
    volume->GetPath(&path);

    String oldState;
    {
        AutoLock lock(mVolumesLock);

        HashMap<String, String>::Iterator iter = mVolumeStates.Find(path);
        if(iter != mVolumeStates.End()) {
            oldState = iter->mSecond;
        }
        mVolumeStates[path] = state;
    }

    if (state.Equals(oldState)) {
        Slogger::W(TAG, "Duplicate state transition (%s -> %s) for %s",
                state.string(), state.string(), path.string());
        return;
    }

    Slogger::D(TAG, "volume state changed for %s (%s -> %s)", path.string(), oldState.string(), state.string());

    // Tell PackageManager about changes to primary volume state, but only
    // when not emulated.
    Boolean result1, result2;
    if ((volume->IsPrimary(&result1), result1) && (volume->IsEmulated(&result2), !result2)) {
        if (IEnvironment::MEDIA_UNMOUNTED.Equals(state)) {
            mPms->UpdateExternalMediaStatus(FALSE, FALSE);

            /*
             * Some OBBs might have been unmounted when this volume was
             * unmounted, so send a message to the handler to let it know to
             * remove those from the list of mounted OBBS.
             */
            AutoPtr<ICharSequence> seq;
            CString::New(path, (ICharSequence**)&seq);
            AutoPtr<IMessage> msg;
            mObbActionHandler->ObtainMessage(OBB_FLUSH_MOUNT_STATE, (IMessage**)&msg);
            msg->SetObj(seq);
            Boolean result;
            mObbActionHandler->SendMessage(msg, &result);
        }
        else if (IEnvironment::MEDIA_MOUNTED.Equals(state)) {
            mPms->UpdateExternalMediaStatus(TRUE, FALSE);
        }
    }

    AutoLock lock(mListenersLock);

    List<AutoPtr<MountServiceBinderListener> >::ReverseIterator rit;
    for(rit = mListeners.RBegin(); rit != mListeners.REnd();) {
        AutoPtr<MountServiceBinderListener> bl = *rit;
        // try {
        ECode ec = bl->mListener->OnStorageStateChanged(path, oldState, state);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Listener dead");
            rit = List<AutoPtr<MountServiceBinderListener> >::ReverseIterator(mListeners.Erase(--(rit.GetBase())));
        }
        else {
            ++rit;
        }
        // } catch (RemoteException rex) {
        //     Slog.e(TAG, "Listener dead");
        //     mListeners.remove(i);
        // } catch (Exception ex) {
        //     Slog.e(TAG, "Listener failed", ex);
        // }
    }
}

ECode CMountService::OnDaemonConnected()
{
    /*
     * Since we'll be calling back into the NativeDaemonConnector,
     * we need to do our work in a new thread.
     */
    AutoPtr<OnDaemonConnectedThread> t = new OnDaemonConnectedThread(String("MountService#onDaemonConnected"), this);
    return t->Start();
}

ECode CMountService::OnEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& raw,
    /* [in] */ const ArrayOf<String>& cooked,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG_EVENTS) {
        StringBuilder builder("OnEvent::");
        builder.Append(" raw= ");
        builder.AppendString(raw);
        //if (cooked != NULL) {
        builder.Append(" cooked = " );
        for (Int32 i = 0; i < cooked.GetLength(); i++) {
                builder.Append(" ");
                builder.AppendString(cooked[i]);
        }
        //}
        Slogger::I(TAG, builder.ToString());
    }

    if (code == VoldResponseCode::VolumeStateChange) {
        /*
         * One of the volumes we're managing has changed state.
         * Format: "NNN Volume <label> <path> state changed
         * from <old_#> (<old_str>) to <new_#> (<new_str>)"
         */
        NotifyVolumeStateChange(cooked[2], cooked[3],
            StringUtils::ParseInt32(cooked[7]),
            StringUtils::ParseInt32(cooked[10]));
    }
    else if ((code == VoldResponseCode::VolumeDiskInserted) ||
            (code == VoldResponseCode::VolumeDiskRemoved) ||
            (code == VoldResponseCode::VolumeBadRemoval)) {
        // FMT: NNN Volume <label> <mountpoint> disk inserted (<major>:<minor>)
        // FMT: NNN Volume <label> <mountpoint> disk removed (<major>:<minor>)
        // FMT: NNN Volume <label> <mountpoint> bad removal (<major>:<minor>)
        String action;
        String label = cooked[2];
        String path = cooked[3];
        Int32 major = -1;
        Int32 minor = -1;

        // try {
        String devComp = cooked[6].Substring(1, cooked[6].GetLength()-1);
        AutoPtr< ArrayOf<String> > devTok;
        if (SUCCEEDED(StringUtils::Split(devComp, String(":"), (ArrayOf<String>**)&devTok))) {
            major = StringUtils::ParseInt32((*devTok)[0]);
            minor = StringUtils::ParseInt32((*devTok)[1]);
        }
        else {
            Slogger::E(TAG, "Failed to parse major/minor");
        }
        // } catch (Exception ex) {
        //     Slog.e(TAG, "Failed to parse major/minor", ex);
        // }

        AutoPtr<IStorageVolume> volume;
        String state;
        {
            AutoLock lock(mVolumesLock);
            volume = mVolumesByPath[path];
            state = mVolumeStates[path];
        }

        if (code == VoldResponseCode::VolumeDiskInserted) {
            AutoPtr<OnEventThread> thread = new OnEventThread(path, this);
            thread->Start();
        }
        else if (code == VoldResponseCode::VolumeDiskRemoved) {
            /*
             * This event gets trumped if we're already in BAD_REMOVAL state
             */
            String tempState;
            GetVolumeState(path, &tempState);
            if (tempState.Equals(IEnvironment::MEDIA_BAD_REMOVAL)) {
                *result = TRUE;
                return NOERROR;
            }
            /* Send the media unmounted event first */
            if (DEBUG_EVENTS) {
                Slogger::I(TAG, "Sending unmounted event first");
            }
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_UNMOUNTED);
            SendStorageIntent(IEnvironment::MEDIA_UNMOUNTED, volume, UserHandle::ALL);

            if (DEBUG_EVENTS) {
                Slogger::I(TAG, "Sending media removed");
            }

            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_REMOVED);
            action = IIntent::ACTION_MEDIA_REMOVED;
        }
        else if (code == VoldResponseCode::VolumeBadRemoval) {
            if (DEBUG_EVENTS) {
                Slogger::I(TAG, "Sending unmounted event first");
            }
            /* Send the media unmounted event first */
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_UNMOUNTED);
            action = IIntent::ACTION_MEDIA_UNMOUNTED;

            if (DEBUG_EVENTS) {
                Slogger::I(TAG, "Sending media bad removal");
            }
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_BAD_REMOVAL);
            action = IIntent::ACTION_MEDIA_BAD_REMOVAL;
        }
        else {
            Slogger::E(TAG, "Unknown code {%d}", code);
        }

        if (!action.IsNull()) {
            SendStorageIntent(action, volume, UserHandle::ALL);
        }
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

void CMountService::NotifyVolumeStateChange(
    /* [in] */ const String& label,
    /* [in] */ const String& path,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    AutoPtr<IStorageVolume> volume;
    String state;
    {
        volume = mVolumesByPath[path];
        GetVolumeState(path, &state);
    }

    if (DEBUG_EVENTS) {
        Slogger::I(TAG, "notifyVolumeStateChange =%s", state.string());
    }

    String action;

    if (oldState == VolumeState::Shared && newState != oldState) {
        if (LOCAL_LOGD) {
            Slogger::D(TAG, "Sending ACTION_MEDIA_UNSHARED intent");
        }
        SendStorageIntent(IIntent::ACTION_MEDIA_UNSHARED, volume, UserHandle::ALL);
    }

    if (newState == VolumeState::Init) {
        //
    }
    else if (newState == VolumeState::NoMedia) {
        // NoMedia is handled via Disk Remove events
    }
    else if (newState == VolumeState::Idle) {
        /*
         * Don't notify if we're in BAD_REMOVAL, NOFS, UNMOUNTABLE, or
         * if we're in the process of enabling UMS
         */
        if (!state.Equals(IEnvironment::MEDIA_BAD_REMOVAL) &&
                !state.Equals(IEnvironment::MEDIA_NOFS) &&
                !state.Equals(IEnvironment::MEDIA_UNMOUNTABLE) && !GetUmsEnabling()) {
            if (DEBUG_EVENTS) {
                Slogger::I(TAG, "updating volume state for media bad removal nofs and unmountable");
            }
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_UNMOUNTED);
            action = IIntent::ACTION_MEDIA_UNMOUNTED;
        }
    }
    else if (newState == VolumeState::Pending) {
        //
    }
    else if (newState == VolumeState::Checking) {
        if (DEBUG_EVENTS) {
            Slogger::I(TAG, "updating volume state checking");
        }
        UpdatePublicVolumeState(volume, IEnvironment::MEDIA_CHECKING);
        action = IIntent::ACTION_MEDIA_CHECKING;
    } else if (newState == VolumeState::Mounted) {
        if (DEBUG_EVENTS) {
            Slogger::I(TAG, String("updating volume state mounted"));
        }
        UpdatePublicVolumeState(volume, IEnvironment::MEDIA_MOUNTED);
        action = IIntent::ACTION_MEDIA_MOUNTED;
    }
    else if (newState == VolumeState::Unmounting) {
        action = IIntent::ACTION_MEDIA_EJECT;
    }
    else if (newState == VolumeState::Formatting) {
        //
    }
    else if (newState == VolumeState::Shared) {
        if (DEBUG_EVENTS) {
            Slogger::I(TAG, "Updating volume state media mounted");
        }
        /* Send the media unmounted event first */
        UpdatePublicVolumeState(volume, IEnvironment::MEDIA_UNMOUNTED);
        SendStorageIntent(IIntent::ACTION_MEDIA_UNMOUNTED, volume, UserHandle::ALL);

        if (DEBUG_EVENTS) {
            Slogger::I(TAG, "Updating media shared");
        }
        UpdatePublicVolumeState(volume, IEnvironment::MEDIA_SHARED);
        action = IIntent::ACTION_MEDIA_SHARED;
        if (LOCAL_LOGD) {
            Slogger::D(TAG, "Sending ACTION_MEDIA_SHARED intent");
        }
    }
    else {
        Slogger::E(TAG, "Unhandled VolumeState {%d}", newState);
    }

    if (!action.IsNull()) {
        SendStorageIntent(action, volume, UserHandle::ALL);
    }
}

Int32 CMountService::DoMountVolume(
    /* [in] */ const String& path)
{
    Int32 rc = StorageResultCode::OperationSucceeded;

    AutoPtr<IStorageVolume> volume;
   {
        AutoLock lock(mVolumesLock);
        volume = mVolumesByPath[path];
    }

    if (DEBUG_EVENTS) Slogger::I(TAG, "doMountVolume: Mouting %s", path.string());
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("mount"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("volume"), args, (NativeDaemonEvent**)&event))) {
        /*
         * Mount failed for some reason
         */
        String action;
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedNoMedia) {
            /*
             * Attempt to mount but no media inserted
             */
            rc = StorageResultCode::OperationFailedNoMedia;
        }
        else if (code == VoldResponseCode::OpFailedMediaBlank) {
            if (DEBUG_EVENTS) Slogger::I(TAG, " updating volume state :: media nofs");
            /*
             * Media is blank or does not contain a supported filesystem
             */
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_NOFS);
            action = IIntent::ACTION_MEDIA_NOFS;
            rc = StorageResultCode::OperationFailedMediaBlank;
        }
        else if (code == VoldResponseCode::OpFailedMediaCorrupt) {
            if (DEBUG_EVENTS) Slogger::I(TAG, "updating volume state media corrupt");
            /*
             * Volume consistency check failed
             */
            UpdatePublicVolumeState(volume, IEnvironment::MEDIA_UNMOUNTABLE);
            action = IIntent::ACTION_MEDIA_UNMOUNTABLE;
            rc = StorageResultCode::OperationFailedMediaCorrupt;
        }
        else {
            rc = StorageResultCode::OperationFailedInternalError;
        }

        /*
         * Send broadcast intent (if required for the failure)
         */
        if (!action.IsNull()) {
            SendStorageIntent(action, volume, UserHandle::ALL);
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     /*
    //      * Mount failed for some reason
    //      */
    //     String action = null;
    //     int code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedNoMedia) {
    //         /*
    //          * Attempt to mount but no media inserted
    //          */
    //         rc = StorageResultCode.OperationFailedNoMedia;
    //     } else if (code == VoldResponseCode.OpFailedMediaBlank) {
    //         if (DEBUG_EVENTS) Slog.i(TAG, " updating volume state :: media nofs");
    //         /*
    //          * Media is blank or does not contain a supported filesystem
    //          */
    //         updatePublicVolumeState(volume, Environment.MEDIA_NOFS);
    //         action = Intent.ACTION_MEDIA_NOFS;
    //         rc = StorageResultCode.OperationFailedMediaBlank;
    //     } else if (code == VoldResponseCode.OpFailedMediaCorrupt) {
    //         if (DEBUG_EVENTS) Slog.i(TAG, "updating volume state media corrupt");
    //         /*
    //          * Volume consistency check failed
    //          */
    //         updatePublicVolumeState(volume, Environment.MEDIA_UNMOUNTABLE);
    //         action = Intent.ACTION_MEDIA_UNMOUNTABLE;
    //         rc = StorageResultCode.OperationFailedMediaCorrupt;
    //     } else {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }

    //     /*
    //      * Send broadcast intent (if required for the failure)
    //      */
    //     if (action != null) {
    //         sendStorageIntent(action, volume, UserHandle.ALL);
    //     }
    // }
    return rc;
}

Int32 CMountService::DoUnmountVolume(
    /* [in] */ const String& path,
    /* [in] */ Boolean force,
    /* [in] */ Boolean removeEncryption)
{
    String volState;
    GetVolumeState(path, &volState);
    if (!volState.Equals(IEnvironment::MEDIA_MOUNTED)) {
        return VoldResponseCode::OpFailedVolNotMounted;
    }

    /*
     * Force a GC to make sure AssetManagers in other threads of the
     * system_server are cleaned up. We have to do this since AssetManager
     * instances are kept as a WeakReference and it's possible we have files
     * open on the external storage.
     */
    // Runtime.getRuntime().gc();

    // Redundant probably. But no harm in updating state again.
    mPms->UpdateExternalMediaStatus(FALSE, FALSE);

    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("unmount"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);

    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(String("volume"), args);
    if (removeEncryption) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("force_and_revert"), (ICharSequence**)&cs);
        cmd->AppendArg(cs);
    }
    else if (force) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("force"), (ICharSequence**)&cs);
        cmd->AppendArg(cs);
    }
    AutoPtr<NativeDaemonEvent> event;
    ECode ec = mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    if (FAILED(ec)) {
        // Don't worry about mismatch in PackageManager since the
        // call back will handle the status changes any way.
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedVolNotMounted) {
            return StorageResultCode::OperationFailedStorageNotMounted;
        }
        else if (code == VoldResponseCode::OpFailedStorageBusy) {
            return StorageResultCode::OperationFailedStorageBusy;
        }
        else {
            return StorageResultCode::OperationFailedInternalError;
        }
    }

    // We unmounted the volume. None of the asec containers are available now.
    {
        AutoLock lock(mAsecMountSetLock);
        mAsecMountSet.Clear();
    }

    return StorageResultCode::OperationSucceeded;
    // } catch (NativeDaemonConnectorException e) {
    //     // Don't worry about mismatch in PackageManager since the
    //     // call back will handle the status changes any way.
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedVolNotMounted) {
    //         return StorageResultCode.OperationFailedStorageNotMounted;
    //     } else if (code == VoldResponseCode.OpFailedStorageBusy) {
    //         return StorageResultCode.OperationFailedStorageBusy;
    //     } else {
    //         return StorageResultCode.OperationFailedInternalError;
    //     }
    // }
}

Int32 CMountService::DoFormatVolume(
    /* [in] */ const String& path)
{
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("format"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("volume"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedNoMedia) {
            return StorageResultCode::OperationFailedNoMedia;
        }
        else if (code == VoldResponseCode::OpFailedMediaCorrupt) {
            return StorageResultCode::OperationFailedMediaCorrupt;
        }
        else {
            return StorageResultCode::OperationFailedInternalError;
        }
    }
    return StorageResultCode::OperationSucceeded;
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedNoMedia) {
    //         return StorageResultCode.OperationFailedNoMedia;
    //     } else if (code == VoldResponseCode.OpFailedMediaCorrupt) {
    //         return StorageResultCode.OperationFailedMediaCorrupt;
    //     } else {
    //         return StorageResultCode.OperationFailedInternalError;
    //     }
    // }
}

Boolean CMountService::DoGetVolumeShared(
    /* [in] */ const String& path,
    /* [in] */ const String& method)
{
    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> cs1, cs2, cs3;
    CString::New(String("shared"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    CString::New(method, (ICharSequence**)&cs3);
    args->Set(2, (IInterface*)cs3);
    if (FAILED(mConnector->Execute(String("volume"), args, (NativeDaemonEvent**)&event))) {
        Slogger::E(TAG, "Failed to read response to volume shared %s %s", path.string(), method.string());
        return FALSE;
    }
    // } catch (NativeDaemonConnectorException ex) {
    //     Slog.e(TAG, "Failed to read response to volume shared " + path + " " + method);
    //     return FALSE;
    // }

    if (event->GetCode() == VoldResponseCode::ShareEnabledResult) {
        return event->GetMessage().EndWith("enabled");
    }
    else {
        return FALSE;
    }
}

void CMountService::NotifyShareAvailabilityChange(
    /* [in] */ const Boolean avail)
{
    {
        AutoLock lock(mListenersLock);
        mUmsAvailable = avail;
        List<AutoPtr<MountServiceBinderListener> >::ReverseIterator rit = mListeners.RBegin();
        while (rit != mListeners.REnd()) {
            AutoPtr<MountServiceBinderListener> bl = *rit;
            // try {
            ECode ec = bl->mListener->OnUsbMassStorageConnectionChanged(avail);
            if (SUCCEEDED(ec)) {
                ++rit;
                continue;
            }
            else if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::E(TAG, "Listener dead");
                rit = List<AutoPtr<MountServiceBinderListener> >::ReverseIterator(mListeners.Erase(--(rit.GetBase())));
            }
            else {
                Slogger::E(TAG, "Listener failed 0x%08x", ec);
                ++rit;
            }
            // } catch (RemoteException rex) {
            //     Slog.e(TAG, "Listener dead");
            //     mListeners.remove(i);
            // } catch (Exception ex) {
            //     Slog.e(TAG, "Listener failed", ex);
            // }
        }
    }

    if (mSystemReady == TRUE) {
        SendUmsIntent(avail);
    }
    else {
        mSendUmsConnectedOnBoot = avail;
    }

    AutoPtr<IStorageVolume> primary = GetPrimaryPhysicalVolume();
    if (avail == FALSE && primary != NULL) {
        String volState, path;
        primary->GetPath(&path);
        GetVolumeState(path, &volState);
        if (IEnvironment::MEDIA_SHARED.Equals(volState)) {
            /*
             * USB mass storage disconnected while enabled
             */
            AutoPtr<UsbMassStorageThread> thread = new UsbMassStorageThread(path, this);
            thread->Start();
        }
    }
}

void CMountService::SendStorageIntent(
    /* [in] */ const String& action,
    /* [in] */ IStorageVolume* volume,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IIntent> intent;
    AutoPtr<IUri> uri;
    String path;
    volume->GetPath(&path);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    ASSERT_SUCCEEDED(helper->Parse(String("file://") + path, (IUri**)&uri));
    ASSERT_SUCCEEDED(CIntent::New(action, uri, (IIntent**)&intent));
    intent->PutExtra(String("storage_volume")/*CStorageVolume::EXTRA_STORAGE_VOLUME*/, IParcelable::Probe(volume));

    Slogger::D(TAG, "sendStorageIntent %p to %p", intent.Get(), user);
    mContext->SendBroadcastAsUser(intent, user);
}

void CMountService::SendUmsIntent(
    /* [in] */ Boolean c)
{
    AutoPtr<IIntent> in;
    CIntent::New(String(c ? IIntent::ACTION_UMS_CONNECTED : IIntent::ACTION_UMS_DISCONNECTED), (IIntent**)&in);
    mContext->SendBroadcastAsUser(in, UserHandle::ALL);
}

ECode CMountService::ValidatePermission(
    /* [in] */ const String& perm)
{
    Int32 val;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(perm, &val));
    if (val != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException(String.format("Requires %s permission", perm));
        Slogger::D(TAG, "Requires %s permission", perm.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CMountService::ReadStorageListLocked()
{
    mVolumes.Clear();
    mVolumeStates.Clear();

    AutoPtr<IResources> resources;
    ASSERT_SUCCEEDED(mContext->GetResources((IResources**)&resources));

    Int32 id = R::xml::storage_list;
    AutoPtr<IXmlResourceParser> parser;
    resources->GetXml(id, (IXmlResourceParser**)&parser);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
    AutoPtr<CUserManagerService> userManager = CUserManagerService::GetInstance();

    // try {
    ECode ec = XmlUtils::BeginDocument(IXmlPullParser::Probe(parser), TAG_STORAGE_LIST);
    if (FAILED(ec)) {
        ec = E_RUNTIME_EXCEPTION;
        goto finally;
    }

    while (TRUE) {
        if (FAILED(XmlUtils::NextElement(IXmlPullParser::Probe(parser)))) {
            ec = E_RUNTIME_EXCEPTION;
            break;
        }

        String element;
        parser->GetName(&element);
        if (element.IsNull()) break;

        if (TAG_STORAGE.Equals(element)) {
            Int32 size = ArraySize(R::styleable::Storage);
            AutoPtr< ArrayOf<Int32> > storage = ArrayOf<Int32>::Alloc(size);
            storage->Copy(R::styleable::Storage, size);
            AutoPtr<ITypedArray> a;
            ASSERT_SUCCEEDED(resources->ObtainAttributes(attrs, storage, (ITypedArray**)&a));

            String path;
            a->GetString(R::styleable::Storage_mountPoint, &path);
            Int32 descriptionId;
            a->GetResourceId(R::styleable::Storage_storageDescription, -1, &descriptionId);
            AutoPtr<ICharSequence> description;
            a->GetText(R::styleable::Storage_storageDescription, (ICharSequence**)&description);
            Boolean primary;
            a->GetBoolean(R::styleable::Storage_primary, FALSE, &primary);
            Boolean removable;
            a->GetBoolean(R::styleable::Storage_removable, FALSE, &removable);
            Boolean emulated;
            a->GetBoolean(R::styleable::Storage_emulated, FALSE, &emulated);
            Int32 mtpReserve;
            a->GetInt32(R::styleable::Storage_mtpReserve, 0, &mtpReserve);
            Boolean allowMassStorage;
            a->GetBoolean(R::styleable::Storage_allowMassStorage, FALSE, &allowMassStorage);
            // resource parser does not support longs, so XML value is in megabytes
            Int32 maxFileSize32;
            a->GetInt32(R::styleable::Storage_maxFileSize, 0, &maxFileSize32);
            Int64 maxFileSize = maxFileSize32 * 1024L * 1024L;

            String descriptionStr;
            description->ToString(&descriptionStr);
            Slogger::D(TAG, "got storage path: %s description: %d primary: %d removable: %d emulated: %d mtpReserve: %d allowMassStorage: %d maxFileSize: %d",
                    path.string(), descriptionStr.string(), primary, removable, emulated, mtpReserve, allowMassStorage, maxFileSize);

            if (emulated) {
                // For devices with emulated storage, we create separate
                // volumes for each known user.
                mEmulatedTemplate = NULL;
                CStorageVolume::New(NULL, descriptionId, TRUE, FALSE,
                        TRUE, mtpReserve, FALSE, maxFileSize, NULL,
                        (IStorageVolume**)&mEmulatedTemplate);

                AutoPtr<IObjectContainer> objectContainer;
                userManager->GetUsers(FALSE, (IObjectContainer**)&objectContainer);
                AutoPtr<IObjectEnumerator> objectEmulator;
                objectContainer->GetObjectEnumerator((IObjectEnumerator**)&objectEmulator);
                Boolean hasNext;
                while(objectEmulator->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IUserInfo> info;
                    objectEmulator->Current((IInterface**)&info);
                    AutoPtr<IUserHandle> userHandle;
                    info->GetUserHandle((IUserHandle**)&userHandle);
                    CreateEmulatedVolumeForUserLocked(userHandle);
                }
                //for (UserInfo user : userManager.getUsers(FALSE)) {
                //    CreateEmulatedVolumeForUserLocked(user.getUserHandle());
                //}
            }
            else {
                if (path.IsNull() || description == NULL) {
                    Slogger::E(TAG, "Missing storage path or description in readStorageList");
                }
                else {
                    AutoPtr<IFile> file;
                    CFile::New(path, (IFile**)&file);
                    AutoPtr<IStorageVolume> volume;
                    CStorageVolume::New(file, descriptionId, primary, removable,
                            emulated, mtpReserve, allowMassStorage, maxFileSize,
                            NULL, (IStorageVolume**)&volume);
                    AddVolumeLocked(volume);
                }
            }
            a->Recycle();
        }
    }
    // } catch (XmlPullParserException e) {
        // throw new RuntimeException(e);
    // } catch (IOException e) {
        // throw new RuntimeException(e);
    // } finally {
    // Compute storage ID for each physical volume; emulated storage is
    // always 0 when defined.
finally:
    Boolean extStorageEmulated;
    IsExternalStorageEmulated(&extStorageEmulated);
    Int32 index = extStorageEmulated ? 1 : 0;
    List<AutoPtr<IStorageVolume> >::Iterator iter;
    for(iter = mVolumes.Begin(); iter != mVolumes.End(); iter++) {
        AutoPtr<IStorageVolume> volume = *iter;
        Boolean isEmulated;
        if (volume->IsEmulated(&isEmulated), !isEmulated) {
            volume->SetStorageId(index++);
        }
    }
    parser->Close();

    return ec;
    // }
}

void CMountService::CreateEmulatedVolumeForUserLocked(
    /* [in] */ IUserHandle* user)
{
    if (mEmulatedTemplate == NULL) {
        // throw new IllegalStateException("Missing emulated volume multi-user template");
        return;
    }

    Int32 identifier;
    user->GetIdentifier(&identifier);
    AutoPtr<IUserEnvironment> userEnv;
    CUserEnvironment::New(identifier, (IUserEnvironment**)&userEnv);
    AutoPtr<IFile> path;
    userEnv->GetExternalStorageDirectory((IFile**)&path);
    AutoPtr<IStorageVolume> volume;
    AutoPtr<IStorageVolumeHelper> helper;
    CStorageVolumeHelper::AcquireSingleton((IStorageVolumeHelper**)&helper);
    ASSERT_SUCCEEDED(helper->FromTemplate(mEmulatedTemplate, path, user, (IStorageVolume**)&volume));
    volume->SetStorageId(0);
    AddVolumeLocked(volume);

    if (mSystemReady) {
        UpdatePublicVolumeState(volume, IEnvironment::MEDIA_MOUNTED);
    }
    else {
        // Place stub status for early callers to find
        String volPath;
        volume->GetPath(&volPath);
        mVolumeStates[volPath] = IEnvironment::MEDIA_MOUNTED;
    }
}

ECode CMountService::AddVolumeLocked(
    /* [in] */ IStorageVolume* volume)
{
    assert(volume != NULL);
    Slogger::D(TAG, "addVolumeLocked() %p", volume);

    mVolumes.PushBack(volume);

    String volPath;
    volume->GetPath(&volPath);
    AutoPtr<IStorageVolume> existing = mVolumesByPath[volPath];
    if (existing != NULL) {
        Slogger::E(TAG, "Volume at %s already exists: %p", volPath.string(), existing.Get());
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException(
        //         "Volume at " + volume.getPath() + " already exists: " + existing);
    }
    mVolumesByPath[volPath] = volume;
    return NOERROR;
}

void CMountService::RemoveVolumeLocked(
    /* [in] */ IStorageVolume* volume)
{
    Slogger::D(TAG, "RemoveVolumeLocked() %p", volume);
    mVolumes.Remove(volume);
    String volPath;
    volume->GetPath(&volPath);
    mVolumesByPath.Erase(volPath);
    mVolumeStates.Erase(volPath);
}

AutoPtr<IStorageVolume> CMountService::GetPrimaryPhysicalVolume()
{
    AutoLock lock(mVolumesLock);
    List<AutoPtr<IStorageVolume> >::Iterator iter;
    for (iter = mVolumes.Begin(); iter != mVolumes.End(); ++iter) {
        AutoPtr<IStorageVolume> volume = *iter;
        Boolean isPrimary, isEmulated;
        if ((volume->IsPrimary(&isPrimary), isPrimary)
                && (volume->IsEmulated(&isEmulated), !isEmulated)) {
            return volume;
        }
    }
    return NULL;
}

void CMountService::SystemReady()
{
    mSystemReady = TRUE;
    Boolean result;
    mHandler->SendEmptyMessage(H_SYSTEM_READY, &result);
}

ECode CMountService::RegisterListener(
    /* [in] */ IIMountServiceListener* listener)
{
    AutoLock lock(mListenersLock);
    AutoPtr<MountServiceBinderListener> bl = new MountServiceBinderListener(listener, this);
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)listener->Probe(EIID_IProxy);
    assert(proxy != NULL);
    ECode ec = proxy->LinkToDeath(bl, 0);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to link to listener death");
        return ec;
    }
    mListeners.PushBack(bl);
    // } catch (RemoteException rex) {
    //     Slog.e(TAG, "Failed to link to listener death");
    // }
    return NOERROR;
}

ECode CMountService::UnregisterListener(
    /* [in] */ IIMountServiceListener* listener)
{
    AutoLock lock(mListenersLock);
    List<AutoPtr<MountServiceBinderListener> >::Iterator iter = mListeners.Begin();
    for(; iter != mListeners.End(); iter++) {
        AutoPtr<MountServiceBinderListener> bl = *iter;
        if (bl->mListener.Get() == listener) {
            mListeners.Erase(iter);
            AutoPtr<IProxy> proxy = (IProxy*)listener->Probe(EIID_IProxy);
            assert(proxy != NULL);
            Boolean result;
            return proxy->UnlinkToDeath(bl, 0, &result);
        }
    }
    return NOERROR;
}

ECode CMountService::Shutdown(
    /* [in] */ IIMountShutdownObserver* observer)
{
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::SHUTDOWN));

    Slogger::I(TAG, "Shutting down");
    AutoLock lock(mVolumesLock);
    HashMap<String, String>::Iterator iter = mVolumeStates.Begin();
    for (; iter != mVolumeStates.End(); ++iter) {
        String path = iter->mFirst;
        String state = iter->mSecond;

        if (state.Equals(IEnvironment::MEDIA_SHARED)) {
            /*
             * If the media is currently shared, unshare it.
             * XXX: This is still dangerous!. We should not
             * be rebooting at *all* if UMS is enabled, since
             * the UMS host could have dirty FAT cache entries
             * yet to flush.
             */
            SetUsbMassStorageEnabled(FALSE);
        }
        else if (state.Equals(IEnvironment::MEDIA_CHECKING)) {
            /*
             * If the media is being checked, then we need to wait for
             * it to complete before being able to proceed.
             */
            // XXX: @hackbod - Should we disable the ANR timer here?
            Int32 retries = 30;
            while (state.Equals(IEnvironment::MEDIA_CHECKING) && (retries-- >=0)) {
                // try {
                ECode ec = Thread::Sleep(1000);
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Slogger::E(TAG, "Interrupted while waiting for media 0x%08x", ec);
                    break;
                }
                // } catch (InterruptedException iex) {
                //     Slog.e(TAG, "Interrupted while waiting for media", iex);
                //     break;
                // }
                AutoPtr<IEnvironment> env;
                CEnvironment::AcquireSingleton((IEnvironment**)&env);
                env->GetExternalStorageState(&state);
            }
            if (retries == 0) {
                Slogger::E(TAG, "Timed out waiting for media to check");
            }
        }

        if (state.Equals(IEnvironment::MEDIA_MOUNTED)) {
            // Post a unmount message.
            AutoPtr<ShutdownCallBack> ucb = new ShutdownCallBack(path, observer, this);

            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(H_UNMOUNT_PM_UPDATE, (IMessage**)&msg);
            msg->SetObj(ucb);
            Boolean result;
            mHandler->SendMessage(msg, &result);
        }
        else if (observer != NULL) {
            /*
             * Observer is waiting for onShutDownComplete when we are done.
             * Since nothing will be done send notification directly so shutdown
             * sequence can continue.
             */
            // try {
            if (FAILED(observer->OnShutDownComplete(StorageResultCode::OperationSucceeded))) {
                Slogger::W(TAG, "RemoteException when shutting down");
            }
            // } catch (RemoteException e) {
            //     Slog.w(TAG, "RemoteException when shutting down");
            // }
        }
    }
    return NOERROR;
}

Boolean CMountService::GetUmsEnabling()
{
    AutoLock lock(mListenersLock);
    return mUmsEnabling;
}

void CMountService::SetUmsEnabling(
    /* [in] */ Boolean enable)
{
    AutoLock lock(mListenersLock);
    mUmsEnabling = enable;
}

ECode CMountService::IsUsbMassStorageConnected(
    /* [out] */ Boolean* connected)
{
    VALIDATE_NOT_NULL(connected);
    WaitForReady();

    if (GetUmsEnabling()) {
        *connected = TRUE;
        return NOERROR;
    }
    AutoLock lock(mListenersLock);
    *connected = mUmsAvailable;
    return NOERROR;
}

ECode CMountService::SetUsbMassStorageEnabled(
    /* [in] */ Boolean enable)
{
    WaitForReady();
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::MOUNT_UNMOUNT_FILESYSTEMS));

    // AutoPtr<IStorageVolume> primary = GetPrimaryPhysicalVolume();
    // if (primary == NULL) {
    //  return NOERROR;
    // }

    // TODO: Add support for multiple share methods
    List< AutoPtr<IStorageVolume> >::Iterator it = mVolumes.Begin();
    for (; it != mVolumes.End(); ++it) {
        /*
         * If the volume is mounted and we're enabling then unmount it
         */
        //String path = Environment.getExternalStorageDirectory().getPath();
        AutoPtr<IStorageVolume> volume = *it;
        Boolean allowMassStorage;
        volume->AllowMassStorage(&allowMassStorage);
        if (!allowMassStorage)
            continue;

        String path, vs;
        volume->GetPath(&path);
        GetVolumeState(path, &vs);
        String method("ums");
        if (enable && vs.Equals(IEnvironment::MEDIA_MOUNTED)) {
            // Override for isUsbMassStorageEnabled()
            SetUmsEnabling(enable);
            AutoPtr<UmsEnableCallBack> umscb = new UmsEnableCallBack(path, method, TRUE, this);
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(H_UNMOUNT_PM_UPDATE, (IMessage**)&msg);
            msg->SetObj(umscb);
            Boolean result;
            mHandler->SendMessage(msg, &result);

            // Clear override
            SetUmsEnabling(FALSE);
        }
        /*
         * If we disabled UMS then mount the volume
         */
        if (!enable && vs.Equals(IEnvironment::MEDIA_SHARED)) {
            DoShareUnshareVolume(path, method, enable);
            if (DoMountVolume(path) != StorageResultCode::OperationSucceeded) {
                Slogger::E(TAG, "Failed to remount %s after disabling share method %s",
                        path.string(), method.string());
                /*
                 * Even though the mount failed, the unshare didn't so don't indicate an error.
                 * The mountVolume() call will have set the storage state and sent the necessary
                 * broadcasts.
                 */
            }
        }
    }

    return NOERROR;
}

ECode CMountService::IsUsbMassStorageEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    WaitForReady();

    /*final StorageVolume primary = getPrimaryPhysicalVolume();
    if (primary != null) {
        return doGetVolumeShared(primary.getPath(), "ums");
    } else {
        return false;
    }*/
    //we should scan all volumes to make sure the massStorage is enable
    List< AutoPtr<IStorageVolume> >::Iterator it = mVolumes.Begin();
    for (; it != mVolumes.End(); ++it) {
        AutoPtr<IStorageVolume> volume = *it;
        Boolean allowMassStorage;
        volume->AllowMassStorage(&allowMassStorage);
        String path;
        volume->GetPath(&path);
        if(allowMassStorage && DoGetVolumeShared(path, String("ums"))) {
            *enabled = TRUE;
            return NOERROR;
        }
    }
    *enabled = FALSE;
    return NOERROR;
}

ECode CMountService::GetVolumeState(
    /* [in] */ const String& mountPoint,
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    AutoLock lock(mVolumesLock);

    String state;
    HashMap<String, String>::Iterator iter = mVolumeStates.Find(mountPoint);
    if (iter != mVolumeStates.End()) {
        state = iter->mSecond;
    }
    if (state.IsNull()) {
        Slogger::W(TAG, "GetVolumeState(%s): Unknown volume", mountPoint.string());
        // don't throw on dynamically created usb mount paths. eg /mnt/sda1
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String value;
        if (sysProp->Get(String("vold.encrypt_progress"), &value), !value.IsNullOrEmpty()) {
            state = IEnvironment::MEDIA_REMOVED;
        }
        else {
//            throw new IllegalArgumentException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    *s = state;
    return NOERROR;
}

ECode CMountService::IsExternalStorageEmulated(
    /* [out] */ Boolean* emulated)
{
    VALIDATE_NOT_NULL(emulated);
    *emulated = mEmulatedTemplate != NULL;
    return NOERROR;
}

ECode CMountService::MountVolume(
    /* [in] */ const String& mountPoint,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::MOUNT_UNMOUNT_FILESYSTEMS));

    WaitForReady();
    *result = DoMountVolume(mountPoint);
    return NOERROR;
}

ECode CMountService::UnmountVolume(
    /* [in] */ const String& mountPoint,
    /* [in] */ Boolean force,
    /* [in] */ Boolean removeEncryption)
{
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::MOUNT_UNMOUNT_FILESYSTEMS));
    WaitForReady();

    String volState;
    FAIL_RETURN(GetVolumeState(mountPoint, &volState));
    if (DEBUG_UNMOUNT) {
        Slogger::I(TAG, "Unmounting %s force = %d removeEncryption = %d", mountPoint.string(), force, removeEncryption);
    }
    if (IEnvironment::MEDIA_UNMOUNTED.Equals(volState) ||
            IEnvironment::MEDIA_REMOVED.Equals(volState) ||
            IEnvironment::MEDIA_SHARED.Equals(volState) ||
            IEnvironment::MEDIA_UNMOUNTABLE.Equals(volState)) {
        // Media already unmounted or cannot be unmounted.
        // TODO return valid return code when adding observer call back.
        return NOERROR;
    }
    AutoPtr<UnmountCallBack> ucb = new UnmountCallBack(mountPoint, force, removeEncryption, this);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H_UNMOUNT_PM_UPDATE, (IMessage**)&msg);
    msg->SetObj(ucb);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CMountService::FormatVolume(
    /* [in] */ const String& mountPoint,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::MOUNT_FORMAT_FILESYSTEMS));
    WaitForReady();

    *result = DoFormatVolume(mountPoint);
    return NOERROR;
}

ECode CMountService::GetStorageUsers(
    /* [in] */ const String& path,
    /* [out, callee] */ ArrayOf<Int32>** pids)
{
    VALIDATE_NOT_NULL(pids);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::MOUNT_UNMOUNT_FILESYSTEMS));
    WaitForReady();
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("users"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(path, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    if (FAILED(mConnector->ExecuteForList(String("storage"), args,
            (ArrayOf<NativeDaemonEvent*>**)&events))) {
        Slogger::E(TAG, "Failed to retrieve storage users list");
        *pids = ArrayOf<Int32>::Alloc(0);
        REFCOUNT_ADD(*pids);
        return NOERROR;
    }
    AutoPtr< ArrayOf<String> > r = NativeDaemonEvent::FilterMessageList(*events, VoldResponseCode::StorageUsersListResult);

    // FMT: <pid> <process name>
    Int32 len = r->GetLength();
    AutoPtr< ArrayOf<Int32> > data = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr< ArrayOf<String> > tok;
        ASSERT_SUCCEEDED(StringUtils::Split((*r)[i], String(" "), (ArrayOf<String>**)&tok));
        // try {
        (*data)[i] = StringUtils::ParseInt32((*tok)[0]);
        // } catch (NumberFormatException nfe) {
        //     Slog.e(TAG, String.format("Error parsing pid %s", tok[0]));
        //     return new int[0];
        // }
    }
    *pids = data;
    REFCOUNT_ADD(*pids);
    // } catch (NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to retrieve storage users list", e);
    //     return new int[0];
    // }
    return NOERROR;
}

void CMountService::WarnOnNotMounted()
{
    AutoPtr<IStorageVolume> primary = GetPrimaryPhysicalVolume();
    if (primary != NULL) {
        Boolean mounted = FALSE;
        String path, state;
        primary->GetPath(&path);
        GetVolumeState(path, &state);
        mounted = IEnvironment::MEDIA_MOUNTED.Equals(state);
        if (!mounted) {
            Slogger::W(TAG, "getSecureContainerList() called when storage not mounted");
        }
    }
}

ECode CMountService::GetSecureContainerList(
    /* [out, callee] */ ArrayOf<String>** pids)
{
    VALIDATE_NOT_NULL(pids);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_ACCESS));
    WaitForReady();
    WarnOnNotMounted();

    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String("list"), (ICharSequence**)&cs);
    args->Set(0, (IInterface*)cs);
    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    if (FAILED(mConnector->ExecuteForList(String("asec"), args,
            (ArrayOf<NativeDaemonEvent*>**)&events))) {
        *pids = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*pids);
        return NOERROR;
    }
    AutoPtr< ArrayOf<String> > ids = NativeDaemonEvent::FilterMessageList(*events, VoldResponseCode::AsecListResult);
    *pids = ids;
    REFCOUNT_ADD(*pids);
    // } catch (NativeDaemonConnectorException e) {
    //     return new String[0];
    // }
    return NOERROR;
}

ECode CMountService::CreateSecureContainer(
    /* [in] */ const String& id,
    /* [in] */ Int32 sizeMb,
    /* [in] */ const String& fstype,
    /* [in] */ const String& key,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Boolean external,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_CREATE));
    WaitForReady();
    WarnOnNotMounted();

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(7);
    AutoPtr<ICharSequence> cs1, cs2, cs3, cs4, cs5, cs6, cs7;
    CString::New(String("create"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    CString::New(StringUtils::Int32ToString(sizeMb), (ICharSequence**)&cs3);
    args->Set(2, (IInterface*)cs3);
    CString::New(fstype, (ICharSequence**)&cs4);
    args->Set(3, (IInterface*)cs4);
    CString::New(key, (ICharSequence**)&cs5);
    args->Set(4, (IInterface*)cs5);
    CString::New(StringUtils::Int32ToString(ownerUid), (ICharSequence**)&cs6);
    args->Set(5, (IInterface*)cs6);
    CString::New(external ? String("1") : String("0"), (ICharSequence**)&cs7);
    args->Set(6, (IInterface*)cs7);
    AutoPtr<NativeDaemonEvent> event;
    if(FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        rc = StorageResultCode::OperationFailedInternalError;
    }
    // } catch (NativeDaemonConnectorException e) {
    //     rc = StorageResultCode.OperationFailedInternalError;
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        AutoLock lock(mAsecMountSetLock);
        mAsecMountSet.Insert(id);
    }
    *result = rc;
    return NOERROR;
}

ECode CMountService::FinalizeSecureContainer(
    /* [in] */ const String& id,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_CREATE));
    WarnOnNotMounted();

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("finalize"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        rc = StorageResultCode::OperationFailedInternalError;
    }
    /*
     * Finalization does a remount, so no need
     * to update mAsecMountSet
     */
    // } catch (NativeDaemonConnectorException e) {
    //     rc = StorageResultCode.OperationFailedInternalError;
    // }
    *result = rc;
    return NOERROR;
}

ECode CMountService::FixPermissionsSecureContainer(
    /* [in] */ const String& id,
    /* [in] */ Int32 gid,
    /* [in] */ const String& filename,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_CREATE));
    WarnOnNotMounted();

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    AutoPtr<ICharSequence> cs1, cs2, cs3, cs4;
    CString::New(String("fixperms"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    CString::New(String("fixperms"), (ICharSequence**)&cs3);
    args->Set(2, (IInterface*)cs3);
    CString::New(filename, (ICharSequence**)&cs4);
    args->Set(3, (IInterface*)cs4);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        rc = StorageResultCode::OperationFailedInternalError;
    }
    /*
     * Fix permissions does a remount, so no need to update
     * mAsecMountSet
     */
    // } catch (NativeDaemonConnectorException e) {
    //     rc = StorageResultCode.OperationFailedInternalError;
    // }
    *result = rc;
    return NOERROR;
}

ECode CMountService::DestroySecureContainer(
    /* [in] */ const String& id,
    /* [in] */ Boolean force,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_DESTROY));
    WaitForReady();
    WarnOnNotMounted();

    /*
     * Force a GC to make sure AssetManagers in other threads of the
     * system_server are cleaned up. We have to do this since AssetManager
     * instances are kept as a WeakReference and it's possible we have files
     * open on the external storage.
     */
    // Runtime.getRuntime().gc();

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("destroy"), (ICharSequence**)&cs1);
    args->Set(0, (IInterface*)cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, (IInterface*)cs2);
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(String("asec"), args);
    if (force) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("force"), (ICharSequence**)&cs);
        cmd->AppendArg((IInterface*)cs);
    }
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(cmd, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageBusy) {
            rc = StorageResultCode::OperationFailedStorageBusy;
        }
        else {
            rc = StorageResultCode::OperationFailedInternalError;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageBusy) {
    //         rc = StorageResultCode.OperationFailedStorageBusy;
    //     } else {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        AutoLock lock(mAsecMountSetLock);
        HashSet<String>::Iterator iter = mAsecMountSet.Find(id);
        if (iter != mAsecMountSet.End()) {
            mAsecMountSet.Erase(iter);
        }
    }
    *result = rc;
    return NOERROR;
}

ECode CMountService::MountSecureContainer(
    /* [in] */ const String& id,
    /* [in] */ const String& key,
    /* [in] */ Int32 ownerUid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_MOUNT_UNMOUNT));
    WaitForReady();
    WarnOnNotMounted();

    {
        AutoLock lock(mAsecMountSetLock);
        HashSet<String>::Iterator iter = mAsecMountSet.Find(id);
        if (iter != mAsecMountSet.End()) {
            *result = StorageResultCode::OperationFailedStorageMounted;
            return NOERROR;
        }
    }

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    AutoPtr<ICharSequence> cs1, cs2, cs3, cs4;
    CString::New(String("mount"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    CString::New(key, (ICharSequence**)&cs3);
    args->Set(2, cs3);
    CString::New(StringUtils::Int32ToString(ownerUid), (ICharSequence**)&cs4);
    args->Set(3, cs4);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code != VoldResponseCode::OpFailedStorageBusy) {
            rc = StorageResultCode::OperationFailedInternalError;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code != VoldResponseCode.OpFailedStorageBusy) {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        AutoLock lock(mAsecMountSetLock);
        mAsecMountSet.Insert(id);
    }
    *result = rc;
    return NOERROR;
}

ECode CMountService::UnmountSecureContainer(
    /* [in] */ const String& id,
    /* [in] */ Boolean force,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_MOUNT_UNMOUNT));
    WaitForReady();
    WarnOnNotMounted();

    HashSet<String>::Iterator iter;
    {
        AutoLock lock(mAsecMountSetLock);
        iter = mAsecMountSet.Find(id);
        if (iter == mAsecMountSet.End()) {
            *result = StorageResultCode::OperationFailedStorageNotMounted;
            return NOERROR;
        }
    }

    /*
     * Force a GC to make sure AssetManagers in other threads of the
     * system_server are cleaned up. We have to do this since AssetManager
     * instances are kept as a WeakReference and it's possible we have files
     * open on the external storage.
     */
    // Runtime.getRuntime().gc();

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("unmount"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(String("asec"), args);
    if (force) {
        AutoPtr<ICharSequence> cs;
        CString::New(String("force"), (ICharSequence**)&cs);
        cmd->AppendArg(cs);
    }
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(cmd, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageBusy) {
            rc = StorageResultCode::OperationFailedStorageBusy;
        }
        else {
            rc = StorageResultCode::OperationFailedInternalError;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageBusy) {
    //         rc = StorageResultCode.OperationFailedStorageBusy;
    //     } else {
    //         rc = StorageResultCode.OperationFailedInternalError;
    //     }
    // }

    if (rc == StorageResultCode::OperationSucceeded) {
        AutoLock lock(mAsecMountSetLock);
        mAsecMountSet.Erase(iter);
    }
    *result = rc;
    return NOERROR;
}

ECode CMountService::IsSecureContainerMounted(
    /* [in] */ const String& id,
    /* [out] */ Boolean* mounted)
{
    VALIDATE_NOT_NULL(mounted);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_ACCESS));
    WaitForReady();
    WarnOnNotMounted();

    AutoLock lock(mAsecMountSetLock);
    HashSet<String>::Iterator iter = mAsecMountSet.Find(String(id));
    *mounted = iter != mAsecMountSet.End();
    return NOERROR;
}

ECode CMountService::RenameSecureContainer(
    /* [in] */ const String& oldId,
    /* [in] */ const String& newId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_RENAME));
    WaitForReady();
    WarnOnNotMounted();

    {
        AutoLock lock(mAsecMountSetLock);
        /*
         * Because a mounted container has active internal state which cannot be
         * changed while active, we must ensure both ids are not currently mounted.
         */
        if (mAsecMountSet.Find(oldId) != mAsecMountSet.End()
                || mAsecMountSet.Find(newId) != mAsecMountSet.End() ) {
            *result = StorageResultCode::OperationFailedStorageMounted;
            return NOERROR;
        }
    }

    Int32 rc = StorageResultCode::OperationSucceeded;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> cs1, cs2, cs3;
    CString::New(String("rename"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(oldId, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    CString::New(newId, (ICharSequence**)&cs3);
    args->Set(2, cs3);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        rc = StorageResultCode::OperationFailedInternalError;
    }
    // } catch (NativeDaemonConnectorException e) {
    //     rc = StorageResultCode.OperationFailedInternalError;
    // }
    *result = rc;
    return NOERROR;
}

ECode CMountService::GetSecureContainerPath(
    /* [in] */ const String& id,
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_ACCESS));
    WaitForReady();
    WarnOnNotMounted();

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("path"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageNotFound) {
            Slogger::I(TAG, "Container '%s' not found", (const char*)id);
            *path = String(NULL);
            return NOERROR;
        }
        else {
            Slogger::E(TAG, "Unexpected response code %d", code);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException(String.format("Unexpected response code %d", code));
        }
    }
    event->CheckCode(VoldResponseCode::AsecPathResult);
    *path = event->GetMessage();
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageNotFound) {
    //         Slog.i(TAG, String.format("Container '%s' not found", id));
    //         return NULL;
    //     } else {
    //         throw new IllegalStateException(String.format("Unexpected response code %d", code));
    //     }
    // }
    return NOERROR;
}

ECode CMountService::GetSecureContainerFilesystemPath(
    /* [in] */ const String& id,
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    FAIL_RETURN(ValidatePermission(Elastos::Droid::Manifest::permission::ASEC_ACCESS));
    WaitForReady();
    WarnOnNotMounted();

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("fspath"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(id, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    if (FAILED(mConnector->Execute(String("asec"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageNotFound) {
            Slogger::I(TAG, "Container '%s' not found", (const char*)id);
            *path = String(NULL);
            return NOERROR;
        }
        else {
            Slogger::E(TAG, "Unexpected response code %d", code);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException(String.format("Unexpected response code %d", code));
        }
    }
    event->CheckCode(VoldResponseCode::AsecPathResult);
    *path = event->GetMessage();
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageNotFound) {
    //         Slog.i(TAG, String.format("Container '%s' not found", id));
    //         return NULL;
    //     } else {
    //         throw new IllegalStateException(String.format("Unexpected response code %d", code));
    //     }
    // }
    return NOERROR;
}

ECode CMountService::FinishMediaUpdate()
{
    Boolean result;
    return mHandler->SendEmptyMessage(H_UNMOUNT_PM_DONE, &result);
}

Boolean CMountService::IsUidOwnerOfPackageOrSystem(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 callerUid)
{
    if (callerUid == IProcess::SYSTEM_UID) {
        return TRUE;
    }
    if (packageName.IsNull()) {
        return FALSE;
    }

    Int32 packageUid;
    mPms->GetPackageUid(packageName, UserHandle::GetUserId(callerUid), &packageUid);

    if (DEBUG_OBB) {
        Slogger::D(TAG, "packageName = %s, packageUid = %d, callerUid = %d", packageName.string(), packageUid, callerUid);
    }
    return callerUid == packageUid;
}

ECode CMountService::GetMountedObbPath(
    /* [in] */ const String& rawPath,
    /* [out] */ String* obbPath)
{
    VALIDATE_NOT_NULL(obbPath);
    // Preconditions.checkNotNull(rawPath, "rawPath cannot be NULL");
    if (rawPath.IsNull()) {
        Slogger::E(TAG, "rawPath cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException(String.valueOf(errorMessage));
    }
    WaitForReady();
    WarnOnNotMounted();

    AutoPtr<ObbState> state;
    {
        AutoLock lock(mObbPathToStateMapLock);
        HashMap<String, AutoPtr<ObbState> >::Iterator it = mObbPathToStateMap.Find(rawPath);
        if (it != mObbPathToStateMap.End()) {
            state = it->mSecond;
        }
    }
    if (state == NULL) {
        Slogger::W(TAG, "Failed to find OBB mounted at %s", rawPath.string());
        *obbPath = String(NULL);
        return NOERROR;
    }

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("path"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(state->mVoldPath, (ICharSequence**)&cs2);
    args->Set(1, cs2);
    if (FAILED(mConnector->Execute(String("obb"), args, (NativeDaemonEvent**)&event))) {
        Int32 code = 0;
        if (event != NULL) {
            code = event->GetCode();
        }
        if (code == VoldResponseCode::OpFailedStorageNotFound) {
            *obbPath = String(NULL);
            return NOERROR;
        }
        else {
            Slogger::E(TAG, "Unexpected response code %d", code);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException(String.format("Unexpected response code %d", code));
        }
    }
    event->CheckCode(VoldResponseCode::AsecPathResult);
    *obbPath = event->GetMessage();
    // } catch (NativeDaemonConnectorException e) {
    //     Int32 code = e.getCode();
    //     if (code == VoldResponseCode.OpFailedStorageNotFound) {
    //         return NULL;
    //     } else {
    //         throw new IllegalStateException(String.format("Unexpected response code %d", code));
    //     }
    // }
    return NOERROR;
}

ECode CMountService::IsObbMounted(
    /* [in] */ const String& rawPath,
    /* [out] */ Boolean* mounted)
{
    VALIDATE_NOT_NULL(mounted);
    // Preconditions.checkNotNull(rawPath, "rawPath cannot be NULL");
    if (rawPath.IsNull()) {
        Slogger::E(TAG, "rawPath cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException(String.valueOf(errorMessage));
    }
    AutoLock lock(mObbMountsLock);
    HashMap<String, AutoPtr<ObbState> >::Iterator iter = mObbPathToStateMap.Find(rawPath);
    *mounted = iter != mObbPathToStateMap.End();
    return NOERROR;
}

ECode CMountService::MountObb(
    /* [in] */ const String& rawPath,
    /* [in] */ const String& canonicalPath,
    /* [in] */ const String& key,
    /* [in] */ IIObbActionListener* token,
    /* [in] */ Int32 nonce)
{
    // Preconditions.checkNotNull(rawPath, "rawPath cannot be NULL");
    // Preconditions.checkNotNull(canonicalPath, "canonicalPath cannot be NULL");
    // Preconditions.checkNotNull(token, "token cannot be NULL");
    if (rawPath.IsNull() || canonicalPath.IsNull() || token == NULL) {
        Slogger::E(TAG, "argument cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException(String.valueOf(errorMessage));
    }

    Int32 callingUid = Binder::GetCallingUid();
    AutoPtr<ObbState> obbState = new ObbState(rawPath, canonicalPath, callingUid, token, nonce, this);
    AutoPtr<ObbAction> action = new MountObbAction(obbState, key, callingUid, this);

    AutoPtr<IMessage> msg;
    mObbActionHandler->ObtainMessage(OBB_RUN_ACTION, (IMessage**)&msg);
    msg->SetObj(action);
    Boolean result;
    mObbActionHandler->SendMessage(msg, &result);

    if (DEBUG_OBB)
        Slogger::I(TAG, "Send to OBB handler: %s", (const char*)action->ToString());
    return NOERROR;
}

ECode CMountService::UnmountObb(
    /* [in] */ const String& rawPath,
    /* [in] */ Boolean force,
    /* [in] */ IIObbActionListener* token,
    /* [in] */ Int32 nonce)
{
    // Preconditions.checkNotNull(rawPath, "rawPath cannot be NULL");
    if (rawPath.IsNull()) {
        Slogger::E(TAG, "rawPath cannot be NULL");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException(String.valueOf(errorMessage));
    }

    AutoPtr<ObbState> existingState;
    {
        AutoLock lock(mObbPathToStateMapLock);
        existingState = mObbPathToStateMap[rawPath];
    }

    if (existingState != NULL) {
        // TODO: separate state object from request data
        Int32 callingUid = Binder::GetCallingUid();
        AutoPtr<ObbState> newState = new ObbState(
                rawPath, existingState->mCanonicalPath, callingUid, token, nonce, this);
        AutoPtr<ObbAction> action = new UnmountObbAction(newState, force, this);

        AutoPtr<IMessage> msg;
        mObbActionHandler->ObtainMessage(OBB_RUN_ACTION, (IMessage**)&msg);
        msg->SetObj(action);
        Boolean result;
        mObbActionHandler->SendMessage(msg, &result);

        if (DEBUG_OBB)
            Slogger::I(TAG, "Send to OBB handler: %s", action->ToString().string());
    }
    else {
        Slogger::W(TAG, "Unknown OBB mount at %s", rawPath.string());
    }
    return NOERROR;
}

ECode CMountService::GetEncryptionState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CRYPT_KEEPER , String("no permission to access the crypt keeper")));

    WaitForReady();

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String("cryptocomplete"), (ICharSequence**)&cs);
    args->Set(0, cs);
    if (FAILED(mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        Slogger::W(TAG, "Error in communicating with cryptfs in validating");
        return ENCRYPTION_STATE_ERROR_UNKNOWN;
    }
    *result = StringUtils::ParseInt32(event->GetMessage());
    // } catch (NumberFormatException e) {
    //     // Bad result - unexpected.
    //     Slog.w(TAG, "Unable to parse result from cryptfs cryptocomplete");
    //     return ENCRYPTION_STATE_ERROR_UNKNOWN;
    // } catch (NativeDaemonConnectorException e) {
    //     // Something bad happened.
    //     Slog.w(TAG, "Error in communicating with cryptfs in validating");
    //     return ENCRYPTION_STATE_ERROR_UNKNOWN;
    // }
    return NOERROR;
}


CMountService::DecryptStorageRunnable::DecryptStorageRunnable(
    /* [in] */ CMountService* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CMountService::DecryptStorageRunnable, IRunnable)

ECode CMountService::DecryptStorageRunnable::Run()
{
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(String("restart"), (ICharSequence**)&cs);
    args->Set(0, cs);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mHost->mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        Slogger::E(TAG, "problem executing in background");
    }
    // } catch (NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "problem executing in background", e);
    // }
    return NOERROR;
}


ECode CMountService::DecryptStorage(
    /* [in] */ const String& password,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> cs;
    CString::New(password, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        Slogger::E(TAG, "password cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("password cannot be empty");
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CRYPT_KEEPER, String("no permission to access the crypt keeper")));

    WaitForReady();

    if (DEBUG_EVENTS) {
        Slogger::I(TAG, "decrypting storage...");
    }

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1;
    CString::New(String("checkpw"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    args->Set(1, cs);
    if (FAILED(mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        // Decryption failed
        if (event != NULL) {
            *result = event->GetCode();
            return NOERROR;
        }
        *result = 0;
        return NOERROR;
    }

    Int32 code = StringUtils::ParseInt32(event->GetMessage());
    if (code == 0) {
        // Decrypt was successful. Post a delayed message before restarting in order
        // to let the UI to clear itself
        AutoPtr<IRunnable> runnable = new DecryptStorageRunnable(this);
        Boolean result;
        mHandler->Post(runnable, &result);
    }

    *result = code;
    return NOERROR;
    // } catch (NativeDaemonConnectorException e) {
    //     // Decryption failed
    //     return e.getCode();
    // }
}

ECode CMountService::EncryptStorage(
    /* [in] */ const String& password,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> cs;
    CString::New(password, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        Slogger::E(TAG, "password cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("password cannot be empty");
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CRYPT_KEEPER, String("no permission to access the crypt keeper")));

    WaitForReady();

    if (DEBUG_EVENTS) {
        Slogger::I(TAG, "%s: encrypting storage...");
    }

    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(String("enablecrypto"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    CString::New(String("inplace"), (ICharSequence**)&cs2);
    args->Set(1, cs2);
    args->Set(2, cs);
    AutoPtr<NativeDaemonEvent> event;
    if (FAILED(mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        if (event != NULL) {
            // Encryption failed
            *result = event->GetCode();
            return NOERROR;
        }
    }
    // } catch (NativeDaemonConnectorException e) {
    //     // Encryption failed
    //     return e.GetCode();
    // }

    *result = 0;
    return NOERROR;
}

ECode CMountService::ChangeEncryptionPassword(
    /* [in] */ const String& password,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> cs;
    CString::New(password, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        Slogger::E(TAG, "password cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("password cannot be empty");
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CRYPT_KEEPER, String("no permission to access the crypt keeper")));

    WaitForReady();

    if (DEBUG_EVENTS) {
        Slogger::I(TAG, "changing encryption password...");
    }

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1;
    CString::New(String("changepw"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    args->Set(1, cs);
    if (FAILED(mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        // Encryption failed
        if (event != NULL) {
            *result = event->GetCode();
            return NOERROR;
        }
        *result = 0;
        return NOERROR;
    }
    *result = StringUtils::ParseInt32(event->GetMessage());
    return NOERROR;
    // } catch (NativeDaemonConnectorException e) {
    //     // Encryption failed
    //     return e.getCode();
    // }
}

ECode CMountService::VerifyEncryptionPassword(
    /* [in] */ const String& password,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Only the system process is permitted to validate passwords
    if (Binder::GetCallingPid() != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "no permission to access the crypt keeper");
        return E_SECURITY_EXCEPTION;
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CRYPT_KEEPER, String("no permission to access the crypt keeper")));

    AutoPtr<ICharSequence> cs;
    CString::New(password, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        Slogger::E(TAG, "password cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("password cannot be empty");
    }

    WaitForReady();

    if (DEBUG_EVENTS) {
        Slogger::I(TAG, "validating encryption password...");
    }

    AutoPtr<NativeDaemonEvent> event;
    // try {
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs1;
    CString::New(String("verifypw"), (ICharSequence**)&cs1);
    args->Set(0, cs1);
    args->Set(1, cs);
    if (FAILED(mConnector->Execute(String("cryptfs"), args, (NativeDaemonEvent**)&event))) {
        // Encryption failed
        if (event != NULL) {
            *result = event->GetCode();
            return NOERROR;
        }
        *result = 0;
        return NOERROR;
    }
    *result = StringUtils::ParseInt32(event->GetMessage());
    Slogger::I(TAG, "cryptfs verifypw => %d", *result);
    return NOERROR;
    // } catch (NativeDaemonConnectorException e) {
    //     // Encryption failed
    //     return e.getCode();
    // }
}

ECode CMountService::GetVolumeList(
    /* [out, callee] */ ArrayOf<IStorageVolume*>** volList)
{
    VALIDATE_NOT_NULL(volList);
    *volList = NULL;

    Int32 callingUserId = UserHandle::GetCallingUserId();
    Int32 permission;
    FAIL_RETURN(mContext->CheckPermission(
            Elastos::Droid::Manifest::permission::ACCESS_ALL_EXTERNAL_STORAGE,
            Binder::GetCallingPid(), Binder::GetCallingUid(), &permission));
    Boolean accessAll = permission == IPackageManager::PERMISSION_GRANTED;

    AutoLock lock(mVolumesLock);
    List<AutoPtr<IStorageVolume> > filtered;
    List<AutoPtr<IStorageVolume> >::Iterator iter;
    for (iter = mVolumes.Begin(); iter != mVolumes.End(); ++iter) {
        AutoPtr<IStorageVolume> volume = *iter;
        AutoPtr<IUserHandle> owner;
        volume->GetOwner((IUserHandle**)&owner);
        Int32 getIdentifierId;
        Boolean ownerMatch = owner == NULL || (owner->GetIdentifier(&getIdentifierId), getIdentifierId == callingUserId);
        if (accessAll || ownerMatch) {
            filtered.PushBack(volume);
        }
    }

    AutoPtr< ArrayOf<IStorageVolume*> > array = ArrayOf<IStorageVolume*>::Alloc(filtered.GetSize());
    Int32 i = 0;
    for (iter = filtered.Begin(); iter != filtered.End(); ++iter) {
        array->Set(i++, *iter);
    }

    *volList = array;
    REFCOUNT_ADD(*volList);
    return NOERROR;
}

ECode CMountService::AddObbStateLocked(
    /* [in] */ ObbState* obbState)
{
    AutoPtr<IBinder> binder = obbState->GetBinder();
    AutoPtr<ObbStateList> obbStates;
    HashMap<AutoPtr<IBinder>, AutoPtr<ObbStateList> >::Iterator iter = mObbMounts.Find(binder);
    if(iter != mObbMounts.End()) {
        obbStates = iter->mSecond;
    }

    if (obbStates == NULL) {
        obbStates = new List<AutoPtr<ObbState> >();
        mObbMounts[binder] = obbStates;
    }
    else {
        List<AutoPtr<ObbState> >::Iterator iter = obbStates->Begin();
        for(; iter != obbStates->End(); iter++) {
            AutoPtr<ObbState> o = *iter;
            if(o->mRawPath.Equals(obbState->mRawPath)) {
                Slogger::E(TAG, "Attempt to add ObbState twice. This indicates an error in the MountService logic.");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
    }

    obbStates->PushBack(obbState);
    // try {
    ECode ec = obbState->Link();
    if (FAILED(ec)) {
        /*
         * The binder died before we could link it, so clean up our state
         * and return failure.
         */
        obbStates->Remove(obbState);
        if (obbStates->Begin() == obbStates->End()) {
            mObbMounts.Erase(binder);
        }

        // Rethrow the error so mountObb can get it
        return ec;
    }
    // } catch (RemoteException e) {
    //     /*
    //      * The binder died before we could link it, so clean up our state
    //      * and return failure.
    //      */
    //     obbStates.remove(obbState);
    //     if (obbStates.isEmpty()) {
    //         mObbMounts.remove(binder);
    //     }

    //     // Rethrow the error so mountObb can get it
    //     throw e;
    // }
    mObbPathToStateMap[obbState->mRawPath] = obbState;
    return NOERROR;
}

void CMountService::RemoveObbStateLocked(
    /* [in] */ ObbState* obbState)
{
    AutoPtr<IBinder> binder = obbState->GetBinder();
    AutoPtr<ObbStateList> obbStates;
    HashMap<AutoPtr<IBinder>, AutoPtr<ObbStateList> >::Iterator iter = mObbMounts.Find(binder);
    if(iter != mObbMounts.End()) {
        obbStates = iter->mSecond;
    }

    if (obbStates != NULL) {
        AutoPtr<ObbState> temp = obbState;
        obbStates->Remove(obbState);
        temp->Unlink();
        if (obbStates->Begin() == obbStates->End()) {
            mObbMounts.Erase(binder);
        }
    }

    mObbPathToStateMap.Erase(obbState->mRawPath);
}

void CMountService::HandleObbRunAction(
    /* [in] */ ObbAction* action)
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "OBB_RUN_ACTION: %s", action->ToString().string());

    // If a bind was already initiated we don't really
    // need to do anything. The pending install
    // will be processed later on.
    if (!mBound) {
        // If this is the only one pending we might
        // have to bind to the service again.
        if (!ConnectToService()) {
            Slogger::E(TAG, "Failed to bind to media container service");
            action->HandleError();
            return;
        }
    }

    mActions.PushBack(action);
}

void CMountService::HandleObbMcsBound(
    /* [in] */ IMediaContainerService* service)
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "OBB_MCS_BOUND");
    if (service != NULL) {
        mContainerService = service;
    }
    if (mContainerService == NULL) {
        // Something seriously wrong. Bail out
        Slogger::E(TAG, "Cannot bind to media container service");
        List<AutoPtr<ObbAction> >::Iterator it = mActions.Begin();
        for (; it != mActions.End(); ++it) {
            // Indicate service bind error
            (*it)->HandleError();
        }
        mActions.Clear();
    }
    else if (mActions.Begin() != mActions.End()) {
        AutoPtr<ObbAction> action = *mActions.Begin();
        if (action != NULL) {
            action->Execute(mObbActionHandler);
        }
    }
    else {
        // Should never happen ideally.
        Slogger::W(TAG, "Empty queue");
    }
}

void CMountService::HandleObbMcsReconnect()
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "OBB_MCS_RECONNECT");
    if (mActions.Begin() != mActions.End()) {
        if (mBound) {
            DisconnectService();
        }
        if (!ConnectToService()) {
            Slogger::E(TAG, "Failed to bind to media container service");
            List<AutoPtr<ObbAction> >::Iterator it = mActions.Begin();
            for (; it != mActions.End(); ++it) {
                // Indicate service bind error
                (*it)->HandleError();
            }
            mActions.Clear();
        }
    }
}

void CMountService::HandleObbMcsUnbind()
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "OBB_MCS_UNBIND");

    // Delete pending install
    if (mActions.Begin() != mActions.End()) {
        mActions.Erase(mActions.Begin());
    }
    if (mActions.Begin() == mActions.End()) {
        if (mBound) {
            DisconnectService();
        }
    }
    else {
        // There are more pending requests in queue.
        // Just post MCS_BOUND message to trigger processing
        // of next pending install.
        Boolean result;
        mObbActionHandler->SendEmptyMessage(OBB_MCS_BOUND, &result);
    }
}

void CMountService::HandleObbFlushMountState(
    /* [in] */ const String& path)
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "Flushing all OBB state for path %s", (const char*)path);

    AutoLock lock(mObbMountsLock);
    List<AutoPtr<ObbState> > obbStatesToRemove;

    HashMap<String, AutoPtr<ObbState> >::Iterator it = mObbPathToStateMap.Begin();
    while (it != mObbPathToStateMap.End()) {
        AutoPtr<ObbState> state = it->mSecond;

        /*
         * If this entry's source file is in the volume path
         * that got unmounted, remove it because it's no
         * longer valid.
         */
        if (state->mCanonicalPath.StartWith(path)) {
            obbStatesToRemove.PushBack(state);
        }
        ++it;
    }

    List<AutoPtr<ObbState> >::Iterator it2 = obbStatesToRemove.Begin();
    for (; it2 != obbStatesToRemove.End(); ++it2) {
        AutoPtr<ObbState> obbState = *it2;
        if (DEBUG_OBB)
            Slogger::I(TAG, "Removing state for %s", (const char*)obbState->mRawPath);

        RemoveObbStateLocked(obbState);

        // try {
        if (FAILED(obbState->mToken->OnObbResult(obbState->mRawPath, obbState->mNonce,
                IOnObbStateChangeListener::UNMOUNTED))) {
            Slogger::I(TAG, "Couldn't send unmount notification for  OBB: %s", obbState->mRawPath.string());
        }
        // } catch (RemoteException e) {
        //     Slog.i(TAG, "Couldn't send unmount notification for  OBB: "
        //             + obbState.rawPath);
        // }
    }
}

Boolean CMountService::ConnectToService()
{
    if (DEBUG_OBB)
        Slogger::I(TAG, "Trying to bind to DefaultContainerService");

    AutoPtr<IIntent> service;
    CIntent::New((IIntent**)&service);
    service->SetComponent(DEFAULT_CONTAINER_COMPONENT);
    Boolean result;
    if (mContext->BindService(service, mDefContainerConn, IContext::BIND_AUTO_CREATE, &result), result) {
        mBound = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CMountService::DisconnectService()
{
    mContainerService = NULL;
    mBound = FALSE;
    mContext->UnbindService(mDefContainerConn);
}

String CMountService::BuildObbPath(
    /* [in] */ const String& canonicalPath,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean forVold)
{
    // TODO: allow caller to provide Environment for full testing
    // Only adjust paths when storage is emulated
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    Boolean isEmulated;
    if (env->IsExternalStorageEmulated(&isEmulated), !isEmulated) {
        return canonicalPath;
    }

    String path = canonicalPath;

    // First trim off any external storage prefix
    AutoPtr<IUserEnvironment> userEnv;
    CUserEnvironment::New(userId, (IUserEnvironment**)&userEnv);

    // /storage/emulated/0
    AutoPtr<IFile> fp;
    userEnv->GetExternalStorageDirectory((IFile**)&fp);
    String externalPath;
    fp->ToString(&externalPath);
    // /storage/emulated_legacy
    fp = NULL;
    env->GetLegacyExternalStorageDirectory((IFile**)&fp);
    String legacyExternalPath;
    fp->ToString(&legacyExternalPath);

    if (path.StartWith(externalPath)) {
        path = path.Substring(externalPath.GetLength() + 1);
    }
    else if (path.StartWith(legacyExternalPath)) {
        path = path.Substring(legacyExternalPath.GetLength() + 1);
    }
    else {
        return canonicalPath;
    }

    // Handle special OBB paths on emulated storage
    String obbPath("Android/obb");
    if (path.StartWith(obbPath)) {
        path = path.Substring(obbPath.GetLength() + 1);

        if (forVold) {
            AutoPtr<IFile> file, obbSource;
            env->GetEmulatedStorageObbSource((IFile**)&obbSource);
            CFile::New(obbSource, path, (IFile**)&file);
            String result;
            file->ToString(&result);
            return result;
        }
        else {
            AutoPtr<IUserEnvironment> ownerEnv;
            CUserEnvironment::New(IUserHandle::USER_OWNER, (IUserEnvironment**)&ownerEnv);
            AutoPtr<IFile> file;
            AutoPtr<IFile> dir;
            userEnv->GetExternalStorageObbDirectory((IFile**)&dir);
            CFile::New(dir, path, (IFile**)&file);
            String result;
            file->ToString(&result);
            return result;
        }
    }

    // Handle normal external storage paths
    if (forVold) {
        AutoPtr<IFile> file, storageSource;
        env->GetEmulatedStorageSource(userId, (IFile**)&storageSource);
        CFile::New(storageSource, path, (IFile**)&file);
        String result;
        file->ToString(&result);
        return result;
    }
    else {
        //return new File(userEnv.getExternalStorageDirectory(), path).toString();
        AutoPtr<IFile> file;
        AutoPtr<IFile> dir;
        userEnv->GetExternalStorageDirectory((IFile**)&dir);
        CFile::New(dir, path, (IFile**)&file);
        String result;
        file->ToString(&result);
        return result;
    }
}

// void CMountService::Dump(
//     /* [in] */ IFileDescriptor* fd,
//     /* [in] */ IPrintWriter* pw,
//     /* [in] */ ArrayOf<String>* args)
// {
//     if (mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP) != PackageManager::PERMISSION_GRANTED) {
//         pw->Println("Permission Denial: can't dump ActivityManager from from pid="
//                 + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid()
//                 + " without permission " + Elastos::Droid::Manifest::permission::DUMP);
//         return;
//     }

//     {
//         AutoLock lock(mObbMountsLock);
//         pw->Println("  mObbMounts:");

//         final Iterator<Entry<IBinder, List<ObbState>>> binders = mObbMounts.entrySet().iterator();
//         while (binders.hasNext()) {
//             AutoPtr<Entry<IBinder*, List<ObbState*> > > e = binders.next();
//             pw->Print("    Key="); pw->Println(e.getKey().toString());
//             AutoPtr<List<ObbState*> > obbStates = e.getValue();
//             for (final ObbState obbState : obbStates) {
//                 pw->Print("      "); pw->Println(obbState.toString());
//             }
//         }

//         pw->Println("");
//         pw->Println("  mObbPathToStateMap:");
//         final Iterator<Entry<String, ObbState>> maps = mObbPathToStateMap.entrySet().iterator();
//         while (maps.hasNext()) {
//             AutoPtr<Entry<String, ObbState*> > e = maps.next();
//             pw->Print("    "); pw->Print(e.getKey());
//             pw->Print(" -> "); pw->Println(e.getValue().toString());
//         }
//     }

//     pw->Println("");

//     {
//         AutoLock lock(mVolumesLock);
//         pw->Println("  mVolumes:");

//         List<IStorageVolume*>::Iterator iter;
//         for (iter = mVolumes.Begin(); iter != mVolumes.End(); ++iter) {
//             IStorageVolume* v = *iter;
//             pw->Print("    ");
//             pw->Println(v->ToString());
//         }
//     }

//     pw->Println();
//     pw->Println("  mConnection:");
//     mConnector->Dump(fd, pw, args);
// }

ECode CMountService::Monitor()
{
    if (mConnector != NULL) {
        mConnector->Monitor();
    }
    return NOERROR;
}

ECode CMountService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    {
        AutoLock lock(mVolumesLock);
        FAIL_RETURN(ReadStorageListLocked());
    }

    // XXX: This will go away soon in favor of IMountServiceObserver
    mPms = (CPackageManagerService*)IIPackageManager::Probe(ServiceManager::GetService(String("package")));

    CHandlerThread::New(String("MountService"), (IHandlerThread**)&mHandlerThread);
    mHandlerThread->Start();
    AutoPtr<ILooper> looper;
    mHandlerThread->GetLooper((ILooper**)&looper);
    mHandler = new MountServiceHandler(looper, this);

    // Watch for user changes
    AutoPtr<IIntentFilter> userFilter;
    FAIL_RETURN(CIntentFilter::New((IIntentFilter**)&userFilter));
    userFilter->AddAction(IIntent::ACTION_USER_ADDED);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IIntent> resIntent;
    mContext->RegisterReceiver(
            mUserReceiver, userFilter, String(NULL), mHandler, (IIntent**)&resIntent);

    // Watch for USB changes on primary volume
    // AutoPtr<IStorageVolume> primary = GetPrimaryPhysicalVolume();
    // Boolean allowMassStorage = FALSE;
    // if (primary != NULL && (primary->AllowMassStorage(&allowMassStorage), allowMassStorage)) {
    //     AutoPtr<IIntentFilter> filter;
    //     CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&filter);
    //     resIntent = NULL;
    //     mContext->RegisterReceiver(
    //             mUsbReceiver, filter, String(NULL), mHandler, (IIntent**)&resIntent);
    // }

    // if volume support mass storage,we should listen to the states of usb

    List< AutoPtr<IStorageVolume> >::Iterator it = mVolumes.Begin();
    for (; it != mVolumes.End(); ++it) {
        AutoPtr<IStorageVolume> volume = *it;
        Boolean allow, isEmu;
        if ((volume->AllowMassStorage(&allow), allow) && (volume->IsEmulated(&isEmu), !isEmu)) {
            String nullStr;
            AutoPtr<IIntentFilter> filter;
            CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&filter);
            AutoPtr<IIntent> resultIntent;
            mContext->RegisterReceiver(
                    mUsbReceiver, filter, String(NULL), mHandler, (IIntent**)&resultIntent);
            break;
        }
    }

    // Add OBB Action Handler to MountService thread.
    mObbActionHandler = new ObbActionHandler(looper, this);

    /*
     * Create the connection to vold with a maximum queue of twice the
     * amount of containers we'd ever expect to have. This keeps an
     * "asec list" from blocking a thread repeatedly.
     */
    mConnector = new NativeDaemonConnector(this, String("vold"), MAX_CONTAINERS * 2, VOLD_TAG, 25);
    AutoPtr<IThread> thread;
    FAIL_RETURN(CThread::New(mConnector, VOLD_TAG, (IThread**)&thread));
    thread->Start();
    // Add ourself to the Watchdog monitors if enabled.
    // if (WATCHDOG_ENABLE) {
    //     Watchdog.getInstance().addMonitor(this);
    // }

    return NOERROR;
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos

