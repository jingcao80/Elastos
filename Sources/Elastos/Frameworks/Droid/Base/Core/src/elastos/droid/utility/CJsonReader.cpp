#include "elastos/droid/utility/CJsonReader.h"
#include <elastos/utility/StringPool.h>

using Elastos::Utility::StringPool;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CJsonReader, Object, IJsonReader, ICloseable)

CAR_OBJECT_IMPL(CJsonReader)

const String CJsonReader::sTRUE("true");
const String CJsonReader::sFALSE("false");

CJsonReader::CJsonReader()
{
}

CARAPI constructor(
    /* [in] */ IReader* in)
{
    assert(0);
    // if (in == null) {
    //     throw new NullPointerException("in == null");
    // }
    // this.mIn = in;
    return NOERROR;
}

/**
 * Configure this parser to be  be liberal in what it accepts. By default,
 * this parser is strict and only accepts JSON as specified by <a
 * href="http://www.ietf.org/rfc/rfc4627.txt">RFC 4627</a>. Setting the
 * parser to mLenient causes it to ignore the following syntax errors:
 *
 * <ul>
 *   <li>End of line comments starting with {@code //} or {@code #} and
 *       ending with a newline character.
 *   <li>C-style comments starting with {@code /*} and ending with
 *       {@code *}{@code /}. Such comments may not be nested.
 *   <li>Names that are unquoted or {@code 'single quoted'}.
 *   <li>Strings that are unquoted or {@code 'single quoted'}.
 *   <li>Array elements separated by {@code ;} instead of {@code ,}.
 *   <li>Unnecessary array separators. These are interpreted as if null
 *       was the omitted value.
 *   <li>Names and values separated by {@code =} or {@code =>} instead of
 *       {@code :}.
 *   <li>Name/value pairs separated by {@code ;} instead of {@code ,}.
 * </ul>
 */
CARAPI SetLenient(Boolean mLenient) {
    this.mLenient = mLenient;
}

/**
 * Returns true if this parser is liberal in what it accepts.
 */
public Boolean IsLenient() {
    return mLenient;
}

/**
 * Consumes the next token from the JSON stream and asserts that it is the
 * beginning of a new array.
 */
CARAPI BeginArray() throws IOException {
    Expect(JsonToken.BEGIN_ARRAY);
}

/**
 * Consumes the next token from the JSON stream and asserts that it is the
 * end of the current array.
 */
CARAPI EndArray() throws IOException {
    Expect(JsonToken.END_ARRAY);
}

/**
 * Consumes the next token from the JSON stream and asserts that it is the
 * beginning of a new object.
 */
CARAPI BeginObject() throws IOException {
    Expect(JsonToken.BEGIN_OBJECT);
}

/**
 * Consumes the next token from the JSON stream and asserts that it is the
 * end of the current array.
 */
CARAPI EndObject() throws IOException {
    Expect(JsonToken.END_OBJECT);
}

/**
 * Consumes {@code expected}.
 */
private void Expect(JsonToken expected) throws IOException {
    Peek();
    if (token != expected) {
        throw new IllegalStateException("Expected " + expected + " but was " + Peek());
    }
    Advance();
}

/**
 * Returns true if the current array or object has another element.
 */
public Boolean HasNext() throws IOException {
    Peek();
    return token != JsonToken.END_OBJECT && token != JsonToken.END_ARRAY;
}

/**
 * Returns the type of the next token without consuming it.
 */
public JsonToken Peek() throws IOException {
    if (token != null) {
      return token;
    }

    switch (peekStack()) {
        case EMPTY_DOCUMENT:
            replaceTop(JsonScope.NONEMPTY_DOCUMENT);
            JsonToken firstToken = nextValue();
            if (!mLenient && token != JsonToken.BEGIN_ARRAY && token != JsonToken.BEGIN_OBJECT) {
                throw new IOException(
                        "Expected JSON document to start with '[' or '{' but was " + token);
            }
            return firstToken;
        case EMPTY_ARRAY:
            return nextInArray(true);
        case NONEMPTY_ARRAY:
            return nextInArray(false);
        case EMPTY_OBJECT:
            return nextInObject(true);
        case DANGLING_NAME:
            return objectValue();
        case NONEMPTY_OBJECT:
            return nextInObject(false);
        case NONEMPTY_DOCUMENT:
            try {
                JsonToken token = nextValue();
                if (mLenient) {
                    return token;
                }
                throw syntaxError("Expected EOF");
            } catch (EOFException e) {
                return token = JsonToken.END_DOCUMENT; // TODO: avoid throwing here?
            }
        case CLOSED:
            throw new IllegalStateException("JsonReader is closed");
        default:
            throw new AssertionError();
    }
}

/**
 * Advances the cursor in the JSON stream to the next token.
 */
private JsonToken Advance() throws IOException {
    Peek();

    JsonToken result = token;
    token = null;
    value = null;
    name = null;
    return result;
}

/**
 * Returns the next token, a {@link JsonToken#NAME property name}, and
 * consumes it.
 *
 * @throws IOException if the next token in the stream is not a property
 *     name.
 */
public String NextName() throws IOException {
    Peek();
    if (token != JsonToken.NAME) {
        throw new IllegalStateException("Expected a name but was " + Peek());
    }
    String result = name;
    Advance();
    return result;
}

/**
 * Returns the {@link JsonToken#STRING string} value of the next token,
 * consuming it. If the next token is a number, this method will return its
 * string form.
 *
 * @throws IllegalStateException if the next token is not a string or if
 *     this reader is closed.
 */
public String NextString() throws IOException {
    Peek();
    if (token != JsonToken.STRING && token != JsonToken.NUMBER) {
        throw new IllegalStateException("Expected a string but was " + Peek());
    }

    String result = value;
    Advance();
    return result;
}

/**
 * Returns the {@link JsonToken#BOOLEAN Boolean} value of the next token,
 * consuming it.
 *
 * @throws IllegalStateException if the next token is not a Boolean or if
 *     this reader is closed.
 */
public Boolean NextBoolean() throws IOException {
    Peek();
    if (token != JsonToken.BOOLEAN) {
        throw new IllegalStateException("Expected a Boolean but was " + token);
    }

    Boolean result = (value == TRUE);
    Advance();
    return result;
}

/**
 * Consumes the next token from the JSON stream and asserts that it is a
 * literal null.
 *
 * @throws IllegalStateException if the next token is not null or if this
 *     reader is closed.
 */
CARAPI nextNull() throws IOException {
    Peek();
    if (token != JsonToken.NULL) {
        throw new IllegalStateException("Expected null but was " + token);
    }

    Advance();
}

/**
 * Returns the {@link JsonToken#NUMBER double} value of the next token,
 * consuming it. If the next token is a string, this method will attempt to
 * parse it as a double using {@link Double#parseDouble(String)}.
 *
 * @throws IllegalStateException if the next token is not a literal value.
 */
public double nextDouble() throws IOException {
    Peek();
    if (token != JsonToken.STRING && token != JsonToken.NUMBER) {
        throw new IllegalStateException("Expected a double but was " + token);
    }

    double result = Double.parseDouble(value);
    Advance();
    return result;
}

/**
 * Returns the {@link JsonToken#NUMBER long} value of the next token,
 * consuming it. If the next token is a string, this method will attempt to
 * parse it as a long. If the next token's numeric value cannot be exactly
 * represented by a Java {@code long}, this method throws.
 *
 * @throws IllegalStateException if the next token is not a literal value.
 * @throws NumberFormatException if the next literal value cannot be parsed
 *     as a number, or exactly represented as a long.
 */
public long nextLong() throws IOException {
    Peek();
    if (token != JsonToken.STRING && token != JsonToken.NUMBER) {
        throw new IllegalStateException("Expected a long but was " + token);
    }

    long result;
    try {
        result = Long.parseLong(value);
    } catch (NumberFormatException ignored) {
        double asDouble = Double.parseDouble(value); // don't catch this NumberFormatException
        result = (long) asDouble;
        if ((double) result != asDouble) {
            throw new NumberFormatException(value);
        }
    }

    Advance();
    return result;
}

/**
 * Returns the {@link JsonToken#NUMBER int} value of the next token,
 * consuming it. If the next token is a string, this method will attempt to
 * parse it as an int. If the next token's numeric value cannot be exactly
 * represented by a Java {@code int}, this method throws.
 *
 * @throws IllegalStateException if the next token is not a literal value.
 * @throws NumberFormatException if the next literal value cannot be parsed
 *     as a number, or exactly represented as an int.
 */
