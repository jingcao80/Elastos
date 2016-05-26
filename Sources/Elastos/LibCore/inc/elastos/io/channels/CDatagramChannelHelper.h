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
