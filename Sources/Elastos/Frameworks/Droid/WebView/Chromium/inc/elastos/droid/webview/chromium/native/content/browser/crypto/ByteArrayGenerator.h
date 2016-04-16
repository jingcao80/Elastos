
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CRYPTO_BYTEARRAYGENERATOR_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CRYPTO_BYTEARRAYGENERATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFileInputStream;
// import java.io.IOException;
// import java.security.GeneralSecurityException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Crypto {

/**
 * Generates byte arrays for use in crypto algorithms. Defaults to pulling random data
 * from /dev/urandom, but can be overwritten for other generation methods.
 */
class ByteArrayGenerator : public Object
{
public:
    /**
     * Polls random data to generate the array.
     * @param numBytes Length of the array to generate.
     * @return byte[] containing randomly generated data.
     */
    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetBytes(
        /* [in] */ Int32 numBytes);
};

} // namespace Crypto
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CRYPTO_BYTEARRAYGENERATOR_H__