public int nextInt() throws IOException {
    Peek();
    if (token != JsonToken.STRING && token != JsonToken.NUMBER) {
        throw new IllegalStateException("Expected an int but was " + token);
    }

    int result;
    try {
        result = Integer.parseInt(value);
    } catch (NumberFormatException ignored) {
        double asDouble = Double.parseDouble(value); // don't catch this NumberFormatException
        result = (int) asDouble;
        if ((double) result != asDouble) {
            throw new NumberFormatException(value);
        }
    }

    Advance();
    return result;
}

/**
 * Closes this JSON reader and the underlying {@link Reader}.
 */
CARAPI close() throws IOException {
    value = null;
    token = null;
    stack.clear();
    stack.add(JsonScope.CLOSED);
    mIn.close();
}

/**
 * Skips the next value recursively. If it is an object or array, all nested
 * elements are skipped. This method is intended for use when the JSON token
 * stream contains unrecognized or unhandled values.
 */
CARAPI skipValue() throws IOException {
    skipping = true;
    try {
        if (!HasNext() || Peek() == JsonToken.END_DOCUMENT) {
            throw new IllegalStateException("No element left to skip");
        }
        int count = 0;
        do {
            JsonToken token = Advance();
            if (token == JsonToken.BEGIN_ARRAY || token == JsonToken.BEGIN_OBJECT) {
                count++;
            } else if (token == JsonToken.END_ARRAY || token == JsonToken.END_OBJECT) {
                count--;
            }
        } while (count != 0);
    } finally {
        skipping = false;
    }
}

private JsonScope peekStack() {
    return stack.get(stack.size() - 1);
}

private JsonScope pop() {
    return stack.remove(stack.size() - 1);
}

private void push(JsonScope newTop) {
    stack.add(newTop);
}

/**
 * Replace the value on the top of the stack with the given value.
 */
private void replaceTop(JsonScope newTop) {
    stack.set(stack.size() - 1, newTop);
}

private JsonToken nextInArray(Boolean firstElement) throws IOException {
    if (firstElement) {
        replaceTop(JsonScope.NONEMPTY_ARRAY);
    } else {
        /* Look for a comma before each element after the first element. */
        switch (nextNonWhitespace()) {
            case ']':
                pop();
                return token = JsonToken.END_ARRAY;
            case ';':
                checkLenient(); // fall-through
            case ',':
                break;
            default:
                throw syntaxError("Unterminated array");
        }
    }

    switch (nextNonWhitespace()) {
        case ']':
            if (firstElement) {
                pop();
                return token = JsonToken.END_ARRAY;
            }
            // fall-through to handle ",]"
        case ';':
        case ',':
            /* In mLenient mode, a 0-length literal means 'null' */
            checkLenient();
            pos--;
            value = "null";
            return token = JsonToken.NULL;
        default:
            pos--;
            return nextValue();
    }
}

private JsonToken nextInObject(Boolean firstElement) throws IOException {
    /*
     * Read delimiters. Either a comma/semicolon separating this and the
     * previous name-value pair, or a close brace to denote the end of the
     * object.
     */
    if (firstElement) {
        /* Peek to see if this is the empty object. */
        switch (nextNonWhitespace()) {
            case '}':
                pop();
                return token = JsonToken.END_OBJECT;
            default:
                pos--;
        }
    } else {
        switch (nextNonWhitespace()) {
            case '}':
                pop();
                return token = JsonToken.END_OBJECT;
            case ';':
            case ',':
                break;
            default:
                throw syntaxError("Unterminated object");
        }
    }

    /* Read the name. */
    int quote = nextNonWhitespace();
    switch (quote) {
        case '\'':
            checkLenient(); // fall-through
        case '"':
            name = NextString((char) quote);
            break;
        default:
            checkLenient();
            pos--;
            name = nextLiteral(false);
            if (name.isEmpty()) {
                throw syntaxError("Expected name");
            }
    }

    replaceTop(JsonScope.DANGLING_NAME);
    return token = JsonToken.NAME;
}

private JsonToken objectValue() throws IOException {
    /*
     * Read the name/value separator. Usually a colon ':'. In mLenient mode
     * we also accept an equals sign '=', or an arrow "=>".
     */
    switch (nextNonWhitespace()) {
        case ':':
            break;
        case '=':
            checkLenient();
            if ((pos < limit || fillBuffer(1)) && (*mBuffer)[pos] == '>') {
                pos++;
            }
            break;
        default:
            throw syntaxError("Expected ':'");
    }

    replaceTop(JsonScope.NONEMPTY_OBJECT);
    return nextValue();
}

