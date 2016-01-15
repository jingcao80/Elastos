
#include "elastos/droid/webkit/CWebChromeClientFileChooserParamsHelper.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebChromeClientFileChooserParamsHelper, Object, IWebChromeClientFileChooserParamsHelper);

CAR_SINGLETON_IMPL(CWebChromeClientFileChooserParamsHelper);

ECode CWebChromeClientFileChooserParamsHelper::ParseResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data,
    /* [out, callee] */ ArrayOf<IUri*>** uriArrayOut)
{
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    WebViewFactory::GetProvider()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    return statics->ParseFileChooserResult(resultCode, data, uriArrayOut);
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
