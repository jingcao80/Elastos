#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwDevToolsServer.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwDevToolsServer_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

AwDevToolsServer::AwDevToolsServer()
{
    mNativeDevToolsServer = NativeInitRemoteDebugging();
}

void AwDevToolsServer::Destroy() {
    NativeDestroyRemoteDebugging(mNativeDevToolsServer);
    mNativeDevToolsServer = 0;
}

void AwDevToolsServer::SetRemoteDebuggingEnabled(
    /* [in] */ Boolean enabled)
{
    NativeSetRemoteDebuggingEnabled(mNativeDevToolsServer, enabled);
}

Int64 AwDevToolsServer::NativeInitRemoteDebugging()
{
    return Elastos_AwDevToolsServer_nativeInitRemoteDebugging(TO_IINTERFACE(this));
}

void AwDevToolsServer::NativeDestroyRemoteDebugging(
    /* [in] */ Int64 devToolsServer)
{
    Elastos_AwDevToolsServer_nativeDestroyRemoteDebugging(TO_IINTERFACE(this), devToolsServer);
}

void AwDevToolsServer::NativeSetRemoteDebuggingEnabled(
    /* [in] */ Int64 devToolsServer,
    /* [in] */ Boolean enabled)
{
    Elastos_AwDevToolsServer_nativeSetRemoteDebuggingEnabled(TO_IINTERFACE(this), devToolsServer, enabled);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
