
#include "JSONStringer.h"
#include "JSON.h"
#include "JSONArray.h"
#include "JSONObject.h"
#include "CoreUtils.h"
#include "Math.h"
#include "utility/CArrayList.h"
#include "utility/Arrays.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::INumber;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Org::Json::JSONStringerScope;

namespace Org {
namespace Json {

CAR_INTERFACE_IMPL(JSONStringer, Object, IJSONStringer);

JSONStringer::JSONStringer()
{
    mOut = new StringBuilder();
    CArrayList::New((IList**)&mStack);
}

JSONStringer::~JSONStringer()
{}


ECode JSONStringer::constructor()
{
    return NOERROR;
}

ECode JSONStringer::constructor(
    /* [in] */ Int32 indentSpaces)
{
    AutoPtr< ArrayOf<Char32> > indentChars = ArrayOf<Char32>::Alloc(indentSpaces);
    Char32 c = ' ';
    Arrays::Fill(indentChars, c);
    mIndent = String(*indentChars);

    return NOERROR;
}

ECode JSONStringer::Array()
{
    return Open(JSONStringerScope_EMPTY_ARRAY, String("["));
}

ECode JSONStringer::EndArray()
{
    return Close(JSONStringerScope_EMPTY_ARRAY, JSONStringerScope_NONEMPTY_ARRAY, String("]"));
}

ECode JSONStringer::Object()
{
    return Open(JSONStringerScope_EMPTY_OBJECT, String("{"));
}

ECode JSONStringer::EndObject()
{
    return Close(JSONStringerScope_EMPTY_OBJECT, JSONStringerScope_NONEMPTY_OBJECT, String("}"));
}

ECode JSONStringer::Open(
    /* [in] */ JSONStringerScope empty,
    /* [in] */ const String& openBracket)
{
    Boolean res;
    mStack->IsEmpty(&res);
    Int32 length;
    mOut->GetLength(&length);
    if (res && length > 0) {
        Logger::E("JSONStringer", "Nesting problem: multiple top-level roots");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem: multiple top-level roots");
    }
    FAIL_RETURN(BeforeValue());
    mStack->Add(CoreUtils::Convert((Int32)empty));
    mOut->Append(openBracket);
    return NOERROR;
}

ECode JSONStringer::Close(
    /* [in] */ JSONStringerScope empty,
    /* [in] */ JSONStringerScope nonempty,
    /* [in] */ const String& closeBracket)
{
    JSONStringerScope context;
    FAIL_RETURN(Peek(&context));
    if (context != nonempty && context != empty) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    Int32 size;
    mStack->GetSize(&size);
    mStack->Remove(size - 1);
    if (context == nonempty) {
        Newline();
    }
    mOut->Append(closeBracket);
    return NOERROR;
}

ECode JSONStringer::Peek(
    /* [out] */ JSONStringerScope* scope)
{
    VALIDATE_NOT_NULL(scope);

    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    Int32 size;
    mStack->GetSize(&size);
    AutoPtr<IInterface> obj;
    mStack->Get(size - 1, (IInterface**)&obj);
    Int32 data;
    IInteger32::Probe(obj)->GetValue(&data);
    *scope = (JSONStringerScope)data;
    return NOERROR;
}

ECode JSONStringer::ReplaceTop(
    /* [in] */ JSONStringerScope topOfStack)
{
    Int32 size;
    mStack->GetSize(&size);
    AutoPtr<IInteger32> iObj = CoreUtils::Convert((Int32)topOfStack);
    mStack->Set(size - 1, iObj);
    return NOERROR;
}

ECode JSONStringer::Value(
    /* [in] */ IInterface* value)
{
    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    if (IJSONArray::Probe(value) != NULL) {
        ((JSONArray*)IJSONArray::Probe(value))->WriteTo((IJSONStringer*)this);
        return NOERROR;

    }
    else if (IJSONObject::Probe(value) != NULL) {
        ((JSONObject*)IJSONObject::Probe(value))->WriteTo((IJSONStringer*)this);
        return NOERROR;
    }

    FAIL_RETURN(BeforeValue());

    if (value == NULL
            || (IBoolean::Probe(value) != NULL)
            || IInterface::Probe(value) == IInterface::Probe(JSONObject::Object_NULL)) {
        mOut->Append(value);

    }
    else if (INumber::Probe(value) != NULL) {
        String str;
        JSONObject::NumberToString(INumber::Probe(value), &str);
        mOut->Append(str);

    }
    else {
        String str;
        IObject::Probe(value)->ToString(&str);
        AppendString(str);
    }

    return NOERROR;
}

ECode JSONStringer::Value(
    /* [in] */ Boolean value)
{
    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    FAIL_RETURN(BeforeValue());
    mOut->Append(value);
    return NOERROR;
}

ECode JSONStringer::Value(
    /* [in] */ Double value)
{
    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    FAIL_RETURN(BeforeValue());
    String str;
    JSONObject::NumberToString(INumber::Probe(CoreUtils::Convert(value)), &str);
    mOut->Append(str);
    return NOERROR;
}

ECode JSONStringer::Value(
    /* [in] */ Int64 value)
{
    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    FAIL_RETURN(BeforeValue());
    mOut->Append(value);
    return NOERROR;
}

void JSONStringer::AppendString(
    /* [in] */ const String& value)
{
    mOut->Append("\"");
    for (Int32 i = 0, length = value.GetLength(); i < length; i++) {
        Char32 c = value.GetChar(i);

        /*
         * From RFC 4627, "All Unicode characters may be placed within the
         * quotation marks except for the characters that must be escaped:
         * quotation mark, reverse solidus, and the control characters
         * (U+0000 through U+001F)."
         */
        switch (c) {
            case '"':
            case '\\':
            case '/':
                mOut->AppendChar('\\');
                mOut->AppendChar(c);
                break;

            case '\t':
                mOut->Append("\\t");
                break;

            case '\b':
                mOut->Append("\\b");
                break;

            case '\n':
                mOut->Append("\\n");
                break;

            case '\r':
                mOut->Append("\\r");
                break;

            case '\f':
                mOut->Append("\\f");
                break;

            default:
                if (c <= 0x1F) {
                    String str("");
                    str.AppendFormat("%04x", (Int32)c);
                    mOut->Append(str);
                }
                else {
                    mOut->AppendChar(c);
                }
                break;
        }

    }
    mOut->Append("\"");
}

void JSONStringer::Newline()
{
    if (mIndent.IsNull()) {
        return;
    }

    mOut->Append("\n");
    Int32 size;
    mStack->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        mOut->Append(mIndent);
    }
}

ECode JSONStringer::Key(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("JSONStringer", "Names must be non-null");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Names must be non-null");
    }
    FAIL_RETURN(BeforeKey());
    AppendString(name);
    return NOERROR;
}

ECode JSONStringer::BeforeKey()
{
    JSONStringerScope context;
    FAIL_RETURN(Peek(&context));
    if (context == JSONStringerScope_NONEMPTY_OBJECT) { // first in object
        mOut->AppendChar(',');
    }
    else if (context != JSONStringerScope_EMPTY_OBJECT) { // not in an object!
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }
    Newline();
    ReplaceTop(JSONStringerScope_DANGLING_KEY);
    return NOERROR;
}

ECode JSONStringer::BeforeValue()
{
    Boolean res;
    if (mStack->IsEmpty(&res), res) {
        return NOERROR;
    }

    JSONStringerScope context;
    FAIL_RETURN(Peek(&context));
    if (context == JSONStringerScope_EMPTY_ARRAY) { // first in array
        ReplaceTop(JSONStringerScope_NONEMPTY_ARRAY);
        Newline();
    }
    else if (context == JSONStringerScope_NONEMPTY_ARRAY) { // another in array
        mOut->AppendChar(',');
        Newline();
    }
    else if (context == JSONStringerScope_DANGLING_KEY) { // value for key
        mOut->Append(mIndent.IsNull() ? ":" : ": ");
        ReplaceTop(JSONStringerScope_NONEMPTY_OBJECT);
    }
    else if (context != JSONStringerScope_NULL) {
        Logger::E("JSONStringer", "Nesting problem");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Nesting problem");
    }

    return NOERROR;
}

ECode JSONStringer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    Int32 length;
    if (mOut->GetLength(&length), length != 0) {
        return mOut->ToString(str);
    }
    return NOERROR;
}

} //namespace Json
} //namespace Org
