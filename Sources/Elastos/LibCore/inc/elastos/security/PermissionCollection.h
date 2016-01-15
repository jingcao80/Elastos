
#ifndef __ELASTOS_SECURITY_PERMISSIONCOLLECTION_H__
#define __ELASTOS_SECURITY_PERMISSIONCOLLECTION_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::IO::ISerializable;
using Elastos::Utility::IEnumeration;

namespace Elastos {
namespace Security {

class PermissionCollection
    : public Object
    , public IPermissionCollection
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL();

    virtual CARAPI IsReadOnly(
        /* [out] */ Boolean *ret);

    virtual CARAPI SetReadOnly();
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_PERMISSIONCOLLECTION_H__
