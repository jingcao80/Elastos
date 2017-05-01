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

#include "elastos/droid/app/CResourcesManager.h"
#include "elastos/droid/app/ApplicationPackageManager.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/CContextImpl.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/CThemeConfig.h"
#include "elastos/droid/content/res/CThemeConfigBuilder.h"
#include "elastos/droid/content/res/ResourcesKey.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"
#include <elastos/core/AutoLock.h>
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::App::CContextImpl;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IThemeUtils;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::CThemeConfig;
using Elastos::Droid::Content::Res::IThemeConfigBuilder;
using Elastos::Droid::Content::Res::CThemeConfigBuilder;
using Elastos::Droid::Content::Res::ResourcesKey;
using Elastos::Droid::Content::Res::EIID_IResources;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::DisplayAdjustments;
using Elastos::Droid::Hardware::Display::DisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CResourcesManager, Object, IResourcesManager)

CAR_OBJECT_IMPL(CResourcesManager)

const String CResourcesManager::TAG("ResourcesManager");
const Boolean CResourcesManager::DEBUG_CACHE = FALSE;
const Boolean CResourcesManager::DEBUG_STATS = TRUE;

AutoPtr<IResourcesManager> CResourcesManager::sResourcesManager;
Object CResourcesManager::sLock;
AutoPtr<IIPackageManager> CResourcesManager::sPackageManager;

CResourcesManager::CResourcesManager()
{
}

ECode CResourcesManager::constructor()
{
    CConfiguration::New((IConfiguration**)&mTmpConfig);
    CArrayMap::New((IArrayMap**)&mActiveResources);
    CArrayMap::New((IArrayMap**)&mDefaultDisplayMetrics);
    return NOERROR;
}

AutoPtr<IResourcesManager> CResourcesManager::GetInstance()
{
    {    AutoLock syncLock(sLock);
        if (sResourcesManager == NULL) {
            CResourcesManager::New((IResourcesManager**)&sResourcesManager);
        }
    }
    return sResourcesManager;
}

ECode CResourcesManager::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config)
    *config = mResConfiguration;
    REFCOUNT_ADD(*config)
    return NOERROR;
}

ECode CResourcesManager::FlushDisplayMetricsLocked()
{
    mDefaultDisplayMetrics->Clear();
    return NOERROR;
}

ECode CResourcesManager::GetDisplayMetricsLocked(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayMetrics** displayMetrics)
{
    return GetDisplayMetricsLocked(displayId,
        DisplayAdjustments::DEFAULT_DISPLAY_ADJUSTMENTS, displayMetrics);
}

ECode CResourcesManager::GetDisplayMetricsLocked(
    /* [in] */ Int32 displayId,
    /* [in] */ IDisplayAdjustments* daj,
    /* [out] */ IDisplayMetrics** displayMetrics)
{
    VALIDATE_NOT_NULL(displayMetrics)
    *displayMetrics = NULL;

    Boolean isDefaultDisplay = (displayId == IDisplay::DEFAULT_DISPLAY);
    AutoPtr<IDisplayMetrics> dm;
    if (isDefaultDisplay) {
        AutoPtr<IInterface> obj;
        mDefaultDisplayMetrics->Get(TO_IINTERFACE(daj), (IInterface**)&obj);
        dm = IDisplayMetrics::Probe(obj);
    }

    if (dm != NULL) {
        *displayMetrics = dm;
        REFCOUNT_ADD(*displayMetrics)
        return NOERROR;
    }

    CDisplayMetrics::New((IDisplayMetrics**)&dm);

    AutoPtr<IDisplayManagerGlobal> displayManager = DisplayManagerGlobal::GetInstance();
    if (displayManager == NULL) {
        // may be NULL early in system startup
        dm->SetToDefaults();
        *displayMetrics = dm;
        REFCOUNT_ADD(*displayMetrics)
        return NOERROR;
    }

    if (isDefaultDisplay) {
        mDefaultDisplayMetrics->Put(TO_IINTERFACE(daj), TO_IINTERFACE(dm));
    }

    AutoPtr<IDisplay> d;
    displayManager->GetCompatibleDisplay(displayId, daj, (IDisplay**)&d);
    if (d != NULL) {
        d->GetMetrics(dm);
    }
    else {
        // Display no longer exists
        // FIXME: This would not be a problem if we kept the Display object around
        // instead of using the raw display id everywhere.  The Display object caches
        // its information even after the display has been removed.
        dm->SetToDefaults();
    }
    //Logger::I("foo", "New metrics: w=" + metrics.widthPixels + " h="
    //        + metrics.heightPixels + " den=" + metrics.density
    //        + " xdpi=" + metrics.xdpi + " ydpi=" + metrics.ydpi);
    *displayMetrics = dm;
    REFCOUNT_ADD(*displayMetrics)
    return NOERROR;
}

void CResourcesManager::ApplyNonDefaultDisplayMetricsToConfigurationLocked(
    /* [in] */ IDisplayMetrics* dmObj,
    /* [in] */ IConfiguration* configObj)
{
    CDisplayMetrics* dm = (CDisplayMetrics*)dmObj;
    CConfiguration* config = (CConfiguration*)configObj;

    config->mTouchscreen = IConfiguration::TOUCHSCREEN_NOTOUCH;
    config->mDensityDpi = dm->mDensityDpi;
    config->mScreenWidthDp = (Int32)(dm->mWidthPixels / dm->mDensity);
    config->mScreenHeightDp = (Int32)(dm->mHeightPixels / dm->mDensity);
    Int32 sl = CConfiguration::ResetScreenLayout(config->mScreenLayout);
    if (dm->mWidthPixels > dm->mHeightPixels) {
        config->mOrientation = IConfiguration::ORIENTATION_LANDSCAPE;
        config->mScreenLayout = CConfiguration::ReduceScreenLayout(sl,
                config->mScreenWidthDp, config->mScreenHeightDp);
    } else {
        config->mOrientation = IConfiguration::ORIENTATION_PORTRAIT;
        config->mScreenLayout = CConfiguration::ReduceScreenLayout(sl,
                config->mScreenHeightDp, config->mScreenWidthDp);
    }
    config->mSmallestScreenWidthDp = config->mScreenWidthDp; // assume screen does not rotate
    config->mCompatScreenWidthDp = config->mScreenWidthDp;
    config->mCompatScreenHeightDp = config->mScreenHeightDp;
    config->mCompatSmallestScreenWidthDp = config->mSmallestScreenWidthDp;
    Logger::I(TAG, " >> ApplyNonDefaultDisplayMetricsToConfigurationLocked: dmObj:%s, config:%s",
        TO_CSTR(dmObj), TO_CSTR(configObj));
}

ECode CResourcesManager::ApplyCompatConfiguration(
    /* [in] */ Int32 displayDensity,
    /* [in] */ IConfiguration* compatConfiguration,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Boolean bval;
    if (mResCompatibilityInfo != NULL && (mResCompatibilityInfo->SupportsScreen(&bval), !bval)) {
        mResCompatibilityInfo->ApplyToConfiguration(displayDensity, compatConfiguration);
        *result = TRUE;
    }
    return NOERROR;
}

