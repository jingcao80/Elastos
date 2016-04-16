
#include "elastos/droid/webkit/webview/chromium/native/net/GURLUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/GURLUtils_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

//=====================================================================
//                              GURLUtils
//=====================================================================
String GURLUtils::GetOrigin(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return nativeGetOrigin(url);

    return NativeGetOrigin(url);
}

String GURLUtils::GetScheme(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return nativeGetScheme(url);

    return NativeGetScheme(url);
}

String GURLUtils::NativeGetOrigin(
    /* [in] */ const String& url)
{
    return Elastos_GURLUtils_nativeGetOrigin(url);
}

String GURLUtils::NativeGetScheme(
    /* [in] */ const String& url)
{
    return Elastos_GURLUtils_nativeGetScheme(url);
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


