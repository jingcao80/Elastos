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

#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/WidgetPreviewLoader.h"
#include "elastos/droid/launcher2/CLauncherModel.h"
#include "elastos/droid/launcher2/CLauncherApplicationContentObserver.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

LauncherApplication::MyContentObserver::MyContentObserver()
{
}

ECode LauncherApplication::MyContentObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ILauncherApplication* host)
{
    mHost = (LauncherApplication*)host;
    return ContentObserver::constructor(handler);
}

ECode LauncherApplication::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    // If the database has ever changed, then we really need to force a reload of the
    // workspace on the next load
    LauncherModel* _mode = (LauncherModel*)(mHost->mModel.Get());
    _mode->ResetLoadedState(FALSE, TRUE);
    return _mode->StartLoaderFromBackground();
}

CAR_INTERFACE_IMPL(LauncherApplication, Application, ILauncherApplication);

const String LauncherApplication::TAG("LauncherApplication");
Boolean LauncherApplication::sIsScreenLarge = FALSE;
Float LauncherApplication::sScreenDensity = 0.0f;
Int32 LauncherApplication::sLongPressTimeout = 300;
const String LauncherApplication::sSharedPreferencesKey("Elastos.Droid.Launcher2.prefs");

LauncherApplication::LauncherApplication()
{
}

ECode LauncherApplication::constructor()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    CLauncherApplicationContentObserver::New(handler, this, (IContentObserver**)&mFavoritesObserver);

    return Application::constructor();
}

ECode LauncherApplication::OnCreate()
{
    Application::OnCreate();

    // set sIsScreenXLarge and sScreenDensity *before* creating icon cache
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetBoolean(Elastos::Droid::Launcher2::R::bool_::is_large_screen,
            &sIsScreenLarge);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensity(&sScreenDensity);

    RecreateWidgetPreviewDb();
    mIconCache = new IconCache();
    mIconCache->constructor(ILauncherApplication::Probe(this));
    CLauncherModel::New(ILauncherApplication::Probe(this), mIconCache, (ILauncherModel**)&mModel);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    mLauncherApps = ILauncherApps::Probe(obj);
    AutoPtr<ILauncherAppsCallback> _callback;
    mModel->GetLauncherAppsCallback((ILauncherAppsCallback**)&_callback);
    mLauncherApps->RegisterCallback(_callback);

    // Register intent receivers
    AutoPtr<IIntentFilter> filter1;
    CIntentFilter::New((IIntentFilter**)&filter1);
    filter1->AddAction(IIntent::ACTION_LOCALE_CHANGED);
    filter1->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    AutoPtr<IIntent> intent1;
    RegisterReceiver(IBroadcastReceiver::Probe(mModel), filter1, (IIntent**)&intent1);

    AutoPtr<IIntentFilter> filter2;
    CIntentFilter::New((IIntentFilter**)&filter2);
    filter2->AddAction(ISearchManager::INTENT_GLOBAL_SEARCH_ACTIVITY_CHANGED);
    AutoPtr<IIntent> intent2;
    RegisterReceiver(IBroadcastReceiver::Probe(mModel), filter2, (IIntent**)&intent2);

    AutoPtr<IIntentFilter> filter3;
    CIntentFilter::New((IIntentFilter**)&filter3);
    filter3->AddAction(ISearchManager::INTENT_ACTION_SEARCHABLES_CHANGED);
    AutoPtr<IIntent> intent3;
    RegisterReceiver(IBroadcastReceiver::Probe(mModel), filter3, (IIntent**)&intent3);

    // Register for changes to the favorites
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    return resolver->RegisterContentObserver(LauncherSettings::Favorites::CONTENT_URI, TRUE,
            mFavoritesObserver);
}

ECode LauncherApplication::RecreateWidgetPreviewDb()
{
    mWidgetPreviewCacheDb = new WidgetPreviewLoader::CacheDb(IContext::Probe(this));
    return NOERROR;
}

ECode LauncherApplication::OnTerminate()
{
    Application::OnTerminate();

    UnregisterReceiver(IBroadcastReceiver::Probe(mModel));

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    return resolver->UnregisterContentObserver(mFavoritesObserver);
}

ECode LauncherApplication::SetLauncher(
    /* [in] */ ILauncher* launcher,
    /* [out] */ ILauncherModel** mode)
{
    VALIDATE_NOT_NULL(mode);

    mModel->Initialize(ILauncherModelCallbacks::Probe(launcher));
    *mode = mModel;
    REFCOUNT_ADD(*mode);
    return NOERROR;
}

ECode LauncherApplication::GetIconCache(
    /* [out] */ IIconCache** cache)
{
    VALIDATE_NOT_NULL(cache);

    *cache = IIconCache::Probe(mIconCache);
    REFCOUNT_ADD(*cache);
    return NOERROR;
}

ECode LauncherApplication::GetModel(
    /* [out] */ ILauncherModel** mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = mModel;
    REFCOUNT_ADD(*mode);
    return NOERROR;
}

ECode LauncherApplication::GetWidgetPreviewCacheDb(
    /* [out] */ IWidgetPreviewLoaderCacheDb** db)
{
    VALIDATE_NOT_NULL(db);

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
    VALIDATE_NOT_NULL(provider);

    AutoPtr<IInterface> obj;
    mLauncherProvider->Resolve(EIID_ILauncherProvider, (IInterface**)&obj);
    *provider = ILauncherProvider::Probe(obj);
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode LauncherApplication::GetSharedPreferencesKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    *key = sSharedPreferencesKey;
    return NOERROR;
}

ECode LauncherApplication::IsScreenLarge(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = sIsScreenLarge;
    return NOERROR;
}

ECode LauncherApplication::IsScreenLandscape(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

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
    VALIDATE_NOT_NULL(density);

    *density = sScreenDensity;
    return NOERROR;
}

ECode LauncherApplication::GetLongPressTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    *timeout = sLongPressTimeout;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos