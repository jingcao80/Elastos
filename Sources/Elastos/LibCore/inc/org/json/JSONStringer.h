//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_JSON_JSONSTRINGER_H__
#define __ORG_JSON_JSONSTRINGER_H__

#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Json {

// Note: this class was written without inspecting the non-free org.json sourcecode.

/**
 * Implements {@link JSONObject#toString} and {@link JSONArray#toString}. Most
 * application developers should use those methods directly and disregard this
 * API. For example:<pre>
 * JSONObject object = ...
 * String json = object.toString();</pre>
 *
 * <p>Stringers only encode well-formed JSON strings. In particular:
 * <ul>
 *   <li>The stringer must have exactly one top-level array or object.
 *   <li>Lexical scopes must be balanced: every call to {@link #array} must
 *       have a matching call to {@link #endArray} and every call to {@link
 *       #object} must have a matching call to {@link #endObject}.
 *   <li>Arrays may not contain keys (property names).
 *   <li>Objects must alternate keys (property names) and values.
 *   <li>Values are inserted with either literal {@link #value(Object) value}
 *       calls, or by nesting arrays or objects.
 * </ul>
 * Calls that would result in a malformed JSON string will fail with a
 * {@link JSONException}.
 *
 * <p>This class provides no facility for pretty-printing (ie. indenting)
 * output. To encode indented output, use {@link JSONObject#toString(int)} or
 * {@link JSONArray#toString(int)}.
 *
 * <p>Some implementations of the API support at most 20 levels of nesting.
 * Attempts to create more than 20 levels of nesting may fail with a {@link
 * JSONException}.
 *
 * <p>Each stringer may be used to encode a single top level value. Instances of
 * this class are not thread safe. Although this class is nonfinal, it was not
 * designed for inheritance and should not be subclassed. In particular,
 * self-use by overrideable methods is not specified. See <i>Effective Java</i>
 * Item 17, "Design and Document or inheritance or else prohibit it" for further
 * information.
 */

class JSONStringer
    : public Object
    , public IJSONStringer
{
    friend class JSONArray;
public:
    CAR_INTERFACE_DECL()

    JSONStringer();

    ~JSONStringer();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 indentSpaces);

    /**
     * Begins encoding a new array. Each call to this method must be paired with
     * a call to {@link #endArray}.
     *
     * @return this stringer.
     */
    CARAPI Array();

    /**
     * Ends encoding the current array.
     *
     * @return this stringer.
     */
    CARAPI EndArray();

    /**
     * Begins encoding a new object. Each call to this method must be paired
     * with a call to {@link #endObject}.
     *
     * @return this stringer.
     */
    CARAPI Object();

    /**
     * Ends encoding the current object.
     *
     * @return this stringer.
     */
    CARAPI EndObject();

    /**
     * Enters a new scope by appending any necessary whitespace and the given
     * bracket.
     */
    CARAPI Open(
        /* [in] */ JSONStringerScope empty,
        /* [in] */ const String& openBracket);

    /**
     * Closes the current scope by appending any necessary whitespace and the
     * given bracket.
     */
    CARAPI Close(
        /* [in] */ JSONStringerScope empty,
        /* [in] */ JSONStringerScope nonempty,
        /* [in] */ const String& closeBracket);

    /**
     * Encodes {@code value}.
     *
     * @param value a {@link JSONObject}, {@link JSONArray}, String, Boolean,
     *     Integer, Long, Double or null. May not be {@link Double#isNaN() NaNs}
     *     or {@link Double#isInfinite() infinities}.
     * @return this stringer.
     */
    CARAPI Value(
        /* [in] */ IInterface* value);

    /**
     * Encodes {@code value} to this stringer.
     *
     * @return this stringer.
     */
    CARAPI Value(
        /* [in] */ Boolean value);

    /**
     * Encodes {@code value} to this stringer.
     *
     * @param value a finite value. May not be {@link Double#isNaN() NaNs} or
     *     {@link Double#isInfinite() infinities}.
     * @return this stringer.
     */
    CARAPI Value(
        /* [in] */ Double value);

    /**
     * Encodes {@code value} to this stringer.
     *
     * @return this stringer.
     */
    CARAPI Value(
        /* [in] */ Int64 value);

    /**
     * Encodes the key (property name) to this stringer.
     *
     * @param name the name of the forthcoming value. May not be null.
     * @return this stringer.
     */
    CARAPI Key(
        /* [in] */ const String& name);

    /**
     * Returns the encoded JSON string.
     *
     * <p>If invoked with unterminated arrays or unclosed objects, this method's
     * return value is undefined.
     *
     * <p><strong>Warning:</strong> although it contradicts the general contract
     * of {@link Object#toString}, this method returns null if the stringer
     * contains no data.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Returns the value on the top of the stack.
     */
    CARAPI Peek(
        /* [out] */ JSONStringerScope* scope);

    /**
     * Replace the value on the top of the stack with the given value.
     */
    CARAPI ReplaceTop(
        /* [in] */ JSONStringerScope topOfStack);

    CARAPI_(void) AppendString(
        /* [in] */ const String& value);

    CARAPI_(void) Newline();

    /**
     * Inserts any necessary separators and whitespace before a name. Also
     * adjusts the stack to expect the key's value.
     */
    CARAPI BeforeKey();

    /**
     * Inserts any necessary separators and whitespace before a literal value,
     * inline array, or inline object. Also adjusts the stack to expect either a
     * closing bracket or another element.
     */
    CARAPI BeforeValue();

protected:
    /** The output data, containing at most one top-level array or object. */
    AutoPtr<StringBuilder> mOut;

private:
    /**
     * Unlike the original implementation, this stack isn't limited to 20
     * levels of nesting.
     */
    AutoPtr<IList> mStack;

    /**
     * A string containing a full set of spaces for a single level of
     * indentation, or null for no pretty printing.
     */
    String mIndent;
};

} //namespace Json
} //namespace Org

#endif //__ORG_JSON_JSONSTRINGER_H__
