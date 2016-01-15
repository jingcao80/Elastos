
#include "CBootReceiver.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Server {

ECode CBootReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // TODO:
    return E_NOT_IMPLEMENTED;
}

ECode CBootReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("CBootReceiver: ");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos