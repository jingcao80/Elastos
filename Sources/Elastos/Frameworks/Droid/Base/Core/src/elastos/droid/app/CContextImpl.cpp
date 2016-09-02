
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/app/CContextImpl.h"
#include "elastos/droid/app/ReceiverRestrictedContext.h"
#include "elastos/droid/app/CReceiverRestrictedContext.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CInstrumentation.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/CUiModeManager.h"
#include "elastos/droid/app/CNotificationManager.h"
#include "elastos/droid/app/ApplicationPackageManager.h"
#include "elastos/droid/app/CWallpaperManager.h"
#include "elastos/droid/app/CAlarmManager.h"
#include "elastos/droid/app/CKeyguardManager.h"
#include "elastos/droid/app/SharedPreferencesImpl.h"
#include "elastos/droid/app/CStatusBarManager.h"
#include "elastos/droid/app/CSearchManager.h"
#include "elastos/droid/app/CResourcesManager.h"
#include "elastos/droid/app/CAppOpsManager.h"
#include "elastos/droid/app/CJobSchedulerImpl.h"
#include "elastos/droid/app/CProfileManager.h"
#include "elastos/droid/app/admin/CDevicePolicyManager.h"
// #include "elastos/droid/app/backup/CBackupManager.h"
#include "elastos/droid/app/usage/CUsageStatsManager.h"
#include "elastos/droid/app/trust/CTrustManager.h"
#include "elastos/droid/appwidget/CAppWidgetManager.h"
#include "elastos/droid/hardware/CConsumerIrManager.h"
#include "elastos/droid/hardware/CCmHardwareManager.h"
#include "elastos/droid/hardware/CSystemSensorManager.h"
//#include "elastos/droid/hardware/CSerialManager.h"
//#include "elastos/droid/hardware/usb/CUsbManager.h"
#include "elastos/droid/hardware/camera2/CCameraManager.h"
#include "elastos/droid/hardware/display/CDisplayManager.h"
#include "elastos/droid/hardware/input/CInputManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CPowerManager.h"
#include "elastos/droid/os/CSystemVibrator.h"
// #include "elastos/droid/os/CDropBoxManager.h"
#include "elastos/droid/os/CUserManager.h"
#include "elastos/droid/os/storage/CStorageManager.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CClipboardManager.h"
#include "elastos/droid/content/CRestrictionsManager.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/CThemeManager.h"
#include "elastos/droid/content/pm/CLauncherApps.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/location/CLocationManager.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CNetworkPolicyManager.h"
#include "elastos/droid/net/CNetworkScoreManager.h"
#include "elastos/droid/telecom/CTelecomManager.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/wifi/CWifiManager.h"
#include "elastos/droid/wifi/p2p/CWifiP2pManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/view/textservice/CTextServicesManager.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/media/CAudioManager.h"
#include "elastos/droid/media/session/CMediaSessionManager.h"
#include "elastos/droid/media/projection/CMediaProjectionManager.h"
#include "elastos/droid/service/persistentdata/CPersistentDataBlockManager.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"
#include "elastos/droid/accounts/CAccountManager.h"
// #include "elastos/droid/privacy/CPrivacySettingsManager.h"
// #include "elastos/droid/privacy/surrogate/CPrivacyLocationManager.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/R.h"
#include <elastos/core/ClassLoader.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::R;
// using Elastos::Droid::Accounts::CAccountManager;
using Elastos::Droid::Accounts::IIAccountManager;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::CAccountManager;
using Elastos::Droid::Accounts::EIID_IIAccountManager;
using Elastos::Droid::App::CUiModeManager;
using Elastos::Droid::App::NotificationManager;
using Elastos::Droid::App::CStatusBarManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::App::CSearchManager;
using Elastos::Droid::App::CKeyguardManager;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::CAppOpsManager;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::App::CProfileManager;
using Elastos::Droid::App::CJobSchedulerImpl;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::Admin::CDevicePolicyManager;
using Elastos::Droid::App::Backup::IBackupManager;
// using Elastos::Droid::App::Backup::CBackupManager;
using Elastos::Droid::App::Job::IIJobScheduler;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Usage::IIUsageStatsManager;
using Elastos::Droid::App::Usage::IUsageStatsManager;
using Elastos::Droid::App::Usage::CUsageStatsManager;
using Elastos::Droid::App::Trust::ITrustManager;
using Elastos::Droid::App::Trust::CTrustManager;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::CAppWidgetManager;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::CClipboardManager;
using Elastos::Droid::Content::IIRestrictionsManager;
using Elastos::Droid::Content::IRestrictionsManager;
using Elastos::Droid::Content::CRestrictionsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Pm::IILauncherApps;
using Elastos::Droid::Content::Pm::ILauncherApps;
using Elastos::Droid::Content::Pm::CLauncherApps;
using Elastos::Droid::Content::Res::IIThemeService;
using Elastos::Droid::Content::Res::IThemeManager;
using Elastos::Droid::Content::Res::CThemeManager;
using Elastos::Droid::Database::Sqlite::SQLiteDatabase;
using Elastos::Droid::Hardware::ISystemSensorManager;
using Elastos::Droid::Hardware::CSystemSensorManager;
// using Elastos::Droid::Hardware::CSerialManager;
using Elastos::Droid::Hardware::IConsumerIrManager;
using Elastos::Droid::Hardware::CConsumerIrManager;
using Elastos::Droid::Hardware::ICmHardwareManager;
using Elastos::Droid::Hardware::CCmHardwareManager;
using Elastos::Droid::Hardware::ISerialManager;
using Elastos::Droid::Hardware::IISerialManager;
using Elastos::Droid::Hardware::EIID_IISerialManager;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::CDisplayManager;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
// using Elastos::Droid::Hardware::Usb::CUsbManager;
using Elastos::Droid::Hardware::Camera2::ICameraManager;
using Elastos::Droid::Hardware::Camera2::CCameraManager;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetService;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Internal::Policy::CPolicyManager;
// using Elastos::Droid::Internal::Os::IDropBoxManagerService;
// using Elastos::Droid::Internal::Os::EIID_IDropBoxManagerService;
using Elastos::Droid::Location::CLocationManager;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::EIID_IILocationManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::CAudioManager;
using Elastos::Droid::Media::Session::IMediaSessionManager;
using Elastos::Droid::Media::Session::CMediaSessionManager;
using Elastos::Droid::Media::Projection::IMediaProjectionManager;
using Elastos::Droid::Media::Projection::CMediaProjectionManager;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::CNetworkPolicyManager;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::IINetworkPolicyManager;
using Elastos::Droid::Net::INetworkScoreManager;
using Elastos::Droid::Net::CNetworkScoreManager;
using Elastos::Droid::Net::EIID_IIEthernetManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::IIVibratorService;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::CSystemVibrator;
// using Elastos::Droid::Os::CDropBoxManager;
using Elastos::Droid::Os::CUserManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IIUserManager;
using Elastos::Droid::Os::CPowerManager;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::CStorageManager;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Privacy::IPrivacySettingsManager;
// using Elastos::Droid::Privacy::CPrivacySettingsManager;
// using Elastos::Droid::Privacy::Surrogate::IPrivacyLocationManager;
// using Elastos::Droid::Privacy::Surrogate::CPrivacyLocationManager;
using Elastos::Droid::Service::Persistentdata::IIPersistentDataBlockService;
using Elastos::Droid::Service::Persistentdata::IPersistentDataBlockManager;
using Elastos::Droid::Service::Persistentdata::CPersistentDataBlockManager;
using Elastos::Droid::Telecom::CTelecomManager;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::DisplayAdjustments;
using Elastos::Droid::View::WindowManagerImpl;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::TextService::CTextServicesManager;
using Elastos::Droid::View::TextService::ITextServicesManager;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::CWifiManager;
using Elastos::Droid::Wifi::IIWifiManager;
using Elastos::Droid::Wifi::EIID_IIWifiManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::CWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IIWifiP2pManager;
using Elastos::Droid::Wifi::P2p::EIID_IIWifiP2pManager;

using Elastos::Core::ClassLoader;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

CContextImpl::ApplicationContentResolver::ApplicationContentResolver()
{}

CContextImpl::ApplicationContentResolver::~ApplicationContentResolver()
{}

ECode CContextImpl::ApplicationContentResolver::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CActivityThread* mainThread,
    /* [in] */ IUserHandle* user)
{
    assert(mainThread != NULL && user != NULL);
    FAIL_RETURN(ContentResolver::constructor(context))
    mMainThread = mainThread;
    mUser = user;
    return NOERROR;
}

ECode CContextImpl::ApplicationContentResolver::AcquireProvider(
    /* [in] */ IContext* context,
    /* [in] */ const String& auth,
    /* [out] */ IIContentProvider** provider)
{
    return mMainThread->AcquireProvider(context,
        ContentProvider::GetAuthorityWithoutUserId(auth),
        ResolveUserIdFromAuthority(auth), TRUE, provider);
}

ECode CContextImpl::ApplicationContentResolver::AcquireExistingProvider(
    /* [in] */ IContext* context,
    /* [in] */ const String& auth,
    /* [out] */ IIContentProvider** provider)
{
    return mMainThread->AcquireExistingProvider(context,
        ContentProvider::GetAuthorityWithoutUserId(auth),
        ResolveUserIdFromAuthority(auth), TRUE, provider);
}

ECode CContextImpl::ApplicationContentResolver::ReleaseProvider(
    /* [in] */ IIContentProvider* provider,
    /* [out] */ Boolean* result)
{
   return mMainThread->ReleaseProvider(provider, TRUE, result);
}

ECode CContextImpl::ApplicationContentResolver::AcquireUnstableProvider(
    /* [in] */ IContext* c,
    /* [in] */ const String& auth,
    /* [out] */ IIContentProvider** provider)
{
    return mMainThread->AcquireProvider(c,
        ContentProvider::GetAuthorityWithoutUserId(auth),
        ResolveUserIdFromAuthority(auth), FALSE, provider);
}

ECode CContextImpl::ApplicationContentResolver::ReleaseUnstableProvider(
    /* [in] */ IIContentProvider* icp,
    /* [out] */ Boolean* result)
{
    return mMainThread->ReleaseProvider(icp, FALSE, result);
}

ECode CContextImpl::ApplicationContentResolver::UnstableProviderDied(
    /* [in] */ IIContentProvider* icp)
{
    return mMainThread->HandleUnstableProviderDied(IBinder::Probe(icp), TRUE);
}

ECode CContextImpl::ApplicationContentResolver::AppNotRespondingViaProvider(
    /* [in] */ IIContentProvider* icp)
{
    return mMainThread->AppNotRespondingViaProvider(IBinder::Probe(icp));
}

/** @hide */
Int32 CContextImpl::ApplicationContentResolver::ResolveUserIdFromAuthority(
    /* [in] */ const String& auth)
{
    Int32 id;
    mUser->GetIdentifier(&id);
    return ContentProvider::GetUserIdFromAuthority(auth, id);
}

//==================================================================================
// CContextImpl
//==================================================================================
const String CContextImpl::TAG("ContextImpl");
const Boolean CContextImpl::DEBUG = FALSE;
Object CContextImpl::sLock;
AutoPtr<IArrayMap> CContextImpl::sSharedPrefs;
AutoPtr< ArrayOf<String> > CContextImpl::EMPTY_FILE_LIST = ArrayOf<String>::Alloc(0);

AutoPtr<IWallpaperManager> CContextImpl::sWallpaperManager;

CAR_INTERFACE_IMPL(CContextImpl, ContextWrapper, IContextImpl)

CAR_OBJECT_IMPL(CContextImpl)

CContextImpl::CContextImpl()
    : mRestricted(FALSE)
    , mThemeResource(0)
{
    mDisplayAdjustments = new DisplayAdjustments();
}

CContextImpl::~CContextImpl()
{
    mContentResolver = NULL;
}

AutoPtr<CContextImpl> CContextImpl::GetImpl(
    /* [in] */ IContext* _context)
{
    AutoPtr<IContext> context = _context;
    AutoPtr<IContext> nextContext;
    AutoPtr<IContextWrapper> cw = IContextWrapper::Probe(context);
    while (cw != NULL && (cw->GetBaseContext((IContext**)&nextContext), nextContext != NULL)) {
        context = nextContext;
        nextContext = NULL;
        cw = IContextWrapper::Probe(context);
    }
    return (CContextImpl*)IContextImpl::Probe(context);
}

ECode CContextImpl::GetAssets(
    /* [out] */ IAssetManager** assetManager)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetAssets(assetManager);
    return NOERROR;
}

ECode CContextImpl::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources);
    *resources = mResources;
    REFCOUNT_ADD(*resources);
    return NOERROR;
}

ECode CContextImpl::GetPackageManager(
    /* [out] */ IPackageManager** packageManager)
{
    VALIDATE_NOT_NULL(packageManager);
    *packageManager = NULL;

    if (mPackageManager != NULL) {
        *packageManager = mPackageManager;
        REFCOUNT_ADD(*packageManager);
        return NOERROR;
    }

    AutoPtr<IIPackageManager> pm = CActivityThread::GetPackageManager();
    if (pm != NULL) {
        // Doesn't matter if we make more than one instance.
        mPackageManager = new ApplicationPackageManager(this, pm);
        *packageManager = mPackageManager;
        REFCOUNT_ADD(*packageManager);
        return NOERROR;
    }

    return NOERROR;
}

