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

#ifndef __ELASTOS_IO_OUTPUTSTREAM_H__
#define __ELASTOS_IO_OUTPUTSTREAM_H__

#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace IO {

class ECO_PUBLIC OutputStream
    : public Object
    , public IOutputStream
    , public ICloseable
    , public IFlushable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Closes this stream. Implementations of this method should free any
     * resources used by the stream. This implementation does nothing.
     *
     * @throws IOException
     *             if an error occurs while closing this stream.
     */
    CARAPI Close();

    /**
     * Flushes this stream. Implementations of this method should ensure that
     * any buffered data is written out. This implementation does nothing.
     *
     * @throws IOException
     *             if an error occurs while flushing this stream.
     */
    CARAPI Flush();

    /**
     * Writes the entire contents of the byte array {@code buffer} to this
     * stream.
     *
     * @param buffer
     *            the buffer to be written.
     * @throws IOException
     *             if an error occurs while writing to this stream.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * position {@code offset} to this stream.
     *
     * @param buffer
     *            the buffer to be written.
     * @param offset
     *            the start position in {@code buffer} from where to get bytes.
     * @param count
     *            the number of bytes from {@code buffer} to write to this
     *            stream.
     * @throws IOException
     *             if an error occurs while writing to this stream.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is bigger than the length of
     *             {@code buffer}.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    virtual CARAPI Write(
        /* [in] */ Int32 oneByte) = 0;

    /**
     * Returns true if this writer has encountered and suppressed an error. Used
     * by PrintStreams as an alternative to checked exceptions.
     */
    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

protected:
    OutputStream();

    virtual ~OutputStream();
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_OUTPUTSTREAM_H__
