
#include "elastos/droid/webkit/CWebViewHelper.h"
#include "elastos/droid/webkit/WebView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewHelper, Object, IWebViewHelper);

CAR_SINGLETON_IMPL(CWebViewHelper);

ECode CWebViewHelper::EnablePlatformNotifications()
{
    WebView::EnablePlatformNotifications();
    return NOERROR;
}

ECode CWebViewHelper::DisablePlatformNotifications()
{
    WebView::DisablePlatformNotifications();
    return NOERROR;
}

ECode CWebViewHelper::FindAddress(
    /* [in] */ const String& addr,
    /* [out] */ String * outAddr)
{
    VALIDATE_NOT_NULL(outAddr);
    *outAddr = WebView::FindAddress(addr);

    return NOERROR;
}

ECode CWebViewHelper::GetPluginList(
    /* [out] */ IPluginList ** pluginList)
{
    VALIDATE_NOT_NULL(pluginList);
    AutoPtr<IPluginList> _pluginList = WebView::GetPluginList();
    *pluginList = _pluginList.Get();
    REFCOUNT_ADD(*pluginList);

    return NOERROR;
}

ECode CWebViewHelper::ClearClientCertPreferences(
    /* [in] */ IRunnable* onCleared)
{
    return WebView::ClearClientCertPreferences(onCleared);
}

ECode CWebViewHelper::EnableSlowWholeDocumentDraw()
{
    return WebView::EnableSlowWholeDocumentDraw();
}

ECode CWebViewHelper::SetWebContentsDebuggingEnabled(
    /* [in] */ Boolean enabled)
{
    return WebView::SetWebContentsDebuggingEnabled(enabled);
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos