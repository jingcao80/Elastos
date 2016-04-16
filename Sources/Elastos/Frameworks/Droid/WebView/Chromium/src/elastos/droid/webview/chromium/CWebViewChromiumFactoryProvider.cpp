
#include "elastos/droid/webkit/webview/chromium/CWebViewChromiumFactoryProvider.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

CAR_OBJECT_IMPL(CWebViewChromiumFactoryProvider)

ECode CWebViewChromiumFactoryProvider::HasStarted(
    /* [out] */ Boolean* result)
{
    return WebViewChromiumFactoryProvider::HasStarted(result);
}

ECode CWebViewChromiumFactoryProvider::StartYourEngines(
    /* [in] */ Boolean onMainThread)
{
    return WebViewChromiumFactoryProvider::StartYourEngines(onMainThread);
}

ECode CWebViewChromiumFactoryProvider::GetBrowserContext(
    /* [out] */ IInterface** result)
{
    return WebViewChromiumFactoryProvider::GetBrowserContext(result);
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

