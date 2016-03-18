
#ifndef __ELASTOS_DROID_APP_CRESOURCEMANAGER_H__
#define __ELASTOS_DROID_APP_CRESOURCEMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_App_CResourcesManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesKey;
using Elastos::Droid::Content::Res::IThemeConfig;
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
        /* [in] */ const String& packageName,
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token,
        /* [in] */ IContext* context,
        /* [out] */ IResources** res);

    /**
     * Creates the top level Resources for applications with the given compatibility info.
     *
     * @param resDir the resource directory.
     * @param compatInfo the compability info. Must not be null.
     * @param token the application token for determining stack bounds.
     *
     * @hide
     */
    CARAPI GetTopLevelThemedResources(
        /* [in] */ const String& resDir,
        /* [in] */ Int32 displayId,
        /* [in] */ const String& packageName,
        /* [in] */ const String& themePackageName,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token,
        /* [out] */ IResources** res);

    CARAPI ApplyConfigurationToResourcesLocked(
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compat,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IIPackageManager>) GetPackageManager();

private:
    /**
     * Creates a map between an activity & app's icon ids to its component info. This map
     * is then stored in the resource object.
     * When resource.getDrawable(id) is called it will check this mapping and replace
     * the id with the themed resource id if one is available
     * @param context
     * @param pkgName
     * @param r
     */
    CARAPI_(void) SetActivityIcons(
        /* [in] */ IResources* r);

    /**
     * Attach the necessary theme asset paths and meta information to convert an
     * AssetManager to being globally "theme-aware".
     *
     * @param assets
     * @param theme
     * @return true if the AssetManager is now theme-aware; false otherwise.
     *         This can fail, for example, if the theme package has been been
     *         removed and the theme manager has yet to revert formally back to
     *         the framework default.
     */
    CARAPI_(Boolean) AttachThemeAssets(
        /* [in] */ IAssetManager* assets,
        /* [in] */ IThemeConfig* theme);

    /**
     * Attach the necessary icon asset paths. Icon assets should be in a different
     * namespace than the standard 0x7F.
     *
     * @param assets
     * @param theme
     * @return true if succes, false otherwise
     */
    CARAPI_(Boolean) AttachIconAssets(
        /* [in] */ IAssetManager* assets,
        /* [in] */ IThemeConfig* theme);

    /**
     * Attach the necessary common asset paths. Common assets should be in a different
     * namespace than the standard 0x7F.
     *
     * @param assets
     * @param theme
     * @return true if succes, false otherwise
     */
    CARAPI_(Boolean) AttachCommonAssets(
        /* [in] */ IAssetManager* assets,
        /* [in] */ IThemeConfig* theme);

    CARAPI_(void) DetachThemeAssets(
        /* [in] */ IAssetManager* assets);

private:
    CARAPI_(AutoPtr<IThemeConfig>) GetThemeConfig();

private:
    static const String TAG;
    static const Boolean DEBUG_CACHE;
    static const Boolean DEBUG_STATS;

    static AutoPtr<IResourcesManager> sResourcesManager;
    static Object sLock;

    AutoPtr<IArrayMap> mActiveResources;//= new ArrayMap<ResourcesKey, WeakReference<Resources> >();

    AutoPtr<IArrayMap>  mDefaultDisplayMetrics;//= new ArrayMap<DisplayAdjustments, DisplayMetrics>();

    AutoPtr<ICompatibilityInfo> mResCompatibilityInfo;
    static AutoPtr<IIPackageManager> sPackageManager;

    AutoPtr<IConfiguration> mResConfiguration;
    AutoPtr<IConfiguration> mTmpConfig;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CRESOURCEMANAGER_H__
