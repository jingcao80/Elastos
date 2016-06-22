
#ifndef __ELASTOS_DROID_CONTENT_RES_THEMECONFIG_H__
#define __ELASTOS_DROID_CONTENT_RES_THEMECONFIG_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IJsonWriter;
using Elastos::Droid::Utility::IJsonReader;
using Elastos::Core::ICloneable;
using Elastos::Core::IComparable;
using Elastos::IO::IReader;
using Elastos::IO::IWriter;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

class CThemeConfigBuilder;

/**
 * The Theme Configuration allows lookup of a theme element (fonts, icon, overlay) for a given
 * application. If there isn't a particular theme designated to an app, it will fallback on the
 * default theme. If there isn't a default theme then it will simply fallback to holo.
 *
 * @hide
 */
class ThemeConfig
    : public Object
    , public IThemeConfig
    , public ICloneable
    , public IParcelable
    , public IComparable
{
public:
    class AppTheme
        : public Object
        , public IAppTheme
        , public ICloneable
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ const String& overlayPkgName,
            /* [in] */ const String& iconPkgName,
            /* [in] */ const String& fontPkgName);

        CARAPI GetIconPackPkgName(
            /* [out] */ String* pkgName);

        CARAPI GetOverlayPkgName(
            /* [out] */ String* pkgName);

        CARAPI GetFontPackPkgName(
            /* [out] */ String* pkgName);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI Clone(
        /* [out] */ IInterface** object);

    protected:
        // If any field is modified or added here be sure to change the serializer accordingly
        String mOverlayPkgName;
        String mIconPkgName;
        String mFontPkgName;
    };

    class JsonSerializer
    {
    public:
        static CARAPI_(String) ToJson(
            /* [in] */ IThemeConfig* theme);

        static CARAPI_(AutoPtr<IThemeConfig>) FromJson(
            /* [in] */ const String& json);

    private:
        static CARAPI WriteTheme(
            /* [in] */ IJsonWriter* writer,
            /* [in] */ IThemeConfig* theme);

        static CARAPI WriteAppTheme(
            /* [in] */ IJsonWriter* writer,
            /* [in] */ IAppTheme* appTheme);

        static CARAPI ReadAppTheme(
            /* [in] */ IJsonReader* reader,
            /* [out] */ IAppTheme** appTheme);

        static CARAPI_(void) CloseQuietly(
            /* [in] */ IReader* reader);

        static CARAPI_(void) CloseQuietly(
            /* [in] */ IJsonReader* reader);

        static CARAPI_(void) CloseQuietly(
            /* [in] */ IWriter* writer);

        static CARAPI_(void) CloseQuietly(
            /* [in] */ IJsonWriter* writer);

    private:
        static const String NAME_OVERLAY_PKG;
        static const String NAME_ICON_PKG;
        static const String NAME_FONT_PKG;

        friend class ThemeConfig;
    };

public:
    ThemeConfig();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IMap* appThemes);

    CARAPI GetOverlayPkgName(
        /* [out] */ String* str);

    CARAPI GetOverlayForStatusBar(
        /* [out] */ String* str);

    CARAPI GetOverlayForNavBar(
        /* [out] */ String* str);

    CARAPI GetOverlayPkgNameForApp(
        /* [in] */ const String& appPkgName,
        /* [out] */ String* str);

    CARAPI GetIconPackPkgName(
        /* [out] */ String* str);

    CARAPI GetIconPackPkgNameForApp(
        /* [in] */ const String& appPkgName,
        /* [out] */ String* str);

    CARAPI GetFontPkgName(
        /* [out] */ String* pkgName);

    CARAPI GetFontPkgNameForApp(
        /* [in] */ const String& appPkgName,
        /* [out] */ String* str);

    CARAPI GetAppThemes(
        /* [out] */ IMap** themes);

    CARAPI GetLastThemeChangeRequestType(
        /* [out] */ RequestType* requestType);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToJson(
        /* [out] */ String* str);

    static CARAPI_(AutoPtr<IThemeConfig>) FromJson(
        /* [in] */ const String& json);

    /**
     * Represents the theme that the device booted into. This is used to
     * simulate a "default" configuration based on the user's last known
     * preference until the theme is switched at runtime.
     */
    static CARAPI GetBootTheme(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IThemeConfig** theme);

    static CARAPI GetBootThemeForUser(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 userHandle,
        /* [out] */ IThemeConfig** theme);

    /**
     * Represents the system framework theme, perceived by the system as there
     * being no theme applied.
     */
    static CARAPI_(AutoPtr<IThemeConfig>) GetSystemTheme();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

    CARAPI Clone(
        /* [out] */ IInterface** object);

private:
    CARAPI_(AutoPtr<IAppTheme>) GetThemeFor(
        /* [in] */ const String& pkgName);

    CARAPI_(AutoPtr<IAppTheme>) GetDefaultTheme();

    static AutoPtr<IAppTheme> GetSystemAppTheme();

public:
    static const String TAG;

protected:
    // Maps pkgname to theme (ex com.angry.birds -> red theme)
    AutoPtr<IMap> mThemes;

private:
    // Key for any app which does not have a specific theme applied
    static const String KEY_DEFAULT_PKG;
    static AutoPtr<IThemeConfig> sSystemConfig;
    static AutoPtr<IAppTheme> sSystemAppTheme;

    RequestType mLastThemeChangeRequestType;

    friend class CThemeConfigBuilder;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_THEMECONFIG_H__
