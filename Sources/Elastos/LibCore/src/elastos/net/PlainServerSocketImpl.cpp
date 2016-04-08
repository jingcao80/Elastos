#include "PlainServerSocketImpl.h"
#include "CBoolean.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(PlainServerSocketImpl, PlainSocketImpl, IPlainServerSocketImpl)

ECode PlainServerSocketImpl::constructor()
{
    return PlainSocketImpl::constructor();
}

ECode PlainServerSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    return PlainSocketImpl::constructor(fd);
}

ECode PlainServerSocketImpl::Create(
    /* [in] */ Boolean isStreaming)
{
    ECode ec = NOERROR;
    ec = PlainSocketImpl::Create(isStreaming);
    FAIL_RETURN(ec)

    if (isStreaming) {
        AutoPtr<IBoolean> obj;
        CBoolean::New(TRUE, (IBoolean**)&obj);
        SetOption(ISocketOptions::_SO_REUSEADDR, obj.Get());
    }
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
