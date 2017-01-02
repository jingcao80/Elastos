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
