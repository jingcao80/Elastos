#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_TYPEDPROPERTIES_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_TYPEDPROPERTIES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/HashMap.h>

using Elastos::Core::IString;
using Elastos::IO::IReader;
using Elastos::IO::IStreamTokenizer;
using Elastos::Utility::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class TypedProperties
    : public HashMap
    , public ITypedProperties
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an empty TypedProperties instance.
     */
    CARAPI constructor();

    /**
     * Loads zero or more properties from the specified Reader.
     * Properties that have already been loaded are preserved unless
     * the new Reader overrides or unsets earlier values for the
     * same properties.
     * <p>
     * File syntax:
     * <blockquote>
     *     <tt>
     *     &lt;type&gt; &lt;property-name&gt; = &lt;value&gt; ;
     *     <br />
     *     unset ( &lt;property-name&gt; ) ;
     *     </tt>
     *     <p>
     *     "//" comments everything until the end of the line.
     *     "/&#2a;" comments everything until the next appearance of "&#2a;/".
     *     <p>
     *     Blank lines are ignored.
     *     <p>
     *     The only required whitespace is between the type and
     *     the property name.
     *     <p>
     *     &lt;type&gt; is one of {boolean, byte, short, Int32, long,
     *     float, double, String}, and is case-sensitive.
     *     <p>
     *     &lt;property-name&gt; is a valid fully-qualified class name
     *     (one or more valid identifiers separated by dot characters).
     *     <p>
     *     &lt;value&gt; depends on the type:
     *     <ul>
     *     <li> boolean: one of {TRUE, false} (case-sensitive)
     *     <li> byte, short, Int32, long: a valid Java integer constant
     *          (including non-base-10 constants like 0xabc and 074)
     *          whose value does not overflow the type.  NOTE: these are
     *          interpreted as Java integer values, so they are all signed.
     *     <li> float, double: a valid Java floating-point constant.
     *          If the type is float, the value must fit in 32 bits.
     *     <li> String: a double-quoted string value, or the word {@code NULL}.
     *          NOTE: the contents of the string must be 7-bit clean ASCII;
     *          C-style octal escapes are recognized, but Unicode escapes are not.
     *     </ul>
     *     <p>
     *     Passing a property-name to {@code unset()} will unset the property,
     *     removing its value and type information, as if it had never been
     *     defined.
     * </blockquote>
     *
     * @param r The Reader to load properties from
     * @throws IOException if an error occurs when reading the data
     * @throws IllegalArgumentException if the data is malformed
     */
    CARAPI Load(
        /* [in] */ IReader* r);

    // @Override
    CARAPI Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    /*
     * Getters with explicit defaults
     */

    /**
     * Returns the value of a boolean property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a boolean
     */
    CARAPI GetBoolean(
        /* [in] */ const String& property,
        /* [in] */ Boolean def,
        /* [out] */ Boolean* value);

    /**
     * Returns the value of a byte property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a byte
     */
    CARAPI GetByte(
        /* [in] */ const String& property,
        /* [in] */ Byte def,
        /* [out] */ Byte* value);

    /**
     * Returns the value of a short property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a short
     */
    CARAPI GetInt16(
        /* [in] */ const String& property,
        /* [in] */ Int16 def,
        /* [out] */ Int16* value);

    /**
     * Returns the value of an integer property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not an integer
     */
    CARAPI GetInt32(
        /* [in] */ const String& property,
        /* [in] */ Int32 def,
        /* [out] */ Int32* value);

    /**
     * Returns the value of a long property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a long
     */
    CARAPI GetInt64(
        /* [in] */ const String& property,
        /* [in] */ Int64 def,
        /* [out] */ Int64* value);

    /**
     * Returns the value of a float property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a float
     */
    CARAPI GetFloat(
        /* [in] */ const String& property,
        /* [in] */ Float def,
        /* [out] */ Float* value);

    /**
     * Returns the value of a double property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a double
     */
    CARAPI GetDouble(
        /* [in] */ const String& property,
        /* [in] */ Double def,
        /* [out] */ Double* value);

    /**
     * Returns the value of a string property, or the default if the property
     * has not been defined.
     *
     * @param property The name of the property to return
     * @param def The default value to return if the property is not set
     * @return the value of the property
     * @throws TypeException if the property is set and is not a string
     */
    CARAPI GetString(
        /* [in] */ const String& property,
        /* [in] */ const String& def,
        /* [out] */ String* value);

    /*
     * Getters with implicit defaults
     */

    /**
     * Returns the value of a boolean property, or false
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a boolean
     */
    CARAPI GetBoolean(
        /* [in] */ const String& property,
        /* [out] */ Boolean* value);

    /**
     * Returns the value of a byte property, or 0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a byte
     */
    CARAPI GetByte(
        /* [in] */ const String& property,
        /* [out] */ Byte* value);

    /**
     * Returns the value of a short property, or 0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a short
     */
    CARAPI GetInt16(
        /* [in] */ const String& property,
        /* [out] */ Int16* value);

    /**
     * Returns the value of an integer property, or 0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not an integer
     */
    CARAPI GetInt32(
        /* [in] */ const String& property,
        /* [out] */ Int32* value);

    /**
     * Returns the value of a long property, or 0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a long
     */
    CARAPI GetInt64(
        /* [in] */ const String& property,
        /* [out] */ Int64* value);

    /**
     * Returns the value of a float property, or 0.0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a float
     */
    CARAPI GetFloat(
        /* [in] */ const String& property,
        /* [out] */ Float* value);

    /**
     * Returns the value of a double property, or 0.0
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a double
     */
    CARAPI GetDouble(
        /* [in] */ const String& property,
        /* [out] */ Double* value);

    /**
     * Returns the value of a String property, or ""
     * if the property has not been defined.
     *
     * @param property The name of the property to return
     * @return the value of the property
     * @throws TypeException if the property is set and is not a string
     */
    CARAPI GetString(
        /* [in] */ const String& property,
        /* [out] */ String* value);

    /**
     * Provides string type information about a property.
     *
     * @param property the property to check
     * @return STRING_SET if the property is a string and is non-NULL.
     *         STRING_NULL if the property is a string and is NULL.
     *         STRING_NOT_SET if the property is not set (no type or value).
     *         STRING_TYPE_MISMATCH if the property is set but is not a string.
     */
    CARAPI GetStringInfo(
        /* [in] */ const String& property,
        /* [out] */ Int32* info);

    CARAPI Clone(
        /* [out] */ IInterface** object);

