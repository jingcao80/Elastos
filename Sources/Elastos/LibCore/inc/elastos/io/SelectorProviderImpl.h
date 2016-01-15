
#ifndef __ELASTOS_IO_SELECTORPROVIDERIMPL_H__
#define __ELASTOS_IO_SELECTORPROVIDERIMPL_H__

#include "SelectorProvider.h"

using Elastos::IO::Channels::IDatagramChannel;
using Elastos::IO::Channels::IPipe;
using Elastos::IO::Channels::IServerSocketChannel;
using Elastos::IO::Channels::ISocketChannel;
using Elastos::IO::Channels::IChannel;
using Elastos::IO::Channels::Spi::IAbstractSelector;
using Elastos::IO::Channels::Spi::SelectorProvider;

namespace Elastos {
namespace IO {

class SelectorProviderImpl : public SelectorProvider
{
public:
    CARAPI OpenDatagramChannel(
        /* [out] */ IDatagramChannel ** ppChannel);

    CARAPI OpenPipe(
        /* [out] */ IPipe ** ppPipe);

    CARAPI OpenSelector(
        /* [out] */ IAbstractSelector ** ppSelector);

    CARAPI OpenServerSocketChannel(
        /* [out] */ IServerSocketChannel ** ppChannel);

    CARAPI OpenSocketChannel(
        /* [out] */ ISocketChannel ** ppSocketChannel);

    CARAPI InheritedChannel(
        /* [out] */ IChannel ** ppChanel);

    CARAPI constructor();
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_SELECTORPROVIDERIMPL_H__
