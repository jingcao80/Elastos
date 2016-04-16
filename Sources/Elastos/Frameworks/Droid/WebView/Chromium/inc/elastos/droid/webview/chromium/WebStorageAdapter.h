
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwQuotaManagerBridge.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwQuotaManagerBridge;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IWebStorage;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of WebStorage -- forwards calls to the
  * chromium internal implementation.
  */
class WebStorageAdapter
    : public Object
    , public IWebStorage
{
private:
    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerValueCallback(
            /* [in] */ WebStorageAdapter* owner,
            /* [in] */ IValueCallback* callback);

        // @Override
        CARAPI OnReceiveValue(
            /* [in] */ IInterface* origins);

    private:
        WebStorageAdapter* mOwner;
        IValueCallback* mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    WebStorageAdapter(
        /* [in] */ AwQuotaManagerBridge* quotaManagerBridge);

    // @Override
    CARAPI GetOrigins(
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetUsageForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI SetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ Int64 quota);

    // @Override
    CARAPI DeleteOrigin(
        /* [in] */ const String& origin);

    // @Override
    CARAPI DeleteAllData();

private:
    /*const*/ AutoPtr<AwQuotaManagerBridge> mQuotaManagerBridge;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_

