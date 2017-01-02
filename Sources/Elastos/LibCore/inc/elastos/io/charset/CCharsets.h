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

#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETS_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETS_H__

#include "_Elastos_IO_Charset_CCharsets.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCharsets)
    , public Singleton
    , public ICharsets
{
private:
    /**
     * Approximates java.lang.UnsafeByteSequence so we don't have to pay the cost of calling back into
     * Java when converting a char[] to a UTF-8 byte[]. This lets us have UTF-8 conversions slightly
     * faster than ICU for large char[]s without paying for the NIO overhead with small char[]s.
     *
     * We could avoid this by keeping the UTF-8 bytes on the native heap until we're done and only
     * creating a byte[] on the Java heap when we know how big it needs to be, but one shouldn't lie
     * to the garbage collector (nor hide potentially large allocations from it).
     *
     * Because a call to append might require an allocation, it might fail. Callers should always
     * check the return value of append.
     */
    class NativeUnsafeByteSequence {
    public:
        NativeUnsafeByteSequence();

        ~NativeUnsafeByteSequence();

        Boolean Append(
            /* [in] */ Byte b);

        Boolean Resize(
            /* [in] */ Int32 newSize);

        AutoPtr<ArrayOf<Byte> > ToByteArray();

    private:
        // Disallow copy and assignment.
        NativeUnsafeByteSequence(const NativeUnsafeByteSequence& sequence);
        void operator=(const NativeUnsafeByteSequence&);

    private:
        AutoPtr<ArrayOf<Byte> > mRawArray;
        Int32 mSize;
        Int32 mOffset;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * A cheap and type-safe constant for the ISO-8859-1 Charset.
     */
    CARAPI GetISO_8859_1(
        /* [out] */ ICharset** charset);

    /**
     * A cheap and type-safe constant for the US-ASCII Charset.
     */
    CARAPI GetUS_ASCII(
        /* [out] */ ICharset** charset);

    /**
     * A cheap and type-safe constant for the UTF-8 Charset.
     */
    CARAPI GetUTF_8(
        /* [out] */ ICharset** charset);

    /**
     * Returns a new byte array containing the bytes corresponding to the given characters,
     * encoded in US-ASCII. Unrepresentable characters are replaced by (byte) '?'.
     */
    CARAPI ToAsciiBytes(
        /* [in] */ const ArrayOf<Char32>& chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns a new byte array containing the bytes corresponding to the given characters,
     * encoded in ISO-8859-1. Unrepresentable characters are replaced by (byte) '?'.
     */
    CARAPI ToIsoLatin1Bytes(
        /* [in] */ const ArrayOf<Char32>& chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns a new byte array containing the bytes corresponding to the given characters,
     * encoded in UTF-8. All characters are representable in UTF-8.
     */
    CARAPI ToUtf8Bytes(
        /* [in] */ const ArrayOf<Char32>& chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns a new byte array containing the bytes corresponding to the given characters,
     * encoded in UTF-16BE. All characters are representable in UTF-16BE.
     */
    CARAPI ToBigEndianUtf16Bytes(
        /* [in] */ const ArrayOf<Char32>& chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Decodes the given US-ASCII bytes into the given char[]. Equivalent to but faster than:
     *
     * for (int i = 0; i < count; ++i) {
     *     char ch = (char) (data[start++] & 0xff);
     *     value[i] = (ch <= 0x7f) ? ch : REPLACEMENT_CHAR;
     * }
     */
    CARAPI AsciiBytesToChars(
        /* [in] */ const ArrayOf<Byte>& bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Char32>** chars);

    /**
     * Decodes the given ISO-8859-1 bytes into the given char[]. Equivalent to but faster than:
     *
     * for (int i = 0; i < count; ++i) {
     *     value[i] = (char) (data[start++] & 0xff);
     * }
     */
    CARAPI IsoLatin1BytesToChars(
        /* [in] */ const ArrayOf<Byte>& bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Char32>** chars);

private:
    /**
     * Translates the given characters to US-ASCII or ISO-8859-1 bytes, using the fact that
     * Unicode code points between U+0000 and U+007f inclusive are identical to US-ASCII, while
     * U+0000 to U+00ff inclusive are identical to ISO-8859-1.
     */
    CARAPI CharsToBytes(
        /* [in] */ const ArrayOf<Char32>& chars,
        /* [in] */ const Int32 offset,
        /* [in] */ const Int32 length,
        /* [in] */ const Byte maxValidChar,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

public:
    static const AutoPtr<ICharset> ISO_8859_1;

    static const AutoPtr<ICharset> US_ASCII;

    static const AutoPtr<ICharset> UTF_8;
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARSET_ELASTOS_IO_CCHARSETS_H__
