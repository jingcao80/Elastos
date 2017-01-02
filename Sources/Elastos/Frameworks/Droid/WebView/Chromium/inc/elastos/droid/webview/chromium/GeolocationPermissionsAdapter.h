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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/GeolocationPermissions.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwGeolocationPermissions.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwGeolocationPermissions;
using Elastos::Droid::Webkit::GeolocationPermissions;
using Elastos::Droid::Webkit::IValueCallback;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of GeolocationPermissions -- forwards calls to the
  * chromium internal implementation.
  */
class GeolocationPermissionsAdapter
    : public GeolocationPermissions
{
public:
    GeolocationPermissionsAdapter(
        /* [in] */ AwGeolocationPermissions* chromeGeolocationPermissions);

    // @Override
    CARAPI Allow(
        /* [in] */ const String& origin);

    // @Override
    CARAPI Clear(
        /* [in] */ const String& origin);

    // @Override
    CARAPI ClearAll();

    // @Override
    CARAPI GetAllowed(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetOrigins(
        /* [in] */ IValueCallback* callback);

private:
    AutoPtr<AwGeolocationPermissions> mChromeGeolocationPermissions;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H_

