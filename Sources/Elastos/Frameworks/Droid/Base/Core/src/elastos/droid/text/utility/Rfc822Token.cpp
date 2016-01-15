#include "elastos/droid/text/utility/Rfc822Token.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(Rfc822Token, Object, IRfc822Token)

Rfc822Token::Rfc822Token()
{}

ECode Rfc822Token::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& address,
    /* [in] */ const String& comment)
{
    mName = name;
    mAddress = address;
    mComment = comment;
    return NOERROR;
}

ECode Rfc822Token::GetName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mName;
    return NOERROR;
}

ECode Rfc822Token::GetAddress(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mAddress;
    return NOERROR;
}

ECode Rfc822Token::GetComment(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mComment;
    return NOERROR;
}

ECode Rfc822Token::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode Rfc822Token::SetAddress(
    /* [in] */ const String& address)
{
    mAddress = address;
    return NOERROR;
}

ECode Rfc822Token::SetComment(
    /* [in] */ const String& comment)
{
    mComment = comment;
    return NOERROR;
}

ECode Rfc822Token::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;

    if (!mName.IsNullOrEmpty()) {
        String qn;
        QuoteNameIfNecessary(mName, &qn);
        sb.Append(qn);
        sb.AppendChar(' ');
    }

    if (!mComment.IsNullOrEmpty()) {
            sb.AppendChar('(');
            String qn;
            QuoteComment(mComment, &qn);
            sb.Append(qn);
            sb.Append(") ");
    }

    if (!mAddress.IsNullOrEmpty()) {
        sb.AppendChar('<');
        sb.Append(mAddress);
        sb.AppendChar('>');
    }

    *str = sb.ToString();
    return NOERROR;
}

ECode Rfc822Token::QuoteNameIfNecessary(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ArrayOf<Char32> > chars = name.GetChars();
    Int32 len = chars->GetLength();
    Char32 c;
    for (Int32 i = 0; i < len; i++) {
        c = (*chars)[i];
        if (! ((c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               (c == ' ') ||
               (c >= '0' && c <= '9'))) {
            StringBuilder sb;
            sb += "\"";
            String qn;
            QuoteName(name, &qn);
            sb += qn;
            sb += "\"";
            *str = sb.ToString();
            return NOERROR;
        }
    }

    *str = name;
    return NOERROR;
}

ECode Rfc822Token::QuoteName(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;

    AutoPtr<ArrayOf<Char32> > chars = name.GetChars();
    Int32 len = chars->GetLength();
    Char32 c;
    for (Int32 i = 0; i < len; i++) {
        c = (*chars)[i];
        if (c == '\\' || c == '"') {
            sb.AppendChar('\\');
        }

        sb.AppendChar(c);
    }

    *str = sb.ToString();
    return NOERROR;
}

ECode Rfc822Token::QuoteComment(
    /* [in] */ const String& comment,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;

    AutoPtr<ArrayOf<Char32> > chars = comment.GetChars();
    Int32 len = chars->GetLength();
    Char32 c;
    for (Int32 i = 0; i < len; i++) {
        c = (*chars)[i];
        if (c == '(' || c == ')' || c == '\\') {
            sb.AppendChar('\\');
        }

        sb.AppendChar(c);
    }

    *str = sb.ToString();
    return NOERROR;
}

ECode Rfc822Token::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 result = 17;
    if (mName.IsNullOrEmpty()) {
        result = 31 * result + mName.GetHashCode();
    }
    if (mAddress.IsNullOrEmpty()) {
        result = 31 * result + mAddress.GetHashCode();
    }
    if (mComment.IsNullOrEmpty()) {
        result = 31 * result + mComment.GetHashCode();
    }
    *hash = result;
    return NOERROR;
}

Boolean Rfc822Token::StringEquals(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    if (a.IsNullOrEmpty()) {
        return (b.IsNullOrEmpty());
    } else {
        return (a.Equals(b));
    }
}

ECode Rfc822Token::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IRfc822Token* rfc822Token = IRfc822Token::Probe(o);
    if(rfc822Token == NULL) {
        return NOERROR;
    }
    IRfc822Token* other = rfc822Token;
    String name, address, comment;

    *result = (StringEquals(mName, (other->GetName(&name), name)) &&
            StringEquals(mAddress, (other->GetAddress(&address), address)) &&
            StringEquals(mComment, (other->GetComment(&comment), comment)));
    return NOERROR;
}

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos
