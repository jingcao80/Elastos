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

#ifndef __ELASTOS_IO_SEQUENCEINPUTSTREAM_H__
#define __ELASTOS_IO_SEQUENCEINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "InputStream.h"

using Elastos::Utility::IEnumeration;

namespace Elastos {
namespace IO {

class SequenceInputStream
    : public InputStream
    , public ISequenceInputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code SequenceInputStream} using the two streams
     * {@code s1} and {@code s2} as the sequence of streams to read from.
     *
     * @param s1
     *            the first stream to get bytes from.
     * @param s2
     *            the second stream to get bytes from.
     * @throws NullPointerException
     *             if {@code s1} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* s1,
        /* [in] */ IInputStream* s2);

    /**
     * Constructs a new SequenceInputStream using the elements returned from
     * Enumeration {@code e} as the stream sequence. The instances returned by
     * {@code e.nextElement()} must be of type {@link InputStream}.
     *
     * @param e
     *            the enumeration of {@code InputStreams} to get bytes from.
     * @throws NullPointerException
     *             if any of the elements in {@code e} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ IEnumeration* e);

    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Closes all streams in this sequence of input stream.
     *
     * @throws IOException
     *             if an error occurs while closing any of the input streams.
     */
    CARAPI Close();

    /**
     * Reads a single byte from this sequence of input streams and returns it as
     * an integer in the range from 0 to 255. It tries to read from the current
     * stream first; if the end of this stream has been reached, it reads from
     * the next one. Blocks until one byte has been read, the end of the last
     * input stream in the sequence has been reached, or an exception is thrown.
     *
     * @return the byte read or -1 if either the end of the last stream in the
     *         sequence has been reached or this input stream sequence is
     *         closed.
     * @throws IOException
     *             if an error occurs while reading the current source input
     *             stream.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code byteCount} bytes from this sequence of input streams and
     * stores them in the byte array {@code buffer} starting at {@code byteOffset}.
     * Blocks only until at least 1 byte has been read, the end of the stream
     * has been reached, or an exception is thrown.
     * <p>
     * This SequenceInputStream shows the same behavior as other InputStreams.
     * To do this it will read only as many bytes as a call to read on the
     * current substream returns. If that call does not return as many bytes as
     * requested by {@code byteCount}, it will not retry to read more on its own
     * because subsequent reads might block. This would violate the rule that
     * it will only block until at least one byte has been read.
     * <p>
     * If a substream has already reached the end when this call is made, it
     * will close that substream and start with the next one. If there are no
     * more substreams it will return -1.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code byteOffset < 0 || byteCount < 0 || byteOffset + byteCount > buffer.length}.
     * @throws IOException
     *             if an I/O error occurs.
     * @throws NullPointerException
     *             if {@code buffer == null}.
     */
    // @Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

private:
    /**
     * Sets up the next InputStream or leaves it alone if there are none left.
     *
     * @throws IOException
     */
    CARAPI NextStream();

private:
    /**
     * An enumeration which will return types of InputStream.
     */
    AutoPtr<IEnumeration> mEnum;

    /**
     * The current input stream.
     */
    AutoPtr<IInputStream> mIn;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_SEQUENCEINPUTSTREAM_H__
