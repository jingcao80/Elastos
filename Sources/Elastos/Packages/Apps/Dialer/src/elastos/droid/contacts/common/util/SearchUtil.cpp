
#include "elastos/droid/contacts/common/util/SearchUtil.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Character.h>

using Elastos::Core::StringUtils;
using Elastos::Core::Character;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

//=================================================================
// SearchUtil::MatchedLine
//=================================================================
ECode SearchUtil::MatchedLine::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("MatchedLine{") + "line='" + mLine + "\'" +
            ", startIndex=" + StringUtils::ToString(mStartIndex) + "}";
    return NOERROR;
}


//=================================================================
// SearchUtil
//=================================================================
AutoPtr<SearchUtil::MatchedLine> SearchUtil::FindMatchingLine(
    /* [in] */ const String& contents,
    /* [in] */ const String& substring)
{
    AutoPtr<MatchedLine> matched = new MatchedLine();

    // Snippet may contain multiple lines separated by "\n".
    // Locate the lines of the content that contain the substring.
    Int32 index = Contains(contents, substring);
    if (index != -1) {
        // Match found.  Find the corresponding line.
        Int32 start = index - 1;
        while (start > -1) {
            if (contents.GetChar(start) == '\n') {
                break;
            }
            start--;
        }
        Int32 end = index + 1;
        while (end < contents.GetLength()) {
            if (contents.GetChar(end) == '\n') {
                break;
            }
            end++;
        }
        matched->mLine = contents.Substring(start + 1, end);
        matched->mStartIndex = index - (start + 1);
    }
    return matched;
}

Int32 SearchUtil::Contains(
    /* [in] */ const String& value,
    /* [in] */ const String& substring)
{
    if (value.GetLength() < substring.GetLength()) {
        return -1;
    }

    // i18n support
    // Generate the code points for the substring once.
    // There will be a maximum of substring.length code points.  But may be fewer.
    // Since the array length is not an accurate size, we need to keep a separate variable.

    for (Int32 i = 0; i < value.GetLength(); i = FindNextTokenStart(value, i)) {
        Int32 numMatch = 0;
        for (Int32 j = i; j < value.GetLength() && numMatch < substring.GetLength(); ++numMatch) {
            Char32 valueCp = Character::ToLowerCase(value.GetChar(j));
            Char32 substringCp = substring.GetChar(numMatch);
            if (valueCp != substringCp) {
                break;
            }
            j++;
        }
        if (numMatch == substring.GetLength()) {
            return i;
        }
    }
    return -1;
}

Int32 SearchUtil::FindNextTokenStart(
    /* [in] */ const String& line,
    /* [in] */ Int32 startIndex)
{
    Int32 index = startIndex;

    // If already in token, eat remainder of token.
    while (index <= line.GetLength()) {
        if (index == line.GetLength()) {
            // No more tokens.
            return index;
        }
        if (!Character::IsLetterOrDigit(line.GetChar(index))) {
            break;
        }
        index++;
    }

    // Out of token, eat all consecutive delimiters.
    while (index <= line.GetLength()) {
        if (index == line.GetLength()) {
            return index;
        }
        if (Character::IsLetterOrDigit(line.GetChar(index))) {
            break;
        }
        index++;
    }

    return index;
}

String SearchUtil::CleanStartAndEndOfSearchQuery(
    /* [in] */ const String& query)
{
    Int32 start = 0;
    while (start < query.GetLength()) {
        Char32 codePoint = query.GetChar(start);
        if (Character::IsLetterOrDigit(codePoint)) {
            break;
        }
        start++;
    }

    if (start == query.GetLength()) {
        // All characters are delimiters.
        return String("");
    }

    Int32 end = query.GetLength() - 1;
    while (end > -1) {
        if (Character::IsLowSurrogate(query.GetChar(end))) {
            // Assume valid i18n string.  There should be a matching high surrogate before it.
            end--;
        }
        Char32 codePoint = query.GetChar(end);
        if (Character::IsLetterOrDigit(codePoint)) {
            break;
        }
        end--;
    }

    // end is a letter or digit.
    return query.Substring(start, end + 1);
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
