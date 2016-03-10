
#ifndef __ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__
#define __ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__

#include "_Elastos.Droid.Content.h"
#include <elastos/droid/content/ContextWrapper.h>
#include "elastos/droid/content/pm/PackageParser.h"

using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IThemeConfig;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

class ThemeUtils
{
public:
    class ThemedUiContext : public ContextWrapper
    {
    public:
        ThemedUiContext(
            /* [in] */ IContext* context,
            /* [in] */ const String& packageName);

        CARAPI GetPackageName(
            /* [out] */ String* name);

    private:
        String mPackageName;
    };

public:
    /**
     * Get the root path of the resource cache for the given theme
     * @param themePkgName
     * @return Root resource cache path for the given theme
     */
    static CARAPI_(String) GetOverlayResourceCacheDir(
        /* [in] */ const String& themePkgName);

    /**
     * Get the path of the resource cache for the given target and theme
     * @param targetPkgName
     * @param themePkg
     * @return Path to the resource cache for this target and theme
     */
    static CARAPI_(String) GetTargetCacheDir(
        /* [in] */ const String& targetPkgName,
        /* [in] */ IPackageInfo* themePkg);

    static CARAPI_(String) GetTargetCacheDir(
        /* [in] */ const String& targetPkgName,
        /* [in] */ PackageParser::Package* themePkg);

    static CARAPI_(String) GetTargetCacheDir(
        /* [in] */ const String& targetPkgName,
        /* [in] */ const String& themePkgName);

    /**
     * Get the path to the icons for the given theme
     * @param pkgName
     * @return
     */
    static CARAPI_(String) GetIconPackDir(
        /* [in] */ const String& pkgName);

    static CARAPI_(String) GetIconHashFile(
        /* [in] */ const String& pkgName);

    static CARAPI_(String) GetIconPackPkgPath(
        /* [in] */ const String& pkgName);

    static CARAPI_(String) GetIconPackResPath(
        /* [in] */ const String& pkgName);

    static CARAPI_(String) GetIdmapPath(
        /* [in] */ const String& targetPkgName,
        /* [in] */ const String& overlayPkgName);

    static CARAPI_(String) GetOverlayPathToTarget(
        /* [in] */ const String& targetPkgName);

    static CARAPI_(String) GetCommonPackageName(
        /* [in] */ const String& themePackageName);

    static CARAPI CreateCacheDirIfNotExists();

    static CARAPI CreateResourcesDirIfNotExists(
        /* [in] */ const String& targetPkgName,
        /* [in] */ const String& overlayPkgName);

    static CARAPI CreateIconDirIfNotExists(
        /* [in] */ const String& pkgName);

    /**
     * Create SYSTEM_THEME_PATH directory if it does not exist
     */
    static CARAPI_(void) CreateThemeDirIfNotExists();

    /**
     * Create SYSTEM_FONT_PATH directory if it does not exist
     */
    static CARAPI_(void) CreateFontDirIfNotExists();

    /**
     * Create SYSTEM_THEME_RINGTONE_PATH directory if it does not exist
     */
    static CARAPI_(void) CreateRingtoneDirIfNotExists();

    /**
     * Create SYSTEM_THEME_NOTIFICATION_PATH directory if it does not exist
     */
    static CARAPI_(void) CreateNotificationDirIfNotExists();

    /**
     * Create SYSTEM_THEME_ALARM_PATH directory if it does not exist
     */
    static CARAPI_(void) CreateAlarmDirIfNotExists();

    /**
     * Create SYSTEM_THEME_ICON_CACHE_DIR directory if it does not exist
     */
    static CARAPI_(void) CreateIconCacheDirIfNotExists();

    static CARAPI_(void) ClearIconCache();

    static CARAPI GetInputStreamFromAsset(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& path,
        /* [out] */ IInputStream** inputStream);

    static CARAPI_(void) CloseQuietly(
        /* [in] */ IInputStream* stream);

    static CARAPI_(void) CloseQuietly(
        /* [in] */ IOutputStream* stream);

    /**
     * Scale the boot animation to better fit the device by editing the desc.txt found
     * in the bootanimation.zip
     * @param context Context to use for getting an instance of the WindowManager
     * @param input InputStream of the original bootanimation.zip
     * @param dst Path to store the newly created bootanimation.zip
     * @throws IOException
     */
    static CARAPI CopyAndScaleBootAnimation(
        /* [in] */ IContext* context,
        /* [in] */ IInputStream* input,
        /* [in] */ const String& dst);

    static CARAPI_(Boolean) IsValidAudible(
        /* [in] */ const String& fileName);

    static CARAPI_(Boolean) SetAudible(
        /* [in] */ IContext* context,
        /* [in] */ IFile* ringtone,
        /* [in] */ Int32 type,
        /* [in] */ const String& name);

    static CARAPI_(Boolean) SetDefaultAudible(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type);

    static CARAPI_(String) GetDefaultAudiblePath(
        /* [in] */ Int32 type);

    static CARAPI_(void) ClearAudibles(
        /* [in] */ IContext* context,
        /* [in] */ const String& audiblePath);