ECode CResourcesManager::GetTopLevelResources(
    /* [in] */ const String& resDir,
    /* [in] */ ArrayOf<String>* splitResDirs,
    /* [in] */ ArrayOf<String>* overlayDirs,
    /* [in] */ ArrayOf<String>* libDirs,
    /* [in] */ Int32 displayId,
    /* [in] */ const String& packageName,
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token,
    /* [in] */ IContext* context,
    /* [out] */ IResources** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    assert(compatInfo != NULL);

    Float scale;
    compatInfo->GetApplicationScale(&scale);
    Boolean isThemeable;
    compatInfo->GetIsThemeable(&isThemeable);
    AutoPtr<IThemeConfig> themeConfig = GetThemeConfig();
    AutoPtr<IResourcesKey> key = new ResourcesKey(resDir, displayId,
            overrideConfiguration, scale, isThemeable, themeConfig, token);
    AutoPtr<IResources> r;
    {
        AutoLock syncLock(this);
        // Resources is app scale dependent.
        if (FALSE) {
            // Logger::W(TAG, "GetTopLevelResources: %s, scale: %.2f", resDir.string(), scale);
        }
        AutoPtr<IInterface> obj;
        mActiveResources->Get(key.Get(), (IInterface**)&obj);
        if (obj != NULL) {
            IWeakReference::Probe(obj)->Resolve(EIID_IResources, (IInterface**)&r);
        }
        if (r != NULL) {
            AutoPtr<IAssetManager> assets;
            r->GetAssets((IAssetManager**)&assets);
            Boolean bval;
            assets->IsUpToDate(&bval);
            if (bval) {
                if (FALSE) {
                    AutoPtr<ICompatibilityInfo> ci;
                    r->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
                    Float applicationScale;
                    ci->GetApplicationScale(&applicationScale);
                    Logger::I(TAG, "Returning cached resources: dir:%s resource:%s, appScale=%f",
                        resDir.string(), TO_CSTR(r), applicationScale);
                }
                *result = r;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }

    // if (r != NULL) {
    //     Logger::W(TAG, "Throwing away out-of-date resources!!!! %s %s", TO_CSTR(r), resDir.string());
    // }
    AutoPtr<IAssetManager> assets;
    CAssetManager::New((IAssetManager**)&assets);
    assets->SetAppName(packageName);
    assets->SetThemeSupport(isThemeable);
    // resDir can be NULL if the 'android' package is creating a new Resources object.
    // This is fine, since each AssetManager automatically loads the 'android' package
    // already.
    if (!resDir.IsNull()) {
        Int32 cookie;
        assets->AddAssetPath(resDir, &cookie);
        if (cookie == 0) {
            return NOERROR;
        }
    }

    if (splitResDirs != NULL) {
        Int32 cookie;
        for (Int32 i = 0; i < splitResDirs->GetLength(); ++i) {
            assets->AddAssetPath((*splitResDirs)[i], &cookie);
            if (cookie == 0) {
                return NOERROR;
            }
        }
    }

    if (overlayDirs != NULL) {
        Int32 ival;
        for (Int32 i = 0; i < overlayDirs->GetLength(); ++i) {
            assets->AddOverlayPath((*overlayDirs)[i], String(NULL), String(NULL), String(NULL), String(NULL), &ival);
        }
    }

    if (libDirs != NULL) {
        Int32 cookie;
        for (Int32 i = 0; i < libDirs->GetLength(); ++i) {
            assets->AddAssetPath((*libDirs)[i], &cookie);
            if (cookie == 0) {
                Logger::W(TAG, "Asset path '%s' does not exist or contains no resources.",
                    (*libDirs)[i].string());
            }
        }
    }

    //Logger::I(TAG, "Resource: key=" + key + ", display metrics=" + metrics);
    AutoPtr<IDisplayMetrics> dm;
    GetDisplayMetricsLocked(displayId, (IDisplayMetrics**)&dm);
    AutoPtr<IConfiguration> config;
    Boolean isDefaultDisplay = (displayId == IDisplay::DEFAULT_DISPLAY);
    Boolean hasOverrideConfig;
    key->HasOverrideConfiguration(&hasOverrideConfig);
    if (!isDefaultDisplay || hasOverrideConfig) {
        AutoPtr<IConfiguration> c;
        GetConfiguration((IConfiguration**)&c);
        CConfiguration::New(c, (IConfiguration**)&config);
        if (!isDefaultDisplay) {
            ApplyNonDefaultDisplayMetricsToConfigurationLocked(dm, config);
        }
        if (hasOverrideConfig) {
            AutoPtr<IConfiguration> config;
            key->GetOverrideConfiguration((IConfiguration**)&config);
            Int32 ival;
            config->UpdateFrom(config, &ival);
        }
    }
    else {
        GetConfiguration((IConfiguration**)&config);
    }

    Boolean iconsAttached = FALSE;
    /* Attach theme information to the resulting AssetManager when appropriate. */
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    Boolean isSafeMode;
    if (isThemeable && config != NULL && (pm->IsSafeMode(&isSafeMode), !isSafeMode)) {
        AutoPtr<IThemeConfig> tc;
        config->GetThemeConfig((IThemeConfig**)&tc);
        if (tc == NULL) {
            // try {
            AutoPtr<IContentResolver> resolver;
            context->GetContentResolver((IContentResolver**)&resolver);
            ECode ec = CThemeConfig::GetBootTheme(resolver, (IThemeConfig**)&tc);
            if (FAILED(ec)) {
                Logger::D(TAG, "ThemeConfig.getBootTheme failed, falling back to system theme");
                tc = CThemeConfig::GetSystemTheme();
            }
            config->SetThemeConfig(tc);
        }

        if (tc != NULL) {
            AttachThemeAssets(assets, tc);
            AttachCommonAssets(assets, tc);
            iconsAttached = AttachIconAssets(assets, tc);
        }
    }

    r = NULL;
    CResources::New(assets, dm, config, compatInfo, token, (IResources**)&r);
    if (iconsAttached) SetActivityIcons(r);

    if (FALSE) {
        AutoPtr<IConfiguration> c;
        r->GetConfiguration((IConfiguration**)&c);
        AutoPtr<ICompatibilityInfo> ci;
        r->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
        Float applicationScale;
        ci->GetApplicationScale(&applicationScale);
        Logger::I(TAG, "=======================================================");
        Logger::I(TAG, "Created app resources: dir:%s resource:%s, config:%s, appScale=%f",
            resDir.string(), TO_CSTR(r), TO_CSTR(c), applicationScale);
        Logger::I(TAG, "=======================================================");
    }

    {
        AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mActiveResources->Get(key.Get(), (IInterface**)&obj);
        AutoPtr<IResources> existing;
        if (obj != NULL) {
            IWeakReference::Probe(obj)->Resolve(EIID_IResources, (IInterface**)&existing);
        }
        if (existing != NULL) {
            AutoPtr<IAssetManager> assets;
            existing->GetAssets((IAssetManager**)&assets);
            Boolean bval;
            assets->IsUpToDate(&bval);
            if (bval) {
                // Someone else already created the resources while we were
                // unlocked; go ahead and use theirs.
                assets->Close();
                *result = existing;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }

        // XXX need to remove entries when weak references go away
        AutoPtr<IWeakReference> wr;
        IWeakReferenceSource::Probe(r)->GetWeakReference((IWeakReference**)&wr);
        mActiveResources->Put(key.Get(), wr.Get());
        *result = r;
        REFCOUNT_ADD(*result)
    }

    return NOERROR;
}

ECode CResourcesManager::GetTopLevelThemedResources(
    /* [in] */ const String& resDir,
    /* [in] */ Int32 displayId,
    /* [in] */ const String& packageName,
    /* [in] */ const String& themePackageName,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token,
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    AutoPtr<IResources> r;

    AutoPtr<IAssetManager> assets;
    CAssetManager::New((IAssetManager**)&assets);
    assets->SetAppName(packageName);
    assets->SetThemeSupport(TRUE);
    Int32 cookie;
    if (assets->AddAssetPath(resDir, &cookie), cookie == 0) {
        return NOERROR;
    }

    //Slog.i(TAG, "Resource: key=" + key + ", display metrics=" + metrics);
    AutoPtr<IDisplayMetrics> dm;
    GetDisplayMetricsLocked(displayId, (IDisplayMetrics**)&dm);
    AutoPtr<IConfiguration> config;
    Boolean isDefaultDisplay = (displayId == IDisplay::DEFAULT_DISPLAY);
    if (!isDefaultDisplay) {
        AutoPtr<IConfiguration> c;
        GetConfiguration((IConfiguration**)&c);
        CConfiguration::New(c, (IConfiguration**)&config);
        ApplyNonDefaultDisplayMetricsToConfigurationLocked(dm, config);
    }
    else {
        GetConfiguration((IConfiguration**)&config);
    }

    /* Attach theme information to the resulting AssetManager when appropriate. */
    AutoPtr<IThemeConfigBuilder> builder;
    CThemeConfigBuilder::New((IThemeConfigBuilder**)&builder);
    builder->DefaultOverlay(themePackageName);
    builder->DefaultIcon(themePackageName);
    builder->DefaultFont(themePackageName);

    AutoPtr<IThemeConfig> themeConfig;
    builder->Build((IThemeConfig**)&themeConfig);
    AttachThemeAssets(assets, themeConfig);
    AttachCommonAssets(assets, themeConfig);
    AttachIconAssets(assets, themeConfig);

    CResources::New(assets, dm, config, compatInfo, token, (IResources**)&r);
    SetActivityIcons(r);

    *res = r;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

void CResourcesManager::SetActivityIcons(
    /* [in] */ IResources* r)
{
    AutoPtr<ISparseArray> iconResources;
    CSparseArray::New((ISparseArray**)&iconResources);
    AutoPtr<IAssetManager> am;
    r->GetAssets((IAssetManager**)&am);
    String pkgName;
    am->GetAppName(&pkgName);
    AutoPtr<IPackageInfo> pkgInfo;
    AutoPtr<IApplicationInfo> appInfo;

    // try {
    ECode ec = GetPackageManager()->GetPackageInfo(pkgName, IPackageManager::GET_ACTIVITIES,
            UserHandle::GetCallingUserId(), (IPackageInfo**)&pkgInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "Unable to get pkg %s", pkgName.string());
        return;
    }
    // } catch (RemoteException e1) {
    //     Log.e(TAG, "Unable to get pkg " + pkgName, e1);
    //     return;
    // }

    AutoPtr<IConfiguration> config;
    r->GetConfiguration((IConfiguration**)&config);
    AutoPtr<IThemeConfig> themeConfig;
    config->GetThemeConfig((IThemeConfig**)&themeConfig);
    String ippName;
    if (!pkgName.IsNull() && themeConfig != NULL &&
            (themeConfig->GetIconPackPkgName(&ippName), pkgName.Equals(ippName))) {
        return;
    }

    //Map application icon
    if (pkgInfo != NULL &&
            (pkgInfo->GetApplicationInfo((IApplicationInfo**)&appInfo), appInfo != NULL)) {
        Int32 themedIcon, icon;
        IPackageItemInfo::Probe(appInfo)->GetThemedIcon(&themedIcon);
        IPackageItemInfo::Probe(appInfo)->GetIcon(&icon);
        AutoPtr<IInterface> value;
        if (themedIcon != 0 || (iconResources->Get(icon, (IInterface**)&value), value == NULL)) {
            iconResources->Put(icon, appInfo);
        }
    }

    //Map activity icons.
    AutoPtr< ArrayOf<IActivityInfo*> > activities;
    if (pkgInfo != NULL &&
            (pkgInfo->GetActivities((ArrayOf<IActivityInfo*>**)&activities), activities != NULL)) {
        for (Int32 i = 0; i < activities->GetLength(); i++) {
            AutoPtr<IActivityInfo> ai = (*activities)[i];
            Int32 icon, themedIcon;
            AutoPtr<IInterface> value;
            if ((IPackageItemInfo::Probe(ai)->GetIcon(&icon), icon != 0) &&
                    ((IPackageItemInfo::Probe(ai)->GetThemedIcon(&themedIcon), themedIcon != 0) ||
                        (iconResources->Get(icon, (IInterface**)&value), value == NULL))) {
                iconResources->Put(icon, ai);
            }
            else if (appInfo != NULL && (IPackageItemInfo::Probe(appInfo)->GetIcon(&icon), icon != 0) &&
                    ((IPackageItemInfo::Probe(ai)->GetThemedIcon(&themedIcon), themedIcon != 0) ||
                        (iconResources->Get(icon, (IInterface**)&value), value == NULL))) {
                iconResources->Put(icon, ai);
            }
        }
    }

    r->SetIconResources(iconResources);
    AutoPtr<IIPackageManager> pm = GetPackageManager();
    // try {
    AutoPtr<IComposedIconInfo> iconInfo;
    ec = pm->GetComposedIconInfo((IComposedIconInfo**)&iconInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to retrieve ComposedIconInfo");
        return;
    }
    r->SetComposedIconInfo(iconInfo);
    // } catch (Exception e) {
    //     Log.wtf(TAG, "Failed to retrieve ComposedIconInfo", e);
    // }
}

ECode CResourcesManager::ApplyConfigurationToResourcesLocked(
    /* [in] */ IConfiguration* config,
    /* [in] */ ICompatibilityInfo* compat,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mResConfiguration == NULL) {
        CConfiguration::New((IConfiguration**)&mResConfiguration);
    }

    Boolean bval;
    mResConfiguration->IsOtherSeqNewer(config, &bval);
    if (!bval && compat == NULL) {
        if (CActivityThread::DEBUG_CONFIGURATION) {
            Int32 s1, s2;
            mResConfiguration->GetSeq(&s1);
            config->GetSeq(&s2);
            Logger::V(TAG, "Skipping new config: curSeq=%d, newSeq=%d", s1, s2);
        }
        return NOERROR;
    }

    if (CActivityThread::DEBUG_CONFIGURATION) {
        Logger::V(TAG, "ApplyConfigurationToResourcesLocked new config: %s", TO_CSTR(config));
    }

    Int32 changes;
    mResConfiguration->UpdateFrom(config, &changes);
    FlushDisplayMetricsLocked();
    AutoPtr<IDisplayMetrics> defaultDisplayMetrics;
    GetDisplayMetricsLocked(IDisplay::DEFAULT_DISPLAY, (IDisplayMetrics**)&defaultDisplayMetrics);

    if (compat != NULL && (mResCompatibilityInfo == NULL ||
            !Object::Equals(mResCompatibilityInfo, compat))) {
        mResCompatibilityInfo = compat;
        changes |= IActivityInfo::CONFIG_SCREEN_LAYOUT
                | IActivityInfo::CONFIG_SCREEN_SIZE
                | IActivityInfo::CONFIG_SMALLEST_SCREEN_SIZE;
    }

    // set it for java, this also affects newly created Resources
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    if (locale != NULL) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        helper->SetDefault(locale);
    }

    CResources::UpdateSystemConfiguration(config, defaultDisplayMetrics, compat);

    ApplicationPackageManager::ConfigurationChanged();

    AutoPtr<IConfiguration> tmpConfig;

    Int32 size;
    mActiveResources->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> ko, vo;
        mActiveResources->GetKeyAt(i, (IInterface**)&ko);
        mActiveResources->GetValueAt(i, (IInterface**)&vo);
        ResourcesKey* key = (ResourcesKey*)IResourcesKey::Probe(ko);
        IResources* r = IResources::Probe(vo);

        if (r != NULL) {
            if (CActivityThread::DEBUG_CONFIGURATION) {
                // Logger::V(TAG, "Changing resources "
                //     + r + " config to: " + config);
            }

            Int32 displayId = key->mDisplayId;
            Boolean isDefaultDisplay = (displayId == IDisplay::DEFAULT_DISPLAY);
            AutoPtr<IDisplayMetrics> dm = defaultDisplayMetrics;
            Boolean hasOverrideConfiguration = key->HasOverrideConfiguration();
            Boolean themeChanged = (changes & IActivityInfo::CONFIG_THEME_RESOURCE) != 0;
            if (themeChanged) {
                AutoPtr<IAssetManager> am;
                r->GetAssets((IAssetManager**)&am);
                Boolean result;
                if (am->HasThemeSupport(&result), result) {
                    r->SetIconResources(NULL);
                    r->SetComposedIconInfo(NULL);
                    DetachThemeAssets(am);
                    AutoPtr<IThemeConfig> themeConfig;
                    config->GetThemeConfig((IThemeConfig**)&themeConfig);
                    if (themeConfig != NULL) {
                        AttachThemeAssets(am, themeConfig);
                        AttachCommonAssets(am, themeConfig);
                        if (AttachIconAssets(am, themeConfig)) {
                            SetActivityIcons(r);
                        }
                    }
                }
            }
            if (!isDefaultDisplay || hasOverrideConfiguration) {
                if (tmpConfig == NULL) {
                    CConfiguration::New((IConfiguration**)&tmpConfig);
                }
                tmpConfig->SetTo(config);

                if (!isDefaultDisplay) {
                    dm = NULL;
                    GetDisplayMetricsLocked(displayId, (IDisplayMetrics**)&dm);
                    ApplyNonDefaultDisplayMetricsToConfigurationLocked(dm, tmpConfig);
                }

                if (hasOverrideConfiguration) {
                    Int32 ival;
                    tmpConfig->UpdateFrom(key->mOverrideConfiguration, &ival);
                }
                r->UpdateConfiguration(tmpConfig, dm, compat);
            }
            else {
                r->UpdateConfiguration(config, dm, compat);
            }
            if (themeChanged) {
                r->UpdateStringCache();
            }
            //Logger::I(TAG, "Updated app resources " + v.getKey()
            //        + " " + r + ": " + r.getConfiguration());
        }
        else {
            //Logger::I(TAG, "Removing old resources " + v.getKey());
            mActiveResources->RemoveAt(i);
        }
    }

    *result = changes != 0;
    return NOERROR;
}

AutoPtr<IIPackageManager> CResourcesManager::GetPackageManager()
{
    if (sPackageManager != NULL) {
        return sPackageManager;
    }
    AutoPtr<IInterface> b = ServiceManager::GetService(String("package"));
    sPackageManager = IIPackageManager::Probe(b);
    return sPackageManager;
}

Boolean CResourcesManager::AttachThemeAssets(
    /* [in] */ IAssetManager* assets,
    /* [in] */ IThemeConfig* theme)
{
    AutoPtr<IPackageInfo> piTheme;
    AutoPtr<IPackageInfo> piTarget;
    AutoPtr<IPackageInfo> piElastos;

    // Some apps run in process of another app (eg keyguard/systemUI) so we must get the
    // package name from the res tables. The 0th base package name will be the android group.
    // The 1st base package name will be the app group if one is attached. Check if it is there
    // first or else the system will crash!
    String basePackageName;
    String resourcePackageName;
    Int32 count;
    assets->GetBasePackageCount(&count);
    if (count > 1) {
        assets->GetBasePackageName(1, &basePackageName);
        assets->GetBaseResourcePackageName(1, &resourcePackageName);
    }
    else if (count == 1) {
        assets->GetBasePackageName(0, &basePackageName);
    }
    else {
        return FALSE;
    }

    // try {
    String overlayPkgName;
    theme->GetOverlayPkgNameForApp(basePackageName, &overlayPkgName);
    GetPackageManager()->GetPackageInfo(
            overlayPkgName, 0,
            UserHandle::GetCallingUserId(), (IPackageInfo**)&piTheme);
    GetPackageManager()->GetPackageInfo(
            basePackageName, 0, UserHandle::GetCallingUserId(), (IPackageInfo**)&piTarget);

    // Handle special case where a system app (ex trebuchet) may have had its pkg name
    // renamed during an upgrade. basePackageName would be the manifest value which will
    // fail on getPackageInfo(). resource pkg is assumed to have the original name
    if (piTarget == NULL && !resourcePackageName.IsNull()) {
        GetPackageManager()->GetPackageInfo(resourcePackageName,
                0, UserHandle::GetCallingUserId(), (IPackageInfo**)&piTarget);
    }
    GetPackageManager()->GetPackageInfo(String("android"), 0,
            UserHandle::GetCallingUserId(), (IPackageInfo**)&piElastos);
    // } catch (RemoteException e) {
    // }

    AutoPtr<IApplicationInfo> piThemeAppInfo;
    AutoPtr<IApplicationInfo> piTargetAppInfo;
    AutoPtr<IApplicationInfo> piElastosAppInfo;
    AutoPtr<IArrayList> piThemeOverlayTargets;
    if (piTheme == NULL || (piTheme->GetApplicationInfo((IApplicationInfo**)&piThemeAppInfo), piThemeAppInfo == NULL) ||
            piTarget == NULL || (piTarget->GetApplicationInfo((IApplicationInfo**)&piTargetAppInfo), piTargetAppInfo == NULL) ||
            piElastos == NULL || (piElastos->GetApplicationInfo((IApplicationInfo**)&piElastosAppInfo), piElastosAppInfo == NULL) ||
            (piTheme->GetOverlayTargets((IArrayList**)&piThemeOverlayTargets), piThemeOverlayTargets == NULL)) {
        return FALSE;
    }

    String themePackageName;
    piTheme->GetPackageName(&themePackageName);
    String themePath;
    piThemeAppInfo->GetPublicSourceDir(&themePath);
    Boolean isThemeApk, result;
    if ((piTarget->GetIsThemeApk(&isThemeApk), !isThemeApk) &&
            (piThemeOverlayTargets->Contains(CoreUtils::Convert(basePackageName), &result), result)) {
        String targetPackagePath;
        piTargetAppInfo->GetSourceDir(&targetPackagePath);
        String prefixPath = ThemeUtils::GetOverlayPathToTarget(basePackageName);

        String targetPackageName;
        piTarget->GetPackageName(&targetPackageName);
        String resCachePath = ThemeUtils::GetTargetCacheDir(targetPackageName, piTheme);
        String resApkPath = resCachePath + "/resources.apk";
        String idmapPath = ThemeUtils::GetIdmapPath(targetPackageName, themePackageName);
        Int32 cookie;
        assets->AddOverlayPath(idmapPath, themePath, resApkPath,
                targetPackagePath, prefixPath, &cookie);

        if (cookie != 0) {
            assets->SetThemePackageName(themePackageName);
            assets->AddThemeCookie(cookie);
        }
    }

    if (!isThemeApk && !basePackageName.Equals("android") &&
            (piThemeOverlayTargets->Contains(CoreUtils::Convert("android"), &result), result)) {
        String elastosPackageName;
        piElastos->GetPackageName(&elastosPackageName);
        String resCachePath= ThemeUtils::GetTargetCacheDir(elastosPackageName, piTheme);
        String prefixPath = ThemeUtils::GetOverlayPathToTarget(elastosPackageName);
        String targetPackagePath;
        piElastosAppInfo->GetPublicSourceDir(&targetPackagePath);
        String resApkPath = resCachePath + "/resources.apk";
        String idmapPath = ThemeUtils::GetIdmapPath(String("android"), themePackageName);
        Int32 cookie;
        assets->AddOverlayPath(idmapPath, themePath,
                resApkPath, targetPackagePath, prefixPath, &cookie);
        if (cookie != 0) {
            assets->SetThemePackageName(themePackageName);
            assets->AddThemeCookie(cookie);
        }
    }

    return TRUE;
}

Boolean CResourcesManager::AttachIconAssets(
    /* [in] */ IAssetManager* assets,
    /* [in] */ IThemeConfig* theme)
{
    AutoPtr<IPackageInfo> piIcon;
    // try {
    String iconPkg;
    theme->GetIconPackPkgName(&iconPkg);
    GetPackageManager()->GetPackageInfo(iconPkg, 0,
            UserHandle::GetCallingUserId(), (IPackageInfo**)&piIcon);
    // } catch (RemoteException e) {
    // }

    AutoPtr<IApplicationInfo> appInfo;
    if (piIcon == NULL ||
            (piIcon->GetApplicationInfo((IApplicationInfo**)&appInfo), appInfo == NULL)) {
        return FALSE;
    }

    if (!iconPkg.IsNullOrEmpty()) {
        String themeIconPath;
        appInfo->GetPublicSourceDir(&themeIconPath);
        String prefixPath = IThemeUtils::ICONS_PATH;
        String iconDir = ThemeUtils::GetIconPackDir(iconPkg);
        String resTablePath = iconDir + "/resources.arsc";
        String resApkPath = iconDir + "/resources.apk";

        // Legacy Icon packs have everything in their APK
        Boolean isLegacyIconPackApk;
        if (piIcon->GetIsLegacyIconPackApk(&isLegacyIconPackApk), isLegacyIconPackApk) {
            prefixPath = "";
            resApkPath = "";
            resTablePath = "";
        }

        Int32 cookie;
        assets->AddIconPath(themeIconPath, resApkPath, prefixPath,
                IResources::THEME_ICON_PKG_ID, &cookie);
        if (cookie != 0) {
            assets->SetIconPackCookie(cookie);
            assets->SetIconPackageName(iconPkg);
        }
    }

    return TRUE;
}

