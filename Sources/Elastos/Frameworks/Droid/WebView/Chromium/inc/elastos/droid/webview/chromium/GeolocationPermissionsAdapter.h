
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

