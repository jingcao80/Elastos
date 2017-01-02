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

#ifndef __ELASTOS_IO_CHARARRAYWRITER_H__
#define __ELASTOS_IO_CHARARRAYWRITER_H__

#include "Writer.h"

namespace Elastos {
namespace IO {

class CharArrayWriter
    : public Writer
    , public ICharArrayWriter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 initialSize);

protected:
    CharArrayWriter();

    virtual ~CharArrayWriter();

public:

    /**
     * Closes this writer. The implementation in {@code CharArrayWriter} does nothing.
     */

    CARAPI Close();

    /**
     * Flushes this writer. The implementation in {@code CharArrayWriter} does nothing.
     */
    CARAPI Flush();

    /**
     * Resets this writer. The current write position is reset to the beginning
     * of the buffer. All written characters are lost and the size of this
     * writer is set to 0.
     */
    virtual CARAPI Reset();

    /**
     * Returns the size of this writer, that is the number of characters it
     * stores. This number changes if this writer is reset or when more
     * characters are written to it.
     *
     * @return this CharArrayWriter's current size in characters.
     */
    virtual CARAPI GetSize(
        /* [out] */Int32* size);

    /**
     * Returns the contents of the receiver as a char array. The array returned
     * is a copy and any modifications made to this writer after calling this
     * method are not reflected in the result.
     *
     * @return this CharArrayWriter's contents as a new char array.
     */
    virtual CARAPI ToCharArray(
        /* [out] */ ArrayOf<Char32>** str);

    /**
     * Returns the contents of this {@code CharArrayWriter} as a string. The
     * string returned is a copy and any modifications made to this writer after
     * calling this method are not reflected in the result.
     *
     * @return this CharArrayWriters contents as a new string.
     */
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Writes the specified character {@code oneChar} to this writer.
     * This implementation writes the two low order bytes of the integer
     * {@code oneChar} to the buffer.
     *
     * @param oneChar
     *            the character to write.
     */
    CARAPI Write(
        /* [in] */ Int32 oneChar32);

    /**
     * Writes {@code count} characters starting at {@code offset} in {@code c}
     * to this writer.
     *
     * @param buffer
     *            the non-null array containing characters to write.
     * @param offset
     *            the index of the first character in {@code buf} to write.
     * @param len
     *            maximum number of characters to write.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code len < 0}, or if
     *             {@code offset + len} is bigger than the size of {@code c}.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /**
     * Writes {@code count} number of characters starting at {@code offset} from
     * the string {@code str} to this CharArrayWriter.
     *
     * @param str
     *            the non-null string containing the characters to write.
     * @param offset
     *            the index of the first character in {@code str} to write.
     * @param len
     *            the number of characters to retrieve and write.
     * @throws NullPointerException
     *             if {@code str} is {@code null}.
     * @throws StringIndexOutOfBoundsException
     *             if {@code offset < 0} or {@code len < 0}, or if
     *             {@code offset + len} is bigger than the length of
     *             {@code str}.
     */
    CARAPI Write(
        /* [in] */ const String& str,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    using Writer::Write;

    /**
     * Writes the contents of this {@code CharArrayWriter} to another {@code
     * Writer}. The output is all the characters that have been written to the
     * receiver since the last reset or since it was created.
     *
     * @param out
     *            the non-null {@code Writer} on which to write the contents.
     * @throws NullPointerException
     *             if {@code out} is {@code null}.
     * @throws IOException
     *             if an error occurs attempting to write out the contents.
     */
    virtual CARAPI WriteTo(
        /* [in] */ IWriter* out);

    /**
     * Appends a char {@code c} to the {@code CharArrayWriter}. The method works
     * the same way as {@code write(c)}.
     *
     * @param c
     *            the character appended to the CharArrayWriter.
     * @return this CharArrayWriter.
     */

    CARAPI AppendChar(
        /* [in] */ Char32 c);

    /**
     * Appends a {@code CharSequence} to the {@code CharArrayWriter}. The method
     * works the same way as {@code write(csq.toString())}. If {@code csq} is
     * {@code null}, then it will be substituted with the string {@code "null"}.
     *
     * @param csq
     *            the {@code CharSequence} appended to the {@code
     *            CharArrayWriter}, may be {@code null}.
     * @return this CharArrayWriter.
     */
    CARAPI Append(
        /* [in] */ ICharSequence* csq);

    /**
     * AppendChar a subsequence of a {@code CharSequence} to the {@code
     * CharArrayWriter}. The first and last characters of the subsequence are
     * specified by the parameters {@code start} and {@code end}. A call to
     * {@code CharArrayWriter.append(csq)} works the same way as {@code
     * CharArrayWriter.write(csq.subSequence(start, end).toString)}. If {@code
     * csq} is {@code null}, then it will be substituted with the string {@code
     * "null"}.
     *
     * @param csq
     *            the {@code CharSequence} appended to the {@code
     *            CharArrayWriter}, may be {@code null}.
     * @param start
     *            the index of the first character in the {@code CharSequence}
     *            appended to the {@code CharArrayWriter}.
     * @param end
     *            the index of the character after the last one in the {@code
     *            CharSequence} appended to the {@code CharArrayWriter}.
     * @return this CharArrayWriter.
     * @throws IndexOutOfBoundsException
     *             if {@code start < 0}, {@code end < 0}, {@code start > end},
     *             or if {@code end} is greater than the length of {@code csq}.
     */
    CARAPI Append(
        /* [in] */ ICharSequence* csq,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

private:
    CARAPI Expand(
        /* [in] */ Int32 i);

private:
    /**
     * The buffer for characters.
     */
    AutoPtr<ArrayOf<Char32> > mBuf;
    AutoPtr<Object> mBufLock;

    /**
     * The ending index of the buffer.
     */
    // In Bytes
    Int32 mCount;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARARRAYWRITER_H__
