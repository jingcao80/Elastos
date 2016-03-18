
#include "elastos/droid/webkit/CWebViewDatabaseHelper.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewDatabaseHelper, Object, IWebViewDatabaseHelper);

CAR_SINGLETON_IMPL(CWebViewDatabaseHelper);

ECode CWebViewDatabaseHelper::GetInstance(
    /* [in] */ IContext * pContext,
    /* [out] */ IWebViewDatabase ** ppInstance)
{
    VALIDATE_NOT_NULL(ppInstance);
    WebViewFactory::GetProvider()->GetWebViewDatabase(pContext, ppInstance);
    REFCOUNT_ADD(*ppInstance);
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
