#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Controller for Remote Web Debugging (Developer Tools).
 */
//@JNINamespace("android_webview")
class AwDevToolsServer
    : public Object
{
public:
    AwDevToolsServer();

    CARAPI_(void) Destroy();

    CARAPI_(void) SetRemoteDebuggingEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(Int64) NativeInitRemoteDebugging();

    CARAPI_(void) NativeDestroyRemoteDebugging(
        /* [in] */ Int64 devToolsServer);

    CARAPI_(void) NativeSetRemoteDebuggingEnabled(
        /* [in] */ Int64 devToolsServer,
        /* [in] */ Boolean enabled);

    Int64 mNativeDevToolsServer;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__
