
#include "elastos/droid/internal/view/CActionBarPolicyHelper.h"
#include "elastos/droid/internal/view/ActionBarPolicy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CActionBarPolicyHelper, Singleton, IActionBarPolicyHelper)

CAR_SINGLETON_IMPL(CActionBarPolicyHelper)

ECode CActionBarPolicyHelper::Get(
    /* [in] */ IContext* context,
    /* [out] */ IActionBarPolicy** policy)
{
    VALIDATE_NOT_NULL(policy)
    AutoPtr<IActionBarPolicy> temp = ActionBarPolicy::Get(context);
    *policy = temp;
    REFCOUNT_ADD(*policy)
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
