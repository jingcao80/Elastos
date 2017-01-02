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

#ifndef __ELASTOS_IO_CHANNELS_PIPE_H__
#define __ELASTOS_IO_CHANNELS_PIPE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::Object;

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

