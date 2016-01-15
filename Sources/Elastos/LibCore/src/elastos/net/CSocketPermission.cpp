
#include "CSocketPermission.h"
#include "InetAddress.h"
#include "StringBuffer.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL_2(CSocketPermission, Object, ISocketPermission, ISerializable)

CAR_OBJECT_IMPL(CSocketPermission)

CSocketPermission::CSocketPermission()
    // TODO : Permission(String(""))
{}

ECode CSocketPermission::constructor(
    /* [in] */ const String& host,
    /* [in] */ const String& action)
{
    //Permission::Permission(String(""));
    return NOERROR;
}

ECode CSocketPermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);
    *actions = String(NULL);

    return NOERROR;
}

ECode CSocketPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* isImplied)
{
    VALIDATE_NOT_NULL(isImplied);

    *isImplied = TRUE;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
