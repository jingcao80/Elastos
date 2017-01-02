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
