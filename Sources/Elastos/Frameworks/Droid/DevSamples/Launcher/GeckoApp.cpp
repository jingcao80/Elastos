
#include "elastos/droid/ext/frameworkext.h"
#include "GeckoApp.h"
#include "GeckoRunnable.h"
#include "GeckoAppShell.h"
#include <stdio.h>
#include <os/Build.h>
#include "CGeckoConnectivityReceiver.h"
#include "CGeckoNetworkManager.h"
#include "CGeckoBatteryManager.h"
#include <elastos/System.h>

#include <unistd.h>

using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::Utility::Zip::CZipFile;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IAlertDialogBuilder;
//using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::CSurfaceView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
//using Elastos::Droid::Widget::CButton;
using Elastos::Droid::Widget::CAbsoluteLayout;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsoluteLayoutLayoutParams;
using Elastos::Droid::Widget::CAbsoluteLayoutLayoutParams;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

//==============================================================================
//          LaunchRunnable
//==============================================================================
class LaunchRunnable : public GeckoRunnable
{
public:
    LaunchRunnable(
        /* [in] */ GeckoApp* host, IIntent* i )
        : GeckoRunnable((Void*)host),i(i)
    {}

    ECode Run()
    {
        PRINT_ENTER_LEAVE("LaunchRunnable::Run()");

        ECode ec;
        GeckoApp* host = (GeckoApp*)mHost;

        if (!host->mLibLoadThread) {
            host->mLibLoadThread->Join();
        }

        // Show the URL we are about to load, if the intent has one
        String action;
        i->GetAction(&action);
        if (action.Equals("android.intent.action.VIEW")) {//action.Equals(Intent_ACTION_VIEW)) {
            String dataString;
            i->GetDataString(&dataString);
            CGeckoSurfaceView::sSplashURL = dataString;
        }

        // GeckoApp::sSurfaceView->DrawSplashScreen();
        // unpack files in the components directory
        //try {
        ec = host->UnpackComponents();
        if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            PRINT_FILE_LINE_EX("GeckoApp error unpacking components: file not found");
            //Looper:Prepare();
            String error;
            host->GetString(0x7f050004, &error); //R.string.error_loading_file
            host->ShowErrorDialog(error);
            //Looper::Loop();
            return NOERROR;
        }
        else if (ec == (ECode)E_IO_EXCEPTION){
            PRINT_FILE_LINE_EX("GeckoApp error unpacking components: io error");
            String msg("io error");
            //Looper::Prepare();
            /*if (msg != null && msg.equalsIgnoreCase("No space left on device"))
                showErrorDialog(getString(0x7f050003)); //R.string.no_space_to_start_error
            else
                showErrorDialog(getString(0x7f050004)); //R.string.error_loading_file*/
            //Looper::Loop();
            return NOERROR;
        }
        /*} catch (FileNotFoundException fnfe) {
            Log.e("GeckoApp", "error unpacking components", fnfe);
            Looper.prepare();
            showErrorDialog(getString(0x7f050004));//R.string.error_loading_file
            Looper.loop();
            return;
        } catch (IOException ie) {
            Log.e("GeckoApp", "error unpacking components", ie);
            String msg = ie.getMessage();
            Looper.prepare();
            if (msg != null && msg.equalsIgnoreCase("No space left on device"))
                showErrorDialog(getString(R.string.no_space_to_start_error));
            else
                showErrorDialog(getString(R.string.error_loading_file));
            Looper.loop();
            return;
        }*/

        // and then fire us up
        //try {
            String env;
            FAIL_RETURN(host->mLaunchIntent->GetStringExtra(String("env0"), &env));

            AutoPtr<IApplication> app;
            FAIL_RETURN(host->GetApplication((IApplication**)&app));
            String resPath;
            //ec = pApp->GetPackageResourcePath(resPath&);

            String args, data;
            FAIL_RETURN(host->mLaunchIntent->GetStringExtra(String("args"), &args));
            FAIL_RETURN(host->mLaunchIntent->GetDataString(&data));
            ec = GeckoAppShell::RunGecko(resPath, args, data);
            if (FAILED(ec)) {
                PRINT_FILE_LINE_EX("GeckoApp top level exception RunGecko %x", ec);
                String error("run gecko failed");
                GeckoAppShell::ReportJavaCrash(error);
            }
        /*} catch (Exception e) {
            Log.e("GeckoApp", "top level exception", e);
            StringWriter sw = new StringWriter();
            e.printStackTrace(new PrintWriter(sw));
            GeckoAppShell.reportJavaCrash(sw.toString());
        }*/
        return NOERROR;
    }

private:
    AutoPtr<IIntent> i;
};

//==============================================================================
//          LoopRunnable
//==============================================================================

class LoopRunnable : public GeckoRunnable
{
public:
    LoopRunnable(
        /* [in] */ GeckoApp* host)
        : GeckoRunnable((Void*)host)
    {}

    ECode Run()
    {
        /*ECode ec = Looper::Loop();
        if (FAILED(ec)) {
            PRINT_FILE_LINE_EX("GeckoApp top level exception Looper::Loop %x\n", ec);
            GeckoAppShell::ReportJavaCrash(String("Looper::Loop error"));
        }*/
        GeckoApp::sTryCatchAttached = FALSE;
        return NOERROR;
    }
};

class LibLoadRunnable : public GeckoRunnable
{
public:
    LibLoadRunnable(
        /* [in] */ GeckoApp* host)
        : GeckoRunnable((Void*)host)
    {
    }

    ECode Run()
    {
        // At some point while loading the gecko libs our default locale gets set
        // so just save it to locale here and reset it as default after the join
        AutoPtr<ILocale> pLocale;
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        FAIL_RETURN(helper->GetDefault((ILocale**)&pLocale));

        GeckoApp* host = (GeckoApp*)mHost;
//        AutoPtr<IApplication> app;
//        FAIL_RETURN(host->GetApplication((IApplication**)&app));
//        String path("/data/data/com.elastos.runtime/elastos/Launcher");
//        FAIL_RETURN(app->GetPackageResourcePath(&path));
//        FAIL_RETURN(GeckoAppShell::LoadGeckoLibs(path));

        helper->SetDefault(pLocale);

        AutoPtr<IContext> pIContext;
        AutoPtr<IResources> pRes;
        AutoPtr<IConfiguration> pConfig;

        FAIL_RETURN(host->GetBaseContext((IContext**)&pIContext));
        FAIL_RETURN(pIContext->GetResources((IResources**)&pRes));
        FAIL_RETURN(pRes->GetConfiguration((IConfiguration**)&pConfig));
        FAIL_RETURN(pConfig->SetLocale(pLocale));

        AutoPtr<IDisplayMetrics> pIDM;
        FAIL_RETURN(pRes->GetDisplayMetrics((IDisplayMetrics**)&pIDM));
        FAIL_RETURN(pRes->UpdateConfiguration(pConfig, pIDM));
        return NOERROR;
    }
};

//==============================================================================
//          GeckoApp::GeckoAppDialogOnClickListener
//==============================================================================

GeckoApp::GeckoAppDialogOnClickListener::GeckoAppDialogOnClickListener(
    /* [in] */ GeckoApp* host)
{
    mHost = host;
}

PInterface GeckoApp::GeckoAppDialogOnClickListener::Probe(
    /* [in] */ REIID riid)
{
     if (riid == EIID_IInterface) {
        return (PInterface)(IDialogInterfaceOnClickListener*)this;
    }
    else if (riid == EIID_IDialogInterfaceOnClickListener) {
        return (IDialogInterfaceOnClickListener*)this;
    }

    return NULL;
}

UInt32 GeckoApp::GeckoAppDialogOnClickListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GeckoApp::GeckoAppDialogOnClickListener::Release()
{
    return ElRefBase::Release();
}

ECode GeckoApp::GeckoAppDialogOnClickListener::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode GeckoApp::GeckoAppDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->Finish();
    //System.exit(0);
    return NOERROR;
}

//==============================================================================
//          GeckoApp
//==============================================================================
AutoPtr<IAbsoluteLayout> GeckoApp::sMainLayout;
AutoPtr<CGeckoSurfaceView> GeckoApp::sSurfaceView;
AutoPtr<GeckoApp> GeckoApp::sAppContext;
AutoPtr<ISurfaceView> GeckoApp::sCameraView;
Boolean GeckoApp::sFullscreen = FALSE;
AutoPtr<IFile> GeckoApp::sGREDir;
GeckoApp::LaunchState GeckoApp::sLaunchState = GeckoApp::LaunchState_PreLaunch;
Mutex GeckoApp::sSyncLaunchState;
Boolean GeckoApp::sTryCatchAttached = FALSE;

GeckoApp::GeckoApp()
    : mHaveKilledZombies(FALSE)
{
}

GeckoApp::~GeckoApp()
{
}

Boolean GeckoApp::CheckLaunchState(
    /* [in] */ LaunchState checkState)
{
    AutoLock lock(&sSyncLaunchState);
    return sLaunchState == checkState;
}

void GeckoApp::SetLaunchState(
    /* [in] */ LaunchState setState)
{
    AutoLock lock(&sSyncLaunchState);
    sLaunchState = setState;
}

// if mLaunchState is equal to checkState this sets mLaunchState to setState
// and return true. Otherwise we return false.
Boolean GeckoApp::CheckAndSetLaunchState(
    /* [in] */ LaunchState checkState,
    /* [in] */ LaunchState setState)
{
    AutoLock lock(&sSyncLaunchState);
    if (sLaunchState != checkState) {
        return FALSE;
    }
    sLaunchState = setState;
    return TRUE;
}

ECode GeckoApp::ShowErrorDialog(
    /* [in] */ const String& message)
{
    AutoPtr<IAlertDialogBuilder> builder;
    ECode ec;
//    ec = CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
//    if (FAILED(ec)) return ec;

    AutoPtr<ICharSequence> charS;
    ec = CStringWrapper::New(message, (ICharSequence**)&(charS));
    if (FAILED(ec)) return ec;
    builder->SetMessage(charS);
    builder->SetCancelable(FALSE);
    AutoPtr<GeckoAppDialogOnClickListener> listener = new GeckoAppDialogOnClickListener(this);
    builder->SetPositiveButton(0x7f05000d, //R.string.exit_label,
                              listener);

    AutoPtr<IAlertDialog> dialog;
    return builder->Show((IAlertDialog**)&dialog);
}


#define LOGTAG \
         "PluginManager"

#define PLUGIN_SYSTEM_LIB \
         "/system/lib/plugins/"

#define PLUGIN_TYPE \
         "type"

#define TYPE_NATIVE \
         "native"
/*
List<String> GeckoApp::GetPluginDirectories() {
    List<String> directories;
    AutoPtr<IPackageManager> pm;
    AutoPtr<IIntent> intent;
    ArrayOf<IResolveInfo*> *plugins;

    sAppContext->GetPackageManager((IPackageManager**)&pm);

    CIntent::New(GeckoApp_PLUGIN_ACTION, (IIntent**)&intent);

    pm->QueryIntentServices(intent,
            PackageManager_GET_SERVICES | PackageManager_GET_META_DATA,
            (ArrayOf<IResolveInfo*>**)&plugins);

    AutoLock lock(sSyncPackageInfoCache);
    // clear the list of existing packageInfo objects
    mPackageInfoCache.clear();

    for (UInt32 i = 0; i < plugins->GetLength(); i++) {
        IResolveInfo* plugin = (*plugins)[i];
        // retrieve the plugin's service information
        AutoPtr<IServiceInfo> serviceInfo;
        plugin->ServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) {
            printf("Ignore bad plugin");
            continue;
        }

        String pkgName;
        serviceInfo->GetPackageName(&pkgName);
        printf("Loading plugin: %s", pkgName);

        // retrieve information from the plugin's manifest
        AutoPtr<IPackageInfo> pkgInfo;
        ECode ec;
        ec = pm->getPackageInfo(pkgName,
                        PackageManager_GET_PERMISSIONS
                        | PackageManager_GET_SIGNATURES,
                        (IPackageInfo**)&pkgInfo);

        if (FAILED(ec)) {
            printf("Can't find plugin: %s\n", pkgName);
            continue;
        }

        if (pkgInfo == NULL) {
            printf("Loading plugin: %s%s\n", pkgName, ". Could not load package information.");
            continue;
        }


        // find the location of the plugin's shared library. The default
        // is to assume the app is either a user installed app or an
        // updated system app. In both of these cases the library is
        // stored in the app's data directory.

        String directory;
        AutoPtr<IApplicationInfo> appInfo;
        String dataDir;
        pkgInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        appInfo->GetDataDir(&directory);

        directory = dataDir + "/lib";
        Int32 appFlags;
        appInfo->GetFlags(&appFlags);
        int updatedSystemFlags = ApplicationInfo_FLAG_SYSTEM |
                                       ApplicationInfo_FLAG_UPDATED_SYSTEM_APP;
        // preloaded system app with no user updates
        if ((appFlags & updatedSystemFlags) == CApplicationInfo_FLAG_SYSTEM) {
            directory = pkgName + PLUGIN_SYSTEM_LIB;
        }

        // check if the plugin has the required permissions
        ArrayOf<String> *permissions;
        pkgInfo->GetRequestedPermissions((ArrayOf<String>**)&permissions);

        if (permissions == NULL) {
            printf("Loading plugin: %s%s\n", pkgName, ". Does not have required permission.");
            continue;
        }

        Boolean permissionOk = false;

        for (UInt32 i = 0; i < permissions->GetLength(); i++) {
            String permission = (*permissions)[i];

            if (permission->Equals(GeckoApp_PLUGIN_PERMISSION)) {
                permissionOk = true;
                break;
            }

            (*permissions)[i] == NULL;
        }

        ArrayOf<String>::Free(permissions);

        if (!permissionOk) {
            printf("Loading plugin: %s%s\n", pkgName, ". Does not have required permission (2).");
            continue;
        }

        // check to ensure the plugin is properly signed
        ArrayOf<ISignature*> *signatures;
        pkgInfo->GetSignatures((ArrayOf<ISignature*>**)&signatures);

        if (signatures == NULL) {
            printf("Loading plugin: %s%s\n", pkgName, ". Not signed.");
            continue;
        }

        for (UInt32 i = 0; i < signatures->GetLength(); i++) {
            ISignature* sig = (*signatures)[i];
            sig->Release();
        }

        ArrayOf<ISignature*>::Free(signatures);

        String serName;
        serviceInfo->GetName(&serName);

        // determine the type of plugin from the manifest
        AutoPtr<IBundle> metaData;
        serviceInfo->GetMetaData((IBundle**)&metaData);

        if (metaData == NULL) {
            printf("The plugin '%s%s\n", serName, "' has no type defined");
            continue;
        }

        String pluginType;
        metaData->GetString(PLUGIN_TYPE, &pluginType);

        if (!pluginType.Equals(TYPE_NATIVE)) {
            printf("Unrecognized plugin type: %s\n",pluginType);
            continue;
        }

        Class<?> cls;
        ec = getPluginClass(pkgName, serName, &cls);

        // TODO implement any requirements of the plugin class here!
        Boolean classFound = true;

        if (!classFound) {
            printf("The plugin's class' " + serName + "' does not extend the appropriate class.");
            continue;
        }

        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            printf("Can't find plugin: %s\n", pkgName);
            continue;
        }
        else if (ec == (ECode)E_CLASS_NOT_FOUND_EXCEPTION) {
            printf("Can't find plugin's class: %s\n", serName);
            continue;
        }

        // if all checks have passed then make the plugin available
        mPackageInfoCache.Add(pkgInfo);
        directories.Add(directory);

        testasdf
        plugin->Release();
    }
    =t=e=s=t
    ArrayOf<IResolveInfo test*>::Free(plugins);

    return testdirectories;
}

Class<?> GeckoApp::GetPluginClass(String packageName, String className)
        throws NameNotFoundException, ClassNotFoundException {
    AutoPtr<IContext> pluginContext;
    mAppContext.CreatePackageContext(packageName,
            Context.CONTEXT_INCLUDE_CODE |
            Context.CONTEXT_IGNORE_SECURITY,(IContext**)&pluginContext);
    AutoPtr<IClassLoader> pluginCL;
    pluginContext->GetClassLoader((IClassLoader**)&pluginCL);
    return pluginCL.LoadClass(className);
}

*/

class GeckoApp_Launch : public ElRefBase
{
public:
    GeckoApp_Launch(GeckoApp* host, IIntent* intent)
        : host(host)
        , intent(intent)
    {
    }

    AutoPtr<GeckoApp> host;
    AutoPtr<IIntent> intent;
};

void* GeckoApp::LaunchEntryRoutine(
    /* [in] */ void *arg)
{
    PRINT_ENTER_LEAVE("GeckoApp::LaunchEntryRoutine");

    ECode ec;
    if (arg == NULL) {
        pthread_exit((void*)E_THREAD_ABORTED);
    }

//    Int64 startup_time = System::GetCurrentTimeMillis();
    AutoPtr<GeckoApp_Launch> closure = (GeckoApp_Launch*)arg;
    AutoPtr<GeckoApp> host = closure->host;
    //try {
        if (host->mLibLoadThread != NULL) {
            //host->mLibLoadThread->Join();
        }
    //} catch (InterruptedException ie) {}
    String action;
    closure->intent->GetAction(&action);
    if (action.Equals("android.intent.action.VIEW")) {//action.Equals(Intent_ACTION_VIEW)) {
        String dataString;
        closure->intent->GetDataString(&dataString);
        GeckoApp::sSurfaceView->sSplashURL = dataString;
    }
    //ASSERT_SUCCEEDED(GeckoApp::sSurfaceView->DrawSplashScreen());

    // unpack files in the components directory
    //try {
    ec = host->UnpackComponents();
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        printf(GeckoApp_LOG_FILE_NAME " error unpacking components: file not found\n");
        //Looper:Prepare();
        String error;
        host->GetString(0x7f050004, &error); //R.string.error_loading_file
        host->ShowErrorDialog(error);
        //Looper::Loop();
        return NOERROR;
    }
    else if (ec == (ECode)E_IO_EXCEPTION){
        printf(GeckoApp_LOG_FILE_NAME " error unpacking components: io error\n");
        String msg("io error");
        //Looper::Prepare();
        /*if (msg != null && msg.equalsIgnoreCase("No space left on device"))
            showErrorDialog(getString(0x7f050003)); //R.string.no_space_to_start_error
        else
            showErrorDialog(getString(0x7f050004)); //R.string.error_loading_file*/
        //Looper::Loop();
        return NOERROR;
    }
    else if (FAILED(ec)) {
        PRINT_FILE_LINE_EX("GeckoApp error unpacking components: %x\n", ec);
        return (void*)ec;
    }
    /*} catch (FileNotFoundException fnfe) {
        Log.e("GeckoApp", "error unpacking components", fnfe);
        Looper.prepare();
        showErrorDialog(getString(0x7f050004));//R.string.error_loading_file
        Looper.loop();
        return;
    } catch (IOException ie) {
        Log.e("GeckoApp", "error unpacking components", ie);
        String msg = ie.getMessage();
        Looper.prepare();
        if (msg != null && msg.equalsIgnoreCase("No space left on device"))
            showErrorDialog(getString(R.string.no_space_to_start_error));
        else
            showErrorDialog(getString(R.string.error_loading_file));
        Looper.loop();
        return;
    }*/

    // and then fire us up
    //try {
       String env;
       ASSERT_SUCCEEDED(host->mLaunchIntent->GetStringExtra(String("env0"), &env));
       AutoPtr<IApplication> app;
       ASSERT_SUCCEEDED(host->GetApplication((IApplication**)&app));
       String resPath("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.apk");
       //ec = pApp->GetPackageResourcePath(resPath&);
       String args, data;
       ASSERT_SUCCEEDED(host->mLaunchIntent->GetStringExtra(String("args"), &args));
       host->mLaunchIntent->GetDataString(&data);
       ASSERT_SUCCEEDED(GeckoAppShell::RunGecko(resPath, args, data));
       if (FAILED(ec)) {
           printf(GeckoApp_LOG_FILE_NAME " top level exception RunGecko %x\n", ec);
           String error("run gecko failed");
           GeckoAppShell::ReportJavaCrash(error);
       }
    /*} catch (Exception e) {
        Log.e(GeckoApp_LOG_FILE_NAME, "top level exception", e);
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        e.printStackTrace(pw);
        pw.flush();
        GeckoAppShell.reportJavaCrash(sw.toString());
    }*/
    return (void*)ec;
}

// Returns true when the intent is going to be handled by gecko launch
Boolean GeckoApp::Launch(
    /* [in] */ IIntent* _intent)
{
    if (!CheckAndSetLaunchState(LaunchState_Launching, LaunchState_Launched)) {
        return FALSE;
    }

    AutoPtr<IIntent> intent = _intent;
    if (intent == NULL) {
        GetIntent((IIntent**)&intent);
    }

    mLaunchIntent = intent;

    GeckoApp_Launch* closure = new GeckoApp_Launch(this, intent);
    pthread_t thread;
    if (pthread_create(&thread, NULL, LaunchEntryRoutine, (void*)closure)) {
        assert(0);
        return FALSE;
    }

//    AutoPtr<LaunchRunnable> runnable = new LaunchRunnable(this, intent);
//    AutoPtr<IThread> launchThread;
//    ASSERT_SUCCEEDED(CThread::New(IRunnable::Probe(runnable.Get()), (IThread**)&launchThread));
//    launchThread->Start();
    return TRUE;
}

ECode GeckoApp::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PRINT_ENTER_LEAVE("GeckoApp::OnCreate()");

    sAppContext = this;
    ECode ec;
    /*ec = CHandler::New(&mMainHandler);
    if (FAILED(ec)) return ec;

    AutoPtr<LoopRunnable> pRunnable = new LoopRunnable(this);
    if (!pRunnable) return E_OUT_OF_MEMORY;

    if (!sTryCatchAttached) {
        sTryCatchAttached = true;
        ec = mMainHandler->Post(IRunnable::Probe(pRunnable));
        if (FAILED(ec)) return ec;
    }*/

    /*ISharedPreferences* pSettings;
    ec = GetPreferences(Activity_MODE_PRIVATE, &pSettings);
    if (FAILED(ec)) return ec;

    String packageName;
    GetPackageName(&packageName)
    packageName += ".locale";
    String localeCode;
    ec = pSettings->GetString(packageName, "", &localeCode);
    pSettings->Release();
    if (FAILED(ec)) return ec;
    if (!localeCode.IsNullOrEmpty()) {
        GeckoAppShell::SetSelectedLocale(localeCode);
    }*/

    Activity::OnCreate(savedInstanceState);

    if (!sGREDir) {
        String dataDir("/data/data/com.elastos.runtime/elastos/Launcher");
        AutoPtr<IApplicationInfo> pAppInfo;
        FAIL_RETURN(GetApplicationInfo((IApplicationInfo**)&pAppInfo));

        FAIL_RETURN(pAppInfo->GetDataDir(&dataDir));
        FAIL_RETURN(CFile::New(dataDir, (IFile**)&sGREDir));
    }

    AutoPtr<IWindow> window;
    ASSERT_SUCCEEDED(GetWindow((IWindow**)&window));
    ASSERT_SUCCEEDED(window->SetFlags(sFullscreen ?
            IWindowManagerLayoutParams::FLAG_FULLSCREEN : 0,
            IWindowManagerLayoutParams::FLAG_FULLSCREEN));

    if (sCameraView == NULL) {
        ASSERT_SUCCEEDED(CSurfaceView::New(this, (ISurfaceView**)&sCameraView));
        AutoPtr<ISurfaceHolder> holder;
        ASSERT_SUCCEEDED(sCameraView->GetHolder((ISurfaceHolder**)&holder));
        ASSERT_SUCCEEDED(holder->SetType(ISurfaceHolder::SURFACE_TYPE_PUSH_BUFFERS));
    }

    if (!sSurfaceView) {
        ASSERT_SUCCEEDED(CGeckoSurfaceView::NewByFriend(this, (CGeckoSurfaceView**)&sSurfaceView));
    }
    else {
        ASSERT_SUCCEEDED(sMainLayout->RemoveAllViews());
    }

    ASSERT_SUCCEEDED(CAbsoluteLayout::New(this, (IAbsoluteLayout**)&sMainLayout));
    AutoPtr<IFrameLayoutLayoutParams> fLayoutParams;
    ASSERT_SUCCEEDED(CAbsoluteLayoutLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            0, 0, (IAbsoluteLayoutLayoutParams**)&fLayoutParams));
    ASSERT_SUCCEEDED(sMainLayout->AddView(sSurfaceView, fLayoutParams));

    AutoPtr<IViewGroupLayoutParams> vgLayoutParams;
    ASSERT_SUCCEEDED(CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::FILL_PARENT,
            IViewGroupLayoutParams::FILL_PARENT,
            (IViewGroupLayoutParams**)&vgLayoutParams));
    SetContentView(sMainLayout, vgLayoutParams);

    ASSERT_SUCCEEDED((CIntentFilter::New((IIntentFilter**)&mConnectivityFilter)));
    mConnectivityFilter->AddAction(String(IConnectivityManager::CONNECTIVITY_ACTION));
//    ASSERT_SUCCEEDED(CGeckoConnectivityReceiver::New((IBroadcastReceiver**)&mConnectivityReceiver));
    // AutoPtr<IIntentFilter> batteryFilter;
    // CIntentFilter::New((IIntentFilter**)&batteryFilter);
    // batteryFilter->AddAction(String(IIntent::ACTION_BATTERY_CHANGED));
    // ASSERT_SUCCEEDED(CGeckoBatteryManager::New((IBroadcastReceiver**)&mBatteryReceiver));
    // //RegisterReceiver(mBatteryReceiver, batteryFilter);
    // if (SmsManager::GetInstance() != NULL) {
    //     SmsManager::GetInstance()->Start();
    // }
    //CGeckoNetworkManager::GetInstance()->Init();
    if (!GeckoApp::CheckAndSetLaunchState(LaunchState_PreLaunch, LaunchState_Launching)) return NOERROR;

    //CheckAndLaunchUpdate();

    AutoPtr<LibLoadRunnable> libLoadRunnable = new LibLoadRunnable(this);
    ASSERT_SUCCEEDED(CThread::New(IRunnable::Probe(libLoadRunnable), (IThread**)&mLibLoadThread));

    mLibLoadThread->Start();
    return NOERROR;
}

ECode GeckoApp::EnableCameraView()
{
    // Some phones (eg. nexus S) need at least a 8x16 preview size
    AutoPtr<IAbsoluteLayoutLayoutParams> params;
    CAbsoluteLayoutLayoutParams::New(8, 16, 0, 0, (IAbsoluteLayoutLayoutParams**)&params);
    return sMainLayout->AddView(sCameraView, params);
}

ECode GeckoApp::DisableCameraView()
{
    return sMainLayout->RemoveViewInLayout(sCameraView);
}

ECode GeckoApp::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    PRINT_ENTER_LEAVE("GeckoApp::OnNewIntent");

    ECode ec = NOERROR;
    if (CheckLaunchState(LaunchState_GeckoExiting)) {
        // We're exiting and shouldn't try to do anything else just incase
        // we're hung for some reason we'll force the process to exit
        exit(0);
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    if (action.Equals("org.mozilla.gecko.DEBUG") &&
        CheckAndSetLaunchState(LaunchState_Launching, LaunchState_WaitForDebugger)) {
//        CButton::New(this, (IButton**)&mLaunchButton);
//
//        AutoPtr<ICharSequence> text;
//        CStringWrapper::New(String("Launch"), (ICharSequence**)&text);
//
//        mLaunchButton->SetText(text); // don't need to localize
//
//        AutoPtr<IViewOnClickListener> listener = new ButtonClickListener(this);
//        assert(listener != NULL);
//        mLaunchButton->SetOnClickListener(listener);
//        sMainLayout->AddView(mLaunchButton, 300, 200);

//        mMainHandler.postDelayed(new Runnable() {
//            public void run() {
//                Log.i(LOG_FILE_NAME, "Launching from debug intent after 5s wait");
//                setLaunchState(LaunchState.Launching);
//                launch(null);
//            }
//        }, 1000 * 5 /* 5 seconds */);
//        Log.i(LOG_FILE_NAME, "Intent : ACTION_DEBUG - waiting 5s before launching");
        return NOERROR;
    }

    if (CheckLaunchState(LaunchState_WaitForDebugger)) return NOERROR;
    if (Launch(intent)) return NOERROR;

    if (action.Equals("android.intent.action.MAIN")) {//action.Equals(ACTION_MAIN)) {
        AutoPtr<GeckoEvent> pEvent = new GeckoEvent(String(""));
        if (!pEvent) return E_OUT_OF_MEMORY;
        GeckoAppShell::SendEventToGecko(pEvent);
    }
    else if (action.Equals("android.intent.action.VIEW")) {//action.Equals(Intent_ACTION_VIEW)) {
        String uri;
        FAIL_RETURN(intent->GetDataString(&uri));
        AutoPtr<GeckoEvent> pEvent = new GeckoEvent(uri);
        if (!pEvent) return E_OUT_OF_MEMORY;
        GeckoAppShell::SendEventToGecko(pEvent);
    }
    else if (action.Equals("org.mozilla.fennec.WEBAPP")) {
        String uri;
        FAIL_RETURN(intent->GetStringExtra(String("args"), &uri));
        AutoPtr<GeckoEvent> pEvent = new GeckoEvent(uri);
        if (!pEvent) return E_OUT_OF_MEMORY;
        GeckoAppShell::SendEventToGecko(pEvent);
    }
    else if (action.Equals("org.mozilla.fennec.BOOKMARK")) {
        String args;
        FAIL_RETURN(intent->GetStringExtra(String("args"), &args));
        AutoPtr<GeckoEvent> pEvent = new GeckoEvent(args);
        if (!pEvent) return E_OUT_OF_MEMORY;
        GeckoAppShell::SendEventToGecko(pEvent);
    }
    return NOERROR;
}

ECode GeckoApp::OnPause()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnPause");
    AutoPtr<GeckoEvent> pEvent = new GeckoEvent(GeckoEvent::ACTIVITY_PAUSING);
    if (!pEvent) return E_OUT_OF_MEMORY;
    GeckoAppShell::SendEventToGecko(pEvent);
    // The user is navigating away from this activity, but nothing
    // has come to the foreground yet; for Gecko, we may want to
    // stop repainting, for example.

    // Whatever we do here should be fast, because we're blocking
    // the next activity from showing up until we finish.

    // onPause will be followed by either onResume or onStop.
    Activity::OnPause();

//    UnregisterReceiver(mConnectivityReceiver);
//    GeckoNetworkManager.getInstance().stop();
    return NOERROR;
}

ECode GeckoApp::OnResume()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnResume");
    if (CheckLaunchState(LaunchState_GeckoRunning)) {
        GeckoAppShell::OnResume();
    }

    // After an onPause, the activity is back in the foreground.
    // Undo whatever we did in onPause.
    Activity::OnResume();

    // Just in case. Normally we start in onNewIntent
    if (CheckLaunchState(LaunchState_PreLaunch) ||
        CheckLaunchState(LaunchState_Launching)) {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        OnNewIntent(intent);
    }

//    RegisterReceiver(mConnectivityReceiver, mConnectivityFilter);
//    GeckoNetworkManager.getInstance().start();
    return NOERROR;
}

ECode GeckoApp::OnStop()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnStop");
    // We're about to be stopped, potentially in preparation for
    // being destroyed.  We're killable after this point -- as I
    // understand it, in extreme cases the process can be terminated
    // without going through onDestroy.
    //
    // We might also get an onRestart after this; not sure what
    // that would mean for Gecko if we were to kill it here.
    // Instead, what we should do here is save prefs, session,
    // etc., and generally mark the profile as 'clean', and then
    // dirty it again if we get an onResume.
    AutoPtr<GeckoEvent> pEvent = new GeckoEvent(GeckoEvent::ACTIVITY_STOPPING);
    if (!pEvent) return E_OUT_OF_MEMORY;
    GeckoAppShell::SendEventToGecko(pEvent);
    Activity::OnStop();
    GeckoAppShell::PutChildInBackground();
    return NOERROR;
}

ECode GeckoApp::OnRestart()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnRestart");
    GeckoAppShell::PutChildInForeground();
    return Activity::OnRestart();
}

ECode GeckoApp::OnStart()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnStart");
    AutoPtr<GeckoEvent> pEvent = new GeckoEvent(GeckoEvent::ACTIVITY_START);
    if (!pEvent) return E_OUT_OF_MEMORY;
    GeckoAppShell::SendEventToGecko(pEvent);
    return Activity::OnStart();
}

ECode GeckoApp::OnDestroy()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnDestroy");
    // Tell Gecko to shutting down; we'll end up calling System.exit()
    // in onXreExit.
    Boolean isFinishing;
    IsFinishing(&isFinishing);
    if (isFinishing) {
        AutoPtr<GeckoEvent> pEvent = new GeckoEvent(GeckoEvent::ACTIVITY_SHUTDOWN);
        if (!pEvent) return E_OUT_OF_MEMORY;
        GeckoAppShell::SendEventToGecko(pEvent);
    }

//    if (SmsManager.getInstance() != null) {
//        SmsManager.getInstance().stop();
//        if (isFinishing)
//            SmsManager.getInstance().shutdown();
//    }

    //GeckoNetworkManager.getInstance().stop();
    Activity::OnDestroy();
//    UnregisterReceiver(mBatteryReceiver);
}

ECode GeckoApp::OnConfigurationChanged(
    /* [in] */ IConfiguration* pNewConfig)
{
    PRINT_ENTER_LEAVE("GeckoApp::OnConfigurationChanged");
    // nothing, just ignore
    return Activity::OnConfigurationChanged(pNewConfig);
}

ECode GeckoApp::OnLowMemory()
{
    PRINT_ENTER_LEAVE("GeckoApp::OnLowMemory");
    if (CheckLaunchState(LaunchState_GeckoRunning)) {
        GeckoAppShell::OnLowMemory();
    }

    return Activity::OnLowMemory();
}

ECode GeckoApp::UnpackComponents()
{
    PRINT_ENTER_LEAVE("GeckoApp::UnpackComponents()");

    String resPath("/data/data/com.elastos.runtime/elastos/Launcher/Launcher.apk");
//    AutoPtr<IApplication> pIApplication;
//    FAIL_RETURN(GetApplication((IApplication**)&pIApplication));
//    FAIL_RETURN(pIApplication->GetPackageResourcePath(&resPath));
    AutoPtr<IFile> applicationPackage;
    FAIL_RETURN(CFile::New(resPath, (IFile**)&applicationPackage));

    AutoPtr<IFile> componentsDir;
    FAIL_RETURN(CFile::New(sGREDir, String("components"), (IFile**)&componentsDir));

    Int64 apLast, cdLast;
    applicationPackage->LastModified(&apLast);
    componentsDir->LastModified(&cdLast);
    if (apLast == cdLast) {
        return NOERROR;
    }

    Boolean succeeded;
    componentsDir->Mkdir(&succeeded);
    componentsDir->SetLastModified(apLast, &succeeded);

    GeckoAppShell::KillAnyZombies();

    AutoPtr<IZipFile> pZip;
    ECode ec = CZipFile::New(resPath, (IZipFile**)&pZip);
    if (FAILED(ec)) {
        PRINT_FILE_LINE_EX("GeckoApp error CZipFile::New %08x\n", ec);
        return ec;
    }

    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(32768);
    FAIL_RETURN(UnpackFile(pZip, buf, NULL, String("removed-files"), &succeeded));
    if (FAILED(ec)) {
        PRINT_FILE_LINE_EX("GeckoApp error UnpackFile removing files %08x\n", ec);
        return ec;
    }
    if (succeeded) {
        //TODO ec = RemoveFiles();
        if (FAILED(ec)) {
            // This file may not be there, so just log any errors and move on
            PRINT_FILE_LINE_EX("GeckoApp error removing files %08x\n", ec);
            return ec;
        }
    }

    // copy any .xpi file into an extensions/ directory
    AutoPtr<IObjectContainer> pContainer;
    AutoPtr<IObjectEnumerator> pEnum;
    FAIL_RETURN(pZip->GetEntries((IObjectContainer**)&pContainer));
    FAIL_RETURN(pContainer->GetObjectEnumerator((IObjectEnumerator**)&pEnum));

    String name;
    AutoPtr<IZipEntry> pEntry;
    ec = pEnum->MoveNext(&succeeded);
    while (SUCCEEDED(ec) && succeeded) {
        pEntry = NULL;
        FAIL_RETURN(pEnum->Current((IInterface**)&pEntry));

        pEntry->GetName(&name);
        if (name.StartWith("extensions/") && name.EndWith(".xpi")) {
            FAIL_RETURN(UnpackFile(pZip, buf, pEntry, name, &succeeded));
        }
        ec = pEnum->MoveNext(&succeeded);
    }

    return NOERROR;
}

ECode GeckoApp::RemoveFiles()
{
    PRINT_ENTER_LEAVE("GeckoApp::RemoveFiles()");
    ECode ec = NOERROR;

    AutoPtr<IFile> pIFile;
    AutoPtr<IFileReader> pIFileReader;
    AutoPtr<IBufferedReader> pReader;
    FAIL_RETURN(CFile::New(sGREDir, String("removed-files"), (IFile**)&pIFile));
    FAIL_RETURN(CFileReader::New(pIFile, (IFileReader**)&pIFileReader));
    FAIL_RETURN(CBufferedReader::New(pIFileReader, (IBufferedReader**)&pReader));

    String removedFileName;
    AutoPtr<IFile> pRemovedFile;
    Boolean exist, isDeleted;
    ec = pReader->ReadLine(&removedFileName);
    while(SUCCEEDED(ec) && !removedFileName.IsNullOrEmpty()) {
        pRemovedFile = NULL;
        ec = CFile::New(sGREDir, removedFileName, (IFile**)&pRemovedFile);
        FAIL_GOTO(ec, exit);
        pRemovedFile->Exists(&exist);
        if (exist) {
            pRemovedFile->Delete(&isDeleted);
        }
        ec = pReader->ReadLine(&removedFileName);
    }

exit:
    ICloseable::Probe(pReader)->Close();
    return ec;
}

ECode GeckoApp::UnpackFile(
    /* [in] */ IZipFile* pZip,
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ IZipEntry* pFileEntry,
    /* [in] */ const String& name,
    /* [out] */ Boolean* pResult)
{
    if (!pZip || !pResult) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = NOERROR;

    AutoPtr<IZipEntry> entry;
    if (!pFileEntry) {
        FAIL_RETURN(pZip->GetEntry(name, (IZipEntry**)&entry));
    }
    else {
        entry = pFileEntry;
    }

    if (entry == NULL) {
        String zipName;
        pZip->GetName(&zipName);
        PRINT_FILE_LINE_EX("Can't find [%s] in [%s]", name.string(), zipName.string());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IFile> pOutFile;
    FAIL_RETURN(CFile::New(sGREDir, name, (IFile**)&pOutFile));

    Boolean exist;
    pOutFile->Exists(&exist);
    Int64 modified, entryTime;
    pOutFile->LastModified(&modified);
    entry->GetTime(&entryTime);
    Int64 length, size;
    pOutFile->GetLength(&length);
    entry->GetSize(&size);
    if (exist && modified == entryTime && length == size) {
        *pResult = FALSE;
        return ec;
    }

    AutoPtr<IFile> pDir;
    FAIL_RETURN(pOutFile->GetParentFile((IFile**)&pDir));
    pDir->Exists(&exist);
    if (!exist) {
        Boolean succeeded;
        pDir->Mkdirs(&succeeded);
    }

    AutoPtr<IInputStream> pFileStream;
    FAIL_RETURN(pZip->GetInputStream(entry, (IInputStream**)&pFileStream));

    AutoPtr<IFileOutputStream> pOutStream;
    ec = CFileOutputStream::New(pOutFile, (IFileOutputStream**)&pOutStream);
    FAIL_GOTO(ec, exit);

    Int32 number, readCount;
    ec = pFileStream->Available(&number);
    while (SUCCEEDED(ec) && number > 0) {
        ec = pFileStream->ReadBytes(buf, 0, buf->GetLength(), &readCount);
        FAIL_GOTO(ec, exit);

        ec = pOutStream->WriteBytes(*buf, 0, readCount);
        FAIL_GOTO(ec, exit);

        ec = pFileStream->Available(&number);
    }

    Boolean result;
    pOutFile->SetLastModified(entryTime, &result);
    *pResult = TRUE;

exit:
    if (pFileStream) {
        pFileStream->Close();
    }
    if (pOutStream) {
        pOutStream->Close();
    }
    return ec;
}

ECode GeckoApp::AddEnvToIntent(
    /* [in] */ IIntent* pIntent)
{
    /*Map<String,String> envMap = System::Getenv();
    Set<Map.Entry<String,String>> envSet = envMap.entrySet();
    Iterator<Map.Entry<String,String>> envIter = envSet.iterator();
    StringBuffer envstr = new StringBuffer();
    Int32 c = 0;
    while (envIter.hasNext()) {
        Map.Entry<String,String> entry = envIter.next();
        String env("env");
        env += c;
        pIntent->PutExtra(env, entry.getKey() + "="
                        + entry.getValue());
        c++;
    }*/

    return E_NOT_IMPLEMENTED;
}

ECode GeckoApp::DoRestart()
{
    PRINT_ENTER_LEAVE("GeckoApp::DoRestart");
    //try {
        String action("org.mozilla.gecko.restart");
        AutoPtr<IIntent> pIIntent;
        FAIL_RETURN(CIntent::New(action, (IIntent**)&pIIntent));

        String packageName;
        ASSERT_SUCCEEDED(GetPackageName(&packageName));
        pIIntent->SetClassName(packageName, packageName + ".Restarter");

        FAIL_RETURN(AddEnvToIntent(pIIntent));

        pIIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                        IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
        GeckoAppShell::KillAnyZombies();
        FAIL_RETURN(StartActivity(pIIntent));
    //} catch (Exception e) {
    //    Log.i("GeckoAppJava", "error doing restart", e);
    //}
    Finish();
    // Give the restart process time to start before we die
    GeckoAppShell::WaitForAnotherGeckoProc();
    return NOERROR;
}

ECode GeckoApp::HandleNotification(
    /* [in] */ const String& action,
    /* [in] */ const String& alertName,
    /* [in] */ const String& alertCookie)
{
    return GeckoAppShell::HandleNotification(action, alertName, alertCookie);
}

ECode GeckoApp::CheckAndLaunchUpdate()
{
    assert(0);
    PRINT_ENTER_LEAVE("GeckoApp::CheckAndLaunchUpdate()");

    ECode ec;
    Int32 statusCode = 8; // UNEXPECTED_ERROR
    AutoPtr<IFile> baseUpdateDir;
    if (Build::VERSION::SDK_INT >= 8) {
        //ec = GetExternalFilesDir(
        //        Environment::DIRECTORY_DOWNLOADS, &baseUpdateDir);
    }
    else {
//        AutoPtr<IFile> pIFile = Environment::GetExternalStorageDirectory();
//        assert(pIFile);

        String path;
//        pIFile->GetPath(&path);
        ec = CFile::New(path, String("download"), (IFile**)&baseUpdateDir);
    }
    if (FAILED(ec)) return ec;

    AutoPtr<IFile> temp, updateDir;
    ASSERT_SUCCEEDED(CFile::New(baseUpdateDir, String("updates"), (IFile**)&temp));
    ASSERT_SUCCEEDED(CFile::New(temp, String("0"), (IFile**)&updateDir));

    AutoPtr<IFile> updateFile, statusFile;
    Boolean exist;
    String updateStatus;
    String status;
    String packageName;
    ASSERT_SUCCEEDED(CFile::New(updateDir, String("update.apk"), (IFile**)&updateFile));
    ASSERT_SUCCEEDED(CFile::New(updateDir, String("update.status"), (IFile**)&statusFile));

    statusFile->Exists(&exist);
    ReadUpdateStatus(statusFile, &updateStatus);
    if (!exist || !updateStatus.Equals("pending")) {
        return NOERROR;
    }

    updateFile->Exists(&exist);
    if (!exist) return NOERROR;

    // Launch APK
    GetPackageName(&packageName);
    AutoPtr<IFile> updateFileToRun;
    ASSERT_SUCCEEDED(CFile::New(updateDir, packageName + "-update.apk", (IFile**)&updateFileToRun));
    //try {
        Boolean succeeded;
        ASSERT_SUCCEEDED(updateFile->RenameTo(updateFileToRun, &succeeded));
        if (succeeded) {
            String path;
            updateFileToRun->GetPath(&path);
            String amCmd("/system/bin/am start -a android.intent.action.VIEW ");
            amCmd += "-n com.android.packageinstaller/.PackageInstallerActivity -d file://";
            amCmd += path;
            /*AutoPtr<IRuntime> pIRuntime;
            ec = CRuntime::GetRuntime((IRuntime**)&pIRuntime);
            if (FAILED(ec)) return ec;

            pIRuntime->Exec(amCmd);*/
            statusCode = 0; // OK
        }
        else {
            statusCode = 7; // WRITE_ERROR
        }
    //} catch (Exception e) {
    //    Log.i("GeckoAppJava", "error launching installer to update", e);
    //}

    // Update the status file
    if (statusCode == 0) {
        status = "succeeded\n";
    }
    else {
        status = "failed: ";
        status += + statusCode + "\n";
    }

    AutoPtr<IFileOutputStream> os;
    //try {
    {
        ArrayOf<Byte> buf((Byte*)status.string(), status.GetLength());

        ASSERT_SUCCEEDED(CFileOutputStream::New(statusFile, (IFileOutputStream**)&os));

        os->WriteBytes(buf, 0, buf.GetLength());
        os->Close();
    }
    //} catch (Exception e) {
    //    Log.i("GeckoAppJava", "error writing status file", e);
    //}

    if (statusCode == 0) {
    //    System::Exit(0);
    }

    return NOERROR;
}

ECode GeckoApp::ReadUpdateStatus(
    /* [in] */ IFile* pStatusFile,
    /* [out] */ String* pStatus)
{
    assert(0);
    //try {
    AutoPtr<IFileReader> fileReader;
    ECode ec = CFileReader::New(pStatusFile, (IFileReader**)&fileReader);
    if (FAILED(ec)) return ec;

    AutoPtr<IBufferedReader> reader;
    ec = CBufferedReader::New(fileReader, (IBufferedReader**)&reader);
    if (FAILED(ec)) return ec;

    ec = reader->ReadLine(pStatus);
    ICloseable::Probe(reader)->Close();
    //} catch (Exception e) {
    //    Log.i("GeckoAppJava", "error reading update status", e);
    //}
    return ec;
}

ECode GeckoApp::ShowFilePicker(
    /* [in] */ const String& aMimeType,
    /* [out] */ String* pFilePickerResult)
{
    assert(0);
    /*AutoPtr<IIntent> pIIntent;
    ECode ec = CIntent::New(Intent_ACTION_GET_CONTENT, (IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;

    pIIntent->AddCategory(String(Intent_CATEGORY_OPENABLE));
    pIIntent->SetType(aMimeType);
    AutoPtr<IIntent> pChooser;

    //ec = CIntent::CreateChooser(pIIntent, "choose a file", (IIntent**)&pChooser);
    //if (FAILED(ec)) return ec;

    ec = StartActivityForResult(pChooser, GeckoApp_FILE_PICKER_REQUEST);
    if (FAILED(ec)) return ec;*/

    //try {
        //mFilePickerResult.Take(pFilePickerResult);
    //} catch (InterruptedException e) {
    //    Log.i("GeckoApp", "showing file picker ",  e);
    //}

    return NOERROR;
}

ECode GeckoApp::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* pData)
{
    ECode ec = NOERROR;
    String filePickerResult("");
    if (pData && resultCode == IActivity::RESULT_OK) {
        //try {
        AutoPtr<IContentResolver> pCr;
        AutoPtr<IUri> pUri;
        AutoPtr<IFile> pIFile;
        AutoPtr<IFileOutputStream> pFos;
        AutoPtr<IInputStream> pIs;

        AutoPtr< ArrayOf<String> > queryColumns = ArrayOf<String>::Alloc(1);
//        (*queryColumns)[0] = OpenableColumns_DISPLAY_NAME;

        FAIL_RETURN(GetContentResolver((IContentResolver**)&pCr));

        FAIL_RETURN(pData->GetData((IUri**)&pUri));

        AutoPtr<ICursor> pCursor;
        FAIL_RETURN(pCr->Query(pUri, queryColumns,
                    String(""), NULL, String(""), (ICursor**)&pCursor));

        String name;
        if (pCursor) {
            //try {
                Boolean succeeded;
                pCursor->MoveToNext(&succeeded);
                if (succeeded) {
                    pCursor->GetString(0, &name);
                }
                pCursor->Close();
            //} finally {
            //    cursor.close();
            //}
        }

        String fileName("tmp_");
        String fileExt;
        Int32 period;
        if (name.IsNull() || (period = name.LastIndexOf('.')) == -1) {
            String mimeType;
            FAIL_RETURN(pCr->GetType(pUri, &mimeType));

            String extension;
            GeckoAppShell::GetExtensionFromMimeType(mimeType, &extension);
            fileExt = ".";
            fileExt += extension;
        } else {
            fileExt = name.Substring(period);
            fileName = name.Substring(0, period);
        }

        AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(4096);
        Int32 len;

        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        ec = helper->CreateTempFile(fileName, fileExt, sGREDir, (IFile**)&pIFile);
        FAIL_GOTO(ec, exit);

        ec = CFileOutputStream::New(pIFile, (IFileOutputStream**)&pFos);
        FAIL_GOTO(ec, exit);

        ec = pCr->OpenInputStream(pUri, (IInputStream**)&pIs);
        FAIL_GOTO(ec, exit);

        ec = pIs->ReadBytes(buf, &len);
        FAIL_GOTO(ec, exit);
        while (len != -1) {
            ec = pFos->WriteBytes(*buf, 0, len);
            FAIL_GOTO(ec, exit);
            ec = pIs->ReadBytes(buf, &len);
            FAIL_GOTO(ec, exit);
        }
        pIFile->GetAbsolutePath(&filePickerResult);
exit:
        if (pFos) {
            pFos->Close();
        }
        if (pIs) {
            pIs->Close();
        }
        //}catch (Exception e) {
        //    Log.e("GeckoApp", "showing file picker", e);
        //}
    }

//    try {
//        mFilePickerResult.Put(filePickerResult);
//    } catch (InterruptedException e) {
//        Log.i("GeckoApp", "error returning file picker result", e);
//    }
    return ec;
}

class ButtonClickListener
    : public ElRefBase
    , public IViewOnClickListener
{
public:
    ButtonClickListener(
        /* [in] */ GeckoApp* host)
        : mHost(host)
    {}

    UInt32 AddRef()
    {
        return ElRefBase::AddRef();
    }

    UInt32 Release()
    {
        return ElRefBase::Release();
    }

    PInterface Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (PInterface)(IViewOnClickListener*)this;
        }
        else if (riid == EIID_IViewOnClickListener) {
            return (IViewOnClickListener*)this;
        }

        return NULL;
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [in] */ InterfaceID* pIID)
    {
        return E_NOT_IMPLEMENTED;
    }

    ECode OnClick (
        /* [in] */ IView* v)
    {
        // hide the button so we can't be launched again
        mHost->sMainLayout->RemoveViewInLayout(mHost->mLaunchButton);
        mHost->SetLaunchState(GeckoApp::LaunchState_Launching);
        mHost->Launch(NULL);

        return NOERROR;
    }

private:
    AutoPtr<GeckoApp> mHost;
};


} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
