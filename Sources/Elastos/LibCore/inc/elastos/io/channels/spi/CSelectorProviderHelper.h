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

#ifndef __ELASTOS_IO_CHANNELS_SPI_ELASTOS_IO_CSELECTORPROVIDERHELPER_H__
#define __ELASTOS_IO_CHANNELS_SPI_ELASTOS_IO_CSELECTORPROVIDERHELPER_H__

#include "_Elastos_IO_Channels_Spi_CSelectorProviderHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

CarClass(CSelectorProviderHelper)
    , public Singleton
    , public ISelectorProviderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Gets a provider instance by executing the following steps when called for
     * the first time:
     * <ul>
     * <li> if the system property "java.nio.channels.spi.SelectorProvider" is
     * set, the value of this property is the class name of the provider
     * returned; </li>
     * <li>if there is a provider-configuration file named
     * "java.nio.channels.spi.SelectorProvider" in META-INF/services of a jar
     * file valid in the system class loader, the first class name is the
     * provider's class name; </li>
     * <li> otherwise, a system default provider will be returned.</li>
     * </ul>
     *
     * @return the provider.
     */
    CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider);
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CDATAGRAMCHANNELHELPER_H__
