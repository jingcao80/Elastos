
#ifndef __ELASTOS_IO_DATAOUTPUTSTREAM_H__
#define __ELASTOS_IO_DATAOUTPUTSTREAM_H__

#include "FilterOutputStream.h"

namespace Elastos {
namespace IO {

class DataOutputStream
    : public FilterOutputStream
    , public IDataOutputStream
    , public IDataOutput
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code DataOutputStream} on the {@code OutputStream}
     * {@code out}. Note that data written by this stream is not in a human
     * readable form but can be reconstructed by using a {@link DataInputStream}
     * on the resulting output.
     *
     * @param out
     *            the target stream for writing.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out);

    /**
     * Flushes this stream to ensure all pending data is sent out to the target
     * stream. This implementation then also flushes the target stream.
     *
     * @throws IOException
     *             if an error occurs attempting to flush this stream.
     */
    CARAPI Flush();

    /**
     * Returns the total number of bytes written to the target stream so far.
     *
     * @return the number of bytes written to the target stream.
     */
    CARAPI GetSize(
    /* [out] */ Int32* value);

    /**
     * Writes a byte to the target stream. Only the least significant byte of
     * the integer {@code oneByte} is written.
     *
     * @param oneByte
     *            the byte to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readByte()
     */
    CARAPI Write(
        /* [in] */ Int32 oneByte);

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * {@code offset} to the target stream.
     *
     * @param buffer
     *            the buffer to write to the target stream.
     * @param offset
     *            the index of the first byte in {@code buffer} to write.
     * @param count
     *            the number of bytes from the {@code buffer} to write.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     * @see DataInputStream#readFully(byte[])
     * @see DataInputStream#readFully(byte[], int, int)
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    /**
     * Writes a boolean to the target stream.
     *
     * @param val
     *            the boolean value to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readBoolean()
     */
    CARAPI WriteBoolean(
        /* [in] */ Boolean val);

    /**
     * Writes an 8-bit byte to the target stream. Only the least significant
     * byte of the integer {@code val} is written.
     *
     * @param val
     *            the byte value to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readByte()
     * @see DataInputStream#readUnsignedByte()
     */
    CARAPI WriteByte(
        /* [in] */ Int32 val);

    /**
     * Writes the low order bytes from a string to the target stream.
     *
     * @param str
     *            the string containing the bytes to write to the target stream.
     * @throws IOException
     *             if an error occurs while writing to the target stream.
     * @see DataInputStream#readFully(byte[])
     * @see DataInputStream#readFully(byte[],int,int)
     */
    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI WriteBytes(
        /* [in] */ const String& str);

    CARAPI WriteChar(
        /* [in] */ Int32 val);

    CARAPI WriteChars(
        /* [in] */ const String& str);

    CARAPI WriteDouble(
        /* [in] */ Double val);

    CARAPI WriteFloat(
        /* [in] */ Float val);

    CARAPI WriteInt32(
        /* [in] */ Int32 val);

    CARAPI WriteInt64(
        /* [in] */ Int64 val);

    CARAPI WriteInt16(
        /* [in] */ Int32 val);

    CARAPI WriteUTF(
        /* [in] */ const String& str);

protected:
    DataOutputStream();

    virtual ~DataOutputStream();

protected:
    CARAPI_(Int32) CountUTFBytes(
        /* [in] */ Char32 srcChar);

    CARAPI_(void) WriteUTFBytesToBuffer(
        /* [in] */ Byte* dstP,
        /* [in] */ Char32 srcChar,
        /* [in] */ Int32 bytes);

protected:
    /**
     * The number of bytes written out so far.
     */
    Int32 mWritten;

    AutoPtr<ArrayOf<Byte> > mBuff;
};

} // namespace IO
} // namespace Elastos

#endif
