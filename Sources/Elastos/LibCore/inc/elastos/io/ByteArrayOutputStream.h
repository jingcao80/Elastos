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

#ifndef __ELASTOS_IO_BYTEARRAYOUTPUTSTREAM_H__
#define __ELASTOS_IO_BYTEARRAYOUTPUTSTREAM_H__

#include "OutputStream.h"

namespace Elastos {
namespace IO {

class ByteArrayOutputStream
    : public OutputStream
    , public IByteArrayOutputStream
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 size);

    /**
     * Closes this stream. This releases system resources used for this stream.
     *
     * @throws IOException
     *             if an error occurs while attempting to close this stream.
     */
    CARAPI Close();

    /**
     * Resets this stream to the beginning of the underlying byte array. All
     * subsequent writes will overwrite any bytes previously stored in this
     * stream.
     */
    // synchronized
    CARAPI Reset();

    /**
     * Returns the total number of bytes written to this stream so far.
     *
     * @return the number of bytes written to this stream.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Returns the contents of this ByteArrayOutputStream as a byte array. Any
     * changes made to the receiver after returning will not be reflected in the
     * byte array returned to the caller.
     *
     * @return this stream's current contents as a byte array.
     */
    // synchronized
    CARAPI ToByteArray(
        /* [out] */ ArrayOf<Byte>** bytes);

    /**
     * Returns the contents of this ByteArrayOutputStream as a string. Any
     * changes made to the receiver after returning will not be reflected in the
     * string returned to the caller.
     *
     * @return this stream's current contents as a string.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the contents of this ByteArrayOutputStream as a string. Each byte
     * {@code b} in this stream is converted to a character {@code c} using the
     * following function:
     * {@code c == (char)(((hibyte & 0xff) << 8) | (b & 0xff))}. This method is
     * deprecated and either {@link #toString()} or {@link #toString(String)}
     * should be used.
     *
     * @param hibyte
     *            the high byte of each resulting Unicode character.
     * @return this stream's current contents as a string with the high byte set
     *         to {@code hibyte}.
     * @deprecated Use {@link #toString()} instead.
     */
    // @Deprecated
    CARAPI ToString(
        /* [in] */ Int32 hibyte,
        /* [out] */ String* rev);

    /**
     * Returns the contents of this ByteArrayOutputStream as a string converted
     * according to the encoding declared in {@code enc}.
     *
     * @param enc
     *            a string representing the encoding to use when translating
     *            this stream to a string.
     * @return this stream's current contents as an encoded string.
     * @throws UnsupportedEncodingException
     *             if the provided encoding is not supported.
     */
    CARAPI ToString(
        /* [in] */ const String& enc,
        /* [out] */ String* str);

    /**
     * Writes the specified byte {@code oneByte} to the OutputStream. Only the
     * low order byte of {@code oneByte} is written.
     *
     * @param oneByte
     *            the byte to be written.
     */
    // synchronized
    CARAPI Write(
        /* [in] */ Int32 oneByte);

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * offset {@code index} to this stream.
     *
     * @param buffer
     *            the buffer to be written.
     * @param offset
     *            the initial position in {@code buffer} to retrieve bytes.
     * @param len
     *            the number of bytes of {@code buffer} to write.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code len < 0}, or if
     *             {@code offset + len} is greater than the length of
     *             {@code buffer}.
     */
    // synchronized
    CARAPI Write(
        /* [in] */ ArrayOf<Byte> * buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /**
     * Takes the contents of this stream and writes it to the output stream
     * {@code out}.
     *
     * @param out
     *            an OutputStream on which to write the contents of this stream.
     * @throws IOException
     *             if an error occurs while writing to {@code out}.
     */
    // synchronized
    CARAPI WriteTo(
        /* [in] */ IOutputStream* out);

protected:
    ByteArrayOutputStream();

    virtual ~ByteArrayOutputStream();

private:
    CARAPI Expand(
        /* [in] */ Int32 i);

protected:
    /**
     * The byte array containing the bytes written.
     */
    AutoPtr<ArrayOf<Byte> > mBuf;

    /**
     * The number of bytes written.
     */
    Int32 mCount;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_BYTEARRAYOUTPUTSTREAM_H__
