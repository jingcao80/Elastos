
#ifndef __ELASTOS_NET_CNETPERMISSION_H__
#define __ELASTOS_NET_CNETPERMISSION_H__

#include "_Elastos_Net_CNetPermission.h"
//#include "security/BasicPermission.h"
#include "Object.h"

using Elastos::Core::Object;
//using Elastos::Security::BasicPermission;

namespace Elastos {
namespace Net {

CarClass(CNetPermission)
    //, public BasicPermission  // BasicPermission has not be compiled ,use Object temp
    , public Object
    , public INetPermission
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CNETPERMISSION_H__
