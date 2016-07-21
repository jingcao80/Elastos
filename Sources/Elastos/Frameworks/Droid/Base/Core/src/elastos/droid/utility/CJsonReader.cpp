#include "elastos/droid/utility/CJsonReader.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

const String CJsonReader::sTRUE("true");
const String CJsonReader::sFALSE("false");

CAR_INTERFACE_IMPL_2(CJsonReader, Object, IJsonReader, ICloseable)

CAR_OBJECT_IMPL(CJsonReader)

CJsonReader::CJsonReader()
    : mLenient(FALSE)
    , mPos(0)
    , mLimit(0)
    , mBufferStartLine(1)
    , mBufferStartColumn(1)
    , mToken(JsonToken_NULL)
    , mValuePos(0)
    , mValueLength(0)
    , mSkipping(FALSE)
{
    mStringPool = new StringPool();
    mBuffer = ArrayOf<Char32>::Alloc(1024);
    CArrayList::New((IList**)&mStack);
    Push(JsonScope_EMPTY_DOCUMENT);
}

CJsonReader::~CJsonReader()
{
}

ECode CJsonReader::constructor(
    /* [in] */ IReader* in)
{
    if (in == NULL) {
        Logger::E("CJsonReader::constructor", "in == null");
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException("in == null");
    }
    mIn = in;
    return NOERROR;
}

ECode CJsonReader::SetLenient(
    /* [in] */ Boolean lenient)
{
    mLenient = lenient;
    return NOERROR;
}

ECode CJsonReader::IsLenient(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mLenient;
    return NOERROR;
}

ECode CJsonReader::BeginArray()
{
    return Expect(JsonToken_BEGIN_ARRAY);
}

ECode CJsonReader::EndArray()
{
    return Expect(JsonToken_END_ARRAY);
}

ECode CJsonReader::BeginObject()
{
    return Expect(JsonToken_BEGIN_OBJECT);
}

ECode CJsonReader::EndObject()
{
    return Expect(JsonToken_END_OBJECT);
}

ECode CJsonReader::Expect(
    /* [in] */ JsonToken expected)
{
    JsonToken jt, jt1;
    Peek(&jt);
    if (mToken != expected) {
        Logger::E("CJsonReader::Expect", "Expected %d but was %d", expected, jt);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected " + expected + " but was " + Peek());
    }
    Advance(&jt1);
    return NOERROR;
}

ECode CJsonReader::HasNext(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    JsonToken jt;
    FAIL_RETURN(Peek(&jt));
    *res = mToken != JsonToken_END_OBJECT && mToken != JsonToken_END_ARRAY;
    return NOERROR;
}

ECode CJsonReader::Peek(
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    if (mToken != JsonToken_NULL) {
        *result = mToken;
        return NOERROR;
    }

    ECode ec = NOERROR;

    switch (PeekStack()) {
        case JsonScope_EMPTY_DOCUMENT:
            ReplaceTop(JsonScope_NONEMPTY_DOCUMENT);
            JsonToken firstToken;
            ec = NextValue(&firstToken);
            if (FAILED(ec) || (!mLenient && mToken != JsonToken_BEGIN_ARRAY && mToken != JsonToken_BEGIN_OBJECT)) {
                Logger::E("CJsonReader::Peek", "Expected JSON document to start with '[' or '{' but was ", mToken);
                return E_IO_EXCEPTION;
                // throw new IOException(
                //         "Expected JSON document to start with '[' or '{' but was " + mToken);
            }
            *result = firstToken;
            return NOERROR;
        case JsonScope_EMPTY_ARRAY:
            return NextInArray(TRUE, result);
        case JsonScope_NONEMPTY_ARRAY:
            return NextInArray(FALSE, result);
        case JsonScope_EMPTY_OBJECT:
            return NextInObject(TRUE, result);
        case JsonScope_DANGLING_NAME:
            return ObjectValue(result);
        case JsonScope_NONEMPTY_OBJECT:
            return NextInObject(FALSE, result);
        case JsonScope_NONEMPTY_DOCUMENT:
            // try {
                JsonToken token;
                ec = NextValue(&token);
                if (ec == (ECode)E_EOF_EXCEPTION) {
                    *result = token = JsonToken_END_DOCUMENT; // TODO: avoid throwing here?
                    return NOERROR;
                }
                else if (FAILED(ec)) {
                    *result = token;
                    return ec;
                }
                else {
                    if (mLenient) {
                        *result = token;
                        return NOERROR;
                    }
                    return SyntaxError(String("Expected EOF"));
                    // throw SyntaxError("Expected EOF");
                }
            // } catch (EOFException e) {
            //     return token = JsonToken_END_DOCUMENT; // TODO: avoid throwing here?
            // }
        case JsonScope_CLOSED:
            Logger::E("CJsonReader::Peek", "JsonReader is closed");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("JsonReader is closed");
        default:
            Logger::E("CJsonReader::Peek", "throw new AssertionError()");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new AssertionError();
    }
    return NOERROR;
}

