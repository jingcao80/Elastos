#include "elastos/droid/utility/CJsonWriter.h"
#include "elastos/core/Math.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IAppendable;
using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL_2(CJsonWriter, Object, IJsonWriter, ICloseable)

CAR_OBJECT_IMPL(CJsonWriter)

CJsonWriter::CJsonWriter()
    : mSeparator(String(":"))
    , mLenient(FALSE)
{
    CArrayList::New((IList**)&mStack);
    mStack->Add(CoreUtils::Convert((Int32)JsonScope_EMPTY_DOCUMENT));
}

CJsonWriter::~CJsonWriter()
{}

ECode CJsonWriter::constructor(
    /* [in] */ IWriter* out)
{
    if (out == NULL) {
        Logger::E("CJsonWriter::constructor", "out == null");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException("out == null");
    }
    mOut = out;
    return NOERROR;
}

ECode CJsonWriter::SetIndent(
    /* [in] */ const String& indent)
{
    if (indent.IsEmpty()) {
        mIndent = String(NULL);
        mSeparator = ":";
    }
    else {
        mIndent = indent;
        mSeparator = ": ";
    }
    return NOERROR;
}

ECode CJsonWriter::SetLenient(
    /* [in] */ Boolean lenient)
{
    mLenient = lenient;
    return NOERROR;
}

ECode CJsonWriter::IsLenient(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mLenient;
    return NOERROR;
}

ECode CJsonWriter::BeginArray()
{
    return Open(JsonScope_EMPTY_ARRAY, String("["));
}

ECode CJsonWriter::EndArray() {
    return Close(JsonScope_EMPTY_ARRAY, JsonScope_NONEMPTY_ARRAY, String("]"));
}

ECode CJsonWriter::BeginObject()
{
    return Open(JsonScope_EMPTY_OBJECT, String("{"));
}

ECode CJsonWriter::EndObject()
{
    return Close(JsonScope_EMPTY_OBJECT, JsonScope_NONEMPTY_OBJECT, String("}"));
}

ECode CJsonWriter::Open(
    /* [in] */ JsonScope empty,
    /* [in] */ const String& openBracket)
{
    BeforeValue(TRUE);
    mStack->Add(CoreUtils::Convert((Int32)empty));
    mOut->Write(openBracket);
    return NOERROR;
}

ECode CJsonWriter::Close(
    /* [in] */ JsonScope empty,
    /* [in] */ JsonScope nonempty,
    /* [in] */ String closeBracket)
{
    JsonScope context = Peek();
    if (context != nonempty && context != empty) {
        Logger::E("CJsonWriter::Close", "Nesting problem: %s", TO_CSTR(mStack));
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Nesting problem: " + mStack);
    }

    Int32 size;
    mStack->GetSize(&size);
    mStack->Remove(size - 1);
    if (context == nonempty) {
        Newline();
    }
    mOut->Write(closeBracket);
    return NOERROR;
}

JsonScope CJsonWriter::Peek()
{
    Int32 size;
    mStack->GetSize(&size);
    AutoPtr<IInterface> obj;
    mStack->Get(size - 1, (IInterface**)&obj);
    Int32 data;
    IInteger32::Probe(obj)->GetValue(&data);
    return (JsonScope)data;
}

void CJsonWriter::ReplaceTop(
    /* [in] */ JsonScope topOfStack)
{
    Int32 size;
    mStack->GetSize(&size);
    mStack->Set(size - 1, CoreUtils::Convert((Int32)topOfStack));
}

ECode CJsonWriter::Name(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("CJsonWriter::Name", "name == null");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException("name == null");
    }
    BeforeName();
    StringImpl(name);
    return NOERROR;
}

ECode CJsonWriter::Value(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        return NullValue();
    }
    BeforeValue(FALSE);
    StringImpl(value);
    return NOERROR;
}

ECode CJsonWriter::NullValue()
{
    BeforeValue(FALSE);
    mOut->Write(String("null"));
    return NOERROR;
}

ECode CJsonWriter::Value(
    /* [in] */ Boolean value)
{
    BeforeValue(FALSE);
    mOut->Write(value ? String("true") : String("false"));
    return NOERROR;
}

ECode CJsonWriter::Value(
    /* [in] */ Double value)
{
    using Elastos::Core::Math;

    if (!mLenient && (Math::IsNaN(value) || Math::IsInfinite(value))) {
        Logger::E("CJsonWriter::Value", "Numeric values must be finite, but was %d", value);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Numeric values must be finite, but was " + value);
    }
    BeforeValue(FALSE);
    IAppendable::Probe(mOut)->Append(CoreUtils::Convert(StringUtils::ToString(value)));
    return NOERROR;
}

ECode CJsonWriter::Value(
    /* [in] */ Int64 value)
{
    BeforeValue(FALSE);
    mOut->Write(StringUtils::ToString(value));
    return NOERROR;
}

ECode CJsonWriter::Value(
    /* [in] */ INumber* value)
{
    if (value == NULL) {
        return NullValue();
    }

    String string;
    IObject::Probe(value)->ToString(&string);
    if (!mLenient &&
            (string.Equals("-Infinity") || string.Equals("Infinity") || string.Equals("NaN"))) {
        Logger::E("CJsonWriter::Value", "Numeric values must be finite, but was %s", TO_CSTR(value));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Numeric values must be finite, but was " + value);
    }
    BeforeValue(FALSE);
    IAppendable::Probe(mOut)->Append(CoreUtils::Convert(string));
    return NOERROR;
}

ECode CJsonWriter::Flush()
{
    IFlushable::Probe(mOut)->Flush();
    return NOERROR;
}

ECode CJsonWriter::Close()
{
    ICloseable::Probe(mOut)->Close();

    if (Peek() != JsonScope_NONEMPTY_DOCUMENT) {
        Logger::E("CJsonWriter::Close", "Incomplete document");
        return E_IO_EXCEPTION;
        // throw new IOException("Incomplete document");
    }
    return NOERROR;
}

ECode CJsonWriter::StringImpl(
    /* [in] */ const String& value)
{
    mOut->Write(String("\""));
    for (Int32 i = 0, length = value.GetLength(); i < length; i++) {
        Char32 c = value.GetChar(i);

        /*
         * From RFC 4627, "All Unicode characters may be placed within the
         * quotation marks except for the characters that must be escaped:
         * quotation mark, reverse solidus, and the control characters
         * (U+0000 through U+001F)."
         *
         * We also escape '\u2028' and '\u2029', which JavaScript interprets
         * as newline characters. This prevents eval() from failing with a
         * syntax error.
         * http://code.google.com/p/google-gson/issues/detail?id=341
         */
        switch (c) {
            case '"':
            case '\\':
                mOut->Write('\\');
                mOut->Write(c);
                break;

            case '\t':
                mOut->Write(String("\\t"));
                break;

            case '\b':
                mOut->Write(String("\\b"));
                break;

            case '\n':
                mOut->Write(String("\\n"));
                break;

            case '\r':
                mOut->Write(String("\\r"));
                break;

            case '\f':
                mOut->Write(String("\\f"));
                break;

            case 0x2028:
            case 0x2029: {
                String str("");
                str.Append(c);
                mOut->Write(str);
                break;
            }

            default: {
                if (c <= 0x1F) {
                    String str("");
                    str.Append(c);
                    mOut->Write(str);
                }
                else {
                    mOut->Write(c);
                }
                break;
            }
        }

    }
    mOut->Write(String("\""));
    return NOERROR;
}

ECode CJsonWriter::Newline()
{
    if (mIndent.IsNull()) {
        return NOERROR;
    }

    mOut->Write(String("\n"));
    Int32 size;
    mStack->GetSize(&size);
    for (Int32 i = 1; i < size; i++) {
        mOut->Write(mIndent);
    }
    return NOERROR;
}

ECode CJsonWriter::BeforeName()
{
    JsonScope context = Peek();
    if (context == JsonScope_NONEMPTY_OBJECT) { // first in object
        mOut->Write(',');
    }
    else if (context != JsonScope_EMPTY_OBJECT) { // not in an object!
        Logger::E("CJsonWriter::BeforeName", "Nesting problem: %s", TO_CSTR(mStack));
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Nesting problem: " + mStack);
    }
    Newline();
    ReplaceTop(JsonScope_DANGLING_NAME);
    return NOERROR;
}

ECode CJsonWriter::BeforeValue(
    /* [in] */ Boolean root)
{
    switch (Peek()) {
        case JsonScope_EMPTY_DOCUMENT: // first in document
            if (!mLenient && !root) {
                Logger::E("CJsonWriter::BeforeValue", "JSON must start with an array or an object.");
                return E_ILLEGAL_STATE_EXCEPTION;
                // throw new IllegalStateException(
                //         "JSON must start with an array or an object.");
            }
            ReplaceTop(JsonScope_NONEMPTY_DOCUMENT);
            break;

        case JsonScope_EMPTY_ARRAY: // first in array
            ReplaceTop(JsonScope_NONEMPTY_ARRAY);
            Newline();
            break;

        case JsonScope_NONEMPTY_ARRAY: // another in array
            IAppendable::Probe(mOut)->AppendChar(',');
            Newline();
            break;

        case JsonScope_DANGLING_NAME: // value for name
            IAppendable::Probe(mOut)->Append(CoreUtils::Convert(mSeparator));
            ReplaceTop(JsonScope_NONEMPTY_OBJECT);
            break;

        case JsonScope_NONEMPTY_DOCUMENT:
            Logger::E("CJsonWriter::BeforeValue", "JSON must have only one top-level value.");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException(
            //         "JSON must have only one top-level value.");

        default:
            Logger::E("CJsonWriter::BeforeValue", "Nesting problem: %s", TO_CSTR(mStack));
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Nesting problem: " + mStack);
    }
    return NOERROR;
}

}
}
}