    static CARAPI_(AutoPtr<IContext>) CreateUiContext(
        /* [in] */ IContext* context);

    static CARAPI_(void) RegisterThemeChangeReceiver(
        /* [in] */ IContext* context,
        /* [in] */ IBroadcastReceiver* receiver);

    static CARAPI GetLockscreenWallpaperPath(
        /* [in] */ IAssetManager* assetManager,
        /* [out] */ String* path);

    static CARAPI GetWallpaperPath(
        /* [in] */ IAssetManager* assetManager,
        /* [out] */ String* path);

    static CARAPI_(String) GetDefaultThemePackageName(
        /* [in] */ IContext* context);

    // Returns a mutable list of all theme components
    static CARAPI_(AutoPtr<List<String> >) GetAllComponents();

    /**
     *  Returns a mutable list of all the theme components supported by a given package
     *  NOTE: This queries the themes content provider. If there isn't a provider installed
     *  or if it is too early in the boot process this method will not work.
     */
    static CARAPI_(AutoPtr<List<String> >) GetSupportedComponents(
        /* [in] */ IContext* context,
        /* [in] */ const String& pkgName);

    /**
     * Get the components from the default theme.  If the default theme is not SYSTEM then any
     * components that are not in the default theme will come from SYSTEM to create a complete
     * component map.
     * @param context
     * @return
     */
    static AutoPtr<HashMap<String, String> > GetDefaultComponents(
        /* [in] */ IContext* context);

    /**
     * Takes an existing component map and adds any missing components from the default
     * map of components.
     * @param context
     * @param componentMap An existing component map
     */
    static CARAPI_(void) CompleteComponentMap(
        /* [in] */ IContext* context,
        /* [in] */ HashMap<String, String>* componentMap);

    /**
     * Get the boot theme by accessing the settings.db directly instead of using a content resolver.
     * Only use this when the system is starting up and the settings content provider is not ready.
     *
     * Note: This method will only succeed if the system is calling this since normal apps will not
     * be able to access the settings db path.
     *
     * @return The boot theme or null if unable to read the database or get the entry for theme
     *         config
     */
    static CARAPI_(AutoPtr<IThemeConfig>) GetBootThemeDirty();

    /**
     * Convenience method to determine if a theme component is a per app theme and not a standard
     * component.
     * @param component
     * @return
     */
    static CARAPI_(Boolean) IsPerAppThemeComponent(
        /* [in] */ const String& component);

private:
    static CARAPI_(Boolean) DirExists(
        /* [in] */ const String& dirPath);

    static CARAPI_(void) CreateDirIfNotExists(
        /* [in] */ const String& dirPath);

    // Returns the first non-empty asset name. Empty assets can occur if the APK is built
    // with folders included as zip entries in the APK. Searching for files inside "folderName" via
    // assetManager.list("folderName") can cause these entries to be included as empty strings.
    static CARAPI_(String) GetFirstNonEmptyAsset(
        /* [in] */ ArrayOf<String>* assets);

public:
    /* Path inside a theme APK to the overlay folder */
    static const String OVERLAY_PATH;
    static const String ICONS_PATH;
    static const String COMMON_RES_PATH;
    static const String FONT_XML;
    static const String RESOURCE_CACHE_DIR;
    static const String IDMAP_SUFFIX;
    static const String COMMON_RES_SUFFIX;
    static const String COMMON_RES_TARGET;
    static const String ICON_HASH_FILENAME;

    // path to external theme resources, i.e. bootanimation.zip
    static const String SYSTEM_THEME_PATH;
    static const String SYSTEM_THEME_FONT_PATH;
    static const String SYSTEM_THEME_RINGTONE_PATH;
    static const String SYSTEM_THEME_NOTIFICATION_PATH;
    static const String SYSTEM_THEME_ALARM_PATH;
    static const String SYSTEM_THEME_ICON_CACHE_DIR;
    // internal path to bootanimation.zip inside theme apk
    static const String THEME_BOOTANIMATION_PATH;

    static const String SYSTEM_MEDIA_PATH;
    static const String SYSTEM_ALARMS_PATH;
    static const String SYSTEM_RINGTONES_PATH;
    static const String SYSTEM_NOTIFICATIONS_PATH;

    // Constants for theme change broadcast
    static const String ACTION_THEME_CHANGED;
    static const String CATEGORY_THEME_COMPONENT_PREFIX;
    static const String EXTRA_COMPONENTS;
    static const String EXTRA_REQUEST_TYPE;
    static const String EXTRA_UPDATE_TIME;

    static const Int32 SYSTEM_TARGET_API = 0;

    // Actions in manifests which identify legacy icon packs
    static const AutoPtr<ArrayOf<String> > sSupportedActions;

    // Categories in manifests which identify legacy icon packs
    static const AutoPtr<ArrayOf<String> > sSupportedCategories;

private:
    static const String TAG;

    static const String MEDIA_CONTENT_URI;

    // Package name for any app which does not have a specific theme applied
    static const String DEFAULT_PKG;

    static const String SETTINGS_DB;
    static const String SETTINGS_SECURE_TABLE;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__
