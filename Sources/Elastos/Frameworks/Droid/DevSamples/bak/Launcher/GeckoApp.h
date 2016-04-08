
#ifndef __GECKOAPP_H__
#define __GECKOAPP_H__

#include "elastos/droid/app/Activity.h"
#include "CGeckoSurfaceView.h"
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using Elastos::Droid::App::Activity;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Droid::View::ISurfaceView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IAbsoluteLayout;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

#define GeckoApp_ACTION_ALERT_CLICK  \
        "org.mozilla.gecko.ACTION_ALERT_CLICK"

#define GeckoApp_ACTION_ALERT_CLEAR \
         "org.mozilla.gecko.ACTION_ALERT_CLEAR"

#define GeckoApp_FILE_PICKER_REQUEST 1

#define GeckoApp_LOG_FILE_NAME \
         "GeckoApp"

#define GeckoApp_ACTION_WEBAPP \
         "org.mozilla.gecko.WEBAPP"

#define GeckoApp_ACTION_DEBUG \
         "org.mozilla.gecko.DEBUG"

#define GeckoApp_ACTION_BOOKMARK \
         "org.mozilla.gecko.BOOKMARK"

#define GeckoApp_PLUGIN_ACTION \
         "android.webkit.PLUGIN"

#define GeckoApp_PLUGIN_PERMISSION \
         "android.webkit.permission.PLUGIN"


class GeckoApp : public Activity
{
public:
    enum LaunchState {
        LaunchState_PreLaunch,
        LaunchState_Launching,
        LaunchState_WaitForDebugger,
        LaunchState_Launched,
        LaunchState_GeckoRunning,
        LaunchState_GeckoExiting
     };

public:
    class GeckoAppDialogOnClickListener
        : public ElRefBase
        , public IDialogInterfaceOnClickListener
    {
    public:
        GeckoAppDialogOnClickListener(
            /* [in] */ GeckoApp* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* pObject,
            /* [in] */ InterfaceID* pIID);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<GeckoApp> mHost;
    };

public:
    GeckoApp();

    ~GeckoApp();

    static CARAPI_(Boolean) CheckLaunchState(
        /* [in] */ LaunchState checkState);

    static CARAPI_(void) SetLaunchState(
        /* [in] */ LaunchState setState);

    // if mLaunchState is equal to checkState this sets mLaunchState to setState
    // and return true. Otherwise we return false.
    static CARAPI_(Boolean) CheckAndSetLaunchState(
        /* [in] */ LaunchState checkState,
        /* [in] */ LaunchState setState);

    CARAPI ShowErrorDialog(
        /* [in] */ const String& message);

    // Returns true when the intent is going to be handled by gecko launch
    CARAPI_(Boolean) Launch(
        /* [in] */ IIntent* intent);

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnRestart();

    CARAPI OnStop();

    CARAPI OnDestroy();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnLowMemory();

    ECode RemoveFiles();

    ECode AddEnvToIntent(
        /* [in] */ IIntent* intent);

    ECode DoRestart();

    ECode HandleNotification(
        /* [in] */ const String& action,
        /* [in] */ const String& alertName,
        /* [in] */ const String& alertCookie);

    ECode ShowFilePicker(
        /* [in] */ const String& aMimeType,
        /* [out] */ String* filePickerResult);

    virtual ECode GetPackageName(
        /* [out] */ String* packageName) = 0;

    virtual ECode GetContentProcessName(
        /* [out] */ String* processName) = 0;

    ECode UnpackComponents();

    ECode EnableCameraView();

    ECode DisableCameraView();

protected:
    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    ECode UnpackFile(
        /* [in] */ IZipFile* zip,
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ IZipEntry* fileEntry,
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    ECode CheckAndLaunchUpdate();

    ECode ReadUpdateStatus(
        /* [in] */ IFile* statusFile,
        /* [out] */ String* status);

    static void* LaunchEntryRoutine(void *arg);



public:

    static AutoPtr<IAbsoluteLayout> sMainLayout;
    static AutoPtr<CGeckoSurfaceView> sSurfaceView;
    static AutoPtr<ISurfaceView> sCameraView;
    static AutoPtr<GeckoApp> sAppContext;
    static Boolean sFullscreen;
    static AutoPtr<IFile> sGREDir;

    AutoPtr<IThread> mLibLoadThread;
    AutoPtr<IHandler> mMainHandler;
    Boolean mHaveKilledZombies;
    AutoPtr<IButton> mLaunchButton;
    AutoPtr<IIntent> mLaunchIntent;
    //List< AutoPtr<IPackageInfo> > mPackageInfoCache;

friend class GeckoApp_Launch;
friend class LoopRunnable;

private:
    static LaunchState sLaunchState;
    static Object sSyncLaunchState;
    static Object sSyncPackageInfoCache;
    static Boolean sTryCatchAttached;

    AutoPtr<IIntentFilter> mConnectivityFilter;
    AutoPtr<IBroadcastReceiver> mConnectivityReceiver;
    AutoPtr<IBroadcastReceiver> mBatteryReceiver;
    //SynchronousQueue<String> mFilePickerResult;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__GECKOAPP_H__