ECode CJsonReader::Advance(
    /* [out] */ JsonToken* res)
{
    VALIDATE_NOT_NULL(res)
    JsonToken jt;
    Peek(&jt);

    JsonToken result = mToken;
    mToken = JsonToken_NULL;
    mValue = String(NULL);
    mName = String(NULL);
    *res = result;
    return NOERROR;
}

ECode CJsonReader::NextName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    JsonToken jt;
    Peek(&jt);

    if (mToken != JsonToken_NAME) {
        Logger::E("CJsonReader::NextName", "Expected a name but was %d", jt);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected a name but was " + Peek());
    }
    String result = mName;
    JsonToken jt1;
    Advance(&jt1);
    *str = result;
    return NOERROR;
}

ECode CJsonReader::NextString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_STRING && mToken != JsonToken_NUMBER) {
        Logger::E("CJsonReader::NextString", "Expected a string but was %d", jt);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected a string but was " + Peek());
    }

    String result = mValue;
    JsonToken jt1;
    Advance(&jt1);
    *str = result;
    return NOERROR;
}

ECode CJsonReader::NextBoolean(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_BOOLEAN) {
        Logger::E("CJsonReader::NextBoolean", "Expected a boolean but was %d", mToken);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected a boolean but was " + mToken);
    }

    Boolean result = (mValue == sTRUE);
    JsonToken jt1;
    Advance(&jt1);
    *res = result;
    return NOERROR;
}

ECode CJsonReader::NextNull()
{
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_NULL) {
        Logger::E("CJsonReader::NextNull", "Expected null but was %d", mToken);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected null but was " + mToken);
    }

    JsonToken jt1;
    Advance(&jt1);
    return NOERROR;
}

ECode CJsonReader::NextDouble(
    /* [out] */ Double* data)
{
    VALIDATE_NOT_NULL(data)
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_STRING && mToken != JsonToken_NUMBER) {
        Logger::E("CJsonReader::NextDouble", "Expected a double but was %d", mToken);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected a double but was " + mToken);
    }

    Double result = StringUtils::ParseDouble(mValue);
    JsonToken jt1;
    Advance(&jt1);
    *data = result;
    return NOERROR;
}

ECode CJsonReader::NextLong(
    /* [out] */ Int64* data)
{
    VALIDATE_NOT_NULL(data)
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_STRING && mToken != JsonToken_NUMBER) {
        Logger::E("CJsonReader::NextLong", "Expected a long but was %d", mToken);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected a long but was " + mToken);
    }

    Int64 result;
    // try {
        result = StringUtils::ParseInt64(mValue);
    // } catch (NumberFormatException ignored) {
    //     Double asDouble = StringUtils::ParseDouble(mValue); // don't catch this NumberFormatException
    //     result = (Int64) asDouble;
    //     if ((Double) result != asDouble) {
    //         throw new NumberFormatException(mValue);
    //     }
    // }

    JsonToken jt1;
    Advance(&jt1);
    *data = result;
    return NOERROR;
}

ECode CJsonReader::NextInt(
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data)
    JsonToken jt;
    Peek(&jt);
    if (mToken != JsonToken_STRING && mToken != JsonToken_NUMBER) {
        Logger::E("CJsonReader::NextInt", "Expected an int but was %d", mToken);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expected an int but was " + mToken);
    }

    Int32 result;
    // try {
        result = StringUtils::ParseInt32(mValue);
    // } catch (NumberFormatException ignored) {
    //     Double asDouble = StringUtils::ParseDouble(mValue); // don't catch this NumberFormatException
    //     result = (Int32) asDouble;
    //     if ((Double) result != asDouble) {
    //         throw new NumberFormatException(mValue);
    //     }
    // }

    JsonToken jt1;
    Advance(&jt1);
    *data = result;
    return NOERROR;
}

