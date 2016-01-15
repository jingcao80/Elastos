
#include "elastos/droid/transition/CTransitionInflaterHelper.h"
#include "elastos/droid/transition/TransitionInflater.h"

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionInflaterHelper::
//===============================================================

CAR_SINGLETON_IMPL(CTransitionInflaterHelper)

CAR_INTERFACE_IMPL(CTransitionInflaterHelper, Singleton, ITransitionInflaterHelper)

ECode CTransitionInflaterHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ ITransitionInflater** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITransitionInflater> p = TransitionInflater::From(context);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
