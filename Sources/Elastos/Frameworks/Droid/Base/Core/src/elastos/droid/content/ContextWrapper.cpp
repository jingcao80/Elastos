
#include "elastos/droid/content/ContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(ContextWrapper, Context, IContextWrapper)

ContextWrapper::ContextWrapper()
{}

ContextWrapper::~ContextWrapper()
{
    if (mHoldBaseContext && mBase) {
        REFCOUNT_RELEASE(mBase)
    }
}

ECode ContextWrapper::constructor(
    /* [in] */ IContext* base)
{
    return constructor(base, TRUE);
}

ECode ContextWrapper::constructor(
    /* [in] */ IContext* base,
    /* [in] */ Boolean holdBaseContext)
{
    mBase = base;
    mHoldBaseContext = holdBaseContext;
    if (mHoldBaseContext) {
        REFCOUNT_ADD(mBase)
    }
    return NOERROR;
}

ECode ContextWrapper::AttachBaseContext(
    /* [in] */ IContext* base)
{
    return AttachBaseContext(base, TRUE);
}

ECode ContextWrapper::AttachBaseContext(
    /* [in] */ IContext* base,
    /* [in] */ Boolean holdBaseContext)
{
    if (NULL != mBase) {
        //throw new IllegalStateException("Base context already set");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mBase = base;
    mHoldBaseContext = holdBaseContext;
    if (mHoldBaseContext) {
        REFCOUNT_ADD(mBase)
    }
    return NOERROR;
}

ECode ContextWrapper::GetBaseContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = mBase;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ContextWrapper::GetAssets(
    /* [out] */ IAssetManager** assetManager)
{
    return mBase->GetAssets(assetManager);
}

ECode ContextWrapper::GetResources(
    /* [out] */ IResources** resources)
{
    return mBase->GetResources(resources);
}

ECode ContextWrapper::GetPackageManager(
    /* [out] */ IPackageManager** packageManager)
{
    return mBase->GetPackageManager(packageManager);
}

ECode ContextWrapper::GetContentResolver(
    /* [out] */ IContentResolver** resolver)
{
    return mBase->GetContentResolver(resolver);
}

ECode ContextWrapper::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    return mBase->GetMainLooper(looper);
}

ECode ContextWrapper::GetApplicationContext(
    /* [out] */ IContext** ctx)
{
    return mBase->GetApplicationContext(ctx);
}

ECode ContextWrapper::SetTheme(
    /* [in] */ Int32 resId)
{
    return mBase->SetTheme(resId);
}

ECode ContextWrapper::GetThemeResId(
    /* [out] */ Int32* resId)
{
    return mBase->GetThemeResId(resId);
}

ECode ContextWrapper::GetTheme(
    /* [out] */ IResourcesTheme** theme)
{
    return mBase->GetTheme(theme);
}

ECode ContextWrapper::RecreateTheme()
{
    return mBase->RecreateTheme();
}

ECode ContextWrapper::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    return ((Context*)mBase)->GetClassLoader(loader);
}

ECode ContextWrapper::GetPackageName(
    /* [out] */ String* packageName)
{
    return mBase->GetPackageName(packageName);
}

ECode ContextWrapper::GetBasePackageName(
    /* [out] */ String* packageName)
{
    return mBase->GetBasePackageName(packageName);
}

ECode ContextWrapper::GetOpPackageName(
    /* [out] */ String* packageName)
{
    return mBase->GetOpPackageName(packageName);
}

ECode ContextWrapper::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    return mBase->GetApplicationInfo(info);
}

ECode ContextWrapper::GetPackageResourcePath(
    /* [out] */ String* path)
{
    return mBase->GetPackageResourcePath(path);
}

ECode ContextWrapper::GetPackageCodePath(
    /* [out] */ String* codePath)
{
    return mBase->GetPackageCodePath(codePath);
}

ECode ContextWrapper::GetSharedPrefsFile(
    /* [in] */ const String& name,
    /* [out] */IFile** file)
{
    return mBase->GetSharedPrefsFile(name, file);
}

ECode ContextWrapper::GetSharedPreferences(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ ISharedPreferences** prefs)
{
    return mBase->GetSharedPreferences(name, mode, prefs);
}

ECode ContextWrapper::OpenFileInput(
    /* [in] */ const String& name,
    /* [out] */ IFileInputStream** fileInputStream)
{
    return mBase->OpenFileInput(name, fileInputStream);
}

ECode ContextWrapper::OpenFileOutput(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFileOutputStream** fileOutputStream)
{
    return mBase->OpenFileOutput(name, mode, fileOutputStream);
}

ECode ContextWrapper::DeleteFile(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    return mBase->DeleteFile(name, succeeded);
}

ECode ContextWrapper::GetFileStreamPath(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    return mBase->GetFileStreamPath(name, file);
}

ECode ContextWrapper::GetFileList(
    /* [out, callee] */ ArrayOf<String>** fileList)
{
    return mBase->GetFileList(fileList);
}

ECode ContextWrapper::GetFilesDir(
    /* [out] */ IFile** filesDir)
{
    return mBase->GetFilesDir(filesDir);
}

ECode ContextWrapper::GetNoBackupFilesDir(
    /* [out] */ IFile** filesDir)
{
    return mBase->GetNoBackupFilesDir(filesDir);
}

ECode ContextWrapper::GetExternalFilesDir(
    /* [in] */ const String& type,
    /* [out] */ IFile** filesDir)
{
    return mBase->GetExternalFilesDir(type, filesDir);
}

ECode ContextWrapper::GetExternalFilesDirs(
    /* [in] */ const String& type,
    /* [out, callee] */ ArrayOf<IFile*>** fileList)
{
    return mBase->GetExternalFilesDirs(type, fileList);
}

ECode ContextWrapper::GetObbDir(
    /* [out] */ IFile** obbDir)
{
    return mBase->GetObbDir(obbDir);
}

ECode ContextWrapper::GetObbDirs(
    /* [out, callee] */ ArrayOf<IFile*>** fileList)
{
    return mBase->GetObbDirs(fileList);
}

ECode ContextWrapper::GetCacheDir(
    /* [out] */ IFile** cacheDir)
{
    return mBase->GetCacheDir(cacheDir);
}

ECode ContextWrapper::GetCodeCacheDir(
    /* [out] */ IFile** cacheDir)
{
    return mBase->GetCodeCacheDir(cacheDir);
}

ECode ContextWrapper::GetExternalCacheDir(
    /* [out] */ IFile** externalDir)
{
    return mBase->GetExternalCacheDir(externalDir);
}

ECode ContextWrapper::GetExternalCacheDirs(
    /* [out, callee] */ ArrayOf<IFile*>** fileList)
{
    return mBase->GetExternalCacheDirs(fileList);
}

ECode ContextWrapper::GetExternalMediaDirs(
    /* [out, callee] */ ArrayOf<IFile*>** fileList)
{
    return mBase->GetExternalMediaDirs(fileList);
}

ECode ContextWrapper::GetDir(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    return mBase->GetDir(name, mode, dir);
}

ECode ContextWrapper::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    return mBase->OpenOrCreateDatabase(name, mode, factory, sqliteDB);
}

ECode ContextWrapper::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    return mBase->OpenOrCreateDatabase(name, mode, factory, errorHandler, sqliteDB);
}

ECode ContextWrapper::DeleteDatabase(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    return mBase->DeleteDatabase(name, succeeded);
}

ECode ContextWrapper::GetDatabasePath(
    /* [in] */ const String& name,
    /* [out] */ IFile** path)
{
    return mBase->GetDatabasePath(name, path);
}

ECode ContextWrapper::GetDatabaseList(
    /* [out, callee] */ ArrayOf<String>** databaseList)
{
    return mBase->GetDatabaseList(databaseList);
}

ECode ContextWrapper::GetWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return mBase->GetWallpaper(drawable);
}

ECode ContextWrapper::PeekWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return mBase->PeekWallpaper(drawable);
}

ECode ContextWrapper::GetWallpaperDesiredMinimumWidth(
    /* [out] */ Int32* minWidth)
{
    return mBase->GetWallpaperDesiredMinimumWidth(minWidth);
}

ECode ContextWrapper::GetWallpaperDesiredMinimumHeight(
    /* [out] */ Int32* minHeight)
{
    return mBase->GetWallpaperDesiredMinimumHeight(minHeight);
}

ECode ContextWrapper::SetWallpaper(
    /* [in] */ IBitmap* bitmap)
{
    return mBase->SetWallpaper(bitmap);
}

ECode ContextWrapper::SetWallpaper(
    /* [in] */ IInputStream* data)
{
    return mBase->SetWallpaper(data);
}

ECode ContextWrapper::ClearWallpaper()
{
    return mBase->ClearWallpaper();
}

ECode ContextWrapper::StartActivity(
    /* [in] */ IIntent* intent)
{
    return mBase->StartActivity(intent);
}

ECode ContextWrapper::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->StartActivityAsUser(intent, user);
}

ECode ContextWrapper::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    return mBase->StartActivity(intent, options);
}

ECode ContextWrapper::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userId)
{
    return mBase->StartActivityAsUser(intent, options, userId);
}

ECode ContextWrapper::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents)
{
    return mBase->StartActivities(intents);
}

ECode ContextWrapper::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    return mBase->StartActivities(intents, options);
}

ECode ContextWrapper::StartActivitiesAsUser(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userHandle)
{
    return mBase->StartActivitiesAsUser(intents, options, userHandle);
}

ECode ContextWrapper::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return mBase->StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags);
}

ECode ContextWrapper::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    return mBase->StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags, options);
}

ECode ContextWrapper::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->SendBroadcast(intent);
}

ECode ContextWrapper::SendBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendBroadcast(intent, receiverPermission);
}

ECode ContextWrapper::SendBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ Int32 appOp)
{
    return ((Context*)mBase)->SendBroadcast(intent, receiverPermission, appOp);
}

ECode ContextWrapper::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendOrderedBroadcast(intent, receiverPermission);
}

ECode ContextWrapper::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendOrderedBroadcast(intent, receiverPermission, resultReceiver, scheduler,
        initialCode, initialData, initialExtras);
}

ECode ContextWrapper::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return ((Context*)mBase)->SendOrderedBroadcast(intent, receiverPermission, appOp, resultReceiver, scheduler,
        initialCode, initialData, initialExtras);
}

ECode ContextWrapper::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->SendBroadcastAsUser(intent, user);
}

ECode ContextWrapper::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission)
{
    return mBase->SendBroadcastAsUser(intent, user, receiverPermission);
}

ECode ContextWrapper::SendOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendOrderedBroadcastAsUser(intent, user, receiverPermission, resultReceiver, scheduler,
        initialCode, initialData, initialExtras);
}

ECode ContextWrapper::SendOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return ((Context*)mBase)->SendOrderedBroadcastAsUser(intent, user, receiverPermission, appOp,
        resultReceiver, scheduler, initialCode, initialData, initialExtras);
}

ECode ContextWrapper::SendStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->SendStickyBroadcast(intent);
}

ECode ContextWrapper::SendStickyOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendStickyOrderedBroadcast(intent, resultReceiver, scheduler, initialCode, initialData,
        initialExtras);
}

ECode ContextWrapper::RemoveStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return mBase->RemoveStickyBroadcast(intent);
}

ECode ContextWrapper::SendStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->SendStickyBroadcastAsUser(intent, user);
}

ECode ContextWrapper::SendStickyOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return mBase->SendStickyOrderedBroadcastAsUser(intent, user, resultReceiver, scheduler, initialCode,
        initialData, initialExtras);
}

ECode ContextWrapper::RemoveStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return mBase->RemoveStickyBroadcastAsUser(intent, user);
}

ECode ContextWrapper::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [out] */ IIntent** stickyIntent)
{
    VALIDATE_NOT_NULL(stickyIntent)
    return mBase->RegisterReceiver(receiver, filter, stickyIntent);
}

ECode ContextWrapper::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    VALIDATE_NOT_NULL(stickyIntent)
    return mBase->RegisterReceiver(receiver, filter, broadcastPermission, scheduler, stickyIntent);
}

ECode ContextWrapper::RegisterReceiverAsUser(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IUserHandle* user,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    VALIDATE_NOT_NULL(stickyIntent)
    return mBase->RegisterReceiverAsUser(receiver, user, filter, broadcastPermission, scheduler, stickyIntent);
}

ECode ContextWrapper::UnregisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver)
{
    return mBase->UnregisterReceiver(receiver);
}

ECode ContextWrapper::StartService(
    /* [in] */ IIntent* service,
    /* [out] */ IComponentName** name)
{
    return mBase->StartService(service, name);
}

ECode ContextWrapper::StopService(
    /* [in] */ IIntent* service,
    /* [out] */ Boolean* succeeded)
{
    return mBase->StopService(service, succeeded);
}

ECode ContextWrapper::StartServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ IComponentName** name)
{
    return mBase->StartServiceAsUser(service, user, name);
}

ECode ContextWrapper::StopServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    return mBase->StopServiceAsUser(service, user, succeeded);
}

ECode ContextWrapper::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    return mBase->BindService(service, conn, flags, succeeded);
}

ECode ContextWrapper::BindServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* succeeded)
{
    return ((Context*)mBase)->BindServiceAsUser(service, conn, flags, userHandle, succeeded);
}

ECode ContextWrapper::UnbindService(
    /* [in] */ IServiceConnection* conn)
{
    return mBase->UnbindService(conn);
}

ECode ContextWrapper::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* succeeded)
{
    return mBase->StartInstrumentation(className, profileFile, arguments, succeeded);
}

ECode ContextWrapper::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    return mBase->GetSystemService(name, object);
}

ECode ContextWrapper::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckPermission(permission, pid, uid, permissionId);
}

ECode ContextWrapper::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckCallingPermission(permission, permissionId);
}

ECode ContextWrapper::CheckCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckCallingOrSelfPermission(permission, permissionId);
}

ECode ContextWrapper::EnforcePermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& message)
{
    return mBase->EnforcePermission(permission, pid, uid, message);
}

ECode ContextWrapper::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingPermission(permission, message);
}

ECode ContextWrapper::EnforceCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingOrSelfPermission(permission, message);
}

ECode ContextWrapper::GrantUriPermission(
    /* [in] */ const String& toPackage,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return mBase->GrantUriPermission(toPackage, uri, modeFlags);
}

ECode ContextWrapper::RevokeUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return mBase->RevokeUriPermission(uri, modeFlags);
}

ECode ContextWrapper::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckUriPermission(uri, pid, uid, modeFlags, permissionId);
}

ECode ContextWrapper::CheckCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckCallingUriPermission(uri, modeFlags, permissionId);
}

ECode ContextWrapper::CheckCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckCallingOrSelfUriPermission(uri, modeFlags, permissionId);
}

ECode ContextWrapper::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* permissionId)
{
    return mBase->CheckUriPermission(uri, readPermission, writePermission, pid, uid, modeFlags, permissionId);
}

ECode ContextWrapper::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceUriPermission(uri, pid, uid, modeFlags, message);
}

ECode ContextWrapper::EnforceCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingUriPermission(uri, modeFlags, message);
}

ECode ContextWrapper::EnforceCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceCallingOrSelfUriPermission(uri, modeFlags, message);
}

ECode ContextWrapper::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return mBase->EnforceUriPermission(uri, readPermission, writePermission, pid, uid, modeFlags, message);
}

ECode ContextWrapper::CreatePackageContext(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    return mBase->CreatePackageContext(packageName, flags, ctx);
}

ECode ContextWrapper::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    return ((Context*)mBase)->CreatePackageContextAsUser(packageName, flags, user, ctx);
}

ECode ContextWrapper::CreateApplicationContext(
    /* [in] */ IApplicationInfo* application,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    return CreateApplicationContext(application, String(NULL), flags, ctx);
}

ECode ContextWrapper::CreateApplicationContext(
    /* [in] */ IApplicationInfo* application,
    /* [in] */ const String& themePackageName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    return mBase->CreateApplicationContext(application, themePackageName, flags, ctx);
}

ECode ContextWrapper::GetUserId(
    /* [out] */ Int32* userId)
{
    return ((Context*)mBase)->GetUserId(userId);
}

ECode ContextWrapper::CreateConfigurationContext(
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [out] */ IContext** ctx)
{
    return mBase->CreateConfigurationContext(overrideConfiguration, ctx);
}

ECode ContextWrapper::CreateDisplayContext(
    /* [in] */ IDisplay* display,
    /* [out] */ IContext** ctx)
{
    return mBase->CreateDisplayContext(display, ctx);
}

ECode ContextWrapper::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ const String& themePackageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    return mBase->CreatePackageContextAsUser(packageName, themePackageName, flags, user, ctx);
}

ECode ContextWrapper::IsRestricted(
    /* [out] */ Boolean* isRestricted)
{
    return mBase->IsRestricted(isRestricted);
}

ECode ContextWrapper::GetDisplayAdjustments(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayAdjustments** da)
{
    return ((Context*)mBase)->GetDisplayAdjustments(displayId, da);
}

}
}
}