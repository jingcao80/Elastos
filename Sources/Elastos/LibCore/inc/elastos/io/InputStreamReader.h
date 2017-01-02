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

#ifndef __ELASTOS_IO_INPUTSTREAMREADER_H__
#define __ELASTOS_IO_INPUTSTREAMREADER_H__

#include "Reader.h"

using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetDecoder;

namespace Elastos {
namespace IO {

class InputStreamReader
    : public Reader
    , public IInputStreamReader
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code InputStreamReader} on the {@link InputStream}
     * {@code in}. This constructor sets the character converter to the encoding
     * specified in the "file.encoding" property and falls back to ISO 8859_1
     * (ISO-Latin-1) if the property doesn't exist.
     *
     * @param in
     *            the input stream from which to read characters.
     */
    CARAPI constructor(
        /* [in] */ IInputStream *in);

    /**
     * Constructs a new InputStreamReader on the InputStream {@code in}. The
     * character converter that is used to decode bytes into characters is
     * identified by name by {@code enc}. If the encoding cannot be found, an
     * UnsupportedEncodingException error is thrown.
     *
     * @param in
     *            the InputStream from which to read characters.
     * @param enc
     *            identifies the character converter to use.
     * @throws NullPointerException
     *             if {@code enc} is {@code null}.
     * @throws UnsupportedEncodingException
     *             if the encoding specified by {@code enc} cannot be found.
     */
    CARAPI constructor(
        /* [in] */ IInputStream *in,
        /* [in] */ const String &enc);

    /**
     * Constructs a new InputStreamReader on the InputStream {@code in} and
     * Charset {@code charset}.
     *
     * @param in
     *            the source InputStream from which to read characters.
     * @param charset
     *            the Charset that defines the character converter
     */
    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ ICharset* charset);

    /**
     * Constructs a new InputStreamReader on the InputStream {@code in} and
     * CharsetDecoder {@code dec}.
     *
     * @param in
     *            the source InputStream from which to read characters.
     * @param dec
     *            the CharsetDecoder used by the character conversion.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ ICharsetDecoder* dec);

    /**
     * Closes this reader. This implementation closes the source InputStream and
     * releases all local storage.
     *
     * @throws IOException
     *             if an error occurs attempting to close this reader.
     */
    CARAPI Close();

    /**
     * Returns the canonical name of the encoding used by this writer to convert characters to
     * bytes, or null if this writer has been closed. Most callers should probably keep
     * track of the String or Charset they passed in; this method may not return the same
     * name.
     */
    CARAPI GetEncoding(
        /* [out] */ String* encoding);

    /**
     * Reads a single character from this reader and returns it as an integer
     * with the two higher-order bytes set to 0. Returns -1 if the end of the
     * reader has been reached. The byte value is either obtained from
     * converting bytes in this reader's buffer or by first filling the buffer
     * from the source InputStream and then reading from the buffer.
     *
     * @return the character read or -1 if the end of the reader has been
     *         reached.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code count} characters from this reader and stores them
     * at position {@code offset} in the character array {@code buffer}. Returns
     * the number of characters actually read or -1 if the end of the reader has
     * been reached. The bytes are either obtained from converting bytes in this
     * reader's buffer or by first filling the buffer from the source
     * InputStream and then reading from the buffer.
     *
     * @throws IndexOutOfBoundsException
     *     if {@code offset < 0 || count < 0 || offset + count > buffer.length}.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Indicates whether this reader is ready to be read without blocking. If
     * the result is {@code true}, the next {@code read()} will not block. If
     * the result is {@code false} then this reader may or may not block when
     * {@code read()} is called. This implementation returns {@code true} if
     * there are bytes available in the buffer or the source stream has bytes
     * available.
     *
     * @return {@code true} if the receiver will not block when {@code read()}
     *         is called, {@code false} if unknown or blocking will occur.
     * @throws IOException
     *             if this reader is closed or some other I/O error occurs.
     */
    CARAPI IsReady(
        /* [out] */ Boolean* ready);

protected:
    InputStreamReader();

    virtual ~InputStreamReader();

private:
    CARAPI_(Boolean) IsOpen();

private:
    AutoPtr<IInputStream> mIn;

    Boolean mEndOfInput;

    AutoPtr<ICharsetDecoder> mDecoder;

    AutoPtr<IByteBuffer> mBytes;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_INPUTSTREAMREADER_H__
