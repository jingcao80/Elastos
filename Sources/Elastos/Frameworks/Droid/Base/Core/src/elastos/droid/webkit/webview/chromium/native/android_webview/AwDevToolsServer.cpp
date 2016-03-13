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
    return Elastos_AwDevToolsServer_nativeInitRemoteDebugging(THIS_PROBE(IInterface));
}

void AwDevToolsServer::NativeDestroyRemoteDebugging(
    /* [in] */ Int64 devToolsServer)
{
    Elastos_AwDevToolsServer_nativeDestroyRemoteDebugging(THIS_PROBE(IInterface), devToolsServer);
}

void AwDevToolsServer::NativeSetRemoteDebuggingEnabled(
    /* [in] */ Int64 devToolsServer,
    /* [in] */ Boolean enabled)
{
    Elastos_AwDevToolsServer_nativeSetRemoteDebuggingEnabled(THIS_PROBE(IInterface), devToolsServer, enabled);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
