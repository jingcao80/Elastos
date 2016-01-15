
#include "CWallpaperManagerService.h"
#include "CWMSUserSwitchObserver.h"
#include "CWMSWallpaperConnection.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SELinux.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "util/Xml.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::ICharSequence;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
// using Elastos::Droid::App::CPendingIntent;
// using Elastos::Droid::App::CPendingIntentHelper;
//using Elastos::Droid::App::CWallpaperInfo;
using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Service::Wallpaper::EIID_IWallpaperConnection;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Kxml2::IO::CKXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {

const String CWallpaperManagerService::TAG("WallpaperService");
const Boolean CWallpaperManagerService::DEBUG = FALSE;
const Int64 CWallpaperManagerService::MIN_WALLPAPER_CRASH_TIME = 10000;
const String CWallpaperManagerService::WALLPAPER("wallpaper");
const String CWallpaperManagerService::WALLPAPER_INFO("wallpaper_info.xml");

static const String SERVICE_INTERFACE("android.service.wallpaper.WallpaperService");

static AutoPtr<IComponentName> InitComponentName()
{
    AutoPtr<IComponentName> cname;
    CComponentName::New(
        String("SystemUI"), String("SystemUI.CImageWallpaper"),
        (IComponentName**)&cname);
    return cname;
}

AutoPtr<IComponentName> CWallpaperManagerService::IMAGE_WALLPAPER = InitComponentName();

static String GetFileAbsolutePath(
    /* [in] */ IFile* file)
{
    String path;
    file->GetAbsolutePath(&path);
    return path;
}

CWallpaperManagerService::WallpaperObserver::WallpaperObserver(
    /* [in] */ WallpaperData* wallpaper,
    /* [in] */ CWallpaperManagerService* owner)
    : FileObserver(GetFileAbsolutePath(GetWallpaperDir(wallpaper->mUserId)),
        CLOSE_WRITE | DELETE | DELETE_SELF)
    , mOwner(owner)
{
    mWallpaperDir = GetWallpaperDir(wallpaper->mUserId);
    mWallpaper = wallpaper;
    CFile::New(mWallpaperDir, WALLPAPER, (IFile**)&mWallpaperFile);
}

ECode CWallpaperManagerService::WallpaperObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    if (path.IsNull()) {
        return NOERROR;
    }

    AutoLock lock(&mOwner->mLock);
    // changing the wallpaper means we'll need to back up the new one
    Int64 origId = Binder::ClearCallingIdentity();
    AutoPtr<IBackupManager> bm;
    CBackupManager::New(mOwner->mContext, (IBackupManager**)&bm);

    bm->DataChanged();
    Binder::RestoreCallingIdentity(origId);

    AutoPtr<IFile> changedFile;
    CFile::New(mWallpaperDir, path, (IFile**)&changedFile);

    Boolean equals;
    mWallpaperFile->Equals(changedFile, &equals);
    if (equals) {
        mOwner->NotifyCallbacksLocked(mWallpaper);
        if (mWallpaper->mWallpaperComponent == NULL || event != CLOSE_WRITE
                || mWallpaper->mImageWallpaperPending) {
            if (event == CLOSE_WRITE) {
                mWallpaper->mImageWallpaperPending = FALSE;
            }
            mOwner->BindWallpaperComponentLocked(IMAGE_WALLPAPER, TRUE,
                    FALSE, mWallpaper, NULL);
            mOwner->SaveSettingsLocked(mWallpaper);
        }
    }

    return NOERROR;
}

CWallpaperManagerService::WallpaperData::WallpaperData(
    /* [in] */ Int32 userId)
    : mUserId(userId)
    , mImageWallpaperPending(FALSE)
    , mName("")
    , mLastDiedTime(0)
    , mWallpaperUpdating(FALSE)
    , mWidth(-1)
    , mHeight(-1)
{
    ASSERT_SUCCEEDED(CFile::New(
        GetWallpaperDir(userId), WALLPAPER, (IFile**)&mWallpaperFile));
    ASSERT_SUCCEEDED(CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks));
}

CWallpaperManagerService::WallpaperData::~WallpaperData()
{
    mConnection = NULL;
}

CWallpaperManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CWallpaperManagerService* owner)
    : mOwner(owner)
{
}

ECode CWallpaperManagerService::MyPackageMonitor::OnPackageUpdateFinished(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mOwner->mLock);
    Int32 userId;
    GetChangingUserId(&userId);
    if (mOwner->mCurrentUserId != userId) {
        return NOERROR;
    }
    AutoPtr<WallpaperData> wallpaper = mOwner->GetWallpaperData(mOwner->mCurrentUserId);
    if (wallpaper != NULL) {
        Boolean equals = FALSE;
        if (wallpaper->mWallpaperComponent != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            equals = pn.Equals(packageName);
        }
        if (equals) {
            wallpaper->mWallpaperUpdating = FALSE;
            AutoPtr<IComponentName> comp = wallpaper->mWallpaperComponent;
            mOwner->ClearWallpaperComponentLocked(wallpaper);
            if (!mOwner->BindWallpaperComponentLocked(comp, FALSE, FALSE,
                    wallpaper, NULL)) {
                Slogger::W(TAG, "Wallpaper no longer available; reverting to default");
                mOwner->ClearWallpaperLocked(FALSE, wallpaper->mUserId, NULL);
            }
        }
    }

    return NOERROR;
}

ECode CWallpaperManagerService::MyPackageMonitor::OnPackageModified(
    /* [in] */ const String& packageName)
{
    AutoLock lock(mOwner->mLock);
    Int32 userId;
    GetChangingUserId(&userId);
    if (mOwner->mCurrentUserId != userId) {
        return NOERROR;
    }
    AutoPtr<WallpaperData> wallpaper = mOwner->GetWallpaperData(mOwner->mCurrentUserId);
    if (wallpaper != NULL) {
        Boolean equals = FALSE;
        if (wallpaper->mWallpaperComponent != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            equals = pn.Equals(packageName);
        }
        if (!equals) {
            return NOERROR;
        }
        DoPackagesChangedLocked(TRUE, wallpaper);
    }

    return NOERROR;
}

ECode CWallpaperManagerService::MyPackageMonitor::OnPackageUpdateStarted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mOwner->mLock);
    Int32 userId;
    GetChangingUserId(&userId);
    if (mOwner->mCurrentUserId != userId) {
        return NOERROR;
    }
    AutoPtr<WallpaperData> wallpaper = mOwner->GetWallpaperData(mOwner->mCurrentUserId);
    if (wallpaper != NULL) {
        Boolean equals = FALSE;
        if (wallpaper->mWallpaperComponent != NULL) {
            String pn;
            wallpaper->mWallpaperComponent->GetPackageName(&pn);
            equals = pn.Equals(packageName);
        }
        if (equals) {
            wallpaper->mWallpaperUpdating = TRUE;
        }
    }

    return NOERROR;
}

ECode CWallpaperManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    AutoLock lock(mOwner->mLock);
    Boolean changed = FALSE;
    Int32 userId;
    GetChangingUserId(&userId);
    if (mOwner->mCurrentUserId != userId) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<WallpaperData> wallpaper = mOwner->GetWallpaperData(mOwner->mCurrentUserId);
    if (wallpaper != NULL) {
        Boolean res = DoPackagesChangedLocked(doit, wallpaper);
        changed |= res;
    }
    *result = changed;

    return NOERROR;
}

ECode CWallpaperManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    AutoLock lock(mOwner->mLock);
    Int32 userId;
    GetChangingUserId(&userId);
    if (mOwner->mCurrentUserId != userId) {
        return NOERROR;
    }
    AutoPtr<WallpaperData> wallpaper = mOwner->GetWallpaperData(mOwner->mCurrentUserId);
    if (wallpaper != NULL) {
        DoPackagesChangedLocked(TRUE, wallpaper);
    }

    return NOERROR;
}

Boolean CWallpaperManagerService::MyPackageMonitor::DoPackagesChangedLocked(
    /* [in] */ Boolean doit,
    /* [in] */ WallpaperData* wallpaper)
{
    String packageName;
    Boolean changed = FALSE;
    if (wallpaper->mWallpaperComponent != NULL) {
        wallpaper->mWallpaperComponent->GetPackageName(&packageName);
        Int32 change;
        IsPackageDisappearing(packageName, &change);
        if (change == PACKAGE_PERMANENT_CHANGE
                || change == PACKAGE_TEMPORARY_CHANGE) {
            changed = TRUE;
            if (doit) {
                Slogger::W(TAG, "Wallpaper uninstalled, removing: %s", packageName.string());
                mOwner->ClearWallpaperLocked(FALSE, wallpaper->mUserId, NULL);
            }
        }
    }
    String packageName2;
    if (wallpaper->mNextWallpaperComponent != NULL) {
        wallpaper->mNextWallpaperComponent->GetPackageName(&packageName2);
        Int32 change;
        IsPackageDisappearing(packageName2, &change);
        if (change == PACKAGE_PERMANENT_CHANGE || change == PACKAGE_TEMPORARY_CHANGE) {
            wallpaper->mNextWallpaperComponent = NULL;
        }
    }

    Boolean modified;
    if (wallpaper->mWallpaperComponent != NULL
        && (IsPackageModified(packageName, &modified), modified)) {
        // try {
            AutoPtr<IPackageManager> pm;
            mOwner->mContext->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IServiceInfo> info;
            pm->GetServiceInfo(wallpaper->mWallpaperComponent, 0, (IServiceInfo**)&info);
        // } catch (NameNotFoundException e) {
            // Slog.w(TAG, "Wallpaper component gone, removing: "
                    // + wallpaper->mWallpaperComponent);
            // clearWallpaperLocked(FALSE, wallpaper->mUserId, NULL);
        // }
    }
    if (wallpaper->mNextWallpaperComponent != NULL
        && (IsPackageModified(packageName2, &modified), modified)) {
        // try {
            AutoPtr<IPackageManager> pm;
            mOwner->mContext->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IServiceInfo> info;
            pm->GetServiceInfo(wallpaper->mNextWallpaperComponent, 0, (IServiceInfo**)&info);
        // } catch (NameNotFoundException e) {
            // wallpaper->mNextWallpaperComponent = NULL;
        // }
    }
    return changed;
}

CWallpaperManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CWallpaperManagerService* owner)
    : mOwner(owner)
{}

ECode CWallpaperManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 userId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
        mOwner->OnRemoveUser(userId);
    }
    // TODO: Race condition causing problems when cleaning up on stopping a user.
    // Comment this out for now.
    // else if (Intent.ACTION_USER_STOPPING.equals(action)) {
    //     onStoppingUser(intent.getIntExtra(Intent.EXTRA_USER_HANDLE,
    //             UserHandle.USER_NULL));
    // }
    return NOERROR;
}

CWallpaperManagerService::~CWallpaperManagerService()
{
    Finalize();
}

ECode CWallpaperManagerService::constructor(
    /* [in] */ IContext* context)
{
    if (DEBUG)
        Slogger::V(TAG, "WallpaperService startup");
    mContext = context;

    mIWindowManager = (IIWindowManager*)ServiceManager::GetService(IContext::WINDOW_SERVICE).Get();
    mIPackageManager = AppGlobals::GetPackageManager();
    mMonitor = new MyPackageMonitor(this);
    mMonitor->Register(context, NULL, UserHandle::ALL, TRUE);
    Boolean res;
    GetWallpaperDir(IUserHandle::USER_OWNER)->Mkdirs(&res);
    LoadSettingsLocked(IUserHandle::USER_OWNER);

    return NOERROR;
}

//static
AutoPtr<IFile> CWallpaperManagerService::GetWallpaperDir(
    /* [in] */ Int32 userId)
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> systemDir;
    env->GetUserSystemDirectory(userId, (IFile**)&systemDir);
    return systemDir;
}

void CWallpaperManagerService::Finalize()
{
    HashMap<Int32, AutoPtr<WallpaperData> >::Iterator iter;
    for (iter = mWallpaperMap.Begin(); iter != mWallpaperMap.End(); ++iter) {
        iter->mSecond->mWallpaperObserver->StopWatching();
    }
}

ECode CWallpaperManagerService::SystemReady()
{
    if (DEBUG)
        Slogger::V(TAG, "systemReady");

    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(mCurrentUserId);
    SwitchWallpaper(wallpaper, NULL);
    wallpaper->mWallpaperObserver = new WallpaperObserver(wallpaper, this);
    wallpaper->mWallpaperObserver->StartWatching();

    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    userFilter->AddAction(IIntent::ACTION_USER_STOPPING);
    AutoPtr<MyBroadcastReceiver> myBR = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(myBR.Get(), userFilter, (IIntent**)&intent);

    AutoPtr<IUserSwitchObserver> uObserver;
    CWMSUserSwitchObserver::New((Handle32)this, (IUserSwitchObserver**)&uObserver);
    ActivityManagerNative::GetDefault()->RegisterUserSwitchObserver(uObserver);

    return NOERROR;
}

String CWallpaperManagerService::GetName()
{
    AutoLock lock(&mLock);
    return mWallpaperMap.Begin()->mSecond->mName;
}

void CWallpaperManagerService::OnStoppingUser(
    /* [in] */ Int32 userId)
{
    if (userId < 1) return;

    AutoLock lock(&mLock);
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper != NULL) {
        if (wallpaper->mWallpaperObserver != NULL) {
            wallpaper->mWallpaperObserver->StopWatching();
            wallpaper->mWallpaperObserver = NULL;
        }
        mWallpaperMap.Erase(userId);
    }
}

void CWallpaperManagerService::OnRemoveUser(
    /* [in] */ Int32 userId)
{
    if (userId < 1) return;

    AutoLock lock(&mLock);
    OnStoppingUser(userId);
    AutoPtr<IFile> wallpaperFile;
    CFile::New(GetWallpaperDir(userId), WALLPAPER, (IFile**)&wallpaperFile);

    Boolean res;
    wallpaperFile->Delete(&res);
    AutoPtr<IFile> wallpaperInfoFile;
    CFile::New(GetWallpaperDir(userId), WALLPAPER_INFO, (IFile**)&wallpaperInfoFile);

    wallpaperInfoFile->Delete(&res);
}

void CWallpaperManagerService::SwitchUser(
    /* [in] */ Int32 userId,
    /* [in] */ IRemoteCallback* reply)
{
    AutoLock lock(&mLock);
    mCurrentUserId = userId;
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper == NULL) {
        wallpaper = new WallpaperData(userId);
        mWallpaperMap[userId] = wallpaper;
        LoadSettingsLocked(userId);
    }
    // Not started watching yet, in case wallpaper data was loaded for other reasons.
    if (wallpaper->mWallpaperObserver == NULL) {
        wallpaper->mWallpaperObserver = new WallpaperObserver(wallpaper, this);
        wallpaper->mWallpaperObserver->StartWatching();
    }
    SwitchWallpaper(wallpaper, reply);
}

void CWallpaperManagerService::SwitchWallpaper(
    /* [in] */ WallpaperData* wallpaper,
    /* [in] */ IRemoteCallback* reply)
{
    AutoLock lock(&mLock);
    //RuntimeException e = NULL;
    // try {
        AutoPtr<IComponentName> cname = wallpaper->mWallpaperComponent != NULL ?
                wallpaper->mWallpaperComponent : wallpaper->mNextWallpaperComponent;
        if (BindWallpaperComponentLocked(cname, TRUE, FALSE, wallpaper, reply)) {
            return;
        }

    // } catch (RuntimeException e1) {
        // e = e1;
    // }
    Slogger::W(TAG, "Failure starting previous wallpaper");
    ClearWallpaperLocked(FALSE, wallpaper->mUserId, reply);
}

ECode CWallpaperManagerService::ClearWallpaper()
{
    if (DEBUG) Slogger::V(TAG, "clearWallpaper");

    AutoLock lock(&mLock);
    ClearWallpaperLocked(FALSE, UserHandle::GetCallingUserId(), NULL);

    return NOERROR;
}

void CWallpaperManagerService::ClearWallpaperLocked(
    /* [in] */ Boolean defaultFailed,
    /* [in] */ Int32 userId,
    /* [in] */ IRemoteCallback* reply)
{
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    AutoPtr<IFile> f;
    CFile::New(GetWallpaperDir(userId), WALLPAPER, (IFile**)&f);

    Boolean exists;
    f->Exists(&exists);
    if (exists) {
        Boolean res;
        f->Delete(&res);
    }
    const Int64 ident = Binder::ClearCallingIdentity();
    //RuntimeException e = NULL;
    // try {
        wallpaper->mImageWallpaperPending = FALSE;
        if (userId != mCurrentUserId) return;
        if (BindWallpaperComponentLocked(defaultFailed
                ? IMAGE_WALLPAPER
                : NULL, TRUE, FALSE, wallpaper, reply)) {
            return;
        }
    // } catch (IllegalArgumentException e1) {
        // e = e1;
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    // This can happen if the default wallpaper component doesn't
    // exist.  This should be a system configuration problem, but
    // let's not let it crash the system and just live with no
    // wallpaper->m
    Slogger::E(TAG, "Default wallpaper component not found!");
    ClearWallpaperComponentLocked(wallpaper);
    if (reply != NULL) {
        // try {
            reply->SendResult(NULL);
        // } catch (RemoteException e1) {
        // }
    }
}

ECode CWallpaperManagerService::HasNamedWallpaper(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    AutoLock lock(&mLock);

    AutoPtr<IObjectContainer> users;
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
        AutoPtr<IUserManager> userMgr;
        mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&userMgr);
        userMgr->GetUsers((IObjectContainer**)&users);
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    AutoPtr<IObjectEnumerator> objEnumerator;
    users->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
    Boolean hasNext = FALSE;
    while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> current;
        objEnumerator->Current((IInterface**)&current);
        AutoPtr<IUserInfo> user = IUserInfo::Probe(current);
        Int32 id;
        user->GetId(&id);
        HashMap<Int32, AutoPtr<WallpaperData> >::Iterator find = mWallpaperMap.Find(id);
        AutoPtr<WallpaperData> wd;
        if (find != mWallpaperMap.End())
            wd = find->mSecond;

        if (wd == NULL) {
            // User hasn't started yet, so load her settings to peek at the wallpaper
            LoadSettingsLocked(id);
            find = mWallpaperMap.Find(id);
            if (find != mWallpaperMap.End())
                wd = find->mSecond;
        }
        if (wd != NULL && name.Equals(wd->mName)) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode CWallpaperManagerService::SetDimensionHints(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::SET_WALLPAPER_HINTS));

    AutoLock lock(&mLock);
    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper == NULL) {
        // throw new IllegalStateException("Wallpaper not yet initialized for user " + userId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("width and height must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (width != wallpaper->mWidth || height != wallpaper->mHeight) {
        wallpaper->mWidth = width;
        wallpaper->mHeight = height;
        SaveSettingsLocked(wallpaper);
        if (mCurrentUserId != userId) return NOERROR; // Don't change the properties now
        if (wallpaper->mConnection != NULL) {
            if (wallpaper->mConnection->mEngine != NULL) {
                // try {
                    wallpaper->mConnection->mEngine->SetDesiredSize(
                            width, height);
                // } catch (RemoteException e) {
                // }
                NotifyCallbacksLocked(wallpaper);
            }
        }
    }

    return NOERROR;
}

ECode CWallpaperManagerService::GetWidthHint(
    /* [out] */ Int32* result)
{
    AutoLock lock(&mLock);
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(UserHandle::GetCallingUserId());
    *result = wallpaper->mWidth;

    return NOERROR;
}

ECode CWallpaperManagerService::GetHeightHint(
    /* [out] */ Int32* result)
{
    AutoLock lock(&mLock);
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(UserHandle::GetCallingUserId());
    *result = wallpaper->mHeight;

    return NOERROR;
}

ECode CWallpaperManagerService::GetWallpaper(
    /* [in] */ IIWallpaperManagerCallback* cb,
    /* [out] */ IBundle** outParams,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(outParams);
    *outParams = NULL;

    AutoLock lock(&mLock);
    // This returns the current user's wallpaper, if called by a system service. Else it
    // returns the wallpaper for the calling user.
    Int32 callingUid = Binder::GetCallingUid();
    Int32 wallpaperUserId = 0;
    if (callingUid == Elastos::Droid::Os::IProcess::SYSTEM_UID) {
        wallpaperUserId = mCurrentUserId;
    }
    else {
        wallpaperUserId = UserHandle::GetUserId(callingUid);
    }
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(wallpaperUserId);
    // try {
    ASSERT_SUCCEEDED(CBundle::New(outParams));
    if (*outParams != NULL) {
        (*outParams)->PutInt32(String("width"), wallpaper->mWidth);
        (*outParams)->PutInt32(String("height"), wallpaper->mHeight);
    }
    Boolean res;
    wallpaper->mCallbacks->Register(cb, &res);
    AutoPtr<IFile> f;
    CFile::New(GetWallpaperDir(wallpaperUserId), WALLPAPER, (IFile**)&f);

    Boolean exists;
    f->Exists(&exists);
    if (!exists) {
        return NOERROR;
    }

    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    ECode ec = helper->Open(f, IParcelFileDescriptor::MODE_READ_ONLY, result);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error getting wallpaper 0x%08x", ec);
        return ec;
    }

    // } catch (FileNotFoundException e) {
        /* Shouldn't happen as we check to see if the file exists */
        // Slog.w(TAG, "Error getting wallpaper", e);
    // }
    return NOERROR;
}

ECode CWallpaperManagerService::GetWallpaperInfo(
    /* [out] */ IWallpaperInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 userId = UserHandle::GetCallingUserId();

    AutoLock lock(&mLock);
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper->mConnection != NULL) {
        *result = wallpaper->mConnection->mInfo;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return NOERROR;
}

ECode CWallpaperManagerService::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** result)
{
    // MyDumpMemery();

    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::SET_WALLPAPER));

    AutoLock lock(&mLock);
    if (DEBUG) Slogger::V(TAG, "setWallpaper");
    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper == NULL) {
        // throw new IllegalStateException("Wallpaper not yet initialized for user " + userId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    const Int64 ident = Binder::ClearCallingIdentity();
    // TRY {
        AutoPtr<IParcelFileDescriptor> pfd = UpdateWallpaperBitmapLocked(name, wallpaper);
        if (pfd != NULL) {
            wallpaper->mImageWallpaperPending = TRUE;
        }
        *result = pfd;
        REFCOUNT_ADD(*result);
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

AutoPtr<IParcelFileDescriptor> CWallpaperManagerService::UpdateWallpaperBitmapLocked(
    /* [in] */ const String& _name,
    /* [in] */ WallpaperData* wallpaper)
{
    String name = _name;
    if (name.IsNull()) {
        name = "";
    }
    // TRY {
        AutoPtr<IFile> dir = GetWallpaperDir(wallpaper->mUserId);
        Boolean exists;
        dir->Exists(&exists);
        if (!exists) {
            Boolean res;
            dir->Mkdir(&res);
            String path;
            dir->GetPath(&path);
            FileUtils::SetPermissions(
                path, FileUtils::sS_IRWXU | FileUtils::sS_IRWXG
                | FileUtils::sS_IXOTH, -1, -1);
        }

        AutoPtr<IFile> file;
        CFile::New(dir, WALLPAPER, (IFile**)&file);

        AutoPtr<IParcelFileDescriptorHelper> helper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);

        AutoPtr<IParcelFileDescriptor> fd;
        helper->Open(file, IParcelFileDescriptor::MODE_CREATE | IParcelFileDescriptor::MODE_READ_WRITE,
            (IParcelFileDescriptor**)&fd);

        if (!SELinux::Restorecon(file)) {
            return NULL;
        }
        wallpaper->mName = name;
        return fd;
    // } catch (FileNotFoundException e) {
        // Slog.w(TAG, "Error setting wallpaper", e);
    // }
    return NULL;
}

