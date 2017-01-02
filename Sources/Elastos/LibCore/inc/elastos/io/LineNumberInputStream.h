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

#ifndef __ELASTOS_IO_ELASTOS_LINENUMBER_INPUTSTREAM_H__
#define __ELASTOS_IO_ELASTOS_LINENUMBER_INPUTSTREAM_H__

#include "FilterInputStream.h"

namespace Elastos {
namespace IO {

class LineNumberInputStream
    : public FilterInputStream
    , public ILineNumberInputStream
{
public:
    CAR_INTERFACE_DECL()

protected:
    LineNumberInputStream();

    virtual ~LineNumberInputStream();

public:
    /**
     * Constructs a new {@code LineNumberInputStream} on the {@link InputStream}
     * {@code in}. Line numbers are counted for all data read from this stream.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code LineNumberInputStream}. All operations on such a stream will fail.
     *
     * @param in
     *            The non-null input stream to count line numbers.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* in);

    /**
     * {@inheritDoc}
     *
     * <p>Note that the source stream may just be a sequence of {@code "\r\n"} bytes
     * which are converted into {@code '\n'} by this stream. Therefore,
     * {@code available} returns only {@code in.available() / 2} bytes as
     * result.
     */
    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Returns the current line number for this stream. Numbering starts at 0.
     *
     * @return the current line number.
     */
    virtual CARAPI GetLineNumber(
        /* [out] */ Int32* lineNumber);

    /**
     * Sets a mark position in this stream. The parameter {@code readlimit}
     * indicates how many bytes can be read before the mark is invalidated.
     * Sending {@code reset()} will reposition this stream back to the marked
     * position, provided that {@code readlimit} has not been surpassed.
     * The line number count will also be reset to the last marked
     * line number count.
     * <p>
     * This implementation sets a mark in the filtered stream.
     *
     * @param readlimit
     *            the number of bytes that can be read from this stream before
     *            the mark is invalidated.
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Reads a single byte from the filtered stream and returns it as an integer
     * in the range from 0 to 255. Returns -1 if the end of this stream has been
     * reached.
     * <p>
     * The line number count is incremented if a line terminator is encountered.
     * Recognized line terminator sequences are {@code '\r'}, {@code '\n'} and
     * {@code "\r\n"}. Line terminator sequences are always translated into
     * {@code '\n'}.
     *
     * @return the byte read or -1 if the end of the filtered stream has been
     *         reached.
     * @throws IOException
     *             if the stream is closed or another IOException occurs.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code byteCount} bytes from the filtered stream and stores
     * them in the byte array {@code buffer} starting at {@code byteOffset}.
     * Returns the number of bytes actually read or -1 if no bytes have been
     * read and the end of this stream has been reached.
     *
     * <p>The line number count is incremented if a line terminator is encountered.
     * Recognized line terminator sequences are {@code '\r'}, {@code '\n'} and
     * {@code "\r\n"}. Line terminator sequences are always translated into
     * {@code '\n'}.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code byteOffset < 0 || byteCount < 0 || byteOffset + byteCount > buffer.length}.
     * @throws IOException
     *             if this stream is closed or another IOException occurs.
     * @throws NullPointerException
     *             if {@code buffer == null}.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Resets this stream to the last marked location. It also resets the line
     * count to what is was when this stream was marked.
     *
     * @throws IOException
     *             if this stream is already closed, no mark has been set or the
     *             mark is no longer valid because more than {@code readlimit}
     *             bytes have been read since setting the mark.
     * @see #mark(int)
     * @see #markSupported()
     */
    CARAPI Reset();

    /**
     * Sets the line number of this stream to the specified
     * {@code lineNumber}. Note that this may have side effects on the
     * line number associated with the last marked position.
     *
     * @param lineNumber
     *            the new lineNumber value.
     * @see #mark(int)
     * @see #reset()
     */
    virtual CARAPI SetLineNumber(
        /* [in] */ Int32 lineNumber);

    /**
     * Skips {@code count} number of bytes in this stream. Subsequent
     * {@code read()}'s will not return these bytes unless {@code reset()} is
     * used. This implementation skips {@code count} number of bytes in the
     * filtered stream and increments the line number count whenever line
     * terminator sequences are skipped.
     *
     * @param count
     *            the number of bytes to skip.
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if this stream is closed or another IOException occurs.
     * @see #mark(int)
     * @see #read()
     * @see #reset()
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

private:
    Int32 mLineNumber;

    Int32 mMarkedLineNumber;

    Int32 mLastChar;

    Int32 mMarkedLastChar;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_ELASTOS_LINENUMBER_INPUTSTREAM_H__
