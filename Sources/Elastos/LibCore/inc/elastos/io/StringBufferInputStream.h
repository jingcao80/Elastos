
#ifndef __ELASTOS_IO_STRINGBUFFERINPUTSTREAM_H__
#define __ELASTOS_IO_STRINGBUFFERINPUTSTREAM_H__

#include "InputStream.h"

namespace Elastos {
namespace IO {

class StringBufferInputStream
    : public InputStream
    , public IStringBufferInputStream
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& str);

    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Reads a single byte from the source string and returns it as an integer
     * in the range from 0 to 255. Returns -1 if the end of the source string
     * has been reached.
     *
     * @return the byte read or -1 if the end of the source string has been
     *         reached.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Resets this stream to the beginning of the source string.
     */
    CARAPI Reset();

    /**
     * Skips {@code n} characters in the source string. It does nothing and
     * returns 0 if {@code n} is negative. Less than {@code n} characters are
     * skipped if the end of the source string is reached before the operation
     * completes.
     *
     * @param n
     *            the number of characters to skip.
     * @return the number of characters actually skipped.
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

protected:
    StringBufferInputStream();

    virtual ~StringBufferInputStream();

protected:
    /**
     * The source string containing the data to read.
     */
    String mBuffer;

    /**
     * The total number of characters in the source string.
     */
    Int32 mCount;

    /**
     * The current position within the source string.
     */
    Int32 mPos;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_STRINGBUFFERINPUTSTREAM_H__
