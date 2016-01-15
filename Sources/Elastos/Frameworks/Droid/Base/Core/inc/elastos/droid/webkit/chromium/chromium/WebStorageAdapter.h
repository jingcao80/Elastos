
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBSTORAGEADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBSTORAGEADAPTER_H__

// import android.webkit.ValueCallback;
// import android.webkit.WebStorage;

// import java.util.HashMap;
// import java.util.Map;

// import org.chromium.android_webview.AwQuotaManagerBridge;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * Chromium implementation of WebStorage -- forwards calls to the
 * chromium internal implementation.
 */
class WebStorageAdapter
    : public WebStorage
{
private:
    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        InnerValueCallback(
            /* [in] */ WebStorageAdapter* owner,
            /* [in] */ const IValueCallback* callback);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* value);

    private:
        WebStorageAdapter* mOwner;
        const IValueCallback* mCallback;
    };

public:
    WebStorageAdapter(
        /* [in] */ AwQuotaManagerBridge* quotaManagerBridge);

    //@Override
    CARAPI GetOrigins(
        /* [in] */ const IValueCallback* callback);

    //@Override
    CARAPI GetUsageForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    //@Override
    CARAPI GetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    //@Override
    CARAPI SetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ Int64 quota);

    //@Override
    CARAPI DeleteOrigin(
        /* [in] */ const String& origin);

    //@Override
    CARAPI DeleteAllData();

private:
    const AutoPtr<AwQuotaManagerBridge> mQuotaManagerBridge;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBSTORAGEADAPTER_H__
