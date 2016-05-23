
#include "SocketFactory.h"
#include "CDefaultSocketFactory.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastosx::Net::CDefaultSocketFactory;

namespace Elastosx {
namespace Net {

AutoPtr<ISocketFactory> SocketFactory::sDefaultFactory;
Object SocketFactory::sLock;

CAR_INTERFACE_IMPL(SocketFactory, Object, ISocketFactory)

SocketFactory::SocketFactory()
{}

SocketFactory::~SocketFactory()
{}

ECode SocketFactory::GetDefault(
    /* [out] */ ISocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    {    AutoLock syncLock(sLock);
        if (sDefaultFactory == NULL) {
            CDefaultSocketFactory::New((ISocketFactory**)&sDefaultFactory);
        }
        *factory = sDefaultFactory;
        REFCOUNT_ADD(*factory)
    }
    return NOERROR;
}

ECode SocketFactory::CreateSocket(
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    // follow RI's behavior
    return E_SOCKET_EXCEPTION;
}

} // namespace Net
} // namespace Elastosx
