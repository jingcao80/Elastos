#ifndef __ELASTOS_DROID_UTILITY_CJSONREADER_H__
#define __ELASTOS_DROID_UTILITY_CJSONREADER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Utility_CJsonReader.h"
#include "elastos/core/Object.h"
#include <elastos/utility/StringPool.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::Utility::IList;
using Elastos::Utility::StringPool;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Reads a JSON (<a href="http://www.ietf.org/rfc/rfc4627.txt">RFC 4627</a>)
 * encoded value as a stream of tokens. This stream includes both literal
 * values (strings, numbers, booleans, and nulls) as well as the begin and
 * end delimiters of objects and arrays. The tokens are traversed in
 * depth-first order, the same order that they appear in the JSON document.
 * Within JSON objects, name/value pairs are represented by a single token.
 *
 * <h3>Parsing JSON</h3>
 * To create a recursive descent parser for your own JSON streams, first create
 * an entry point method that creates a {@code JsonReader}.
 *
 * <p>Next, create handler methods for each structure in your JSON text. You'll
 * need a method for each object type and for each array type.
 * <ul>
 *   <li>Within <strong>array handling</strong> methods, first call {@link
 *       #beginArray} to consume the array's opening bracket. Then create a
 *       while loop that accumulates values, terminating when {@link #hasNext}
 *       is false. Finally, read the array's closing bracket by calling {@link
 *       #endArray}.
 *   <li>Within <strong>object handling</strong> methods, first call {@link
 *       #beginObject} to consume the object's opening brace. Then create a
 *       while loop that assigns values to local variables based on their name.
 *       This loop should terminate when {@link #hasNext} is false. Finally,
 *       read the object's closing brace by calling {@link #endObject}.
 * </ul>
 * <p>When a nested object or array is encountered, delegate to the
 * corresponding handler method.
 *
 * <p>When an unknown name is encountered, strict parsers should fail with an
 * exception. Lenient parsers should call {@link #skipValue()} to recursively
 * skip the value's nested tokens, which may otherwise conflict.
 *
 * <p>If a value may be null, you should first check using {@link #peek()}.
 * Null literals can be consumed using either {@link #nextNull()} or {@link
 * #skipValue()}.
 *
 * <h3>Example</h3>
 * Suppose we'd like to parse a stream of messages such as the following: <pre> {@code
 * [
 *   {
 *     "id": 912345678901,
 *     "text": "How do I read JSON on Android?",
 *     "geo": null,
 *     "user": {
 *       "name": "android_newb",
 *       "followers_count": 41
 *      }
 *   },
 *   {
 *     "id": 912345678902,
 *     "text": "@android_newb just use android.util.JsonReader!",
 *     "geo": [50.454722, -104.606667],
 *     "user": {
 *       "name": "jesse",
 *       "followers_count": 2
 *     }
 *   }
 * ]}</pre>
 * This code implements the parser for the above structure: <pre>   {@code
 *
 *   public List<Message> readJsonStream(InputStream in) throws IOException {
 *     JsonReader reader = new JsonReader(new InputStreamReader(in, "UTF-8"));
 *     try {
 *       return readMessagesArray(reader);
 *     } finally {
 *       reader.close();
 *     }
 *   }
 *
 *   public List<Message> readMessagesArray(JsonReader reader) throws IOException {
 *     List<Message> messages = new ArrayList<Message>();
 *
 *     reader.beginArray();
 *     while (reader.hasNext()) {
 *       messages.add(readMessage(reader));
 *     }
 *     reader.endArray();
 *     return messages;
 *   }
 *
 *   public Message readMessage(JsonReader reader) throws IOException {
 *     long id = -1;
 *     String text = null;
 *     User user = null;
 *     List<Double> geo = null;
 *
 *     reader.beginObject();
 *     while (reader.hasNext()) {
 *       String name = reader.nextName();
 *       if (name.equals("id")) {
 *         id = reader.nextLong();
 *       } else if (name.equals("text")) {
 *         text = reader.nextString();
 *       } else if (name.equals("geo") && reader.peek() != JsonToken.NULL) {
 *         geo = readDoublesArray(reader);
 *       } else if (name.equals("user")) {
 *         user = readUser(reader);
 *       } else {
 *         reader.skipValue();
 *       }
 *     }
 *     reader.endObject();
 *     return new Message(id, text, user, geo);
 *   }
 *
 *   public List<Double> readDoublesArray(JsonReader reader) throws IOException {
 *     List<Double> doubles = new ArrayList<Double>();
 *
 *     reader.beginArray();
 *     while (reader.hasNext()) {
 *       doubles.add(reader.nextDouble());
 *     }
 *     reader.endArray();
 *     return doubles;
 *   }
 *
 *   public User readUser(JsonReader reader) throws IOException {
 *     String username = null;
 *     int followersCount = -1;
 *
 *     reader.beginObject();
 *     while (reader.hasNext()) {
 *       String name = reader.nextName();
 *       if (name.equals("name")) {
 *         username = reader.nextString();
 *       } else if (name.equals("followers_count")) {
 *         followersCount = reader.nextInt();
 *       } else {
 *         reader.skipValue();
 *       }
 *     }
 *     reader.endObject();
 *     return new User(username, followersCount);
 *   }}</pre>
 *
 * <h3>Number Handling</h3>
 * This reader permits numeric values to be read as strings and string values to
 * be read as numbers. For example, both elements of the JSON array {@code
 * [1, "1"]} may be read using either {@link #nextInt} or {@link #nextString}.
 * This behavior is intended to prevent lossy numeric conversions: double is
 * JavaScript's only numeric type and very large values like {@code
 * 9007199254740993} cannot be represented exactly on that platform. To minimize
 * precision loss, extremely large values should be written and read as strings
 * in JSON.
 *
 * <p>Each {@code JsonReader} may be used to read a single JSON stream. Instances
 * of this class are not thread safe.
 */
CarClass(CJsonReader)
    , public Object
    , public IJsonReader
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJsonReader();

    ~CJsonReader();

    /**
     * Creates a new instance that reads a JSON-encoded stream from {@code in}.
     */
    CARAPI constructor(
        /* [in] */ IReader* in);

    /**
     * Configure this parser to be  be liberal in what it accepts. By default,
     * this parser is strict and only accepts JSON as specified by <a
     * href="http://www.ietf.org/rfc/rfc4627.txt">RFC 4627</a>. Setting the
     * parser to lenient causes it to ignore the following syntax errors:
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
    CARAPI SetLenient(
        /* [in] */ Boolean lenient);

    /**
     * Returns true if this parser is liberal in what it accepts.
     */
    CARAPI IsLenient(
        /* [out] */ Boolean* res);

    /**
     * Consumes the next token from the JSON stream and asserts that it is the
     * beginning of a new array.
     */
    CARAPI BeginArray();

    /**
     * Consumes the next token from the JSON stream and asserts that it is the
     * end of the current array.
     */
    CARAPI EndArray();

    /**
     * Consumes the next token from the JSON stream and asserts that it is the
     * beginning of a new object.
     */
    CARAPI BeginObject();

    /**
     * Consumes the next token from the JSON stream and asserts that it is the
     * end of the current array.
     */
    CARAPI EndObject();

    /**
     * Returns true if the current array or object has another element.
     */
    CARAPI HasNext(
        /* [out] */ Boolean* res);

    /**
     * Returns the type of the next token without consuming it.
     */
    CARAPI Peek(
        /* [out] */ JsonToken* result);

    /**
     * Returns the next token, a {@link JsonToken#NAME property name}, and
     * consumes it.
     *
     * @throws IOException if the next token in the stream is not a property
     *     name.
     */
    CARAPI NextName(
        /* [out] */ String* str);

    /**
     * Returns the {@link JsonToken#STRING string} value of the next token,
     * consuming it. If the next token is a number, this method will return its
     * string form.
     *
     * @throws IllegalStateException if the next token is not a string or if
     *     this reader is closed.
     */
    CARAPI NextString(
        /* [out] */ String* str);

    /**
     * Returns the {@link JsonToken#BOOLEAN boolean} value of the next token,
     * consuming it.
     *
     * @throws IllegalStateException if the next token is not a boolean or if
     *     this reader is closed.
     */
    CARAPI NextBoolean(
        /* [out] */ Boolean* res);

    /**
     * Consumes the next token from the JSON stream and asserts that it is a
     * literal null.
     *
     * @throws IllegalStateException if the next token is not null or if this
     *     reader is closed.
     */
    CARAPI NextNull();

    /**
     * Returns the {@link JsonToken#NUMBER double} value of the next token,
     * consuming it. If the next token is a string, this method will attempt to
     * parse it as a double using {@link Double#parseDouble(String)}.
     *
     * @throws IllegalStateException if the next token is not a literal value.
     */
    CARAPI NextDouble(
        /* [out] */ Double* data);

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
    CARAPI NextLong(
        /* [out] */ Int64* data);

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
    CARAPI NextInt(
        /* [out] */ Int32* data);

    /**
     * Closes this JSON reader and the underlying {@link Reader}.
     */
    CARAPI Close();

    /**
     * Skips the next value recursively. If it is an object or array, all nested
     * elements are skipped. This method is intended for use when the JSON token
     * stream contains unrecognized or unhandled values.
     */
    CARAPI SkipValue();

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Consumes {@code expected}.
     */
    CARAPI Expect(
        /* [in] */ JsonToken expected);

    /**
     * Advances the cursor in the JSON stream to the next token.
     */
    CARAPI Advance(
        /* [out] */ JsonToken* result);

    CARAPI_(JsonScope) PeekStack();

    CARAPI_(JsonScope) Pop();

    CARAPI_(void) Push(
        /* [in] */ JsonScope newTop);

    /**
     * Replace the value on the top of the stack with the given value.
     */
    CARAPI_(void) ReplaceTop(
        /* [in] */ JsonScope newTop);

    CARAPI NextInArray(
        /* [in] */ Boolean firstElement,
        /* [out] */ JsonToken* result);

    CARAPI NextInObject(
        /* [in] */ Boolean firstElement,
        /* [out] */ JsonToken* result);

    CARAPI ObjectValue(
        /* [out] */ JsonToken* result);

    CARAPI NextValue(
        /* [out] */ JsonToken* result);

    /**
     * Returns true once {@code limit - pos >= minimum}. If the data is
     * exhausted before that many characters are available, this returns
     * false.
     */
    CARAPI FillBuffer(
        /* [in] */ Int32 minimum,
        /* [out] */ Boolean* res);

    CARAPI_(Int32) GetLineNumber();

    CARAPI_(Int32) GetColumnNumber();

    CARAPI NextNonWhitespace(
        /* [out] */ Int32* result);

    CARAPI CheckLenient();

    /**
     * Advances the position until after the next newline character. If the line
     * is terminated by "\r\n", the '\n' must be consumed as whitespace by the
     * caller.
     */
    CARAPI SkipToEndOfLine();

    CARAPI SkipTo(
        /* [in] */ const String& toFind,
        /* [out] */ Boolean* result);

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
    CARAPI NextString(
        /* [in] */ Char32 quote,
        /* [out] */ String* str);

    /**
     * Reads the value up to but not including any delimiter characters. This
     * does not consume the delimiter character.
     *
     * @param assignOffsetsOnly true for this method to only set the valuePos
     *     and valueLength fields and return a null result. This only works if
     *     the literal is short; a string is returned otherwise.
     */
    CARAPI NextLiteral(
        /* [in] */ Boolean assignOffsetsOnly,
        /* [out] */ String* str);

    /**
     * Unescapes the character identified by the character or characters that
     * immediately follow a backslash. The backslash '\' should have already
     * been read. This supports both unicode escapes "u000A" and two-character
     * escapes "\n".
     *
     * @throws NumberFormatException if any unicode escape sequences are
     *     malformed.
     */
    CARAPI ReadEscapeCharacter(
        /* [out] */ Char32* ch);

    /**
     * Reads a null, boolean, numeric or unquoted string literal value.
     */
    CARAPI ReadLiteral(
        /* [out] */ JsonToken* result);

    /**
     * Assigns {@code nextToken} based on the value of {@code NextValue}.
     */
    CARAPI DecodeLiteral(
        /* [out] */ JsonToken* result);

    /**
     * Determine whether the characters is a JSON number. Numbers are of the
     * form -12.34e+56. Fractional and exponential parts are optional. Leading
     * zeroes are not allowed in the value or exponential part, but are allowed
     * in the fraction.
     */
    CARAPI_(JsonToken) DecodeNumber(
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Throws a new IO exception with the given message and a context snippet
     * with this reader's content.
     */
    CARAPI SyntaxError(
        /* [in] */ const String& message);

    CARAPI_(AutoPtr<ICharSequence>) GetSnippet();

private:
    static const String sTRUE;// = "true";
    static const String sFALSE;// = "false";

private:
    AutoPtr<StringPool> mStringPool;

    /** The input JSON. */
    AutoPtr<IReader> mIn;

    /** True to accept non-spec compliant JSON */
    Boolean mLenient;// = false;

    /**
     * Use a manual buffer to easily read and unread upcoming characters, and
     * also so we can create strings without an intermediate StringBuilder.
     * We decode literals directly out of this buffer, so it must be at least as
     * long as the longest token that can be reported as a number.
     */
    AutoPtr<ArrayOf<Char32> > mBuffer;// = new char[1024];
    Int32 mPos;
    Int32 mLimit;

    /*
     * The offset of the first character in the buffer.
     */
    Int32 mBufferStartLine;// = 1;
    Int32 mBufferStartColumn;// = 1;

    AutoPtr<IList> mStack;

    /**
     * The type of the next token to be returned by {@link #peek} and {@link
     * #advance}. If null, peek() will assign a value.
     */
    JsonToken mToken;

    /** The text of the next name. */
    String mName;

    /*
     * For the next literal value, we may have the text value, or the position
     * and length in the buffer.
     */
    String mValue;
    Int32 mValuePos;
    Int32 mValueLength;

    /** True if we're currently handling a skipValue() call. */
    Boolean mSkipping;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CJSONREADER_H__
