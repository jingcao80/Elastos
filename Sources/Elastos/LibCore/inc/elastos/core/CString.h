
#ifndef __ELASTOS_CORE_CSTRING_H__
#define __ELASTOS_CORE_CSTRING_H__

#include "_Elastos_Core_CString.h"
#include "Object.h"

namespace Elastos {
namespace Core {

CarClass(CString)
    , public Object
    , public ICharSequence
    , public IString
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& str);

    /**
     * Returns the number of characters in this sequence.
     *
     * @return the number of characters.
     */
    CARAPI GetLength(
        /* [out] */ Int32* number);

    /**
     * Returns the character at the specified index, with the first character
     * having index zero.
     *
     * @param index
     *            the index of the character to return.
     * @return the requested character.
     * @throws IndexOutOfBoundsException
     *             if {@code index < 0} or {@code index} is greater than the
     *             length of this sequence.
     */
    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* c);

    /**
     * Returns a {@code CharSequence} from the {@code start} index (inclusive)
     * to the {@code end} index (exclusive) of this sequence.
     *
     * @param start
     *            the start offset of the sub-sequence. It is inclusive, that
     *            is, the index of the first character that is included in the
     *            sub-sequence.
     * @param end
     *            the end offset of the sub-sequence. It is exclusive, that is,
     *            the index of the first character after those that are included
     *            in the sub-sequence
     * @return the requested sub-sequence.
     * @throws IndexOutOfBoundsException
     *             if {@code start < 0}, {@code end < 0}, {@code start > end},
     *             or if {@code start} or {@code end} are greater than the
     *             length of this sequence.
     */
    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    /**
     * Returns a string with the same characters in the same order as in this
     * sequence.
     *
     * @return a string based on this sequence.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

private:
    String mString;
};

} // namespace Core
} // namespace Elastos

#endif // __ELASTOS_CORE_CSTRING_H__
