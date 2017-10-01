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

#ifndef __ELASTOS_IO_DATAINPUTSTREAM_H__
#define __ELASTOS_IO_DATAINPUTSTREAM_H__

#include "FilterInputStream.h"

namespace Elastos {
namespace IO {

class DataInputStream
    : public FilterInputStream
    , public IDataInputStream
    , public IDataInput
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IInputStream* is);

protected:
    DataInputStream();

    virtual ~DataInputStream();

    CARAPI DecodeUTF(
        /* [in] */ Int32 utfSize,
        /* [out] */ String* str);

public:
    using FilterInputStream::Read;

    /**
     * Reads bytes from this stream into the byte array {@code buffer}. Returns
     * the number of bytes that have been read.
     *
     * @param buffer
     *            the buffer to read bytes into.
     * @return the number of bytes that have been read or -1 if the end of the
     *         stream has been reached.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#write(byte[])
     * @see DataOutput#write(byte[], int, int)
     */
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    // @Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Reads a boolean from this stream.
     *
     * @return the next boolean value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before one byte
     *             has been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeBoolean(boolean)
     */
    CARAPI ReadBoolean(
        /* [out] */ Boolean* value);

    /**
     * Reads an 8-bit byte value from this stream.
     *
     * @return the next byte value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before one byte
     *             has been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeByte(int)
     */
    CARAPI ReadByte(
        /* [out] */ Byte* value);

    /**
     * Reads a 32-bit character value from this stream.
     *
     * @return the next char value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before two bytes
     *             have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeChar(int)
     */
    CARAPI ReadChar(
        /* [out] */ Char32* value);

    /**
     * Reads a 64-bit double value from this stream.
     *
     * @return the next double value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before eight
     *             bytes have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeDouble(double)
     */
    CARAPI ReadDouble(
        /* [out] */ Double* value);

    /**
     * Reads a 32-bit float value from this stream.
     *
     * @return the next float value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before four
     *             bytes have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeFloat(float)
     */
    CARAPI ReadFloat(
        /* [out] */ Float* value);

    /**
     * Reads bytes from this stream into the byte array {@code buffer}. This
     * method will block until {@code buffer.length} number of bytes have been
     * read.
     *
     * @param buffer
     *            to read bytes into.
     * @throws EOFException
     *             if the end of the source stream is reached before enough
     *             bytes have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#write(byte[])
     * @see DataOutput#write(byte[], int, int)
     */
    CARAPI ReadFully(
        /* [out] */ ArrayOf<byte> * buffer);

    /**
     * Reads bytes from this stream and stores them in the byte array {@code
     * buffer} starting at the position {@code offset}. This method blocks until
     * {@code length} bytes have been read. If {@code length} is zero, then this
     * method returns without reading any bytes.
     *
     * @param buffer
     *            the byte array into which the data is read.
     * @param offset
     *            the offset in {@code buffer} from where to store the bytes
     *            read.
     * @param length
     *            the maximum number of bytes to read.
     * @throws EOFException
     *             if the end of the source stream is reached before enough
     *             bytes have been read.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code length < 0}, or if {@code
     *             offset + length} is greater than the size of {@code buffer}.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @throws NullPointerException
     *             if {@code buffer} or the source stream are null.
     * @see java.io.DataInput#readFully(byte[], int, int)
     */
    CARAPI ReadFully(
        /* [out] */ ArrayOf<byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Reads a 32-bit integer value from this stream.
     *
     * @return the next int value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before four
     *             bytes have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeInt(int)
     */
    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    /**
     * @deprecated This method cannot be trusted to convert bytes to characters correctly.
     * Wrap this stream with a {@link BufferedReader} instead.
     */
    // @Deprecated
    CARAPI ReadLine(
         /* [out] */ String* value);

    /**
     * Reads a 64-bit long value from this stream.
     *
     * @return the next long value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before eight
     *             bytes have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeLong(long)
     */
    CARAPI ReadInt64(
        /* [out] */ Int64* value);

    /**
     * Reads a 16-bit short value from this stream.
     *
     * @return the next short value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before two bytes
     *             have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeShort(int)
     */
    CARAPI ReadInt16(
        /* [out] */ Int16* value);

    /**
     * Reads an unsigned 8-bit byte value from this stream and returns it as an
     * int.
     *
     * @return the next unsigned byte value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream has been reached before one
     *             byte has been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeByte(int)
     */
    CARAPI ReadUnsignedByte(
        /* [out] */ Int32* value);

    /**
     * Reads a 16-bit unsigned short value from this stream and returns it as an
     * int.
     *
     * @return the next unsigned short value from the source stream.
     * @throws EOFException
     *             if the end of the filtered stream is reached before two bytes
     *             have been read.
     * @throws IOException
     *             if a problem occurs while reading from this stream.
     * @see DataOutput#writeShort(int)
     */
    CARAPI ReadUnsignedInt16(
        /* [out] */ Int32* value);

    /**
     * Reads a string encoded with {@link DataInput modified UTF-8}.
     *
     * @return the next string encoded with {@link DataInput modified UTF-8}.
     * @throws EOFException if the end of the input is reached before the read
     *         request can be satisfied.
     * @throws IOException
     *             if an I/O error occurs while reading.
     * @see DataOutput#writeUTF(java.lang.String)
     */
    CARAPI ReadUTF(
        /* [out] */ String* str);

    /**
     * Skips {@code count} number of bytes in this stream. Subsequent {@code
     * read()}s will not return these bytes unless {@code reset()} is used.
     *
     * This method will not throw an {@link EOFException} if the end of the
     * input is reached before {@code count} bytes where skipped.
     *
     * @param count
     *            the number of bytes to skip.
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if a problem occurs during skipping.
     * @see #mark(int)
     * @see #reset()
     */
    CARAPI SkipBytes(
        /* [in] */ Int32 count,
        /* [out] */ Int32* value);

    static CARAPI_(String) ReadUTF(
        /* [in] */ IDataInput* dataInput);

    static CARAPI_(String) DecodeUTF(
        /* [in] */ Int32 utfSize,
        /* [in] */ IDataInput* in);
private:
    CARAPI ReadToBuff(
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

private:
    AutoPtr<ArrayOf<Byte> > mBuff;
};

} // namespace IO
} // namespace Elastos

#endif