ECode CJsonReader::Close()
{
    mValue = String(NULL);
    mToken = JsonToken_NULL;
    mStack->Clear();
    mStack->Add(CoreUtils::Convert((Int32)JsonScope_CLOSED));
    ICloseable::Probe(mIn)->Close();
    return NOERROR;
}

ECode CJsonReader::SkipValue()
{
    mSkipping = TRUE;
    ECode ec = NOERROR;
    Boolean res;
    // try {
        ec = HasNext(&res);
        if (FAILED(ec)) {
            mSkipping = FALSE;
            return ec;
        }
        else if (!res) {
            Logger::E("CJsonReader::SkipValue", "No element left to skip");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("No element left to skip");
        }
        else {
            JsonToken jt;
            ec = Peek(&jt);
            if (FAILED(ec)) {
                mSkipping = FALSE;
                return ec;
            }
            else if (jt == JsonToken_END_DOCUMENT) {
                Logger::E("CJsonReader::SkipValue", "No element left to skip");
                return E_ILLEGAL_STATE_EXCEPTION;
                // throw new IllegalStateException("No element left to skip");
            }
        }

        Int32 count = 0;
        do {
            JsonToken token;
            ec = Advance(&token);
            if (FAILED(ec)) {
                mSkipping = FALSE;
                return ec;
            }
            else if (token == JsonToken_BEGIN_ARRAY || token == JsonToken_BEGIN_OBJECT) {
                count++;
            }
            else if (token == JsonToken_END_ARRAY || token == JsonToken_END_OBJECT) {
                count--;
            }
        } while (count != 0);
    // } finally {
        mSkipping = FALSE;
    // }
    return NOERROR;
}

JsonScope CJsonReader::PeekStack()
{
    Int32 size;
    mStack->GetSize(&size);
    AutoPtr<IInterface> obj;
    mStack->Get(size - 1, (IInterface**)&obj);
    Int32 data;
    IInteger32::Probe(obj)->GetValue(&data);
    return (JsonScope)data;
}

JsonScope CJsonReader::Pop()
{
    Int32 size;
    mStack->GetSize(&size);
    AutoPtr<IInterface> obj;
    mStack->Remove(size - 1, (IInterface**)&obj);
    Int32 data;
    IInteger32::Probe(obj)->GetValue(&data);
    return (JsonScope)data;
}

void CJsonReader::Push(
    /* [in] */ JsonScope newTop)
{
    mStack->Add(CoreUtils::Convert((Int32)newTop));
}

void CJsonReader::ReplaceTop(
    /* [in] */ JsonScope newTop)
{
    Int32 size;
    mStack->GetSize(&size);
    mStack->Set(size - 1, CoreUtils::Convert((Int32)newTop));
}

ECode CJsonReader::NextInArray(
    /* [in] */ Boolean firstElement,
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    *result = JsonToken_NULL;
    if (firstElement) {
        ReplaceTop(JsonScope_NONEMPTY_ARRAY);
    }
    else {
        /* Look for a comma before each element after the first element. */
        Int32 nnw;
        FAIL_RETURN(NextNonWhitespace(&nnw));
        switch (nnw) {
            case ']':
                Pop();
                *result = mToken = JsonToken_END_ARRAY;
                return NOERROR;
            case ';':
                CheckLenient(); // fall-through
            case ',':
                break;
            default:
                return SyntaxError(String("Unterminated array"));
                // throw SyntaxError("Unterminated array");
        }
    }

    Int32 nnw;
    FAIL_RETURN(NextNonWhitespace(&nnw));
    switch (nnw) {
        case ']':
            if (firstElement) {
                Pop();
                *result = mToken = JsonToken_END_ARRAY;
                return NOERROR;
            }
            // fall-through to handle ",]"
        case ';':
        case ',':
            /* In lenient mode, a 0-length literal means 'null' */
            CheckLenient();
            mPos--;
            mValue = "null";
            *result = mToken = JsonToken_NULL;
            return NOERROR;
        default:
            mPos--;
            return NextValue(result);
    }
    return NOERROR;
}