ECode CContextImpl::GetContentResolver(
    /* [out] */ IContentResolver** resolver)
{
    VALIDATE_NOT_NULL(resolver)
    *resolver = (IContentResolver*)mContentResolver.Get();
    REFCOUNT_ADD(*resolver);
    return NOERROR;
}

ECode CContextImpl::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    mMainThread->GetLooper(looper);
    return NOERROR;
}

ECode CContextImpl::GetApplicationContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);

    AutoPtr<IApplication> app;
    if (mPackageInfo != NULL) {
        app = mPackageInfo->GetApplication();
    }
    else {
        mMainThread->GetApplication((IApplication**)&app);
    }

    *ctx = IContext::Probe(app);
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CContextImpl::RegisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    VALIDATE_NOT_NULL(componentCallback);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    ctx->RegisterComponentCallbacks(componentCallback);
    return NOERROR;
}

ECode CContextImpl::UnregisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    VALIDATE_NOT_NULL(componentCallback);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    ctx->UnregisterComponentCallbacks(componentCallback);
    return NOERROR;
}

ECode CContextImpl::GetText(
    /* [in] */ Int32 resId,
    /* [out] */ ICharSequence** text)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetText(resId, text);
    return NOERROR;
}

ECode CContextImpl::GetString(
    /* [in] */ Int32 resId,
    /* [out] */ String* str)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetString(resId, str);
    return NOERROR;
}

ECode CContextImpl::GetString(
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetString(resId, formatArgs, str);
    return NOERROR;
}

ECode CContextImpl::SetTheme(
    /* [in] */ Int32 resid)
{
    mThemeResource = resid;
    return NOERROR;
}

ECode CContextImpl::GetThemeResId(
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    *resId = mThemeResource;
    return NOERROR;
}

ECode CContextImpl::GetTheme(
    /* [out] */ IResourcesTheme** theme)
{
    VALIDATE_NOT_NULL(theme);

    if (mTheme == NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        GetOuterContext()->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version;
        appInfo->GetTargetSdkVersion(&version);
        mThemeResource = CResources::SelectDefaultTheme(mThemeResource, version);
        mResources->NewTheme((IResourcesTheme**)&mTheme);
        mTheme->ApplyStyle(mThemeResource, TRUE);
    }

    *theme = mTheme;
    REFCOUNT_ADD(*theme);
    return NOERROR;
}

ECode CContextImpl::RecreateTheme()
{
    AutoPtr<IResourcesTheme> newTheme;
    mResources->NewTheme((IResourcesTheme**)&newTheme);
    newTheme->ApplyStyle(mThemeResource, TRUE);
    mTheme->SetTo(newTheme);
    return NOERROR;
}

ECode CContextImpl::ObtainStyledAttributes(
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    theme->ObtainStyledAttributes(attrs, styles);
    return NOERROR;
}

ECode CContextImpl::ObtainStyledAttributes(
    /* [in] */ Int32 resid,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    theme->ObtainStyledAttributes(resid, attrs, styles);
    return NOERROR;
}

ECode CContextImpl::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    theme->ObtainStyledAttributes(set, attrs, 0, 0, styles);
    return NOERROR;
}

ECode CContextImpl::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [out] */ ITypedArray** styles)
{
    AutoPtr<IResourcesTheme> theme;
    GetTheme((IResourcesTheme**)&theme);
    theme->ObtainStyledAttributes(
            set, attrs, defStyleAttr, defStyleRes, styles);
    return NOERROR;
}

ECode CContextImpl::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);

    AutoPtr<IClassLoader> cl;
    if (mPackageInfo != NULL) {
        mPackageInfo->GetClassLoader((IClassLoader**)&cl);
    }
    else {
        cl = ClassLoader::GetSystemClassLoader();
    }

    *loader = cl;
    REFCOUNT_ADD(*loader)
    return NOERROR;
}

ECode CContextImpl::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);

    if (mPackageInfo != NULL) {
        return mPackageInfo->GetPackageName(packageName);
    }

    // No mPackageInfo means this is a Context for the system itself,
    // and this here is its name.
    *packageName = String("android");
    return NOERROR;
}

ECode CContextImpl::GetBasePackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);

    if (!mBasePackageName.IsNull()) {
        *packageName = mBasePackageName;
        return NOERROR;
    }

    return GetPackageName(packageName);
}

ECode CContextImpl::GetOpPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);

    if (!mOpPackageName.IsNull()) {
        *packageName = mOpPackageName;
        return NOERROR;
    }

    return GetPackageName(packageName);
}

ECode CContextImpl::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if (mPackageInfo != NULL) {
        return mPackageInfo->GetApplicationInfo(info);
    }
    return E_RUNTIME_EXCEPTION;
}

ECode CContextImpl::GetPackageResourcePath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = String(NULL);

    if (mPackageInfo != NULL) {
        return mPackageInfo->GetResDir(path);
    }
    return E_RUNTIME_EXCEPTION;
}

ECode CContextImpl::GetPackageCodePath(
    /* [out] */ String* codePath)
{
    VALIDATE_NOT_NULL(codePath);
    *codePath = String(NULL);

    if (mPackageInfo != NULL) {
        return mPackageInfo->GetAppDir(codePath);
    }
    return E_RUNTIME_EXCEPTION;
}

ECode CContextImpl::GetSharedPrefsFile(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    return MakeFilename(GetPreferencesDir(), name + ".xml", file);
}

ECode CContextImpl::GetSharedPreferences(
    /* [in] */ const String& inName,
    /* [in] */ Int32 mode,
    /* [out] */ ISharedPreferences** prefs)
{
    VALIDATE_NOT_NULL(prefs);
    *prefs = NULL;

    String name(inName);
    AutoPtr<SharedPreferencesImpl> sp;
    {    AutoLock syncLock(sLock);
        if (sSharedPrefs == NULL) {
            CArrayMap::New((IArrayMap**)&sSharedPrefs);
        }

        String packageName;
        GetPackageName(&packageName);
        AutoPtr<ICharSequence> packageNameObj = CoreUtils::Convert(packageName);
        AutoPtr<IInterface> packagePrefsObj;
        sSharedPrefs->Get(packageNameObj.Get(), (IInterface**)&packagePrefsObj);
        AutoPtr<IArrayMap> packagePrefs = IArrayMap::Probe(packagePrefsObj);

        if (packagePrefs == NULL) {
            CArrayMap::New((IArrayMap**)&packagePrefs);
            sSharedPrefs->Put(packageNameObj.Get(), packagePrefs.Get());
        }

        // At least one application in the world actually passes in a NULL
        // name.  This happened to work because when we generated the file name
        // we would stringify it to "NULL.xml".  Nice.
        AutoPtr<IApplicationInfo> appInfo;
        mPackageInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version;
        appInfo->GetTargetSdkVersion(&version);

        if (version < Build::VERSION_CODES::KITKAT) {
            if (name.IsNull()) {
                name = "NULL";
            }
        }

        AutoPtr<ICharSequence> nameObj = CoreUtils::Convert(name);
        AutoPtr<IInterface> spObj;

        packagePrefs->Get(nameObj.Get(), (IInterface**)&spObj);
        if (spObj == NULL) {
            AutoPtr<IFile> prefsFile;
            GetSharedPrefsFile(name, (IFile**)&prefsFile);
            sp = new SharedPreferencesImpl();
            sp->constructor(prefsFile, mode);
            packagePrefs->Put(nameObj.Get(), TO_IINTERFACE(sp));

            *prefs = sp;
            REFCOUNT_ADD(*prefs);
            return NOERROR;
        }
        else {
            sp = (SharedPreferencesImpl*)ISharedPreferences::Probe(spObj);
        }
    }

    AutoPtr<IApplicationInfo> appInfo;
    GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 version;
    appInfo->GetTargetSdkVersion(&version);
    if ((mode & IContext::MODE_MULTI_PROCESS) != 0 ||
        version < Elastos::Droid::Os::Build::VERSION_CODES::HONEYCOMB) {
        // If somebody else (some other process) changed the prefs
        // file behind our back, we reload it.  This has been the
        // historical (if undocumented) behavior.
        sp->StartReloadIfChangedUnexpectedly();
    }

    *prefs = sp;
    REFCOUNT_ADD(*prefs);
    return NOERROR;
}

AutoPtr<IFile> CContextImpl::GetPreferencesDir()
{
    AutoLock lock(mSync);
    if (mPreferencesDir == NULL) {
        AutoPtr<IFile> file;
        GetDataDirFile((IFile**)&file);
        CFile::New(file, String("shared_prefs"), (IFile**)&mPreferencesDir);
    }
    return mPreferencesDir;
}

ECode CContextImpl::OpenFileInput(
    /* [in] */ const String& name,
    /* [out] */ IFileInputStream** fileInputStream)
{
    AutoPtr<IFile> filesDir;
    GetFilesDir((IFile**)&filesDir);
    AutoPtr<IFile> f;
    MakeFilename(filesDir, name, (IFile**)&f);
    return CFileInputStream::New(f, fileInputStream);
}

ECode CContextImpl::OpenFileOutput(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */IFileOutputStream** fileOutputStream)
{
    Boolean append = (mode&MODE_APPEND) != 0;
    AutoPtr<IFile> filesDir;
    GetFilesDir((IFile**)&filesDir);
    AutoPtr<IFile> f;
    MakeFilename(filesDir, name, (IFile**)&f);
    // try {
    CFileOutputStream::New(f, append, fileOutputStream);
    String path;
    f->GetPath(&path);
    SetFilePermissionsFromMode(path, mode, 0);
    return NOERROR;
    // } catch (FileNotFoundException e) {
    // }

    AutoPtr<IFile> parent;
    f->GetParentFile((IFile**)&parent);
    Boolean res;
    parent->Mkdir(&res);
    String parentPath;
    parent->GetPath(&parentPath);
    FileUtils::SetPermissions(
        parentPath,
        FileUtils::sS_IRWXU|FileUtils::sS_IRWXG|FileUtils::sS_IXOTH,
        -1, -1);
    CFileOutputStream::New(f, append, fileOutputStream);
    SetFilePermissionsFromMode(path, mode, 0);
    return NOERROR;
}

AutoPtr<IFile> CContextImpl::CreateFilesDirLocked(
        /* [in] */ IFile* file)
{
    Boolean bval;
    file->Exists(&bval);
    if (!bval) {
        file->Mkdirs(&bval);
        String path;
        file->GetPath(&path);

        if (!bval) {
            file->Exists(&bval);
            if (bval) {
                // spurious failure; probably racing with another process for this app
                return file;
            }

            Logger::W(TAG, "Unable to create files subdir %s", path.string());
            return NULL;
        }

        FileUtils::SetPermissions(
            path,
            FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH,
            -1, -1);
    }
    return file;
}

ECode CContextImpl::DeleteFile(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    AutoPtr<IFile> filesDir;
    GetFilesDir((IFile**)&filesDir);
    AutoPtr<IFile> f;
    MakeFilename(filesDir, name, (IFile**)&f);
    return f->Delete(succeeded);
}

ECode CContextImpl::GetFileStreamPath(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    AutoPtr<IFile> filesDir;
    GetFilesDir((IFile**)&filesDir);
    MakeFilename(filesDir, name, file);
    return NOERROR;
}

ECode CContextImpl::GetFilesDir(
    /* [out] */ IFile** filesDir)
{
    AutoLock lock(mSync);
    if (mFilesDir == NULL) {
        AutoPtr<IFile> file;
        GetDataDirFile((IFile**)&file);
        CFile::New(file, String("files"), (IFile**)&mFilesDir);
    }
    Boolean bval;
    mFilesDir->Exists(&bval);
    if (!bval) {
        String path;
        mFilesDir->GetPath(&path);
        mFilesDir->Mkdirs(&bval);
        if (!bval) {
            Slogger::W(TAG, "Unable to create files directory %s", path.string());
            *filesDir = NULL;
            return NOERROR;
        }
        FileUtils::SetPermissions(
            path,
            FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH,
            -1, -1);
    }
    *filesDir = mFilesDir;
    REFCOUNT_ADD(*filesDir);
    return NOERROR;
}

ECode CContextImpl::GetNoBackupFilesDir(
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir)
    *filesDir = NULL;

    {    AutoLock syncLock(mSync);
        if (mNoBackupFilesDir == NULL) {
            AutoPtr<IFile> file;
            GetDataDirFile((IFile**)&file);
            CFile::New(file, String("no_backup"), (IFile**)&mNoBackupFilesDir);
        }
        AutoPtr<IFile> result = CreateFilesDirLocked(mNoBackupFilesDir);
        *filesDir = result;
        REFCOUNT_ADD(*filesDir)
    }

    return NOERROR;
}

ECode CContextImpl::GetExternalFilesDir(
    /* [in] */ const String& type,
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir)
    *filesDir = NULL;

    // Operates on primary external storage
    AutoPtr<ArrayOf<IFile*> > dirs;
    GetExternalFilesDirs(type, (ArrayOf<IFile*>**)&dirs);
    if (dirs != NULL && dirs->GetLength() > 0) {
        *filesDir = (*dirs)[0];
        REFCOUNT_ADD(*filesDir)
    }
    return NOERROR;
}

