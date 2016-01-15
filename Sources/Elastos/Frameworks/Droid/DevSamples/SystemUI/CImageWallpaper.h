
#ifndef __CIMAGEWALLPAPER_H__
#define __CIMAGEWALLPAPER_H__

#include "_CImageWallpaper.h"
#include <ext/frameworkdef.h>
#include <service/wallpaper/WallpaperService.h>

using Elastos::Droid::Service::Wallpaper::WallpaperService;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

CarClass(CImageWallpaper), public WallpaperService
{
private:
    class DrawableEngine : public Engine
    {
    private:
        class WallpaperObserver : public BroadcastReceiver
        {
        public:
            WallpaperObserver(
                /* [in] */ DrawableEngine* owner);

            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

            CARAPI ToString(
                /* [out] */ String* info)
            {
                VALIDATE_NOT_NULL(info);
                *info = String("CImageWallpaper::DrawableEngine::WallpaperObserver");
                (*info).AppendFormat("%p", this);
                return NOERROR;
            }
        private:
            DrawableEngine* mOwner;
        };

    public:
        DrawableEngine(
            /* [in] */ CImageWallpaper* owner);

        CARAPI_(void) TrimMemory(
            /* [in] */ Int32 level);

        //@Override
        CARAPI_(void) OnCreate(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        //@Override
        CARAPI_(void) OnDestroy();

        //@Override
        CARAPI_(void) OnDesiredSizeChanged(
            /* [in] */ Int32 desiredWidth,
            /* [in] */ Int32 desiredHeight);

        //@Override
        CARAPI_(void) OnVisibilityChanged(
            /* [in] */ Boolean visible);

        //@Override
        CARAPI_(void) OnTouchEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI_(void) OnOffsetsChanged(
            /* [in] */ Float xOffset,
            /* [in] */ Float yOffset,
            /* [in] */ Float xOffsetStep,
            /* [in] */ Float yOffsetStep,
            /* [in] */ Int32 xPixels,
            /* [in] */ Int32 yPixels);

        //@Override
        CARAPI_(void) OnSurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        //@Override
        CARAPI_(void) OnSurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

        //@Override
        CARAPI_(void) OnSurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        //@Override
        CARAPI_(void) OnSurfaceRedrawNeeded(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CARAPI_(void) UpdateSurfaceSize(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        CARAPI_(void) DrawFrameLocked();

        CARAPI_(void) UpdateWallpaperLocked();

        CARAPI_(void) DrawWallpaperWithCanvas(
            /* [in] */ ISurfaceHolder* sh,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

    private:
        static const Int32 EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        static const Int32 EGL_OPENGL_ES2_BIT = 4;

        static const Int32 FLOAT_SIZE_BYTES = 4;
        static const Int32 TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 5 * FLOAT_SIZE_BYTES;
        static const Int32 TRIANGLE_VERTICES_DATA_POS_OFFSET = 0;
        static const Int32 TRIANGLE_VERTICES_DATA_UV_OFFSET = 3;

    private:
        Object mLock;

        // TODO: Not currently used, keeping around until we know we don't need it
        //@SuppressWarnings({"UnusedDeclaration"})
        AutoPtr<WallpaperObserver> mReceiver;

        AutoPtr<IBitmap> mBackground;
        Int32 mLastSurfaceWidth;
        Int32 mLastSurfaceHeight;
        Int32 mLastRotation;
        Float mXOffset;
        Float mYOffset;

        Boolean mVisible;
        Boolean mRedrawNeeded;
        Boolean mOffsetsChanged;
        Int32 mLastXTranslation;
        Int32 mLastYTranslation;

        // private EGL10 mEgl;
        // private EGLDisplay mEglDisplay;
        // private EGLConfig mEglConfig;
        // private EGLContext mEglContext;
        // private EGLSurface mEglSurface;
        // private GL mGL;

        // private static final String sSimpleVS =
        //         "attribute vec4 position;\n" +
        //         "attribute vec2 texCoords;\n" +
        //         "varying vec2 outTexCoords;\n" +
        //         "uniform mat4 projection;\n" +
        //         "\nvoid main(void) {\n" +
        //         "    outTexCoords = texCoords;\n" +
        //         "    gl_Position = projection * position;\n" +
        //         "}\n\n";
        // private static final String sSimpleFS =
        //         "precision mediump Float;\n\n" +
        //         "varying vec2 outTexCoords;\n" +
        //         "uniform sampler2D texture;\n" +
        //         "\nvoid main(void) {\n" +
        //         "    gl_FragColor = texture2D(texture, outTexCoords);\n" +
        //         "}\n\n";

        CImageWallpaper* mOwner;
    };

public:
    CImageWallpaper();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    CARAPI OnRebind(
        /* [in] */ IIntent* intent);

    CARAPI GetClassName(
        /* [out] */ String* className);

    /** Return the application that owns this service. */
    CARAPI GetApplication(
        /* [out] */ IApplication** application);

    CARAPI OnTaskRemoved(
        /* [in] */ IIntent* rootIntent);

    CARAPI StopSelf();

    CARAPI StopSelf(
        /* [in] */ Int32 startId);

    CARAPI StopSelfResult(
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* res);

    CARAPI SetForeground(
        /* [in] */ Boolean isForeground);

    CARAPI StartForeground(
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification);

    CARAPI StopForeground(
        /* [in] */ Boolean removeNotification);

    CARAPI Attach(
        /* [in] */ IContext* ctx,
        /* [in] */ IActivityThread* thread,
        /* [in] */ const String& className,
        /* [in] */ IBinder* token,
        /* [in] */ IApplication* application,
        /* [in] */ IIActivityManager* activityManager);

    //
    // IContext interfaces
    //
    /**
     * @return the base context as set by the constructor or setBaseContext
     */
    CARAPI GetBaseContext(
        /* [out] */ IContext** ctx);

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

    CARAPI GetExternalFilesDir(
        /* [in] */ const String& type,
        /* [out] */ IFile** filesDir);

    CARAPI GetObbDir(
        /* [out] */ IFile** obbDir);

    CARAPI GetCacheDir(
        /* [out] */ IFile** cacheDir);

    CARAPI GetExternalCacheDir(
        /* [out] */ IFile** externalDir);

    CARAPI GetFileList(
        /* [out, callee] */ ArrayOf<String>** fileList);

    CARAPI GetDir(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IFile** dir);

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
        /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI BindService(
        /* [in] */ IIntent* service,
        /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* succeeded);

    CARAPI UnbindService(
        /* [in] */ Elastos::Droid::Content::IServiceConnection* conn);

    CARAPI StartInstrumentation(
        /* [in] */ IComponentName* className,
        /* [in] */ const String& profileFile,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* succeeded);

    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

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

    CARAPI CreatePackageContext(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [out] */ IContext** ctx);

    CARAPI CreatePackageContextAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ IUserHandle* user,
        /* [out] */ IContext** ctx);

    CARAPI CreateConfigurationContext(
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [out] */ IContext** ctx);

    CARAPI CreateDisplayContext(
        /* [in] */ IDisplay* display,
        /* [out] */ IContext** ctx);

    CARAPI GetCompatibilityInfo(
        /* [in] */ Int32 displayId,
        /* [out] */ ICompatibilityInfoHolder** infoHolder);

    CARAPI IsRestricted(
        /* [out] */ Boolean* isRestricted);

    CARAPI_(AutoPtr<WallpaperService::Engine>) OnCreateEngine();

private:
    static const String TAG;
    static const String GL_LOG_TAG;
    static const Boolean DEBUG;
    static const String PROPERTY_KERNEL_QEMU;

    static const Boolean FIXED_SIZED_SURFACE = TRUE;
    static const Boolean USE_OPENGL = TRUE;

private:
    AutoPtr<IWallpaperManager> mWallpaperManager;

    AutoPtr<DrawableEngine> mEngine;

    Boolean mIsHwAccelerated;
};


} // namespace SystemUI
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CIMAGEWALLPAPER_H__