ECode CJsonReader::NextInObject(
    /* [in] */ Boolean firstElement,
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    *result = JsonToken_NULL;
    /*
     * Read delimiters. Either a comma/semicolon separating this and the
     * previous name-value pair, or a close brace to denote the end of the
     * object.
     */
    if (firstElement) {
        /* Peek to see if this is the empty object. */
        Int32 nnw;
        FAIL_RETURN(NextNonWhitespace(&nnw));
        switch (nnw) {
            case '}':
                Pop();
                *result = mToken = JsonToken_END_OBJECT;
                return NOERROR;
            default:
                mPos--;
        }
    }
    else {
        Int32 nnw;
        FAIL_RETURN(NextNonWhitespace(&nnw));
        switch (nnw) {
            case '}':
                Pop();
                *result = mToken = JsonToken_END_OBJECT;
                return NOERROR;
            case ';':
            case ',':
                break;
            default:
                return SyntaxError(String("Unterminated object"));
                // throw SyntaxError("Unterminated object");
        }
    }

    /* Read the name. */
    Int32 quote;
    FAIL_RETURN(NextNonWhitespace(&quote));
    switch (quote) {
        case '\'':
            CheckLenient(); // fall-through
        case '"':
            NextString((Char32) quote, &mName);
            break;
        default:
            CheckLenient();
            mPos--;
            NextLiteral(FALSE, &mName);
            if (mName.IsEmpty()) {
                return SyntaxError(String("Expected name"));
                // throw SyntaxError("Expected name");
            }
    }

    ReplaceTop(JsonScope_DANGLING_NAME);
    *result = mToken = JsonToken_NAME;
    return NOERROR;
}

ECode CJsonReader::ObjectValue(
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    *result = JsonToken_NULL;
    /*
     * Read the name/value separator. Usually a colon ':'. In lenient mode
     * we also accept an equals sign '=', or an arrow "=>".
     */
    Boolean res;
    Int32 nnw;
    FAIL_RETURN(NextNonWhitespace(&nnw));
    switch (nnw) {
        case ':':
            break;
        case '=':
            CheckLenient();
            if ((mPos < mLimit || (FillBuffer(1, &res), res)) && (*mBuffer)[mPos] == '>') {
                mPos++;
            }
            break;
        default:
            return SyntaxError(String("Expected ':'"));
            // throw SyntaxError("Expected ':'");
    }

    ReplaceTop(JsonScope_NONEMPTY_OBJECT);
    return NextValue(result);
}

ECode CJsonReader::NextValue(
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    *result = JsonToken_NULL;
    Int32 c;
    FAIL_RETURN(NextNonWhitespace(&c));
    switch (c) {
        case '{':
            Push(JsonScope_EMPTY_OBJECT);
            *result = mToken = JsonToken_BEGIN_OBJECT;
            return NOERROR;

        case '[':
            Push(JsonScope_EMPTY_ARRAY);
            *result = mToken = JsonToken_BEGIN_ARRAY;
            return NOERROR;

        case '\'':
            CheckLenient(); // fall-through
        case '"':
            NextString((Char32) c, &mValue);
            *result = mToken = JsonToken_STRING;
            return NOERROR;

        default:
            mPos--;
            return ReadLiteral(result);
    }
    return NOERROR;
}

