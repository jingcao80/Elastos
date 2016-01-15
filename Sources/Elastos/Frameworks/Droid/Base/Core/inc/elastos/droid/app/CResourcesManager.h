
#ifndef __ELASTOS_DROID_APP_CRESOURCEMANAGER_H__
#define __ELASTOS_DROID_APP_CRESOURCEMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_App_CResourcesManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesKey;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayAdjustments;

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace App {

/** @hide */
CarClass(CResourcesManager)
    , public Object
    , public IResourcesManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CResourcesManager();

    CARAPI constructor();

    static AutoPtr<IResourcesManager> GetInstance();

    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    CARAPI FlushDisplayMetricsLocked();

    CARAPI GetDisplayMetricsLocked(
        /* [in] */ Int32 displayId,
        /* [out] */ IDisplayMetrics** displayMetrics);

    CARAPI GetDisplayMetricsLocked(
        /* [in] */ Int32 displayId,
        /* [in] */ IDisplayAdjustments* daj,
        /* [out] */ IDisplayMetrics** displayMetrics);

    void ApplyNonDefaultDisplayMetricsToConfigurationLocked(
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ IConfiguration* config);

    CARAPI ApplyCompatConfiguration(
        /* [in] */ Int32 displayDensity,
        /* [in] */ IConfiguration* compatConfiguration,
        /* [out] */ Boolean* result);

    /**
     * Creates the top level Resources for applications with the given compatibility info.
     *
     * @param resDir the resource directory.
     * @param overlayDirs the resource overlay directories.
     * @param libDirs the shared library resource dirs this app references.
     * @param compatInfo the compability info. Must not be null.
     * @param token the application token for determining stack bounds.
     */
    CARAPI GetTopLevelResources(
        /* [in] */ const String& resDir,
        /* [in] */ ArrayOf<String>* splitResDirs,
        /* [in] */ ArrayOf<String>* overlayDirs,
        /* [in] */ ArrayOf<String>* libDirs,
        /* [in] */ Int32 displayId,
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token,
        /* [out] */ IResources** res);

    CARAPI ApplyConfigurationToResourcesLocked(
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compat,
        /* [out] */ Boolean* result);

private:
    static const String TAG;
    static const Boolean DEBUG_CACHE;
    static const Boolean DEBUG_STATS;

    static AutoPtr<IResourcesManager> sResourcesManager;
    static Object sLock;

    AutoPtr<IArrayMap> mActiveResources;//= new ArrayMap<ResourcesKey, WeakReference<Resources> >();

    AutoPtr<IArrayMap>  mDefaultDisplayMetrics;//= new ArrayMap<DisplayAdjustments, DisplayMetrics>();

    AutoPtr<ICompatibilityInfo> mResCompatibilityInfo;

    AutoPtr<IConfiguration> mResConfiguration;
    AutoPtr<IConfiguration> mTmpConfig;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CRESOURCEMANAGER_H__
