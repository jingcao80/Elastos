#include "elastos/droid/internal/policy/impl/CPhoneLayoutInflater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CAR_OBJECT_IMPL(CPhoneLayoutInflater)

ECode CPhoneLayoutInflater::CloneInContext(
    /* [in] */ IContext* newContext,
    /* [out] */ ILayoutInflater** newInflater)
{
    return CPhoneLayoutInflater::New(newContext, newInflater);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
