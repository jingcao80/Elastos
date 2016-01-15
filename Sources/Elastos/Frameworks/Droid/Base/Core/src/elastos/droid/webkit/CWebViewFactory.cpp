
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/CWebViewFactory.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewFactory, Object, IWebViewFactory);

CAR_SINGLETON_IMPL(CWebViewFactory);

ECode CWebViewFactory::GetWebViewPackageName(
        /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = WebViewFactory::GetWebViewPackageName();
    return NOERROR;
}

ECode CWebViewFactory::GetLoadedPackageInfo(
    /* [out] */ IPackageInfo** packageInfo)
{
    VALIDATE_NOT_NULL(packageInfo);
    AutoPtr<IPackageInfo> _packageInfo = WebViewFactory::GetLoadedPackageInfo();
    *packageInfo = _packageInfo;
    REFCOUNT_ADD(*packageInfo);
    return NOERROR;
}

ECode CWebViewFactory::PrepareWebViewInZygote()
{
    return WebViewFactory::PrepareWebViewInZygote();
}

ECode CWebViewFactory::PrepareWebViewInSystemServer()
{
    return WebViewFactory::PrepareWebViewInSystemServer();
}

ECode CWebViewFactory::OnWebViewUpdateInstalled()
{
    return WebViewFactory::OnWebViewUpdateInstalled();
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
