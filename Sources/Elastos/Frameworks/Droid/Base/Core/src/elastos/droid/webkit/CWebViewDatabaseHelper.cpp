
#include "elastos/droid/webkit/CWebViewDatabaseHelper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewDatabaseHelper, Object, IWebViewDatabaseHelper);

CAR_SINGLETON_IMPL(CWebViewDatabaseHelper);

ECode CWebViewDatabaseHelper::GetInstance(
    /* [in] */ IContext * pContext,
    /* [out] */ IWebViewDatabase ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
