#ifndef __ELASTOS_DROID_UTILITY_CJSONWRITER_H__
#define __ELASTOS_DROID_UTILITY_CJSONWRITER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Utility_CJsonWriter.h"
#include "elastos/core/Object.h"

using Elastos::Core::INumber;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Writes a JSON (<a href="http://www.ietf.org/rfc/rfc4627.txt">RFC 4627</a>)
 * encoded value to a stream, one token at a time. The stream includes both
 * literal values (strings, numbers, booleans and nulls) as well as the begin
 * and end delimiters of objects and arrays.
 *
 * <h3>Encoding JSON</h3>
 * To encode your data as JSON, create a new {@code JsonWriter}. Each JSON
 * document must contain one top-level array or object. Call methods on the
 * writer as you walk the structure's contents, nesting arrays and objects as
 * necessary:
 * <ul>
 *   <li>To write <strong>arrays</strong>, first call {@link #beginArray()}.
 *       Write each of the array's elements with the appropriate {@link #value}
 *       methods or by nesting other arrays and objects. Finally close the array
 *       using {@link #endArray()}.
 *   <li>To write <strong>objects</strong>, first call {@link #beginObject()}.
 *       Write each of the object's properties by alternating calls to
 *       {@link #name} with the property's value. Write property values with the
 *       appropriate {@link #value} method or by nesting other objects or arrays.
 *       Finally close the object using {@link #endObject()}.
 * </ul>
 *
 * <h3>Example</h3>
 * Suppose we'd like to encode a stream of messages such as the following: <pre> {@code
 * [
 *   {
 *     "id": 912345678901,
 *     "text": "How do I write JSON on Android?",
 *     "geo": null,
 *     "user": {
 *       "name": "android_newb",
 *       "followers_count": 41
 *      }
 *   },
 *   {
 *     "id": 912345678902,
 *     "text": "@android_newb just use android.util.JsonWriter!",
 *     "geo": [50.454722, -104.606667],
 *     "user": {
 *       "name": "jesse",
 *       "followers_count": 2
 *     }
 *   }
 * ]}</pre>
 * This code encodes the above structure: <pre>   {@code
 *   public void writeJsonStream(OutputStream out, List<Message> messages) throws IOException {
 *     JsonWriter writer = new JsonWriter(new OutputStreamWriter(out, "UTF-8"));
 *     writer.setIndent("  ");
 *     writeMessagesArray(writer, messages);
 *     writer.close();
 *   }
 *
 *   public void writeMessagesArray(JsonWriter writer, List<Message> messages) throws IOException {
 *     writer.beginArray();
 *     for (Message message : messages) {
 *       writeMessage(writer, message);
 *     }
 *     writer.endArray();
 *   }
 *
 *   public void writeMessage(JsonWriter writer, Message message) throws IOException {
 *     writer.beginObject();
 *     writer.name("id").value(message.getId());
 *     writer.name("text").value(message.getText());
 *     if (message.getGeo() != null) {
 *       writer.name("geo");
 *       writeDoublesArray(writer, message.getGeo());
 *     } else {
 *       writer.name("geo").nullValue();
 *     }
 *     writer.name("user");
 *     writeUser(writer, message.getUser());
 *     writer.endObject();
 *   }
 *
 *   public void writeUser(JsonWriter writer, User user) throws IOException {
 *     writer.beginObject();
 *     writer.name("name").value(user.getName());
 *     writer.name("followers_count").value(user.getFollowersCount());
 *     writer.endObject();
 *   }
 *
 *   public void writeDoublesArray(JsonWriter writer, List<Double> doubles) throws IOException {
 *     writer.beginArray();
 *     for (Double value : doubles) {
 *       writer.value(value);
 *     }
 *     writer.endArray();
 *   }}</pre>
 *
 * <p>Each {@code JsonWriter} may be used to write a single JSON stream.
 * Instances of this class are not thread safe. Calls that would result in a
 * malformed JSON string will fail with an {@link IllegalStateException}.
 */
CarClass(CJsonWriter)
    , public Object
    , public IJsonWriter
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJsonWriter();

    ~CJsonWriter();

    /**
     * Creates a new instance that writes a JSON-encoded stream to {@code out}.
     * For best performance, ensure {@link Writer} is buffered; wrapping in
     * {@link java.io.BufferedWriter BufferedWriter} if necessary.
     */
    CARAPI constructor(
        /* [in] */ IWriter* out);

    /**
     * Sets the indentation string to be repeated for each level of indentation
     * in the encoded document. If {@code indent.isEmpty()} the encoded document
     * will be compact. Otherwise the encoded document will be more
     * human-readable.
     *
     * @param indent a string containing only whitespace.
     */
    CARAPI SetIndent(
        /* [in] */ const String& indent);

    /**
     * Configure this writer to relax its syntax rules. By default, this writer
     * only emits well-formed JSON as specified by <a
     * href="http://www.ietf.org/rfc/rfc4627.txt">RFC 4627</a>. Setting the writer
     * to lenient permits the following:
     * <ul>
     *   <li>Top-level values of any type. With strict writing, the top-level
     *       value must be an object or an array.
     *   <li>Numbers may be {@link Double#isNaN() NaNs} or {@link
     *       Double#isInfinite() infinities}.
     * </ul>
     */
    CARAPI SetLenient(
        /* [in] */ Boolean lenient);

    /**
     * Returns true if this writer has relaxed syntax rules.
     */
    CARAPI IsLenient(
        /* [out] */ Boolean* res);

    /**
     * Begins encoding a new array. Each call to this method must be paired with
     * a call to {@link #endArray}.
     *
     * @return this writer.
     */
    CARAPI BeginArray();

    /**
     * Ends encoding the current array.
     *
     * @return this writer.
     */
    CARAPI EndArray();

    /**
     * Begins encoding a new object. Each call to this method must be paired
     * with a call to {@link #endObject}.
     *
     * @return this writer.
     */
    CARAPI BeginObject();

    /**
     * Ends encoding the current object.
     *
     * @return this writer.
     */
    CARAPI EndObject();

    /**
     * Encodes the property name.
     *
     * @param name the name of the forthcoming value. May not be null.
     * @return this writer.
     */
    CARAPI Name(
        /* [in] */ const String& name);

    /**
     * Encodes {@code value}.
     *
     * @param value the literal string value, or null to encode a null literal.
     * @return this writer.
     */
    CARAPI Value(
        /* [in] */ const String& value);

    /**
     * Encodes {@code null}.
     *
     * @return this writer.
     */
    CARAPI NullValue();

    /**
     * Encodes {@code value}.
     *
     * @return this writer.
     */
    CARAPI Value(
        /* [in] */ Boolean value);

    /**
     * Encodes {@code value}.
     *
     * @param value a finite value. May not be {@link Double#isNaN() NaNs} or
     *     {@link Double#isInfinite() infinities} unless this writer is lenient.
     * @return this writer.
     */
    CARAPI Value(
        /* [in] */ Double value);

    /**
     * Encodes {@code value}.
     *
     * @return this writer.
     */
    CARAPI Value(
        /* [in] */ Int64 value);

    /**
     * Encodes {@code value}.
     *
     * @param value a finite value. May not be {@link Double#isNaN() NaNs} or
     *     {@link Double#isInfinite() infinities} unless this writer is lenient.
     * @return this writer.
     */
    CARAPI Value(
        /* [in] */ INumber* value);

    /**
     * Ensures all buffered data is written to the underlying {@link Writer}
     * and flushes that writer.
     */
    CARAPI Flush();

    /**
     * Flushes and closes this writer and the underlying {@link Writer}.
     *
     * @throws IOException if the JSON document is incomplete.
     */
    CARAPI Close();

private:
    /**
     * Enters a new scope by appending any necessary whitespace and the given
     * bracket.
     */
    CARAPI Open(
        /* [in] */ JsonScope empty,
        /* [in] */ const String& openBracket);

    /**
     * Closes the current scope by appending any necessary whitespace and the
     * given bracket.
     */
    CARAPI Close(
        /* [in] */ JsonScope empty,
        /* [in] */ JsonScope nonempty,
        /* [in] */ String closeBracket);

    /**
     * Returns the value on the top of the stack.
     */
    CARAPI_(JsonScope) Peek();

    /**
     * Replace the value on the top of the stack with the given value.
     */
    CARAPI_(void) ReplaceTop(
        /* [in] */ JsonScope topOfStack);

    CARAPI StringImpl(
        /* [in] */ const String& value);

    CARAPI Newline();

    /**
     * Inserts any necessary separators and whitespace before a name. Also
     * adjusts the stack to expect the name's value.
     */
    CARAPI BeforeName();

    /**
     * Inserts any necessary separators and whitespace before a literal value,
     * inline array, or inline object. Also adjusts the stack to expect either a
     * closing bracket or another element.
     *
     * @param root true if the value is a new array or object, the two values
     *     permitted as top-level elements.
     */
    CARAPI BeforeValue(
        /* [in] */ Boolean root);

private:
    /** The output data, containing at most one top-level array or object. */
    AutoPtr<IWriter> mOut;

    // List<JsonScope> mStack = new ArrayList<JsonScope>();
    // {
    //     mStack.add(JsonScope.EMPTY_DOCUMENT);
    // }
    AutoPtr<IList> mStack;

    /**
     * A string containing a full set of spaces for a single level of
     * indentation, or null for no pretty printing.
     */
    String mIndent;

    /**
     * The name/value separator; either ":" or ": ".
     */
    String mSeparator;

    Boolean mLenient;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_CJSONWRITER_H__
