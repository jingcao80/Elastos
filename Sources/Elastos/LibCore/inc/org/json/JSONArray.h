
#ifndef __ORG_JSON_JSONARRAY_H__
#define __ORG_JSON_JSONARRAY_H__

#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::IBoolean;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Org {
namespace Json {

// Note: this class was written without inspecting the non-free org.json sourcecode.

/**
 * A dense indexed sequence of values. Values may be any mix of
 * {@link JSONObject JSONObjects}, other {@link JSONArray JSONArrays}, Strings,
 * Booleans, Integers, Longs, Doubles, {@code null} or {@link JSONObject#NULL}.
 * Values may not be {@link Double#isNaN() NaNs}, {@link Double#isInfinite()
 * infinities}, or of any type not listed here.
 *
 * <p>{@code JSONArray} has the same type coercion behavior and
 * optional/mandatory accessors as {@link JSONObject}. See that class'
 * documentation for details.
 *
 * <p><strong>Warning:</strong> this class represents null in two incompatible
 * ways: the standard Java {@code null} reference, and the sentinel value {@link
 * JSONObject#NULL}. In particular, {@code get} fails if the requested index
 * holds the null reference, but succeeds if it holds {@code JSONObject.NULL}.
 *
 * <p>Instances of this class are not thread safe. Although this class is
 * nonfinal, it was not designed for inheritance and should not be subclassed.
 * In particular, self-use by overridable methods is not specified. See
 * <i>Effective Java</i> Item 17, "Design and Document or inheritance or else
 * prohibit it" for further information.
 */
class JSONArray
    : public Object
    , public IJSONArray
{
    friend class JSONObject;
public:
    CAR_INTERFACE_DECL();

    JSONArray();

    ~JSONArray();

    /**
     * Creates a {@code JSONArray} with no values.
     */
    CARAPI constructor();

    /**
     * Creates a new {@code JSONArray} by copying all values from the given
     * collection.
     *
     * @param copyFrom a collection whose values are of supported types.
     *     Unsupported values are not permitted and will yield an array in an
     *     inconsistent state.
     */
    /* Accept a raw type for API compatibility */
    CARAPI constructor(
        /* [in] */ ICollection* copyFrom);

    /**
     * Creates a new {@code JSONArray} with values from the next array in the
     * tokener.
     *
     * @param readFrom a tokener whose nextValue() method will yield a
     *     {@code JSONArray}.
     * @throws JSONException if the parse fails or doesn't yield a
     *     {@code JSONArray}.
     */
    CARAPI constructor(
        /* [in] */ IJSONTokener* readFrom);

    /**
     * Creates a new {@code JSONArray} with values from the JSON string.
     *
     * @param json a JSON-encoded string containing an array.
     * @throws JSONException if the parse fails or doesn't yield a {@code
     *     JSONArray}.
     */
    CARAPI constructor(
        /* [in] */ const String& json);

    /**
     * Creates a new {@code JSONArray} with values from the given primitive array.
     */
    CARAPI constructor(
        /* [in] */ IInterface* array);

    /**
     * Returns the number of values in this array.
     */
    CARAPI GetLength(
        /* [out] */ Int32* len);

    /**
     * Appends {@code value} to the end of this array.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Boolean value);

    /**
     * Appends {@code value} to the end of this array.
     *
     * @param value a finite value. May not be {@link Double#isNaN() NaNs} or
     *     {@link Double#isInfinite() infinities}.
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Double value);

    /**
     * Appends {@code value} to the end of this array.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 value);

    /**
     * Appends {@code value} to the end of this array.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int64 value);

    /**
     * Appends {@code value} to the end of this array.
     *
     * @param value a {@link JSONObject}, {@link JSONArray}, String, Boolean,
     *     Integer, Long, Double, {@link JSONObject#NULL}, or {@code null}. May
     *     not be {@link Double#isNaN() NaNs} or {@link Double#isInfinite()
     *     infinities}. Unsupported values are not permitted and will cause the
     *     array to be in an inconsistent state.
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ IInterface* value);

    /**
     * Sets the value at {@code index} to {@code value}, null padding this array
     * to the required length if necessary. If a value already exists at {@code
     * index}, it will be replaced.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Boolean value);

    /**
     * Sets the value at {@code index} to {@code value}, null padding this array
     * to the required length if necessary. If a value already exists at {@code
     * index}, it will be replaced.
     *
     * @param value a finite value. May not be {@link Double#isNaN() NaNs} or
     *     {@link Double#isInfinite() infinities}.
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    /**
     * Sets the value at {@code index} to {@code value}, null padding this array
     * to the required length if necessary. If a value already exists at {@code
     * index}, it will be replaced.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int32 value);

    /**
     * Sets the value at {@code index} to {@code value}, null padding this array
     * to the required length if necessary. If a value already exists at {@code
     * index}, it will be replaced.
     *
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    /**
     * Sets the value at {@code index} to {@code value}, null padding this array
     * to the required length if necessary. If a value already exists at {@code
     * index}, it will be replaced.
     *
     * @param value a {@link JSONObject}, {@link JSONArray}, String, Boolean,
     *     Integer, Long, Double, {@link JSONObject#NULL}, or {@code null}. May
     *     not be {@link Double#isNaN() NaNs} or {@link Double#isInfinite()
     *     infinities}.
     * @return this array.
     */
    CARAPI Put(
        /* [in] */ Int32 index,
        /* [in] */ IInterface* value);

    /**
     * Returns true if this array has no value at {@code index}, or if its value
     * is the {@code null} reference or {@link JSONObject#NULL}.
     */
    CARAPI IsNull(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* res);

    /**
     * Returns the value at {@code index}.
     *
     * @throws JSONException if this array has no value at {@code index}, or if
     *     that value is the {@code null} reference. This method returns
     *     normally if the value is {@code JSONObject#NULL}.
     */
    CARAPI Get(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** obj);

    /**
     * Returns the value at {@code index}, or null if the array has no value
     * at {@code index}.
     */
    CARAPI Opt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** obj);

    /**
     * Removes and returns the value at {@code index}, or null if the array has no value
     * at {@code index}.
     */
    CARAPI Remove(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** obj);

    /**
     * Returns the value at {@code index} if it exists and is a boolean or can
     * be coerced to a boolean.
     *
     * @throws JSONException if the value at {@code index} doesn't exist or
     *     cannot be coerced to a boolean.
     */
    CARAPI GetBoolean(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* res);

    /**
     * Returns the value at {@code index} if it exists and is a boolean or can
     * be coerced to a boolean. Returns false otherwise.
     */
    CARAPI OptBoolean(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* res);

    /**
     * Returns the value at {@code index} if it exists and is a boolean or can
     * be coerced to a boolean. Returns {@code fallback} otherwise.
     */
    CARAPI OptBoolean(
        /* [in] */ Int32 index,
        /* [in] */ Boolean fallback,
        /* [out] */ Boolean* res);

    /**
     * Returns the value at {@code index} if it exists and is a double or can
     * be coerced to a double.
     *
     * @throws JSONException if the value at {@code index} doesn't exist or
     *     cannot be coerced to a double.
     */
    CARAPI GetDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* data);

    /**
     * Returns the value at {@code index} if it exists and is a double or can
     * be coerced to a double. Returns {@code NaN} otherwise.
     */
    CARAPI OptDouble(
        /* [in] */ Int32 index,
        /* [out] */ Double* data);

    /**
     * Returns the value at {@code index} if it exists and is a double or can
     * be coerced to a double. Returns {@code fallback} otherwise.
     */
    CARAPI OptDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double fallback,
        /* [out] */ Double* data);

    /**
     * Returns the value at {@code index} if it exists and is an int or
     * can be coerced to an int.
     *
     * @throws JSONException if the value at {@code index} doesn't exist or
     *     cannot be coerced to a int.
     */
    CARAPI GetInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* data);

    /**
     * Returns the value at {@code index} if it exists and is an int or
     * can be coerced to an int. Returns 0 otherwise.
     */
    CARAPI OptInt32(
        /* [in] */ Int32 index,
        /* [out] */ Int32* data);

    /**
     * Returns the value at {@code index} if it exists and is an int or
     * can be coerced to an int. Returns {@code fallback} otherwise.
     */
    CARAPI OptInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 fallback,
        /* [out] */ Int32* data);

    /**
     * Returns the value at {@code index} if it exists and is a long or
     * can be coerced to a long.
     *
     * @throws JSONException if the value at {@code index} doesn't exist or
     *     cannot be coerced to a long.
     */
    CARAPI GetInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* data);

    /**
     * Returns the value at {@code index} if it exists and is a long or
     * can be coerced to a long. Returns 0 otherwise.
     */
    CARAPI OptInt64(
        /* [in] */ Int32 index,
        /* [out] */ Int64* data);

    /**
     * Returns the value at {@code index} if it exists and is a long or
     * can be coerced to a long. Returns {@code fallback} otherwise.
     */
    CARAPI OptInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 fallback,
        /* [out] */ Int64* data);

    /**
     * Returns the value at {@code index} if it exists, coercing it if
     * necessary.
     *
     * @throws JSONException if no such value exists.
     */
    CARAPI GetString(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    /**
     * Returns the value at {@code index} if it exists, coercing it if
     * necessary. Returns the empty string if no such value exists.
     */
    CARAPI OptString(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    /**
     * Returns the value at {@code index} if it exists, coercing it if
     * necessary. Returns {@code fallback} if no such value exists.
     */
    CARAPI OptString(
        /* [in] */ Int32 index,
        /* [in] */ const String& fallback,
        /* [out] */ String* str);

    /**
     * Returns the value at {@code index} if it exists and is a {@code
     * JSONArray}.
     *
     * @throws JSONException if the value doesn't exist or is not a {@code
     *     JSONArray}.
     */
    CARAPI GetJSONArray(
        /* [in] */ Int32 index,
        /* [out] */ IJSONArray** jsonArray);

    /**
     * Returns the value at {@code index} if it exists and is a {@code
     * JSONArray}. Returns null otherwise.
     */
    CARAPI OptJSONArray(
        /* [in] */ Int32 index,
        /* [out] */ IJSONArray** jsonArray);

    /**
     * Returns the value at {@code index} if it exists and is a {@code
     * JSONObject}.
     *
     * @throws JSONException if the value doesn't exist or is not a {@code
     *     JSONObject}.
     */
    CARAPI GetJSONObject(
        /* [in] */ Int32 index,
        /* [out] */ IJSONObject** jsonObject);

    /**
     * Returns the value at {@code index} if it exists and is a {@code
     * JSONObject}. Returns null otherwise.
     */
    CARAPI OptJSONObject(
        /* [in] */ Int32 index,
        /* [out] */ IJSONObject** jsonObject);

    /**
     * Returns a new object whose values are the values in this array, and whose
     * names are the values in {@code names}. Names and values are paired up by
     * index from 0 through to the shorter array's length. Names that are not
     * strings will be coerced to strings. This method returns null if either
     * array is empty.
     */
    CARAPI ToJSONObject(
        /* [in] */ IJSONArray* names,
        /* [out] */ IJSONObject** jsonObject);

    /**
     * Returns a new string by alternating this array's values with {@code
     * separator}. This array's string values are quoted and have their special
     * characters escaped. For example, the array containing the strings '12"
     * pizza', 'taco' and 'soda' joined on '+' returns this:
     * <pre>"12\" pizza"+"taco"+"soda"</pre>
     */
    CARAPI Join(
        /* [in] */ const String& separator,
        /* [out] */ String* res);

    /**
     * Encodes this array as a compact JSON string, such as:
     * <pre>[94043,90210]</pre>
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Encodes this array as a human readable JSON string for debugging, such
     * as:
     * <pre>
     * [
     *     94043,
     *     90210
     * ]</pre>
     *
     * @param indentSpaces the number of spaces to indent for each level of
     *     nesting.
     */
    CARAPI ToString(
        /* [in] */ Int32 indentSpaces,
        /* [out] */ String* str);

    CARAPI WriteTo(
        /* [in] */ IJSONStringer* stringer);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

protected:
    /**
     * Same as {@link #put}, with added validity checks.
     */
    CARAPI CheckedPut(
        /* [in] */ IInterface* value);

private:
    AutoPtr<IList> mValues;
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_JSONARRAY_H__