public:
    /**
     * Instantiates a {@link java.io.StreamTokenizer} and sets its syntax tables
     * appropriately for the {@code TypedProperties} file format.
     *
     * @param r The {@code Reader} that the {@code StreamTokenizer} will read from
     * @return a newly-created and initialized {@code StreamTokenizer}
     */
    static CARAPI_(AutoPtr<IStreamTokenizer>) InitTokenizer(
        /* [in] */ IReader* r);

    static CARAPI ThrowParseException(
            /* [in] */ IStreamTokenizer* st,
            /* [in] */ const String& expected);

    /**
     * Converts a string to an internal type constant.
     *
     * @param typeName the type name to convert
     * @return the type constant that corresponds to {@code typeName},
     *         or {@code TYPE_ERROR} if the type is unknown
     */
    static CARAPI_(Int32) InterpretType(
        /* [in] */ const String& typeName);

    /**
     * Parses the data in the reader.
     *
     * @param r The {@code Reader} containing input data to parse
     * @param map The {@code Map} to insert parameter values into
     * @throws ParseException if the input data is malformed
     * @throws IOException if there is a problem reading from the {@code Reader}
     */
    static CARAPI Parse(
        /* [in] */ IReader* r,
        /* [in] */ IMap* map);

    /**
     * Parses the next token in the StreamTokenizer as the specified type.
     *
     * @param st The token source
     * @param type The type to interpret next token as
     * @return a Boolean, Number subclass, or String representing the value.
     *         Null strings are represented by the String instance NULL_STRING
     * @throws IOException if there is a problem reading from the {@code StreamTokenizer}
     */
    static CARAPI ParseValue(
        /* [in] */ IStreamTokenizer* st,
        /* [in] */  Int32 type,
        /* [out] */ IObject** object);

    /**
     * An unchecked exception that is thrown if a {@code get<TYPE>()} method
     * is used to retrieve a parameter whose type does not match the method name.
     */
    static CARAPI ThrowTypeException(
        /* [in] */ const String& property,
        /* [in] */ IInterface* value,
        /* [in] */ const String& requestedType);

protected:
    CARAPI CloneImpl(
        /* [in] */ ITypedProperties* object);

public:
    // A sentinel instance used to indicate a NULL string.
    static const AutoPtr<IString> NULL_STRING;

    // Constants used to represent the supported types.
    static const Int32 TYPE_UNSET;
    static const Int32 TYPE_BOOLEAN;
    static const Int32 TYPE_BYTE;
    // TYPE_CHAR: character literal syntax not supported; use short.
    static const Int32 TYPE_SHORT;
    static const Int32 TYPE_INT;
    static const Int32 TYPE_LONG;
    static const Int32 TYPE_FLOAT;
    static const Int32 TYPE_DOUBLE;
    static const Int32 TYPE_STRING;
    static const Int32 TYPE_ERROR;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_TYPEDPROPERTIES_H__
