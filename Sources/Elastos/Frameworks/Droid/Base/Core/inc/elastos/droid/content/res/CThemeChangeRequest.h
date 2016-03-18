
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUEST_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUEST_H__

#include "_Elastos_Droid_Content_Res_CThemeChangeRequest.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

class CThemeChangeRequestBuilder;

CarClass(CThemeChangeRequest)
    , public Object
    , public IThemeChangeRequest
    , public IParcelable
{
public:
    CThemeChangeRequest();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetOverlayThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetStatusBarThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetNavBarThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetFontThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetIconsThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetBootanimationThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetWallpaperThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetLockWallpaperThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetAlarmThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetNotificationThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetRingtoneThemePackageName(
        /* [out] */ String* pkgName);

    CARAPI GetThemeComponentsMap(
        /* [out] */ IMap** components);

    CARAPI SetThemeComponentsMap(
        /* [in] */ IMap* components);

    /**
     * Get the mapping for per app themes
     * @return A mapping of apps and the theme to apply for each one. or null if none set.
     */
    CARAPI GetPerAppOverlays(
        /* [out] */ IMap** components);

    CARAPI SetPerAppOverlays(
        /* [in] */ IMap* components);

    CARAPI GetNumChangesRequested(
        /* [out] */ Int32* num);

    CARAPI GetReqeustType(
        /* [out] */ RequestType* type);

    CARAPI SetReqeustType(
        /* [in] */ RequestType type);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    CARAPI_(String) GetThemePackageNameForComponent(
        /* [in] */ const String& componentName);

    CARAPI constructor(
        /* [in] */ IMap* components,
        /* [in] */ IMap* perAppThemes,
        /* [in] */ RequestType requestType);

private:
    AutoPtr<IMap> mThemeComponents;
    AutoPtr<IMap> mPerAppOverlays;
    RequestType mRequestType;

    friend class CThemeChangeRequestBuilder;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUEST_H__
