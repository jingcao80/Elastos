#include "elastos/droid/app/CInnerConnection.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CInnerConnection, Object, IIServiceConnection, IBinder)

CAR_OBJECT_IMPL(CInnerConnection)

ECode CInnerConnection::constructor(
    /* [in] */ IServiceDispatcher* sd)
{
    mDispatcher = sd;
    return NOERROR;
}

ECode CInnerConnection::Connected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (mDispatcher != NULL) {
        mDispatcher->Connected(name, service);
    }
    return NOERROR;
}

ECode CInnerConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ServiceDispatcher::InnerConnection");
    return NOERROR;
}

}
}
}