Boolean CResourcesManager::AttachCommonAssets(
    /* [in] */ IAssetManager* assets,
    /* [in] */ IThemeConfig* theme)
{
    // Some apps run in process of another app (eg keyguard/systemUI) so we must get the
    // package name from the res tables. The 0th base package name will be the android group.
    // The 1st base package name will be the app group if one is attached. Check if it is there
    // first or else the system will crash!
    String basePackageName;
    Int32 count;
    assets->GetBasePackageCount(&count);
    if (count > 1) {
        assets->GetBasePackageName(1, &basePackageName);
    }
    else if (count == 1) {
        assets->GetBasePackageName(0, &basePackageName);
    }
    else {
        return FALSE;
    }

    AutoPtr<IPackageInfo> piTheme;
    // try {
    String overlayPkgName;
    theme->GetOverlayPkgNameForApp(basePackageName, &overlayPkgName);
    GetPackageManager()->GetPackageInfo(
            overlayPkgName, 0,
            UserHandle::GetCallingUserId(), (IPackageInfo**)&piTheme);
    // } catch (RemoteException e) {
    // }

    AutoPtr<IApplicationInfo> appInfo;
    if (piTheme == NULL ||
            (piTheme->GetApplicationInfo((IApplicationInfo**)&appInfo), appInfo == NULL)) {
        return FALSE;
    }

    String pkgName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkgName);
    String themePackageName = ThemeUtils::GetCommonPackageName(pkgName);
    if (!themePackageName.IsNullOrEmpty()) {
        String themePath;
        appInfo->GetPublicSourceDir(&themePath);
        String prefixPath = IThemeUtils::COMMON_RES_PATH;
        String resCachePath =
                ThemeUtils::GetTargetCacheDir(IThemeUtils::COMMON_RES_TARGET, piTheme);
        String resApkPath = resCachePath + "/resources.apk";
        Int32 cookie;
        assets->AddCommonOverlayPath(themePath, resApkPath,
                prefixPath, &cookie);
        if (cookie != 0) {
            assets->SetCommonResCookie(cookie);
            assets->SetCommonResPackageName(themePackageName);
        }
    }

    return TRUE;
}

void CResourcesManager::DetachThemeAssets(
    /* [in] */ IAssetManager* assets)
{
    String themePackageName;
    assets->GetThemePackageName(&themePackageName);
    String iconPackageName;
    assets->GetIconPackageName(&iconPackageName);
    String commonResPackageName;
    assets->GetCommonResPackageName(&commonResPackageName);

    //Remove Icon pack if it exists
    Int32 cookie;
    if (!TextUtils::IsEmpty(iconPackageName) && (assets->GetIconPackCookie(&cookie), cookie > 0)) {
        Boolean result;
        assets->RemoveOverlayPath(iconPackageName, cookie, &result);
        assets->SetIconPackageName(String(NULL));
        assets->SetIconPackCookie(0);
    }
    //Remove common resources if it exists
    if (!TextUtils::IsEmpty(commonResPackageName) && (assets->GetCommonResCookie(&cookie), cookie > 0)) {
        Boolean result;
        assets->RemoveOverlayPath(commonResPackageName, cookie, &result);
        assets->SetCommonResPackageName(String(NULL));
        assets->SetCommonResCookie(0);
    }
    AutoPtr<IArrayList> themeCookies;
    assets->GetThemeCookies((IArrayList**)&themeCookies);
    Boolean isEmpty;
    if (!TextUtils::IsEmpty(themePackageName) && (themeCookies->IsEmpty(&isEmpty), !isEmpty)) {
        // remove overlays in reverse order
        Boolean result;
        Int32 N;
        themeCookies->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            themeCookies->Get(i, (IInterface**)&obj);
            assets->RemoveOverlayPath(themePackageName, CoreUtils::Unbox(IInteger32::Probe(obj)), &result);
        }
    }
    themeCookies = NULL;
    assets->GetThemeCookies((IArrayList**)&themeCookies);
    themeCookies->Clear();
    assets->SetThemePackageName(String(NULL));
}

AutoPtr<IThemeConfig> CResourcesManager::GetThemeConfig()
{
    AutoPtr<IConfiguration> config;
    GetConfiguration((IConfiguration**)&config);
    if (config != NULL) {
        AutoPtr<IThemeConfig> themeConfig;
        config->GetThemeConfig((IThemeConfig**)&themeConfig);
        return themeConfig;
    }
    return NULL;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
