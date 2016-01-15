
#ifndef __ELASTOS_DROID_WEBKIT_URLUTIL_H__
#define __ELASTOS_DROID_WEBKIT_URLUTIL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Webkit {

class URLUtil
{
public:
    static CARAPI_(String) GuessUrl(
        /* [in] */ const String& inUrl);

    static CARAPI_(String) ComposeSearchUrl(
        /* [in] */ const String& inQuery,
        /* [in] */ const String& templateStr,
        /* [in] */ const String& queryPlaceHolder);

    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* url,
        /* [out, callee] */ ArrayOf<Byte>** urlList);

    /**
     * @return True iff the url is correctly URL encoded
     */
    static CARAPI_(Boolean) VerifyURLEncoding(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsAssetUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsResourceUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsCookielessProxyUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsFileUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsAboutUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsDataUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsJavaScriptUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsHttpUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsHttpsUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsNetworkUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsContentUrl(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsValidUrl(
        /* [in] */ const String& url);

    static CARAPI_(String) StripAnchor(
        /* [in] */ const String& url);

    static CARAPI_(String) GuessFileName(
        /* [in] */ const String& url,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType);

    /*
     * Parse the Content-Disposition HTTP Header. The format of the header
     * is defined here: http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html
     * This header provides a filename for content that is going to be
     * downloaded to the file system. We only support the attachment type.
     * Note that RFC 2616 specifies the filename value must be double-quoted.
     * Unfortunately some servers do not quote the value so to maintain
     * consistent behaviour with other browsers, we allow unquoted values too.
     */
    static CARAPI_(String) ParseContentDisposition(
        /* [in] */ const String& contentDisposition);

private:
    static CARAPI ParseHex(
        /* [in] */ Byte b,
        /* [out] */ Int32* outByte);

public:
    // to refer to bar.png under your package's asset/foo/ directory, use
    // "file:///android_asset/foo/bar.png".
    static const String ASSET_BASE;
    // to refer to bar.png under your package's res/drawable/ directory, use
    // "file:///android_res/drawable/bar.png". Use "drawable" to refer to
    // "drawable-hdpi" directory as well.
    static const String RESOURCE_BASE;
    static const String FILE_BASE;
    static const String PROXY_BASE;
    static const String CONTENT_BASE;

private:
    static const String LOGTAG;
    /** Regex used to parse content-disposition headers */
    static AutoPtr<IPattern> CONTENT_DISPOSITION_PATTERN;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_URLUTIL_H__
