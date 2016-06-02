
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/utility/TypedProperties.h"
#include "elastos/droid/internal/utility/CTypedProperties.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/core/StringToReal.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CBoolean;
using Elastos::Core::CByte;
using Elastos::Core::CDouble;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger16;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::IByte;
using Elastos::Core::IDouble;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringToIntegral;
using Elastos::Core::StringToReal;
using Elastos::IO::CStreamTokenizer;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

static AutoPtr<IString> InitNullString()
{
    AutoPtr<IString> str;
    CString::New(String("<TypedProperties:NULL_STRING>"), (IString**)&str);
    return str;
}

const AutoPtr<IString> TypedProperties::NULL_STRING = InitNullString();
const Int32 TypedProperties::TYPE_UNSET = 'x';
const Int32 TypedProperties::TYPE_BOOLEAN = 'Z';
const Int32 TypedProperties::TYPE_BYTE = 'I' | 1 << 8;
const Int32 TypedProperties::TYPE_SHORT = 'I' | 2 << 8;
const Int32 TypedProperties::TYPE_INT = 'I' | 4 << 8;
const Int32 TypedProperties::TYPE_LONG = 'I' | 8 << 8;
const Int32 TypedProperties::TYPE_FLOAT = 'F' | 4 << 8;
const Int32 TypedProperties::TYPE_DOUBLE = 'F' | 8 << 8;
const Int32 TypedProperties::TYPE_STRING = 'L' | 's' << 8;
const Int32 TypedProperties::TYPE_ERROR = -1;

CAR_INTERFACE_IMPL(TypedProperties, HashMap, ITypedProperties)

AutoPtr<IStreamTokenizer> TypedProperties::InitTokenizer(
    /* [in] */ IReader* r)
{
    AutoPtr<IStreamTokenizer> st;
    CStreamTokenizer::New(r, (IStreamTokenizer**)&st);

    // Treat everything we don't specify as "ordinary".
    st->ResetSyntax();

    /* The only non-quoted-string words we'll be reading are:
     * - property names: [._$a-zA-Z0-9]
     * - type names: [a-zS]
     * - number literals: [-0-9.eExXA-Za-z]  ('x' for 0xNNN hex literals. "NaN", "Infinity")
     * - "TRUE" or "false" (case insensitive): [a-zA-Z]
     */
    st->SetWordChars('0', '9');
    st->SetWordChars('A', 'Z');
    st->SetWordChars('a', 'z');
    st->SetWordChars('_', '_');
    st->SetWordChars('$', '$');
    st->SetWordChars('.', '.');
    st->SetWordChars('-', '-');
    st->SetWordChars('+', '+');

    // Single-character tokens
    st->SetOrdinaryChar('=');

    // Other special characters
    st->SetWhitespaceChars(' ', ' ');
    st->SetWhitespaceChars('\t', '\t');
    st->SetWhitespaceChars('\n', '\n');
    st->SetWhitespaceChars('\r', '\r');
    st->SetQuoteChar('"');

    // Java-style comments
    st->SetSlashStarComments(TRUE);
    st->SetSlashSlashComments(TRUE);

    return st;
}

ECode TypedProperties::ThrowParseException(
        /* [in] */ IStreamTokenizer* st,
        /* [in] */ const String& expected)
{
    String stStr;
    IObject::Probe(st)->ToString(&stStr);
    Logger::E("TypedProperties", "expected %s, saw %s", expected.string(), stStr.string());
    return E_PARSE_EXCEPTION;
}

Int32 TypedProperties::InterpretType(
    /* [in] */ const String& typeName)
{
    if (typeName.Equals("unset")) {
        return TYPE_UNSET;
    }
    else if (typeName.Equals("boolean")) {
        return TYPE_BOOLEAN;
    }
    else if (typeName.Equals("byte")) {
        return TYPE_BYTE;
    }
    else if (typeName.Equals("short")) {
        return TYPE_SHORT;
    }
    else if (typeName.Equals("int")) {
        return TYPE_INT;
    }
    else if (typeName.Equals("long")) {
        return TYPE_LONG;
    }
    else if (typeName.Equals("float")) {
        return TYPE_FLOAT;
    }
    else if (typeName.Equals("double")) {
        return TYPE_DOUBLE;
    }
    else if (typeName.Equals("String")) {
        return TYPE_STRING;
    }
    return TYPE_ERROR;
}

ECode TypedProperties::Parse(
    /* [in] */ IReader* r,
    /* [in] */ IMap* map)
{
    AutoPtr<IStreamTokenizer> st = InitTokenizer(r);

    /* A property name must be a valid fully-qualified class + package name.
     * We don't support Unicode, though.
     */
    String identifierPattern("[a-zA-Z_$][0-9a-zA-Z_$]*");
    StringBuilder pattern;
    pattern += "(";
    pattern += identifierPattern;
    pattern += "\\.)*";
    pattern += identifierPattern;
    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
    AutoPtr<IPattern> propertyNamePattern;
    FAIL_RETURN(patternHelper->Compile(pattern.ToString(), (IPattern**)&propertyNamePattern))

    while (TRUE) {
        Int32 token;

        // Read the next token, which is either the type or EOF.
        FAIL_RETURN(st->GetNextToken(&token))
        if (token == IStreamTokenizer::TT_EOF) {
            break;
        }
        if (token != IStreamTokenizer::TT_WORD) {
            return ThrowParseException(st, String("type name"));
        }
        String sval;
        st->GetSval(&sval);
        Int32 type = InterpretType(sval);
        if (type == TYPE_ERROR) {
            return ThrowParseException(st, String("valid type name"));
        }
        st->SetSval(String(NULL));

        if (type == TYPE_UNSET) {
            // Expect '('.
            FAIL_RETURN(st->GetNextToken(&token))
            if (token != '(') {
                return ThrowParseException(st, String("'('"));
            }
        }

        // Read the property name.
        FAIL_RETURN(st->GetNextToken(&token))
        if (token != IStreamTokenizer::TT_WORD) {
            return ThrowParseException(st, String("property name"));
        }
        String propertyName;
        st->GetSval(&propertyName);
        AutoPtr<IMatcher> matcher;
        propertyNamePattern->Matcher(propertyName, (IMatcher**)&matcher);
        Boolean matches;
        matcher->Matches(&matches);
        if (!matches) {
            return ThrowParseException(st, String("valid property name"));
        }
        st->SetSval(String(NULL));

        if (type == TYPE_UNSET) {
            // Expect ')'.
            FAIL_RETURN(st->GetNextToken(&token))
            if (token != ')') {
                return ThrowParseException(st, String("')'"));
            }
            AutoPtr<IString> str;
            CString::New(propertyName, (IString**)&str);
            map->Remove(str);
        }
        else {
            // Expect '='.
            FAIL_RETURN(st->GetNextToken(&token))
            if (token != '=') {
                return ThrowParseException(st, String("'='"));
            }

            // Read a value of the appropriate type, and insert into the map.
            AutoPtr<IObject> value;
            FAIL_RETURN(ParseValue(st, type, (IObject**)&value))
            AutoPtr<IInterface> oldValue;
            AutoPtr<IString> str;
            CString::New(propertyName, (IString**)&str);
            map->Remove(str, (IInterface**)&oldValue);
            if (oldValue != NULL) {
                // TODO: catch the case where a string is set to NULL and then
                //       the same property is defined with a different type.
                ClassID id1, id2;
                value->GetClassID(&id1);
                IObject::Probe(oldValue)->GetClassID(&id2);
                if (id1 != id2) {
                    return ThrowParseException(st,
                        String("(property previously declared as a different type)"));
                }
            }
            map->Put(str, value);
        }

        // Expect ';'.
        FAIL_RETURN(st->GetNextToken(&token))
        if (token != ';') {
            return ThrowParseException(st, String("';'"));
        }
    }
    return NOERROR;
}

