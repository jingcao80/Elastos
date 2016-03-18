
#include "elastos/droid/webkit/CWebStorageHelper.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebStorageHelper, Object, IWebStorageHelper);

CAR_SINGLETON_IMPL(CWebStorageHelper);

ECode CWebStorageHelper::GetInstance(
    /* [out] */ IWebStorage ** ppInstance)
{
    VALIDATE_NOT_NULL(ppInstance);
    WebViewFactory::GetProvider()->GetWebStorage(ppInstance);
    REFCOUNT_ADD(*ppInstance);
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
