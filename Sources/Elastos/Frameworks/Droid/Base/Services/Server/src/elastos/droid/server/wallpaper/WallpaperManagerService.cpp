
#include "elastos/droid/server/wallpaper/WallpaperManagerService.h"
#include "elastos/droid/server/wallpaper/CWallpaperConnection.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
// #include "elstos/droid/os/FileUtils.h"
// #include "elstos/droid/os/SELinux.h"
// #include "elstos/droid/utility/Xml.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include "elastos/utility/logging/Slogger.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::App::CContextImpl;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::CWallpaperInfo;
using Elastos::Droid::App::CWallpaperManager;
using Elastos::Droid::App::EIID_IIWallpaperManager;
using Elastos::Droid::App::EIID_IIUserSwitchObserver;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::App::Backup::IWallpaperBackupHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CContextWrapper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CJournaledFile;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
// using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Server::Wallpaper::CWallpaperConnection;
using Elastos::Droid::Service::Wallpaper::EIID_IIWallpaperConnection;
using Elastos::Droid::Service::Wallpaper::IWallpaperService;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Manifest;
using Elastos::Droid::R;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wallpaper {

//==========================================
// WallpaperManagerService::WallpaperObserver
//==========================================

WallpaperManagerService::WallpaperObserver::WallpaperObserver(
    /* [in] */ WallpaperData* wallpaper,
    /* [in] */ WallpaperManagerService* host)
    : mWallpaper(wallpaper)
    , mHost(host)
{
    Int32 userId = wallpaper->mUserId;
    AutoPtr<IFile> file;
    mHost->GetWallpaperDir(userId, (IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    FileObserver::constructor(path, IFileObserver::CLOSE_WRITE |
        IFileObserver::MOVED_TO | IFileObserver::DELETE | IFileObserver::DELETE_SELF);
    mWallpaperDir = file;
    AutoPtr<IFile> wfile;
    CFile::New(mWallpaperDir, WallpaperManagerService::WALLPAPER, (IFile**)&wfile);
    mWallpaperFile = wfile;
}

ECode WallpaperManagerService::WallpaperObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    if (path.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    synchronized(this) {
        // changing the wallpaper means we'll need to back up the new one
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int64 origId;
        bh->ClearCallingIdentity(&origId);
        #if 0 //waiting CBackupManager attend compile
        AutoPtr<IBackupManager> bm;
        CBackupManager::New(mHost->mContext, (IBackupManager**)&bm);
        bm->DataChanged();
        #endif
        bh->RestoreCallingIdentity(origId);

        AutoPtr<IFile> changedFile;
        CFile::New(mWallpaperDir, path, (IFile**)&changedFile);
        Boolean isEquals;
        IObject::Probe(mWallpaperFile)->Equals(IInterface::Probe(changedFile), &isEquals);
        if (isEquals) {
            mHost->NotifyCallbacksLocked(mWallpaper.Get());
            const Boolean written = (event == IFileObserver::CLOSE_WRITE || event == IFileObserver::MOVED_TO);
            if (mWallpaper->mWallpaperComponent == NULL
                    || event != IFileObserver::CLOSE_WRITE // includes the MOVED_TO case
                    || mWallpaper->mImageWallpaperPending) {
                if (written) {
                    mWallpaper->mImageWallpaperPending = FALSE;
                }
                Boolean bwresult;
                mHost->BindWallpaperComponentLocked(mHost->mImageWallpaper, TRUE,
                        FALSE, mWallpaper, NULL, &bwresult);
                mHost->SaveSettingsLocked(mWallpaper);
            }
        }
    }
    return NOERROR;
}


//==========================================
// WallpaperManagerService::WallpaperData
//==========================================

CAR_INTERFACE_IMPL(WallpaperManagerService::WallpaperData, Object, IWallpaperData)

WallpaperManagerService::WallpaperData::WallpaperData(
    /* [in] */ Int32 userId,
    /* [in] */ WallpaperManagerService* host)
    : mUserId(userId)
    , mLastDiedTime(0L)
    , mWallpaperUpdating(FALSE)
    , mHost(host)
    , mWidth(-1)
    , mHeight(-1)
{
    AutoPtr<IFile> file;
    mHost->GetWallpaperDir(userId, (IFile**)&file);
    CFile::New(file.Get(), WallpaperManagerService::WALLPAPER, (IFile**)&mWallpaperFile);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks);
    CRect::New(0, 0, 0, 0, (IRect**)&mPadding);
}

//==========================================
// WallpaperManagerService::WallpaperConnection
//==========================================

CAR_INTERFACE_IMPL_4(WallpaperManagerService::WallpaperConnection, Object, IWallpaperConnection, IIWallpaperConnection, IBinder, IServiceConnection)

WallpaperManagerService::WallpaperConnection::WallpaperConnection()
{
}

WallpaperManagerService::WallpaperConnection::WallpaperConnection(
    /*  [in] */ WallpaperManagerService* host)
    : mDimensionsChanged(FALSE)
    , mPaddingChanged(FALSE)
    , mHost(host)
{
    CBinder::New((IBinder**)&mToken);
}

ECode WallpaperManagerService::WallpaperConnection::constructor(
    /* [in] */ IWallpaperInfo* info,
    /* [in] */ IWallpaperData* wallpaper)
{
    mInfo = info;
    mWallpaper = wallpaper;
    return NOERROR;
}

ECode WallpaperManagerService::WallpaperConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    synchronized(this) {
        AutoPtr<WallpaperData> _mWallpaper = (WallpaperData*)mWallpaper.Get();
        if ((_mWallpaper->mConnection).Get() == this) {
            mService = IIWallpaperService::Probe(service);
            mHost->AttachServiceLocked(this, mWallpaper);
            // XXX should probably do saveSettingsLocked() later
            // when we have an engine, but I'm not sure about
            // locking there and anyway we always need to be able to
            // recover if there is something wrong.
            mHost->SaveSettingsLocked(_mWallpaper);
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::WallpaperConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    synchronized(this) {
        mService = NULL;
        mEngine = NULL;
        AutoPtr<WallpaperData> _mWallpaper = (WallpaperData*)mWallpaper.Get();
        if ((_mWallpaper->mConnection).Get() == this) {
            String str;
            IObject::Probe(_mWallpaper->mWallpaperComponent)->ToString(&str);
            Slogger::W(WallpaperManagerService::TAG, "Wallpaper service gone: %s", str.string());
            if (!_mWallpaper->mWallpaperUpdating
                    && _mWallpaper->mUserId == mHost->mCurrentUserId) {
                // There is a race condition which causes
                // {@link #mWallpaper.wallpaperUpdating} to be false even if it is
                // currently updating since the broadcast notifying us is async.
                // This race is overcome by the general rule that we only reset the
                // wallpaper if its service was shut down twice
                // during {@link #MIN_WALLPAPER_CRASH_TIME} millis.
                #if 0 // TODO need CSystemClockHelper
                if (_mWallpaper->mLastDiedTime != 0
                        && _mWallpaper->mLastDiedTime + WallpaperManagerService::MIN_WALLPAPER_CRASH_TIME
                            > SystemClock::GetUptimeMillis()) {
                    Slogger::W(WallpaperManagerService::TAG, "Reverting to built-in wallpaper!");
                    mHost->ClearWallpaperLocked(TRUE, _mWallpaper->mUserId, NULL);
                } else {
                    _mWallpaper->mLastDiedTime = SystemClock::GetUptimeMillis();
                }
                #endif
            }
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::WallpaperConnection::AttachEngine(
    /* [in] */ IIWallpaperEngine* engine)
{
    synchronized(this) {
        AutoPtr<WallpaperData> _mWallpaper = (WallpaperData*)mWallpaper.Get();
        mEngine = engine;
        if (mDimensionsChanged) {
            ECode ec = mEngine->SetDesiredSize(_mWallpaper->mWidth, _mWallpaper->mHeight);
            if (FAILED(ec)) {
                Slogger::W(WallpaperManagerService::TAG, "Failed to set wallpaper dimensions");
                return E_REMOTE_EXCEPTION;
            }
            mDimensionsChanged = FALSE;
        }
        if (mPaddingChanged) {
            ECode ec = mEngine->SetDisplayPadding(_mWallpaper->mPadding);
            if (FAILED(ec)) {
                Slogger::W(WallpaperManagerService::TAG, "Failed to set wallpaper padding");
                return E_REMOTE_EXCEPTION;
            }
            mPaddingChanged = FALSE;
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::WallpaperConnection::EngineShown(
    /* [in] */ IIWallpaperEngine* engine)
{
    synchronized(this) {
        if (mReply != NULL) {
            AutoPtr<IBinderHelper> bh;
            CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
            Int64 ident;
            bh->ClearCallingIdentity(&ident);
            ECode ec = mReply->SendResult(NULL);
            if (FAILED(ec)) {
                bh->RestoreCallingIdentity(ident);
                return E_REMOTE_EXCEPTION;
            }
            mReply = NULL;
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::WallpaperConnection::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    synchronized(this) {
        AutoPtr<WallpaperData> _mWallpaper = (WallpaperData*)mWallpaper.Get();
        if ((_mWallpaper->mConnection).Get() == this) {
            return mHost->UpdateWallpaperBitmapLocked(name, mWallpaper, descriptor);
        }
        *descriptor = NULL;
    }
    return NOERROR;
}

//==========================================
// WallpaperManagerService::MyPackageMonitor
//==========================================

WallpaperManagerService::MyPackageMonitor::MyPackageMonitor(
    /*  [in] */ WallpaperManagerService* host)
    : mHost(host)
{
}

ECode WallpaperManagerService::MyPackageMonitor::OnPackageUpdateFinished(
    /* [in] */ const String& packageName,
    /* [in] */  Int32 uid)
{
    synchronized(this) {
        Int32 userId;
        GetChangingUserId(&userId);
        if (mHost->mCurrentUserId != userId) {
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mHost->mWallpaperMap->Get(mHost->mCurrentUserId, (IInterface**)&obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)(IWallpaperData::Probe(obj));
        if (wallpaper != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            if (wallpaper->mWallpaperComponent != NULL
                    && pn.Equals(packageName)) {
                wallpaper->mWallpaperUpdating = FALSE;
                AutoPtr<IComponentName> comp = wallpaper->mWallpaperComponent;
                mHost->ClearWallpaperComponentLocked(wallpaper);
                Boolean result;
                mHost->BindWallpaperComponentLocked(comp.Get(), FALSE, FALSE, wallpaper, NULL, &result);
                if (!result) {
                    Slogger::W(WallpaperManagerService::TAG, "Wallpaper no longer available; reverting to default");
                    mHost->ClearWallpaperLocked(FALSE, wallpaper->mUserId, NULL);
                }
            }
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::MyPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    synchronized(this) {
        Int32 userId;
        GetChangingUserId(&userId);
        if (mHost->mCurrentUserId != userId) {
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mHost->mWallpaperMap->Get(mHost->mCurrentUserId, (IInterface**)&obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)(IWallpaperData::Probe(obj));
        if (wallpaper != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            if (wallpaper->mWallpaperComponent == NULL
                    || !pn.Equals(packageName)) {
                return E_NULL_POINTER_EXCEPTION;
            }
            Boolean result;
            DoPackagesChangedLocked(TRUE, wallpaper, &result);
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::MyPackageMonitor::OnPackageUpdateStarted(
    /* [in] */ const String& packageName,
    /* [in] */  Int32 uid)
{
    synchronized(this) {
        Int32 userId;
        GetChangingUserId(&userId);
        if (mHost->mCurrentUserId != userId) {
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mHost->mWallpaperMap->Get(mHost->mCurrentUserId, (IInterface**)&obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)(IWallpaperData::Probe(obj));
        if (wallpaper != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            if (wallpaper->mWallpaperComponent != NULL
                    && pn.Equals(packageName)) {
                wallpaper->mWallpaperUpdating = TRUE;
            }
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(this) {
        Int32 userId;
        GetChangingUserId(&userId);
        if (mHost->mCurrentUserId != userId) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        mHost->mWallpaperMap->Get(mHost->mCurrentUserId, (IInterface**)&obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)(IWallpaperData::Probe(obj));
        Boolean changed = FALSE;
        if (wallpaper != NULL) {
            Boolean res;
            DoPackagesChangedLocked(doit, wallpaper, &res);
            changed |= res;
        }
        *result = changed;
        return NOERROR;
    }
    return NOERROR;
}

ECode WallpaperManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    synchronized(this) {
        Int32 userId;
        GetChangingUserId(&userId);
        if (mHost->mCurrentUserId != userId) {
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mHost->mWallpaperMap->Get(mHost->mCurrentUserId, (IInterface**)&obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)(IWallpaperData::Probe(obj));
        if (wallpaper != NULL) {
            Boolean res;
            DoPackagesChangedLocked(TRUE, wallpaper, &res);
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::MyPackageMonitor::DoPackagesChangedLocked(
    /* [in] */ Boolean doit,
    /* [in] */ IWallpaperData* wallpaper,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)(wallpaper);
    Boolean changed = FALSE;
    if (_wallpaper->mWallpaperComponent != NULL) {
        String pn;
        _wallpaper->mWallpaperComponent->GetPackageName(&pn);
        Int32 change;
        IsPackageDisappearing(pn, &change);
        if (change == IPackageMonitor::PACKAGE_PERMANENT_CHANGE
            || change == IPackageMonitor::PACKAGE_TEMPORARY_CHANGE) {
            changed = TRUE;
            if (doit) {
                String str;
                IObject::Probe(_wallpaper->mWallpaperComponent)->ToString(&str);
                Slogger::W(WallpaperManagerService::TAG, "Wallpaper uninstalled, removing: %s", str.string());
                mHost->ClearWallpaperLocked(FALSE, _wallpaper->mUserId, NULL);
            }
        }
    }
    if (_wallpaper->mNextWallpaperComponent != NULL) {
        Int32 change;
        String pn;
        _wallpaper->mNextWallpaperComponent->GetPackageName(&pn);
        IsPackageDisappearing(pn, &change);
        if (change == IPackageMonitor::PACKAGE_PERMANENT_CHANGE
                || change == IPackageMonitor::PACKAGE_TEMPORARY_CHANGE) {
            _wallpaper->mNextWallpaperComponent = NULL;
        }
    }

    String wcpn;
    _wallpaper->mWallpaperComponent->GetPackageName(&wcpn);
    Boolean isPackageModified = FALSE;
    IsPackageModified(wcpn, &isPackageModified);
    if (_wallpaper->mWallpaperComponent != NULL && isPackageModified) {
        AutoPtr<IPackageManager> pm;
        mHost->mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IServiceInfo> info;
        ECode ec = pm->GetServiceInfo(_wallpaper->mWallpaperComponent, 0, (IServiceInfo**)&info);
        if (FAILED(ec)) {
            String str;
            IObject::Probe(_wallpaper->mWallpaperComponent)->ToString(&str);
            Slogger::W(WallpaperManagerService::TAG, "Wallpaper component gone, removing: %s", str.string());
            mHost->ClearWallpaperLocked(FALSE, _wallpaper->mUserId, NULL);
            return E_NAME_NOT_FOUND_EXCEPTION;
        }
    }

    String nwcpn;
    _wallpaper->mNextWallpaperComponent->GetPackageName(&nwcpn);
    IsPackageModified(nwcpn, &isPackageModified);
    if (_wallpaper->mNextWallpaperComponent != NULL && isPackageModified) {
        AutoPtr<IPackageManager> pm;
        mHost->mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IServiceInfo> info;
        ECode ec = pm->GetServiceInfo(_wallpaper->mNextWallpaperComponent, 0, (IServiceInfo**)&info);
        if (FAILED(ec)) {
            _wallpaper->mNextWallpaperComponent = NULL;
            return E_NAME_NOT_FOUND_EXCEPTION;
        }
    }
    *result = changed;
    return NOERROR;
}

//==========================================
// WallpaperManagerService::SystemRunningBR
//==========================================

WallpaperManagerService::SystemRunningBR::SystemRunningBR(
    /* [in] */ WallpaperManagerService* host)
    : mHost(host)
{
}

ECode WallpaperManagerService::SystemRunningBR::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 value;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &value);
        mHost->OnRemoveUser(value);
    }
    // TODO: Race condition causing problems when cleaning up on stopping a user.
    // Comment this out for now.
    // else if (Intent.ACTION_USER_STOPPING.equals(action)) {
    //     onStoppingUser(intent.getIntExtra(Intent.EXTRA_USER_HANDLE,
    //             UserHandle.USER_NULL));
    // }
    return NOERROR;
}

//==========================================
// WallpaperManagerService::SystemRunningObserver
//==========================================

CAR_INTERFACE_IMPL_2(WallpaperManagerService::SystemRunningObserver, Object, IIUserSwitchObserver, IBinder)

WallpaperManagerService::SystemRunningObserver::SystemRunningObserver(
    /* [in] */ WallpaperManagerService* host)
    : mHost(host)
{
}

ECode WallpaperManagerService::SystemRunningObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    return mHost->SwitchUser(newUserId, IIRemoteCallback::Probe(reply));
}

ECode WallpaperManagerService::SystemRunningObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    return NOERROR;
}

//==========================================
// WallpaperManagerService
//==========================================

const String WallpaperManagerService::TAG("WallpaperManagerService");
const Boolean WallpaperManagerService::DEBUG = FALSE;

const Int64 WallpaperManagerService::MIN_WALLPAPER_CRASH_TIME;
const String WallpaperManagerService::WALLPAPER("wallpaper");
const String WallpaperManagerService::WALLPAPER_INFO("wallpaper_info.xml");

CAR_INTERFACE_IMPL_3(WallpaperManagerService, Object, IWallpaperManagerService, IIWallpaperManager, IBinder)

WallpaperManagerService::WallpaperManagerService()
{
    CSparseArray::New((ISparseArray**)&mWallpaperMap);
}

ECode WallpaperManagerService::constructor(
    /* [in] */ IContext* context)
{
    if (DEBUG) Slogger::V(TAG, "WallpaperService startup");
    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String str;
    res->GetString(R::string::image_wallpaper_component, &str);
    AutoPtr<IComponentNameHelper> cnh;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnh);
    cnh->UnflattenFromString(str, (IComponentName**)&mImageWallpaper);

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> obj;
    sm->GetService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mIWindowManager = IIWindowManager::Probe(obj);

    mIPackageManager = AppGlobals::GetPackageManager();
    mMonitor = new MyPackageMonitor(this);
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    AutoPtr<IUserHandle> uh;
    uhh->GetALL((IUserHandle**)&uh);
    mMonitor->Register(context, NULL, uh.Get(), TRUE);
    AutoPtr<IFile> wallpaperDir;
    GetWallpaperDir(IUserHandle::USER_OWNER, (IFile**)&wallpaperDir);
    Boolean succeeded;
    wallpaperDir->Mkdirs(&succeeded);
    LoadSettingsLocked(IUserHandle::USER_OWNER);
    return NOERROR;
}

ECode WallpaperManagerService::GetWallpaperDir(
    /* [in] */ Int32 userId,
    /* [out] */ IFile** wallpaperDir)
{
    VALIDATE_NOT_NULL(wallpaperDir)
    AutoPtr<IEnvironment> ie;
    CEnvironment::AcquireSingleton((IEnvironment**)&ie);
    ie->GetUserSystemDirectory(userId, wallpaperDir);
    return NOERROR;
}

ECode WallpaperManagerService::Finalize()
{
    // super.finalize();
    Int32 size;
    mWallpaperMap->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> outface;
        mWallpaperMap->ValueAt(i, (IInterface**)&outface);
        AutoPtr<IWallpaperData> wallpaper = IWallpaperData::Probe(outface.Get());
        AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)wallpaper.Get();
        _wallpaper->mWallpaperObserver->StopWatching();
    }
    return NOERROR;
}

ECode WallpaperManagerService::SystemRunning()
{
    if (DEBUG) Slogger::V(TAG, "systemReady");
    AutoPtr<IInterface> obj;
    mWallpaperMap->Get(IUserHandle::USER_OWNER, (IInterface**)&obj);
    AutoPtr<IWallpaperData> wallpaper = IWallpaperData::Probe(obj);
    SwitchWallpaper(wallpaper, NULL);
    AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)wallpaper.Get();
    _wallpaper->mWallpaperObserver = new WallpaperObserver(_wallpaper, this);
    _wallpaper->mWallpaperObserver->StartWatching();

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    userFilter->AddAction(IIntent::ACTION_USER_STOPPING);
    AutoPtr<SystemRunningBR> srbr = new SystemRunningBR(this);
    AutoPtr<IIntent> strickyIntent;
    mContext->RegisterReceiver((BroadcastReceiver*)srbr.Get(), userFilter.Get(), (IIntent**)&strickyIntent);

    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<SystemRunningObserver> observer = new SystemRunningObserver(this);
    ECode ec = am->RegisterUserSwitchObserver((IIUserSwitchObserver*)observer);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode WallpaperManagerService::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    // Verify caller is the system
    AutoPtr<IBinderHelper> bh;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
    Int32 pid;
    bh->GetCallingUid(&pid);
    if (pid != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "getName() can only be called from the system process");
        return E_RUNTIME_EXCEPTION;
    }
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(0, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> _wd = (WallpaperData*)wd.Get();
        *name = _wd->mName;
        return NOERROR;
    }
    return NOERROR;
}

ECode WallpaperManagerService::OnStoppingUser(
    /* [in] */ Int32 userId)
{
    if (userId < 1) return E_NULL_POINTER_EXCEPTION;
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();
        if (wallpaper != NULL) {
            if (wallpaper->mWallpaperObserver != NULL) {
                wallpaper->mWallpaperObserver->StopWatching();
                wallpaper->mWallpaperObserver = NULL;
            }
            mWallpaperMap->Remove(userId);
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::OnRemoveUser(
    /* [in] */ Int32 userId)
{
    if (userId < 1) return E_NULL_POINTER_EXCEPTION;
    synchronized(this) {
        OnStoppingUser(userId);
        AutoPtr<IFile> file;
        GetWallpaperDir(userId, (IFile**)&file);
        AutoPtr<IFile> wallpaperFile;
        CFile::New(file.Get(), WALLPAPER, (IFile**)&wallpaperFile);
        wallpaperFile->Delete();

        AutoPtr<IFile> wallpaperInfoFile;
        CFile::New(file.Get(), WALLPAPER_INFO, (IFile**)&wallpaperInfoFile);
        wallpaperInfoFile->Delete();
    }
    return NOERROR;
}

ECode WallpaperManagerService::SwitchUser(
    /* [in] */ Int32 userId,
    /* [in] */ IIRemoteCallback* reply)
{
    synchronized(this) {
        mCurrentUserId = userId;
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

        if (wallpaper == NULL) {
            wallpaper = new WallpaperData(userId, this);
            mWallpaperMap->Put(userId, IInterface::Probe((IWallpaperData*)wallpaper));
            LoadSettingsLocked(userId);
        }
        // Not started watching yet, in case wallpaper data was loaded for other reasons.
        if (wallpaper->mWallpaperObserver == NULL) {
            wallpaper->mWallpaperObserver = new WallpaperObserver(wallpaper, this);
            wallpaper->mWallpaperObserver->StartWatching();
        }
        SwitchWallpaper((IWallpaperData*)wallpaper, reply);
    }
    return NOERROR;
}

ECode WallpaperManagerService::SwitchWallpaper(
    /* [in] */ IWallpaperData* wallpaper,
    /* [in] */ IIRemoteCallback* reply)
{
    synchronized(this) {
        // RuntimeException e = null;
        // try {
        AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)wallpaper;
        AutoPtr<IComponentName> cname = _wallpaper->mWallpaperComponent != NULL ?
            _wallpaper->mWallpaperComponent : _wallpaper->mNextWallpaperComponent;

        Boolean isBind;
        BindWallpaperComponentLocked(cname.Get(), TRUE, FALSE, wallpaper, reply, &isBind);
        if (isBind) {
            return E_NULL_POINTER_EXCEPTION;
        }
        // } catch (RuntimeException e1) {
        //     e = e1;
        // }
        Slogger::W(TAG, "Failure starting previous wallpaper");
        ClearWallpaperLocked(FALSE, _wallpaper->mUserId, reply);
    }
    return NOERROR;
}

ECode WallpaperManagerService::ClearWallpaper()
{
    if (DEBUG) Slogger::V(TAG, "clearWallpaper");
    synchronized(this) {
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);
        ClearWallpaperLocked(FALSE, userId, NULL);
    }
    return NOERROR;
}

ECode WallpaperManagerService::ClearWallpaperLocked(
    /* [in] */ Boolean defaultFailed,
    /* [in] */ Int32 userId,
    /* [in] */ IIRemoteCallback* reply)
{
    AutoPtr<IInterface> obj;
    mWallpaperMap->Get(userId, (IInterface**)&obj);
    AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

    AutoPtr<IFile> file;
    GetWallpaperDir(userId, (IFile**)&file);
    AutoPtr<IFile> f;
    CFile::New(file.Get(), WALLPAPER, (IFile**)&f);

    Boolean isExist;
    if (f->Exists(&isExist), isExist) {
        f->Delete();
    }
    AutoPtr<IBinderHelper> bh;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
    Int64 _ident;
    bh->ClearCallingIdentity(&_ident);
    const Int64 ident = _ident;

    wallpaper->mImageWallpaperPending = FALSE;
    if (userId != mCurrentUserId) return E_NULL_POINTER_EXCEPTION;
    Boolean isBind;
    ECode ec = BindWallpaperComponentLocked(defaultFailed ? mImageWallpaper : NULL,
        TRUE, FALSE, wallpaper.Get(), reply, &isBind);
    if (isBind) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if(FAILED(ec)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // This can happen if the default wallpaper component doesn't
    // exist.  This should be a system configuration problem, but
    // let's not let it crash the system and just live with no
    // wallpaper.
    Slogger::E(TAG, "Default wallpaper component not found!");
    ClearWallpaperComponentLocked((IWallpaperData*)wallpaper.Get());
    if (reply != NULL) {
        ec = reply->SendResult(NULL);
        if(FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
    }
    bh->RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode WallpaperManagerService::HasNamedWallpaper(
    /* [in] */ const String& name,
    /* [out] */ Boolean* hasNamedWallpaper)
{
    VALIDATE_NOT_NULL(hasNamedWallpaper)
    synchronized(this) {
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int64 ident;
        bh->ClearCallingIdentity(&ident);
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        AutoPtr<IUserManager> um = IUserManager::Probe(obj);
        AutoPtr<IList> users;
        um->GetUsers((IList**)&users);
        bh->RestoreCallingIdentity(ident);
        Int32 size;
        users->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
            Int32 id;
            user->GetId(&id);
            AutoPtr<IInterface> obj2;
            mWallpaperMap->Get(id, (IInterface**)&obj2);
            AutoPtr<IWallpaperData> _wd = IWallpaperData::Probe(obj2);
            AutoPtr<WallpaperData> wd = (WallpaperData*)_wd.Get();
            if (wd == NULL) {
                // User hasn't started yet, so load her settings to peek at the wallpaper
                LoadSettingsLocked(id);
                AutoPtr<IInterface> obj3;
                mWallpaperMap->Get(id, (IInterface**)&obj3);
                AutoPtr<IWallpaperData> _wdd = IWallpaperData::Probe(obj3);
                AutoPtr<WallpaperData> wdd = (WallpaperData*)_wdd.Get();
                wd = wdd.Get();
            }
            if (wd != NULL && name.Equals(wd->mName)) {
                *hasNamedWallpaper = TRUE;
                return NOERROR;
            }
        }
    }
    *hasNamedWallpaper = FALSE;
    return NOERROR;
}

ECode WallpaperManagerService::GetDefaultDisplaySize(
    /* [out] */ IPoint** displaySize)
{
    VALIDATE_NOT_NULL(displaySize)
    AutoPtr<IPoint> p;
    CPoint::New((IPoint**)&p);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(obj);
    AutoPtr<IDisplay> d;
    wm->GetDefaultDisplay((IDisplay**)&d);
    d->GetRealSize(p);
    *displaySize = p.Get();
    REFCOUNT_ADD(*displaySize);
    return NOERROR;
}

ECode WallpaperManagerService::SetDimensionHints(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    CheckPermission(Manifest::permission::SET_WALLPAPER_HINTS);
    synchronized(this) {
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();
        if (wallpaper == NULL) {
            Slogger::E(TAG, "Wallpaper not yet initialized for user %d", userId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (width <= 0 || height <= 0) {
            Slogger::E(TAG, "width and height must be > 0");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        // Make sure it is at least as large as the display.
        AutoPtr<IPoint> displaySize;
        GetDefaultDisplaySize((IPoint**)&displaySize);
        Int32 x, y;
        displaySize->GetX(&x);
        displaySize->GetY(&y);
        width = Elastos::Core::Math::Max(width, x);
        height = Elastos::Core::Math::Max(height, y);

        if (width != wallpaper->mWidth || height != wallpaper->mHeight) {
            wallpaper->mWidth = width;
            wallpaper->mHeight = height;
            SaveSettingsLocked(wallpaper);
            if (mCurrentUserId != userId) return E_NULL_POINTER_EXCEPTION; // Don't change the properties now
            if (wallpaper->mConnection != NULL) {
                if (wallpaper->mConnection->mEngine != NULL) {
                    ECode ec = wallpaper->mConnection->mEngine->SetDesiredSize(width, height);
                    if (FAILED(ec)) {
                        return E_REMOTE_EXCEPTION;
                    }
                    NotifyCallbacksLocked(wallpaper);
                } else if (wallpaper->mConnection->mService != NULL) {
                    // We've attached to the service but the engine hasn't attached back to us
                    // yet. This means it will be created with the previous dimensions, so we
                    // need to update it to the new dimensions once it attaches.
                    wallpaper->mConnection->mDimensionsChanged = TRUE;
                }
            }
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::GetWidthHint(
    /* [out] */ Int32* widthHint)
{
    VALIDATE_NOT_NULL(widthHint)
    synchronized(this) {
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();
        *widthHint = wallpaper->mWidth;
    }
    return NOERROR;
}

ECode WallpaperManagerService::GetHeightHint(
    /* [out] */ Int32* heightHint)
{
    VALIDATE_NOT_NULL(heightHint)
    synchronized(this) {
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();
        *heightHint = wallpaper->mHeight;
    }
    return NOERROR;
}

ECode WallpaperManagerService::SetDisplayPadding(
    /* [in] */ IRect* padding)
{
    CheckPermission(Manifest::permission::SET_WALLPAPER_HINTS);
    synchronized(this) {
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();
        if (wallpaper == NULL) {
            Slogger::E(TAG, "Wallpaper not yet initialized for user %d", userId);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Int32 left, right, top, bottom;
        padding->GetLeft(&left);
        padding->GetRight(&right);
        padding->GetTop(&top);
        padding->GetBottom(&bottom);
        if (left < 0 || top < 0 || right < 0 || bottom < 0) {
            String str;
            IObject::Probe(padding)->ToString(&str);
            Slogger::E(TAG, "padding must be positive: %s", str.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Boolean isEquals;
        IObject::Probe(padding)->Equals(IInterface::Probe(wallpaper->mPadding), &isEquals);
        if (!isEquals) {
            wallpaper->mPadding->Set(padding);
            SaveSettingsLocked(wallpaper);
            if (mCurrentUserId != userId) return E_NULL_POINTER_EXCEPTION; // Don't change the properties now
            if (wallpaper->mConnection != NULL) {
                if (wallpaper->mConnection->mEngine != NULL) {
                    ECode ec = wallpaper->mConnection->mEngine->SetDisplayPadding(padding);
                    if (FAILED(ec)) {
                        return E_REMOTE_EXCEPTION;
                    }
                    NotifyCallbacksLocked(wallpaper);
                } else if (wallpaper->mConnection->mService != NULL) {
                    // We've attached to the service but the engine hasn't attached back to us
                    // yet. This means it will be created with the previous dimensions, so we
                    // need to update it to the new dimensions once it attaches.
                    wallpaper->mConnection->mPaddingChanged = TRUE;
                }
            }
        }
    }
    return NOERROR;
}

ECode WallpaperManagerService::GetWallpaper(
    /* [in] */ IIWallpaperManagerCallback* cb,
    /* [out] */ IBundle** outParams,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(outParams)
    VALIDATE_NOT_NULL(descriptor)
    synchronized(this) {
        // This returns the current user's wallpaper, if called by a system service. Else it
        // returns the wallpaper for the calling user.
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int32 callingUid;
        bh->GetCallingUid(&callingUid);
        Int32 wallpaperUserId = 0;
        if (callingUid == IProcess::SYSTEM_UID) {
            wallpaperUserId = mCurrentUserId;
        } else {
            AutoPtr<IUserHandleHelper> uhh;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
            uhh->GetUserId(callingUid, &wallpaperUserId);
        }
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(wallpaperUserId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

        if ((*outParams) != NULL) {
            (*outParams)->PutInt32(String("width"), wallpaper->mWidth);
            (*outParams)->PutInt32(String("height"), wallpaper->mHeight);
        }
        Boolean isRegister;
        wallpaper->mCallbacks->Register(IInterface::Probe(cb), &isRegister);

        AutoPtr<IFile> file;
        GetWallpaperDir(wallpaperUserId, (IFile**)&file);
        AutoPtr<IFile> f;
        CFile::New(file.Get(), WALLPAPER, (IFile**)&f);

        Boolean isExist;
        if (f->Exists(&isExist), !isExist) {
            *descriptor = NULL;
            return NOERROR;
        }
        AutoPtr<IParcelFileDescriptorHelper> pfdh;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdh);
        ECode ec = pfdh->Open(f, IParcelFileDescriptor::MODE_READ_ONLY, descriptor);
        if (FAILED(ec)) {
            /* Shouldn't happen as we check to see if the file exists */
            Slogger::W(TAG, "Error getting wallpaper");
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
        else {
            return NOERROR;
        }
        *descriptor = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode WallpaperManagerService::GetWallpaperInfo(
    /* [out] */ IWallpaperInfo** info)
{
    VALIDATE_NOT_NULL(info)
    AutoPtr<IUserHandleHelper> uhh;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
    Int32 userId;
    uhh->GetCallingUserId(&userId);
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

        if (wallpaper->mConnection != NULL) {
            *info = wallpaper->mConnection->mInfo;
            REFCOUNT_ADD(*info)
            return NOERROR;
        }
        *info = NULL;
    }
    return NOERROR;
}

ECode WallpaperManagerService::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    CheckPermission(Manifest::permission::SET_WALLPAPER);
    synchronized(this) {
        if (DEBUG) Slogger::V(TAG, "setWallpaper");
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);

        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

        if (wallpaper == NULL) {
            Slogger::E(TAG, "Wallpaper not yet initialized for user %d", userId);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int64 _ident;
        bh->ClearCallingIdentity(&_ident);
        const Int64 ident = _ident;
        AutoPtr<IParcelFileDescriptor> pfd;
        UpdateWallpaperBitmapLocked(name, wallpaper, (IParcelFileDescriptor**)&pfd);
        if (pfd != NULL) {
            wallpaper->mImageWallpaperPending = TRUE;
        }
        *descriptor = pfd.Get();
        REFCOUNT_ADD(*descriptor)
        bh->RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode WallpaperManagerService::UpdateWallpaperBitmapLocked(
    /* [in] */ const String& name,
    /* [in] */ IWallpaperData* _wallpaper,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)

    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    AutoPtr<IFile> dir;
    GetWallpaperDir(wallpaper->mUserId, (IFile**)&dir);
    Boolean isExist;
    if (dir->Exists(&isExist), !isExist) {
        Boolean isMkdir;
        dir->Mkdir(&isMkdir);
        String path;
        dir->GetPath(&path);
        //TODO
        // FileUtils::SetPermissions(
        //     path, FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH, -1, -1);
    }
    AutoPtr<IFile> file;
    CFile::New(dir, WALLPAPER, (IFile**)&file);
    AutoPtr<IParcelFileDescriptorHelper> pfh;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfh);
    AutoPtr<IParcelFileDescriptor> fd;
    ECode ec = pfh->Open(file.Get(),
            IParcelFileDescriptor::MODE_CREATE | IParcelFileDescriptor::MODE_READ_WRITE | IParcelFileDescriptor::MODE_TRUNCATE, (IParcelFileDescriptor**)&fd);
#if 0 //TODO need CSELinuxHelper
    if (!SELinux::Restorecon(file.Get())) {
        *descriptor = NULL;
        return NOERROR;
    }
#endif
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error setting wallpaper");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    else {
        wallpaper->mName = name;
        *descriptor = fd.Get();
        REFCOUNT_ADD(*descriptor)
        return NOERROR;
    }
    *descriptor = NULL;
    return NOERROR;
}

ECode WallpaperManagerService::SetWallpaperComponent(
    /* [in] */ IComponentName* name)
{
    CheckPermission(Manifest::permission::SET_WALLPAPER_COMPONENT);
    synchronized(this) {
        if (DEBUG) {
            String str;
            IObject::Probe(name)->ToString(&str);
            Slogger::V(TAG, "setWallpaperComponent name=%s", str.string());
        }
        AutoPtr<IUserHandleHelper> uhh;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhh);
        Int32 userId;
        uhh->GetCallingUserId(&userId);

        AutoPtr<IInterface> obj;
        mWallpaperMap->Get(userId, (IInterface**)&obj);
        AutoPtr<IWallpaperData> wd = IWallpaperData::Probe(obj);
        AutoPtr<WallpaperData> wallpaper = (WallpaperData*)wd.Get();

        if (wallpaper == NULL) {
            Slogger::E(TAG, "Wallpaper not yet initialized for user %d", userId);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int64 _ident;
        bh->ClearCallingIdentity(&_ident);
        const Int64 ident = _ident;
        wallpaper->mImageWallpaperPending = FALSE;
        Boolean isBind;
        BindWallpaperComponentLocked(name, FALSE, TRUE, wallpaper, NULL, &isBind);
        bh->RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode WallpaperManagerService::BindWallpaperComponentLocked(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Boolean force,
    /* [in] */ Boolean fromUser,
    /* [in] */ IWallpaperData* _wallpaper,
    /* [in] */ IIRemoteCallback* reply,
    /* [out] */ Boolean* isBinded)
{
    VALIDATE_NOT_NULL(isBinded)
    if (DEBUG) {
        String str;
        IObject::Probe(componentName)->ToString(&str);
        Slogger::V(TAG, "bindWallpaperComponentLocked componentName=%s", str.string());
    }
    // Has the component changed?
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    if (!force) {
        if (wallpaper->mConnection != NULL) {
            Boolean isEquals = FALSE;
            IObject::Probe(wallpaper->mWallpaperComponent)->Equals(IInterface::Probe(componentName), &isEquals);
            if (wallpaper->mWallpaperComponent == NULL) {
                if (componentName == NULL) {
                    if (DEBUG) Slogger::V(TAG, "bindWallpaperComponentLocked: still using default");
                    // Still using default wallpaper.
                    *isBinded = TRUE;
                    return NOERROR;
                }
            }
            else if (isEquals) {
                // Changing to same wallpaper.
                if (DEBUG) Slogger::V(TAG, "same wallpaper");
                *isBinded = TRUE;
                return NOERROR;
            }
        }
    }

    // try {
        #if 0 //TODO CWallpaperManager
        if (componentName == NULL) {
            componentName = CWallpaperManager::GetDefaultWallpaperComponent(mContext);
            if (componentName == NULL) {
                // Fall back to static image wallpaper
                componentName = mImageWallpaper.Get();
                //clearWallpaperComponentLocked();
                //return;
                if (DEBUG) Slogger::V(TAG, "Using image wallpaper");
            }
        }
        #endif
        Int32 serviceUserId = wallpaper->mUserId;
        AutoPtr<IServiceInfo> si;
        mIPackageManager->GetServiceInfo(componentName,
            IPackageManager::GET_META_DATA | IPackageManager::GET_PERMISSIONS,
            serviceUserId, (IServiceInfo**)&si);
        if (si == NULL) {
            // The wallpaper component we're trying to use doesn't exist
            String str;
            IObject::Probe(componentName)->ToString(&str);
            Slogger::W(TAG, "Attempted wallpaper %s is unavailable", str.string());
            *isBinded = FALSE;
            return NOERROR;
        }
        String permission;
        si->GetPermission(&permission);
        if (!Manifest::permission::BIND_WALLPAPER.Equals(permission)) {
            StringBuilder sb("Selected service does not require ");
            sb += Manifest::permission::BIND_WALLPAPER;
            sb += ": ";
            String str;
            IObject::Probe(componentName)->ToString(&str);
            sb += str;
            String msg = sb.ToString();
            if (fromUser) {
                Slogger::E(TAG, msg.string());
                return E_SECURITY_EXCEPTION;
            }
            Slogger::W(TAG, msg.string());
            *isBinded = FALSE;
            return NOERROR;
        }

        AutoPtr<IWallpaperInfo> wi = NULL;
        AutoPtr<IIntent> intent;
        CIntent::New(IWallpaperService::SERVICE_INTERFACE, (IIntent**)&intent);
        Boolean isEquals = FALSE;
        IObject::Probe(componentName)->Equals(IInterface::Probe(mImageWallpaper), &isEquals);
        if (componentName != NULL && !isEquals) {
            // Make sure the selected service is actually a wallpaper service.
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            String type;
            intent->ResolveTypeIfNeeded(resolver, &type);
            AutoPtr<IList> ris;
            mIPackageManager->QueryIntentServices(intent, type, IPackageManager::GET_META_DATA,
                serviceUserId, (IList**)&ris);
            Int32 risSize;
            ris->GetSize(&risSize);
            for (Int32 i=0; i< risSize; i++) {
                AutoPtr<IInterface> risElement;
                ris->Get(i, (IInterface**)&risElement);
                AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(risElement);
                AutoPtr<IServiceInfo> rsi;
                ri->GetServiceInfo((IServiceInfo**)&rsi);
                String rsiName, siName;
                IPackageItemInfo::Probe(rsi)->GetName(&rsiName);
                IPackageItemInfo::Probe(si)->GetName(&siName);
                String rsiPackageName, siPackageName;
                IPackageItemInfo::Probe(rsi)->GetPackageName(&rsiPackageName);
                IPackageItemInfo::Probe(si)->GetPackageName(&siPackageName);
                if (rsiName.Equals(siName) && rsiPackageName.Equals(siPackageName)) {
                    ECode ec = CWallpaperInfo::New(mContext.Get(), ri.Get(), (IWallpaperInfo**)&wi);
                    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
                        if (fromUser) {
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                        }
                        Slogger::W(TAG, "%d", ec);
                        *isBinded = FALSE;
                        return ec;
                    }
                    if (ec == (ECode)E_IO_EXCEPTION) {
                        if (fromUser) {
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                        }
                        Slogger::W(TAG, "%d", ec);
                        *isBinded = FALSE;
                        return ec;
                    }
                    break;
                }
            }
            if (wi == NULL) {
                String msgstr;
                IObject::Probe(componentName)->ToString(&msgstr);
                String msg = String("Selected service is not a wallpaper: ") + msgstr;
                if (fromUser) {
                    Slogger::E(TAG, msg.string());
                    return E_SECURITY_EXCEPTION;
                }
                Slogger::W(TAG, msg.string());
                *isBinded = FALSE;
                return NOERROR;
            }
        }

        // Bind the service!
        if (DEBUG) {
            String str;
            IObject::Probe(componentName)->ToString(&str);
            Slogger::V(TAG, "Binding to:%s", str.string());
        }
        AutoPtr<IServiceConnection> newConn;
        CWallpaperConnection::New(wi, wallpaper, (IServiceConnection**)&newConn);
        intent->SetComponent(componentName);
        intent->PutExtra(IIntent::EXTRA_CLIENT_LABEL, R::string::wallpaper_binding_label);

        AutoPtr<IIntent> target;
        CIntent::New(IIntent::ACTION_SET_WALLPAPER, (IIntent**)&target);
        AutoPtr<ICharSequence> title;
        mContext->GetText(R::string::chooser_wallpaper, (ICharSequence**)&title);
        AutoPtr<IIntentHelper> iih;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&iih);
        AutoPtr<IIntent> chooser;
        iih->CreateChooser(target.Get(), title.Get(), (IIntent**)&chooser);
        AutoPtr<IUserHandle> userhandle;
        CUserHandle::New(serviceUserId, (IUserHandle**)&userhandle);

        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pendingIntent;
        pih->GetActivityAsUser(mContext, 0, chooser.Get(), 0, NULL, userhandle.Get(), (IPendingIntent**)&pendingIntent);

        AutoPtr<IParcelable> parcelable = IParcelable::Probe(pendingIntent);
        intent->PutExtra(IIntent::EXTRA_CLIENT_INTENT, parcelable.Get());

        //TODO
        // AutoPtr<CContextImpl> impl = CContextImpl::GetImpl(mContext);
        Boolean isBindServiceAsUser = FALSE;
        // impl->BindServiceAsUser(intent.Get(), newConn.Get(), IContext::BIND_AUTO_CREATE | IContext::BIND_SHOWING_UI, userhandle, &isBindServiceAsUser);

        if (!isBindServiceAsUser) {
            String cnstr;
            IObject::Probe(componentName)->ToString(&cnstr);
            String msg = String("Unable to bind service: ") + cnstr;
            if (fromUser) {
                Slogger::E(TAG, msg.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            Slogger::W(TAG, msg.string());
            *isBinded = FALSE;
            return NOERROR;
        }
        if (wallpaper->mUserId == mCurrentUserId && mLastWallpaper != NULL) {
            DetachWallpaperLocked((IWallpaperData*)mLastWallpaper);
        }
        wallpaper->mWallpaperComponent = componentName;
        wallpaper->mConnection = (WallpaperConnection*)newConn.Get();
        AutoPtr<WallpaperConnection> _newConn = (WallpaperConnection*)newConn.Get();
        _newConn->mReply = reply;

        if (wallpaper->mUserId == mCurrentUserId) {
            if (DEBUG) {
                String token;
                IObject::Probe(_newConn->mToken)->ToString(&token);
                String strr = String("Adding window token: ") + token;
                Slogger::V(TAG, strr.string());
            }
            ECode ec = mIWindowManager->AddWindowToken(_newConn->mToken, IWindowManagerLayoutParams::TYPE_WALLPAPER);
            if (FAILED(ec)) {
                return E_REMOTE_EXCEPTION;
            }
            mLastWallpaper = wallpaper;
        }

    // } catch (RemoteException e) {
    //     String msg = "Remote exception for " + componentName + "\n" + e;
    //     if (fromUser) {
    //         throw new IllegalArgumentException(msg);
    //     }
    //     Slog.w(TAG, msg);
    //     return false;
    // }
    *isBinded = TRUE;
    return NOERROR;
}

ECode WallpaperManagerService::DetachWallpaperLocked(
    /* [in] */ IWallpaperData* _wallpaper)
{
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    if (wallpaper->mConnection != NULL) {
        if (wallpaper->mConnection->mReply != NULL) {
            ECode ec = wallpaper->mConnection->mReply->SendResult(NULL);
            if (FAILED(ec)) {
                return E_REMOTE_EXCEPTION;
            }
            wallpaper->mConnection->mReply = NULL;
        }
        if (wallpaper->mConnection->mEngine != NULL) {
            ECode ec = wallpaper->mConnection->mEngine->Destroy();
            if (FAILED(ec)) {
                return E_REMOTE_EXCEPTION;
            }
        }
        mContext->UnbindService((IServiceConnection*)(wallpaper->mConnection));

        if (DEBUG) {
            String token;
            IObject::Probe(wallpaper->mConnection->mToken)->ToString(&token);
            Slogger::V(TAG, "Removing window token: %s", token.string());
        }

        ECode ec = mIWindowManager->RemoveWindowToken(wallpaper->mConnection->mToken);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
        wallpaper->mConnection->mService = NULL;
        wallpaper->mConnection->mEngine = NULL;
        wallpaper->mConnection = NULL;
    }
    return NOERROR;
}

ECode WallpaperManagerService::ClearWallpaperComponentLocked(
    /* [in] */ IWallpaperData* _wallpaper)
{
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    wallpaper->mWallpaperComponent = NULL;
    return DetachWallpaperLocked((IWallpaperData*)wallpaper);
}

ECode WallpaperManagerService::AttachServiceLocked(
    /* [in] */ IWallpaperConnection* _conn,
    /* [in] */ IWallpaperData* _wallpaper)
{
    AutoPtr<WallpaperConnection> conn = (WallpaperConnection*)_conn;
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    ECode ec = conn->mService->Attach((IIWallpaperConnection*)conn, conn->mToken,
            IWindowManagerLayoutParams::TYPE_WALLPAPER, FALSE,
            wallpaper->mWidth, wallpaper->mHeight, wallpaper->mPadding);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Failed attaching wallpaper; clearing%d", ec);
        if (!wallpaper->mWallpaperUpdating) {
            Boolean result;
            BindWallpaperComponentLocked(NULL, FALSE, FALSE, (IWallpaperData*)wallpaper.Get(), NULL, &result);
        }
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode WallpaperManagerService::NotifyCallbacksLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    Int32 n;
    wallpaper->mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> cb;
        ECode ec1 = wallpaper->mCallbacks->GetBroadcastItem(i, (IInterface**)&cb);
        AutoPtr<IIWallpaperManagerCallback> wmc = IIWallpaperManagerCallback::Probe(cb);
        ECode ec2 = wmc->OnWallpaperChanged();
        if (FAILED(ec1) || FAILED(ec2)) {
            // The RemoteCallbackList will take care of removing
            // the dead object for us.
            return E_REMOTE_EXCEPTION;
        }
    }
    wallpaper->mCallbacks->FinishBroadcast();
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_WALLPAPER_CHANGED, (IIntent**)&intent);
    AutoPtr<IUserHandle> userhandle;
    CUserHandle::New(mCurrentUserId, (IUserHandle**)&userhandle);
    mContext->SendBroadcastAsUser(intent.Get(), userhandle.Get());
    return NOERROR;
}

ECode WallpaperManagerService::CheckPermission(
    /* [in] */ const String& permission)
{
    Int32 result;
    mContext->CheckCallingOrSelfPermission(permission, &result);
    if (IPackageManager::PERMISSION_GRANTED != result) {
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int32 pid;
        bh->GetCallingPid(&pid);
        Slogger::E(TAG, "Access denied to process: %d, must have permission %s",
            pid, permission.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode WallpaperManagerService::MakeJournaledFile(
    /* [in] */ Int32 userId,
    /* [out] */ IJournaledFile** outfile)
{
    VALIDATE_NOT_NULL(outfile)
    AutoPtr<IFile> file, f, baseFile, baseFile2;
    GetWallpaperDir(userId, (IFile**)&file);
    CFile::New(file.Get(), WALLPAPER_INFO, (IFile**)&f);
    String base;
    f->GetAbsolutePath(&base);
    CFile::New(base, (IFile**)&baseFile);
    CFile::New(base + ".tmp", (IFile**)&baseFile2);
    CJournaledFile::New(baseFile, baseFile2, outfile);
    return NOERROR;
}

ECode WallpaperManagerService::SaveSettingsLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    AutoPtr<IJournaledFile> journal;
    MakeJournaledFile(wallpaper->mUserId, (IJournaledFile**)&journal);
    AutoPtr<IFileOutputStream> stream;
    // try {
    AutoPtr<IFile> f;
    journal->ChooseForWrite((IFile**)&f);
    CFileOutputStream::New(f.Get(), FALSE, (IFileOutputStream**)&stream);
    AutoPtr<IXmlSerializer> out;
    CFastXmlSerializer::New((IXmlSerializer**)&out);
    out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
    out->StartDocument(String(NULL), TRUE);

    out->WriteStartTag(String(NULL), String("wp"));
    out->WriteAttribute(String(NULL), String("width"), StringUtils::ToString(wallpaper->mWidth));
    out->WriteAttribute(String(NULL), String("height"), StringUtils::ToString(wallpaper->mHeight));
    Int32 left, top, right, bottom;
    wallpaper->mPadding->GetLeft(&left);
    wallpaper->mPadding->GetTop(&top);
    wallpaper->mPadding->GetRight(&right);
    wallpaper->mPadding->GetBottom(&bottom);
    if (left != 0) {
        out->WriteAttribute(String(NULL), String("paddingLeft"), StringUtils::ToString(left));
    }
    if (top != 0) {
        out->WriteAttribute(String(NULL), String("paddingTop"), StringUtils::ToString(top));
    }
    if (right != 0) {
        out->WriteAttribute(String(NULL), String("paddingRight"), StringUtils::ToString(right));
    }
    if (bottom != 0) {
        out->WriteAttribute(String(NULL), String("paddingBottom"), StringUtils::ToString(bottom));
    }
    out->WriteAttribute(String(NULL), String("name"), wallpaper->mName);
    Boolean isEquals;
    IObject::Probe(wallpaper->mWallpaperComponent)->Equals(IInterface::Probe(mImageWallpaper), &isEquals);
    if (wallpaper->mWallpaperComponent != NULL && !isEquals) {
        String flattern;
        wallpaper->mWallpaperComponent->FlattenToShortString(&flattern);
        out->WriteAttribute(String(NULL), String("component"), flattern);
    }
    out->WriteEndTag(String(NULL), String("wp"));

    out->EndDocument();
    ICloseable::Probe(stream)->Close();
    journal->Commit();
    // } catch (IOException e) {
    //     try {
    //         if (stream != null) {
    //             stream.close();
    //         }
    //     } catch (IOException ex) {
    //         // Ignore
    //     }
    //     journal.rollback();
    // }
    return NOERROR;
}

ECode WallpaperManagerService::MigrateFromOld()
{
    AutoPtr<IFile> oldWallpaper;
    CFile::New(IWallpaperBackupHelper::WALLPAPER_IMAGE_KEY, (IFile**)&oldWallpaper);
    AutoPtr<IFile> oldInfo;
    CFile::New(IWallpaperBackupHelper::WALLPAPER_INFO_KEY, (IFile**)&oldInfo);
    Boolean isExist;
    oldWallpaper->Exists(&isExist);
    if (isExist) {
        AutoPtr<IFile> f;
        GetWallpaperDir(0, (IFile**)&f);
        AutoPtr<IFile> newWallpaper;
        CFile::New(f, WALLPAPER, (IFile**)&newWallpaper);
        Boolean succeeded;
        oldWallpaper->RenameTo(newWallpaper, &succeeded);
    }
    oldInfo->Exists(&isExist);
    if (isExist) {
        AutoPtr<IFile> f;
        GetWallpaperDir(0, (IFile**)&f);
        AutoPtr<IFile> newInfo;
        CFile::New(f, WALLPAPER_INFO, (IFile**)&newInfo);
        Boolean succeeded;
        oldInfo->RenameTo(newInfo, &succeeded);
    }
    return NOERROR;
}

Int32 WallpaperManagerService::GetAttributeInt32(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& name,
    /* [in] */ Int32 defValue)
{
    String value;
    parser->GetAttributeValue(String(NULL), name, &value);
    if (value.IsNull()) {
        return defValue;
    }
    return StringUtils::ParseInt32(value);
}

ECode WallpaperManagerService::LoadSettingsLocked(
    /* [in] */ Int32 userId)
{
    if (DEBUG) Slogger::V(TAG, "loadSettingsLocked");

    AutoPtr<IJournaledFile> journal;
    MakeJournaledFile(userId, (IJournaledFile**)&journal);
    AutoPtr<IFileInputStream> stream;
    AutoPtr<IFile> file;
    journal->ChooseForRead((IFile**)&file);
    Boolean isExist;
    file->Exists(&isExist);
    if (!isExist) {
        // This should only happen one time, when upgrading from a legacy system
        MigrateFromOld();
    }
    AutoPtr<IInterface> obj;
    mWallpaperMap->Get(userId, (IInterface**)&obj);
    AutoPtr<IWallpaperData> _wallpaper = IWallpaperData::Probe(obj);
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper.Get();
    if (wallpaper == NULL) {
        wallpaper = new WallpaperData(userId, this);
        mWallpaperMap->Put(userId, IInterface::Probe((IWallpaperData*)wallpaper));
    }
    Boolean success = FALSE;
    // try {
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    AutoPtr<IXmlPullParser> parser;
#if 0 // TODO need CXmlHelper
    ECode ec = Xml::NewPullParser((IXmlPullParser**)&parser);

    if (FAILED(ec)) {
        String ecstr;
        IObject::Probe(file)->ToString(&ecstr);
        Slogger::W(TAG, "failed parsing %s %d", ecstr.string(), ec);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    parser->SetInput(IInputStream::Probe(stream), String(NULL));

    Int32 type;
    do {
        parser->Next(&type);
        if (type == IXmlPullParser::START_TAG) {
            String tag;
            parser->GetName(&tag);
            if (String("wp").Equals(tag)) {
                String attriValue1;
                parser->GetAttributeValue(String(NULL), String("width"), &attriValue1);
                wallpaper->mWidth = StringUtils::ParseInt32(attriValue1);
                String attriValue2;
                parser->GetAttributeValue(String(NULL), String("height"), &attriValue2);
                wallpaper->mHeight = StringUtils::ParseInt32(attriValue2);

                wallpaper->mPadding->SetLeft(GetAttributeInt32(parser, String("paddingLeft"), 0));
                wallpaper->mPadding->SetTop(GetAttributeInt32(parser, String("paddingTop"), 0));
                wallpaper->mPadding->SetRight(GetAttributeInt32(parser, String("paddingRight"), 0));
                wallpaper->mPadding->SetBottom(GetAttributeInt32(parser, String("paddingBottom"), 0));

                parser->GetAttributeValue(String(NULL), String("name"), &(wallpaper->mName));

                String comp;
                parser->GetAttributeValue(String(NULL), String("component"), &comp);

                AutoPtr<IComponentName> cn;
                CComponentName::UnflattenFromString(comp, (IComponentName**)&cn);

                wallpaper->mNextWallpaperComponent = !(comp.IsNull()) ? cn : NULL;

                String packageName;
                wallpaper->mNextWallpaperComponent->GetPackageName(&packageName);
                if (wallpaper->mNextWallpaperComponent == NULL || String("android").Equals(packageName)) {
                    wallpaper->mNextWallpaperComponent = mImageWallpaper;
                }

                if (DEBUG) {
                    Slogger::V(TAG, "mWidth:%d", wallpaper->mWidth);
                    Slogger::V(TAG, "mHeight:%d", wallpaper->mHeight);
                    Slogger::V(TAG, "mName:%s", (wallpaper->mName).string());
                    String nwc;
                    IObject::Probe(wallpaper->mNextWallpaperComponent)->ToString(&nwc);
                    Slogger::V(TAG, "mNextWallpaperComponent:%s", nwc.string());
                }
            }
        }
    } while (type != IXmlPullParser::END_DOCUMENT);
    success = TRUE;
    // } catch (FileNotFoundException e) {
    //     Slog.w(TAG, "no current wallpaper -- first boot?");
    // } catch (NullPointerException e) {
    //     Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (NumberFormatException e) {
    //     Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (XmlPullParserException e) {
    //     Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (IOException e) {
    //     Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (IndexOutOfBoundsException e) {
    //     Slog.w(TAG, "failed parsing " + file + " " + e);
    // }

    if (stream != NULL) {
        ECode ec = ICloseable::Probe(stream)->Close();
        if (FAILED(ec)) {
            return E_IO_EXCEPTION;
        }
    }

    if (!success) {
        wallpaper->mWidth = -1;
        wallpaper->mHeight = -1;
        wallpaper->mPadding->Set(0, 0, 0, 0);
        wallpaper->mName = "";
    }

    // We always want to have some reasonable width hint.
    Int32 baseSize = GetMaximumSizeDimension();
    if (wallpaper->mWidth < baseSize) {
        wallpaper->mWidth = baseSize;
    }
    if (wallpaper->mHeight < baseSize) {
        wallpaper->mHeight = baseSize;
    }
#endif
    return NOERROR;
}

Int32 WallpaperManagerService::GetMaximumSizeDimension()
{
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
    AutoPtr<IDisplay> d;
    wm->GetDefaultDisplay((IDisplay**)&d);
    Int32 maximum;
    d->GetMaximumSizeDimension(&maximum);
    return maximum;
}

ECode WallpaperManagerService::SettingsRestored()
{
    // Verify caller is the system
    AutoPtr<IBinderHelper> bh;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
    Int32 cid;
    bh->GetCallingUid(&cid);
    if (cid != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "settingsRestored() can only be called from the system process");
        return E_RUNTIME_EXCEPTION;
    }
    // TODO: If necessary, make it work for secondary users as well. This currently assumes
    // restores only to the primary user
    if (DEBUG) Slogger::V(TAG, "settingsRestored");
    AutoPtr<IWallpaperData> wallpaper;
    Boolean success = FALSE;
    synchronized(this) {
        LoadSettingsLocked(0);
        AutoPtr<IInterface> wallpaperobj;
        mWallpaperMap->Get(0, (IInterface**)&wallpaperobj);
        wallpaper = IWallpaperData::Probe(wallpaperobj);
        AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)wallpaper.Get();
        Boolean isEquals;
        IObject::Probe(_wallpaper->mNextWallpaperComponent)->Equals(
            IInterface::Probe(mImageWallpaper), &isEquals);
        if (_wallpaper->mNextWallpaperComponent != NULL && !isEquals) {
            Boolean isBind;
            BindWallpaperComponentLocked(_wallpaper->mNextWallpaperComponent, FALSE, FALSE,
                wallpaper, NULL, &isBind);
            if (!isBind) {
                // No such live wallpaper or other failure; fall back to the default
                // live wallpaper (since the profile being restored indicated that the
                // user had selected a live rather than static one).
                BindWallpaperComponentLocked(NULL, FALSE, FALSE, wallpaper, NULL, &isBind);
            }
            success = TRUE;
        }
        else {
            // If there's a wallpaper name, we use that.  If that can't be loaded, then we
            // use the default.
            if (String("").Equals(_wallpaper->mName)) {
                if (DEBUG) Slogger::V(TAG, "settingsRestored: name is empty");
                success = TRUE;
            }
            else {
                if (DEBUG) Slogger::V(TAG, "settingsRestored: attempting to restore named resource");
                RestoreNamedResourceLocked(wallpaper, &success);
            }
            if (DEBUG) Slogger::V(TAG, "settingsRestored: success=%s", success ? "TRUE" : "FALSE");
            if (success) {
                Boolean isBinded;
                BindWallpaperComponentLocked(_wallpaper->mNextWallpaperComponent, FALSE, FALSE,
                    wallpaper, NULL, &isBinded);
            }
        }
    }

    if (!success) {
        AutoPtr<WallpaperData> _wallpaper = (WallpaperData*)wallpaper.Get();
        Slogger::E(TAG, "Failed to restore wallpaper: '%s'", (_wallpaper->mName).string());
        _wallpaper->mName = "";
        AutoPtr<IFile> f;
        GetWallpaperDir(0, (IFile**)&f);
        f->Delete();
    }

    synchronized(this) {
        SaveSettingsLocked((WallpaperData*)wallpaper.Get());
    }
    return NOERROR;
}

ECode WallpaperManagerService::RestoreNamedResourceLocked(
    /* [in] */ IWallpaperData* _wallpaper,
    /* [out] */ Boolean* isRestored)
{
    VALIDATE_NOT_NULL(isRestored)
    AutoPtr<WallpaperData> wallpaper = (WallpaperData*)_wallpaper;
    if (wallpaper->mName.GetLength() > 4 && String("res:").Equals(wallpaper->mName.Substring(0, 4))) {
        String resName = wallpaper->mName.Substring(4);

        String pkg(NULL);
        Int32 colon = resName.IndexOf(':');
        if (colon > 0) {
            pkg = resName.Substring(0, colon);
        }

        String ident(NULL);
        Int32 slash = resName.LastIndexOf('/');
        if (slash > 0) {
            ident = resName.Substring(slash+1);
        }

        String type(NULL);
        if (colon > 0 && slash > 0 && (slash-colon) > 1) {
            type = resName.Substring(colon+1, slash);
        }

        if (!pkg.IsNull() && !ident.IsNull() && !type.IsNull()) {
            Int32 resId = -1;
            AutoPtr<IContext> c;
            ECode ec = mContext->CreatePackageContext(pkg, IContext::CONTEXT_RESTRICTED, (IContext**)&c);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Package name %s not found", pkg.string());
                return E_NAME_NOT_FOUND_EXCEPTION;
            }

            AutoPtr<IResources> r;
            c->GetResources((IResources**)&r);
            r->GetIdentifier(resName, String(NULL), String(NULL), &resId);
            if (resId == 0) {
                Slogger::E(TAG, "couldn't resolve identifier pkg=%s type=%s ident=%s", pkg.string(), type.string(), ident.string());
                *isRestored = FALSE;
                return NOERROR;
            }
            AutoPtr<IInputStream> res;
            ec = r->OpenRawResource(resId, (IInputStream**)&res);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Resource not found: %d", resId);
                return E_RESOURCES_NOT_FOUND_EXCEPTION;
            }
            Boolean isExist;
            wallpaper->mWallpaperFile->Exists(&isExist);
            if (isExist) {
                wallpaper->mWallpaperFile->Delete();
            }
            AutoPtr<IFileOutputStream> fos;
            CFileOutputStream::New(wallpaper->mWallpaperFile, (IFileOutputStream**)&fos);
            AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(32768);
            Int32 amt;
            ec = res->Read(buffer.Get(), &amt);
            if (FAILED(ec)) {
                Slogger::E(TAG, "IOException while restoring wallpaper %d", ec);
                return E_IO_EXCEPTION;
            }
            while (amt > 0) {
                IOutputStream::Probe(fos)->Write(buffer.Get(), 0, amt);
            }
            // mWallpaperObserver will notice the close and send the change broadcast

            Slogger::V(TAG, "Restored wallpaper: %s", resName.string());
            *isRestored = TRUE;
            if (res != NULL) {
                ECode ec = ICloseable::Probe(res)->Close();
                if (FAILED(ec)) {
                    return E_IO_EXCEPTION;
                }
            }
            if (fos != NULL) {
                //TODO
                // FileUtils::Sync(fos.Get());
                ECode ec = ICloseable::Probe(fos)->Close();
                if (FAILED(ec)) {
                    return E_IO_EXCEPTION;
                }
            }
            return NOERROR;
        }
    }
    *isRestored = FALSE;
    return NOERROR;
}

} // namespace Wallpaper
} // namespace Server
} // namespace Droid
} // namespace Elastos