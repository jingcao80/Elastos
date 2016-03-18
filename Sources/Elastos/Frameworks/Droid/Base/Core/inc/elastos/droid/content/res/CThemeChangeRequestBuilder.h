
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUESTBUILDER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUESTBUILDER_H__

#include "_Elastos_Droid_Content_Res_CThemeChangeRequestBuilder.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CThemeChangeRequestBuilder)
    , public Object
    , public IThemeChangeRequestBuilder
{
public:
    CThemeChangeRequestBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IThemeConfig* themeConfig);

    CARAPI SetOverlay(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetStatusBar(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetNavBar(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetFont(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetIcons(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetBootanimation(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetWallpaper(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetLockWallpaper(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetAlarm(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetNotification(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetRingtone(
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetComponent(
        /* [in] */ const String& component,
        /* [in] */ const String& pkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetAppOverlay(
        /* [in] */ const String& appPkgName,
        /* [in] */ const String& themePkgName,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI SetRequestType(
        /* [in] */ RequestType requestType,
        /* [out] */ IThemeChangeRequestBuilder** builder);

    CARAPI Build(
        /* [out] */ IThemeChangeRequest** request);

private:
    CARAPI_(void) BuildChangeRequestFromThemeConfig(
        /* [in] */ IThemeConfig* themeConfig);

private:
    AutoPtr<IMap> mThemeComponents;
    AutoPtr<IMap> mPerAppOverlays;
    RequestType mRequestType;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTHEMECHANGEREQUESTBUILDER_H__
