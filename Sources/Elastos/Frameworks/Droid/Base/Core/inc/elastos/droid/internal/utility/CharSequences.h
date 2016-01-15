#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CHARSEQUENCES_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CHARSEQUENCES_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * {@link CharSequence} utility methods.
 */
class CharSequences
{
private:
    class AsciiBytesCharSequence1
        : public Object
        , public ICharSequence
    {
    public:
        AsciiBytesCharSequence1(
            /* [in] */ ArrayOf<Byte>* bytes);

        CAR_INTERFACE_DECL()

        CARAPI GetCharAt(
            /* [in] */ Int32 index,
            /* [out] */ Char32* c);

        CARAPI GetLength(
            /* [out] */ Int32* length);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** cs);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<ArrayOf<Byte> > mBytes;
    };

    class AsciiBytesCharSequence2
        : public Object
        , public ICharSequence
    {
    public:
        AsciiBytesCharSequence2(
            /* [in] */ ArrayOf<Byte>* bytes,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CAR_INTERFACE_DECL()

        CARAPI GetCharAt(
            /* [in] */ Int32 index,
            /* [out] */ Char32* c);

        CARAPI GetLength(
            /* [out] */ Int32* length);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** cs);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<ArrayOf<Byte> > mBytes;
        Int32 mStart;
        Int32 mEnd;
    };


public:
    /**
     * Adapts {@link CharSequence} to an array of ASCII (7-bits per character)
     * bytes.
     *
     * @param bytes ASCII bytes
     */
    static CARAPI_(AutoPtr<ICharSequence>) ForAsciiBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    /**
     * Adapts {@link CharSequence} to an array of ASCII (7-bits per character)
     * bytes.
     *
     * @param bytes ASCII bytes
     * @param start index, inclusive
     * @param end index, exclusive
     *
     * @throws IndexOutOfBoundsException if start or end are negative, if end
     *  is greater than length(), or if start is greater than end
     */
    static CARAPI ForAsciiBytes(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** cs);

    static CARAPI Validate(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 length);

    /**
     * Compares two character sequences for equality.
     */
    static CARAPI_(Boolean) Equals(
         /* [in] */ ICharSequence* a,
         /* [in] */ ICharSequence* b);

        /**
     * Compares two character sequences with API like {@link Comparable#compareTo}.
     *
     * @param me The CharSequence that receives the compareTo call.
     * @param another The other CharSequence.
     * @return See {@link Comparable#compareTo}.
     */
    static CARAPI_(Int32) CompareToIgnoreCase(
         /* [in] */ ICharSequence* a,
         /* [in] */ ICharSequence* b);

private:
    CharSequences();
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CHARSEQUENCES_H__
