
#ifndef __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__
#define __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__

#include "_Elastos_Droid_Server_Am_CUriPermissionOwnerExternalToken.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class UriPermissionOwner;

CarClass(CUriPermissionOwnerExternalToken)
    , public Object
    , public IUriPermissionOwnerExternalToken
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IObject* owner);

    CARAPI GetOwner(
        /* [out] */ IObject** owner);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IObject> mUriOwner; // UriPermissionOwner
};

} // namespace Am
} // namespace Droid
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__
