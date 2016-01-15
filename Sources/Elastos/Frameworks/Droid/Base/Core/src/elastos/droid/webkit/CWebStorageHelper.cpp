
#include "elastos/droid/webkit/CWebStorageHelper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebStorageHelper, Object, IWebStorageHelper);

CAR_SINGLETON_IMPL(CWebStorageHelper);

ECode CWebStorageHelper::GetInstance(
    /* [out] */ IWebStorage ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos