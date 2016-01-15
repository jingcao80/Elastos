
#ifndef __ELASTOS_NET_CSOCKETPERMISSION_H__
#define __ELASTOS_NET_CSOCKETPERMISSION_H__

#include "_Elastos_Net_CSocketPermission.h"
//#include "security/Permission.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Security::IPermission;
using Elastos::Security::IPermissionCollection;

namespace Elastos {
namespace Net {

CarClass(CSocketPermission)
    //, public Permission  // BasicPermission has not compile ,use Object temp
    , public Object
    , public ISocketPermission
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSocketPermission();

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ const String& action);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* isImplied);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSOCKETPERMISSION_H__
