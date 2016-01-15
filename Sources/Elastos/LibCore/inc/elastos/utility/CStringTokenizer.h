#ifndef __ELASTOS_UTILITY_CSTINGTOKENIZER_H__
#define __ELASTOS_UTILITY_CSTINGTOKENIZER_H__

#include "_Elastos_Utility_CStringTokenizer.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Utility {

CarClass(CStringTokenizer)
    , public Object
    , public IStringTokenizer
    , public IEnumeration
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStringTokenizer();

    /**
     * Constructs a new {@code StringTokenizer} for the parameter string using
     * whitespace as the delimiter. The {@code returnDelimiters} flag is set to
     * {@code false}.
     *
     * @param string
     *            the string to be tokenized.
     */
    CARAPI constructor(
        /* [in] */ const String& string);

    /**
     * Constructs a new {@code StringTokenizer} for the parameter string using
     * the specified delimiters. The {@code returnDelimiters} flag is set to
     * {@code false}. If {@code delimiters} is {@code null}, this constructor
     * doesn't throw an {@code Exception}, but later calls to some methods might
     * throw a {@code NullPointerException}.
     *
     * @param string
     *            the string to be tokenized.
     * @param delimiters
     *            the delimiters to use.
     */
    CARAPI constructor(
        /* [in] */ const String& string,
        /* [in] */ const String& delimiters);

    /**
     * Constructs a new {@code StringTokenizer} for the parameter string using
     * the specified delimiters, returning the delimiters as tokens if the
     * parameter {@code returnDelimiters} is {@code true}. If {@code delimiters}
     * is null this constructor doesn't throw an {@code Exception}, but later
     * calls to some methods might throw a {@code NullPointerException}.
     *
     * @param string
     *            the string to be tokenized.
     * @param delimiters
     *            the delimiters to use.
     * @param returnDelimiters
     *            {@code true} to return each delimiter as a token.
     */
    CARAPI constructor(
        /* [in] */ const String& string,
        /* [in] */ const String& delimiters,
        /* [in] */ Boolean returnDelimiters);

    CARAPI HasMoreElements(
        /* [out] */ Boolean * value);

    /**
     * Returns the next element in this {@code Enumeration}.
     *
     * @return the next element..
     * @throws NoSuchElementException
     *             if there are no more elements.
     * @see #hasMoreElements
     */
    CARAPI GetNextElement(
        /* [out] */ IInterface ** inter);

    /**
     * Returns the number of unprocessed tokens remaining in the string.
     *
     * @return number of tokens that can be retreived before an {@code
     *         Exception} will result from a call to {@code nextToken()}.
     */
    CARAPI CountTokens(
        /* [out] */ Int32* value);

    /**
     * Returns {@code true} if unprocessed tokens remain.
     *
     * @return {@code true} if unprocessed tokens remain.
     */
    CARAPI HasMoreTokens(
        /* [out] */ Boolean* value);

    /**
     * Returns the next token in the string as a {@code String}.
     *
     * @return next token in the string as a {@code String}.
     * @throws NoSuchElementException
     *                if no tokens remain.
     */
    CARAPI GetNextToken(
        /* [out] */ String* str);

    /**
     * Returns the next token in the string as a {@code String}. The delimiters
     * used are changed to the specified delimiters.
     *
     * @param delims
     *            the new delimiters to use.
     * @return next token in the string as a {@code String}.
     * @throws NoSuchElementException
     *                if no tokens remain.
     */
    CARAPI GetNextToken(
        /* [in] */ const String& delims,
        /* [out] */ String* str);

private:
    String mString;

    String mDelimiters;

    Boolean mReturnDelimiters;

    Int32 mPosition;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CSTINGTOKENIZER_H__