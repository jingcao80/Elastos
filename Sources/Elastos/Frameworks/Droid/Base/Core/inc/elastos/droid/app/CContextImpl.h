//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_APP_CCONTEXTIMPL_H__
#define __ELASTOS_DROID_APP_CCONTEXTIMPL_H__

#include "_Elastos_Droid_App_CContextImpl.h"
#include "elastos/droid/app/LoadedPkg.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/CDownloadManager.h"
#include "elastos/droid/app/SharedPreferencesImpl.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/ContextWrapper.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::ContentResolver;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Utility::IArrayMap;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CContextImpl)
    , public ContextWrapper
    , public IContextImpl
{
private:
    class ApplicationContentResolver
        : public ContentResolver
    {
    public:
        ApplicationContentResolver();

        virtual ~ApplicationContentResolver();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ CActivityThread* mainThread,
            /* [in] */ IUserHandle* user);

        CARAPI AcquireProvider(
            /* [in] */ IContext* context,
            /* [in] */ const String& auth,
            /* [out] */ IIContentProvider** provider);

        CARAPI AcquireExistingProvider(
            /* [in] */ IContext* context,
            /* [in] */ const String& auth,
            /* [out] */ IIContentProvider** provider);

        CARAPI ReleaseProvider(
            /* [in] */ IIContentProvider* provider,
            /* [out] */ Boolean* result);

        CARAPI AcquireUnstableProvider(
            /* [in] */ IContext* c,
            /* [in] */ const String& auth,
            /* [out] */ IIContentProvider** provider);

        CARAPI ReleaseUnstableProvider(
            /* [in] */ IIContentProvider* icp,
            /* [out] */ Boolean* result);

        CARAPI UnstableProviderDied(
            /* [in] */ IIContentProvider* icp);

        CARAPI AppNotRespondingViaProvider(
            /* [in] */ IIContentProvider* icp);

        /** @hide */
        Int32 ResolveUserIdFromAuthority(
            /* [in] */ const String& auth);

    private:
        AutoPtr<CActivityThread> mMainThread;
        AutoPtr<IUserHandle> mUser;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContextImpl();

    virtual ~CContextImpl();

    CARAPI constructor(
        /* [in]*/ IContextImpl* container,
        /* [in] */ IActivityThread* mainThread,
        /* [in] */ ILoadedPkg* packageInfo,
        /* [in] */ IBinder* activityToken,
        /* [in] */ IUserHandle* user,
        /* [in] */ Boolean restricted,
        /* [in] */ IDisplay* display,
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [in] */ const String& themePackageName);

    static CARAPI_(AutoPtr<CContextImpl>) GetImpl(
        /* [in] */ IContext* context);

    CARAPI GetAssets(
        /* [out] */ IAssetManager** assetManager);

    CARAPI GetResources(
        /* [out] */ IResources** resources);

    CARAPI GetPackageManager(
        /* [out] */ IPackageManager** packageManager);

    CARAPI GetContentResolver(
        /* [out] */ IContentResolver** resolver);

    CARAPI GetMainLooper(
        /* [out] */ ILooper** looper);

    CARAPI GetApplicationContext(
        /* [out] */ IContext** ctx);

    CARAPI RegisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    CARAPI UnregisterComponentCallbacks(
        /* [in] */ IComponentCallbacks* componentCallback);

    CARAPI GetText(
        /* [in] */ Int32 resId,
        /* [out] */ ICharSequence** text);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [out] */ String* str);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [in] */ ArrayOf<IInterface*>* formatArgs,
        /* [out] */ String* str);

    CARAPI SetTheme(
        /* [in] */ Int32 resid);

    CARAPI GetThemeResId(
        /* [out] */ Int32* resId);

    CARAPI GetTheme(
        /* [out] */ IResourcesTheme** theme);

    CARAPI RecreateTheme();

    CARAPI ObtainStyledAttributes(
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ Int32 resid,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** styles);

    CARAPI ObtainStyledAttributes(
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes,
        /* [out] */ ITypedArray** styles);

    CARAPI GetClassLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI GetBasePackageName(
        /* [out] */ String* packageName);

    CARAPI GetOpPackageName(
        /* [out] */ String* packageName);

    CARAPI GetApplicationInfo(
        /* [out] */ IApplicationInfo** info);

    CARAPI GetPackageResourcePath(
        /* [out] */ String* path);

    CARAPI GetPackageCodePath(
        /* [out] */ String* codePath);

    CARAPI GetSharedPrefsFile(
        /* [in] */ const String& name,
        /* [out] */ IFile** file);

    CARAPI GetSharedPreferences(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ ISharedPreferences** prefs);

    CARAPI OpenFileInput(
        /* [in] */ const String& name,
        /* [out] */ IFileInputStream** fileInputStream);

    CARAPI OpenFileOutput(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */IFileOutputStream** fileOutputStream);

    CARAPI DeleteFile(
        /* [in] */ const String& name,
        /* [out] */ Boolean* succeeded);

    CARAPI GetFileStreamPath(
        /* [in] */ const String& name,
        /* [out] */ IFile** file);

    CARAPI GetFilesDir(
        /* [out] */ IFile** filesDir);

    CARAPI GetNoBackupFilesDir(
        /* [out] */ IFile** filesDir);

    CARAPI GetExternalFilesDir(
        /* [in] */ const String& type,
        /* [out] */ IFile** filesDir);

    CARAPI GetExternalFilesDirs(
        /* [in] */ const String& type,
         /* [out, callee] */ ArrayOf<IFile*>** filesDirs);

    CARAPI GetObbDir(
        /* [out] */ IFile** obbDir);

    CARAPI GetObbDirs(
         /* [out, callee] */ ArrayOf<IFile*>** dirs);

    CARAPI GetCacheDir(
        /* [out] */ IFile** cacheDir);

    CARAPI GetCodeCacheDir(
        /* [out] */ IFile** cacheDir);

    CARAPI GetExternalCacheDir(
        /* [out] */ IFile** externalDir);

    CARAPI GetExternalCacheDirs(
         /* [out, callee] */ ArrayOf<IFile*>** dirs);

    CARAPI GetExternalMediaDirs(
         /* [out, callee] */ ArrayOf<IFile*>** dirs);

    CARAPI GetFileList(
        /* [out, callee] */ ArrayOf<String>** fileList);

    CARAPI GetDir(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetUserId(
        /* [out] */ Int32* id);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [out] */ ISQLiteDatabase** sqliteDB);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ IDatabaseErrorHandler* errorHandler,
        /* [out] */ ISQLiteDatabase** sqliteDB);

    CARAPI DeleteDatabase(
        /* [in] */ const String& name,
        /* [out] */ Boolean* succeeded);

    CARAPI GetDatabasePath(
        /* [in] */ const String& name,
        /* [out] */ IFile** path);

    CARAPI GetDatabaseList(
        /* [out, callee] */ ArrayOf<String>** databaseList);

    CARAPI GetWallpaper(
        /* [out] */ IDrawable** drawable);

    CARAPI PeekWallpaper(
        /* [out] */ IDrawable** drawable);

    CARAPI GetWallpaperDesiredMinimumWidth(
        /* [out] */ Int32* minWidth);

    CARAPI GetWallpaperDesiredMinimumHeight(
        /* [out] */ Int32* minHeight);

    CARAPI SetWallpaper(
        /* [in] */ IBitmap* bitmap);

    CARAPI SetWallpaper(
        /* [in] */ IInputStream* data);

    CARAPI ClearWallpaper();

    CARAPI StartActivity(
        /* [in] */ IIntent* intent);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options);

    CARAPI StartActivitiesAsUser(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* userHandle);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags,
        /* [in] */ IBundle* options);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& receiverPermission,
        /* [in] */ Int32 appOp,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI SendBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ const String& receiverPermission);

    CARAPI SendOrderedBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ const String& receiverPermission,
        /* [in] */ Int32 appOp,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendOrderedBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ const String& receiverPermission,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI SendStickyBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI SendStickyOrderedBroadcast(
        /* [in] */ IIntent* intent,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI RemoveStickyBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI SendStickyBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI SendStickyOrderedBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IHandler* scheduler,
        /* [in] */ Int32 initialCode,
        /* [in] */ const String& initialData,
        /* [in] */ IBundle* initialExtras);

    CARAPI RemoveStickyBroadcastAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [out] */ IIntent** intent);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** intent);

    CARAPI RegisterReceiverAsUser(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IUserHandle* user,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** stickyIntent);

    CARAPI UnregisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver);

    CARAPI StartService(
        /* [in] */ IIntent* service,
        /* [out] */ IComponentName** name);

    CARAPI StopService(
        /* [in] */ IIntent* service,
        /* [out] */ Boolean* succeeded);

    CARAPI StartServiceAsUser(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ IComponentName** name);

    CARAPI StopServiceAsUser(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* succeeded);

    CARAPI BindService(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI BindServiceAsUser(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* succeeded);

    CARAPI UnbindService(
        /* [in] */ IServiceConnection* conn);

    CARAPI StartInstrumentation(
        /* [in] */ IComponentName* className,
        /* [in] */ const String& profileFile,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* succeeded);

    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

//    /* package */
//    static CARAPI_(AutoPtr<IDropBoxManager>) CreateDropBoxManager();

    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32 * result);

    CARAPI CheckCallingPermission(
        /* [in] */ const String& permission,
        /* [out] */ Int32* value);

    CARAPI CheckCallingOrSelfPermission(
        /* [in] */ const String& permission,
        /* [out] */ Int32* perm);

    CARAPI EnforcePermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ const String& message);

    CARAPI EnforceCallingPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& message);

    CARAPI EnforceCallingOrSelfPermission(
        /* [in] */ const String& permission,
        /* [in] */ const String& message);

    CARAPI GrantUriPermission(
        /* [in] */ const String& toPackage,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    CARAPI RevokeUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags);

    CARAPI CheckUriPermission(
        /* [in] */ IUri * uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32 * result);

    CARAPI CheckCallingUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32* result);

    CARAPI CheckCallingOrSelfUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32* result);

    CARAPI CheckUriPermission(
        /* [in] */ IUri * uri,
        /* [in] */ const String& readPermission,
        /* [in] */ const String& writePermission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [out] */ Int32 * result);

    CARAPI EnforceUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceCallingUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceCallingOrSelfUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI EnforceUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ const String& readPermission,
        /* [in] */ const String& writePermission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& message);

    CARAPI CreateApplicationContext(
        /* [in] */ IApplicationInfo* application,
        /* [in] */ Int32 flags,
        /* [out] */ IContext** ctx);

    CARAPI CreateApplicationContext(
        /* [in] */ IApplicationInfo* application,
        /* [in] */ const String& themePackageName,
        /* [in] */ Int32 flags,
        /* [out] */ IContext** ctx);

    CARAPI CreatePackageContext(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [out] */ IContext** ctx);

    CARAPI CreatePackageContextAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* user,
        /* [out] */ IContext** ctx);

    CARAPI CreatePackageContextAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ const String& themePackageName,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* user,
        /* [out] */ IContext** ctx);

    CARAPI CreateConfigurationContext(
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [out] */ IContext** ctx);

    CARAPI CreateDisplayContext(
        /* [in] */ IDisplay* display,
        /* [out] */ IContext** ctx);

    CARAPI IsRestricted(
        /* [out] */ Boolean* isRestricted);

    CARAPI GetDisplayAdjustments(
        /* [in] */ Int32 displayId,
        /* [out] */ IDisplayAdjustments** das);

    static CARAPI_(AutoPtr<CContextImpl>) CreateSystemContext(
        /* [in] */ IActivityThread* mainThread);

    static CARAPI CreateAppContext(
        /* [in] */ IActivityThread* mainThread,
        /* [in] */ ILoadedPkg* packageInfo,
        /* [out] */ IContextImpl** result);

    static CARAPI CreateActivityContext(
        /* [in] */ IActivityThread* mainThread,
        /* [in] */ ILoadedPkg* packageInfo,
        /* [in] */ IBinder* activityToken,
        /* [out] */ IContextImpl** result);

    CARAPI InstallSystemApplicationInfo(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ IClassLoader* classLoader);

    CARAPI ScheduleFinalCleanup(
        /* [in] */ const String& who,
        /* [in] */ const String& what);

    CARAPI PerformFinalCleanup(
        /* [in] */ const String& who,
        /* [in] */ const String& what);

    CARAPI GetReceiverRestrictedContext(
        /* [out] */ IContext** ctx);

    CARAPI_(void) SetOuterContext(
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<IContext>) GetOuterContext();

    CARAPI_(AutoPtr<IBinder>) GetActivityToken();

    static CARAPI_(void) SetFilePermissionsFromMode(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 extraPermissions);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(AutoPtr<IFile>) GetPreferencesDir();

    CARAPI_(AutoPtr<IFile>) GetDatabasesDir();

    CARAPI RegisterReceiverInternal(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ Int32 userId,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [in] */ IContext* context,
        /* [out] */ IIntent** intent);

    CARAPI_(AutoPtr<IWallpaperManager>) GetWallpaperManager();

    CARAPI Enforce(
        /* [in] */ const String& permission,
        /* [in] */ Int32 resultOfCheck,
        /* [in] */ Boolean selfToo,
        /* [in] */ Int32 uid,
        /* [in] */ const String& message);

    CARAPI UriModeFlagToString(
        /* [in] */ Int32 uriModeFlags,
        /* [out] */ String* mode);

    CARAPI EnforceForUri(
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int32 resultOfCheck,
        /* [in] */ Boolean selfToo,
        /* [in] */ Int32 uid,
        /* [in] */ IUri* uri,
        /* [in] */ const String& message);

    /**
     * Logs a warning if the system process directly called a method such as
     * {@link #startService(Intent)} instead of {@link #startServiceAsUser(Intent, UserHandle)}.
     * The "AsUser" variants allow us to properly enforce the user's restrictions.
     */
    CARAPI_(void) WarnIfCallingFromSystemProcess();

    CARAPI_(Int32) GetDisplayId();

    CARAPI GetDataDirFile(
        /* [out] */ IFile** file);

    CARAPI_(AutoPtr<IFile>) ValidateFilePath(
        /* [in] */ const String& name,
        /* [in] */ Boolean createDirectory);

    CARAPI MakeFilename(
        /* [in] */ IFile* base,
        /* [in] */ const String& name,
        /* [out] */ IFile** file);

    /**
     * Ensure that given directories exist, trying to create them if missing. If
     * unable to create, they are filtered by replacing with {@code null}.
     */
    AutoPtr<ArrayOf<IFile*> > EnsureDirsExistOrFilter(
        /* [in] */ ArrayOf<IFile*>* dirs);

    // Common-path handling of app data dir creation
    static AutoPtr<IFile> CreateFilesDirLocked(
        /* [in] */ IFile* file);

    CARAPI ValidateServiceIntent(
        /* [in] */ IIntent* service);

    CARAPI StartServiceCommon(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ IComponentName** name);

    CARAPI StopServiceCommon(
        /* [in] */ IIntent* service,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* succeeded);

    CARAPI BindServiceCommon(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* succeeded);

    Int32 ResolveUserId(
        /* [in] */ IUri* uri);

public:
    AutoPtr<CActivityThread> mMainThread;
    AutoPtr<LoadedPkg> mPackageInfo;

private:
    const static String TAG;
    const static Boolean DEBUG;

    /**
     * Map from package name, to preference name, to cached preferences.
     */
    static AutoPtr<IArrayMap> sSharedPrefs;

    AutoPtr<IBinder> mActivityToken;

    AutoPtr<IUserHandle> mUser;

    AutoPtr<ApplicationContentResolver> mContentResolver;

    String mBasePackageName;
    String mOpPackageName;

    AutoPtr<IResourcesManager> mResourcesManager;
    AutoPtr<IResources> mResources;
    AutoPtr<IDisplay> mDisplay; // may be null if default display
    AutoPtr<IDisplayAdjustments> mDisplayAdjustments;
    AutoPtr<IConfiguration> mOverrideConfiguration;;

    Boolean mRestricted;

    // mOuterContext maybe is this, IActivity, IService or IApplication
    // IActivity, IService, IApplication has this's reference
    //
    AutoPtr<IWeakReference> mOuterContext; // IContext*

    Int32 mThemeResource;
    AutoPtr<IResourcesTheme> mTheme;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IContext> mReceiverRestrictedContext;

    Object mSync;
    static Object sLock;

    //@GuardedBy("mSync")
    AutoPtr<IFile> mDatabasesDir;
    //@GuardedBy("mSync")
    AutoPtr<IFile> mPreferencesDir;
    //@GuardedBy("mSync")
    AutoPtr<IFile> mFilesDir;
    //@GuardedBy("mSync")
    AutoPtr<IFile> mNoBackupFilesDir;
    //@GuardedBy("mSync")
    AutoPtr<IFile> mCacheDir;
    //@GuardedBy("mSync")
    AutoPtr<IFile> mCodeCacheDir;

    //@GuardedBy("mSync")
    AutoPtr<ArrayOf<IFile*> > mExternalObbDirs;
    //@GuardedBy("mSync")
    AutoPtr<ArrayOf<IFile*> > mExternalFilesDirs;
    //@GuardedBy("mSync")
    AutoPtr<ArrayOf<IFile*> > mExternalCacheDirs;
    //@GuardedBy("mSync")
    AutoPtr<ArrayOf<IFile*> > mExternalMediaDirs;

    static AutoPtr< ArrayOf<String> > EMPTY_FILE_LIST;

    // The system service cache for the system services that are
    // cached per-ContextImpl.  Package-scoped to avoid accessor
    // methods.
    HashMap< String, AutoPtr<IInterface> > mServiceCache;
    Object mCacheLock;

    static AutoPtr<IWallpaperManager> sWallpaperManager;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CCONTEXTIMPL_H__
