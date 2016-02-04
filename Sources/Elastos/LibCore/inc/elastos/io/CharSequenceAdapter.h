#ifndef __ELASTOS_IO_CHARSEQUENCEADAPTER_H__
#define __ELASTOS_IO_CHARSEQUENCEADAPTER_H__

#include "CharBuffer.h"

namespace Elastos {
namespace IO {

/**
 * This class wraps a char sequence to be a char buffer.
 * <p>
 * Implementation notice:
 * <ul>
 * <li>Char sequence based buffer is always readonly.</li>
 * </ul>
 * </p>
 *
 */
class CharSequenceAdapter
    : public CharBuffer
{
public:
    CharSequenceAdapter();

    CARAPI constructor(
        /* [in] */ Int32 capacity,
        /* [in] */ ICharSequence* chseq);

    CARAPI GetPrimitiveArray(
        /* [out] */ Handle64* arrayHandle);

    static CARAPI Copy(
        /* [in] */ CharSequenceAdapter* other,
        /* [out] */ CharSequenceAdapter** buffer);

    CARAPI AsReadOnlyBuffer(
        /* [out] */ ICharBuffer** buffer);

    CARAPI Compact();

    CARAPI Duplicate(
        /* [out] */ ICharBuffer** buffer);

    CARAPI Get(
        /* [out] */ Char32* value);

    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ Char32* value);

    CARAPI Get(
        /* [out] */ ArrayOf<Char32>* dst,
        /* [in] */ Int32 dstOffset,
        /* [in] */ Int32 charCount);

    CARAPI IsDirect(
        /* [out] */ Boolean* isDirect);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* isReadOnly);

    CARAPI GetOrder(
        /* [out] */ ByteOrder* order);

    CARAPI ProtectedArray(
        /* [out, callee] */ ArrayOf<Char32>** array);

    CARAPI ProtectedArrayOffset(
        /* [out] */ Int32* offset);

    CARAPI ProtectedHasArray(
        /* [out] */ Boolean* result);

    CARAPI Put(
        /* [in] */ Char32 c);

    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Char32 c);

    CARAPI Put(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 charCount);

    CARAPI Put(
        /* [in] */ const String& str,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI Slice(
        /* [out] */ ICharBuffer** buffer);

    CARAPI GetLength(
        /* [out] */ Int32* number);

    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* c);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI ToString(
        /* [out] */ String* str);
public:
    AutoPtr<ICharSequence> mSequence;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARSEQUENCEADAPTER_H__
