
#ifndef __ELASTOS_DROID_WEBKIT_CURLUTIL_H__
#define __ELASTOS_DROID_WEBKIT_CURLUTIL_H__

#include "_Elastos_Droid_Webkit_CURLUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CURLUtil)
    , public Singleton
    , public IURLUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GuessUrl(
        /* [in] */ const String& inUrl,
        /* [out] */ String* url);

    CARAPI ComposeSearchUrl(
        /* [in] */ const String& inQuery,
        /* [in] */ const String& tmp,
        /* [in] */ const String& queryPlaceHolder,
        /* [out] */ String* url);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* url,
        /* [out, callee] */ ArrayOf<Byte>** outUrl);

    CARAPI IsAssetUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsResourceUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsCookielessProxyUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsFileUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsAboutUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsDataUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsJavaScriptUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsHttpUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsHttpsUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsNetworkUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsContentUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsValidUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI StripAnchor(
        /* [in] */ const String& url,
        /* [out] */ String* result);

    CARAPI GuessFileName(
        /* [in] */ const String& url,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [out] */ String* fileName);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CURLUTIL_H__