private JsonToken nextValue() throws IOException {
    int c = nextNonWhitespace();
    switch (c) {
        case '{':
            push(JsonScope.EMPTY_OBJECT);
            return token = JsonToken.BEGIN_OBJECT;

        case '[':
            push(JsonScope.EMPTY_ARRAY);
            return token = JsonToken.BEGIN_ARRAY;

        case '\'':
            checkLenient(); // fall-through
        case '"':
            value = NextString((char) c);
            return token = JsonToken.STRING;

        default:
            pos--;
            return readLiteral();
    }
}

/**
 * Returns true once {@code limit - pos >= minimum}. If the data is
 * exhausted before that many characters are available, this returns
 * false.
 */
private Boolean fillBuffer(int minimum) throws IOException {
    // Before clobbering the old characters, update where buffer starts
    for (int i = 0; i < pos; i++) {
        if ((*mBuffer)[i] == '\n') {
            mBufferStartLine++;
            mBufferStartColumn = 1;
        } else {
            mBufferStartColumn++;
        }
    }

    if (limit != pos) {
        limit -= pos;
        System.arraycopy(mBuffer, pos, mBuffer, 0, limit);
    } else {
        limit = 0;
    }

    pos = 0;
    int total;
    while ((total = mIn.read(mBuffer, limit, mBuffer.length - limit)) != -1) {
        limit += total;

        // if this is the first read, consume an optional byte order mark (BOM) if it exists
            if (mBufferStartLine == 1 && mBufferStartColumn == 1
                    && limit > 0 && (*mBuffer)[0] == '\ufeff') {
            pos++;
            mBufferStartColumn--;
        }

        if (limit >= minimum) {
            return true;
        }
    }
    return false;
}

private int getLineNumber() {
    int result = mBufferStartLine;
    for (int i = 0; i < pos; i++) {
        if ((*mBuffer)[i] == '\n') {
            result++;
        }
    }
    return result;
}

private int getColumnNumber() {
    int result = mBufferStartColumn;
    for (int i = 0; i < pos; i++) {
        if ((*mBuffer)[i] == '\n') {
            result = 1;
        } else {
            result++;
        }
    }
    return result;
}

private int nextNonWhitespace() throws IOException {
    while (pos < limit || fillBuffer(1)) {
        int c = (*mBuffer)[pos++];
        switch (c) {
            case '\t':
            case ' ':
            case '\n':
            case '\r':
                continue;

            case '/':
                if (pos == limit && !fillBuffer(1)) {
                    return c;
                }

                checkLenient();
                char peek = (*mBuffer)[pos];
                switch (peek) {
                    case '*':
                        // skip a /* c-style comment */
                        pos++;
                        if (!skipTo("*/")) {
                            throw syntaxError("Unterminated comment");
                        }
                        pos += 2;
                        continue;

                    case '/':
                        // skip a // end-of-line comment
                        pos++;
                        skipToEndOfLine();
                        continue;

                    default:
                        return c;
                }

            case '#':
                /*
                 * Skip a # hash end-of-line comment. The JSON RFC doesn't
                 * specify this behaviour, but it's required to parse
                 * existing documents. See http://b/2571423.
                 */
                checkLenient();
                skipToEndOfLine();
                continue;

            default:
                return c;
        }
    }

    throw new EOFException("End of input");
}

private void checkLenient() throws IOException {
    if (!mLenient) {
        throw syntaxError("Use JsonReader.SetLenient(true) to accept malformed JSON");
    }
}

/**
 * Advances the position until after the next newline character. If the line
 * is terminated by "\r\n", the '\n' must be consumed as whitespace by the
 * caller.
 */
private void skipToEndOfLine() throws IOException {
    while (pos < limit || fillBuffer(1)) {
        char c = (*mBuffer)[pos++];
        if (c == '\r' || c == '\n') {
            break;
        }
    }
}

private Boolean skipTo(String toFind) throws IOException {
    outer:
    for (; pos + toFind.length() <= limit || fillBuffer(toFind.length()); pos++) {
        for (int c = 0; c < toFind.length(); c++) {
            if ((*mBuffer)[pos + c] != toFind.charAt(c)) {
                continue outer;
            }
        }
        return true;
    }
    return false;
}

