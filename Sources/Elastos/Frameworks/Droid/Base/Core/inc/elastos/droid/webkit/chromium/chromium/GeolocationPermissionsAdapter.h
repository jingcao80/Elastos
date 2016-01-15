
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H__

// import android.webkit.GeolocationPermissions;
// import android.webkit.ValueCallback;

// import org.chromium.android_webview.AwGeolocationPermissions;

// import java.util.Set;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * Chromium implementation of GeolocationPermissions -- forwards calls to the
 * chromium internal implementation.
 */
class GeolocationPermissionsAdapter : public GeolocationPermissions
{
public:
    GeolocationPermissionsAdapter(
        /* [in] */ AwGeolocationPermissions* chromeGeolocationPermissions);

    //@Override
    CARAPI Allow(
        /* [in] */ const String& origin);

    //@Override
    CARAPI Clear(
        /* [in] */ const String& origin);

    //@Override
    CARAPI ClearAll();

    //@Override
    CARAPI GetAllowed(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    //@Override
    CARAPI GetOrigins(
        /* [in] */ IValueCallback* callback);

private:
    AutoPtr<AwGeolocationPermissions> mChromeGeolocationPermissions;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_GEOLOCATIONPERMISSIONSADAPTER_H__
