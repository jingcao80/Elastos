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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWGEOLOCATIONPERMISSIONS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWGEOLOCATIONPERMISSIONS_H__

#include <Elastos.Droid.Webkit.h>
#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Webkit::IValueCallback;

using Elastos::Core::IRunnable;
using Elastos::Utility::ISet;
// import java.util.HashSet;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This class is used to manage permissions for the WebView's Geolocation JavaScript API.
 *
 * Callbacks are posted on the UI thread.
 */
class AwGeolocationPermissions
    : public Object
{
private:
    class GetAllowedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        GetAllowedRunnable(
            /* [in] */ AwGeolocationPermissions* owner,
            /* [in] */ IValueCallback* callback,
            /* [in] */ Boolean finalAllowed);

        CARAPI Run();

    private:
        AwGeolocationPermissions* mOwner;
        AutoPtr<IValueCallback> mCallback;
        Boolean mFinalAllowed;
    };

    class GetOriginsRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        GetOriginsRunnable(
            /* [in] */ AwGeolocationPermissions* owner,
            /* [in] */ IValueCallback* callback);

        CARAPI Run();

    private:
        AwGeolocationPermissions* mOwner;
        AutoPtr<IValueCallback> mCallback;
        AutoPtr<ISet> mOrigins;
    };

public:
    AwGeolocationPermissions(
        /* [in] */ ISharedPreferences* sharedPreferences);

    /**
     * Set one origin to be allowed.
     */
    CARAPI_(void) Allow(
        /* [in] */ const String& origin);

    /**
     * Set one origin to be denied.
     */
    CARAPI_(void) Deny(
        /* [in] */ const String& origin);

    /**
     * Clear the stored permission for a particular origin.
     */
    CARAPI_(void) Clear(
        /* [in] */ const String& origin);

    /**
     * Clear stored permissions for all origins.
     */
    CARAPI_(void) ClearAll();

    /**
     * Synchronous method to get if an origin is set to be allowed.
     */
    CARAPI_(Boolean) IsOriginAllowed(
        /* [in] */ const String& origin);

    /**
     * Returns true if the origin is either set to allowed or denied.
     */
    CARAPI_(Boolean) HasOrigin(
        /* [in] */ const String& origin);

    /**
     * Asynchronous method to get if an origin set to be allowed.
     */
    CARAPI_(void) GetAllowed(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    /**
     * Async method to get the domains currently allowed or denied.
     */
    CARAPI_(void) GetOrigins(
        /* [in] */ IValueCallback* callback);

private:
    /**
     * Get the domain of an URL using the GURL library.
     */
    CARAPI_(String) GetOriginKey(
        /* [in] */ const String& url);

private:
    static const String PREF_PREFIX;
    AutoPtr<ISharedPreferences> mSharedPreferences;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWGEOLOCATIONPERMISSIONS_H__
