
#include "FormatBase.h"
#include "CAttributedString.h"
#include "CFieldPosition.h"
#include "CParsePosition.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_ICloneable;
using Elastos::IO::EIID_ISerializable;
using Elastos::Text::IAttributedString;
using Elastos::Text::CAttributedString;
using Elastos::Text::IFieldPosition;
using Elastos::Text::CFieldPosition;
using Elastos::Text::CParsePosition;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_3(FormatBase, Object, IFormat, ISerializable, ICloneable)

FormatBase::FormatBase()
{}

FormatBase::~FormatBase()
{}


ECode FormatBase::CloneImpl(
    /* [in] */ IFormat* format)
{
    assert(format);
    return NOERROR;
}

ECode FormatBase::Format(
    /* [in] */ IInterface* object,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;
    VALIDATE_NOT_NULL(object);

    AutoPtr<IFieldPosition> field;
    FAIL_RETURN(CFieldPosition::New(0, (IFieldPosition**)&field));
    AutoPtr<IStringBuffer> sb = new StringBuffer();
    Format(object, sb, field);
    return ICharSequence::Probe(sb)->ToString(value);
}

ECode FormatBase::FormatToCharacterIterator(
    /* [in] */ IInterface* object,
    /* [out] */ IAttributedCharacterIterator** characterIterator)
{
    VALIDATE_NOT_NULL(characterIterator);

    String value;
    Format(object, &value);
    AutoPtr<IAttributedString> as;
    CAttributedString::New(value, (IAttributedString**)&as);
    as->GetIterator(characterIterator);
    return NOERROR;
}

ECode FormatBase::ParseObject(
    /* [in] */ const String& string,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = NULL;

    AutoPtr<IParsePosition> position;
    CParsePosition::New(0, (IParsePosition**)&position);
    FAIL_RETURN(ParseObject(string, position, object));
    Int32 index;
    position->GetIndex(&index);
    if (index == 0) {
        return E_PARSE_EXCEPTION;
    }
    return NOERROR;
}

ECode FormatBase::UpTo(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [in] */ StringBuffer& buffer,
    /* [in] */ Char32 stop,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;
    VALIDATE_NOT_NULL(position);

    Int32 index, length;
    position->GetIndex(&index);
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    length = charArray->GetLength();
    Boolean lastQuote = FALSE, quote = FALSE;
    Char32 ch;
    while (index < length) {
        ch = (*charArray)[index++];
        if (ch == '\'') {
            if (lastQuote) {
                buffer.AppendChar('\'');
            }
            quote = !quote;
            lastQuote = TRUE;
        }
        else if (ch == stop && !quote) {
            position->SetIndex(index);
            *succeeded = TRUE;
            return NOERROR;
        }
        else {
            lastQuote = FALSE;
            buffer.AppendChar(ch);
        }
    }
    position->SetIndex(index);
    return NOERROR;
}

ECode FormatBase::UpToWithQuotes(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [in] */ StringBuffer& buffer,
    /* [in] */ Char32 stop,
    /* [in] */ Char32 start,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;
    VALIDATE_NOT_NULL(position);

    Int32 index, length, count = 1;
    position->GetIndex(&index);
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    length = charArray->GetLength();
    Boolean quote = FALSE;
    Char32 ch;
    while (index < length) {
        ch = (*charArray)[index++];
        if (ch == '\'') {
            quote = !quote;
        }
        if (!quote) {
            if (ch == stop) {
                count--;
            }
            if (count == 0) {
                position->SetIndex(index);
                *succeeded = TRUE;
                return NOERROR;
            }
            if (ch == start) {
                count++;
            }
        }
        buffer.AppendChar(ch);
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

} // namespace Text
} // namespace Elastos
