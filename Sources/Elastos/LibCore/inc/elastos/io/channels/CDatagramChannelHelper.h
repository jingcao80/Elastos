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

#ifndef __ELASTOS_IO_CHANNELS_ELASTOS_IO_CDATAGRAMCHANNELHELPER_H__
#define __ELASTOS_IO_CHANNELS_ELASTOS_IO_CDATAGRAMCHANNELHELPER_H__

#include "_Elastos_IO_Channels_Elastos_IO_CDatagramChannelHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {
namespace Channels {

CarClass(CDatagramChannelHelper)
    , public Singleton
    , public IDatagramChannelHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates an opened and not-connected datagram channel.
     * <p>
     * This channel is created by calling the <code>openDatagramChannel</code>
     * method of the default {@link SelectorProvider} instance.
     *
     * @return the new channel which is open but not connected.
     * @throws IOException
     *             if some I/O error occurs.
     */
    CARAPI Open(
        /* [out] */ IDatagramChannel** channel);

};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_ELASTOS_IO_CDATAGRAMCHANNELHELPER_H__
