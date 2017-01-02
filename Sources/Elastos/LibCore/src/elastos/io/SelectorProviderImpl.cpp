//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    dci->constructor(this);

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
    return CSelectorImpl::New(this, ppSelector);
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
