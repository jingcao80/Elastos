
#include "SelectorProviderImpl.h"
#include "DatagramChannelImpl.h"
#include "PipeImpl.h"
#include "CSelectorImpl.h"
#include "ServerSocketChannelImpl.h"
#include "SocketChannelImpl.h"

using Elastos::IO::Channels::Spi::EIID_ISelectorProvider;

namespace Elastos {
namespace IO {

ECode SelectorProviderImpl::OpenDatagramChannel(
    /* [out] */ IDatagramChannel** ppChannel)
{
    VALIDATE_NOT_NULL(ppChannel)

    AutoPtr<DatagramChannelImpl> dci = new DatagramChannelImpl();
    dci->constructor(THIS_PROBE(ISelectorProvider));

    *ppChannel = IDatagramChannel::Probe(dci);
    REFCOUNT_ADD(*ppChannel)
    return NOERROR;
}

ECode SelectorProviderImpl::OpenPipe(
    /* [out] */ IPipe** ppPipe)
{
    VALIDATE_NOT_NULL(ppPipe)

    *ppPipe = (IPipe*) new PipeImpl(this);
    REFCOUNT_ADD(*ppPipe)
    return NOERROR;
}

ECode SelectorProviderImpl::OpenSelector(
    /* [out] */ IAbstractSelector** ppSelector)
{
    VALIDATE_NOT_NULL(ppSelector)
    return CSelectorImpl::New(THIS_PROBE(ISelectorProvider), ppSelector);
}

ECode SelectorProviderImpl::OpenServerSocketChannel(
    /* [out] */ IServerSocketChannel** ppChannel)
{
    VALIDATE_NOT_NULL(ppChannel)

    *ppChannel = (IServerSocketChannel*) new ServerSocketChannelImpl(this);
    REFCOUNT_ADD(*ppChannel)
    return NOERROR;
}

ECode SelectorProviderImpl::OpenSocketChannel(
    /* [out] */ ISocketChannel** ppSocketChannel)
{
    VALIDATE_NOT_NULL(ppSocketChannel)

    *ppSocketChannel = (ISocketChannel*) new SocketChannelImpl(this);
    REFCOUNT_ADD(*ppSocketChannel)
    return NOERROR;
}

ECode SelectorProviderImpl::InheritedChannel(
    /* [out] */ IChannel** ppChanel)
{
    VALIDATE_NOT_NULL(ppChanel)

    *ppChanel = NULL;
    return NOERROR;
}

ECode SelectorProviderImpl::constructor()
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
