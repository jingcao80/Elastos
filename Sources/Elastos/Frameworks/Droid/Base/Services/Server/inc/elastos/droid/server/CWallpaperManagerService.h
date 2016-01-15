
#ifndef __ELASTOS_DROID_SERVER_CWALLPAPERMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CWALLPAPERMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CWallpaperManagerService.h"
#include "elastos/droid/os/FileObserver.h"
#include "elastos/droid/content/PackageMonitor.h"
#include "util/JournaledFile.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::App::IWallpaperInfo;
using Elastos::Droid::App::IIWallpaperManagerCallback;
using Elastos::Droid::App::IUserSwitchObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Os::FileObserver;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Service::Wallpaper::IWallpaperConnection;
using Elastos::Droid::Service::Wallpaper::IWallpaperEngine;
using Elastos::Droid::Service::Wallpaper::IIWallpaperService;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::Internal::Utility::JournaledFile;

namespace Elastos {
namespace Droid {
namespace Server {

class CWMSWallpaperConnection;

CarClass(CWallpaperManagerService)
{
    friend class CWMSWallpaperConnection;
    friend class CWMSUserSwitchObserver;
private:
    class WallpaperData;

    /**
     * Observes the wallpaper for changes and notifies all IWallpaperServiceCallbacks
     * that the wallpaper has changed. The CREATE is triggered when there is no
     * wallpaper set and is created for the first time. The CLOSE_WRITE is triggered
     * everytime the wallpaper is changed.
     */
    class WallpaperObserver : public FileObserver
    {
    public:
        WallpaperObserver(
            /* [in] */ WallpaperData* wallpaper,
            /* [in] */ CWallpaperManagerService* owner);

        CARAPI OnEvent(
            /* [in] */ Int32 event,
            /* [in] */ const String& path);

    private:
        AutoPtr<WallpaperData> mWallpaper;
        AutoPtr<IFile> mWallpaperDir;
        AutoPtr<IFile> mWallpaperFile;
        CWallpaperManagerService* mOwner;
    };

    class WallpaperData : public ElRefBase
    {
    public:
        WallpaperData(
            /* [in] */ Int32 userId);

        ~WallpaperData();

    public:
        Int32 mUserId;

        AutoPtr<IFile> mWallpaperFile;

        /**
         * Client is currently writing a new image wallpaper.
         */
        Boolean mImageWallpaperPending;

        /**
         * Resource name if using a picture from the wallpaper gallery
         */
        String mName;;

        /**
         * The component name of the currently set live wallpaper.
         */
        AutoPtr<IComponentName> mWallpaperComponent;

        /**
         * The component name of the wallpaper that should be set next.
         */
        AutoPtr<IComponentName> mNextWallpaperComponent;

        AutoPtr<CWMSWallpaperConnection> mConnection;
        Int64 mLastDiedTime;
        Boolean mWallpaperUpdating;
        AutoPtr<WallpaperObserver> mWallpaperObserver;

        /**
         * List of callbacks registered they should each be notified when the wallpaper is changed.
         */
        AutoPtr<IRemoteCallbackList> mCallbacks;
                // = new RemoteCallbackList<IIWallpaperManagerCallback>();
        Int32 mWidth;
        Int32 mHeight;
    };

    class MyPackageMonitor : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ CWallpaperManagerService* owner);

        CARAPI OnPackageUpdateFinished(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI OnPackageModified(
            /* [in] */ const String& packageName);

        CARAPI OnPackageUpdateStarted(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

        CARAPI OnSomePackagesChanged();

    private:
        CARAPI_(Boolean) DoPackagesChangedLocked(
            /* [in] */ Boolean doit,
            /* [in] */ WallpaperData* wallpaper);

    private:
        CWallpaperManagerService* mOwner;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CWallpaperManagerService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CWallpaperManagerService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CWallpaperManagerService* mOwner;
    };

public:
    ~CWallpaperManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemReady();

    CARAPI ClearWallpaper();

    CARAPI HasNamedWallpaper(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI SetDimensionHints(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GetWidthHint(
        /* [out] */ Int32* result);

    CARAPI GetHeightHint(
        /* [out] */ Int32* result);

    CARAPI GetWallpaper(
        /* [in] */ IIWallpaperManagerCallback* cb,
        /* [out] */ IBundle** outParams,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI GetWallpaperInfo(
        /* [out] */ IWallpaperInfo** result);

    CARAPI SetWallpaper(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI SetWallpaperComponent(
        /* [in] */ IComponentName* name);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) GetName();

    // Called by SystemBackupAgent after files are restored to disk.
    CARAPI_(void) SettingsRestored();


private:
    static CARAPI_(AutoPtr<IFile>) GetWallpaperDir(
        /* [in] */ Int32 userId);

    static CARAPI_(AutoPtr<JournaledFile>) MakeJournaledFile(
        /* [in] */ Int32 userId);

    CARAPI_(void) OnStoppingUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) OnRemoveUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) SwitchUser(
        /* [in] */ Int32 userId,
        /* [in] */ IRemoteCallback* reply);

    CARAPI_(void) SwitchWallpaper(
        /* [in] */ WallpaperData* wallpaper,
        /* [in] */ IRemoteCallback* reply);

    CARAPI_(void) ClearWallpaperLocked(
        /* [in] */ Boolean defaultFailed,
        /* [in] */ Int32 userId,
        /* [in] */ IRemoteCallback* reply);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) UpdateWallpaperBitmapLocked(
        /* [in] */ const String& name,
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(Boolean) BindWallpaperComponentLocked(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Boolean force,
        /* [in] */ Boolean fromUser,
        /* [in] */ WallpaperData* wallpaper,
        /* [in] */ IRemoteCallback* reply);

    CARAPI_(void) DetachWallpaperLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) ClearWallpaperComponentLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) AttachServiceLocked(
        /* [in] */ CWMSWallpaperConnection* conn,
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) NotifyCallbacksLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI CheckPermission(
        /* [in] */ const String& permission);

    CARAPI_(void) SaveSettingsLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(void) MigrateFromOld();

    CARAPI_(void) LoadSettingsLocked(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) RestoreNamedResourceLocked(
        /* [in] */ WallpaperData* wallpaper);

    CARAPI_(AutoPtr<WallpaperData>) GetWallpaperData(
        /* [in] */ Int32 userId);

protected:
    CARAPI_(void) Finalize();

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    static const String TAG ;
    static const Boolean DEBUG;

    /**
     * Minimum time between crashes of a wallpaper service for us to consider
     * restarting it vs. just reverting to the static wallpaper.
     */
    static const Int64 MIN_WALLPAPER_CRASH_TIME;
    static const String WALLPAPER;
    static const String WALLPAPER_INFO;

    /**
     * Name of the component used to display bitmap wallpapers from either the gallery or
     * built-in wallpapers.
     */
    static AutoPtr<IComponentName> IMAGE_WALLPAPER;

private:
    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWindowManager> mIWindowManager;
    AutoPtr<IIPackageManager> mIPackageManager;
    AutoPtr<MyPackageMonitor> mMonitor;
    AutoPtr<WallpaperData> mLastWallpaper;

    HashMap<Int32, AutoPtr<WallpaperData> > mWallpaperMap;

    Int32 mCurrentUserId;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CWALLPAPERMANAGERSERVICE_H__
