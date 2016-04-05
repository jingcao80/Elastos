
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ LauncherApplication* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);
}

ECode MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    // If the database has ever changed, then we really need to force a reload of the
    // workspace on the next load
    mHost->mModel->ResetLoadedState(FALSE, TRUE);
    return mHost->mModel->StartLoaderFromBackground();
}

CAR_INTERFACE_IMPL(LauncherApplication, Application, ILauncherApplication);

const String LauncherApplication::TAG("LauncherApplication");
Boolean LauncherApplication::sIsScreenLarge = FALSE;
Float LauncherApplication::sScreenDensity = 0.0f;
Int32 LauncherApplication::sLongPressTimeout = 300;
const String LauncherApplication::sSharedPreferencesKey("com.android.launcher2.prefs");

LauncherApplication::LauncherApplication()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mFavoritesObserver = new MyContentObserver(handler);
}

ECode LauncherApplication::OnCreate();
{
    Application::OnCreate();

    // set sIsScreenXLarge and sScreenDensity *before* creating icon cache
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetBoolean(Elastos::Droid::Launcher2::R::bool::is_large_screen,
            &sIsScreenLarge);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensity(&sScreenDensity);

    RecreateWidgetPreviewDb();
    mIconCache = new IconCache(this);
    mModel = new LauncherModel(this, mIconCache);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    AutoPtr<ILauncherApps> launcherApps = ILauncherApps::Probe(obj);
    AutoPtr<ILauncherAppsCallback> _callback;
    mModel->GetLauncherAppsCallback((ILauncherAppsCallback**)&_callback);
    launcherApps->RegisterCallback(_callback);

    // Register intent receivers
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    RegisterReceiver(mModel, filter);
    AutoPtr<IIntentFilter> filter2;
    CIntentFilter::New((IIntentFilter**)&filter2);
    filter2->AddAction(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED);
    registerReceiver(mModel, filter2);
    AutoPtr<IIntentFilter> filter3;
    CIntentFilter::New((IIntentFilter**)&filter3);
    filter3->AddAction(ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED);
    RegisterReceiver(mModel, filter3);

    // Register for changes to the favorites
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    return resolver->RegisterContentObserver(ILauncherSettings::Favorites::CONTENT_URI, TRUE,
            mFavoritesObserver);
}

ECode LauncherApplication::RecreateWidgetPreviewDb()
{
    mWidgetPreviewCacheDb = new WidgetPreviewLoader::CacheDb(this);
    return NOERROR;
}

ECode LauncherApplication::OnTerminate()
{
    Application::OnTerminate();

    UnregisterReceiver(mModel);

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    return resolver->UnregisterContentObserver(mFavoritesObserver);
}

ECode LauncherApplication::SetLauncher(
    /* [in] */ ILauncher* launcher,
    /* [out] */ LauncherModel** mode)
{
    VALIDATR_NOT_NULL(mode);

    mModel->Initialize(launcher);
    *mode = mModel;
    REFCOUNT_ADD(*mode);
    return NOERROR;
}

ECode LauncherApplication::GetIconCache(
    /* [out] */ IconCache** cache)
{
    VALIDATR_NOT_NULL(cache);

    *cache = mIconCache;
    REFCOUNT_ADD(*cache);
    return NOERROR;
}

ECode LauncherApplication::GetModel(
    /* [out] */ LauncherModel** mode)
{
    VALIDATR_NOT_NULL(mode);

    *mode = mModel;
    REFCOUNT_ADD(*mode);
    return NOERROR;
}

ECode LauncherApplication::GetWidgetPreviewCacheDb(
    /* [out] */ WidgetPreviewLoader::CacheDb** db)
{
    VALIDATR_NOT_NULL(db);

    *db = mWidgetPreviewCacheDb;
    REFCOUNT_ADD(*db);
    return NOERROR;
}

ECode LauncherApplication::SetLauncherProvider(
    /* [in] */ ILauncherProvider* provider)
{
    return IWeakReferenceSource::Probe(provider)->GetWeakReference((IWeakReference**)&mLauncherProvider);
}

ECode LauncherApplication::GetLauncherProvider(
    /* [out] */ ILauncherProvider** provider)
{
    VALIDATR_NOT_NULL(provider);

    AutoPtr<IInterface> obj;
    mLauncherProvider->Resolve(EIID_ILauncherProvider, (IInterface**)&obj);
    *provider = ILauncherProvider::Probe(obj);
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode LauncherApplication::GetSharedPreferencesKey(
    /* [out] */ String* key)
{
    VALIDATR_NOT_NULL(key);

    *key = sSharedPreferencesKey;
    return NOERROR;
}

ECode LauncherApplication::IsScreenLarge(
    /* [out] */ Boolean* result)
{
    VALIDATR_NOT_NULL(result);

    *result = sIsScreenLarge;
    return NOERROR;
}

ECode LauncherApplication::IsScreenLandscape(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATR_NOT_NULL(result);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);

    *result = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    return NOERROR;
}

ECode LauncherApplication::GetScreenDensity(
    /* [out] */ Float* density)
{
    VALIDATR_NOT_NULL(density);

    *density = sScreenDensity;
    return NOERROR;
}

ECode LauncherApplication::GetLongPressTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATR_NOT_NULL(timeout);

    *timeout = sLongPressTimeout;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos