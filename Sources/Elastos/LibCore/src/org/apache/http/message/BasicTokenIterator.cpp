
#include "org/apache/http/message/BasicTokenIterator.h"
#include "elastos/core/Character.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Character;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

BasicTokenIterator::BasicTokenIterator()
    : mSearchPos(0)
{}

CAR_INTERFACE_IMPL_3(BasicTokenIterator, Object, IBasicTokenIterator, ITokenIterator, IIterator)

ECode BasicTokenIterator::constructor(
    /* [in] */ IHeaderIterator* headerIterator)
{
    if (headerIterator == NULL) {
        Logger::E("BasicTokenIterator", "Header iterator must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mHeaderIt = headerIterator;
    FindNext(-1, &mSearchPos);
    return NOERROR;
}

ECode BasicTokenIterator::HasNext(
    /* [out] */ Boolean* hasNext)
{
    VALIDATE_NOT_NULL(hasNext)
    *hasNext = (mCurrentToken != NULL);
    return NOERROR;
}

ECode BasicTokenIterator::NextToken(
    /* [out] */ String* element)
{
    VALIDATE_NOT_NULL(element)
    *element = String(NULL);
    if (mCurrentToken == NULL) {
        Logger::E("BasicTokenIterator", "Iteration already finished.");
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    String result = mCurrentToken;
    // updates currentToken, may trigger ParseException:
    FindNext(mSearchPos, &mSearchPos);

    *element = result;
    return NOERROR;
}

ECode BasicTokenIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    String elem;
    NextToken(&elem);
    AutoPtr<ICharSequence> cs;
    CString::New(elem, (ICharSequence**)&cs);
    *object = cs->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode BasicTokenIterator::Remove()
{
    Logger::E("BasicTokenIterator", "Removing tokens is not supported.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode BasicTokenIterator::FindNext(
    /* [in] */ Int32 from,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    if (from < 0) {
        // called from the constructor, initialize the first header
        Boolean hasNext;
        if (mHeaderIt->HasNext(&hasNext), !hasNext) {
            *pos = -1;
            return NOERROR;
        }
        AutoPtr<IHeader> header;
        mHeaderIt->NextHeader((IHeader**)&header);
        header->GetValue(&mCurrentHeader);
        from = 0;
    }
    else {
        // called after a token, make sure there is a separator
        FindTokenSeparator(from, &from);
    }

    Int32 start;
    FindTokenStart(from, &start);
    if (start < 0) {
        mCurrentToken = NULL;
        *pos = -1; // nothing found
        return NOERROR;
    }

    Int32 end;
    FindTokenEnd(start, &end);
    CreateToken(mCurrentHeader, start, end, &mCurrentToken);
    *pos = end;
    return NOERROR;
}

ECode BasicTokenIterator::CreateToken(
    /* [in] */ const String& value,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = value.Substring(start, end);
    return NOERROR;
}

ECode BasicTokenIterator::FindTokenStart(
    /* [in] */ Int32 from,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = -1;

    if (from < 0) {
        Logger::E("BasicTokenIterator", "Search position must not be negative: %d", from);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean found = FALSE;
    while (!found && (!mCurrentHeader.IsNull())) {
        Int32 to = mCurrentHeader.GetLength();
        while (!found && (from < to)) {
            Char32 ch = mCurrentHeader.GetChar(from);
            Boolean isWhitespace, isSeparator, isTokenChar;
            if ((IsTokenSeparator(ch, &isSeparator), isSeparator) || (IsWhitespace(ch, &isWhitespace), isWhitespace)) {
                // whitspace and token separators are skipped
                from++;
            }
            else if (IsTokenChar(mCurrentHeader.GetChar(from), &isTokenChar), isTokenChar) {
                // found the start of a token
                found = TRUE;
            }
            else {
                Logger::E("BasicTokenIterator", "Invalid character before token (pos %d): %s", from, mCurrentHeader.string());
                return E_PARSE_EXCEPTION;
            }
        }
        if (!found) {
            Boolean hasNext;
            if (mHeaderIt->HasNext(&hasNext), hasNext) {
                AutoPtr<IHeader> header;
                mHeaderIt->NextHeader((IHeader**)&header);
                header->GetValue(&mCurrentHeader);
                from = 0;
            }
            else {
                mCurrentHeader = String(NULL);
            }
        }
    } // while headers

    *pos = found ? from : -1;
    return NOERROR;
}

ECode BasicTokenIterator::FindTokenSeparator(
    /* [in] */ Int32 from,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = -1;

    if (from < 0) {
        Logger::E("BasicTokenIterator", "Search position must not be negative: %d", from);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean found = FALSE;
    Int32 to = mCurrentHeader.GetLength();
    while (!found && (from < to)) {
        Char32 ch = mCurrentHeader.GetChar(from);
        Boolean isSeparator, isWhitespace, isTokenChar;
        if (IsTokenSeparator(ch, &isSeparator), isSeparator) {
            found = TRUE;
        }
        else if (IsWhitespace(ch, &isWhitespace), isWhitespace) {
            from++;
        }
        else if (IsTokenChar(ch, &isTokenChar), isTokenChar) {
            Logger::E("BasicTokenIterator", "Tokens without separator (pos %d): %s", from, mCurrentHeader.string());
            return E_PARSE_EXCEPTION;
        }else {
            Logger::E("BasicTokenIterator", "Invalid character after token (pos %d): %s", from, mCurrentHeader.string());
            return E_PARSE_EXCEPTION;
        }
    }

    *pos = from;
    return NOERROR;
}

ECode BasicTokenIterator::FindTokenEnd(
    /* [in] */ Int32 from,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = -1;

    if (from < 0) {
        Logger::E("BasicTokenIterator", "Search position must not be negative: %d", from);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 to = mCurrentHeader.GetLength();
    Int32 end = from + 1;
    Boolean isTokenChar;
    while ((end < to) && (IsTokenChar(mCurrentHeader.GetChar(end), &isTokenChar))) {
        end++;
    }

    *pos = end;
    return NOERROR;
}

ECode BasicTokenIterator::IsTokenSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (ch == ',');
    return NOERROR;
}

ECode BasicTokenIterator::IsWhitespace(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // we do not use Character.isWhitspace(ch) here, since that allows
    // many control characters which are not whitespace as per RFC 2616
    *result = ((ch == '\t') || Character::IsSpaceChar(ch));
    return NOERROR;
}

ECode BasicTokenIterator::IsTokenChar(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // common sense extension of ALPHA + DIGIT
    if (Character::IsLetterOrDigit(ch)) {
        *result = TRUE;
        return NOERROR;
    }

    // common sense extension of CTL
    if (Character::IsISOControl(ch)) {
        *result = FALSE;
        return NOERROR;
    }

    // no common sense extension for this
    Boolean isSeparator;
    if (IsHttpSeparator(ch, &isSeparator), isSeparator) {
        *result = FALSE;
        return NOERROR;
    }

    // RFC 2616, section 2.2 defines a token character as
    // "any CHAR except CTLs or separators". The controls
    // and separators are included in the checks above.
    // This will yield unexpected results for Unicode format characters.
    // If that is a problem, overwrite isHttpSeparator(char) to filter
    // out the false positives.
    *result = TRUE;
    return NOERROR;
}

ECode BasicTokenIterator::IsHttpSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (String(" ,;=()<>@:\\\"/[]?{}\t")/*HTTP_SEPARATORS*/.IndexOf(ch) >= 0);
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