/**
 * Returns the string up to but not including {@code quote}, unescaping any
 * character escape sequences encountered along the way. The opening quote
 * should have already been read. This consumes the closing quote, but does
 * not include it in the returned string.
 *
 * @param quote either ' or ".
 * @throws NumberFormatException if any unicode escape sequences are
 *     malformed.
 */
private String NextString(char quote) throws IOException {
    StringBuilder builder = null;
    do {
        /* the index of the first character not yet appended to the builder. */
        int start = pos;
        while (pos < limit) {
            int c = (*mBuffer)[pos++];

            if (c == quote) {
                if (skipping) {
                    return "skipped!";
                } else if (builder == null) {
                    return mStringPool.get(mBuffer, start, pos - start - 1);
                } else {
                    builder.append(mBuffer, start, pos - start - 1);
                    return builder.toString();
                }

            } else if (c == '\\') {
                if (builder == null) {
                    builder = new StringBuilder();
                }
                builder.append(mBuffer, start, pos - start - 1);
                builder.append(readEscapeCharacter());
                start = pos;
            }
        }

        if (builder == null) {
            builder = new StringBuilder();
        }
        builder.append(mBuffer, start, pos - start);
    } while (fillBuffer(1));

    throw syntaxError("Unterminated string");
}

/**
 * Reads the value up to but not including any delimiter characters. This
 * does not consume the delimiter character.
 *
 * @param assignOffsetsOnly true for this method to only set the valuePos
 *     and valueLength fields and return a null result. This only works if
 *     the literal is short; a string is returned otherwise.
 */
private String nextLiteral(Boolean assignOffsetsOnly) throws IOException {
    StringBuilder builder = null;
    valuePos = -1;
    valueLength = 0;
    int i = 0;

    findNonLiteralCharacter:
    while (true) {
        for (; pos + i < limit; i++) {
            switch ((*mBuffer)[pos + i]) {
            case '/':
            case '\\':
            case ';':
            case '#':
            case '=':
                checkLenient(); // fall-through
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
                break findNonLiteralCharacter;
            }
        }

        /*
         * Attempt to load the entire literal into the buffer at once. If
         * we run out of input, add a non-literal character at the end so
         * that decoding doesn't need to do bounds checks.
         */
        if (i < mBuffer.length) {
            if (fillBuffer(i + 1)) {
                continue;
            } else {
                (*mBuffer)[limit] = '\0';
                break;
            }
        }

        // use a StringBuilder when the value is too long. It must be an unquoted string.
        if (builder == null) {
            builder = new StringBuilder();
        }
        builder.append(mBuffer, pos, i);
        valueLength += i;
        pos += i;
        i = 0;
        if (!fillBuffer(1)) {
            break;
        }
    }

    String result;
    if (assignOffsetsOnly && builder == null) {
        valuePos = pos;
        result = null;
    } else if (skipping) {
        result = "skipped!";
    } else if (builder == null) {
        result = mStringPool.get(mBuffer, pos, i);
    } else {
        builder.append(mBuffer, pos, i);
        result = builder.toString();
    }
    valueLength += i;
    pos += i;
    return result;
}

@Override public String toString() {
    return getClass().getSimpleName() + " near " + getSnippet();
}

/**
 * Unescapes the character identified by the character or characters that
 * immediately follow a backslash. The backslash '\' should have already
 * been read. This supports both unicode escapes "u000A" and two-character
 * escapes "\n".
 *
 * @throws NumberFormatException if any unicode escape sequences are
 *     malformed.
 */
private char readEscapeCharacter() throws IOException {
    if (pos == limit && !fillBuffer(1)) {
        throw syntaxError("Unterminated escape sequence");
    }

    char escaped = (*mBuffer)[pos++];
    switch (escaped) {
        case 'u':
            if (pos + 4 > limit && !fillBuffer(4)) {
                throw syntaxError("Unterminated escape sequence");
            }
            String hex = mStringPool.get(mBuffer, pos, 4);
            pos += 4;
            return (char) Integer.parseInt(hex, 16);

        case 't':
            return '\t';

        case 'b':
            return '\b';

        case 'n':
            return '\n';

        case 'r':
            return '\r';

        case 'f':
            return '\f';

        case '\'':
        case '"':
        case '\\':
        default:
            return escaped;
    }
}

/**
 * Reads a null, Boolean, numeric or unquoted string literal value.
 */
private JsonToken readLiteral() throws IOException {
    value = nextLiteral(true);
    if (valueLength == 0) {
        throw syntaxError("Expected literal value");
    }
    token = decodeLiteral();
    if (token == JsonToken.STRING) {
      checkLenient();
    }
    return token;
}

/**
 * Assigns {@code nextToken} based on the value of {@code nextValue}.
 */
private JsonToken decodeLiteral() throws IOException {
    if (valuePos == -1) {
        // it was too long to fit in the buffer so it can only be a string
        return JsonToken.STRING;
    } else if (valueLength == 4
            && ('n' == (*mBuffer)[valuePos    ] || 'N' == (*mBuffer)[valuePos    ])
            && ('u' == (*mBuffer)[valuePos + 1] || 'U' == (*mBuffer)[valuePos + 1])
            && ('l' == (*mBuffer)[valuePos + 2] || 'L' == (*mBuffer)[valuePos + 2])
            && ('l' == (*mBuffer)[valuePos + 3] || 'L' == (*mBuffer)[valuePos + 3])) {
        value = "null";
        return JsonToken.NULL;
    } else if (valueLength == 4
            && ('t' == (*mBuffer)[valuePos    ] || 'T' == (*mBuffer)[valuePos    ])
            && ('r' == (*mBuffer)[valuePos + 1] || 'R' == (*mBuffer)[valuePos + 1])
            && ('u' == (*mBuffer)[valuePos + 2] || 'U' == (*mBuffer)[valuePos + 2])
            && ('e' == (*mBuffer)[valuePos + 3] || 'E' == (*mBuffer)[valuePos + 3])) {
        value = TRUE;
        return JsonToken.BOOLEAN;
    } else if (valueLength == 5
            && ('f' == (*mBuffer)[valuePos    ] || 'F' == (*mBuffer)[valuePos    ])
            && ('a' == (*mBuffer)[valuePos + 1] || 'A' == (*mBuffer)[valuePos + 1])
            && ('l' == (*mBuffer)[valuePos + 2] || 'L' == (*mBuffer)[valuePos + 2])
            && ('s' == (*mBuffer)[valuePos + 3] || 'S' == (*mBuffer)[valuePos + 3])
            && ('e' == (*mBuffer)[valuePos + 4] || 'E' == (*mBuffer)[valuePos + 4])) {
        value = FALSE;
        return JsonToken.BOOLEAN;
    } else {
        value = mStringPool.get(mBuffer, valuePos, valueLength);
        return decodeNumber(mBuffer, valuePos, valueLength);
    }
}

/**
 * Determine whether the characters is a JSON number. Numbers are of the
 * form -12.34e+56. Fractional and exponential parts are optional. Leading
 * zeroes are not allowed in the value or exponential part, but are allowed
 * in the fraction.
 */
private JsonToken decodeNumber(char[] chars, int offset, int length) {
    int i = offset;
    int c = chars[i];

    if (c == '-') {
        c = chars[++i];
    }

    if (c == '0') {
        c = chars[++i];
    } else if (c >= '1' && c <= '9') {
        c = chars[++i];
        while (c >= '0' && c <= '9') {
            c = chars[++i];
        }
    } else {
        return JsonToken.STRING;
    }

    if (c == '.') {
        c = chars[++i];
        while (c >= '0' && c <= '9') {
            c = chars[++i];
        }
    }

    if (c == 'e' || c == 'E') {
        c = chars[++i];
        if (c == '+' || c == '-') {
            c = chars[++i];
        }
        if (c >= '0' && c <= '9') {
            c = chars[++i];
            while (c >= '0' && c <= '9') {
                c = chars[++i];
            }
        } else {
            return JsonToken.STRING;
        }
    }

    if (i == offset + length) {
        return JsonToken.NUMBER;
    } else {
        return JsonToken.STRING;
    }
}

/**
 * Throws a new IO exception with the given message and a context snippet
 * with this reader's content.
 */
private IOException syntaxError(String message) throws IOException {
    throw new MalformedJsonException(message
            + " at line " + getLineNumber() + " column " + getColumnNumber());
}

private CharSequence getSnippet() {
    StringBuilder snippet = new StringBuilder();
    int beforePos = Math.min(pos, 20);
    snippet.append(mBuffer, pos - beforePos, beforePos);
    int afterPos = Math.min(limit - pos, 20);
    snippet.append(mBuffer, pos, afterPos);
    return snippet;
}

}
}
}
