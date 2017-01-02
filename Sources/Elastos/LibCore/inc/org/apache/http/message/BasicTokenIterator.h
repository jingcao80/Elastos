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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICTOKENITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICTOKENITERATOR_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IIterator;
using Org::Apache::Http::ITokenIterator;
using Org::Apache::Http::IHeaderIterator;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link TokenIterator}.
 * This implementation parses <tt>#token<tt> sequences as
 * defined by RFC 2616, section 2.
 * It extends that definition somewhat beyond US-ASCII.
 *
 * @version $Revision: 602520 $
 */
class BasicTokenIterator
    : public Object
    , public IBasicTokenIterator
    , public ITokenIterator
    , public IIterator
{
public:
    CARAPI constructor(
        /* [in] */ IHeaderIterator* headerIterator);

    CAR_INTERFACE_DECL()

    BasicTokenIterator();

    virtual ~BasicTokenIterator() {}

    /**
     * Indicates whether there is another token in this iteration.
     *
     * @return  <code>true</code> if there is another token,
     *          <code>false</code> otherwise
     */
    CARAPI HasNext(
        /* [out] */ Boolean* hasNext);

    /**
     * Obtains the next header element from this iteration.
     * This method should only be called while {@link #hasNext hasNext}
     * is true.
     *
     * @return  the next header element in this iteration
     */
    CARAPI NextToken(
        /* [out] */ String* element);

    /**
     * Returns the next token.
     * Same as {@link #nextToken}, but with generic return type.
     *
     * @return  the next token in this iteration
     *
     * @throws NoSuchElementException   if there are no more tokens
     * @throws ParseException   if an invalid header value is encountered
     */
    CARAPI GetNext(
        /* [out] */ IInterface** object);

    /**
     * Removing tokens is not supported.
     *
     * @throws UnsupportedOperationException    always
     */
    CARAPI Remove();

protected:
    /**
     * Determines the next token.
     * If found, the token is stored in {@link #currentToken}.
     * The return value indicates the position after the token
     * in {@link #currentHeader}. If necessary, the next header
     * will be obtained from {@link #headerIt}.
     * If not found, {@link #currentToken} is set to <code>null</code>.
     *
     * @param from      the position in the current header at which to
     *                  start the search, -1 to search in the first header
     *
     * @return  the position after the found token in the current header, or
     *          negative if there was no next token
     *
     * @throws ParseException   if an invalid header value is encountered
     */
    CARAPI FindNext(
        /* [in] */ Int32 from,
        /* [out] */ Int32* pos);

    /**
     * Creates a new token to be returned.
     * Called from {@link #findNext findNext} after the token is identified.
     * The default implementation simply calls
     * {@link java.lang.String#substring String.substring}.
     * <br/>
     * If header values are significantly longer than tokens, and some
     * tokens are permanently referenced by the application, there can
     * be problems with garbage collection. A substring will hold a
     * reference to the full characters of the original string and
     * therefore occupies more memory than might be expected.
     * To avoid this, override this method and create a new string
     * instead of a substring.
     *
     * @param value     the full header value from which to create a token
     * @param start     the index of the first token character
     * @param end       the index after the last token character
     *
     * @return  a string representing the token identified by the arguments
     */
    CARAPI CreateToken(
        /* [in] */ const String& value,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ String* str);

    /**
     * Determines the starting position of the next token.
     * This method will iterate over headers if necessary.
     *
     * @param from      the position in the current header at which to
     *                  start the search
     *
     * @return  the position of the token start in the current header,
     *          negative if no token start could be found
     */
    CARAPI FindTokenStart(
        /* [in] */ Int32 from,
        /* [out] */ Int32* pos);

    /**
     * Determines the position of the next token separator.
     * Because of multi-header joining rules, the end of a
     * header value is a token separator. This method does
     * therefore not need to iterate over headers.
     *
     * @param from      the position in the current header at which to
     *                  start the search
     *
     * @return  the position of a token separator in the current header,
     *          or at the end
     *
     * @throws ParseException
     *         if a new token is found before a token separator.
     *         RFC 2616, section 2.1 explicitly requires a comma between
     *         tokens for <tt>#</tt>.
     */
    CARAPI FindTokenSeparator(
        /* [in] */ Int32 from,
        /* [out] */ Int32* pos);

    /**
     * Determines the ending position of the current token.
     * This method will not leave the current header value,
     * since the end of the header value is a token boundary.
     *
     * @param from      the position of the first character of the token
     *
     * @return  the position after the last character of the token.
     *          The behavior is undefined if <code>from</code> does not
     *          point to a token character in the current header value.
     */
    CARAPI FindTokenEnd(
        /* [in] */ Int32 from,
        /* [out] */ Int32* pos);

    /**
     * Checks whether a character is a token separator.
     * RFC 2616, section 2.1 defines comma as the separator for
     * <tt>#token</tt> sequences. The end of a header value will
     * also separate tokens, but that is not a character check.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is a token separator,
     *          <code>false</code> otherwise
     */
    CARAPI IsTokenSeparator(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    /**
     * Checks whether a character is a whitespace character.
     * RFC 2616, section 2.2 defines space and horizontal tab as whitespace.
     * The optional preceeding line break is irrelevant, since header
     * continuation is handled transparently when parsing messages.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is whitespace,
     *          <code>false</code> otherwise
     */
    CARAPI IsWhitespace(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    /**
     * Checks whether a character is a valid token character.
     * Whitespace, control characters, and HTTP separators are not
     * valid token characters. The HTTP specification (RFC 2616, section 2.2)
     * defines tokens only for the US-ASCII character set, this
     * method extends the definition to other character sets.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is a valid token start,
     *          <code>false</code> otherwise
     */
    CARAPI IsTokenChar(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

    /**
     * Checks whether a character is an HTTP separator.
     * The implementation in this class checks only for the HTTP separators
     * defined in RFC 2616, section 2.2. If you need to detect other
     * separators beyond the US-ASCII character set, override this method.
     *
     * @param ch        the character to check
     *
     * @return  <code>true</code> if the character is an HTTP separator
     */
    CARAPI IsHttpSeparator(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* result);

protected:
    /** The iterator from which to obtain the next header. */
    AutoPtr<IHeaderIterator> mHeaderIt;

    /**
     * The value of the current header.
     * This is the header value that includes {@link #currentToken}.
     * Undefined if the iteration is over.
     */
    String mCurrentHeader;

    /**
     * The token to be returned by the next call to {@link #currentToken}.
     * <code>null</code> if the iteration is over.
     */
    String mCurrentToken;

    /**
     * The position after {@link #currentToken} in {@link #currentHeader}.
     * Undefined if the iteration is over.
     */
    Int32 mSearchPos;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICTOKENITERATOR_H_
