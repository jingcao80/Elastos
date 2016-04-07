#include "MessageFormat.h"
#include "CDecimalFormat.h"
#include "CSimpleDateFormat.h"
#include "CChoiceFormat.h"
#include "CMessageFormatField.h"
#include "CMessageFormat.h"
#include "CString.h"
#include "CAttributedString.h"
#include "CDecimalFormatSymbols.h"
#include "CDouble.h"
#include "CInteger64.h"
#include "CDecimalFormatSymbols.h"
#include "CFieldPosition.h"
#include "CParsePosition.h"
#include "CLocale.h"
#include "Arrays.h"
#include "EmptyArray.h"
#include "CArrayOf.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::IByte;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Utility::EIID_IDate;
using Elastos::Utility::CLocale;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Text::IAttributedString;
using Elastos::Text::CAttributedString;
using Elastos::Text::IDecimalFormatSymbols;
using Elastos::Text::CDecimalFormatSymbols;
using Elastos::Text::IFieldPosition;
using Elastos::Text::CFieldPosition;
using Elastos::Text::CParsePosition;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Text {

static AutoPtr<IMessageFormatField> InitARGUMENT(const String& name)
{
    AutoPtr<CMessageFormatField> field;
    CMessageFormatField::NewByFriend(name, (CMessageFormatField**)&field);
    return (IMessageFormatField*)field.Get();
}

const AutoPtr<IMessageFormatField> MessageFormat::MessageFormatField::ARGUMENT
    = InitARGUMENT(String("message argument field"));

CAR_INTERFACE_IMPL(MessageFormat::MessageFormatField, FormatBase::Field, IMessageFormatField)

ECode MessageFormat::MessageFormatField::constructor(
    /* [in] */ const String& fieldName)
{
    return Field::constructor(fieldName);
}

CAR_INTERFACE_IMPL(MessageFormat, FormatBase, IMessageFormat)

MessageFormat::~MessageFormat()
{
}

ECode MessageFormat::constructor(
    /* [in] */ const String& tem,
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return ApplyPattern(tem);
}

ECode MessageFormat::constructor(
    /* [in] */ const String& tem)
{
    AutoPtr<ILocale> pLocale = CLocale::GetDefault();
    return constructor(tem, (ILocale*)pLocale);
}

ECode MessageFormat::ApplyPattern(
    /* [in] */ const String& tem)
{
    if (tem.IsNull()) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Char32> > charArray = tem.GetChars();
    Int32 length = charArray->GetLength();
    StringBuffer buffer("");
    AutoPtr<IParsePosition> position;
    CParsePosition::New(0, (IParsePosition**)&position);
    List<String> localStrings;
    Int32 argCount = 0;
    AutoPtr<ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(10);
    Int32 maxArg = -1;
    List< AutoPtr<IFormat> > localFormats;
    Int32 index, arg, offset;
    position->GetIndex(&index);
    Boolean succeeded;
    Char32 ch;

    while (index < length) {
        if (FormatBase::UpTo(tem, position, buffer, '{', &succeeded), succeeded) {
            arg = 0;
            position->GetIndex(&offset);
            if (offset >= length) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            // Get argument number
            while ((ch = (*charArray)[offset++]) != '}' && ch != ',') {
                if (ch < '0' && ch > '9') {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                arg = arg * 10 + (ch - '0');

                if (arg < 0 || offset >= length) {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }

            offset--;
            position->SetIndex(offset);
            AutoPtr<IFormat> f;
            FAIL_RETURN(ParseVariable(tem, position, (IFormat**)&f));
            localFormats.PushBack(f);
            if (argCount >= args->GetLength()) {
                AutoPtr<ArrayOf<Int32> > newArgs = ArrayOf<Int32>::Alloc(args->GetLength() * 2);
                newArgs->Copy(args);
                args = newArgs;
            }
            (*args)[argCount++] = arg;
            if (arg > maxArg) {
                maxArg = arg;
            }
        }

        String outstr;
        buffer.Substring(0, buffer.GetLength(), &outstr);
        localStrings.PushBack(outstr);
        buffer.Reset();
        position->GetIndex(&index);
    }

    mStrings = ArrayOf<String>::Alloc(localStrings.GetSize());
    List<String>::Iterator it = localStrings.Begin();
    for (Int32 i = 0; i < (Int32)(localStrings.GetSize()); ++i, ++it) {
        (*mStrings)[i] = *it;
    }

    mArgumentNumbers = args;
    mFormats = ArrayOf<IFormat* >::Alloc(argCount);
    List< AutoPtr<IFormat> >::Iterator ite = localFormats.Begin();
    for (Int32 i = 0; i < argCount; ++i, ++ite) {
        mFormats->Set(i, *ite);
    }

    mMaxOffset = argCount - 1;
    mMaxArgumentIndex = maxArg;
    return NOERROR;
}

ECode MessageFormat::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    result = FALSE;

    IMessageFormat* mfobj = IMessageFormat::Probe(object);
    if (mfobj == NULL) {
        return NOERROR;
    }

    MessageFormat* format = (MessageFormat*)mfobj;
    if (this == format) {
        return NOERROR;
    }

    if (mMaxOffset != format->mMaxOffset) {
        return NOERROR;
    }
    // Must use a loop since the lengths may be different due
    // to serialization cross-loading
    for (Int32 i = 0; i <= mMaxOffset; i++) {
        if ((*mArgumentNumbers)[i] != (*(format->mArgumentNumbers))[i]) {
            return NOERROR;
        }
    }
    return Object::Equals(mLocale, format->mLocale)
            && Arrays::Equals(mStrings, format->mStrings)
            && Arrays::Equals(mFormats, format->mFormats);
}

ECode MessageFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* object,
    /* [out] */ IAttributedCharacterIterator** characterIterator)
{
    VALIDATE_NOT_NULL(characterIterator);
    *characterIterator = NULL;
    VALIDATE_NOT_NULL(object);

    StringBuffer buffer;
    List<AutoPtr<FieldContainer> > fields;

    // format the message, and find fields
    AutoPtr<IFieldPosition> position;
    CFieldPosition::New(0, (IFieldPosition**)&position);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    arr->Set(0, object);
    FormatImpl(arr, &buffer, position, &fields);

    // create an AttributedString with the formatted buffer
    AutoPtr<IAttributedString> as;
    String outstr;
    buffer.Substring(0, buffer.GetLength(),&outstr);
    CAttributedString::New(outstr, (IAttributedString**)&as);

    // add MessageFormat field attributes and values to the AttributedString
    List<AutoPtr<FieldContainer> >::Iterator fc = fields.Begin();
    for ( ; fc != fields.End(); ++fc) {
        FAIL_RETURN(as->AddAttribute((*fc)->mAttribute, (*fc)->mValue, (*fc)->mStart, (*fc)->mEnd));
    }

    // return the CharacterIterator from AttributedString
    return as->GetIterator(characterIterator);
}

ECode MessageFormat::Format(
    /* [in] */ ArrayOf< IInterface* >* objects,
    /* [in] */ IStringBuffer * buffer,
    /* [in] */ IFieldPosition* field)
{
    return FormatImpl(objects, buffer, field, NULL);
}

ECode MessageFormat::FormatImpl(
    /* [in] */ ArrayOf< IInterface* >* objects,
    /* [in] */ IStringBuffer* inbuffer,
    /* [in] */ IFieldPosition* position,
    /* [in] */ List<AutoPtr<FieldContainer> >* fields)
{
    VALIDATE_NOT_NULL(inbuffer);

    StringBuffer * buffer = (StringBuffer *)inbuffer;
    AutoPtr<IFieldPosition> passedField;
    CFieldPosition::New(0, (IFieldPosition**)&passedField);

    Int32 begin;
    String result;
    for (Int32 i = 0; i <= mMaxOffset; i++) {
        *buffer += (*mStrings)[i];
        begin = buffer->GetLength();
        AutoPtr<IInterface> arg;
        if (objects != NULL && (*mArgumentNumbers)[i] < objects->GetLength()) {
            arg = (*objects)[(*mArgumentNumbers)[i]];
        }
        else {
            buffer->AppendChar('{');
            *buffer += (*mArgumentNumbers)[i];
            buffer->AppendChar('}');
            HandleArgumentField(begin, buffer->GetLength(), (*mArgumentNumbers)[i],
                    position, fields);
            continue;
        }

        AutoPtr<IFormat> format = (*mFormats)[i];
        if (format == NULL || arg == NULL) {
            if (INumber::Probe(arg) != NULL) {
                AutoPtr<INumberFormat> nf;
                NumberFormat::GetInstance((INumberFormat**)&nf);
                format = IFormat::Probe(nf);
            }
            else if (IDate::Probe(arg) != NULL) {
                AutoPtr<IDateFormat> df;
                DateFormat::GetInstance((IDateFormat**)&df);
                format = IFormat::Probe(df);
            }
            else {
                buffer->Append(Object::ToString(arg));
                HandleArgumentField(begin, buffer->GetLength(),
                        (*mArgumentNumbers)[i], position, fields);
                continue;
            }
        }

        if (IChoiceFormat::Probe(format) != NULL) {
            format->Format(arg, &result);
            AutoPtr<IMessageFormat> mf;
            CMessageFormat::New(result, (IMessageFormat**)&mf);
            mf->SetLocale(mLocale);
            mf->Format(objects, buffer , passedField);
            HandleArgumentField(begin, buffer->GetLength(), (*mArgumentNumbers)[i], position, fields);
            HandleFormat(format, arg, begin, fields);
        }
        else {
            format->Format(arg, buffer, passedField);
            HandleArgumentField(begin, buffer->GetLength(), (*mArgumentNumbers)[i], position, fields);
            HandleFormat(format, arg, begin, fields);
        }
    }
    if (mMaxOffset + 1 < mStrings->GetLength()) {
        (*buffer) += (*mStrings)[mMaxOffset + 1];
    }

    return NOERROR;
}

ECode MessageFormat::HandleArgumentField(
    /* [in] */ Int32 begin,
    /* [in] */ Int32 end,
    /* [in] */ Int32 argIndex,
    /* [in] */ IFieldPosition* position,
    /* [in] */ List<AutoPtr<FieldContainer> >* fields)
{
    if (fields != NULL) {
        AutoPtr<FieldContainer> fc = new FieldContainer(begin, end,
            (IAttributedCharacterIteratorAttribute*)MessageFormatField::ARGUMENT.Get(),
            (IInterface*)argIndex);
        fields->PushBack(fc);
    }
    else if (position != NULL) {
        Int32 endIndex;
        position->GetEndIndex(&endIndex);
        AutoPtr<IAttributedCharacterIteratorAttribute> fa;
        position->GetFieldAttribute((IAttributedCharacterIteratorAttribute**)&fa);
        Boolean isflag = FALSE;
        if ((fa != NULL) && IObject::Probe(fa)->Equals(MessageFormatField::ARGUMENT, &isflag), isflag && endIndex == 0) {
            position->SetBeginIndex(begin);
            position->SetEndIndex(end);
        }
    }

    return NOERROR;
}

MessageFormat::FieldContainer::FieldContainer(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
    /* [in] */ IInterface* value)
{
    mStart = start;
    mEnd = end;
    mAttribute = attribute;
    mValue = value;
}

ECode MessageFormat::HandleFormat(
    /* [in] */ IFormat* format,
    /* [in] */ IInterface* arg,
    /* [in] */ Int32 begin,
    /* [in] */ List<AutoPtr<FieldContainer> >* fields)
{
    VALIDATE_NOT_NULL(fields);
    VALIDATE_NOT_NULL(format);

    AutoPtr<IAttributedCharacterIterator> iterator;
    format->FormatToCharacterIterator(arg, (IAttributedCharacterIterator**)&iterator);
    Int32 index, endIndex, start, end;
    Char32 character;
    ICharacterIterator::Probe(iterator)->GetIndex(&index);
    ICharacterIterator::Probe(iterator)->GetEndIndex(&endIndex);
    while (index != endIndex) {
        iterator->GetRunStart(&start);
        iterator->GetRunLimit(&end);

        AutoPtr<IMap> map;
        iterator->GetAttributes((IMap**)&map);
        AutoPtr<ISet> set;
        map->GetKeySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        IAttributedCharacterIteratorAttribute* attribute;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            attribute = IAttributedCharacterIteratorAttribute::Probe(obj);
            AutoPtr<IInterface> value;
           iterator->GetAttribute(attribute, (IInterface**)&value);

           AutoPtr<FieldContainer> fc = new FieldContainer(
                begin + start, begin + end, attribute, value);
           fields->PushBack(fc);
        }
        ICharacterIterator::Probe(iterator)->SetIndex(end, &character);
    }
    return NOERROR;
}

ECode MessageFormat::Format(
    /* [in] */ IInterface* object,
    /* [in] */ IStringBuffer * buffer,
    /* [in] */ IFieldPosition* field)
{
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    arr->Set(0, object);
    return Format(arr, buffer, field);
}

ECode MessageFormat::GetFormats(
    /* [out] */ ArrayOf< IFormat* >** arrayOfInstances)
{
    VALIDATE_NOT_NULL(arrayOfInstances);
    AutoPtr<ArrayOf<IFormat*> > arr = mFormats->Clone();
    *arrayOfInstances = arr;
    REFCOUNT_ADD(*arrayOfInstances);
    return NOERROR;
}

ECode MessageFormat::GetFormatsByArgumentIndex(
    /* [out] */ ArrayOf< IFormat* >** arrayOfInstances)
{
    VALIDATE_NOT_NULL(arrayOfInstances);
    AutoPtr<ArrayOf< IFormat* > > answer = ArrayOf< IFormat* >::Alloc(mMaxArgumentIndex + 1);
    for (Int32 i = 0; i < mMaxOffset + 1; i++) {
        answer->Set((*mArgumentNumbers)[i], (*mFormats)[i]);
    }
    *arrayOfInstances = answer;
    REFCOUNT_ADD(*arrayOfInstances)
    return NOERROR;
}

ECode MessageFormat::SetFormatByArgumentIndex(
    /* [in] */ Int32 argIndex,
    /* [in] */ IFormat* format)
{
    for (Int32 i = 0; i < mMaxOffset + 1; i++) {
        if ((*mArgumentNumbers)[i] == argIndex) {
            mFormats->Set(i, format);
        }
    }
    return NOERROR;
}

ECode MessageFormat::SetFormatsByArgumentIndex(
    /* [in] */ ArrayOf< IFormat* >* formats)
{
    for (Int32 j = 0; j < mFormats->GetLength(); j++) {
        for (Int32 i = 0; i < mMaxOffset + 1; i++) {
            if ((*mArgumentNumbers)[i] == j) {
                mFormats->Set(i, (*formats)[j]);
            }
        }
    }
    return NOERROR;
}

ECode MessageFormat::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode MessageFormat::Parse(
    /* [in] */ const String& string,
    /* [out, callee] */ ArrayOf< IInterface* >** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    AutoPtr<IParsePosition> position;
    CParsePosition::New(0, (IParsePosition**)&position);
    AutoPtr<ArrayOf< IInterface* > > result;
    Parse(string, position, (ArrayOf<IInterface*>**)&result);

    Int32 index;
    position->GetIndex(&index);
    if (index == 0) {
        return E_PARSE_EXCEPTION;
    }

    if (result) {
        *value = result;
        REFCOUNT_ADD(*value)
    }
    return NOERROR;
}

