#include "elastos/droid/providers/media/MtpService.h"
#include "elastos/droid/providers/media/MediaProvider.h"
#include "elastos/droid/providers/media/CIMtpService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/content/ContextWrapper.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/core/Object.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Mtp::CMtpServer;
using Elastos::Droid::Mtp::CMtpStorage;
using Elastos::Droid::Mtp::IMtpServer;
using Elastos::Droid::Mtp::CMtpDatabase;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Storage::CStorageManager;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Os::Storage::EIID_IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Elastos::Droid::Providers::Media::EIID_IMtpService;
using Elastos::Droid::Providers::Media::IMtpService;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::Iterator;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MtpService::TAG("MtpService");
const Boolean MtpService::LOGD = TRUE;

static AutoPtr<ArrayOf<String> > InitPTP_DIRECTORIES()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IEnvironment::DIRECTORY_DCIM;
    (*arr)[1] = IEnvironment::DIRECTORY_PICTURES;
    return arr;
}

const AutoPtr<ArrayOf<String> > MtpService::PTP_DIRECTORIES = InitPTP_DIRECTORIES();
AutoPtr<IMtpServer> MtpService::mServer;

HashMap<String, AutoPtr<IStorageVolume> > MtpService::mVolumeMap;
HashMap<String, AutoPtr<IMtpStorage> > MtpService::mStorageMap;

AutoPtr<IIMtpService> MtpService::mBinder;

//===========================================================
// MtpService::MyStorageEventListener
//===========================================================
CAR_INTERFACE_IMPL(MtpService::MyStorageEventListener, Object, IStorageEventListener)

MtpService::MyStorageEventListener::MyStorageEventListener(
    /* [in] */ MtpService* owner)
    : mOwner(owner)
{}

MtpService::MyStorageEventListener::~MyStorageEventListener()
{}

ECode MtpService::MyStorageEventListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    AutoLock syncLock(mOwner->mBinder);
    Logger::D(TAG, "onStorageStateChanged %s %s -> %s", path.string(), oldState.string(), newState.string());
    if (IEnvironment::MEDIA_MOUNTED.Equals(newState)) {
        mOwner->VolumeMountedLocked(path);
    }
    else if (IEnvironment::MEDIA_MOUNTED.Equals(oldState)) {
        HashMap<String, AutoPtr<IStorageVolume> >::Iterator it = mOwner->mVolumeMap.Find(path);
        AutoPtr<IStorageVolume> volume;
        if (it != mOwner->mVolumeMap.End()) {
            volume = it->mSecond;
            mOwner->mVolumeMap.Erase(it);
        }
        if (volume != NULL) {
            mOwner->RemoveStorageLocked(volume);
        }
    }
    return NOERROR;
}

ECode MtpService::MyStorageEventListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    return NOERROR;
}

//===========================================================
// MtpService::MyRunnable
//===========================================================
MtpService::MyRunnable::MyRunnable(
    /* [in] */ MtpService* owner)
    : mOwner(owner)
{}

ECode MtpService::MyRunnable::Run()
{
    AutoLock syncLock(mOwner->mBinder);
    // Unhide the storage units when the user has unlocked the lockscreen
    if (mOwner->mMtpDisabled) {
        mOwner->AddStorageDevicesLocked();
        mOwner->mMtpDisabled = FALSE;
    }
    return NOERROR;
}

//===========================================================
// MtpService::MyBroadcastReceiver
//===========================================================
MtpService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ MtpService* owner)
    : mOwner(owner)
{}

ECode MtpService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_PRESENT.Equals(action)) {
        // If the media scanner is running, it may currently be calling
        // sendObjectAdded/Removed, which also synchronizes on mBinder
        // (and in addition to that, all the native MtpServer methods
        // lock the same Mutex). If it happens to be in an mtp device
        // write(), it may block for some time, so process this broadcast
        // in a thread.
        AutoPtr<MyRunnable> runnable = new MyRunnable(mOwner);
        AutoPtr<Thread> thread = new Thread();
        thread->constructor(IRunnable::Probe(runnable), String("addStorageDevices"));
        thread->Start();
    }
    return NOERROR;
}

//===========================================================
// MtpService::MyIMtpService
//===========================================================

CAR_INTERFACE_IMPL_2(MtpService::MyIMtpService, Object, IIMtpService, IBinder)

MtpService::MyIMtpService::MyIMtpService()
{}

MtpService::MyIMtpService::~MyIMtpService()
{}

ECode MtpService::MyIMtpService::constructor(
    /* [in] */ IMtpService* owner)
{
    mOwner = (MtpService*)owner;
    return NOERROR;
}

ECode MtpService::MyIMtpService::SendObjectAdded(
    /* [in] */ Int32 objectHandle)
{
    AutoLock syncLock(mOwner->mBinder);
    if (mServer != NULL) {
        return mOwner->mServer->SendObjectAdded(objectHandle);
    }
    return NOERROR;
}

ECode MtpService::MyIMtpService::SendObjectRemoved(
    /* [in] */ Int32 objectHandle)
{
    AutoLock syncLock(mOwner->mBinder);
    if (mServer != NULL) {
        return mServer->SendObjectRemoved(objectHandle);
    }
    return NOERROR;
}

ECode MtpService::MyIMtpService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//===========================================================
// MtpService
//===========================================================
CAR_INTERFACE_IMPL(MtpService, Service, IMtpService)

MtpService::MtpService()
    : mMtpDisabled(FALSE)
    , mPtpMode(FALSE)
{
}

MtpService::~MtpService()
{}

ECode MtpService::constructor()
{
    Logger::I(TAG, " >> constructor()");
    AutoPtr<IBroadcastReceiver> mReceiver = new MyBroadcastReceiver(this);
    AutoPtr<IStorageEventListener> mStorageEventListener = new MyStorageEventListener(this);
    CIMtpService::New(this, (IIMtpService**)&mBinder);
    return Service::constructor();
}

ECode MtpService::OnCreate()
{
    Logger::I(TAG, " >> OnCreate()");
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_USER_PRESENT, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mReceiver, intentFilter, (IIntent**)&intent);

    AutoPtr<IStorageManagerHelper> smr;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&smr);
    smr->From((IContext*)this, (IStorageManager**)&mStorageManager);
    {
        AutoLock syncLock(mBinder);
        UpdateDisabledStateLocked();
        mStorageManager->RegisterListener(mStorageEventListener);
        AutoPtr<ArrayOf<IStorageVolume*> > volumes;
        mStorageManager->GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);

        mVolumes = volumes;
        String path, state;
        for (Int32 i = 0; i < volumes->GetLength(); i++) {
            (*volumes)[i]->GetPath(&path);
            mStorageManager->GetVolumeState(path, &state);
            if (IEnvironment::MEDIA_MOUNTED.Equals(state)) {
                VolumeMountedLocked(path);
            }
        }
    }
    return NOERROR;
}

ECode MtpService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    Logger::I(TAG, " >> OnStartCommand()");

    AutoLock syncLock(mBinder);
    UpdateDisabledStateLocked();
    Boolean flag = FALSE;
    intent->GetBooleanExtra(IUsbManager::USB_FUNCTION_PTP, FALSE, &flag);
    mPtpMode = (intent == NULL ? FALSE : flag);
    AutoPtr<ArrayOf<String> > subdirs;
    if (mPtpMode) {
        Int32 count = PTP_DIRECTORIES->GetLength();
        subdirs = ArrayOf<String>::Alloc(count);
        AutoPtr<IFile> file;
        String path;
        for (Int32 i = 0; i < count; i++) {
            file = NULL;
            file = Environment::GetExternalStoragePublicDirectory((*PTP_DIRECTORIES)[i]);
            // make sure this directory exists
            file->Mkdirs(&flag);
            file->GetPath(&path);
            (*subdirs)[i] = path;
        }
    }
    AutoPtr<IStorageManagerHelper> smh;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&smh);
    AutoPtr<IStorageVolume> primary;
    smh->GetPrimaryVolume(mVolumes, (IStorageVolume**)&primary);
    if (mDatabase != NULL) {
        mDatabase->SetServer(NULL);
    }
    String path;
    primary->GetPath(&path);
    CMtpDatabase::New((IContext*)this, MediaProvider::EXTERNAL_VOLUME,
        path, subdirs, (IMtpDatabase**)&mDatabase);
    ManageServiceLocked();

    *result = IService::START_STICKY;
    return NOERROR;
}

ECode MtpService::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    UnregisterReceiver(mReceiver);
    mStorageManager->UnregisterListener(mStorageEventListener);
    if (mDatabase != NULL) {
        return mDatabase->SetServer(NULL);
    }
    return NOERROR;
}

ECode MtpService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    Logger::I(TAG, " >> OnBind()");

    VALIDATE_NOT_NULL(result)
    *result = IBinder::Probe(mBinder);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void MtpService::AddStorageDevicesLocked()
{
    if (mPtpMode) {
        // In PTP mode we support only primary storage
        AutoPtr<IStorageManagerHelper> smh;
        CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&smh);
        AutoPtr<IStorageVolume> primary;
        smh->GetPrimaryVolume(mVolumes, (IStorageVolume**)&primary);
        String path;
        primary->GetPath(&path);
        if (!path.IsNull()) {
            String state;
            mStorageManager->GetVolumeState(path, &state);
            if (IEnvironment::MEDIA_MOUNTED.Equals(state)) {
                AddStorageLocked(mVolumeMap[path]);
            }
        }
    } else {
        HashMap<String, AutoPtr<IStorageVolume> >::Iterator it = mVolumeMap.Begin();
        AutoPtr<IStorageVolume> volume;
        for(; it != mVolumeMap.End(); ++it) {
            volume = NULL;
            volume = it->mSecond;
            AddStorageLocked(volume);
        }
    }
}

void MtpService::UpdateDisabledStateLocked()
{
    AutoPtr<IActivityManagerHelper> amh;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amh);
    Int32 currentUser;
    amh->GetCurrentUser(&currentUser);
    Boolean isCurrentUser = UserHandle::GetMyUserId() == currentUser;
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&obj);
    AutoPtr<IKeyguardManager> keyguardManager = IKeyguardManager::Probe(obj);
    Boolean flag = FALSE;
    keyguardManager->IsKeyguardLocked(&flag);
    Boolean isKs = FALSE;
    keyguardManager->IsKeyguardSecure(&isKs);
    mMtpDisabled = (flag && isKs) || !isCurrentUser;
    if (LOGD) {
        Logger::D(TAG, "updating state; isCurrentUser=%d, mMtpLocked=%d", isCurrentUser, mMtpDisabled);
    }
}

void MtpService::ManageServiceLocked()
{
    AutoPtr<IActivityManagerHelper> amh;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amh);
    Int32 currentUser;
    amh->GetCurrentUser(&currentUser);
    Boolean isCurrentUser = UserHandle::GetMyUserId() == currentUser;
    if (mServer == NULL && isCurrentUser) {
        if (mPtpMode) {
            Logger::D(TAG, "starting MTP server in PTP mode");
        } else {
            Logger::D(TAG, "starting MTP server in MTP mode");
        }
        CMtpServer::New(mDatabase, mPtpMode, (IMtpServer**)&mServer);
        mDatabase->SetServer(mServer);
        if (!mMtpDisabled) {
            AddStorageDevicesLocked();
        }
        mServer->Start();
    } else if (mServer != NULL && !isCurrentUser) {
        Logger::D(TAG, "no longer current user; shutting down MTP server");
        // Internally, kernel will close our FD, and server thread will
        // handle cleanup.
        mServer = NULL;
        mDatabase->SetServer(NULL);
    }
}

void MtpService::VolumeMountedLocked(
    /* [in] */ const String& path)
{
    Int32 length = mVolumes->GetLength();
    AutoPtr<IStorageVolume> volume;
    String ph;
    Boolean flag = FALSE;
    for (Int32 i = 0; i < length; i++) {
        volume = NULL;
        volume = (*mVolumes)[i];
        volume->GetPath(&ph);
        if (ph.Equals(path)) {
            mVolumeMap[path] = volume;
            if (!mMtpDisabled) {
                // In PTP mode we support only primary storage
                if ((volume->IsPrimary(&flag), flag) || !mPtpMode) {
                    AddStorageLocked(volume);
                }
            }
            break;
        }
    }
}

void MtpService::AddStorageLocked(
    /* [in] */ IStorageVolume* volume)
{
    AutoPtr<IMtpStorage> storage;
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    CMtpStorage::New(volume, context, (IMtpStorage**)&storage);
    String path;
    storage->GetPath(&path);
    mStorageMap[path] = storage;

    Int32 vol;
    storage->GetStorageId(&vol);
    Logger::D(TAG, "addStorageLocked %d %s", vol, path.string());
    if (mDatabase != NULL) {
        mDatabase->AddStorage(storage);
    }
    if (mServer != NULL) {
        mServer->AddStorage(storage);
    }
}

void MtpService::RemoveStorageLocked(
    /* [in] */ IStorageVolume* volume)
{
    String path;
    volume->GetPath(&path);
    AutoPtr<IMtpStorage> storage = mStorageMap[path];
    if (storage == NULL) {
        Logger::E(TAG, "no MtpStorage for %s", path.string());
        mStorageMap.Erase(path);
        return;
    }
    Int32 storageId;
    storage->GetStorageId(&storageId);
    storage->GetPath(&path);
    Logger::D(TAG, "removeStorageLocked %d %s", storageId, path.string());
    if (mDatabase != NULL) {
        mDatabase->RemoveStorage(storage);
    }
    if (mServer != NULL) {
        mServer->RemoveStorage(storage);
    }
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos
