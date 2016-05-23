#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/Manifest.h>
#include "elastos/droid/server/CThemeService.h"
#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/content/pm/ThemeUtils.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/droid/os/FileUtils.h>
#include <elastos/droid/os/HandlerThread.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CWallpaperManager;
using Elastos::Droid::App::CWallpaperManagerHelper;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CNotificationManager;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::App::IWallpaperManagerHelper;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IBaseThemeInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IThemeInfo;
using Elastos::Droid::Content::Pm::IThemeUtils;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::CThemeConfigBuilder;
using Elastos::Droid::Content::Res::CThemeChangeRequestBuilder;
using Elastos::Droid::Content::Res::CThemeConfigHelper;
using Elastos::Droid::Content::Res::EIID_IIThemeService;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IThemeChangeRequestBuilder;
using Elastos::Droid::Content::Res::IThemeConfig;
using Elastos::Droid::Content::Res::IThemeConfigHelper;
using Elastos::Droid::Content::Res::RequestType;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::Utility::Cm::CCmImageUtils;
using Elastos::Droid::Internal::Utility::Cm::ICmImageUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::CThemesContractMixnMatchColumns;
using Elastos::Droid::Provider::CThemesContractThemesColumns;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::IThemesContractMixnMatchColumns;
using Elastos::Droid::Provider::IThemesContractThemesColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileHelper;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Zip::CZipFile;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
//-----------------------------------------------------------------------------------
//                          CThemeService::ThemeWorkerHandler
//-----------------------------------------------------------------------------------
const Int32 CThemeService::ThemeWorkerHandler::MESSAGE_CHANGE_THEME = 1;
const Int32 CThemeService::ThemeWorkerHandler::MESSAGE_APPLY_DEFAULT_THEME = 2;
const Int32 CThemeService::ThemeWorkerHandler::MESSAGE_REBUILD_RESOURCE_CACHE = 3;

CThemeService::ThemeWorkerHandler::ThemeWorkerHandler(
    /* [in] */ CThemeService* host,
    /* [in] */ ILooper* looper)
    : mHost(host)
{
    Handler::constructor(looper);
}

ECode CThemeService::ThemeWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_CHANGE_THEME:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                IThemeChangeRequest* request = IThemeChangeRequest::Probe(obj);
                Int32 arg1;
                msg->GetArg1(&arg1);
                mHost->DoApplyTheme(request, arg1 == 1);
                break;
            }

        case MESSAGE_APPLY_DEFAULT_THEME:
            {
                mHost->DoApplyDefaultTheme();
                break;
            }

        case MESSAGE_REBUILD_RESOURCE_CACHE:
            {
                mHost->DoRebuildResourceCache();
                break;
            }

        default:
            {
                Logger::W(TAG, "Unknown message %d", what);
                break;
            }
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService::ResourceProcessingHandler
//-----------------------------------------------------------------------------------
const Int32 CThemeService::ResourceProcessingHandler::MESSAGE_QUEUE_THEME_FOR_PROCESSING = 3;
const Int32 CThemeService::ResourceProcessingHandler::MESSAGE_DEQUEUE_AND_PROCESS_THEME = 4;

CThemeService::ResourceProcessingHandler::ResourceProcessingHandler(
    /* [in] */ CThemeService* host,
    /* [in] */ ILooper* lp)
    : mHost(host)
{
    Handler::constructor(lp);
}

ECode CThemeService::ResourceProcessingHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_QUEUE_THEME_FOR_PROCESSING:
            {
                AutoPtr<IInterface> pkgName;
                msg->GetObj((IInterface**)&pkgName);
                Boolean flag = FALSE;
                {    AutoLock syncLock(mThemesToProcessQueue);
                    if (mThemesToProcessQueue->Contains(pkgName, &flag), !flag) {
                        String str;
                        ICharSequence* cs = ICharSequence::Probe(pkgName);
                        cs->ToString(&str);
                        if (DEBUG) Logger::D(TAG, "Adding %s for processing", str.string());
                        mThemesToProcessQueue->Add(pkgName);
                        Int32 size;
                        if (mThemesToProcessQueue->GetSize(&size), size == 1) {
                            SendEmptyMessage(MESSAGE_DEQUEUE_AND_PROCESS_THEME, &flag);
                        }
                    }
                }
               break;
            }

        case MESSAGE_DEQUEUE_AND_PROCESS_THEME:
            {
                AutoPtr<IInterface> pkgName;
                {    AutoLock syncLock(mThemesToProcessQueue);
                    mThemesToProcessQueue->Get(0, (IInterface**)&pkgName);
                }
                if (pkgName != NULL) {
                    String str;
                    ICharSequence::Probe(pkgName)->ToString(&str);
                    if (DEBUG) Logger::D(TAG, "Processing %s", str.string());
                    String name;
                    // try {
                        ECode ec = NOERROR;
                        AutoPtr<IPackageInfo> pi;
                        ec = mPM->GetPackageInfo(str, 0, (IPackageInfo**)&pi);
                        if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
                            name = String(NULL);
                            return E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION;
                        }
                        name = mHost->GetThemeName(pi.Get());
                    // } catch (PackageManager.NameNotFoundException e) {
                        // name = null;
                    // }

                    Int32 result;
                    mPM->ProcessThemeResources(str, &result);
                    if (result < 0) {
                        mHost->PostFailedThemeInstallNotification(name.IsNull() ? name : str);
                    }
                    mHost->SendThemeResourcesCachedBroadcast(str, result);

                    {    AutoLock syncLock(mThemesToProcessQueue);
                        mThemesToProcessQueue->Remove(0);
                        Int32 size;
                        Boolean flag = FALSE;
                        if ((mThemesToProcessQueue->GetSize(&size), size > 0) &&
                           (HasMessages(MESSAGE_DEQUEUE_AND_PROCESS_THEME, &flag), !flag)) {
                            SendEmptyMessage(MESSAGE_DEQUEUE_AND_PROCESS_THEME, &flag);
                        }
                    }
                    mHost->PostFinishedProcessing(str);
                }
                break;
            }

        default:
            {
                Logger::W(TAG, "Unknown message %d", what);
                break;
            }
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService::SettingsObserver
//-----------------------------------------------------------------------------------
CThemeService::SettingsObserver::SettingsObserver(
    /* [in] */ CThemeService* host)
    : mHost(host)
{
    AutoPtr<ISettingsSystem> settings;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settings);
    settings->GetDEFAULT_ALARM_ALERT_URI((IUri**)&ALARM_ALERT_URI);
    settings->GetDEFAULT_NOTIFICATION_URI((IUri**)&NOTIFICATION_URI);
    settings->GetDEFAULT_RINGTONE_URI((IUri**)&RINGTONE_URI);
    ContentObserver::constructor(NULL);
}

ECode CThemeService::SettingsObserver::Register(
    /* [in] */ Boolean bregister)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (bregister) {
        cr->RegisterContentObserver(ALARM_ALERT_URI.Get(), FALSE, this);
        cr->RegisterContentObserver(NOTIFICATION_URI.Get(), FALSE, this);
        return cr->RegisterContentObserver(RINGTONE_URI.Get(), FALSE, this);
    }
    else {
        return cr->UnregisterContentObserver(this);
    }
}

ECode CThemeService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    ContentObserver::OnChange(selfChange, uri);
    Boolean changed = FALSE;
    AutoPtr<IThemeChangeRequestBuilder> builder;
    CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&builder);
    AutoPtr<IThemeChangeRequestBuilder> tmp;
    if (Object::Equals(ALARM_ALERT_URI, uri)) {
        // In case the mixnmatch table has a mods_alarms entry, we'll clear it
        builder->SetAlarm(String(""), (IThemeChangeRequestBuilder**)&tmp);
        builder = NULL;
        builder = tmp;
        changed = TRUE;
    }
    if (Object::Equals(NOTIFICATION_URI, uri)) {
        // In case the mixnmatch table has a mods_notifications entry, we'll clear it
        tmp = NULL;
        builder->SetNotification(String(""), (IThemeChangeRequestBuilder**)&tmp);
        builder = NULL;
        builder = tmp;
        changed = TRUE;
    }
    if (Object::Equals(RINGTONE_URI, uri)) {
        // In case the mixnmatch table has a mods_ringtones entry, we'll clear it
        tmp = NULL;
        builder->SetRingtone(String(""), (IThemeChangeRequestBuilder**)&tmp);
        builder = NULL;
        builder = tmp;
        changed = TRUE;
    }

    if (changed) {
        AutoPtr<IThemeChangeRequest> tcr;
        builder->Build((IThemeChangeRequest**)&tcr);
        AutoPtr<ISystem> isystem;
        CSystem::AcquireSingleton((ISystem**)&isystem);
        Int64 millis;
        isystem->GetCurrentTimeMillis(&millis);
        mHost->UpdateProvider(tcr.Get(), millis);
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService::MyWallpaperChangeReceiver
//-----------------------------------------------------------------------------------
CThemeService::MyWallpaperChangeReceiver::MyWallpaperChangeReceiver(
    /* [in] */ CThemeService* host)
    : mHost(host)
{}

ECode CThemeService::MyWallpaperChangeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (!mWallpaperChangedByUs) {
        // In case the mixnmatch table has a mods_launcher entry, we'll clear it
        AutoPtr<IThemeChangeRequestBuilder> builder;
        CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&builder);
        AutoPtr<IThemeChangeRequestBuilder> tmp;
        builder->SetWallpaper(String(""), (IThemeChangeRequestBuilder**)&tmp);
        builder = NULL;
        builder = tmp;
        AutoPtr<IThemeChangeRequest> request;
        builder->Build((IThemeChangeRequest**)&request);
        AutoPtr<ISystem> isystem;
        CSystem::AcquireSingleton((ISystem**)&isystem);
        Int64 millis;
        isystem->GetCurrentTimeMillis(&millis);
        mHost->UpdateProvider(request, millis);
    }
    else {
        mWallpaperChangedByUs = FALSE;
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService::MyUserChangeReceiver
//-----------------------------------------------------------------------------------
CThemeService::MyUserChangeReceiver::MyUserChangeReceiver(
    /* [in] */ CThemeService* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CThemeService::MyUserChangeReceiver, BroadcastReceiver, IBroadcastReceiver)

ECode CThemeService::MyUserChangeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 userHandle;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle);
    if (userHandle >= 0) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IThemeConfigHelper> tch;
        CThemeConfigHelper::AcquireSingleton((IThemeConfigHelper**)&tch);
        AutoPtr<IThemeConfig> config;
        tch->GetBootThemeForUser(cr.Get(), userHandle, (IThemeConfig**)&config);
        if (DEBUG) {
            String str = Object::ToString(config.Get());
            Logger::D(TAG, "Changing theme for user %d to %s", userHandle, str.string());
        }
        AutoPtr<IThemeChangeRequestBuilder> tcrb;
        CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&tcrb);
        AutoPtr<IThemeChangeRequest> request;
        tcrb->Build((IThemeChangeRequest**)&request);
        // try {
            ECode ec = mHost->RequestThemeChange(request, TRUE);
        // } catch (RemoteException e) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::E(TAG, "Unable to change theme for user change");
            }
        // }
    }
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService::MyComparator
//-----------------------------------------------------------------------------------
CThemeService::MyComparator::MyComparator()
{}

CAR_INTERFACE_IMPL(CThemeService::MyComparator, Object, IComparator)

ECode CThemeService::MyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    IFile* flhs = IFile::Probe(lhs);
    Int64 llastModified;
    flhs->GetLastModified(&llastModified);
    IFile* rlhs = IFile::Probe(lhs);
    Int64 rlastModified;
    rlhs->GetLastModified(&rlastModified);
    *result = (Int32)(llastModified - rlastModified);
    return NOERROR;
}

//-----------------------------------------------------------------------------------
//                          CThemeService
//-----------------------------------------------------------------------------------
const String CThemeService::TAG("CThemeService");

const Boolean CThemeService::DEBUG = false;

const String CThemeService::GOOGLE_SETUPWIZARD_PACKAGE("com.google.android.setupwizard");
const String CThemeService::CM_SETUPWIZARD_PACKAGE("com.cyanogenmod.setupwizard");

const Int64 CThemeService::MAX_ICON_CACHE_SIZE = 33554432; // 32MB
const Int64 CThemeService::PURGED_ICON_CACHE_SIZE = 25165824; // 24 MB

// Defines a min and max compatible api level for themes on this system.
const Int32 CThemeService::MIN_COMPATIBLE_VERSION = 21;

AutoPtr<IContext> CThemeService::mContext;
AutoPtr<IPackageManager> CThemeService::mPM;
Boolean CThemeService::mWallpaperChangedByUs = FALSE;
AutoPtr<IArrayList> CThemeService::mThemesToProcessQueue;

CThemeService::CThemeService()
    : mIconCacheSize(0)
    , mIsThemeApplying(FALSE)
{}

CThemeService::~CThemeService()
{}

CAR_INTERFACE_IMPL_2(CThemeService, Object, IIThemeService, IBinder)

CAR_OBJECT_IMPL(CThemeService)

ECode CThemeService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mWorker = new HandlerThread(String("ThemeServiceWorker"), IProcess::THREAD_PRIORITY_BACKGROUND);
    IThread::Probe(mWorker)->Start();
    AutoPtr<ILooper> looper;
    mWorker->GetLooper((ILooper**)&looper);
    mHandler = new ThemeWorkerHandler(this, looper);
    Logger::I(TAG, "Spawned worker thread");

    AutoPtr<IHandlerThread> processingThread = new HandlerThread(String("ResourceProcessingThread"),
            IProcess::THREAD_PRIORITY_BACKGROUND);
    IThread::Probe(processingThread)->Start();
    looper = NULL;
    processingThread->GetLooper((ILooper**)&looper);
    mResourceProcessingHandler = new ResourceProcessingHandler(this, looper);

    // create the theme directory if it does not exist
    ThemeUtils::CreateThemeDirIfNotExists();
    ThemeUtils::CreateFontDirIfNotExists();
    ThemeUtils::CreateAlarmDirIfNotExists();
    ThemeUtils::CreateNotificationDirIfNotExists();
    ThemeUtils::CreateRingtoneDirIfNotExists();
    ThemeUtils::CreateIconCacheDirIfNotExists();
    mSettingsObserver = new SettingsObserver(this);
    mWallpaperChangeReceiver = new MyWallpaperChangeReceiver(this);
    mUserChangeReceiver = new MyUserChangeReceiver(this);
    mOldestFilesFirstComparator = new MyComparator();
    return NOERROR;
}

void CThemeService::SystemRunning()
{
    // listen for wallpaper changes
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_WALLPAPER_CHANGED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(((IBroadcastReceiver*)mWallpaperChangeReceiver.Get()), filter.Get(), (IIntent**)&intent);

    filter = NULL;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    intent = NULL;
    mContext->RegisterReceiver(((IBroadcastReceiver*)mUserChangeReceiver.Get()), filter, (IIntent**)&intent);

    // listen for alarm/notifications/ringtone changes
    mSettingsObserver->Register(TRUE);

    mPM = NULL;
    mContext->GetPackageManager((IPackageManager**)&mPM);

    ProcessInstalledThemes();

    if (!IsThemeApiUpToDate()) {
        Logger::D(TAG, "The system has been upgraded to a theme new api, checking if currently set theme is compatible");
        RemoveObsoleteThemeOverlayIfExists();
        UpdateThemeApi();
    }
}

void CThemeService::RemoveObsoleteThemeOverlayIfExists()
{
    // Get the current overlay theme so we can see it it's overlay should be unapplied
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IThemeConfig> config;
    ECode ec = NOERROR;
    // try {
        if (am != NULL) {
            AutoPtr<IConfiguration> tmpconfig;
            ec = am->GetConfiguration((IConfiguration**)&tmpconfig);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::E(TAG, "Failed to get the theme config ");
            }
            ec = tmpconfig->GetThemeConfig((IThemeConfig**)&config);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::E(TAG, "Failed to get the theme config ");
            }
        }
        else {
            Logger::E(TAG, "ActivityManager getDefault() returned null, cannot remove obsolete theme");
        }
    // } catch(RemoteException e) {
        // Logger::E(TAG, "Failed to get the theme config ", e);
    // }
    if (config == NULL) return; // No need to unapply a theme if one isn't set

    // Populate the currentTheme map for the components we care about, we'll look
    // at the compatibility of each pkg below.
    AutoPtr<HashMap<String, String> > currentThemeMap = new HashMap<String, String>();
    String overlay;
    config->GetOverlayForStatusBar(&overlay);
    (*currentThemeMap)[IThemesContractThemesColumns::MODIFIES_STATUS_BAR] = overlay;
    String navbar;
    config->GetOverlayForNavBar(&navbar);
    (*currentThemeMap)[IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR] = navbar;
    String opgname;
    config->GetOverlayPkgName(&opgname);
    (*currentThemeMap)[IThemesContractThemesColumns::MODIFIES_OVERLAYS] = opgname;

    // Look at each component's theme (that we care about at least) and check compatibility
    // of the pkg with the system. If it is not compatible then we will add it to a theme
    // change request.
    AutoPtr<HashMap<String, String> > defaults = ThemeUtils::GetDefaultComponents(mContext.Get());

    AutoPtr<IThemeChangeRequestBuilder> builder;
    CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&builder);
    HashMap<String, String>::Iterator it = currentThemeMap->Begin();
    for (; it != currentThemeMap->End(); ++it) {
        String component = it->mFirst;
        String pkgName = it->mSecond;
        String defaultPkg = (*defaults)[component];

        // Check that the default overlay theme is not currently set
        if (defaultPkg.Equals(pkgName)) {
            Logger::D(TAG, "Current overlay theme is same as default. Not doing anything for %s", component.string());
            continue;
        }

        // No need to unapply a system theme since it is always compatible
        if (IThemeConfig::SYSTEM_DEFAULT.Equals(pkgName)) {
            Logger::D(TAG, "Current overlay theme for %s was system. no need to unapply", component.string());
            continue;
        }

        if (!IsThemeCompatibleWithUpgradedApi(pkgName)) {
            Logger::D(TAG, "%s is incompatible with latest theme api for component %s + %s ", pkgName.string(), component.string(), defaultPkg.string());
            AutoPtr<IThemeChangeRequestBuilder> tcr;
            builder->SetComponent(component, pkgName, (IThemeChangeRequestBuilder**)&tcr);
        }
    }
    // Now actually unapply the incompatible themes
    AutoPtr<IThemeChangeRequest> request;
    builder->Build((IThemeChangeRequest**)&request);
    AutoPtr<IMap> components;
    request->GetThemeComponentsMap((IMap**)&components);
    Boolean flag = FALSE;
    components->IsEmpty(&flag);
    if (components->IsEmpty(&flag), !flag) {
        // try {
            RequestThemeChange(request, TRUE);
        // } catch(RemoteException e) {
            // This cannot happen
        // }
    }
    else {
        Logger::D(TAG, "Current theme is compatible with the system. Not unapplying anything");
    }
}

Boolean CThemeService::IsThemeCompatibleWithUpgradedApi(
    /* [in] */ const String& pkgName)
{
    // Note this function does not cover the case of a downgrade. That case is out of scope and
    // would require predicting whether the future API levels will be compatible or not.
    Boolean compatible = FALSE;
    // try {
        AutoPtr<IPackageInfo> pi;
        ECode ec = mPM->GetPackageInfo(pkgName, 0, (IPackageInfo**)&pi);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "Unable to get package info for %s", pkgName.string());
        }
        AutoPtr<IApplicationInfo> info;
        pi->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 sdkVersion;
        info->GetTargetSdkVersion(&sdkVersion);
        Logger::D(TAG, "Comparing theme target: %d to %d", sdkVersion, Build::VERSION::SDK_INT);
        compatible = sdkVersion >= MIN_COMPATIBLE_VERSION;
    // } catch (NameNotFoundException e) {
        // Log.e(TAG, "Unable to get package info for " + pkgName, e);
    // }
    return compatible;
}

Boolean CThemeService::IsThemeApiUpToDate()
{
    // We can't be 100% sure its an upgrade. If the field is undefined it
    // could have been a factory reset.
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    // final ContentResolver resolver = mContext.getContentResolver();
    Int32 recordedApiLevel = Build::VERSION::SDK_INT;
    // try {
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        ECode ec = settingsSecure->GetInt32(resolver, ISettingsSecure::THEME_PREV_BOOT_API_LEVEL, &recordedApiLevel);
    // } catch (SettingNotFoundException e) {
        if (ec == (ECode)E_SETTING_NOT_FOUND_EXCEPTION) {
            recordedApiLevel = -1;
            Logger::D(TAG, "Previous api level not found. First time booting?");
        }
    // }
    Logger::D(TAG, "Prev api level was: %d, api is now: %d", recordedApiLevel, Build::VERSION::SDK_INT);

    return recordedApiLevel == Build::VERSION::SDK_INT;
}

void CThemeService::UpdateThemeApi()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Boolean success = FALSE;
    settingsSecure->PutInt32(resolver,
            ISettingsSecure::THEME_PREV_BOOT_API_LEVEL, Build::VERSION::SDK_INT, &success);
    if (!success) {
        Logger::E(TAG, "Unable to store latest API level to secure settings");
    }
}

void CThemeService::DoApplyTheme(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Boolean removePerAppTheme)
{
    {    AutoLock syncLock(this);
        mProgress = 0;
    }

    Int32 num;
    if (request == NULL || (request->GetNumChangesRequested(&num), num == 0)) {
        PostFinish(TRUE, request, 0);
        return;
    }
    mIsThemeApplying = TRUE;
    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    Int64 updateTime;
    isystem->GetCurrentTimeMillis(&updateTime);

    // Stop listening for settings changes while applying theme
    mSettingsObserver->Register(FALSE);

    IncrementProgress(5);

    // TODO: provide progress updates that reflect the time needed for each component
    Int32 progressIncrement = 75 / num;

    String pkgName;
    if (request->GetIconsThemePackageName(&pkgName), !pkgName.IsNull()) {
        String str;
        request->GetIconsThemePackageName(&str);
        UpdateIcons(str);
        IncrementProgress(progressIncrement);
    }

    if (request->GetWallpaperThemePackageName(&pkgName), !pkgName.IsNull()) {
        if (UpdateWallpaper(pkgName)) {
            mWallpaperChangedByUs = TRUE;
        }
        IncrementProgress(progressIncrement);
    }

    if (request->GetLockWallpaperThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateLockscreen(pkgName);
        IncrementProgress(progressIncrement);
    }

    Environment::SetUserRequired(FALSE);
    if (request->GetNotificationThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateNotifications(pkgName);
        IncrementProgress(progressIncrement);
    }

    if (request->GetAlarmThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateAlarms(pkgName);
        IncrementProgress(progressIncrement);
    }

    if (request->GetRingtoneThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateRingtones(pkgName);
        IncrementProgress(progressIncrement);
    }
    Environment::SetUserRequired(TRUE);

    if (request->GetBootanimationThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateBootAnim(pkgName);
        IncrementProgress(progressIncrement);
    }

    if (request->GetFontThemePackageName(&pkgName), !pkgName.IsNull()) {
        UpdateFonts(pkgName);
        IncrementProgress(progressIncrement);
    }

    // try {
        UpdateProvider(request, updateTime);
    // } catch(IllegalArgumentException e) {
        // Safeguard against provider not being ready yet.
        // Log.e(TAG, "Not updating the theme provider since it is unavailable");
    // }

    if (ShouldUpdateConfiguration(request)) {
        UpdateConfiguration(request, removePerAppTheme);
    }

    KillLaunchers(request);

    PostFinish(TRUE, request, updateTime);
    mIsThemeApplying = FALSE;

    // Start listening for settings changes while applying theme
    mSettingsObserver->Register(TRUE);
}

void CThemeService::DoApplyDefaultTheme()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    String defaultThemePkg;
    settingsSecure->GetString(resolver, ISettingsSecure::DEFAULT_THEME_PACKAGE, &defaultThemePkg);
    if (!TextUtils::IsEmpty(StringUtils::ParseCharSequence(defaultThemePkg))) {
        String defaultThemeComponents;
        settingsSecure->GetString(resolver, ISettingsSecure::DEFAULT_THEME_COMPONENTS, &defaultThemeComponents);
        AutoPtr<List<String> > components;
        if (TextUtils::IsEmpty(StringUtils::ParseCharSequence(defaultThemeComponents))) {
            components = ThemeUtils::GetAllComponents();
        }
        else {
            AutoPtr<ArrayOf<String> > strings;
            StringUtils::Split(defaultThemeComponents, String("\\|"), (ArrayOf<String>**)&strings);
            for (Int32 i = 0; i < strings->GetLength(); ++i) {
                components->PushBack((*strings)[i]);
            }
            // components = new ArrayList<String>(
            //         Arrays.asList(defaultThemeComponents.split("\\|")));
        }
        AutoPtr<IThemeChangeRequestBuilder> builder;
        CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&builder);
               // ThemeChangeRequest.Builder builder = new ThemeChangeRequest.Builder();
        List<String>::Iterator it = components->Begin();
        for (; it != components->End(); ++it) {
            AutoPtr<IThemeChangeRequestBuilder> builder;
            builder->SetComponent(*it, defaultThemePkg, (IThemeChangeRequestBuilder**)&builder);
        }
        // try {
            AutoPtr<IThemeChangeRequest> request;
            builder->Build((IThemeChangeRequest**)&request);
            ECode ec = RequestThemeChange(request, TRUE);
        // } catch (RemoteException e) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::W(TAG, "Unable to set default theme");
            }
        // }
    }
}

void CThemeService::DoRebuildResourceCache()
{
    AutoPtr<IFile> file;
    CFile::New(IThemeUtils::RESOURCE_CACHE_DIR, (IFile**)&file);
    FileUtils::DeleteContents(file);
    ProcessInstalledThemes();
}

void CThemeService::UpdateProvider(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Int64 updateTime)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IThemesContractMixnMatchColumns::COL_UPDATE_TIME, updateTime);
    AutoPtr<IMap> componentMap;
    request->GetThemeComponentsMap((IMap**)&componentMap);
    AutoPtr<ISet> keySet;
    componentMap->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IInterface> obj;
        componentMap->Get((*array)[i], (IInterface**)&obj);
        String str;
        ICharSequence::Probe(obj)->ToString(&str);
        values->Put(IThemesContractMixnMatchColumns::COL_VALUE, str);
        String where = IThemesContractMixnMatchColumns::COL_KEY + "=?";
        AutoPtr<IThemesContractMixnMatchColumns> columns;
        CThemesContractMixnMatchColumns::AcquireSingleton((IThemesContractMixnMatchColumns**)&columns);
        ICharSequence::Probe((*array)[i])->ToString(&str);
        String mixNMatchKey;
        columns->ComponentToMixNMatchKey(str, &mixNMatchKey);
        AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = mixNMatchKey;
        if (mixNMatchKey.IsNull()) {
            continue; // No equivalence between mixnmatch and theme
        }
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IUri> uri;
        columns->GetCONTENT_URI((IUri**)&uri);
        Int32 vol;
        resolver->Update(uri, values, where, selectionArgs, &vol);
    }
}

Boolean CThemeService::UpdateIcons(
    /* [in] */ const String& pkgName)
{
    // try {
        ECode ec = NOERROR;
        if (pkgName.Equals(IThemeConfig::SYSTEM_DEFAULT)) {
            ec = mPM->UpdateIconMaps(String(NULL));
            if (FAILED(ec)) {
                Logger::W(TAG, "Changing icons failed");
                return FALSE;
            }
        }
        else {
            ec = mPM->UpdateIconMaps(pkgName);
            if (FAILED(ec)) {
                Logger::W(TAG, "Changing icons failed");
                return FALSE;
            }
        }
    // } catch (Exception e) {
        // Logger::W(TAG, "Changing icons failed");
        // return false;
    // }
    return TRUE;
}

Boolean CThemeService::UpdateFonts(
    /* [in] */ const String& pkgName)
{
    //Clear the font dir
    AutoPtr<IFile> file;
    CFile::New(IThemeUtils::SYSTEM_THEME_FONT_PATH, (IFile**)&file);
    FileUtils::DeleteContents(file);

    if (!pkgName.Equals(IThemeConfig::SYSTEM_DEFAULT)) {
        //Get Font Assets
        AutoPtr<IContext> themeCtx;
        AutoPtr<ArrayOf<String> > assetList;
        // try {
            ECode ec = NOERROR;
            ec = mContext->CreatePackageContext(pkgName, IContext::CONTEXT_IGNORE_SECURITY, (IContext**)&themeCtx);
            if (FAILED(ec)) {
                Logger::E(TAG, "There was an error getting assets  for pkg %s", pkgName.string());
                return FALSE;
            }
            AutoPtr<IAssetManager> assetManager;
            ec = themeCtx->GetAssets((IAssetManager**)&assetManager);
            if (FAILED(ec)) {
                Logger::E(TAG, "There was an error getting assets  for pkg %s", pkgName.string());
                return FALSE;
            }
            ec = assetManager->List(String("fonts"), (ArrayOf<String>**)&assetList);
            if (FAILED(ec)) {
                Logger::E(TAG, "There was an error getting assets  for pkg %s", pkgName.string());
                return FALSE;
            }
        // } catch (Exception e) {
            // Log.e(TAG, "There was an error getting assets  for pkg " + pkgName, e);
            // return false;
        // }
        if (assetList == NULL || assetList->GetLength() == 0) {
            Logger::E(TAG, "Could not find any font assets");
            return FALSE;
        }

        //Copy font assets to font dir
        for (Int32 i = 0; i < assetList->GetLength(); ++i) {
            AutoPtr<IInputStream> is;
            AutoPtr<IOutputStream> os;
            // try {
                ThemeUtils::GetInputStreamFromAsset(themeCtx,
                        String("file:///android_asset/fonts/") + (*assetList)[i], (IInputStream**)&is);
                AutoPtr<IFile> outFile;
                CFile::New(IThemeUtils::SYSTEM_THEME_FONT_PATH, (*assetList)[i], (IFile**)&outFile);
                ec = FileUtils::CopyToFile(is, outFile);
                if (FAILED(ec)) {
                    Logger::E(TAG, "There was an error installing the new fonts for pkg %s", pkgName.string());
                    return FALSE;
                }
                ec = FileUtils::SetPermissions(outFile,
                        FileUtils::sS_IRWXU|FileUtils::sS_IRGRP|FileUtils::sS_IRWXO, -1, -1);
                if (FAILED(ec)) {
                    Logger::E(TAG, "There was an error installing the new fonts for pkg %s", pkgName.string());
                    return FALSE;
                }
            // } catch (Exception e) {
                // Log.e(TAG, "There was an error installing the new fonts for pkg " + pkgName, e);
                // return false;
            // } finally {
                ThemeUtils::CloseQuietly(is);
                ThemeUtils::CloseQuietly(os);
            // }
        }
    }

    //Notify zygote that themes need a refresh
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    systemProperties->Set(String("sys.refresh_theme"), String("1"));
    return TRUE;
}

Boolean CThemeService::UpdateBootAnim(
    /* [in] */ const String& pkgName)
{
    if (IThemeConfig::SYSTEM_DEFAULT.Equals(pkgName)) {
        ClearBootAnimation();
        return TRUE;
    }

    // try {
        AutoPtr<IApplicationInfo> ai;
        ECode ec = mPM->GetApplicationInfo(pkgName, 0, (IApplicationInfo**)&ai);
        if (FAILED(ec)) {
            Logger::W(TAG, "Changing boot animation failed");
            return FALSE;
        }
        String str;
        ai->GetSourceDir(&str);
        ApplyBootAnimation(str);
    // } catch (PackageManager.NameNotFoundException e) {
        // Log.w(TAG, "Changing boot animation failed", e);
        // return false;
    // }
    return TRUE;
}

Boolean CThemeService::UpdateAlarms(
    /* [in] */ const String& pkgName)
{
    return UpdateAudible(IThemeUtils::SYSTEM_THEME_ALARM_PATH, String("alarms"),
            IRingtoneManager::TYPE_ALARM, pkgName);
}

Boolean CThemeService::UpdateNotifications(
    /* [in] */ const String& pkgName)
{
    return UpdateAudible(IThemeUtils::SYSTEM_THEME_NOTIFICATION_PATH, String("notifications"),
            IRingtoneManager::TYPE_NOTIFICATION, pkgName);
}

Boolean CThemeService::UpdateRingtones(
    /* [in] */ const String& pkgName)
{
    return UpdateAudible(IThemeUtils::SYSTEM_THEME_RINGTONE_PATH, String("ringtones"),
            IRingtoneManager::TYPE_RINGTONE, pkgName);
}

Boolean CThemeService::UpdateAudible(
    /* [in] */ const String& dirPath,
    /* [in] */ const String& assetPath,
    /* [in] */ Int32 type,
    /* [in] */ const String& pkgName)
{
    //Clear the dir
    ThemeUtils::ClearAudibles(mContext, dirPath);
    if (pkgName.Equals(IThemeConfig::SYSTEM_DEFAULT)) {
        if (!ThemeUtils::SetDefaultAudible(mContext, type)) {
            Logger::E(TAG, "There was an error installing the default audio file");
            return FALSE;
        }
        return true;
    }

    AutoPtr<IPackageInfo> pi;
    ECode ec = NOERROR;
    // try {
        ec = mPM->GetPackageInfo(pkgName, 0, (IPackageInfo**)&pi);
        if (FAILED(ec)) {
            Logger::E(TAG, "Unable to update audible %s", dirPath.string());
            return FALSE;
        }
    // } catch (PackageManager.NameNotFoundException e) {
        // Log.e(TAG, "Unable to update audible " + dirPath, e);
        // return FALSE;
    // }

    //Get theme Assets
    AutoPtr<IContext> themeCtx;
    AutoPtr<ArrayOf<String> > assetList;
    // try {
        ec = mContext->CreatePackageContext(pkgName, IContext::CONTEXT_IGNORE_SECURITY, (IContext**)&themeCtx);
        if (FAILED(ec)) {
            Logger::E(TAG, "There was an error getting assets for pkg %s", pkgName.string());
            return FALSE;
        }
        AutoPtr<IAssetManager> assetManager;
        ec = themeCtx->GetAssets((IAssetManager**)&assetManager);
        if (FAILED(ec)) {
            Logger::E(TAG, "There was an error getting assets for pkg %s", pkgName.string());
            return FALSE;
        }
        ec = assetManager->List(assetPath, (ArrayOf<String>**)&assetList);
        if (FAILED(ec)) {
            Logger::E(TAG, "There was an error getting assets for pkg %s", pkgName.string());
            return FALSE;
        }
    // } catch (Exception e) {
        // Log.e(TAG, "There was an error getting assets for pkg " + pkgName, e);
        // return false;
    // }
    if (assetList == NULL || assetList->GetLength() == 0) {
        Logger::E(TAG, "Could not find any audio assets");
        return FALSE;
    }

    // TODO: right now we just load the first file but this will need to be changed
    // in the future if multiple audio files are supported.
    String asset = (*assetList)[0];
    if (!ThemeUtils::IsValidAudible(asset)) return FALSE;

    AutoPtr<IInputStream> is;
    AutoPtr<IOutputStream> os;
    // try {
        AutoPtr<IFileHelper> fh;
        CFileHelper::AcquireSingleton((IFileHelper**)&fh);
        String separator;
        fh->GetSeparator(&separator);
        ThemeUtils::GetInputStreamFromAsset(themeCtx, String("file:///android_asset/")
                + assetPath + separator + asset, (IInputStream**)&is);
        AutoPtr<IFile> outFile;
        CFile::New(dirPath, asset, (IFile**)&outFile);
        FileUtils::CopyToFile(is, outFile);
        FileUtils::SetPermissions(outFile,
                FileUtils::sS_IRWXU|FileUtils::sS_IRGRP|FileUtils::sS_IRWXO, -1, -1);
        AutoPtr<IThemeInfo> themeInfo;
        pi->GetThemeInfo((IThemeInfo**)&themeInfo);
        String name;
        IBaseThemeInfo::Probe(themeInfo)->GetName(&name);
        ThemeUtils::SetAudible(mContext, outFile, type, name);
    // } catch (Exception e) {
        // Log.e(TAG, "There was an error installing the new audio file for pkg " + pkgName, e);
        // return false;
    // } finally {
        ThemeUtils::CloseQuietly(is);
        ThemeUtils::CloseQuietly(os);
    // }
    return TRUE;
}

Boolean CThemeService::UpdateLockscreen(
    /* [in] */ const String& pkgName)
{
    Boolean success;
    success = SetCustomLockScreenWallpaper(pkgName);

    if (success) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_KEYGUARD_WALLPAPER_CHANGED, (IIntent**)&intent);
        mContext->SendBroadcastAsUser(intent,
                UserHandle::ALL);
    }
    return success;
}

Boolean CThemeService::SetCustomLockScreenWallpaper(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IWallpaperManagerHelper> wmh;
    CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&wmh);
    AutoPtr<IWallpaperManager> wm;
    wmh->GetInstance(mContext, (IWallpaperManager**)&wm);
    ECode ec = NOERROR;
    // try {
        if (IThemeConfig::SYSTEM_DEFAULT.Equals(pkgName) || TextUtils::IsEmpty(StringUtils::ParseCharSequence(pkgName))) {
            ec = wm->ClearKeyguardWallpaper();
            if (FAILED(ec)) {
                Logger::E(TAG, "There was an error setting lockscreen wp for pkg %s", pkgName.string());
                return FALSE;
            }
        } else {
            AutoPtr<ICmImageUtils> ciu;
            CCmImageUtils::AcquireSingleton((ICmImageUtils**)&ciu);
            AutoPtr<IInputStream> in;
            ec = ciu->GetCroppedKeyguardStream(pkgName, mContext, (IInputStream**)&in);
            if (FAILED(ec)) {
                Logger::E(TAG, "There was an error setting lockscreen wp for pkg %s", pkgName.string());
                return FALSE;
            }
            if (in != NULL) {
                ec = wm->SetKeyguardStream(in);
                if (FAILED(ec)) {
                    Logger::E(TAG, "There was an error setting lockscreen wp for pkg %s", pkgName.string());
                    return FALSE;
                }
                ThemeUtils::CloseQuietly(in);
            }
        }
    // } catch (Exception e) {
        // Log.e(TAG, "There was an error setting lockscreen wp for pkg " + pkgName, e);
        // return FALSE;
    // }
    return TRUE;
}

Boolean CThemeService::UpdateWallpaper(
    /* [in] */ const String pkgName)
{
    String selection = IThemesContractThemesColumns::PKG_NAME + String("= ?");
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = pkgName;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    AutoPtr<IThemesContractThemesColumns> tctc;
    CThemesContractThemesColumns::AcquireSingleton((IThemesContractThemesColumns**)&tctc);
    tctc->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> c;
    resolver->Query(uri, NULL, selection, selectionArgs, String(NULL), (ICursor**)&c);
    Boolean flag = FALSE;
    c->MoveToFirst(&flag);
    AutoPtr<IWallpaperManagerHelper> wmh;
    CWallpaperManagerHelper::AcquireSingleton((IWallpaperManagerHelper**)&wmh);
    AutoPtr<IWallpaperManager> wm;
    wmh->GetInstance(mContext, (IWallpaperManager**)&wm);
    ECode ec = NOERROR;
    if (IThemeConfig::SYSTEM_DEFAULT.Equals(pkgName)) {
        // try {
            ec = wm->Clear();
        // } catch (IOException e) {
            if (ec == (ECode)E_IO_EXCEPTION) {
                return FALSE;
            }
        // } finally {
            ICloseable::Probe(c)->Close();
        // }
    }
    else if (TextUtils::IsEmpty(StringUtils::ParseCharSequence(pkgName))) {
        // try {
            ec = wm->Clear(FALSE);
        // } catch (IOException e) {
            if (ec = (ECode)E_IO_EXCEPTION) {
                return FALSE;
            }
        // } finally {
            ICloseable::Probe(c)->Close();
        // }
    }
    else {
        AutoPtr<IInputStream> in;
        // try {
            AutoPtr<ICmImageUtils> ciu;
            CCmImageUtils::AcquireSingleton((ICmImageUtils**)&ciu);
            ciu->GetCroppedWallpaperStream(pkgName, mContext, (IInputStream**)&in);
            if (in != NULL)
                ec = wm->SetStream(in);
        // } catch (Exception e) {
                if (FAILED(ec)) {
                    return FALSE;
                }
        // } finally {
            ThemeUtils::CloseQuietly(in);
            ICloseable::Probe(c)->Close();
        // }
    }
    return TRUE;
}

Boolean CThemeService::UpdateConfiguration(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Boolean removePerAppThemes)
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am != NULL) {
        Int64 token = Binder::ClearCallingIdentity();
        ECode ec = NOERROR;
        // try {
            AutoPtr<IConfiguration> config;
            ec = am->GetConfiguration((IConfiguration**)&config);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }
            AutoPtr<IThemeConfigBuilder> themeBuilder = CreateBuilderFrom(config, request,
                String(NULL), removePerAppThemes);
            AutoPtr<IThemeConfig> newConfig;
            ec = themeBuilder->Build((IThemeConfig**)&newConfig);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }

            ec = config->SetThemeConfig(newConfig);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }
            ec = am->UpdateConfiguration(config);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }
        // } catch (RemoteException e) {
            // return false;
        // } finally {
            Binder::RestoreCallingIdentity(token);
        // }
    }
    return TRUE;
}

Boolean CThemeService::UpdateConfiguration(
    /* [in] */ IThemeConfig* themeConfig)
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am != NULL) {
        Int64 token = Binder::ClearCallingIdentity();
        // try {
            AutoPtr<IConfiguration> config;
            ECode ec = NOERROR;
            ec = am->GetConfiguration((IConfiguration**)&config);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }

            ec = config->SetThemeConfig(themeConfig);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }
            ec = am->UpdateConfiguration(config);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                return FALSE;
            }
        // } catch (RemoteException e) {
            // return FALSE;
        // } finally {
            Binder::RestoreCallingIdentity(token);
        // }
    }
    return TRUE;
}

Boolean CThemeService::ShouldUpdateConfiguration(
    /* [in] */ IThemeChangeRequest* request)
{
    String overlay;
    String fontTheme;
    String iconsTheme;
    String statusBar;
    String navBar;
    AutoPtr<IMap> map;
    request->GetPerAppOverlays((IMap**)&map);
    Int32 size;
    return (request->GetOverlayThemePackageName(&overlay), !overlay.IsNull()) ||
            (request->GetFontThemePackageName(&fontTheme), !fontTheme.IsNull()) ||
            (request->GetIconsThemePackageName(&iconsTheme), !iconsTheme.IsNull()) ||
            (request->GetStatusBarThemePackageName(&statusBar), !statusBar.IsNull()) ||
            (request->GetNavBarThemePackageName(&navBar), !navBar.IsNull()) ||
            (map->GetSize(&size), size > 0);
}

AutoPtr<IThemeConfigBuilder> CThemeService::CreateBuilderFrom(
    /* [in] */ IConfiguration* config,
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ const String& pkgName,
    /* [in] */ Boolean removePerAppThemes)
{
    AutoPtr<IThemeConfig> tcf;
    config->GetThemeConfig((IThemeConfig**)&tcf);
    AutoPtr<IThemeConfigBuilder> builder;
    CThemeConfigBuilder::New(tcf, (IThemeConfigBuilder**)&builder);
    if (removePerAppThemes) RemovePerAppThemesFromConfig(builder, tcf);

    String name;
    String str;
    if (request->GetIconsThemePackageName(&name), !name.IsNull()) {
        builder->DefaultIcon(pkgName.IsNull() ? (request->GetIconsThemePackageName(&str), str) : pkgName);
    }

    if (request->GetOverlayThemePackageName(&str), !str.IsNull()) {
        builder->DefaultOverlay(pkgName.IsNull() ?
                (request->GetOverlayThemePackageName(&str), str) : pkgName);
    }

    if (request->GetFontThemePackageName(&str), !str.IsNull()) {
        builder->DefaultFont(pkgName.IsNull() ? (request->GetFontThemePackageName(&str), str) : pkgName);
    }

    if (request->GetStatusBarThemePackageName(&str), !str.IsNull()) {
        builder->Overlay(IThemeConfig::SYSTEMUI_STATUS_BAR_PKG, pkgName.IsNull() ?
                (request->GetStatusBarThemePackageName(&str), str) : pkgName);
    }

    if (request->GetNavBarThemePackageName(&str), !str.IsNull()) {
        builder->Overlay(IThemeConfig::SYSTEMUI_NAVBAR_PKG, pkgName.IsNull() ?
                (request->GetNavBarThemePackageName(&str), str) : pkgName);
    }

    // check for any per app overlays being applied
    AutoPtr<IMap> appOverlays;
    request->GetPerAppOverlays((IMap**)&appOverlays);
    AutoPtr<ISet> keySet;
    appOverlays->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > array;
    keySet->ToArray((ArrayOf<IInterface*>**)&array);
    String appPkgName;
    String secondName;
    for (Int32 i = 0; i < array->GetLength(); i++) {
        ICharSequence::Probe((*array)[i])->ToString(&appPkgName);
        AutoPtr<IInterface> obj;
        appOverlays->Get((*array)[i], (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&secondName);
        builder->Overlay(appPkgName, secondName);
    }

    RequestType rt;
    request->GetReqeustType(&rt);
    builder->SetLastThemeChangeRequestType(rt);

    return builder;
}

void CThemeService::RemovePerAppThemesFromConfig(
    /* [in] */ IThemeConfigBuilder* builder,
    /* [in] */ IThemeConfig* themeConfig)
{
    if (themeConfig != NULL) {
        AutoPtr<IMap> themes;
        themeConfig->GetAppThemes((IMap**)&themes);
        AutoPtr<ISet> keySet;
        themes->GetKeySet((ISet**)&keySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        keySet->ToArray((ArrayOf<IInterface*>**)&array);
        String appPkgName;
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            ICharSequence::Probe((*array)[i])->ToString(&appPkgName);
            if (ThemeUtils::IsPerAppThemeComponent(appPkgName)) {
                builder->Overlay(appPkgName, String(NULL));
            }
        }
    }
}

void CThemeService::KillLaunchers(
    /* [in] */ IThemeChangeRequest* request)
{
    String pkgName;
    String str;
    if ((request->GetOverlayThemePackageName(&pkgName), pkgName.IsNull())
            && (request->GetIconsThemePackageName(&str), str.IsNull())) {
        return;
    }

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);

    AutoPtr<IIntent> homeIntent;
    CIntent::New((IIntent**)&homeIntent);
    homeIntent->SetAction(IIntent::ACTION_MAIN);
    homeIntent->AddCategory(IIntent::CATEGORY_HOME);


    AutoPtr<IList> infos;
    mPM->QueryIntentActivities(homeIntent, 0, (IList**)&infos);
    AutoPtr<IIntent> intent;
    CIntent::New(ThemeUtils::ACTION_THEME_CHANGED, 0, (IIntent**)&intent);
    AutoPtr<IList> themeChangeInfos;
    mPM->QueryBroadcastReceivers(intent, 0, (IList**)&themeChangeInfos);
    Int32 size;
    infos->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        infos->Get(i, (IInterface**)&obj);
        IResolveInfo* info = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> ai;
        info->GetActivityInfo((IActivityInfo**)&ai);
        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        String pkgName;
        IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&pkgName);
        if (ai != NULL && applicationInfo != NULL &&
            !IsSetupActivity(info) && !HandlesThemeChanges(pkgName, themeChangeInfos)) {
            String pkgToStop = pkgName;
            Logger::D(TAG, "Force stopping %s for theme change", pkgToStop.string());
            // try {
                ECode ec = NOERROR;
                ec = am->ForceStopPackage(pkgToStop);
            // } catch(Exception e) {
                if (FAILED(ec)) {
                    Logger::E(TAG, "Unable to force stop package, did you forget platform signature?");
                }
            // }
        }
    }
}

Boolean CThemeService::IsSetupActivity(
    /* [in] */ IResolveInfo* info)
{
    AutoPtr<IActivityInfo> activityInfo;
    info->GetActivityInfo((IActivityInfo**)&activityInfo);
    String packageName;
    IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
    return GOOGLE_SETUPWIZARD_PACKAGE.Equals(packageName) ||
           CM_SETUPWIZARD_PACKAGE.Equals(packageName);
}

Boolean CThemeService::HandlesThemeChanges(
    /* [in] */ const String& pkgName,
    /* [in] */ IList* infos)
{
    Int32 size;
    infos->GetSize(&size);
    if (infos != NULL && size > 0) {
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            infos->Get(i, (IInterface**)&obj);
            IResolveInfo* info = IResolveInfo::Probe(obj);
            AutoPtr<IActivityInfo> activityInfo;
            info->GetActivityInfo((IActivityInfo**)&activityInfo);
            String packageName;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
            if (packageName.Equals(pkgName)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void CThemeService::PostProgress()
{
    Int32 N;
    mClients->BeginBroadcast(&N);
    ECode ec = NOERROR;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mClients->GetBroadcastItem(i, (IInterface**)&obj);
        IIThemeChangeListener* listener = IIThemeChangeListener::Probe(obj);
        // try {
            ec = listener->OnProgress(mProgress);
        // } catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::W(TAG, "Unable to post progress to client listener");
            }
        // }
    }
    mClients->FinishBroadcast();
}

void CThemeService::PostFinish(
    /* [in] */ Boolean isSuccess,
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Int64 updateTime)
{
    {    AutoLock syncLock(this);
        mProgress = 0;
    }

    Int32 N;
    mClients->BeginBroadcast(&N);
    ECode ec = NOERROR;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mClients->GetBroadcastItem(i, (IInterface**)&obj);
        IIThemeChangeListener* listener = IIThemeChangeListener::Probe(obj);
        // try {
            ec = listener->OnFinish(isSuccess);
        // } catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::W(TAG, "Unable to post progress to client listener");
            }
        // }
    }
    mClients->FinishBroadcast();

    // if successful, broadcast that the theme changed
    if (isSuccess) {
        BroadcastThemeChange(request, updateTime);
    }
}

void CThemeService::PostFinishedProcessing(
    /* [in] */ const String& pkgName)
{
    Int32 N;
    mProcessingListeners->BeginBroadcast(&N);
    ECode ec = NOERROR;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mProcessingListeners->GetBroadcastItem(i, (IInterface**)&obj);
        IIThemeProcessingListener* listener = IIThemeProcessingListener::Probe(obj);
        // try {
            // listener.onFinishedProcessing(pkgName);
        // } catch(RemoteException e) {
            if (FAILED(ec)) {
                Logger::W(TAG, "Unable to post progress to listener");
            }
        // }
    }
    mProcessingListeners->FinishBroadcast();
}

void CThemeService::BroadcastThemeChange(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Int64 updateTime)
{
    AutoPtr<IMap> componentMap;
    request->GetThemeComponentsMap((IMap**)&componentMap);

    Int32 size;
    if (componentMap == NULL || (componentMap->GetSize(&size), size == 0)) return;

    AutoPtr<IIntent> intent;
    CIntent::New(IThemeUtils::ACTION_THEME_CHANGED, (IIntent**)&intent);
    AutoPtr<ISet> keySet;
    componentMap->GetKeySet((ISet**)&keySet);
    AutoPtr<IArrayList> componentsArrayList;
    CArrayList::New(ICollection::Probe(keySet), (IArrayList**)&componentsArrayList);
    intent->PutStringArrayListExtra(IThemeUtils::EXTRA_COMPONENTS, componentsArrayList);
    RequestType rt;
    request->GetReqeustType(&rt);
    intent->PutExtra(IThemeUtils::EXTRA_REQUEST_TYPE, (Int32)rt);
    intent->PutExtra(IThemeUtils::EXTRA_UPDATE_TIME, updateTime);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void CThemeService::IncrementProgress(
    /* [in] */ Int32 increment)
{
    {    AutoLock syncLock(this);
        mProgress += increment;
        if (mProgress > 100) mProgress = 100;
    }
    PostProgress();
}

ECode CThemeService::RequestThemeChangeUpdates(
    /* [in] */ IIThemeChangeListener* listener)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    Boolean flag = FALSE;
    return mClients->Register(listener, &flag);
}

ECode CThemeService::RemoveUpdates(
    /* [in] */ IIThemeChangeListener* listener)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    Boolean flag = FALSE;
    return mClients->Unregister(listener, &flag);
}

ECode CThemeService::RequestThemeChange(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Boolean removePerAppThemes)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    AutoPtr<IMessage> msg;
    Boolean flag = FALSE;
    /**
     * Since the ThemeService handles compiling theme resource we need to make sure that any
     * of the components we are trying to apply are either already processed or put to the
     * front of the queue and handled before the theme change takes place.
     *
     * TODO: create a callback that can be sent to any ThemeChangeListeners to notify them that
     * the theme will be applied once the processing is done.
     */
    {    AutoLock syncLock(mThemesToProcessQueue);
        AutoPtr<IMap> componentMap;
        request->GetThemeComponentsMap((IMap**)&componentMap);
        AutoPtr<ISet> keySet;
        componentMap->GetKeySet((ISet**)&keySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        keySet->ToArray((ArrayOf<IInterface*>**)&array);
        String key;
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            ICharSequence::Probe((*array)[i])->ToString(&key);
            if (IThemesContractThemesColumns::MODIFIES_OVERLAYS.Equals(key) ||
                    IThemesContractThemesColumns::MODIFIES_NAVIGATION_BAR.Equals(key) ||
                    IThemesContractThemesColumns::MODIFIES_STATUS_BAR.Equals(key) ||
                    IThemesContractThemesColumns::MODIFIES_ICONS.Equals(key)) {
                AutoPtr<IInterface> pkgName;
                componentMap->Get((*array)[i], (IInterface**)&pkgName);
                Int32 index;
                if (mThemesToProcessQueue->IndexOf(pkgName, &index), index > 0) {
                    mThemesToProcessQueue->Remove(pkgName);
                    mThemesToProcessQueue->Add(0, pkgName);
                    // We want to make sure these resources are taken care of first so
                    // send the dequeue message and place it in the front of the queue
                    mResourceProcessingHandler->ObtainMessage(
                        ResourceProcessingHandler::MESSAGE_DEQUEUE_AND_PROCESS_THEME, (IMessage**)&msg);
                    mResourceProcessingHandler->SendMessageAtFrontOfQueue(msg, &flag);
                }
            }

        }
    }
    AutoPtr<IMessageHelper> messageHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&messageHelper);
    msg = NULL;
    messageHelper->Obtain((IMessage**)&msg);
    msg->SetWhat(ThemeWorkerHandler::MESSAGE_CHANGE_THEME);
    msg->SetObj(request);
    msg->SetArg1(removePerAppThemes ? 1 : 0);
    return mHandler->SendMessage(msg, &flag);
}

ECode CThemeService::ApplyDefaultTheme()
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    AutoPtr<IMessageHelper> messageHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&messageHelper);
    AutoPtr<IMessage> msg;
    messageHelper->Obtain((IMessage**)&msg);
    msg->SetWhat(ThemeWorkerHandler::MESSAGE_APPLY_DEFAULT_THEME);
    Boolean flag = FALSE;
    return mHandler->SendMessage(msg, &flag);
}

ECode CThemeService::IsThemeApplying(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    *result = mIsThemeApplying;
    return NOERROR;
}

ECode CThemeService::GetProgress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    {    AutoLock syncLock(this);
        *result = mProgress;
        return NOERROR;
    }
    return NOERROR;
}

ECode CThemeService::CacheComposedIcon(
    /* [in] */ IBitmap* icon,
    /* [in] */ const String& fileName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 token = Binder::ClearCallingIdentity();
    Boolean success;
    AutoPtr<IFileOutputStream> os;
    AutoPtr<IFile> cacheDir;
    CFile::New(IThemeUtils::SYSTEM_THEME_ICON_CACHE_DIR, (IFile**)&cacheDir);
    AutoPtr<ArrayOf<IFile*> > files;
    cacheDir->ListFiles((ArrayOf<IFile*>**)&files);
    Int32 length = files->GetLength();
    if (length == 0) {
        mIconCacheSize = 0;
    }
    // try {
        AutoPtr<IFile> outFile;
        CFile::New(cacheDir, fileName, (IFile**)&outFile);
        CFileOutputStream::New(outFile, (IFileOutputStream**)&os);
        Boolean flag = FALSE;
        ECode ec = NOERROR;
        ec = icon->Compress(BitmapCompressFormat_PNG, 90, IOutputStream::Probe(os), &flag);
        if (FAILED(ec)) {
            success = FALSE;
            Logger::W(TAG, "Unable to cache icon %s", fileName.string());
            return ec;
        }
        ICloseable::Probe(os)->Close();
        FileUtils::SetPermissions(outFile,
                FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IROTH,
                -1, -1);
        Int64 len;
        outFile->GetLength(&len);
        mIconCacheSize += len;
        if (mIconCacheSize > MAX_ICON_CACHE_SIZE) {
            PurgeIconCache();
        }
        success = TRUE;
    // } catch (Exception e) {
        // success = false;
        // Log.w(TAG, "Unable to cache icon " + fileName, e);
    // }
    Binder::RestoreCallingIdentity(token);
    *result = success;
    return NOERROR;
}

ECode CThemeService::ProcessThemeResources(
    /* [in] */ const String& themePkgName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    // try {
        AutoPtr<IPackageInfo> info;
        ECode ec = NOERROR;
        ec = mPM->GetPackageInfo(themePkgName, 0, (IPackageInfo**)&info);
    // } catch (PackageManager.NameNotFoundException e) {
        // Package doesn't exist so nothing to process
        if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
            *result = FALSE;
            return NOERROR;
        }
    // }
    // Obtain a message and send it to the handler to process this theme
    AutoPtr<IMessage> msg;
    mResourceProcessingHandler->ObtainMessage(
            ResourceProcessingHandler::MESSAGE_QUEUE_THEME_FOR_PROCESSING, 0, 0, StringUtils::ParseCharSequence(themePkgName), (IMessage**)&msg);
    Boolean flag = FALSE;
    mResourceProcessingHandler->SendMessage(msg, &flag);
    *result = TRUE;
    return NOERROR;
}