ECode MessageFormat::Parse(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out, callee] */ ArrayOf< IInterface* >** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    if (string.IsNull()) {
        *value = EmptyArray::OBJECT;
        REFCOUNT_ADD(*value)
        return NOERROR;
    }

    AutoPtr<IParsePosition> internalPos;
    CParsePosition::New(0, (IParsePosition**)&internalPos);
    Int32 offset, next, errorIndex;
    position->GetIndex(&offset);
    AutoPtr<ArrayOf< IInterface* > > result = ArrayOf< IInterface* >::Alloc(mMaxArgumentIndex + 1);

    String sub, str, target;
    for (Int32 i = 0; i <= mMaxOffset; i++) {
        sub = (*mStrings)[i];
        if (!string.RegionMatches(offset, sub, 0, (Int32)(sub.GetLength()))) {
            position->SetErrorIndex(offset);
            *value = NULL;
            return NOERROR;
        }

        offset += (Int32)(sub.GetLength());
        AutoPtr<IInterface> parse;
        AutoPtr<IFormat> format = (*mFormats)[i];

        if (format == NULL) {
            if (i + 1 < mStrings->GetLength()) {
                next = string.IndexOf((*mStrings)[i + 1], offset);
                if (next == -1) {
                    position->SetErrorIndex(offset);
                    *value = NULL;
                    return NOERROR;
                }

                target = string.Substring(offset, next);
                offset = next;
            }
            else {
                target = string.Substring(offset);
                offset = (Int32)(string.GetLength());
            }
            AutoPtr<ICharSequence> cs;
            CString::New(target, (ICharSequence**)&cs);
            parse = ICharSequence::Probe(cs);
        }
        else {
            internalPos->SetIndex(offset);
            format->ParseObject(string, internalPos, (IInterface**)&parse);
            internalPos->GetErrorIndex(&errorIndex);
            if (errorIndex != -1) {
                position->SetErrorIndex(offset);
                *value = NULL;
                return NOERROR;
            }
            internalPos->GetIndex(&offset);
        }

        result->Set((*mArgumentNumbers)[i], parse);
    }

    if (mMaxOffset + 1 < mStrings->GetLength()) {
        sub = (*mStrings)[mMaxOffset + 1];
        if (!string.RegionMatches(offset, sub, 0, (Int32)(sub.GetLength()))) {
            position->SetErrorIndex(offset);
            *value = NULL;
            return NOERROR;
        }
        offset += (Int32)(sub.GetLength());
    }
    position->SetIndex(offset);

    *value = result;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode MessageFormat::ParseObject(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<IInterface*> > objects;
    FAIL_RETURN(Parse(string, position, (ArrayOf<IInterface*>**)&objects));

    if (objects != NULL) {
        AutoPtr<IArrayOf> bc;
        CArrayOf::New(EIID_IInterface, objects->GetLength(), (IArrayOf**)&bc);
        for (Int32 i = 0; i < objects->GetLength(); ++i) {
            bc->Set(i, (*objects)[i]);
        }

        *result = bc->Probe(EIID_IInterface);
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode MessageFormat::Match(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [in] */ Boolean last,
    /* [in] */ ArrayOf<String>* tokens,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;
    VALIDATE_NOT_NULL(position);
    VALIDATE_NOT_NULL(tokens);

    if (string.IsNull()) return NOERROR;

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength();
    Int32 offset;
    position->GetIndex(&offset);
    Int32 token = -1;
    while (offset < length && Character::IsWhitespace((*charArray)[offset])) {
        offset++;
    }

    for (Int32 i = tokens->GetLength() - 1; i >= 0; --i) {
        if (string.RegionMatchesIgnoreCase(offset, (*tokens)[i], 0, (*tokens)[i].GetLength())) {
            token = i;
            break;
        }
    }

    if (token == -1) {
        return NOERROR;
    }
    offset += (*tokens)[token].GetLength();
    while (offset < length && Character::IsWhitespace((*charArray)[offset])) {
        offset++;
    }
    Char32 ch;
    if (offset < length
            && ((ch = (*charArray)[offset]) == '}' || (!last && ch == ','))) {
        position->SetIndex(offset + 1);
        *value = token;
        return NOERROR;
    }

    return NOERROR;
}

ECode MessageFormat::ParseVariable(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ IFormat** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength();
    Int32 offset;
    position->GetIndex(&offset);
    Char32 ch;
    if (offset >= length || ((ch = (*charArray)[offset++]) != '}' && ch != ',')) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    position->SetIndex(offset);
    if (ch == '}') {
        return NOERROR;
    }

    Int32 type;
    AutoPtr<ArrayOf<String> > tokens1 = ArrayOf<String>::Alloc(4);
    AutoPtr<ArrayOf<String> > tokens2 = ArrayOf<String>::Alloc(4);
    (*tokens1)[0] = String("time");
    (*tokens1)[1] = String("date");
    (*tokens1)[2] = String("number");
    (*tokens1)[3] = String("choice");

    Match(string, position, FALSE, tokens1, &type);
    if (type == -1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuffer buffer("");
    Int32 index;
    position->GetIndex(&index);
    ch = (*charArray)[index - 1];

    AutoPtr<IDateFormat> dv;
    AutoPtr<INumberFormat> nv;
    switch (type) {
    case 0: // time
    case 1: // date
        if (ch == '}') {
            if (type == 1) {
                DateFormat::GetDateInstance(
                    IDateFormat::DEFAULT, mLocale, (IDateFormat**)&dv);
            }
            else {
                DateFormat::GetTimeInstance(
                    IDateFormat::DEFAULT, mLocale, (IDateFormat**)&dv);
            }

            *value = IFormat::Probe(dv);
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
        (*tokens2)[0] = String("full");
        (*tokens2)[1] = String("long");
        (*tokens2)[2] = String("medium");
        (*tokens2)[3] = String("short");

        Int32 dateStyle;
        Match(string, position, TRUE, tokens2, &dateStyle);
        if (dateStyle == -1) {
            Boolean succeeded;
            FormatBase::UpToWithQuotes(string, position, buffer, '}', '{', &succeeded);
            AutoPtr<ISimpleDateFormat> sdf;
            String outstr;
            buffer.Substring(0, buffer.GetLength(),&outstr);
            CSimpleDateFormat::New(outstr, mLocale.Get(), (ISimpleDateFormat**)&sdf);
            *value = IFormat::Probe(sdf);
            REFCOUNT_ADD(*value);
            return NOERROR;
        }

        switch (dateStyle) {
            case 0:
                dateStyle = IDateFormat::FULL;
                break;
            case 1:
                dateStyle = IDateFormat::LONG;
                break;
            case 2:
                dateStyle = IDateFormat::MEDIUM;
                break;
            case 3:
                dateStyle = IDateFormat::SHORT;
                break;
        }

        if (type == 1) {
            DateFormat::GetDateInstance(dateStyle, mLocale.Get(), (IDateFormat**)&dv);
        }
        else {
            DateFormat::GetTimeInstance(dateStyle, mLocale.Get(), (IDateFormat**)&dv);
        }
        *value = IFormat::Probe(dv);
        REFCOUNT_ADD(*value);
        return NOERROR;

    case 2: // number
        if (ch == '}') {
            NumberFormat::GetInstance(mLocale, (INumberFormat**)&nv);
            *value = IFormat::Probe(nv);
            REFCOUNT_ADD(*value);
            return NOERROR;
        }
        Int32 numberStyle;
        AutoPtr<ArrayOf<String> > tokens3 = ArrayOf<String>::Alloc(3);
        (*tokens3)[0] = String("currency");
        (*tokens3)[1] = String("percent");
        (*tokens3)[2] = String("integer");
        Match(string, position, TRUE, tokens3, &numberStyle);
        if (numberStyle == -1) {
            Boolean succeeded;
            FAIL_RETURN(FormatBase::UpToWithQuotes(string, position, buffer, '}', '{', &succeeded));
            AutoPtr<IDecimalFormatSymbols> dfs;
            CDecimalFormatSymbols::New(mLocale, (IDecimalFormatSymbols**)&dfs);
            AutoPtr<IDecimalFormat> df;
            String outstr;
            buffer.Substring(0, buffer.GetLength(),&outstr);
            CDecimalFormat::New(outstr, dfs, (IDecimalFormat**)&df);
            *value = IFormat::Probe(df);
            REFCOUNT_ADD(*value);
            return NOERROR;
        }

        switch (numberStyle) {
            case 0: // currency
                NumberFormat::GetCurrencyInstance(mLocale, (INumberFormat**)&nv);
                break;
            case 1: // percent
                NumberFormat::GetPercentInstance(mLocale, (INumberFormat**)&nv);
                break;
            default:
                NumberFormat::GetIntegerInstance(mLocale, (INumberFormat**)&nv);
                break;
        }
        *value = IFormat::Probe(nv);
        REFCOUNT_ADD(*value);
        return NOERROR;
    }

    Boolean succeeded;
    FormatBase::UpToWithQuotes(string, position, buffer, '}', '{', &succeeded);
    AutoPtr<IChoiceFormat> cf;
    String outstr;
    buffer.Substring(0, buffer.GetLength(), &outstr);
    CChoiceFormat::New(outstr, (IChoiceFormat**)&cf);
    *value = IFormat::Probe(cf);
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode MessageFormat::SetFormat(
    /* [in] */ Int32 offset,
    /* [in] */ IFormat* format)
{
    if (offset < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFormats->Set(offset, format);
    return NOERROR;
}

ECode MessageFormat::SetFormats(
    /* [in] */ ArrayOf< IFormat* >* formats)
{
    Int32 min = mFormats->GetLength();
    if (formats->GetLength() < min) {
        min = formats->GetLength();
    }
    for (Int32 i = 0; i < min; i++) {
        mFormats->Set(i, (*formats)[i]);
    }
    return NOERROR;
}

ECode MessageFormat::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    for (Int32 i = 0; i <= mMaxOffset; i++) {
        AutoPtr<IFormat> format = (*mFormats)[i];
        // java specification undefined for null argument, change into
        // a more tolerant implementation
        if (IDecimalFormat::Probe(format) != NULL) {
            String pattern;
            IDecimalFormat* df = IDecimalFormat::Probe(format);
            assert(df != NULL);
            df->ToPattern(&pattern);
            AutoPtr<IDecimalFormatSymbols> dfs;
            CDecimalFormatSymbols::New(mLocale, (IDecimalFormatSymbols**)&dfs);
            AutoPtr<IDecimalFormat> dff;
            CDecimalFormat::New(pattern, dfs, (IDecimalFormat**)&dff);
            mFormats->Set(i, IFormat::Probe(dff));
        }
        else if (ISimpleDateFormat::Probe(format) != NULL) {
            String pattern;
            ISimpleDateFormat* sdf = ISimpleDateFormat::Probe(format);
            sdf->ToPattern(&pattern);
            AutoPtr<ISimpleDateFormat> sdff;
            CSimpleDateFormat::New(pattern, mLocale, (ISimpleDateFormat**)&sdff);
            mFormats->Set(i, IFormat::Probe(sdff));
        }
    }
    return NOERROR;
}

ECode MessageFormat::DecodeDecimalFormat(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ IFormat* format,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);
    VALIDATE_NOT_NULL(buffer);
    VALIDATE_NOT_NULL(format);

    *buffer += ",number";

    INumberFormat* nff = INumberFormat::Probe(format);
    AutoPtr<INumberFormat> nfn, nfi, nfc, nfp;
    NumberFormat::GetNumberInstance(mLocale, (INumberFormat**)&nfn);
    NumberFormat::GetIntegerInstance(mLocale, (INumberFormat**)&nfi);
    NumberFormat::GetCurrencyInstance(mLocale, (INumberFormat**)&nfc);
    NumberFormat::GetPercentInstance(mLocale, (INumberFormat**)&nfp);

    Boolean result = FALSE;
    if (IObject::Probe(nff)->Equals(nfn, &result), result) {
        // Empty block
    } else if (IObject::Probe(nff)->Equals(nfi, &result), result) {
        buffer->Append(",integer");
    } else if (IObject::Probe(nff)->Equals(nfc, &result), result) {
        buffer->Append(",currency");
    } else if (IObject::Probe(nff)->Equals(nfp, &result), result) {
        buffer->Append(",percent");
    } else {
        buffer->AppendChar(',');
        IDecimalFormat* nfff = IDecimalFormat::Probe(format);
        nfff->ToPattern(value);
    }
    return NOERROR;
}

ECode MessageFormat::DecodeSimpleDateFormat(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ IFormat* format,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);
    VALIDATE_NOT_NULL(format);
    VALIDATE_NOT_NULL(buffer);

    IDateFormat* dateFormat = IDateFormat::Probe(format);

    AutoPtr<IDateFormat> dft, dft2, dft3, dft4, dfd, dfd2, dfd3, dfd4;
    DateFormat::GetTimeInstance(IDateFormat::DEFAULT, mLocale, (IDateFormat**)&dft);
    DateFormat::GetDateInstance(IDateFormat::DEFAULT, mLocale, (IDateFormat**)&dfd);
    DateFormat::GetTimeInstance(IDateFormat::SHORT, mLocale, (IDateFormat**)&dft2);
    DateFormat::GetDateInstance(IDateFormat::SHORT, mLocale, (IDateFormat**)&dfd2);
    DateFormat::GetTimeInstance(IDateFormat::LONG, mLocale, (IDateFormat**)&dft3);
    DateFormat::GetDateInstance(IDateFormat::LONG, mLocale, (IDateFormat**)&dfd3);
    DateFormat::GetTimeInstance(IDateFormat::FULL, mLocale, (IDateFormat**)&dft4);
    DateFormat::GetDateInstance(IDateFormat::FULL, mLocale, (IDateFormat**)&dfd4);

    Boolean result = FALSE;
    if (IObject::Probe(dateFormat)->Equals(dft, &result), result) {
        *buffer += ",time";
    }
    else if (IObject::Probe(dateFormat)->Equals(dfd, &result), result) {
        *buffer += ",date";
    }
    else if (IObject::Probe(dateFormat)->Equals(dft2, &result), result) {
        *buffer += ",time,short";
    }
    else if (IObject::Probe(dateFormat)->Equals(dfd2, &result), result) {
        *buffer += ",date,short";
    }
    else if (IObject::Probe(dateFormat)->Equals(dft3, &result), result) {
        *buffer += ",time,long";
    }
    else if (IObject::Probe(dateFormat)->Equals(dfd3, &result), result) {
        *buffer += ",date,long";
    }
    else if (IObject::Probe(dateFormat)->Equals(dft4, &result), result) {
        *buffer += ",time,full";
    }
    else if (IObject::Probe(dateFormat)->Equals(dfd4, &result), result) {
        *buffer += ",date,full";
    }
    else {
        *buffer += ",date,";
        ISimpleDateFormat* sdf = ISimpleDateFormat::Probe(format);
        sdf->ToPattern(value);
    }

    return NOERROR;
}

ECode MessageFormat::ToPattern(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    StringBuffer buffer;
    String pattern;
    Int32 index = 0, count = 0, length;
    Char32 ch;
    Boolean quote;

    for (Int32 i = 0; i <= mMaxOffset; i++) {
        AppendQuoted(&buffer, (*mStrings)[i]);
        buffer += "{";
        buffer += (*mArgumentNumbers)[i];
        AutoPtr<IFormat> format = (*mFormats)[i];
        if (IChoiceFormat::Probe(format) != NULL) {
            buffer += ",choice,";
            IChoiceFormat* cf = IChoiceFormat::Probe(format);
            cf->ToPattern(&pattern);
        }
        else if (IDecimalFormat::Probe(format) != NULL) {
            DecodeDecimalFormat(&buffer, format, &pattern);
        }
        else if (ISimpleDateFormat::Probe(format) != NULL) {
            DecodeSimpleDateFormat(&buffer, format, &pattern);
        }
        else if (format != NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (!pattern.IsNull()) {
            quote = FALSE;
            index = 0, count = 0;
            AutoPtr<ArrayOf<Char32> > charArray = pattern.GetChars();
            length = charArray->GetLength();
            while (index < length) {
                ch = (*charArray)[index++];
                if (ch == '\'') {
                    quote = !quote;
                }
                if (!quote) {
                    if (ch == '{') {
                        count++;
                    }
                    if (ch == '}') {
                        if (count > 0) {
                            count--;
                        } else {
                            buffer += "'}";
                            ch = '\'';
                        }
                    }
                }
                buffer += ch;
            }
        }
        buffer += "}";
    }
    if (mMaxOffset + 1 < mStrings->GetLength()) {
        AppendQuoted(&buffer, (*mStrings)[mMaxOffset + 1]);
    }
    buffer.Substring(0, buffer.GetLength(),value);
    return NOERROR;
}

ECode MessageFormat::AppendQuoted(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ const String& string)
{
    VALIDATE_NOT_NULL(buffer);

    if (string.IsNull()) return NOERROR;

    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 length = charArray->GetLength();
    Char32 ch;
    for (Int32 i = 0; i < length; i++) {
        ch = (*charArray)[i];
        if (ch == '{' || ch == '}') {
            buffer->AppendChar('\'');
            buffer->AppendChar(ch);
            buffer->AppendChar('\'');
        }
        else {
            buffer->AppendChar(ch);
        }
    }
    return NOERROR;
}

ECode MessageFormat::GetMaxOffset(
    /* [out] */ Int32* maxOffset)
{
    VALIDATE_NOT_NULL(maxOffset)
    *maxOffset = mMaxOffset;
    return NOERROR;
}

ECode MessageFormat::GetArgumentNumbers(
    /* [out, callee] */ ArrayOf<Int32>** argumentNumbers)
{
    VALIDATE_NOT_NULL(argumentNumbers)
    *argumentNumbers = mArgumentNumbers;
    REFCOUNT_ADD(*argumentNumbers);
    return NOERROR;
}

ECode MessageFormat::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings)
    *strings = mStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode MessageFormat::Clone(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    AutoPtr<IMessageFormat> mf;
    CMessageFormat::New(String(NULL), (IMessageFormat**)&mf);
    CloneImpl(mf);

    *outface = mf;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode MessageFormat::CloneImpl(
    /* [in] */ IMessageFormat* clone)
{
    VALIDATE_NOT_NULL(clone)

    FormatBase::CloneImpl(IFormat::Probe(clone));

    MessageFormat* other = (MessageFormat*)clone;
    other->mLocale = mLocale;
    other->mStrings = mStrings;
    other->mArgumentNumbers = mArgumentNumbers;
    other->mMaxOffset = mMaxOffset;
    other->mMaxArgumentIndex = mMaxArgumentIndex;

    AutoPtr<ArrayOf<IFormat*> > array = ArrayOf<IFormat*>::Alloc(mFormats->GetLength());
    for (Int32 i = 0; i < mFormats->GetLength(); ++i) {
        IFormat* f = (*mFormats)[i];
        if (f != NULL) {
            AutoPtr<IInterface> copy;
            ICloneable::Probe(f)->Clone((IInterface**)&copy);
            array->Set(i, IFormat::Probe(copy));
        }
    }
    other->mFormats = array;
    return NOERROR;
}

ECode MessageFormat::GetHashCode(
    /* [out] */ Int32* value)
{
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
