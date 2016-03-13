
#include "elastos/droid/app/CResourcesManager.h"
#include "elastos/droid/app/ApplicationPackageManager.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/content/res/ResourcesKey.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::ResourcesKey;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::View::DisplayAdjustments;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Hardware::Display::DisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
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
    synchronized(sLock) {
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
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token,
    /* [out] */ IResources** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    assert(compatInfo != NULL);

    Float scale;
    compatInfo->GetApplicationScale(&scale);
    AutoPtr<IResourcesKey> key = (IResourcesKey*)new ResourcesKey(resDir, displayId, overrideConfiguration, scale, token);
    AutoPtr<IResources> r;
    synchronized(this) {
        // Resources is app scale dependent.
        AutoPtr<IInterface> obj, resObj;
        mActiveResources->Get(TO_IINTERFACE(key), (IInterface**)&obj);
        if (obj != NULL) {
            IWeakReference* wr = IWeakReference::Probe(obj);
            wr->Resolve(EIID_IInterface, (IInterface**)&resObj);
            if (resObj != NULL) {
                r = IResources::Probe(resObj);

                //if (r != NULL) Logger::I(TAG, "isUpToDate " + resDir + ": " + r.getAssets().isUpToDate());
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
                        Logger::W(TAG, "Returning cached resources %s %s : appScale=%f",
                            TO_CSTR(r), resDir.string(), applicationScale);
                    }
                    *result = r;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
        }
        else {
            Logger::W(TAG, "getTopLevelResources: %s / %f", resDir.string(), scale);
        }
    }

    //if (r != NULL) {
    //    Logger::W(TAG, "Throwing away out-of-date resources!!!! "
    //            + r + " " + resDir);
    //}

    AutoPtr<IAssetManager> assets;
    CAssetManager::New((IAssetManager**)&assets);
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
            assets->AddOverlayPath((*overlayDirs)[i], &ival);
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

    r = NULL;
    CResources::New(assets, dm, config, compatInfo, token, (IResources**)&r);
    if (FALSE) {
        AutoPtr<IConfiguration> c;
        r->GetConfiguration((IConfiguration**)&c);
        AutoPtr<ICompatibilityInfo> ci;
        r->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
        Float applicationScale;
        ci->GetApplicationScale(&applicationScale);
        Logger::I(TAG, "Created app resources %s %s: %s appScale=%f",
            resDir.string(), TO_CSTR(r),
            TO_CSTR(c), applicationScale);
    }

    synchronized(this) {
        AutoPtr<IInterface> obj, resObj;
        mActiveResources->Get(TO_IINTERFACE(key), (IInterface**)&obj);

        AutoPtr<IResources> existing;
        if (obj != NULL) {
            IWeakReference* wr = IWeakReference::Probe(obj);
            wr->Resolve(EIID_IInterface, (IInterface**)&resObj);
            if (resObj != NULL) {
                existing = IResources::Probe(resObj);
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
            IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(r);
            wr = NULL;
            wrs->GetWeakReference((IWeakReference**)&wr);
            mActiveResources->Put(TO_IINTERFACE(key), TO_IINTERFACE(wr));
            *result = r;
            REFCOUNT_ADD(*result)
        }
    }

    return NOERROR;
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
            Logger::V(TAG, "Skipping new config: curSeq=%d, newSeq=%d",
                s1, s2);
        }
        return NOERROR;
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
    //Logger::I(TAG, "Configuration changed in " + currentPackageName());

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

} // namespace App
} // namespace Droid
} // namespace Elastos