ECode CContextImpl::GetExternalFilesDirs(
    /* [in] */ const String& type,
     /* [out, callee] */ ArrayOf<IFile*>** filesDirs)
{
    VALIDATE_NOT_NULL(filesDirs)
    *filesDirs = NULL;

    {    AutoLock syncLock(mSync);
        if (mExternalFilesDirs == NULL) {
            String packageName;
            GetPackageName(&packageName);
            mExternalFilesDirs = Environment::BuildExternalStorageAppFilesDirs(packageName);
        }

        // Splice in requested type, if any
        AutoPtr<ArrayOf<IFile*> > dirs = mExternalFilesDirs;
        if (!type.IsNull()) {
            AutoPtr<ArrayOf<String> > segments = ArrayOf<String>::Alloc(1);
            segments->Set(0, type);
            dirs = Environment::BuildPaths(mExternalFilesDirs, segments);
        }

        // Create dirs if needed
        AutoPtr<ArrayOf<IFile*> > result = EnsureDirsExistOrFilter(dirs);
        *filesDirs = result;
        REFCOUNT_ADD(*filesDirs);
    }

    return NOERROR;
}

ECode CContextImpl::GetObbDir(
    /* [out] */ IFile** obbDir)
{
    VALIDATE_NOT_NULL(obbDir)
    *obbDir = NULL;

    // Operates on primary external storage
    AutoPtr<ArrayOf<IFile*> > dirs;
    GetObbDirs((ArrayOf<IFile*>**)&dirs);
    if (dirs != NULL && dirs->GetLength() > 0) {
        *obbDir = (*dirs)[0];
        REFCOUNT_ADD(*obbDir)
    }
    return NOERROR;
}

ECode CContextImpl::GetObbDirs(
     /* [out, callee] */ ArrayOf<IFile*>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = NULL;

    {    AutoLock syncLock(mSync);
        if (mExternalObbDirs == NULL) {
            String packageName;
            GetPackageName(&packageName);
            mExternalObbDirs = Environment::BuildExternalStorageAppObbDirs(packageName);
        }

        // Create dirs if needed
        AutoPtr<ArrayOf<IFile*> > result = EnsureDirsExistOrFilter(mExternalObbDirs);
        *dirs = result;
        REFCOUNT_ADD(*dirs);
    }

    return NOERROR;
}

ECode CContextImpl::GetCacheDir(
    /* [out] */ IFile** cacheDir)
{
    VALIDATE_NOT_NULL(cacheDir)
    {
        AutoLock lock(mSync);
        if (mCacheDir == NULL) {
            AutoPtr<IFile> file;
            GetDataDirFile((IFile**)&file);
            CFile::New(file, String("cache"), (IFile**)&mCacheDir);
        }

        AutoPtr<IFile> result = CreateFilesDirLocked(mCacheDir);
        *cacheDir = result;
        REFCOUNT_ADD(*cacheDir);
    }

    return NOERROR;
}

ECode CContextImpl::GetCodeCacheDir(
    /* [out] */ IFile** cacheDir)
{
    VALIDATE_NOT_NULL(cacheDir)
    {
        AutoLock lock(mSync);
        if (mCodeCacheDir == NULL) {
            AutoPtr<IFile> file;
            GetDataDirFile((IFile**)&file);
            CFile::New(file, String("code_cache"), (IFile**)&mCodeCacheDir);
        }

        AutoPtr<IFile> result = CreateFilesDirLocked(mCodeCacheDir);
        *cacheDir = result;
        REFCOUNT_ADD(*cacheDir);
    }

    return NOERROR;
}

ECode CContextImpl::GetExternalCacheDir(
    /* [out] */ IFile** externalDir)
{
    VALIDATE_NOT_NULL(externalDir)
    *externalDir = NULL;

    // Operates on primary external storage
    AutoPtr<ArrayOf<IFile*> > dirs;
    GetExternalCacheDirs((ArrayOf<IFile*>**)&dirs);
    if (dirs != NULL && dirs->GetLength() > 0) {
        *externalDir = (*dirs)[0];
        REFCOUNT_ADD(*externalDir)
    }
    return NOERROR;
}

ECode CContextImpl::GetExternalCacheDirs(
     /* [out, callee] */ ArrayOf<IFile*>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = NULL;

    {    AutoLock syncLock(mSync);
        if (mExternalCacheDirs == NULL) {
            String packageName;
            GetPackageName(&packageName);
            mExternalCacheDirs = Environment::BuildExternalStorageAppCacheDirs(packageName);
        }

        // Create dirs if needed
        AutoPtr<ArrayOf<IFile*> > result = EnsureDirsExistOrFilter(mExternalCacheDirs);
        *dirs = result;
        REFCOUNT_ADD(*dirs)
    }

    return NOERROR;
}

ECode CContextImpl::GetExternalMediaDirs(
     /* [out, callee] */ ArrayOf<IFile*>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = NULL;

    {    AutoLock syncLock(mSync);
        if (mExternalMediaDirs == NULL) {
            String packageName;
            GetPackageName(&packageName);
            mExternalMediaDirs = Environment::BuildExternalStorageAppMediaDirs(packageName);
        }

        // Create dirs if needed
        AutoPtr<ArrayOf<IFile*> > tmp = EnsureDirsExistOrFilter(mExternalMediaDirs);
        *dirs = tmp;
        REFCOUNT_ADD(*dirs)
    }
    return NOERROR;
}

ECode CContextImpl::GetFileList(
    /* [out, callee] */ ArrayOf<String>** fileList)
{
    VALIDATE_NOT_NULL(fileList);

    AutoPtr<IFile> filesDir;
    GetFilesDir((IFile**)&filesDir);
    AutoPtr< ArrayOf<String> > list;
    filesDir->List((ArrayOf<String>**)&list);
    *fileList = (list != NULL) ? list: EMPTY_FILE_LIST;
    REFCOUNT_ADD(*fileList);
    return NOERROR;
}

ECode CContextImpl::GetDataDirFile(
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file);
    *file = NULL;

    if (mPackageInfo != NULL) {
        mPackageInfo->GetDataDirFile(file);
        return NOERROR;
    }
    return E_RUNTIME_EXCEPTION;
}

ECode CContextImpl::GetDir(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir);

    String lname = String("app_") + name;
    AutoPtr<IFile> ff;
    GetDataDirFile((IFile**)&ff);
    AutoPtr<IFile> file;
    MakeFilename(ff, lname, (IFile**)&file);
    Boolean isExist;
    if (file->Exists(&isExist), !isExist) {
        Boolean isMk;
        file->Mkdir(&isMk);
        String path;
        file->GetPath(&path);
        SetFilePermissionsFromMode(path, mode,
                FileUtils::sS_IRWXU|FileUtils::sS_IRWXG|FileUtils::sS_IXOTH);
    }
    *dir = file;
    REFCOUNT_ADD(*dir);
    return NOERROR;
}

ECode CContextImpl::GetUserId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    if (mUser != NULL) {
        return mUser->GetIdentifier(id);
    }
    else {
        *id = 0;
    }
    return NOERROR;
}

ECode CContextImpl::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    return OpenOrCreateDatabase(name, mode, factory, NULL, sqliteDB);
}

ECode CContextImpl::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    VALIDATE_NOT_NULL(sqliteDB);
    *sqliteDB = NULL;

    AutoPtr<IFile> f = ValidateFilePath(name, TRUE);
    assert(f != NULL);
    if (f == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 flags = ISQLiteDatabase::CREATE_IF_NECESSARY;
    if ((mode & IContext::MODE_ENABLE_WRITE_AHEAD_LOGGING) != 0) {
        flags |= ISQLiteDatabase::ENABLE_WRITE_AHEAD_LOGGING;
    }
    String path;
    f->GetPath(&path);
    AutoPtr<ISQLiteDatabase> db;
    FAIL_RETURN(SQLiteDatabase::OpenDatabase(path, factory, flags, errorHandler, (ISQLiteDatabase**)&db))
    SetFilePermissionsFromMode(path, mode, 0);
    *sqliteDB = db;
    REFCOUNT_ADD(*sqliteDB);
    return NOERROR;
}

ECode CContextImpl::DeleteDatabase(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

//    try {
    AutoPtr<IFile> f = ValidateFilePath(name, FALSE);
    return SQLiteDatabase::DeleteDatabase(f, succeeded);
//    } catch (Exception e) {
//    }
}

ECode CContextImpl::GetDatabasePath(
    /* [in] */ const String& name,
    /* [out] */ IFile** path)
{
    VALIDATE_NOT_NULL(path);
    AutoPtr<IFile> f = ValidateFilePath(name, FALSE);
    *path = f;
    REFCOUNT_ADD(*path);
    return NOERROR;
}

ECode CContextImpl::GetDatabaseList(
    /* [out, callee] */ ArrayOf<String>** databaseList)
{
    VALIDATE_NOT_NULL(databaseList);

    AutoPtr< ArrayOf<String> > list;
    GetDatabasesDir()->List((ArrayOf<String>**)&list);
    *databaseList = (list != NULL) ? list: EMPTY_FILE_LIST;
    REFCOUNT_ADD(*databaseList);
    return NOERROR;
}

AutoPtr<IFile> CContextImpl::GetDatabasesDir()
{
    AutoLock lock(mSync);
    if (mDatabasesDir == NULL) {
        AutoPtr<IFile> file;
        GetDataDirFile((IFile**)&file);
        CFile::New(file, String("databases"), (IFile**)&mDatabasesDir);
    }

    String path;
    mDatabasesDir->GetPath(&path);
    if (path.Equals("databases")) {
        mDatabasesDir = NULL;
        CFile::New(String("/data/system"), (IFile**)&mDatabasesDir);
    }
    return mDatabasesDir;
}

ECode CContextImpl::GetWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return GetWallpaperManager()->GetDrawable(drawable);
}

ECode CContextImpl::PeekWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return GetWallpaperManager()->PeekDrawable(drawable);
}

ECode CContextImpl::GetWallpaperDesiredMinimumWidth(
    /* [out] */ Int32* minWidth)
{
    return GetWallpaperManager()->GetDesiredMinimumWidth(minWidth);
}

ECode CContextImpl::GetWallpaperDesiredMinimumHeight(
    /* [out] */ Int32* minHeight)
{
    return GetWallpaperManager()->GetDesiredMinimumHeight(minHeight);
}

ECode CContextImpl::SetWallpaper(
    /* [in] */ IBitmap* bitmap)
{
    return GetWallpaperManager()->SetBitmap(bitmap);
}

ECode CContextImpl::SetWallpaper(
    /* [in] */ IInputStream* data)
{
    return GetWallpaperManager()->SetStream(data);
}

ECode CContextImpl::ClearWallpaper()
{
    return GetWallpaperManager()->Clear();
}

ECode CContextImpl::StartActivity(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    WarnIfCallingFromSystemProcess();
    StartActivity(intent, NULL);
    return NOERROR;
}

ECode CContextImpl::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    VALIDATE_NOT_NULL(intent);

    WarnIfCallingFromSystemProcess();
    Int32 flags;
    intent->GetFlags(&flags);
    if ((flags&IIntent::FLAG_ACTIVITY_NEW_TASK) == 0) {
        return E_RUNTIME_EXCEPTION;
//        throw new AndroidRuntimeException(
//                "Calling startActivity() from outside of an Activity "
//                + " context requires the FLAG_ACTIVITY_NEW_TASK flag."
//                + " Is this really what you want?");
    }
    AutoPtr<IInstrumentation> instrumentation;
    mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IContext> ctx = GetOuterContext();
    AutoPtr<IInstrumentationActivityResult> instrumentationResult;
    AutoPtr<IActivity> actvity;
    instrumentation->ExecStartActivity(
        ctx, IBinder::Probe(appThread), NULL,
        actvity, intent, -1, options, (IInstrumentationActivityResult**)&instrumentationResult);
    return NOERROR;
}

ECode CContextImpl::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return StartActivityAsUser(intent, NULL, user);
}

ECode CContextImpl::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user)
{
    VALIDATE_NOT_NULL(intent);
    VALIDATE_NOT_NULL(user);

//    try {
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    String type, packageName;
    GetBasePackageName(&packageName);
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    intent->ResolveTypeIfNeeded(resolver, &type);
    Int32 id, result;
    user->GetIdentifier(&id);
    ActivityManagerNative::GetDefault()->StartActivityAsUser(
        appThread, packageName, intent, type,
        NULL, String(NULL), 0, IIntent::FLAG_ACTIVITY_NEW_TASK, NULL, options,
        id, &result);
    return NOERROR;
//    } catch (RemoteException re) {
//    }
}

ECode CContextImpl::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents)
{
    VALIDATE_NOT_NULL(intents);

    WarnIfCallingFromSystemProcess();
    StartActivities(intents, NULL);
    return NOERROR;
}

ECode CContextImpl::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    VALIDATE_NOT_NULL(intents);
    VALIDATE_NOT_NULL(options);

    WarnIfCallingFromSystemProcess();
    Int32 flags;
    (*intents)[0]->GetFlags(&flags);
    if ((flags&IIntent::FLAG_ACTIVITY_NEW_TASK) == 0) {
        return E_RUNTIME_EXCEPTION;
//        throw new AndroidRuntimeException(
//                "Calling startActivities() from outside of an Activity "
//                + " context requires the FLAG_ACTIVITY_NEW_TASK flag on first Intent."
//                + " Is this really what you want?");
    }
    AutoPtr<IInstrumentation> instrumentation;
    mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IContext> ctx = GetOuterContext();
    instrumentation->ExecStartActivities(
        ctx, IBinder::Probe(appThread.Get()), NULL, NULL, intents, options);
    return NOERROR;
}

ECode CContextImpl::StartActivitiesAsUser(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userHandle)
{
    Int32 flags;
    (*intents)[0]->GetFlags(&flags);
    if ((flags&IIntent::FLAG_ACTIVITY_NEW_TASK) == 0) {
        return E_RUNTIME_EXCEPTION;
//        throw new AndroidRuntimeException(
//                "Calling startActivities() from outside of an Activity "
//                + " context requires the FLAG_ACTIVITY_NEW_TASK flag on first Intent."
//                + " Is this really what you want?");
    }
    AutoPtr<IInstrumentation> instrumentation;
    mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    userHandle->GetIdentifier(&id);
    AutoPtr<IContext> ctx = GetOuterContext();
    instrumentation->ExecStartActivitiesAsUser(
        ctx, IBinder::Probe(appThread.Get()), NULL,
        NULL, intents, options, id);
    return NOERROR;
}

ECode CContextImpl::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags, NULL);
}

ECode CContextImpl::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
//    try {
    String resolvedType = String(NULL);
    if (fillInIntent != NULL) {
        Boolean bval;
        fillInIntent->MigrateExtraStreamToClipData(&bval);
        fillInIntent->PrepareToLeaveProcess();
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        fillInIntent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
    }
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 result;
    ActivityManagerNative::GetDefault()->
        StartActivityIntentSender(appThread, intent,
                fillInIntent, resolvedType, NULL, String(NULL),
                0, flagsMask, flagsValues, options, &result);
    if (result == IActivityManager::START_CANCELED) {
        return E_SEND_INTENT_EXCEPTION;
//        throw new IntentSender.SendIntentException();
    }
    CInstrumentation::CheckStartActivityResult(result, NULL);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    WarnIfCallingFromSystemProcess();
    String resolvedType;
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, NULL, IActivity::RESULT_OK,
        String(NULL), NULL, String(NULL),
        IAppOpsManager::OP_NONE, FALSE, FALSE, id, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//     try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, NULL, IActivity::RESULT_OK,
        String(NULL), NULL, receiverPermission,
        IAppOpsManager::OP_NONE, FALSE, FALSE, id, &result);
//     } catch (RemoteException e) {
//     }
    return NOERROR;
}

ECode CContextImpl::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, NULL, IActivity::RESULT_OK,
        String(NULL), NULL, receiverPermission,
        IAppOpsManager::OP_NONE, TRUE, FALSE, id, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return SendOrderedBroadcast(intent, receiverPermission, IAppOpsManager::OP_NONE,
        resultReceiver, scheduler, initialCode, initialData, initialExtras);
}

ECode CContextImpl::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IIntentReceiver> rd;
    if (resultReceiver != NULL) {
        if (mPackageInfo != NULL) {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IInstrumentation> instrumentation;
            mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
            AutoPtr<IContext> ctx = GetOuterContext();
            mPackageInfo->GetReceiverDispatcher(
                resultReceiver, ctx, scheduler,
                instrumentation, FALSE, (IIntentReceiver**)&rd);
        } else {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IContext> ctx = GetOuterContext();
            AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher = new LoadedPkg::ReceiverDispatcher();
            FAIL_RETURN(dispatcher->constructor(resultReceiver, ctx, scheduler, NULL, FALSE))
            rd = dispatcher->GetIIntentReceiver();
        }
    }
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, rd,
        initialCode, initialData, initialExtras, receiverPermission, appOp,
        TRUE, FALSE, id, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
        intent->PrepareToLeaveProcess();
        AutoPtr<IApplicationThread> appThread;
        mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
        Int32 identifier;
        user->GetIdentifier(&identifier);
        Int32 result;
        ActivityManagerNative::GetDefault()->BroadcastIntent(appThread,
            intent, resolvedType, NULL, IActivity::RESULT_OK, String(NULL),
            NULL, String(NULL), IAppOpsManager::OP_NONE, FALSE, FALSE, identifier, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission)
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
        intent->PrepareToLeaveProcess();
        AutoPtr<IApplicationThread> appThread;
        mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
        Int32 identifier = 0;
        user->GetIdentifier(&identifier);
        Int32 result;
        ActivityManagerNative::GetDefault()->BroadcastIntent(
            appThread, intent, resolvedType, NULL,
            IActivity::RESULT_OK, String(NULL), NULL, receiverPermission,
            IAppOpsManager::OP_NONE, FALSE, FALSE, identifier, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return SendOrderedBroadcastAsUser(
        intent, user, receiverPermission, IAppOpsManager::OP_NONE,
            resultReceiver, scheduler, initialCode, initialData, initialExtras);;
}

ECode CContextImpl::SendOrderedBroadcastAsUser(
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
    AutoPtr<IIntentReceiver> rd;
    if (resultReceiver != NULL) {
        if (mPackageInfo != NULL) {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IInstrumentation> instrumentation;
            mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
            AutoPtr<IContext> ctx = GetOuterContext();
            mPackageInfo->GetReceiverDispatcher(
                resultReceiver, ctx, scheduler,
                instrumentation, FALSE, (IIntentReceiver**)&rd);
        } else {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IContext> ctx = GetOuterContext();
            AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher = new LoadedPkg::ReceiverDispatcher();
            FAIL_RETURN(dispatcher->constructor(resultReceiver, ctx, scheduler, NULL, FALSE))
            rd = dispatcher->GetIIntentReceiver();
        }
    }
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 identifier = 0;
    user->GetIdentifier(&identifier);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, rd,
        initialCode, initialData, initialExtras, receiverPermission,
        appOp, TRUE, FALSE, identifier, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id = 0;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, NULL,
        IActivity::RESULT_OK, String(NULL), NULL, String(NULL),
        IAppOpsManager::OP_NONE, FALSE, TRUE, id, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendStickyOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IIntentReceiver> rd;
    if (resultReceiver != NULL) {
        if (mPackageInfo != NULL) {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IInstrumentation> instrumentation;
            mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
            AutoPtr<IContext> ctx = GetOuterContext();
            mPackageInfo->GetReceiverDispatcher(
                resultReceiver, ctx, scheduler,
                instrumentation, FALSE, (IIntentReceiver**)&rd);
        } else {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IContext> ctx = GetOuterContext();
            AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher = new LoadedPkg::ReceiverDispatcher();
            FAIL_RETURN(dispatcher->constructor(resultReceiver, ctx, scheduler, NULL, FALSE))
            rd = dispatcher->GetIIntentReceiver();
        }
    }
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, rd,
        initialCode, initialData, initialExtras, String(NULL),
        IAppOpsManager::OP_NONE, TRUE, TRUE, id, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::RemoveStickyBroadcast(
    /* [in] */ IIntent* inIntent)
{
    AutoPtr<IIntent> intent = inIntent;
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
    if (!resolvedType.IsNull()) {
        intent = NULL;
        CIntent::New(inIntent, (IIntent**)&intent);
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        intent->SetDataAndType(data, resolvedType);
    }
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 id;
    GetUserId(&id);
    ActivityManagerNative::GetDefault()->UnbroadcastIntent(
            appThread, intent, id);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
        intent->PrepareToLeaveProcess();
        AutoPtr<IApplicationThread> appThread;
        mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
        Int32 identifier;
        user->GetIdentifier(&identifier);
        Int32 result;
        ActivityManagerNative::GetDefault()->BroadcastIntent(
            appThread, intent, resolvedType, NULL,
            IActivity::RESULT_OK, String(NULL), NULL, String(NULL),
            IAppOpsManager::OP_NONE, FALSE, TRUE, identifier, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::SendStickyOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    AutoPtr<IIntentReceiver> rd;
    if (resultReceiver != NULL) {
        if (mPackageInfo != NULL) {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IInstrumentation> instrumentation;
            mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
            AutoPtr<IContext> ctx = GetOuterContext();
            mPackageInfo->GetReceiverDispatcher(
                resultReceiver, ctx, scheduler,
                instrumentation, FALSE, (IIntentReceiver**)&rd);
        } else {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IContext> ctx = GetOuterContext();
            AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher = new LoadedPkg::ReceiverDispatcher();
            FAIL_RETURN(dispatcher->constructor(resultReceiver, ctx, scheduler, NULL, FALSE))
            rd = dispatcher->GetIIntentReceiver();
        }
    }
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
//    try {
    intent->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    Int32 identifier;
    user->GetIdentifier(&identifier);
    Int32 result;
    ActivityManagerNative::GetDefault()->BroadcastIntent(
        appThread, intent, resolvedType, rd,
        initialCode, initialData, initialExtras, String(NULL),
        IAppOpsManager::OP_NONE, TRUE, TRUE, identifier, &result);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::RemoveStickyBroadcastAsUser(
    /* [in] */ IIntent* inIntent,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IIntent> intent = inIntent;
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String resolvedType;
    intent->ResolveTypeIfNeeded(contentResolver, &resolvedType);
    if (resolvedType != String(NULL)) {
        intent = NULL;
        CIntent::New(inIntent, (IIntent**)&intent);
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        intent->SetDataAndType(data, resolvedType);
    }
//    try {
        intent->PrepareToLeaveProcess();
        AutoPtr<IApplicationThread> appThread;
        mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
        Int32 identifier;
        user->GetIdentifier(&identifier);
        ActivityManagerNative::GetDefault()->UnbroadcastIntent(
                appThread, intent, identifier);
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}

ECode CContextImpl::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [out] */ IIntent** intent)
{
    return RegisterReceiver(receiver, filter, String(NULL), NULL, intent);
}

ECode CContextImpl::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** intent)
{
    Int32 id = 0;
    GetUserId(&id);
    AutoPtr<IContext> ctx = GetOuterContext();
    return RegisterReceiverInternal(receiver, id,
            filter, broadcastPermission, scheduler, ctx, intent);
}

ECode CContextImpl::RegisterReceiverAsUser(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IUserHandle* user,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    Int32 identifier;
    user->GetIdentifier(&identifier);
    AutoPtr<IContext> ctx = GetOuterContext();
    return RegisterReceiverInternal(receiver, identifier,
            filter, broadcastPermission, scheduler, ctx, stickyIntent);
}

ECode CContextImpl::RegisterReceiverInternal(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ Int32 userId,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    AutoPtr<IIntentReceiver> rd;
    if (receiver != NULL) {
        if (mPackageInfo != NULL && context != NULL) {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<IInstrumentation> instrumentation;
            mMainThread->GetInstrumentation((IInstrumentation**)&instrumentation);
            mPackageInfo->GetReceiverDispatcher(
                receiver, context, scheduler,
                instrumentation, TRUE, (IIntentReceiver**)&rd);
        }
        else {
            if (scheduler == NULL) {
                scheduler = mMainThread->GetHandler();
            }
            AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher = new LoadedPkg::ReceiverDispatcher();
            FAIL_RETURN(dispatcher->constructor(receiver, context, scheduler, NULL, TRUE))
            rd = dispatcher->GetIIntentReceiver();
        }
    }
//     try {
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    ActivityManagerNative::GetDefault()->RegisterReceiver(
            appThread, mBasePackageName,
            rd, filter, broadcastPermission, userId, intent);
//     } catch (RemoteException e) {
//         return NULL;
//     }
    return NOERROR;
}

ECode CContextImpl::UnregisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver)
{
    if (mPackageInfo != NULL) {
        AutoPtr<IContext> ctx = GetOuterContext();
        AutoPtr<IIntentReceiver> rd;
        mPackageInfo->ForgetReceiverDispatcher(ctx, receiver, (IIntentReceiver**)&rd);
        ActivityManagerNative::GetDefault()->UnregisterReceiver(rd);
        return NOERROR;
    }

    Logger::E(TAG, "Not supported in system context");
    return E_RUNTIME_EXCEPTION;
}

ECode CContextImpl::ValidateServiceIntent(
    /* [in] */ IIntent* service)
{
    AutoPtr<IComponentName> cn;
    service->GetComponent((IComponentName**)&cn);
    String pkgName;
    service->GetPackage(&pkgName);
    if (cn == NULL && pkgName.IsNull()) {
        AutoPtr<IApplicationInfo> ai;
        GetApplicationInfo((IApplicationInfo**)&ai);
        Int32 targetSdkVersion;
        ai->GetTargetSdkVersion(&targetSdkVersion);
        if (targetSdkVersion >= Build::VERSION_CODES::LOLLIPOP) {
            Logger::E(TAG, "IllegalArgumentException : Service Intent must be explicit: %s",
                TO_CSTR(service));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else {
            Logger::W(TAG, "Implicit intents with startService are not safe: %s",
                TO_CSTR(service)); //Debug.getCallers(2, 3));
        }
    }
    return NOERROR;
}

ECode CContextImpl::StartService(
    /* [in] */ IIntent* service,
    /* [out] */ IComponentName** name)
{
    WarnIfCallingFromSystemProcess();
    return StartServiceCommon(service, mUser, name);
}

ECode CContextImpl::StopService(
    /* [in] */ IIntent* service,
    /* [out] */ Boolean* succeeded)
{
    WarnIfCallingFromSystemProcess();
    return StopServiceCommon(service, mUser, succeeded);
}

ECode CContextImpl::StartServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ IComponentName** name)
{
    return StartServiceCommon(service, user, name);
}

