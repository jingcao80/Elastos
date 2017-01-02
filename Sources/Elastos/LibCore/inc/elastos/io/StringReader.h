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

#ifndef __ELASTOS_IO_STRINGREADER_H__
#define __ELASTOS_IO_STRINGREADER_H__

#include "Reader.h"

namespace Elastos {
namespace IO {

class StringReader
    : public Reader
    , public IStringReader
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Construct a new {@code StringReader} with {@code str} as source. The size
     * of the reader is set to the {@code length()} of the string and the Object
     * to synchronize access through is set to {@code str}.
     *
     * @param str
     *            the source string for this reader.
     */
    CARAPI constructor(
        /* [in] */ const String& str);

    /**
     * Closes this reader. Once it is closed, read operations on this reader
     * will throw an {@code IOException}. Only the first invocation of this
     * method has any effect.
     */
    CARAPI Close();

    /**
     * Returns a boolean indicating whether this reader is closed.
     *
     * @return {@code true} if closed, otherwise {@code false}.
     */
    CARAPI_(Boolean) IsClosed();

    /**
     * Sets a mark position in this reader. The parameter {@code readLimit} is
     * ignored for this class. Calling {@code reset()} will reposition the
     * reader back to the marked position.
     *
     * @param readLimit
     *            ignored for {@code StringReader} instances.
     * @throws IllegalArgumentException
     *             if {@code readLimit < 0}.
     * @throws IOException
     *             if this reader is closed.
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    /**
     * Indicates whether this reader supports the {@code mark()} and {@code
     * reset()} methods. This implementation returns {@code true}.
     *
     * @return always {@code true}.
     */
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * Reads a single character from the source string and returns it as an
     * integer with the two higher-order bytes set to 0. Returns -1 if the end
     * of the source string has been reached.
     *
     * @return the character read or -1 if the end of the source string has been
     *         reached.
     * @throws IOException
     *             if this reader is closed.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads up to {@code count} characters from the source string and stores
     * them at {@code offset} in the character array {@code buffer}. Returns the
     * number of characters actually read or -1 if the end of the source string
     * has been reached.
     *
     * @throws IndexOutOfBoundsException
                if {@code offset < 0 || count < 0 || offset + count > buffer.length}.
     * @throws IOException
     *             if this reader is closed.
     */
    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Char32>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

    /**
     * Indicates whether this reader is ready to be read without blocking. This
     * implementation always returns {@code true}.
     *
     * @return always {@code true}.
     * @throws IOException
     *             if this reader is closed.
     * @see #read()
     * @see #read(char[], int, int)
     */
    CARAPI IsReady(
        /* [out] */ Boolean* ready);

    /**
     * Resets this reader's position to the last {@code mark()} location.
     * Invocations of {@code read()} and {@code skip()} will occur from this new
     * location. If this reader has not been marked, it is reset to the
     * beginning of the source string.
     *
     * @throws IOException
     *             if this reader is closed.
     * @see #mark(int)
     * @see #markSupported()
     */
    CARAPI Reset();

    /**
     * Moves {@code ns} characters in the source string. Unlike the {@link
     * Reader#skip(long) overridden method}, this method may skip negative skip
     * distances: this rewinds the input so that characters may be read again.
     * When the end of the source string has been reached, the input cannot be
     * rewound.
     *
     * @param ns
     *            the maximum number of characters to skip. Positive values skip
     *            forward; negative values skip backward.
     * @return the number of characters actually skipped. This is bounded below
     *            by the number of characters already read and above by the
     *            number of characters remaining:<br> {@code -(num chars already
     *            read) <= distance skipped <= num chars remaining}.
     * @throws IOException
     *             if this reader is closed.
     * @see #mark(int)
     * @see #markSupported()
     * @see #reset()
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

protected:
    StringReader();

    virtual ~StringReader();

private:
    CARAPI CheckNotClosed();

private:
    String mStr;

    Int32 mMarkpos;

    Int32 mPos;

    Int32 mCount;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_STRINGREADER_H__
