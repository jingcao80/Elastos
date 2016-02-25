
#include "CSubjectDomainCombiner.h"

using Elastos::Security::EIID_IDomainCombiner;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CSubjectDomainCombiner)

CAR_INTERFACE_IMPL_2(CSubjectDomainCombiner, Object, IDomainCombiner, ISubjectDomainCombiner)

ECode CSubjectDomainCombiner::Combine(
    /* [in] */ ArrayOf<IProtectionDomain*> *current,
    /* [in] */ ArrayOf<IProtectionDomain*> *assigned,
    /* [out, callee] */ ArrayOf<IProtectionDomain*> **result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(current)
    VALIDATE_NOT_NULL(assigned)

    return NOERROR;
}

ECode CSubjectDomainCombiner::GetSubject(
    /* [out] */ ISubject **subject)
{
    VALIDATE_NOT_NULL(subject)
    *subject = NULL;

    return NOERROR;
}

ECode CSubjectDomainCombiner::constructor(
    /* [in] */ ISubject *sub)
{
    VALIDATE_NOT_NULL(sub)

    return NOERROR;
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx
