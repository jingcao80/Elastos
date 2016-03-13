#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWASSETS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWASSETS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

// import java.io.IOException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * A utility class to retrieve references to uncompressed assets insides the apk. A reference is
 * defined as tuple (file descriptor, offset, size) enabling direct mapping without deflation.
 */
//@JNINamespace("android_webview")
class AwAssets
    : public Object
{
public:
    //@CalledByNative
    static CARAPI_(AutoPtr<ArrayOf<Int64> >) OpenAsset(
        /* [in] */ IContext* context,
        /* [in] */ const String& fileName);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwAssetsCallback_Init();

private:
    static CARAPI_(AutoPtr<ArrayOf<Int64> >) OpenAsset(
        /* [in] */ IInterface* context,
        /* [in] */ const String& fileName);

private:
    static const String LOGTAG;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWASSETS_H__
