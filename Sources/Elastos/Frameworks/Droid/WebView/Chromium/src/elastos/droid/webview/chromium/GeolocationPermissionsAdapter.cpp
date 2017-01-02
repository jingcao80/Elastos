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

#include "elastos/droid/webkit/webview/chromium/GeolocationPermissionsAdapter.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                    GeolocationPermissionsAdapter
//=====================================================================
GeolocationPermissionsAdapter::GeolocationPermissionsAdapter(
    /* [in] */ AwGeolocationPermissions* chromeGeolocationPermissions)
    : mChromeGeolocationPermissions(chromeGeolocationPermissions)
{
    // ==================before translated======================
    // mChromeGeolocationPermissions = chromeGeolocationPermissions;
}

ECode GeolocationPermissionsAdapter::Allow(
    /* [in] */ const String& origin)
{
    // ==================before translated======================
    // mChromeGeolocationPermissions.allow(origin);

    mChromeGeolocationPermissions->Allow(origin);
    return NOERROR;
}

ECode GeolocationPermissionsAdapter::Clear(
    /* [in] */ const String& origin)
{
    // ==================before translated======================
    // mChromeGeolocationPermissions.clear(origin);

    mChromeGeolocationPermissions->Clear(origin);
    return NOERROR;
}

ECode GeolocationPermissionsAdapter::ClearAll()
{
    // ==================before translated======================
    // mChromeGeolocationPermissions.clearAll();

    mChromeGeolocationPermissions->ClearAll();
    return NOERROR;
}

ECode GeolocationPermissionsAdapter::GetAllowed(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // mChromeGeolocationPermissions.getAllowed(origin, callback);

    mChromeGeolocationPermissions->GetAllowed(origin, callback);
    return NOERROR;
}

ECode GeolocationPermissionsAdapter::GetOrigins(
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // mChromeGeolocationPermissions.getOrigins(callback);

    mChromeGeolocationPermissions->GetOrigins(callback);
    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


