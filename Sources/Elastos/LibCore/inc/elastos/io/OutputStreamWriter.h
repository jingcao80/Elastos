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

#ifndef __ELASTOS_IO_OUTPUTSTREAMWRITER_H__
#define __ELASTOS_IO_OUTPUTSTREAMWRITER_H__

#include "Writer.h"

using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetEncoder;

namespace Elastos {
namespace IO {

class OutputStreamWriter
    : public Writer
    , public IOutputStreamWriter
{
public:
    CAR_INTERFACE_DECL()

public:
    /**
     * Constructs a new OutputStreamWriter using {@code out} as the target
     * stream to write converted characters to. The default character encoding
     * is used.
     *
     * @param out
     *            the non-null target stream to write converted bytes to.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream *os);

    /**
     * Constructs a new OutputStreamWriter using {@code out} as the target
     * stream to write converted characters to and {@code enc} as the character
     * encoding. If the encoding cannot be found, an
     * UnsupportedEncodingException error is thrown.
     *
     * @param out
     *            the target stream to write converted bytes to.
     * @param enc
     *            the string describing the desired character encoding.
     * @throws NullPointerException
     *             if {@code enc} is {@code null}.
     * @throws UnsupportedEncodingException
     *             if the encoding specified by {@code enc} cannot be found.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream *os,
        /* [in] */ const String &enc);

    /**
     * Constructs a new OutputStreamWriter using {@code out} as the target
     * stream to write converted characters to and {@code cs} as the character
     * encoding.
     *
     * @param out
     *            the target stream to write converted bytes to.
     * @param charset
     *            the {@code Charset} that specifies the character encoding.
     */
     CARAPI constructor(
        /* [in] */ IOutputStream *os,
        /* [in] */ ICharset* charset);

    /**
     * Constructs a new OutputStreamWriter using {@code out} as the target
     * stream to write converted characters to and {@code charsetEncoder} as the character
     * encoder.
     *
     * @param out
     *            the target stream to write converted bytes to.
     * @param charsetEncoder
     *            the character encoder used for character conversion.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream *os,
        /* [in] */ ICharsetEncoder* charsetEncoder);

    /**
     * Closes this writer. This implementation flushes the buffer but <strong>does not</strong>
     * flush the target stream. The target stream is then closed and the resources for the
     * buffer and converter are released.
     *
     * <p>Only the first invocation of this method has any effect. Subsequent calls
     * do nothing.
     *
     * @throws IOException
     *             if an error occurs while closing this writer.
     */
    // @Override
    CARAPI Close();

    /**
     * Flushes this writer. This implementation ensures that all buffered bytes
     * are written to the target stream. After writing the bytes, the target
     * stream is flushed as well.
     *
     * @throws IOException
     *             if an error occurs while flushing this writer.
     */
    CARAPI Flush();

    /**
     * Returns the canonical name of the encoding used by this writer to convert characters to
     * bytes, or null if this writer has been closed. Most callers should probably keep
     * track of the String or Charset they passed in; this method may not return the same
     * name.
     */
    CARAPI GetEncoding(
        /* [out] */ String *encoding);

    /**
     * Writes {@code count} characters starting at {@code offset} in {@code buf}
     * to this writer. The characters are immediately converted to bytes by the
     * character converter and stored in a local buffer. If the buffer gets full
     * as a result of the conversion, this writer is flushed.
     *
     * @param buffer
     *            the array containing characters to write.
     * @param offset
     *            the index of the first character in {@code buf} to write.
     * @param count
     *            the maximum number of characters to write.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is greater than the size of
     *             {@code buf}.
     * @throws IOException
     *             if this writer has already been closed or another I/O error
     *             occurs.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /**
     * Writes the character {@code oneChar} to this writer. The lowest two bytes
     * of the integer {@code oneChar} are immediately converted to bytes by the
     * character converter and stored in a local buffer. If the buffer gets full
     * by converting this character, this writer is flushed.
     *
     * @param oneChar
     *            the character to write.
     * @throws IOException
     *             if this writer is closed or another I/O error occurs.
     */
    CARAPI Write(
        /* [in] */ Int32 oneChar32);

    /**
     * Writes {@code count} characters starting at {@code offset} in {@code str}
     * to this writer. The characters are immediately converted to bytes by the
     * character converter and stored in a local buffer. If the buffer gets full
     * as a result of the conversion, this writer is flushed.
     *
     * @param str
     *            the string containing characters to write.
     * @param offset
     *            the start position in {@code str} for retrieving characters.
     * @param count
     *            the maximum number of characters to write.
     * @throws IOException
     *             if this writer has already been closed or another I/O error
     *             occurs.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is bigger than the length of
     *             {@code str}.
     */
    CARAPI Write(
        /* [in] */ const String& str,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

protected:
    OutputStreamWriter();

    virtual ~OutputStreamWriter();

private:
    CARAPI FlushBytes(
        /* [in] */ Boolean flushUnderlyingStream);

    CARAPI Convert(
       /* [in] */ ICharBuffer* chars);

    CARAPI DrainEncoder();

    CARAPI CheckStatus();

private:
    AutoPtr<IByteBuffer> mBytes;

    AutoPtr<IOutputStream> mOut;

    AutoPtr<ICharsetEncoder> mEncoder;
};

} // namespace IO
} // namespace Elastos

#endif  //__ELASTOS_IO_OUTPUTSTREAMWRITER_H__
