
#ifndef __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__
#define __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__

#include "_Elastosx_Security_Auth_CSubjectDomainCombiner.h"
#include "Object.h"

using Elastos::Security::IProtectionDomain;
using Elastos::Core::Object;
using Elastos::Security::IDomainCombiner;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CSubjectDomainCombiner)
    , public Object
    , public IDomainCombiner
    , public ISubjectDomainCombiner
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Combine(
        /* [in] */ ArrayOf<IProtectionDomain*> *current,
        /* [in] */ ArrayOf<IProtectionDomain*> *assigned,
        /* [out, callee] */ ArrayOf<IProtectionDomain*> **result);

    CARAPI GetSubject(
        /* [out] */ ISubject **subject);

    CARAPI constructor(
        /* [in] */ ISubject *sub);
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__
