#ifndef __ELASTOS_IO_CHANNELS_PIPE_H__
#define __ELASTOS_IO_CHANNELS_PIPE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

namespace Elastos {
namespace IO {
namespace Channels {

class Pipe
    : public Object
    , public IPipe
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a new pipe from the default {@see java.nio.channels.spi.SelectorProvider}.
     *
     * @throws IOException
     *             if an I/O error occurs.
     */
    static CARAPI Open(
        /* [out] */ IPipe** pipe);

    /**
     * Returns the sink channel of the pipe.
     *
     * @return a writable sink channel of the pipe.
     */
    virtual CARAPI Sink(
        /* [out] */ ISinkChannel** channel) = 0;

    /**
     * Returns the source channel of the pipe.
     *
     * @return a readable source channel of the pipe.
     */
    virtual CARAPI Source(
        /* [out] */ ISourceChannel** channel) = 0;

protected:
    Pipe();
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_PIPE_H__

