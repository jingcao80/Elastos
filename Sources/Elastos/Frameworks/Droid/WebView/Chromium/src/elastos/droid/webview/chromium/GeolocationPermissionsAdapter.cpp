
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