ECode CContextImpl::StartServiceCommon(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;
    VALIDATE_NOT_NULL(user);

    FAIL_RETURN(ValidateServiceIntent(service))
    service->PrepareToLeaveProcess();

    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String type;
    service->ResolveTypeIfNeeded(contentResolver, &type);
    Int32 identifier = 0;
    user->GetIdentifier(&identifier);

    AutoPtr<IComponentName> cn;
    ActivityManagerNative::GetDefault()->StartService(
        appThread, service, type, identifier, (IComponentName**)&cn);
    if (cn != NULL) {
        String pkgName;
        cn->GetPackageName(&pkgName);
        if (pkgName.Equals("!")) {
            String className;
            cn->GetClassName(&className);
            Slogger::E(TAG, "Not allowed to start service without permission: %s", className.string());
            return E_SECURITY_EXCEPTION;
        }
        else if (pkgName.Equals("!!")) {
            String className;
            cn->GetClassName(&className);
            Slogger::E(TAG, "Unable to start service: %s", className.string());
            return E_SECURITY_EXCEPTION;
        }
    }

    *name = cn;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode CContextImpl::StopServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    return StopServiceCommon(service, user, succeeded);
}

ECode CContextImpl::StopServiceCommon(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;
    VALIDATE_NOT_NULL(user);
    VALIDATE_NOT_NULL(service);

//     try {
    service->SetAllowFds(FALSE);
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    String type;
    service->ResolveTypeIfNeeded(contentResolver, &type);
    Int32 identifier;
    user->GetIdentifier(&identifier);
    Int32 res;
    ActivityManagerNative::GetDefault()->StopService(
        appThread, service, type, identifier, &res);
    if (res < 0) {
        String intentStr;
        service->ToString(&intentStr);
        Slogger::E(TAG, "Not allowed to stop service : %s", intentStr.string());
        return E_SECURITY_EXCEPTION;
    }
    *succeeded = res != 0;
    return NOERROR;
//     } catch (RemoteException e) {
//         return false;
//     }
}

ECode CContextImpl::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    WarnIfCallingFromSystemProcess();
    AutoPtr<IUserHandle> user;
    Process::MyUserHandle((IUserHandle**)&user);
    return BindServiceCommon(service, conn, flags, user, succeeded);
}

ECode CContextImpl::BindServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* succeeded)
{
    return BindServiceCommon(service, conn, flags, userHandle, succeeded);
}

ECode CContextImpl::BindServiceCommon(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;

    AutoPtr<IIServiceConnection> sd;
    if (conn == NULL) {
        Slogger::E(TAG, "connection is NULL");
//        throw new IllegalArgumentException("connection is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mPackageInfo != NULL) {
        AutoPtr<IContext> ctx = GetOuterContext();
        mPackageInfo->GetServiceDispatcher(conn, ctx,
                mMainThread->GetHandler(), flags, (IIServiceConnection**)&sd);
    }
    else {
        Slogger::E(TAG, "Not supported in system context");
//        throw new RuntimeException("Not supported in system context");
        return E_RUNTIME_EXCEPTION;
    }

    FAIL_RETURN(ValidateServiceIntent(service))

//    try {
    AutoPtr<IBinder> token = GetActivityToken();
    AutoPtr<IApplicationInfo> appInfo;
    mPackageInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 version;
    appInfo->GetTargetSdkVersion(&version);
    if (token == NULL && (flags & IContext::BIND_AUTO_CREATE) == 0 && mPackageInfo != NULL
           && version < Elastos::Droid::Os::Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
       flags |= BIND_WAIVE_PRIORITY;
    }

    service->PrepareToLeaveProcess();
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IContentResolver> cResolver;
    GetContentResolver((IContentResolver**)&cResolver);
    String type;
    service->ResolveTypeIfNeeded(cResolver, &type);
    Int32 res, id;
    user->GetIdentifier(&id);
    ActivityManagerNative::GetDefault()->BindService(
        appThread, GetActivityToken(), service, type, sd, flags, id, &res);
    if (res < 0) {
        Slogger::E(TAG, "Not allowed to bind to service %s", TO_CSTR(service));
        return E_SECURITY_EXCEPTION;
    }
    *succeeded = res != 0;
    return NOERROR;
//    } catch (RemoteException e) {
//        return false;
//    }
}

ECode CContextImpl::UnbindService(
    /* [in] */ Elastos::Droid::Content::IServiceConnection* conn)
{
    if (conn == NULL) {
        Slogger::E(TAG, "connection is NULL");
//        throw new IllegalArgumentException("connection is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mPackageInfo != NULL) {
        AutoPtr<IContext> ctx = GetOuterContext();
        AutoPtr<IIServiceConnection> sd;
        mPackageInfo->ForgetServiceDispatcher(ctx, conn, (IIServiceConnection**)&sd);
//        try {
        Boolean unbinded;
        ActivityManagerNative::GetDefault()->UnbindService(sd, &unbinded);
//        } catch (RemoteException e) {
//        }
    }
    else {
        Slogger::E(TAG, "Not supported in system context");
//        throw new RuntimeException("Not supported in system context");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CContextImpl::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

//    try {
    if (arguments != NULL) {
        Boolean prev;
        arguments->SetAllowFds(FALSE, &prev);
    }
    Int32 id;
    GetUserId(&id);
    return ActivityManagerNative::GetDefault()->StartInstrumentation(
        className, profileFile, 0, arguments, NULL, NULL, id,
        String(NULL) /* ABI override */, succeeded);
//    } catch (RemoteException e) {
//        // System has crashed, nothing we can do.
//    }
}

ECode CContextImpl::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;

    HashMap< String, AutoPtr<IInterface> >::Iterator it = mServiceCache.Find(name);
    if (it != mServiceCache.End()) {
        *object = it->mSecond;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    if (IContext::ACCESSIBILITY_SERVICE.Equals(name)) {
        AutoPtr<IAccessibilityManager> aManager;
        CAccessibilityManager::GetInstance(this, (IAccessibilityManager**)&aManager);
        *object = aManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::CAPTIONING_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0 && "TODO");
        // public Object getService(ContextImpl ctx) {
        //     return new CaptioningManager(ctx);
        // }});
        return NOERROR;
    }
    else if (IContext::ACCOUNT_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::ACCOUNT_SERVICE);
        AutoPtr<IIAccountManager> accountService = IIAccountManager::Probe(service);
        AutoPtr<IAccountManager> accountManager;
        CAccountManager::New(this, accountService, (IAccountManager**)&accountManager);
        *object = accountManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::ACTIVITY_SERVICE.Equals(name)) {
        AutoPtr<IActivityManager> activityManager;
        FAIL_RETURN(CActivityManager::New(GetOuterContext(), mMainThread->GetHandler(),
                (IActivityManager**)&activityManager));
        *object = activityManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::ALARM_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::ALARM_SERVICE);
        AutoPtr<IIAlarmManager> alarmService = IIAlarmManager::Probe(service.Get());
        assert(alarmService != NULL);
        AutoPtr<IAlarmManager> alarmManager;
        CAlarmManager::New(alarmService , this, (IAlarmManager**)&alarmManager);
        *object = alarmManager.Get();
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::AUDIO_SERVICE.Equals(name)) {
        AutoPtr<IAudioManager> obj;
        CAudioManager::New(this, (IAudioManager**)&obj);
        *object = obj.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    else if (IContext::MEDIA_ROUTER_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        // assert(0 && "TODO");
    //     AutoPtr<IMediaRouter> obj;
    //     CMediaRouter::New(this, (IMediaRouter**)&obj);
    //     *object = obj.Get();
    //     REFCOUNT_ADD(*object);
    //     return NOERROR;
    }
    else if (IContext::BLUETOOTH_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0 && "TODO");
        // public Object createService(ContextImpl ctx) {
        //     return new BluetoothManager(ctx);
        // }});
        return NOERROR;
    }
    else if (IContext::HDMI_CONTROL_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        // assert(0 && "TODO");
        // public Object createStaticService() {
        //     IBinder b = ServiceManager.getService(HDMI_CONTROL_SERVICE);
        //     return new HdmiControlManager(IHdmiControlService.Stub.asInterface(b));
        // }});
        return NOERROR;
    }
    else if (IContext::CLIPBOARD_SERVICE.Equals(name)) {
        AutoPtr<IClipboardManager> cbm;
        FAIL_RETURN(CClipboardManager::New(GetOuterContext(), (IClipboardManager**)&cbm))
        *object = cbm.Get();
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    else if (IContext::CONNECTIVITY_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::CONNECTIVITY_SERVICE);
        AutoPtr<IIConnectivityManager> mgr = IIConnectivityManager::Probe(service.Get());
        AutoPtr<IConnectivityManager> connManager;
        CConnectivityManager::New(mgr , (IConnectivityManager**)&connManager);
        *object = connManager.Get();
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::COUNTRY_DETECTOR.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0 && "TODO");
        // registerService(COUNTRY_DETECTOR, new StaticServiceFetcher() {
        //         public Object createStaticService() {
        //             IBinder b = ServiceManager.getService(COUNTRY_DETECTOR);
        //             return new CountryDetector(ICountryDetector.Stub.asInterface(b));
        //         }});
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::DEVICE_POLICY_SERVICE.Equals(name)) {
        AutoPtr<IHandler> handler = mMainThread->GetHandler();
        AutoPtr<IDevicePolicyManager> devicePolicy;
        CDevicePolicyManager::New(this, handler, (IDevicePolicyManager**)&devicePolicy);
        *object = devicePolicy.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::DOWNLOAD_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IContentResolver> resolver;
        GetContentResolver((IContentResolver**)&resolver);
        String pkgName;
        GetPackageName(&pkgName);
        AutoPtr<IDownloadManager> downloadManager;
        CDownloadManager::New(resolver, pkgName, (IDownloadManager**)&downloadManager);
        *object = downloadManager.Get();
        mServiceCache[name] = downloadManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::BATTERY_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0 && "TODO");
        // public Object createService(ContextImpl ctx) {
        //     return new BatteryManager();
        // }});
        return NOERROR;
    }
    else if (IContext::NFC_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        return E_NOT_IMPLEMENTED;
    }
    else if (IContext::DROPBOX_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        // public Object createStaticService() {
        //     return createDropBoxManager();
        // }});
        // assert(0 && "TODO");
        return NOERROR;
    }
    else if (IContext::INPUT_SERVICE.Equals(name)) {
        AutoPtr<IInputManager> mgr = CInputManager::GetInstance();
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::DISPLAY_SERVICE.Equals(name)) {
        AutoPtr<IDisplayManager> dm;
        CDisplayManager::New(GetOuterContext(), (IDisplayManager**)&dm);
        *object = dm.Get();
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    else if (IContext::INPUT_METHOD_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IInputMethodManager> iManager = CInputMethodManager::GetInstance();
        mServiceCache[name] = iManager;
        *object = iManager;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TEXT_SERVICES_MANAGER_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);
        AutoPtr<ITextServicesManager> iManager = CTextServicesManager::GetInstance();
        mServiceCache[name] = iManager;
        *object = iManager;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::KEYGUARD_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IKeyguardManager> obj;
        CKeyguardManager::New((IKeyguardManager**)&obj);
        mServiceCache[name] = obj.Get();
        *object = obj.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::LAYOUT_INFLATER_SERVICE.Equals(name)) {
        AutoPtr<IPolicyManager> pm;
        FAIL_RETURN(CPolicyManager::AcquireSingleton((IPolicyManager**)&pm));
        AutoPtr<ILayoutInflater> inflater;
        FAIL_RETURN(pm->MakeNewLayoutInflater((IContext*)this, (ILayoutInflater**)&inflater));
        *object = inflater.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::LOCATION_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::LOCATION_SERVICE);
        AutoPtr<IILocationManager> mgr = IILocationManager::Probe(service);
        AutoPtr<ILocationManager> lm;
        CLocationManager::New(this, mgr, (ILocationManager**)&lm);
        *object = lm.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::NETWORK_POLICY_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(NETWORK_POLICY_SERVICE);
        AutoPtr<INetworkPolicyManager> npm;
        CNetworkPolicyManager::New(IINetworkPolicyManager::Probe(service), (INetworkPolicyManager**)&npm);
        *object = npm.Get();
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    else if (IContext::NOTIFICATION_SERVICE.Equals(name)) {
        AutoPtr<IContext> ctx = GetOuterContext();
        AutoPtr<IApplicationInfo> appInfo;
        ctx->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version;
        appInfo->GetTargetSdkVersion(&version);
        Int32 value = CResources::SelectSystemTheme(
            0,
            version,
            R::style::Theme_Dialog,
            R::style::Theme_Holo_Dialog,
            R::style::Theme_DeviceDefault_Dialog,
            R::style::Theme_DeviceDefault_Light_Dialog);
        AutoPtr<IContext> wrapper;
        CContextThemeWrapper::New(ctx, value, (IContext**)&wrapper);
        AutoPtr<INotificationManager> nm;
        CNotificationManager::New(wrapper, mMainThread->GetHandler(), (INotificationManager**)&nm);
        *object = TO_IINTERFACE(nm);
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::POWER_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::POWER_SERVICE);
        AutoPtr<IIPowerManager> powerService = IIPowerManager::Probe(service);
        AutoPtr<IPowerManager> powerManager;
        CPowerManager::New(GetOuterContext(), powerService, mMainThread->GetHandler(),
            (IPowerManager**)&powerManager);
        *object = powerManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::SEARCH_SERVICE.Equals(name)) {
        AutoPtr<ISearchManager> searchManager;
        CSearchManager::New(GetOuterContext(), mMainThread->GetHandler(), (ISearchManager**)&searchManager);
        *object = searchManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::SENSOR_SERVICE.Equals(name)) {
        AutoPtr<ILooper> looper;
        mMainThread->GetHandler()->GetLooper((ILooper**)&looper);
        AutoPtr<ISystemSensorManager> managersensor;
        CSystemSensorManager::New(GetOuterContext(), looper, (ISystemSensorManager**)&managersensor);
        *object = managersensor.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::STATUS_BAR_SERVICE.Equals(name)) {
        AutoPtr<IStatusBarManager> statusBarManager;
        ASSERT_SUCCEEDED(CStatusBarManager::New(GetOuterContext(), (IStatusBarManager**)&statusBarManager));
        *object = statusBarManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::STORAGE_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<ILooper> looper;
        mMainThread->GetHandler()->GetLooper((ILooper**)&looper);
        AutoPtr<IContentResolver> cr;
        GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IStorageManager> sManager;
        CStorageManager::New(cr, looper, (IStorageManager**)&sManager);
        *object = sManager.Get();
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TELEPHONY_SERVICE.Equals(name)) {
        AutoPtr<ITelephonyManager> tm;
        CTelephonyManager::New(GetOuterContext(), (ITelephonyManager**)&tm);
        *object = tm.Get();
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TELECOM_SERVICE.Equals(name)) {
        AutoPtr<ITelecomManager> ts;
        CTelecomManager::New(GetOuterContext(), (ITelecomManager**)&ts);
        *object = ts.Get();
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::UI_MODE_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IUiModeManager> uManager;
        CUiModeManager::New((IUiModeManager**)&uManager);
        *object = uManager.Get();
        mServiceCache[name] = uManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::USB_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::USB_SERVICE);
        AutoPtr<IIUsbManager> mgr = IIUsbManager::Probe(service);
        AutoPtr<IUsbManager> usbManager;
        assert(0 && "TODO");
        // CUsbManager::New(this, mgr, (IUsbManager**)&usbManager);
        *object = usbManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::SERIAL_SERVICE.Equals(name)) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::SERIAL_SERVICE);
        AutoPtr<IISerialManager> mgr = IISerialManager::Probe(service);
        AutoPtr<ISerialManager> serialManager;
        assert(0 && "TODO");
        // CSerialManager::New(this, mgr , (ISerialManager**)&serialManager);
        *object = serialManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::VIBRATOR_SERVICE.Equals(name)) {
        AutoPtr<IVibrator> service;
        CSystemVibrator::New(this, (IVibrator**)&service);
        *object = service.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::WALLPAPER_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        *object = GetWallpaperManager().Get();
        mServiceCache[name] = GetWallpaperManager().Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::WIFI_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::WIFI_SERVICE);
        AutoPtr<IIWifiManager> service = IIWifiManager::Probe(b);
        AutoPtr<IWifiManager> wifiManager;
        CWifiManager::New(GetOuterContext(), service, (IWifiManager**)&wifiManager);
        *object = wifiManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::WIFI_P2P_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::WIFI_P2P_SERVICE);
        AutoPtr<IIWifiP2pManager> service = IIWifiP2pManager::Probe(b);
        AutoPtr<IWifiP2pManager> wifiP2pManager;
        CWifiP2pManager::New(service, (IWifiP2pManager**)&wifiP2pManager);
        *object = wifiP2pManager.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::WIFI_SCANNING_SERVICE.Equals(name)) {
        assert(0 && "TODO");
//     public Object createService(ContextImpl ctx) {
//         IBinder b = ServiceManager.getService(WIFI_SCANNING_SERVICE);
//         IWifiScanner service = IWifiScanner.Stub.asInterface(b);
//         return new WifiScanner(ctx.getOuterContext(), service);
//     }});
        return NOERROR;
    }
    else if (IContext::WIFI_RTT_SERVICE.Equals(name)) {
        assert(0 && "TODO");
//     public Object createService(ContextImpl ctx) {
//         IBinder b = ServiceManager.getService(WIFI_RTT_SERVICE);
//         IRttManager service = IRttManager.Stub.asInterface(b);
//         return new RttManager(ctx.getOuterContext(), service);
//     }});
        return NOERROR;
    }
    else if (IContext::ETHERNET_SERVICE.Equals(name)) {
        assert(0 && "TODO");
        // registerService(ETHERNET_SERVICE, new ServiceFetcher() {
        //         public Object createService(ContextImpl ctx) {
        //             IBinder b = ServiceManager.getService(ETHERNET_SERVICE);
        //             IEthernetManager service = IEthernetManager.Stub.asInterface(b);
        //             return new EthernetManager(ctx.getOuterContext(), service);
        //         }});
    }
    else if (IContext::WINDOW_SERVICE.Equals(name)) {
        AutoLock lock(mCacheLock);

        AutoPtr<IDisplay> display = mDisplay;
        if (display == NULL) {
            AutoPtr<IInterface> obj;
            GetOuterContext()->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj);
            IDisplayManager* dm = IDisplayManager::Probe(obj);
            dm->GetDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&display);
        }
        AutoPtr<WindowManagerImpl> obj = new WindowManagerImpl(display);
        *object = TO_IINTERFACE(obj);
        mServiceCache[name] = *object;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::USER_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::USER_SERVICE);
        AutoPtr<IIUserManager> service = IIUserManager::Probe(b);
        AutoPtr<IUserManager> userMgr;
        CUserManager::New(this, service, (IUserManager**)&userMgr);
        *object = userMgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::APP_OPS_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::APP_OPS_SERVICE);
        AutoPtr<IIAppOpsService> service = IIAppOpsService::Probe(b);
        AutoPtr<IAppOpsManager> mgr;
        CAppOpsManager::New(this, service, (IAppOpsManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::CAMERA_SERVICE.Equals(name)) {
        AutoPtr<ICameraManager> mgr;
        CCameraManager::New(this, (ICameraManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::LAUNCHER_APPS_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::LAUNCHER_APPS_SERVICE);
        AutoPtr<IILauncherApps> service = IILauncherApps::Probe(b);
        AutoPtr<ILauncherApps> mgr;
        CLauncherApps::New(this, service, (ILauncherApps**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::RESTRICTIONS_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::RESTRICTIONS_SERVICE);
        AutoPtr<IIRestrictionsManager> service = IIRestrictionsManager::Probe(b);
        AutoPtr<IRestrictionsManager> mgr;
        CRestrictionsManager::New(this, service, (IRestrictionsManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::PRINT_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0);
        // AutoPtr<IInterface> b = ServiceManager::GetService(IContext::PRINT_SERVICE);
        // AutoPtr<IIPrintManager> service = IIPrintManager::Probe(b);
        // AutoPtr<IPrintManager> mgr;
        // CPrintManager::New(GetOuterContext(), service, UserHandle::GetMyUserId(),
        //     UserHandle::GetAppId(Process::MyUid()), (IPrintManager**)&mgr);
        // *object = mgr.Get();
        // REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::CONSUMER_IR_SERVICE.Equals(name)) {
        AutoPtr<IConsumerIrManager> mgr;
        CConsumerIrManager::New(this, (IConsumerIrManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::MEDIA_SESSION_SERVICE.Equals(name)) {
        AutoPtr<IMediaSessionManager> mgr;
        CMediaSessionManager::New(this, (IMediaSessionManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TRUST_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::TRUST_SERVICE);
        AutoPtr<IBinder> service = IBinder::Probe(b);
        AutoPtr<ITrustManager> mgr;
        CTrustManager::New(service, (ITrustManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::FINGERPRINT_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        // assert(0);
    //     AutoPtr<IInterface> b = ServiceManager::GetService(IContext::FINGERPRINT_SERVICE);
    //     AutoPtr<IIFingerprintService> IIFingerprintService = IBinder::Probe(b);
    //     AutoPtr<IFingerprintManager> mgr;
    //     CFingerprintManager::New(GetOuterContext(), service, (IFingerprintManager**)&mgr);
    //     *object = mgr.Get();
    //     REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TV_INPUT_SERVICE.Equals(name)) {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0);
        // public Object createService(ContextImpl ctx) {
        //     IBinder iBinder = ServiceManager.getService(TV_INPUT_SERVICE);
        //     ITvInputManager service = ITvInputManager.Stub.asInterface(iBinder);
        //     return new TvInputManager(service, UserHandle.myUserId());
        // }
        return NOERROR;
    }
    else if (IContext::NETWORK_SCORE_SERVICE.Equals(name)) {
        AutoPtr<INetworkScoreManager> mgr;
        CNetworkScoreManager::New(this, (INetworkScoreManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::USAGE_STATS_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::USAGE_STATS_SERVICE);
        AutoPtr<IIUsageStatsManager> service = IIUsageStatsManager::Probe(b);
        AutoPtr<IUsageStatsManager> mgr;
        CUsageStatsManager::New(GetOuterContext(), service, (IUsageStatsManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::JOB_SCHEDULER_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::JOB_SCHEDULER_SERVICE);
        AutoPtr<IIJobScheduler> service = IIJobScheduler::Probe(b);
        AutoPtr<IJobScheduler> mgr;
        CJobSchedulerImpl::New(service, (IJobScheduler**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::PERSISTENT_DATA_BLOCK_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::PERSISTENT_DATA_BLOCK_SERVICE);
        AutoPtr<IIPersistentDataBlockService> service = IIPersistentDataBlockService::Probe(b);
        if (service != NULL) {
            AutoPtr<IPersistentDataBlockManager> mgr;
            CPersistentDataBlockManager::New(service, (IPersistentDataBlockManager**)&mgr);
            *object = mgr.Get();
            REFCOUNT_ADD(*object);
        }
        else {
            // not supported
            *object = NULL;
        }
        return NOERROR;
    }
    else if (IContext::MEDIA_PROJECTION_SERVICE.Equals(name)) {
        AutoPtr<IMediaProjectionManager> mgr;
        CMediaProjectionManager::New(this, (IMediaProjectionManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::APPWIDGET_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::APPWIDGET_SERVICE);
        AutoPtr<IIAppWidgetService> service = IIAppWidgetService::Probe(b);
        AutoPtr<IAppWidgetManager> mgr;
        CAppWidgetManager::New(this, service, (IAppWidgetManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::PROFILE_SERVICE.Equals(name)) {
        AutoPtr<IProfileManager> mgr;
        CProfileManager::New(GetOuterContext(),
                mMainThread->GetHandler(), (IProfileManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::THEME_SERVICE.Equals(name)) {
        AutoPtr<IInterface> b = ServiceManager::GetService(IContext::THEME_SERVICE);
        AutoPtr<IIThemeService> service = IIThemeService::Probe(b);
        AutoPtr<IThemeManager> mgr;
        CThemeManager::New(GetOuterContext(), service, (IThemeManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::TORCH_SERVICE.Equals(name)) {
        assert(0 && "TODO");
        // AutoPtr<IInterface> b = ServiceManager::GetService(IContext::TORCH_SERVICE);
        // AutoPtr<IITorchService> service = IITorchService::Probe(b);
        // AutoPtr<ITorchManager> mgr;
        // CTorchManager::New(GetOuterContext(), service, (ITorchManager**)&mgr);
        // *object = mgr.Get();
        // REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else if (IContext::CMHW_SERVICE.Equals(name)) {
        AutoPtr<ICmHardwareManager> mgr;
        CCmHardwareManager::New(this, (ICmHardwareManager**)&mgr);
        *object = mgr.Get();
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    else {
        Slogger::E(TAG, " >>> TODO: Service %s is not ready!", name.string());
        assert(0 && "TODO");
    }
    return NOERROR;
}

AutoPtr<IWallpaperManager> CContextImpl::GetWallpaperManager()
{
    if (sWallpaperManager == NULL) {
        CWallpaperManager::New(GetOuterContext(), mMainThread->GetHandler(),
            (IWallpaperManager**)&sWallpaperManager);
    }

    return sWallpaperManager;
}

// AutoPtr<IDropBoxManager> CreateDropBoxManager()
// {
//     IBinder b = ServiceManager.getService(DROPBOX_SERVICE);
//     IDropBoxManagerService service = IDropBoxManagerService.Stub.asInterface(b);
//     if (service == NULL) {
//         // Don't return a DropBoxManager that will NPE upon use.
//         // This also avoids caching a broken DropBoxManager in
//         // getDropBoxManager during early boot, before the
//         // DROPBOX_SERVICE is registered.
//         return NULL;
//     }
//     return new DropBoxManager(service);
// }

ECode CContextImpl::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPackageManager::PERMISSION_DENIED;

    if (permission.IsNull()) {
        Logger::E(TAG, "permission is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

//    try {
    ECode ec = NOERROR;
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if (am != NULL) {
        ec = am->CheckPermission(permission, pid, uid, result);
        if (FAILED(ec)) {
            *result = IPackageManager::PERMISSION_DENIED;
            Logger::W(TAG, " CheckPermission for %s, pid %d uid %d, DENIED!",
                permission.string(), pid, uid);
        }
    }
    return ec;
//    } catch (RemoteException e) {
//        return PackageManager.PERMISSION_DENIED;
//    }
}

ECode CContextImpl::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = IPackageManager::PERMISSION_DENIED;

    if (permission.IsNull()) {
        Logger::E(TAG, "permission is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 pid = Binder::GetCallingPid();
    if (pid != Process::MyPid()) {
        return CheckPermission(permission, pid, Binder::GetCallingUid(), value);
    }
    *value = IPackageManager::PERMISSION_DENIED;
    return NOERROR;
}

ECode CContextImpl::CheckCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* perm)
{
    VALIDATE_NOT_NULL(perm);
    *perm = IPackageManager::PERMISSION_DENIED;

    if (permission.IsNull()) {
        Logger::E(TAG, "permission is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return CheckPermission(permission, Binder::GetCallingPid(),
            Binder::GetCallingUid(), perm);
}

ECode CContextImpl::Enforce(
    /* [in] */ const String& permission,
    /* [in] */ Int32 resultOfCheck,
    /* [in] */ Boolean selfToo,
    /* [in] */ Int32 uid,
    /* [in] */ const String& message)
{
    if (resultOfCheck != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("");
        if (!message.IsNull()) {
            sb += message;
            sb += ": ";
        }
        if (selfToo) {
            sb += "Neither user ";
            sb += uid;
            sb += " nor current process has ";
        }
        else {
            sb += "uid ";
            sb += uid;
            sb += " does not have ";
        }
        sb += permission;
        sb += ".";
        Slogger::E(TAG, sb.ToString());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CContextImpl::EnforcePermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& message)
{
    Int32 res;
    FAIL_RETURN(CheckPermission(permission, pid, uid, &res));
    return Enforce(permission,
            res,
            FALSE,
            uid,
            message);
}

ECode CContextImpl::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    Int32 res;
    FAIL_RETURN(CheckCallingPermission(permission, &res));
    return Enforce(permission,
            res,
            false,
            Binder::GetCallingUid(),
            message);
}

ECode CContextImpl::EnforceCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    Int32 res;
    FAIL_RETURN(CheckCallingOrSelfPermission(permission, &res));
    return Enforce(permission,
            res,
            TRUE,
            Binder::GetCallingUid(),
            message);
}

ECode CContextImpl::GrantUriPermission(
    /* [in] */ const String& toPackage,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
//     try {
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IUri> tmp;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&tmp);
    return ActivityManagerNative::GetDefault()->GrantUriPermission(
        appThread, toPackage, tmp, modeFlags, ResolveUserId(uri));
//    } catch (RemoteException e) {
//    }
}

ECode CContextImpl::RevokeUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
//     try {
    AutoPtr<IApplicationThread> appThread;
    mMainThread->GetApplicationThread((IApplicationThread**)&appThread);
    AutoPtr<IUri> tmp;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&tmp);
    return ActivityManagerNative::GetDefault()->RevokeUriPermission(appThread,
        tmp, modeFlags, ResolveUserId(uri));
//    } catch (RemoteException e) {
//    }
}

ECode CContextImpl::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
//    try {
    AutoPtr<IUri> tmp;
    ContentProvider::GetUriWithoutUserId(uri, (IUri**)&tmp);
    return ActivityManagerNative::GetDefault()->CheckUriPermission(
        tmp, pid, uid, modeFlags, ResolveUserId(uri), result);
//    } catch (RemoteException e) {
//        return PackageManager.PERMISSION_DENIED;
//    }
}

Int32 CContextImpl::ResolveUserId(
    /* [in] */ IUri* uri)
{
    Int32 id;
    GetUserId(&id);
    return ContentProvider::GetUserIdFromUri(uri, id);
}

ECode CContextImpl::CheckCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 pid = Binder::GetCallingPid();
    if (pid != Process::MyPid()) {
        return CheckUriPermission(uri, pid,
                Binder::GetCallingUid(), modeFlags, result);
    }
    *result = IPackageManager::PERMISSION_DENIED;
    return NOERROR;
}

ECode CContextImpl::CheckCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
    return CheckUriPermission(uri, Binder::GetCallingPid(),
            Binder::GetCallingUid(), modeFlags, result);
}

ECode CContextImpl::CheckUriPermission(
    /* [in] */ IUri * uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Slogger::I("foo", "checkUriPermission: uri=%preadPermission=%s writePermission=%s pid=%d uid=%d mode%d"
            , uri, readPermission.string(), writePermission.string()
            , pid, uid, modeFlags);
    }
    Int32 checked;
    CheckPermission(readPermission, pid, uid, &checked);
    if ((modeFlags&IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
        if (readPermission == NULL
                || checked == IPackageManager::PERMISSION_GRANTED) {
            *result = IPackageManager::PERMISSION_GRANTED;
            return NOERROR;
        }
    }
    Int32 checked2;
    CheckPermission(writePermission, pid, uid, &checked2);
    if ((modeFlags&IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
        if (writePermission == NULL
                || checked2 == IPackageManager::PERMISSION_GRANTED) {
            *result = IPackageManager::PERMISSION_GRANTED;
            return NOERROR;
        }
    }
    Int32 checkedUri;
    CheckUriPermission(uri, pid, uid, modeFlags, &checkedUri);
    *result = uri != NULL ? checkedUri : IPackageManager::PERMISSION_DENIED;
    return NOERROR;
}

ECode CContextImpl::UriModeFlagToString(
    /* [in] */ Int32 uriModeFlags,
    /* [out] */ String* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = String(NULL);

    StringBuilder builder;
    if ((uriModeFlags & IIntent::FLAG_GRANT_READ_URI_PERMISSION) != 0) {
        builder += "read and ";
    }
    if ((uriModeFlags & IIntent::FLAG_GRANT_WRITE_URI_PERMISSION) != 0) {
        builder += "write and ";
    }
    if ((uriModeFlags & IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION) != 0) {
        builder += "persistable and ";
    }
    if ((uriModeFlags & IIntent::FLAG_GRANT_PREFIX_URI_PERMISSION) != 0) {
        builder += "prefix and ";
    }

    if (builder.GetLength() > 5) {
        builder.SetLength(builder.GetLength() - 5);
        *mode = builder.ToString();
        return NOERROR;
    }

    Logger::E(TAG, "Unknown permission mode flags: %d", uriModeFlags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CContextImpl::EnforceForUri(
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 resultOfCheck,
    /* [in] */ Boolean selfToo,
    /* [in] */ Int32 uid,
    /* [in] */ IUri* uri,
    /* [in] */ const String& message)
{
    if (resultOfCheck != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder sb("");
        if (message != NULL) sb += (message + ": ");
        if (selfToo) {
            sb += "Neither user ";
            sb += uid;
            sb += " nor current process has ";
        }
        else {
            sb += "User ";
            sb += uid;
            sb += " does not have ";
        }
        String mode;
        UriModeFlagToString(modeFlags, &mode);
        sb += mode;
        sb += " permission on ";
        sb += uri;
        sb += ".";
        Slogger::E(TAG, sb.ToString());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CContextImpl::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    Int32 checked;
    CheckUriPermission(uri, pid, uid, modeFlags, &checked);
    return EnforceForUri(
            modeFlags, checked,
            FALSE, uid, uri, message);
}

ECode CContextImpl::EnforceCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    Int32 checked;
    CheckCallingUriPermission(uri, modeFlags, &checked);
    return EnforceForUri(
            modeFlags, checked,
            FALSE,
            Binder::GetCallingUid(), uri, message);
}

ECode CContextImpl::EnforceCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    Int32 checked;
    CheckCallingOrSelfUriPermission(uri, modeFlags, &checked);
    return EnforceForUri(
            modeFlags,
            checked, TRUE,
            Binder::GetCallingUid(), uri, message);
}

ECode CContextImpl::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    Int32 checked;
    CheckUriPermission(
            uri, readPermission, writePermission, pid, uid,
            modeFlags, &checked);
    return EnforceForUri(modeFlags, checked, FALSE, uid, uri, message);
}

void CContextImpl::WarnIfCallingFromSystemProcess()
{
    if (Process::MyUid() == IProcess::SYSTEM_UID) {
        Slogger::W(TAG, "Calling a method in the system process without a qualified user: "
                /*+ Debug.getCallers(5)*/);
    }
}

ECode CContextImpl::CreateApplicationContext(
    /* [in] */ IApplicationInfo* application,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    return CreateApplicationContext(application, String(NULL), flags, ctx);
}

ECode CContextImpl::CreateApplicationContext(
    /* [in] */ IApplicationInfo* application,
    /* [in] */ const String& themePackageName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = NULL;
    AutoPtr<ICompatibilityInfo> ci;
    mResources->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
    AutoPtr<ILoadedPkg> pi;
    mMainThread->GetPackageInfo(application, ci,
        flags | IContext::CONTEXT_REGISTER_PACKAGE, (ILoadedPkg**)&pi);
    if (pi != NULL) {
        Boolean restricted = (flags & IContext::CONTEXT_RESTRICTED) == IContext::CONTEXT_RESTRICTED;
        Int32 uid;
        application->GetUid(&uid);
        Int32 userId = UserHandle::GetUserId(uid);
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        AutoPtr<IContextImpl> c;
        CContextImpl::New(this, mMainThread, pi, mActivityToken,
            uh, restricted,
            mDisplay, mOverrideConfiguration, themePackageName, (IContextImpl**)&c);
        if (((CContextImpl*)c.Get())->mResources != NULL) {
            *ctx = IContext::Probe(c);
            REFCOUNT_ADD(*ctx)
            return NOERROR;
        }
    }

    String pkgName;
    IPackageItemInfo::Probe(application)->GetPackageName(&pkgName);
    Logger::E(TAG, "NameNotFoundException: Application package %s not found", pkgName.string());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode CContextImpl::CreatePackageContext(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    return CreatePackageContextAsUser(packageName, String(NULL), flags,
            mUser != NULL ? mUser : userHandle, ctx);
}

ECode CContextImpl::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    return CreatePackageContextAsUser(packageName, String(NULL), flags, user, ctx);
}

ECode CContextImpl::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ const String& themePackageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = NULL;

    Boolean restricted = (flags & IContext::CONTEXT_RESTRICTED) == IContext::CONTEXT_RESTRICTED;
    if (packageName.Equals("system") || packageName.Equals("android")) {
        return CContextImpl::New(this, mMainThread, mPackageInfo, mActivityToken,
            user, restricted, mDisplay, mOverrideConfiguration, themePackageName, ctx);
    }

    AutoPtr<ICompatibilityInfo> ci;
    mResources->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
    Int32 uid;
    user->GetIdentifier(&uid);
    AutoPtr<ILoadedPkg> pi;
    mMainThread->GetPackageInfo(packageName, ci,
        flags | IContext::CONTEXT_REGISTER_PACKAGE, uid, (ILoadedPkg**)&pi);
    if (pi != NULL) {
        AutoPtr<CContextImpl> c;
        CContextImpl::NewByFriend(this, mMainThread, pi, mActivityToken,
            user, restricted, mDisplay, mOverrideConfiguration, themePackageName, (CContextImpl**)&c);
        if (c->mResources != NULL) {
            *ctx = IContext::Probe(c);
            REFCOUNT_ADD(*ctx)
            return NOERROR;
        }
    }

    // Should be a better exception.
    Logger::E(TAG, "NameNotFoundException : Application package %s not found",
        packageName.string());
    return E_NAME_NOT_FOUND_EXCEPTION;
}

ECode CContextImpl::CreateConfigurationContext(
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = NULL;

    if (overrideConfiguration == NULL) {
        Logger::E(TAG, "overrideConfiguration must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContextImpl> c;
    CContextImpl::New(this, mMainThread, mPackageInfo, mActivityToken,
        mUser, mRestricted, mDisplay, overrideConfiguration, String(NULL), (IContextImpl**)&c);

    *ctx = IContext::Probe(c);
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CContextImpl::CreateDisplayContext(
    /* [in] */ IDisplay* display,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = NULL;

    if (display == NULL) {
        Logger::E(TAG, "display must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContextImpl> c;
    CContextImpl::New(this, mMainThread, mPackageInfo, mActivityToken,
        mUser, mRestricted, display, mOverrideConfiguration, String(NULL), (IContextImpl**)&c);

    *ctx = IContext::Probe(c);
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

Int32 CContextImpl::GetDisplayId()
{
    Int32 displayId = IDisplay::DEFAULT_DISPLAY;
    if (mDisplay != NULL) {
        mDisplay->GetDisplayId(&displayId);
    }

    return displayId;
}

ECode CContextImpl::IsRestricted(
    /* [out] */ Boolean* isRestricted)
{
    VALIDATE_NOT_NULL(isRestricted);

    *isRestricted = mRestricted;
    return NOERROR;
}

ECode CContextImpl::GetDisplayAdjustments(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayAdjustments** das)
{
    VALIDATE_NOT_NULL(das);
    *das = mDisplayAdjustments;
    REFCOUNT_ADD(*das)
    return NOERROR;
}

AutoPtr<CContextImpl> CContextImpl::CreateSystemContext(
    /* [in] */ IActivityThread* mainThread)
{
    assert(mainThread != NULL);
    AutoPtr<LoadedPkg> packageInfo = new LoadedPkg();
    packageInfo->constructor(mainThread);

    AutoPtr<CContextImpl> ci;
    CContextImpl::NewByFriend(NULL, mainThread,
        (ILoadedPkg*)packageInfo.Get(), NULL, NULL, FALSE, NULL, NULL, String(NULL), (CContextImpl**)&ci);

    AutoPtr<IConfiguration> config;
    ci->mResourcesManager->GetConfiguration((IConfiguration**)&config);
    AutoPtr<IDisplayMetrics> dm;
    ci->mResourcesManager->GetDisplayMetricsLocked(IDisplay::DEFAULT_DISPLAY, (IDisplayMetrics**)&dm);
    ci->mResources->UpdateConfiguration(config, dm);
    return ci;
}

ECode CContextImpl::CreateAppContext(
    /* [in] */ IActivityThread* mainThread,
    /* [in] */ ILoadedPkg* packageInfo,
    /* [out] */ IContextImpl** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (packageInfo == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return CContextImpl::New(NULL, mainThread,
        packageInfo, NULL, NULL, FALSE, NULL, NULL, String(NULL), result);
}

ECode CContextImpl::CreateActivityContext(
    /* [in] */ IActivityThread* mainThread,
    /* [in] */ ILoadedPkg* packageInfo,
    /* [in] */ IBinder* activityToken,
    /* [out] */ IContextImpl** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (packageInfo == NULL || activityToken == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return CContextImpl::New(NULL, mainThread,
        packageInfo, activityToken, NULL, FALSE, NULL, NULL, String(NULL), result);
}

ECode CContextImpl::constructor(
    /* [in]*/ IContextImpl* container,
    /* [in] */ IActivityThread* mainThread,
    /* [in] */ ILoadedPkg* packageInfo,
    /* [in] */ IBinder* activityToken,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean restricted,
    /* [in] */ IDisplay* display,
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [in] */ const String& themePackageName)
{
    assert(mainThread != NULL);

    GetWeakReference((IWeakReference**)&mOuterContext);

    mMainThread = (CActivityThread*)mainThread;
    mActivityToken = activityToken;
    mRestricted = restricted;

    mUser = user;
    if (mUser == NULL) {
        Process::MyUserHandle((IUserHandle**)&mUser);
    }

    mPackageInfo = (LoadedPkg*)packageInfo;
    mResourcesManager = CResourcesManager::GetInstance();
    mDisplay = display;
    mOverrideConfiguration = overrideConfiguration;

    mContentResolver = new ApplicationContentResolver();
    mContentResolver->constructor(this, mMainThread, mUser);

    IContext* ctxContainer = IContext::Probe(container);
    Int32 displayId = GetDisplayId();
    AutoPtr<ICompatibilityInfo> compatInfo;
    if (container != NULL) {
        AutoPtr<IDisplayAdjustments> da;
        ctxContainer->GetDisplayAdjustments(displayId, (IDisplayAdjustments**)&da);
        da->GetCompatibilityInfo((ICompatibilityInfo**)&compatInfo);
    }
    if (compatInfo == NULL && displayId == IDisplay::DEFAULT_DISPLAY) {
        mPackageInfo->GetCompatibilityInfo((ICompatibilityInfo**)&compatInfo);
    }
    mDisplayAdjustments->SetCompatibilityInfo(compatInfo);
    mDisplayAdjustments->SetActivityToken(activityToken);

    AutoPtr<IResources> resources;
    mPackageInfo->GetResources(mainThread, (IResources**)&resources);
    if (resources != NULL) {
        Boolean isCompat = FALSE;
        if (compatInfo != NULL) {
            Float as, cs;
            AutoPtr<ICompatibilityInfo> ci;
            resources->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
            ci->GetApplicationScale(&cs);
            compatInfo->GetApplicationScale(&as);
            isCompat = as != cs;
        }

        Boolean get = (activityToken != NULL || !themePackageName.IsNull()
            || displayId != IDisplay::DEFAULT_DISPLAY
            || overrideConfiguration != NULL || isCompat);
        if (get) {
            String resDir;
            mPackageInfo->GetResDir(&resDir);
            if (themePackageName.IsNull()) {
                AutoPtr< ArrayOf<String> > splitResDirs, overlayDirs, sharedLibraryFiles;
                mPackageInfo->GetSplitResDirs((ArrayOf<String>**)&splitResDirs);
                mPackageInfo->GetOverlayDirs((ArrayOf<String>**)&overlayDirs);
                AutoPtr<IApplicationInfo> ai;
                mPackageInfo->GetApplicationInfo((IApplicationInfo**)&ai);
                ai->GetSharedLibraryFiles((ArrayOf<String>**)&sharedLibraryFiles);
                String appDir;
                mPackageInfo->GetAppDir(&appDir);
                AutoPtr<IContext> ctx;
                if (mOuterContext != NULL) {
                    mOuterContext->Resolve(EIID_IContext, (IInterface**)&ctx);
                }

                resources = NULL;
                mResourcesManager->GetTopLevelResources(
                    resDir, splitResDirs, overlayDirs, sharedLibraryFiles, displayId,
                    appDir, overrideConfiguration, compatInfo, activityToken,
                    ctx, (IResources**)&resources);
            }
            else {
                String packageName;
                mPackageInfo->GetPackageName(&packageName);
                resources = NULL;
                mResourcesManager->GetTopLevelThemedResources(resDir, displayId,
                    packageName, themePackageName, compatInfo ,activityToken, (IResources**)&resources);
            }
        }
    }

    mResources = resources;

    if (container != NULL) {
        ctxContainer->GetBasePackageName(&mBasePackageName);
        ctxContainer->GetOpPackageName(&mOpPackageName);
    }
    else {
        mPackageInfo->GetPackageName(&mBasePackageName);
        AutoPtr<IApplicationInfo> ainfo;
        mPackageInfo->GetApplicationInfo((IApplicationInfo**)&ainfo);
        Int32 uid;
        ainfo->GetUid(&uid);
        if (uid == IProcess::SYSTEM_UID && uid != Process::MyUid()) {
            // Special case: system components allow themselves to be loaded in to other
            // processes.  For purposes of app ops, we must then consider the context as
            // belonging to the package of this process, not the system itself, otherwise
            // the package+uid verifications in app ops will fail.
            mOpPackageName = CActivityThread::GetCurrentPackageName();
        }
        else {
            mOpPackageName = mBasePackageName;
        }
    }

    return NOERROR;
}

ECode CContextImpl::InstallSystemApplicationInfo(
    /* [in] */ IApplicationInfo* info,
    /* [in] */ IClassLoader* classLoader)
{
    return mPackageInfo->InstallSystemApplicationInfo(info, classLoader);;
}

ECode CContextImpl::ScheduleFinalCleanup(
    /* [in] */ const String& who,
    /* [in] */ const String& what)
{
    return mMainThread->ScheduleContextCleanup(this, who, what);
}

ECode CContextImpl::PerformFinalCleanup(
    /* [in] */ const String& who,
    /* [in] */ const String& what)
{
    //Logger::I(TAG, "Cleanup up context: %p, %s, %s", this, who.string(), what.string());
    AutoPtr<IContext> ctx = GetOuterContext();
    return mPackageInfo->RemoveContextRegistrations(ctx, who, what);
}

ECode CContextImpl::GetReceiverRestrictedContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = NULL;

    if (mReceiverRestrictedContext != NULL) {
        *ctx = mReceiverRestrictedContext;
        REFCOUNT_ADD(*ctx);
        return NOERROR;
    }
    AutoPtr<IContext> outCtx = GetOuterContext();
    CReceiverRestrictedContext::New(outCtx, (IContext**)&mReceiverRestrictedContext);
    *ctx = mReceiverRestrictedContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
 }

void CContextImpl::SetOuterContext(
    /* [in] */ IContext* context)
{
    mOuterContext = NULL;
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(context);
    assert(wrs != NULL);
    wrs->GetWeakReference((IWeakReference**)&mOuterContext);
}

AutoPtr<IContext> CContextImpl::GetOuterContext()
{
    AutoPtr<IContext> context;
    if (mOuterContext != NULL) {
        mOuterContext->Resolve(EIID_IContext, (IInterface**)&context);
    }
    return context;
}

AutoPtr<IBinder> CContextImpl::GetActivityToken()
{
    return mActivityToken;
}

void CContextImpl::SetFilePermissionsFromMode(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 extraPermissions)
{
    Int32 perms = FileUtils::sS_IRUSR|FileUtils::sS_IWUSR
        |FileUtils::sS_IRGRP|FileUtils::sS_IWGRP
        |extraPermissions;
    if ((mode&IContext::MODE_WORLD_READABLE) != 0) {
        perms |= FileUtils::sS_IROTH;
    }
    if ((mode&IContext::MODE_WORLD_WRITEABLE) != 0) {
        perms |= FileUtils::sS_IWOTH;
    }
    if (DEBUG) {
        Slogger::I(TAG, "File %s: mode=0x%x, perms=0x%x", name.string(), mode, perms);
    }
    FileUtils::SetPermissions(name, perms, -1, -1);
}

ECode CContextImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CContextImpl{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " BasePackageName=";
    sb += mBasePackageName;
    sb += " OpPackageName=";
    sb += mOpPackageName;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

AutoPtr<IFile> CContextImpl::ValidateFilePath(
    /* [in] */ const String& name,
    /* [in] */ Boolean createDirectory)
{
    AutoPtr<IFile> dir, f;
    Char32 separatorChar;
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    helper->GetSeparatorChar(&separatorChar);

    if (name.GetChar(0) == separatorChar) {
        String dirPath = name.Substring(0, name.LastIndexOf(separatorChar));
        CFile::New(dirPath, (IFile**)&dir);
        String _name = name.Substring(name.LastIndexOf(separatorChar));
        CFile::New(dir, _name, (IFile**)&f);
    }
    else {
        dir = GetDatabasesDir();
        MakeFilename(dir, name, (IFile**)&f);
    }

    Boolean isDir, isMk;
    if (createDirectory && (dir->IsDirectory(&isDir), !isDir) && (dir->Mkdir(&isMk), isMk)) {
        String path;
        dir->GetPath(&path);
        FileUtils::SetPermissions(path,
            FileUtils::sS_IRWXU|FileUtils::sS_IRWXG|FileUtils::sS_IXOTH,
            -1, -1);
    }

    return f;
}

ECode CContextImpl::MakeFilename(
    /* [in] */ IFile* base,
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file);
    *file = NULL;

    Char32 separatorChar;
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    helper->GetSeparatorChar(&separatorChar);
    if (name.IndexOf(separatorChar) < 0) {
        return CFile::New(base, name, file);
    }
    //throw new IllegalArgumentException(
    //        "File " + name + " contains a path separator");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

AutoPtr<ArrayOf<IFile*> > CContextImpl::EnsureDirsExistOrFilter(
    /* [in] */ ArrayOf<IFile*>* dirs)
{
    AutoPtr<ArrayOf<IFile*> > result = ArrayOf<IFile*>::Alloc(dirs->GetLength());
    String packageName;
    GetPackageName(&packageName);
    Boolean bval;
    for (Int32 i = 0; i < dirs->GetLength(); i++) {
        AutoPtr<IFile> dir = (*dirs)[i];
        if (IEnvironment::MEDIA_REMOVED.Equals(Environment::GetStorageState(dir))) {
            continue;
        }

        dir->Exists(&bval);
        if (!bval) {
            dir->Mkdirs(&bval);
            if (!bval) {
                dir->Exists(&bval);
                // recheck existence in case of cross-process race
                if (!bval) {
                    // Failing to mkdir() may be okay, since we might not have
                    // enough permissions; ask vold to create on our behalf.
                    AutoPtr<IInterface> obj = ServiceManager::GetService(String("mount"));
                    AutoPtr<IIMountService> mount = IIMountService::Probe(obj);
                    Int32 res = -1;
                    String path;
                    dir->GetAbsolutePath(&path);
                    mount->Mkdirs(packageName, path, &res);
                    if (res != 0) {
                        String path;
                        dir->GetPath(&path);
                        Logger::W(TAG, "Failed to ensure directory: %s", path.string());
                        dir = NULL;
                    }
                }
            }
        }
        result->Set(i, dir);
    }
    return result;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
