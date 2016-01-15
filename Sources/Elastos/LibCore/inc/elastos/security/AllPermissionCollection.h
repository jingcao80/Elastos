
#ifndef __ELASTOS_SECURITY_ALLPERMISSIONCOLLECTION_H__
#define __ELASTOS_SECURITY_ALLPERMISSIONCOLLECTION_H__

#include "PermissionCollection.h"

using Elastos::Utility::IEnumeration;

namespace Elastos {
namespace Security {

class AllPermissionCollection
    : public PermissionCollection
{
public:
    CARAPI Add(
        /* [in] */ IPermission *permission);

    CARAPI GetElements(
        /* [out] */ IEnumeration **elems);

    CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *ret);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_ALLPERMISSIONCOLLECTION_H__