ECode TypedProperties::ParseValue(
    /* [in] */ IStreamTokenizer* st,
    /* [in] */  Int32 type,
    /* [out] */ IObject** object)
{
    VALIDATE_NOT_NULL(object)
    Int32 token;
    FAIL_RETURN(st->GetNextToken(&token))

    String sval;
    st->GetSval(&sval);
    if (type == TYPE_BOOLEAN) {
        if (token != IStreamTokenizer::TT_WORD) {
            return ThrowParseException(st, String("boolean constant"));
        }

        if (sval.Equals("true")) {
            return CBoolean::New(TRUE, object);
        }
        else if (sval.Equals("false")) {
            return CBoolean::New(FALSE, object);
        }

        return ThrowParseException(st, String("boolean constant"));
    }
    else if ((type & 0xff) == 'I') {
        if (token != IStreamTokenizer::TT_WORD) {
            return ThrowParseException(st, String("integer constant"));
        }

        /* Parse the string.  Long.decode() handles C-style integer constants
         * ("0x" -> hex, "0" -> octal).  It also treats numbers with a prefix of "#" as
         * hex, but our syntax intentionally does not list '#' as a word character.
         */
        Int64 value;
        if (StringToIntegral::Decode(sval, &value) == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            return ThrowParseException(st, String("integer constant"));
        }

        // Ensure that the type can hold this value, and return.
        Int32 width = (type >> 8) & 0xff;
        switch (width) {
        case 1:
            if (value < Elastos::Core::Math::BYTE_MIN_VALUE
                || value > Elastos::Core::Math::BYTE_MAX_VALUE) {
                return ThrowParseException(st, String("8-bit integer constant"));
            }
            return CByte::New((Byte)value, object);
        case 2:
            if (value < Elastos::Core::Math::INT16_MIN_VALUE
                || value > Elastos::Core::Math::INT16_MAX_VALUE) {
                return ThrowParseException(st, String("16-bit integer constant"));
            }
            return CInteger16::New((Int16)value, object);
        case 4:
            if (value < Elastos::Core::Math::INT32_MIN_VALUE
                || value > Elastos::Core::Math::INT32_MAX_VALUE) {
                return ThrowParseException(st, String("32-bit integer constant"));
            }
            return CInteger32::New((Int32)value, object);
        case 8:
            if (value < Elastos::Core::Math::INT64_MIN_VALUE
                || value > Elastos::Core::Math::INT64_MAX_VALUE) {
                return ThrowParseException(st, String("64-bit integer constant"));
            }
            return CInteger64::New((Int64)value, object);
        default:
            Logger::E("TypedProperties",
                "Internal error; unexpected integer type width %d", width);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else if ((type & 0xff) == 'F') {
        if (token != IStreamTokenizer::TT_WORD) {
            return ThrowParseException(st, String("float constant"));
        }

        // Parse the string.
        /* TODO: Maybe just parse as float or double, losing precision if necessary.
         *       Parsing as double and converting to float can change the value
         *       compared to just parsing as float.
         */
        Double value;
        if (StringToReal::Parse(sval, &value) == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            return ThrowParseException(st, String("float constant"));
        }

        // Ensure that the type can hold this value, and return.
        if (((type >> 8) & 0xff) == 4) {
            // This property is a float; make sure the value fits.
            Double absValue = Elastos::Core::Math::Abs(value);
            if (absValue != 0.0 && !Elastos::Core::Math::IsInfinite(value)
                && !Elastos::Core::Math::IsNaN(value)) {
                if (absValue < Elastos::Core::Math::FLOAT_MIN_VALUE
                    || absValue > Elastos::Core::Math::FLOAT_MAX_VALUE) {
                    return ThrowParseException(st, String("32-bit float constant"));
                }
            }
            return CFloat::New((Float)value, object);
        }
        else {
            // This property is a double; no need to truncate.
            return CDouble::New(value, object);
        }
    }
    else if (type == TYPE_STRING) {
        // Expect a quoted string or the word "NULL".
        if (token == '"') {
            return CString::New(sval, object);
        }
        else if (token == IStreamTokenizer::TT_WORD && sval.Equals("null")) {
            *object = IObject::Probe(NULL_STRING);
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
        return ThrowParseException(st, String("double-quoted string or 'NULL'"));
    }
    Logger::E("TypedProperties", "Internal error; unknown type %d", type);
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode TypedProperties::constructor()
{
    return HashMap::constructor();
}

ECode TypedProperties::Load(
    /* [in] */ IReader* r)
{
    return Parse(r, this);
}

ECode TypedProperties::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    AutoPtr<IInterface> obj;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&obj))
    if (IString::Probe(obj) != NULL_STRING) {
        *value = obj;
        REFCOUNT_ADD(*value)
    }
    return NOERROR;
}

ECode TypedProperties::ThrowTypeException(
    /* [in] */ const String& property,
    /* [in] */ IInterface* value,
    /* [in] */ const String& requestedType)
{
    Logger::E("TypedProperties", "%s has type not %s", property.string(),
        /*value.getClass().getName(), */requestedType.string());
    return E_TYPE_EXCEPTION;
}

ECode TypedProperties::GetBoolean(
    /* [in] */ const String& property,
    /* [in] */ Boolean def,
    /* [out] */ Boolean* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IBoolean::Probe(ivalue)) {
        return IBoolean::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Boolean"));
}

ECode TypedProperties::GetByte(
    /* [in] */ const String& property,
    /* [in] */ Byte def,
    /* [out] */ Byte* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IByte::Probe(ivalue)) {
        return IByte::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Byte"));
}

ECode TypedProperties::GetInt16(
    /* [in] */ const String& property,
    /* [in] */ Int16 def,
    /* [out] */ Int16* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IInteger16::Probe(ivalue)) {
        return IInteger16::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Int16"));
}

ECode TypedProperties::GetInt32(
    /* [in] */ const String& property,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IInteger32::Probe(ivalue)) {
        return IInteger32::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Int32"));
}

ECode TypedProperties::GetInt64(
    /* [in] */ const String& property,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IInteger64::Probe(ivalue)) {
        return IInteger64::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Int64"));
}

ECode TypedProperties::GetFloat(
    /* [in] */ const String& property,
    /* [in] */ Float def,
    /* [out] */ Float* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IFloat::Probe(ivalue)) {
        return IFloat::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Float"));
}

ECode TypedProperties::GetDouble(
    /* [in] */ const String& property,
    /* [in] */ Double def,
    /* [out] */ Double* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IDouble::Probe(ivalue)) {
        return IDouble::Probe(ivalue)->GetValue(value);
    }
    return ThrowTypeException(property, ivalue, String("Double"));
}

ECode TypedProperties::GetString(
    /* [in] */ const String& property,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> ivalue;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&ivalue))
    if (ivalue == NULL) {
        *value = def;
        return NOERROR;
    }
    if (IString::Probe(ivalue) == NULL_STRING) {
        *value = NULL;
        return NOERROR;
    }
    else if (IString::Probe(ivalue)) {
        return IObject::Probe(ivalue)->ToString(value);
    }
    return ThrowTypeException(property, ivalue, String("String"));
}

ECode TypedProperties::GetBoolean(
    /* [in] */ const String& property,
    /* [out] */ Boolean* value)
{
    return GetBoolean(property, FALSE, value);
}

ECode TypedProperties::GetByte(
    /* [in] */ const String& property,
    /* [out] */ Byte* value)
{
    return GetByte(property, (Byte)0, value);
}

ECode TypedProperties::GetInt16(
    /* [in] */ const String& property,
    /* [out] */ Int16* value)
{
    return GetInt16(property, (Int16)0, value);
}

ECode TypedProperties::GetInt32(
    /* [in] */ const String& property,
    /* [out] */ Int32* value)
{
    return GetInt32(property, 0, value);
}

ECode TypedProperties::GetInt64(
    /* [in] */ const String& property,
    /* [out] */ Int64* value)
{
    return GetInt64(property, 0L, value);
}

ECode TypedProperties::GetFloat(
    /* [in] */ const String& property,
    /* [out] */ Float* value)
{
    return GetFloat(property, 0.0f, value);
}

ECode TypedProperties::GetDouble(
    /* [in] */ const String& property,
    /* [out] */ Double* value)
{
    return GetDouble(property, 0.0, value);
}

ECode TypedProperties::GetString(
    /* [in] */ const String& property,
    /* [out] */ String* value)
{
    return GetString(property, String(""), value);
}

ECode TypedProperties::GetStringInfo(
    /* [in] */ const String& property,
    /* [out] */ Int32* info)
{
    AutoPtr<IString> key;
    CString::New(property, (IString**)&key);
    AutoPtr<IInterface> value;
    FAIL_RETURN(HashMap::Get(key, (IInterface**)&value))
    if (value == NULL) {
        *info = STRING_NOT_SET;
        return NOERROR;
    }

    if (IString::Probe(value) == NULL_STRING) {
        *info = STRING_NULL;
        return NOERROR;
    }
    else if (IString::Probe(value)) {
        *info = STRING_SET;
        return NOERROR;
    }
    *info = STRING_TYPE_MISMATCH;
    return NOERROR;
}

ECode TypedProperties::Clone(
    /* [out] */ IInterface** object)
{
    AutoPtr<ITypedProperties> cloneObj;
    CTypedProperties::New((ITypedProperties**)&cloneObj);
    CloneImpl(cloneObj);
    *object = cloneObj;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode TypedProperties::CloneImpl(
    /* [in] */ ITypedProperties* object)
{
    // HashMap::CloneImpl(IHashMap::Probe(object));
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
