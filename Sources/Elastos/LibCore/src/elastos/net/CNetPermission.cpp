
#include "CNetPermission.h"

//using Elastos::Security::IBasicPermission;
//using Elastos::Security::IPermission;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CNetPermission, Object, INetPermission)

CAR_OBJECT_IMPL(CNetPermission)

ECode CNetPermission::constructor(
    /* [in] */ const String& name)
{
    //return BasicPermission::Init(name);
    return NOERROR;
}

ECode CNetPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    //return BasicPermission::Init(name, action);
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