ECode CThemeService::IsThemeBeingProcessed(
    /* [in] */ const String& themePkgName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    {    AutoLock syncLock(mThemesToProcessQueue);
        Boolean flag = FALSE;
        mThemesToProcessQueue->Contains(StringUtils::ParseCharSequence(themePkgName), &flag);
        *result = flag;
        return NOERROR;
    }
    return NOERROR;
}

ECode CThemeService::RegisterThemeProcessingListener(
    /* [in] */ IIThemeProcessingListener* listener)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    Boolean flag = FALSE;
    return mProcessingListeners->Register(listener, &flag);
}

ECode CThemeService::UnregisterThemeProcessingListener(
    /* [in] */ IIThemeProcessingListener* listener)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    Boolean flag = FALSE;
    return mProcessingListeners->Unregister(listener, &flag);
}

ECode CThemeService::RebuildResourceCache()
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_THEME_MANAGER, String(NULL));
    Boolean flag = FALSE;
    return mHandler->SendEmptyMessage(ThemeWorkerHandler::MESSAGE_REBUILD_RESOURCE_CACHE, &flag);
}

ECode CThemeService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CThemeService");
    return NOERROR;
}

void CThemeService::PurgeIconCache()
{
    Logger::D(TAG, "Purging icon cahe of size %d", mIconCacheSize);
    AutoPtr<IFile> cacheDir;
    CFile::New(IThemeUtils::SYSTEM_THEME_ICON_CACHE_DIR, (IFile**)&cacheDir);
    AutoPtr<ArrayOf<IFile*> > files;
    cacheDir->ListFiles((ArrayOf<IFile*>**)&files);
    Arrays::Sort(files, IComparator::Probe(mOldestFilesFirstComparator));
    Boolean flag = FALSE;
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        AutoPtr<IFile> f;
        f = (*files)[i];
        if (f->IsDirectory(&flag), flag) {
            Int64 size;
            f->GetLength(&size);
            if (f->Delete(&flag), flag)
                mIconCacheSize -= size;
        }
        if (mIconCacheSize <= PURGED_ICON_CACHE_SIZE) break;
    }
}

Boolean CThemeService::ApplyBootAnimation(
    /* [in] */ const String& themePath)
{
    Boolean success = FALSE;
    // try {
        AutoPtr<IFile> file;
        CFile::New(themePath, (IFile**)&file);
        AutoPtr<IZipFile> zip;
        CZipFile::New(file, (IZipFile**)&zip);
        AutoPtr<IZipEntry> ze;
        zip->GetEntry(IThemeUtils::THEME_BOOTANIMATION_PATH, (IZipEntry**)&ze);
        if (ze != NULL) {
            ClearBootAnimation();
            AutoPtr<IInputStream> is;
            zip->GetInputStream(ze, (IInputStream**)&is);

            AutoPtr<IBufferedInputStream> bis;
            CBufferedInputStream::New(is, (IBufferedInputStream**)&bis);
            AutoPtr<IFileHelper> ifh;
            CFileHelper::AcquireSingleton((IFileHelper**)&ifh);
            String separator;
            ifh->GetSeparator(&separator);
            String bootAnimationPath = IThemeUtils::SYSTEM_THEME_PATH + separator
                    + "bootanimation.zip";
            ThemeUtils::CopyAndScaleBootAnimation(mContext, is, bootAnimationPath);
            FileUtils::SetPermissions(bootAnimationPath,
                    FileUtils::sS_IRWXU|FileUtils::sS_IRGRP|FileUtils::sS_IROTH, -1, -1);
        }
        ICloseable::Probe(zip)->Close();
        success = TRUE;
    // } catch (Exception e) {
        // Log.w(TAG, "Unable to load boot animation for " + themePath, e);
    // }
    return success;
}

void CThemeService::ClearBootAnimation()
{
    AutoPtr<IFileHelper> ifh;
    CFileHelper::AcquireSingleton((IFileHelper**)&ifh);
    String separator;
    ifh->GetSeparator(&separator);
    AutoPtr<IFile> anim;
    CFile::New(IThemeUtils::SYSTEM_THEME_PATH + separator + "bootanimation.zip", (IFile**)&anim);
    Boolean flag = FALSE;
    if (anim->Exists(&flag), flag) {
        anim->Delete();
    }
}

void CThemeService::ProcessInstalledThemes()
{
    String defaultTheme = ThemeUtils::GetDefaultThemePackageName(mContext);
    AutoPtr<IMessage> msg;
    // Make sure the default theme is the first to get processed!
    if (!IThemeConfig::SYSTEM_DEFAULT.Equals(defaultTheme)) {
        mHandler->ObtainMessage(
                ResourceProcessingHandler::MESSAGE_QUEUE_THEME_FOR_PROCESSING,
                0, 0, StringUtils::ParseCharSequence(defaultTheme), (IMessage**)&msg);
        Boolean flag = FALSE;
        mResourceProcessingHandler->SendMessage(msg, &flag);
    }
    // Iterate over all installed packages and queue up the ones that are themes or icon packs
    AutoPtr<IList> packages;
    mPM->GetInstalledPackages(0, (IList**)&packages);
    Int32 size;
    packages->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        packages->Get(i, (IInterface**)&obj);
        IPackageInfo* info = IPackageInfo::Probe(obj);
        String packageName;
        info->GetPackageName(&packageName);
        Boolean isThemeApk = FALSE;
        info->GetIsThemeApk(&isThemeApk);
        Boolean isLegacyIconPackApk = FALSE;
        info->GetIsLegacyIconPackApk(&isLegacyIconPackApk);
        if (!defaultTheme.Equals(packageName) &&
                (isThemeApk || isLegacyIconPackApk)) {

            mHandler->ObtainMessage(
                    ResourceProcessingHandler::MESSAGE_QUEUE_THEME_FOR_PROCESSING,
                    0, 0, StringUtils::ParseCharSequence(packageName), (IMessage**)&msg);
            Boolean flag = FALSE;
            mResourceProcessingHandler->SendMessage(msg, &flag);
        }
    }
}

void CThemeService::SendThemeResourcesCachedBroadcast(
    /* [in] */ const String& themePkgName,
    /* [in] */ Int32 resultCode)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_THEME_RESOURCES_CACHED, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_THEME_PACKAGE_NAME, themePkgName);
    intent->PutExtra(IIntent::EXTRA_THEME_RESULT, resultCode);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void CThemeService::PostFailedThemeInstallNotification(
    /* [in] */ const String& name)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> nm = INotificationManager::Probe(obj);
    AutoPtr<INotificationBuilder> nbr;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&nbr);
    nbr->SetAutoCancel(TRUE);
    nbr->SetOngoing(FALSE);
    String str;
    mContext->GetString(R::string::theme_install_error_title, &str);
    nbr->SetContentTitle(StringUtils::ParseCharSequence(str));
    mContext->GetString(R::string::theme_install_error_message, &str);
    String ft("");
    ft.AppendFormat("%s%s", str.string(), name.string());
    nbr->SetContentText(StringUtils::ParseCharSequence(ft));
    nbr->SetSmallIcon(R::drawable::stat_notify_error);
    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    Int64 millis;
    isystem->GetCurrentTimeMillis(&millis);
    nbr->SetWhen(millis);
    AutoPtr<INotification> notice;
    nbr->Build((INotification**)&notice);

    nm->Notify(name.GetHashCode(), notice);
}

String CThemeService::GetThemeName(
    /* [in] */ IPackageInfo* pi)
{
    AutoPtr<IThemeInfo> themeInfo;
    pi->GetThemeInfo((IThemeInfo**)&themeInfo);
    String name;
    Boolean isLegacyIconPackApk = FALSE;
    if (themeInfo != NULL) {
        IBaseThemeInfo::Probe(themeInfo)->GetName(&name);
        return name;
    }
    else if (pi->GetIsLegacyIconPackApk(&isLegacyIconPackApk), isLegacyIconPackApk) {
        AutoPtr<IApplicationInfo> info;
        pi->GetApplicationInfo((IApplicationInfo**)&info);
        IPackageItemInfo::Probe(info)->GetName(&name);
        return name;
    }

    return String(NULL);
}

} // Server
} // Droid
} // Elastos