ECode CJsonReader::FillBuffer(
    /* [in] */ Int32 minimum,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // Before clobbering the old characters, update where buffer starts
    for (Int32 i = 0; i < mPos; i++) {
        if ((*mBuffer)[i] == '\n') {
            mBufferStartLine++;
            mBufferStartColumn = 1;
        }
        else {
            mBufferStartColumn++;
        }
    }

    if (mLimit != mPos) {
        mLimit -= mPos;
        // System.arraycopy(mBuffer, mPos, mBuffer, 0, mLimit);
        mBuffer->Copy(0, mBuffer, mPos, mLimit);
    }
    else {
        mLimit = 0;
    }

    mPos = 0;
    Int32 total;
    while ((mIn->Read(mBuffer, mLimit, mBuffer->GetLength() - mLimit, &total), total) != -1) {
        mLimit += total;

        // if this is the first read, consume an optional byte order mark (BOM) if it exists
        if (mBufferStartLine == 1 && mBufferStartColumn == 1
                && mLimit > 0 && (*mBuffer)[0] == 0xfeff) {
            mPos++;
            mBufferStartColumn--;
        }

        if (mLimit >= minimum) {
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

Int32 CJsonReader::GetLineNumber()
{
    Int32 result = mBufferStartLine;
    for (Int32 i = 0; i < mPos; i++) {
        if ((*mBuffer)[i] == '\n') {
            result++;
        }
    }
    return result;
}

Int32 CJsonReader::GetColumnNumber()
{
    Int32 result = mBufferStartColumn;
    for (Int32 i = 0; i < mPos; i++) {
        if ((*mBuffer)[i] == '\n') {
            result = 1;
        }
        else {
            result++;
        }
    }
    return result;
}

ECode CJsonReader::NextNonWhitespace(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Boolean res;
    while (mPos < mLimit || (FillBuffer(1, &res), res)) {
        Int32 c = (*mBuffer)[mPos++];
        switch (c) {
            case '\t':
            case ' ':
            case '\n':
            case '\r':
                continue;

            case '/': {
                if (mPos == mLimit && (FillBuffer(1, &res), !res)) {
                    *result = c;
                    return NOERROR;
                }

                CheckLenient();
                Char32 peek = (*mBuffer)[mPos];
                switch (peek) {
                    case '*':
                        // skip a /* c-style comment */
                        mPos++;
                        if (SkipTo(String("*/"), &res), !res) {
                            return SyntaxError(String("Unterminated comment"));
                            // throw SyntaxError("Unterminated comment");
                        }
                        mPos += 2;
                        continue;

                    case '/':
                        // skip a // end-of-line comment
                        mPos++;
                        SkipToEndOfLine();
                        continue;

                    default:
                        *result = c;
                        return NOERROR;
                }
            }

            case '#':
                /*
                 * Skip a # hash end-of-line comment. The JSON RFC doesn't
                 * specify this behaviour, but it's required to parse
                 * existing documents. See http://b/2571423.
                 */
                CheckLenient();
                SkipToEndOfLine();
                continue;

            default:
                *result = c;
                return NOERROR;
        }
    }

    Logger::E("CJsonReader::NextNonWhitespace", "End of input");
    return E_EOF_EXCEPTION;
    // throw new EOFException("End of input");
}

ECode CJsonReader::CheckLenient()
{
    if (!mLenient) {
        return SyntaxError(String("Use JsonReader.setLenient(true) to accept malformed JSON"));
        // throw SyntaxError("Use JsonReader.setLenient(true) to accept malformed JSON");
    }
    return NOERROR;
}

ECode CJsonReader::SkipToEndOfLine()
{
    Boolean res;
    while (mPos < mLimit || (FillBuffer(1, &res), res)) {
        Char32 c = (*mBuffer)[mPos++];
        if (c == '\r' || c == '\n') {
            break;
        }
    }
    return NOERROR;
}

ECode CJsonReader::SkipTo(
    /* [in] */ const String& toFind,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 length = toFind.GetLength();
    Boolean res;
    outer:
    for (; mPos + length <= mLimit || (FillBuffer(length, &res), res); mPos++) {
        for (Int32 c = 0; c < length; c++) {
            if ((*mBuffer)[mPos + c] != toFind.GetChar(c)) {
                goto outer;
            }
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CJsonReader::NextString(
    /* [in] */ Char32 quote,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Boolean res;

    AutoPtr<StringBuilder> builder;
    do {
        /* the index of the first character not yet appended to the builder. */
        Int32 start = mPos;
        while (mPos < mLimit) {
            Int32 c = (*mBuffer)[mPos++];

            if (c == (Int32)quote) {
                if (mSkipping) {
                    *str = "skipped!";
                    return NOERROR;
                }
                else if (builder == NULL) {
                    *str = mStringPool->Get(mBuffer, start, mPos - start - 1);
                    return NOERROR;
                }
                else {
                    builder->Append(*mBuffer, start, mPos - start - 1);
                    *str = builder->ToString();
                    return NOERROR;
                }

            }
            else if (c == '\\') {
                if (builder == NULL) {
                    builder = new StringBuilder();
                }
                builder->Append(*mBuffer, start, mPos - start - 1);
                Char32 ch;
                ReadEscapeCharacter(&ch);
                builder->AppendChar(ch);
                start = mPos;
            }
        }

        if (builder == NULL) {
            builder = new StringBuilder();
        }
        builder->Append(*mBuffer, start, mPos - start);
    } while (FillBuffer(1, &res), res);

    return SyntaxError(String("Unterminated string"));
    // throw SyntaxError("Unterminated string");
}

ECode CJsonReader::NextLiteral(
    /* [in] */ Boolean assignOffsetsOnly,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<StringBuilder> builder;
    mValuePos = -1;
    mValueLength = 0;
    Int32 i = 0;

    while (TRUE) {
        for (; mPos + i < mLimit; i++) {
            switch ((*mBuffer)[mPos + i]) {
                case '/':
                case '\\':
                case ';':
                case '#':
                case '=':
                    CheckLenient(); // fall-through
                case '{':
                case '}':
                case '[':
                case ']':
                case ':':
                case ',':
                case ' ':
                case '\t':
                case '\f':
                case '\r':
                case '\n':
                    goto findNonLiteralCharacter;
            }
        }

        /*
         * Attempt to load the entire literal into the buffer at once. If
         * we run out of input, add a non-literal character at the end so
         * that decoding doesn't need to do bounds checks.
         */
        Boolean res;
        if (i < mBuffer->GetLength()) {
            if (FillBuffer(i + 1, &res), res) {
                continue;
            }
            else {
                (*mBuffer)[mLimit] = '\0';
                break;
            }
        }

        // use a StringBuilder when the value is too long. It must be an unquoted string.
        if (builder == NULL) {
            builder = new StringBuilder();
        }
        builder->Append(*mBuffer, mPos, i);
        mValueLength += i;
        mPos += i;
        i = 0;
        if (FillBuffer(1, &res), !res) {
            break;
        }
    }
    findNonLiteralCharacter:

    String result;
    if (assignOffsetsOnly && builder == NULL) {
        mValuePos = mPos;
        result = String(NULL);
    }
    else if (mSkipping) {
        result = "skipped!";
    }
    else if (builder == NULL) {
        result = mStringPool->Get(mBuffer, mPos, i);
    }
    else {
        builder->Append(*mBuffer, mPos, i);
        result = builder->ToString();
    }
    mValueLength += i;
    mPos += i;
    *str = result;
    return NOERROR;
}

ECode CJsonReader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder builder("CJsonReader");
    builder += " near ";
    builder += GetSnippet();
    *str = builder.ToString();
    return NOERROR;
}

ECode CJsonReader::ReadEscapeCharacter(
    /* [out] */ Char32* ch)
{
    VALIDATE_NOT_NULL(ch)
    *ch = 0;

    Boolean res;
    if (mPos == mLimit && (FillBuffer(1, &res), !res)) {
        return SyntaxError(String("Unterminated escape sequence"));
        // throw SyntaxError("Unterminated escape sequence");
    }

    Char32 escaped = (*mBuffer)[mPos++];
    switch (escaped) {
        case 'u': {
            if (mPos + 4 > mLimit && (FillBuffer(4, &res), !res)) {
                return SyntaxError(String("Unterminated escape sequence"));
                // throw SyntaxError("Unterminated escape sequence");
            }
            String hex = mStringPool->Get(mBuffer, mPos, 4);
            mPos += 4;
            *ch = (Char32) StringUtils::ParseInt32(hex, 16);
            return NOERROR;
        }

        case 't':
            *ch = '\t';
            return NOERROR;

        case 'b':
            *ch = '\b';
            return NOERROR;

        case 'n':
            *ch = '\n';
            return NOERROR;

        case 'r':
            *ch = '\r';
            return NOERROR;

        case 'f':
            *ch = '\f';
            return NOERROR;

        case '\'':
        case '"':
        case '\\':
        default:
            *ch = escaped;
            return NOERROR;
    }
    return NOERROR;
}

ECode CJsonReader::ReadLiteral(
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)

    NextLiteral(TRUE, &mValue);
    if (mValueLength == 0) {
        return SyntaxError(String("Expected literal value"));
        // throw SyntaxError("Expected literal value");
    }
    DecodeLiteral(&mToken);
    if (mToken == JsonToken_STRING) {
        CheckLenient();
    }
    *result = mToken;
    return NOERROR;
}

ECode CJsonReader::DecodeLiteral(
    /* [out] */ JsonToken* result)
{
    VALIDATE_NOT_NULL(result)
    if (mValuePos == -1) {
        // it was too long to fit in the buffer so it can only be a string
        *result = JsonToken_STRING;
        return NOERROR;
    }
    else if (mValueLength == 4
            && ('n' == (*mBuffer)[mValuePos    ] || 'N' == (*mBuffer)[mValuePos    ])
            && ('u' == (*mBuffer)[mValuePos + 1] || 'U' == (*mBuffer)[mValuePos + 1])
            && ('l' == (*mBuffer)[mValuePos + 2] || 'L' == (*mBuffer)[mValuePos + 2])
            && ('l' == (*mBuffer)[mValuePos + 3] || 'L' == (*mBuffer)[mValuePos + 3])) {
        mValue = "null";
        *result = JsonToken_NULL;
        return NOERROR;
    }
    else if (mValueLength == 4
            && ('t' == (*mBuffer)[mValuePos    ] || 'T' == (*mBuffer)[mValuePos    ])
            && ('r' == (*mBuffer)[mValuePos + 1] || 'R' == (*mBuffer)[mValuePos + 1])
            && ('u' == (*mBuffer)[mValuePos + 2] || 'U' == (*mBuffer)[mValuePos + 2])
            && ('e' == (*mBuffer)[mValuePos + 3] || 'E' == (*mBuffer)[mValuePos + 3])) {
        mValue = sTRUE;
        *result = JsonToken_BOOLEAN;
        return NOERROR;
    }
    else if (mValueLength == 5
            && ('f' == (*mBuffer)[mValuePos    ] || 'F' == (*mBuffer)[mValuePos    ])
            && ('a' == (*mBuffer)[mValuePos + 1] || 'A' == (*mBuffer)[mValuePos + 1])
            && ('l' == (*mBuffer)[mValuePos + 2] || 'L' == (*mBuffer)[mValuePos + 2])
            && ('s' == (*mBuffer)[mValuePos + 3] || 'S' == (*mBuffer)[mValuePos + 3])
            && ('e' == (*mBuffer)[mValuePos + 4] || 'E' == (*mBuffer)[mValuePos + 4])) {
        mValue = sFALSE;
        *result = JsonToken_BOOLEAN;
        return NOERROR;
    }
    else {
        mValue = mStringPool->Get(mBuffer, mValuePos, mValueLength);
        *result = DecodeNumber(mBuffer, mValuePos, mValueLength);
        return NOERROR;
    }
}

JsonToken CJsonReader::DecodeNumber(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    Int32 i = offset;
    Int32 c = (*chars)[i];

    if (c == '-') {
        c = (*chars)[++i];
    }

    if (c == '0') {
        c = (*chars)[++i];
    }
    else if (c >= '1' && c <= '9') {
        c = (*chars)[++i];
        while (c >= '0' && c <= '9') {
            c = (*chars)[++i];
        }
    }
    else {
        return JsonToken_STRING;
    }

    if (c == '.') {
        c = (*chars)[++i];
        while (c >= '0' && c <= '9') {
            c = (*chars)[++i];
        }
    }

    if (c == 'e' || c == 'E') {
        c = (*chars)[++i];
        if (c == '+' || c == '-') {
            c = (*chars)[++i];
        }
        if (c >= '0' && c <= '9') {
            c = (*chars)[++i];
            while (c >= '0' && c <= '9') {
                c = (*chars)[++i];
            }
        }
        else {
            return JsonToken_STRING;
        }
    }

    if (i == offset + length) {
        return JsonToken_NUMBER;
    }
    else {
        return JsonToken_STRING;
    }
}

ECode CJsonReader::SyntaxError(
    /* [in] */ const String& message)
{
    StringBuilder builder("");
    builder += message;
    builder += " at line ";
    builder += GetLineNumber();
    builder += " column ";
    builder += GetColumnNumber();
    Logger::E("CJsonReader::SyntaxError", "%s", builder.ToString().string());
    return E_MALFORMED_JSON_EXCEPTION;
    // throw new MalformedJsonException(message
    //         + " at line " + GetLineNumber() + " column " + GetColumnNumber());
}

AutoPtr<ICharSequence> CJsonReader::GetSnippet()
{
    StringBuilder snippet;
    using Elastos::Core::Math;
    Int32 beforePos = Math::Min(mPos, 20);
    snippet.Append(*mBuffer, mPos - beforePos, beforePos);
    Int32 afterPos = Math::Min(mLimit - mPos, 20);
    snippet.Append(*mBuffer, mPos, afterPos);
    return snippet.ToCharSequence();
}

} //Utility
} //Droid
} //Elastos