ECode CWallpaperManagerService::SetWallpaperComponent(
    /* [in] */ IComponentName* name)
{
    FAIL_RETURN(CheckPermission(Elastos::Droid::Manifest::permission::SET_WALLPAPER_COMPONENT));

    AutoLock lock(&mLock);
    String str;
    name->ToString(&str);
    if (DEBUG) Slogger::V(TAG, "setWallpaperComponent name=%s", str.string());

    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper == NULL) {
        Slogger::E(TAG, "Wallpaper not yet initialized for user %d", userId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    const Int64 ident = Binder::ClearCallingIdentity();
    // try {
        wallpaper->mImageWallpaperPending = FALSE;
        BindWallpaperComponentLocked(name, FALSE, TRUE, wallpaper, NULL);
    // } finally {
        Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

Boolean CWallpaperManagerService::BindWallpaperComponentLocked(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Boolean force,
        /* [in] */ Boolean fromUser,
        /* [in] */ WallpaperData* wallpaper,
        /* [in] */ IRemoteCallback* reply)
{
    String strComponentName;
    if (componentName != NULL)
        componentName->ToString(&strComponentName);

    if (DEBUG) Slogger::V(TAG, "bindWallpaperComponentLocked: componentName=%s", strComponentName.string());
    // Has the component changed?
    if (!force) {
        if (wallpaper->mConnection != NULL) {
            Boolean equals;
            if (wallpaper->mWallpaperComponent == NULL) {
                if (componentName == NULL) {
                    if (DEBUG) Slogger::V(TAG, "bindWallpaperComponentLocked: still using default");
                    // Still using default wallpaper->m
                    return TRUE;
                }
            }
            else if (wallpaper->mWallpaperComponent->Equals(componentName, &equals), equals) {
                // Changing to same wallpaper->m
                if (DEBUG) Slogger::V(TAG, "same wallpaper");
                return TRUE;
            }
        }
    }

    // try {
        if (componentName == NULL) {
            String defaultComponent;
            mContext->GetString(R::string::default_wallpaper_component, &defaultComponent);
            if (!defaultComponent.IsNull()) {
                AutoPtr<IComponentNameHelper> cnHelper;
                CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
                // See if there is a default wallpaper component specified
                cnHelper->UnflattenFromString(defaultComponent, (IComponentName**)&componentName);
                if (componentName != NULL)
                    componentName->ToString(&strComponentName);
                if (DEBUG) Slogger::V(TAG, "Use default component wallpaper:%s", strComponentName.string());
            }
            if (componentName == NULL) {
                // Fall back to static image wallpaper
                componentName = IMAGE_WALLPAPER;
                //clearWallpaperComponentLocked();
                //return;
                if (DEBUG) Slogger::V(TAG, "Using image wallpaper");
            }
        }

        Int32 serviceUserId = wallpaper->mUserId;
        AutoPtr<IServiceInfo> si;
        mIPackageManager->GetServiceInfo(componentName,
            IPackageManager::GET_META_DATA | IPackageManager::GET_PERMISSIONS,
            serviceUserId, (IServiceInfo**)&si);
        assert(si != NULL);
        String permission;
        si->GetPermission(&permission);
        if (componentName != NULL)
            componentName->ToString(&strComponentName);

        if (!permission.Equals(Elastos::Droid::Manifest::permission::BIND_WALLPAPER)) {
            Slogger::W(TAG, "Selected service does not require android.permission.BIND_WALLPAPER: %s",
                strComponentName.string());
            if (fromUser) {
                //return E_SECURITY_EXCEPTION;
                return FALSE;
            }
            return FALSE;
        }

        AutoPtr<IWallpaperInfo> wi;

        AutoPtr<IIntent> intent;
        CIntent::New(SERVICE_INTERFACE, (IIntent**)&intent);
        Boolean equals;
        if (componentName != NULL && !(componentName->Equals(IMAGE_WALLPAPER, &equals), equals)) {
            // Make sure the selected service is actually a wallpaper service.
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            String type;
            intent->ResolveTypeIfNeeded(cr, &type);
            AutoPtr<IObjectContainer> ris;
            mIPackageManager->QueryIntentServices(
                intent, type, IPackageManager::GET_META_DATA, serviceUserId, (IObjectContainer**)&ris);

            String rsiName, siName, rsiPackageName, siPackageName;
            si->GetName(&siName);
            si->GetPackageName(&siPackageName);

            AutoPtr<IObjectEnumerator> enumerator;
            ris->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext;
            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<IResolveInfo> resInfo;
                enumerator->Current((IInterface**)&resInfo);

                AutoPtr<IServiceInfo> rsi;
                resInfo->GetServiceInfo((IServiceInfo**)&rsi);
                rsi->GetName(&rsiName);
                rsi->GetPackageName(&rsiPackageName);

                if (rsiName.Equals(siName) && rsiPackageName.Equals(siPackageName)) {
                    // try {
                    ECode ec = CWallpaperInfo::New(mContext, resInfo, (IWallpaperInfo**)&wi);
                    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
                        Slogger::W(TAG, "(%s/%s)Xml Pull Parser Exception", siPackageName.string(), siName.string());
                        return FALSE;
                    } else if (ec == (ECode)E_IO_EXCEPTION) {
                        Slogger::W(TAG, "(%s/%s)IO Exception", siPackageName.string(), siName.string());
                        return FALSE;
                    }
                    // } catch (XmlPullParserException e) {
                        // if (fromUser) {
                            // throw new IllegalArgumentException(e);
                        // }
                        // Slog.w(TAG, e);
                        // return false;
                    // } catch (IOException e) {
                        // if (fromUser) {
                            // throw new IllegalArgumentException(e);
                        // }
                        // Slog.w(TAG, e);
                        // return false;
                    // }
                    break;
                }
            }

            if (wi == NULL) {
                Slogger::E(TAG, "Selected service is not a wallpaper: %s", strComponentName.string());
                if (fromUser) {
                    //return E_SECURITY_EXCEPTION;
                    return FALSE;
                }
                return FALSE;
            }
        }

        // Bind the service!
        if (DEBUG)
            Slogger::V(TAG, "Binding to:%s", strComponentName.string());

        AutoPtr<CWMSWallpaperConnection> newConn;
        CWMSWallpaperConnection::NewByFriend(
            wi, (Handle32)wallpaper, (Handle32)this, (CWMSWallpaperConnection**)&newConn);
        intent->SetComponent(componentName);
        intent->PutExtra(
            IIntent::EXTRA_CLIENT_LABEL, R::string::wallpaper_binding_label);

        AutoPtr<IIntent> tempIntent;
        CIntent::New(IIntent::ACTION_SET_WALLPAPER, (IIntent**)&tempIntent);
        AutoPtr<ICharSequence> text;
        mContext->GetText(R::string::chooser_wallpaper, (ICharSequence**)&text);
        AutoPtr<IIntentHelper> iHelper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&iHelper);
        AutoPtr<IIntent> chooser;
        iHelper->CreateChooser(tempIntent, text, (IIntent**)&chooser);

        AutoPtr<IPendingIntentHelper> piHelper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(serviceUserId, (IUserHandle**)&userHandle);
        AutoPtr<IPendingIntent> pendingIntent;
        piHelper->GetActivityAsUser(
            mContext, 0, chooser, 0 , NULL, userHandle, (IPendingIntent**)&pendingIntent);
        intent->PutExtra(
            IIntent::EXTRA_CLIENT_INTENT, IParcelable::Probe(pendingIntent));

        Boolean res;
        mContext->BindService(
            intent, newConn, IContext::BIND_AUTO_CREATE,
            serviceUserId, &res);
        if (!res) {
            String msg = String("Unable to bind service: ") + strComponentName;
            if (fromUser) {
                Slogger::E(TAG, msg);
                // throw new IllegalArgumentException(msg);
                //return E_ILLEGAL_ARGUMENT_EXCEPTION;
                return FALSE;
            }
            Slogger::W(TAG, msg);
            return FALSE;
        }

        if (wallpaper->mUserId == mCurrentUserId && mLastWallpaper != NULL) {
            DetachWallpaperLocked(mLastWallpaper);
        }

        wallpaper->mWallpaperComponent = componentName;
        wallpaper->mConnection = newConn;
        wallpaper->mLastDiedTime = SystemClock::GetUptimeMillis();
        newConn->mReply = reply;
        // try {
            if (wallpaper->mUserId == mCurrentUserId) {
                if (DEBUG)
                    Slogger::V(TAG, "Adding window token: %p", newConn->mToken.Get());
                mIWindowManager->AddWindowToken(newConn->mToken,
                        IWindowManagerLayoutParams::TYPE_WALLPAPER);
                mLastWallpaper = wallpaper;
            }

        // } catch (RemoteException e) {
        // }
    // } catch (RemoteException e) {
        // String msg = "Remote exception for " + componentName + "\n" + e;
        // if (fromUser) {
            // throw new IllegalArgumentException(msg);
        // }
        // Slog.w(TAG, msg);
        // return false;
    // }
    return TRUE;
}

void CWallpaperManagerService::DetachWallpaperLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    if (wallpaper->mConnection != NULL) {
        if (wallpaper->mConnection->mReply != NULL) {
            // try {
                wallpaper->mConnection->mReply->SendResult(NULL);
            // } catch (RemoteException e) {
            // }
            wallpaper->mConnection->mReply = NULL;
        }
        if (wallpaper->mConnection->mEngine != NULL) {
            // try {
                wallpaper->mConnection->mEngine->Destroy();
            // } catch (RemoteException e) {
            // }
        }
        mContext->UnbindService(wallpaper->mConnection);
        // try {
            if (DEBUG)
                Slogger::V(TAG, "Removing window token: %p",
                    wallpaper->mConnection->mToken.Get());
            mIWindowManager->RemoveWindowToken(wallpaper->mConnection->mToken);
        // } catch (RemoteException e) {
        // }
        wallpaper->mConnection->mService = NULL;
        wallpaper->mConnection->mEngine = NULL;
        wallpaper->mConnection = NULL;
    }
}

void CWallpaperManagerService::ClearWallpaperComponentLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    wallpaper->mWallpaperComponent = NULL;
    DetachWallpaperLocked(wallpaper);
}

void CWallpaperManagerService::AttachServiceLocked(
    /* [in] */ CWMSWallpaperConnection* conn,
    /* [in] */ WallpaperData* wallpaper)
{
    // try {
        conn->mService->Attach(conn, conn->mToken,
                IWindowManagerLayoutParams::TYPE_WALLPAPER, FALSE,
                wallpaper->mWidth, wallpaper->mHeight);
    // } catch (RemoteException e) {
        // Slog.w(TAG, "Failed attaching wallpaper; clearing", e);
        // if (!wallpaper->mWallpaperUpdating) {
            // bindWallpaperComponentLocked(NULL, false, false, wallpaper, NULL);
        // }
    // }
}

void CWallpaperManagerService::NotifyCallbacksLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    Int32 n;
    wallpaper->mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        // try {
        AutoPtr<IIWallpaperManagerCallback> item;
            wallpaper->mCallbacks->GetBroadcastItem(i, (IInterface**)&item);
            item->OnWallpaperChanged();
        // } catch (RemoteException e) {

            // The RemoteCallbackList will take care of removing
            // the dead object for us.
        // }
    }
    wallpaper->mCallbacks->FinishBroadcast();
    // const Intent intent = new Intent(Intent::ACTION_WALLPAPER_CHANGED);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_WALLPAPER_CHANGED, (IIntent**)&intent);

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(mCurrentUserId, (IUserHandle**)&userHandle);
    mContext->SendBroadcastAsUser(intent, userHandle);
}

ECode CWallpaperManagerService::CheckPermission(
    /* [in] */ const String& permission)
{
    Int32 result;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(permission, &result));
    if (IPackageManager::PERMISSION_GRANTED != result) {
        Slogger::E(TAG, "Access denied to process: %d, must have permission %s",
            Binder::GetCallingPid(), permission.string());
        return E_SECURITY_EXCEPTION;
    }

    return NOERROR;
}

AutoPtr<JournaledFile> CWallpaperManagerService::MakeJournaledFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IFile> file;
    CFile::New(GetWallpaperDir(userId), WALLPAPER_INFO, (IFile**)&file);
    String base;
    file->GetAbsolutePath(&base);
    AutoPtr<IFile> f1, f2;
    CFile::New(base, (IFile**)&f1);
    CFile::New(base + ".tmp", (IFile**)&f2);
    AutoPtr<JournaledFile> jFile = new JournaledFile(f1, f2);

    return jFile;
}

void CWallpaperManagerService::SaveSettingsLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    AutoPtr<JournaledFile> journal = MakeJournaledFile(wallpaper->mUserId);

    AutoPtr<IFileOutputStream> stream;
    ECode ec;
    do {
        AutoPtr<IFile> file = journal->ChooseForWrite();
        ec = CFileOutputStream::New(file, FALSE, (IFileOutputStream**)&stream);
        if (FAILED(ec))
            break;

        // TODO:
        //
        AutoPtr<IXmlSerializer> out;// = new FastXmlSerializer();
        ec = CKXmlSerializer::New((IXmlSerializer**)&out);
        if (FAILED(ec))
            break;
        ec = out->SetOutput(stream, String("utf-8"));
        if (FAILED(ec))
            break;

        AutoPtr<IBoolean> b;
        CBoolean::New(TRUE, (IBoolean**)&b);
        ec = out->StartDocument(String(NULL), b);
        if (FAILED(ec))
            break;
        ec = out->WriteStartTag(String(NULL), String("wp"));
        if (FAILED(ec))
            break;
        ec = out->WriteAttribute(String(NULL), String("width"), StringUtils::Int32ToString(wallpaper->mWidth));
        if (FAILED(ec))
            break;
        ec = out->WriteAttribute(String(NULL), String("height"), StringUtils::Int32ToString(wallpaper->mHeight));
        if (FAILED(ec))
            break;
        ec = out->WriteAttribute(String(NULL), String("name"), wallpaper->mName);
        if (FAILED(ec))
            break;

        Boolean equals;
        if (wallpaper->mWallpaperComponent != NULL
            && !(wallpaper->mWallpaperComponent->Equals(IMAGE_WALLPAPER, &equals), equals)) {
            String name;
            wallpaper->mWallpaperComponent->FlattenToShortString(&name);
            ec = out->WriteAttribute(String(NULL), String("component"), name);
            if (FAILED(ec))
                break;
        }
        ec = out->WriteEndTag(String(NULL), String("wp"));
        if (FAILED(ec))
            break;
        ec = out->EndDocument();
        if (FAILED(ec))
            break;
        stream->Close();
        stream = NULL;
        journal->Commit();
    } while(0);

    if (FAILED(ec)) {
        if (stream != NULL) {
            stream->Close();
            stream = NULL;
        }
        journal->Rollback();
    }
}

void CWallpaperManagerService::MigrateFromOld()
{
    // File oldWallpaper = new File(WallpaperBackupHelper::WALLPAPER_IMAGE_KEY);
    AutoPtr<IFile> oldWallpaper;
    CFile::New(
        String("/data/data/com.android.settings/files/wallpaper"), //WallpaperBackupHelper::WALLPAPER_IMAGE_KEY
        (IFile**)&oldWallpaper);

    AutoPtr<IFile> oldInfo;
    CFile::New(
        String("/data/system/wallpaper_info.xml"), //WallpaperBackupHelper::WALLPAPER_INFO_KEY
        (IFile**)&oldInfo);

    Boolean exists;
    oldWallpaper->Exists(&exists);
    if (exists) {
        AutoPtr<IFile> newWallpaper;
        CFile::New(GetWallpaperDir(0), WALLPAPER, (IFile**)&newWallpaper);
        Boolean res;
        oldWallpaper->RenameTo(newWallpaper, &res);
    }
    oldInfo->Exists(&exists);
    if (exists) {
        AutoPtr<IFile> newInfo;
        CFile::New(GetWallpaperDir(0), WALLPAPER_INFO, (IFile**)&newInfo);
        Boolean res;
        oldInfo->RenameTo(newInfo, &res);
    }
}

void CWallpaperManagerService::LoadSettingsLocked(
    /* [in] */ Int32 userId)
{
    if (DEBUG)
        Slogger::V(TAG, "loadSettingsLocked");

    AutoPtr<JournaledFile> journal = MakeJournaledFile(userId);
    AutoPtr<IFileInputStream> stream;
    AutoPtr<IFile> file = journal->ChooseForRead();
    Boolean exists;
    file->Exists(&exists);
    if (!exists) {
        // This should only happen one time, when upgrading from a legacy system
        MigrateFromOld();
    }
    AutoPtr<WallpaperData> wallpaper = GetWallpaperData(userId);
    if (wallpaper == NULL) {
        wallpaper = new WallpaperData(userId);
        mWallpaperMap[userId] = wallpaper;
    }
    Boolean success = FALSE;
    AutoPtr<IXmlPullParser> parser;
    Int32 type;
    ECode ec = CFileInputStream::New(file, (IFileInputStream**)&stream);
    if (FAILED(ec))
        goto __FAILED__;

    parser = Xml::NewPullParser();
    ec = parser->SetInput(stream, String(NULL));
    if (FAILED(ec))
        goto __FAILED__;

    do {
        ec = parser->Next(&type);
        if (FAILED(ec))
            break;
        if (type == IXmlPullParser::START_TAG) {
            String tag;
            ec = parser->GetName(&tag);
            if (FAILED(ec))
                break;
            if (tag.Equals("wp")) {
                String value;
                ec = parser->GetAttributeValue(String(NULL), String("width"), &value);
                if (FAILED(ec))
                    break;
                wallpaper->mWidth = StringUtils::ParseInt32(value);
                ec = parser->GetAttributeValue(String(NULL), String("height"), &value);
                if (FAILED(ec))
                    break;
                wallpaper->mHeight = StringUtils::ParseInt32(value);
                ec = parser->GetAttributeValue(String(NULL), String("name"), &wallpaper->mName);
                if (FAILED(ec))
                    break;
                String comp;
                ec = parser->GetAttributeValue(String(NULL), String("component"), &comp);
                if (FAILED(ec))
                    break;

                wallpaper->mNextWallpaperComponent =NULL;
                if (!comp.IsNull()) {
                    AutoPtr<IComponentNameHelper> cnHelper;
                    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
                    cnHelper->UnflattenFromString(comp, (IComponentName**)&wallpaper->mNextWallpaperComponent);
                }

                String packageName;
                if (wallpaper->mNextWallpaperComponent == NULL
                    || (wallpaper->mNextWallpaperComponent->GetPackageName(&packageName),
                        packageName.Equals("android"))) {
                    wallpaper->mNextWallpaperComponent = IMAGE_WALLPAPER;
                }

                if (DEBUG) {
                    Slogger::V(TAG, "mWidth:%d", wallpaper->mWidth);
                    Slogger::V(TAG, "mHeight:%d", wallpaper->mHeight);
                    Slogger::V(TAG, "mName:%s", wallpaper->mName.string());
                    Slogger::V(TAG, "mNextWallpaperComponent:%s", packageName.string());
                }
            }
        }
    } while (type != IXmlPullParser::END_DOCUMENT);

__FAILED__:
    if (FAILED(ec)) {
        String path;
        file->GetPath(&path);
        Slogger::W(TAG, "failed parsing %s, ec = %x", path.string(), ec);
    }
    else
        success = TRUE;
    // } catch (FileNotFoundException e) {
    // Slog.w(TAG, "no current wallpaper -- first boot?");
    // } catch (NullPointerException e) {
        // Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (NumberFormatException e) {
        // Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (XmlPullParserException e) {
        // Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (IOException e) {
        // Slog.w(TAG, "failed parsing " + file + " " + e);
    // } catch (IndexOutOfBoundsException e) {
        // Slog.w(TAG, "failed parsing " + file + " " + e);
    // }
    // try {
        if (stream != NULL) {
            stream->Close();
        }
    // } catch (IOException e) {
        // Ignore
    // }

    if (!success) {
        wallpaper->mWidth = -1;
        wallpaper->mHeight = -1;
        wallpaper->mName = String("");
    }
    else if (wallpaper->mName.IsNull()) {
        wallpaper->mName = String("");
    }

    // We always want to have some reasonable width hint.
    AutoPtr<IWindowManager> wm;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
    AutoPtr<IDisplay> d;
    wm->GetDefaultDisplay((IDisplay**)&d);
    Int32 baseSize;
    d->GetMaximumSizeDimension(&baseSize);
    if (wallpaper->mWidth < baseSize) {
        wallpaper->mWidth = baseSize;
    }
    if (wallpaper->mHeight < baseSize) {
        wallpaper->mHeight = baseSize;
    }
}

void CWallpaperManagerService::SettingsRestored()
{
    // TODO: If necessary, make it work for secondary users as well. This currently assumes
    // restores only to the primary user
    if (DEBUG)
        Slogger::V(TAG, "settingsRestored");
    AutoPtr<WallpaperData> wallpaper;
    Boolean success = FALSE;
    {
        AutoLock lock(&mLock);
        LoadSettingsLocked(0);
        wallpaper = GetWallpaperData(0);
        Boolean equals;
        if (wallpaper->mNextWallpaperComponent != NULL
            && !(wallpaper->mNextWallpaperComponent->Equals(
                IMAGE_WALLPAPER, &equals), equals)) {
            if (!BindWallpaperComponentLocked(wallpaper->mNextWallpaperComponent, FALSE, FALSE,
                    wallpaper, NULL)) {
                // No such live wallpaper or other failure; fall back to the default
                // live wallpaper (since the profile being restored indicated that the
                // user had selected a live rather than static one).
                BindWallpaperComponentLocked(NULL, FALSE, FALSE, wallpaper, NULL);
            }
            success = TRUE;
        }
        else {
            // If there's a wallpaper name, we use that.  If that can't be loaded, then we
            // use the default.
            if (wallpaper->mName.IsEmpty()) {
                if (DEBUG)
                    Slogger::V(TAG, "settingsRestored: name is empty");
                success = TRUE;
            }
            else {
                if (DEBUG)
                    Slogger::V(TAG, "settingsRestored: attempting to restore named resource");
                success = RestoreNamedResourceLocked(wallpaper);
            }
            if (DEBUG)
                Slogger::V(TAG, "settingsRestored: success=%d", success);
            if (success) {
                BindWallpaperComponentLocked(
                    wallpaper->mNextWallpaperComponent,
                    FALSE, FALSE, wallpaper, NULL);
            }
        }
    }

    if (!success) {
        Slogger::E(TAG, "Failed to restore wallpaper: '%s'", wallpaper->mName.string());
        wallpaper->mName = String("");
        Boolean res;
        GetWallpaperDir(0)->Delete(&res);
    }

    AutoLock lock(&mLock);
    SaveSettingsLocked(wallpaper);
}

Boolean CWallpaperManagerService::RestoreNamedResourceLocked(
    /* [in] */ WallpaperData* wallpaper)
{
    if (wallpaper->mName.GetLength() > 4 && String("res:").Equals(wallpaper->mName.Substring(0, 4))) {
        String resName = wallpaper->mName.Substring(4);

        String pkg;
        Int32 colon = resName.IndexOf(':');
        if (colon > 0) {
            pkg = resName.Substring(0, colon);
        }

        String ident;
        Int32 slash = resName.LastIndexOf('/');
        if (slash > 0) {
            ident = resName.Substring(slash+1);
        }

        String type;
        if (colon > 0 && slash > 0 && (slash-colon) > 1) {
            type = resName.Substring(colon+1, slash);
        }

        if (pkg != NULL && ident != NULL && type != NULL) {
            Int32 resId = -1;
            AutoPtr<IInputStream> res;
            AutoPtr<IFileOutputStream> fos;
            // try {
                AutoPtr<IContext> c;
                mContext->CreatePackageContext(pkg, IContext::CONTEXT_RESTRICTED, (IContext**)&c);
                AutoPtr<IResources> r;
                c->GetResources((IResources**)&r);
                r->GetIdentifier(resName, String(NULL), String(NULL), &resId);
                if (resId == 0) {
                    Slogger::E(TAG, "couldn't resolve identifier pkg=%s type=%s ident=%s",
                        pkg.string(), type.string(), ident.string());
                    return FALSE;
                }

                r->OpenRawResource(resId, (IInputStream**)&res);
                Boolean exists;
                wallpaper->mWallpaperFile->Exists(&exists);
                if (exists) {
                    Boolean bval;
                    wallpaper->mWallpaperFile->Delete(&bval);
                }
                CFileOutputStream::New(wallpaper->mWallpaperFile, (IFileOutputStream**)&fos);

                AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(32768);
                Int32 amt;
                while ((res->ReadBytes(buffer, &amt), amt) > 0) {
                    fos->WriteBytes(*buffer, 0, amt);
                }
                // mWallpaperObserver will notice the close and send the change broadcast

                Slogger::V(TAG, "Restored wallpaper: %s", resName.string());
                return TRUE;
            // } catch (NameNotFoundException e) {
                // Slog.e(TAG, "Package name " + pkg + " not found");
            // } catch (Resources.NotFoundException e) {
                // Slog.e(TAG, "Resource not found: " + resId);
            // } catch (IOException e) {
                // Slog.e(TAG, "IOException while restoring wallpaper ", e);
            // } finally {
                if (res != NULL) {
                    // try {
                        res->Close();
                    // } catch (IOException ex) {}
                }
                if (fos != NULL) {
                    Boolean res;
                    FileUtils::Sync(fos, &res);
                    // try {
                        fos->Close();
                    // } catch (IOException ex) {}
                }
            // }
        }
    }
    return FALSE;
}

void CWallpaperManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 perm;
    mContext->CheckCallingOrSelfPermission(
        String("android.permission.DUMP")/*android.Manifest.permission.DUMP*/, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        String str;
        str.AppendFormat("Permission Denial: can't dump wallpaper service from from pid=%d, uid=%d",
            Binder::GetCallingPid(), Binder::GetCallingUid());
        pw->PrintStringln(str);
        return;
    }

    {
        AutoLock lock(&mLock);
        pw->PrintStringln(String("Current Wallpaper Service state:"));
        HashMap<Int32, AutoPtr<WallpaperData> >::Iterator iter;
        for (iter = mWallpaperMap.Begin(); iter != mWallpaperMap.End(); ++iter) {
            AutoPtr<WallpaperData> wallpaper = iter->mSecond;
            pw->PrintStringln(String(" User ") + StringUtils::Int32ToString(wallpaper->mUserId) + ":");
            pw->PrintString(String("  mWidth="));
            pw->PrintInt32(wallpaper->mWidth);
            pw->PrintString(String(" mHeight="));
            pw->PrintInt32ln(wallpaper->mHeight);
            pw->PrintString(String("  mName="));
            pw->PrintStringln(wallpaper->mName);
            pw->PrintString(String("  mWallpaperComponent="));
            String str;
            wallpaper->mWallpaperComponent->ToString(&str);
            pw->PrintStringln(str);
            if (wallpaper->mConnection != NULL) {
                AutoPtr<CWMSWallpaperConnection> conn = wallpaper->mConnection;
                pw->PrintString(String("  Wallpaper connection "));
                // pw->Print(conn);
                pw->PrintStringln(String(":"));
                if (conn->mInfo != NULL) {
                    pw->PrintString(String("    mInfo.component="));
                    AutoPtr<IComponentName> componentName;
                    conn->mInfo->GetComponent((IComponentName**)&componentName);
                    componentName->ToString(&str);
                    pw->PrintString(str);
                }
                pw->PrintString(String("    mToken="));
                // pw->Println(conn->mToken);
                pw->PrintString(String("    mService="));
                // pw->Println(conn->mService);
                pw->PrintString(String("    mEngine="));
                // pw->Println(conn->mEngine);
                pw->PrintString(String("    mLastDiedTime="));
                pw->PrintInt32ln(wallpaper->mLastDiedTime - SystemClock::GetUptimeMillis());
            }
        }
    }
}

AutoPtr<CWallpaperManagerService::WallpaperData> CWallpaperManagerService::GetWallpaperData(
    /* [in] */ Int32 userId)
{
    HashMap<Int32, AutoPtr<WallpaperData> >::Iterator find = mWallpaperMap.Find(userId);
    if (find != mWallpaperMap.End()) {
        return find->mSecond;
    }

    return NULL;
}

ECode CWallpaperManagerService::ToString(
    /* [in] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
