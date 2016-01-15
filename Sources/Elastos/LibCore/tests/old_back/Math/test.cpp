#include <elastos.h>
#include <elautoptr.h>
#include <eltypes.h>
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/IntegralToString.h>
#include <elastos/RealToString.h>
#include <elastos/StringUtils.h>
#include <elastos/Math.h>
#include <Elastos.CoreLibrary.h>
#include <cmdef.h>

using namespace Elastos;
using Elastos::Core::IntegralToString;
using Elastos::Core::RealToString;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigDecimal;
using Elastos::Math::CBigDecimal;
using Elastos::Math::IBigDecimalHelper;
using Elastos::Math::CBigDecimalHelper;


void assertEquals(const String& aspect, const String& test)
{
    printf("aspect: [%s], test: [%s]\n", aspect.string(), test.string());
    assert(aspect.Equals(test) && "result not equals aspect!");
}

void assertEquals(Double aspect, Double test)
{
    printf("aspect: %f, test: %f\n", aspect, test);
    assert(aspect == test);
}

void assertEquals(Float aspect, Float test)
{
    printf("aspect: %f, test: %f\n", aspect, test);
    assert(aspect == test);
}

void assertEquals(Int64 aspect, Int64 test)
{
    printf("aspect: %lld, test: %lld\n", aspect, test);
    assert(aspect == test);
}

void assertEquals(Int32 aspect, Int32 test)
{
    printf("aspect: %d, test: %d\n", aspect, test);
    assert(aspect == test);
}

void assertEquals(const char* info, Int64 test, Int64 aspect)
{
    printf("aspect: %lld, test: %lld. %s\n", aspect, test, info);
    assert(aspect == test);
}

void printArray(ArrayOf<IBigInteger *> * v, const char* info)
{
    printf("  >------Start print %s ------<\n", info);
    Int32 len = v->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        if ((*v)[i]) {
            IBigInteger* bi = (*v)[i];
            Int32 sign = 0;
            bi->GetSignum(&sign);
            printf("    > %d: %p, sign %d\n", i, bi, sign);
        }
        else {
            printf("    > %d: NULL\n", i);
        }
    }
    printf("  >------End print %s ------<\n", info);
}

void testBigInteger()
{
    printf(" ------ Enter testBigInteger------\n");

    AutoPtr<IBigDecimal> d1;
    ECode ec = CBigDecimal::New(1, (IBigDecimal**)&d1);
    if (FAILED(ec) || d1 == NULL) {
        printf(" Failed to create IBigDecimal. Error %08X\n", ec);
    }

    AutoPtr<IBigDecimal> d2;
    ec = CBigDecimal::New(99999, 9999, (IBigDecimal**)&d2);
    if (FAILED(ec) || d2 == NULL) {
        printf(" Failed to create IBigDecimal. Error %08X\n", ec);
    }

    String d2Str;
    d2->ToString(&d2Str);
    printf(" d2 to string : %s\n", (const char*)d2Str);

    AutoPtr<IBigInteger> i1;
    ec = CBigInteger::New(1, 1, (IBigInteger**)&i1);
    if (FAILED(ec) || i1 == NULL) {
        printf(" Failed to create IBigInteger. Error %08X\n", ec);
    }

    Int32 aignum;
    i1->GetSignum(&aignum);
    printf(" Signum of i1 is %d\n", aignum);

    printf(" ------ Leave testBigInteger------\n");
}

void assertEquals(Float aspect, const String& test)
{
    using Elastos::Core::Math;

    printf("ParseFloat %s: ", test.string());
    Float floatValue;
    ECode ec = StringUtils::ParseFloat(test, &floatValue);
    printf("%f\n", floatValue);
    ASSERT_SUCCEEDED(ec);
    if (!test.Equals("NaN") && !test.Equals("-NaN") && !test.Equals("+NaN")
        && !test.Equals("Infinity") && !test.Equals("-Infinity") && !test.Equals("+Infinity"))
        assert(aspect == floatValue && "Failed to ParseFloat");
}

void assertEquals(Double aspect, const String& test)
{
    using Elastos::Core::Math;

    printf("ParseDouble [%s]: aspect: [%f]", test.string(), aspect);
    Double doubleValue;
    ECode ec = StringUtils::ParseDouble(test, &doubleValue);
    printf(", result: [%f]\n", doubleValue);
    ASSERT_SUCCEEDED(ec);
    if (!test.Equals("NaN") && !test.Equals("-NaN") && !test.Equals("+NaN")
        && !test.Equals("Infinity") && !test.Equals("-Infinity") && !test.Equals("+Infinity"))
        assert(aspect == doubleValue && "Failed to ParseFloat");
}

void testStringToIntegral()
{
    using Elastos::Core::Math;

    printf(" =======testStringToIntegral========\n");

    printf(" =======ParseInt64========\n");

    assertEquals(StringUtils::ParseInt64(String("0")), 0L);
    assertEquals(StringUtils::ParseInt64(String("1")), 1L);
    assertEquals(StringUtils::ParseInt64(String("-1")), -1L);
    assertEquals(StringUtils::ParseInt64(String("0"), 10), 0L);
    assertEquals(StringUtils::ParseInt64(String("1"), 10), 1L);
    assertEquals(StringUtils::ParseInt64(String("-1"), 10), -1L);

    assertEquals("Returned incorrect value",
        StringUtils::ParseInt64(String("100000000"), 10), 100000000L);
    assertEquals("Returned incorrect value from hex string",
        StringUtils::ParseInt64(String("FFFFFFFFF"), 16), 68719476735L);
    assertEquals("Returned incorrect value from octal string: 77777777777",
        StringUtils::ParseInt64(String("77777777777"), 8), 8589934591L);
    assertEquals("Returned incorrect value for 0 hex",
        StringUtils::ParseInt64(String("0"), 16), 0);
    assertEquals("Returned incorrect value for most negative value hex",
        StringUtils::ParseInt64(String("-8000000000000000"), 16), 0x8000000000000000L);
    assertEquals("Returned incorrect value for most positive value hex",
        StringUtils::ParseInt64(String("7fffffffffffffff"), 16), 0x7fffffffffffffffL);
    assertEquals("Returned incorrect value for 0 decimal",
        StringUtils::ParseInt64(String("0"), 10), 0);
    assertEquals("Returned incorrect value for most negative value decimal",
        StringUtils::ParseInt64(String("-9223372036854775808"), 10), 0x8000000000000000L);
    assertEquals("Returned incorrect value for most positive value decimal",
        StringUtils::ParseInt64(String("9223372036854775807"), 10), 0x7fffffffffffffffL);

    assertEquals("Parsed to incorrect long value",
        StringUtils::ParseInt64(String("89000000005")), 89000000005L);
    assertEquals("Returned incorrect value for 0",
        StringUtils::ParseInt64(String("0")), 0);
    assertEquals("Returned incorrect value for most negative value",
        StringUtils::ParseInt64(String("-9223372036854775808")), 0x8000000000000000L);
    assertEquals("Returned incorrect value for most positive value",
        StringUtils::ParseInt64(String("9223372036854775807")), 0x7fffffffffffffffL);

    Int64 value;
    assert(FAILED(StringUtils::ParseInt64(String("999999999999"), 8, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("9223372036854775808"), 10, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("-9223372036854775809"), 10, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("8000000000000000"), 16, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("-8000000000000001"), 16, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("42325917317067571199"), 10, &value)));
    assert(FAILED(StringUtils::ParseInt64(String("-9223372036854775809"), &value)));
    assert(FAILED(StringUtils::ParseInt64(String("9223372036854775808"), &value)));
}

void testIntegralToString()
{
    using Elastos::Core::Math;

    printf(" =======testIntegralToString========\n");

    Int32 i = Math::INT32_MAX_VALUE;
    printf("\n INT32_MAX_VALUE %d: ", i);

    printf("=======Int32ToString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::ToString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::ToString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::ToString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::ToString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToString(0xFFFFFFFF));
    printf(" -1: %s\n", (const char*)IntegralToString::ToString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::ToString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToString(-0xFFFFFFFF));
    printf("INT32_MAX_VALUE %s\n", (const char*)IntegralToString::ToString(i));
    printf("\n");

    printf(" =======Int32ToBinaryString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::ToBinaryString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::ToBinaryString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::ToBinaryString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::ToBinaryString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToBinaryString(0xFFFFFFFF));
    printf(" -1: %s\n", (const char*)IntegralToString::ToBinaryString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::ToBinaryString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToBinaryString(-0xFFFFFFFF));
    printf("INT32_MAX_VALUE %s\n", (const char*)IntegralToString::ToBinaryString(i));
    printf("\n");

    printf(" =======Int32ToOctalString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::ToOctalString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::ToOctalString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::ToOctalString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::ToOctalString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToOctalString(0xFFFFFFFF));
    printf(" -1: %s\n", (const char*)IntegralToString::ToOctalString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::ToOctalString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToOctalString(-0xFFFFFFFF));
    printf("INT32_MAX_VALUE %s\n", (const char*)IntegralToString::ToOctalString(i));
    printf("\n");

    printf(" =======Int32ToHexString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::ToHexString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::ToHexString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::ToHexString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::ToHexString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToHexString(0xFFFFFFFF));
    printf(" -1: %s\n", (const char*)IntegralToString::ToHexString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::ToHexString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::ToHexString(-0xFFFFFFFF));
    printf("INT32_MAX_VALUE %s\n", (const char*)IntegralToString::ToHexString(i));
    printf("\n");

    Int64 l = 0xEFFFFFFF;

    printf(" =======Int64ToBinaryString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::Int64ToBinaryString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::Int64ToBinaryString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::Int64ToBinaryString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::Int64ToBinaryString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::Int64ToBinaryString(l));
    printf(" -1: %s\n", (const char*)IntegralToString::Int64ToBinaryString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::Int64ToBinaryString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::Int64ToBinaryString(-l));
    printf("\n");

    printf(" =======Int64ToOctalString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::Int64ToOctalString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::Int64ToOctalString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::Int64ToOctalString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::Int64ToOctalString(0xFF));
    printf(" 0xFFFFFFFF: %s\n", (const char*)IntegralToString::Int64ToOctalString(l));
    printf(" -1: %s\n", (const char*)IntegralToString::Int64ToOctalString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::Int64ToOctalString(-2));
    printf(" -0xFFFFFFFF: %s\n", (const char*)IntegralToString::Int64ToOctalString(-l));
    printf("\n");

    l = 92233720368547L;

    printf(" =======Int64ToString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::Int64ToString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::Int64ToString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::Int64ToString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::Int64ToString(0xFF));
    printf(" 92233720368547L: %s\n", (const char*)IntegralToString::Int64ToString(l));
    printf(" -1: %s\n", (const char*)IntegralToString::Int64ToString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::Int64ToString(-2));
    printf(" -92233720368547L: %s\n", (const char*)IntegralToString::Int64ToString(-l));
    l = Elastos::Core::Math::INT64_MAX_VALUE;
    printf(" INT64_MAX_VALUE %lld: ", l);
    printf("%s\n", (const char*)IntegralToString::Int64ToString(l));
    printf("\n");

    printf(" =======Int64ToHexString========\n");
    printf(" 0x0: %s\n", (const char*)IntegralToString::Int64ToHexString(0x0));
    printf(" 0x1: %s\n", (const char*)IntegralToString::Int64ToHexString(0x1));
    printf(" 0x10: %s\n", (const char*)IntegralToString::Int64ToHexString(0x10));
    printf(" 0xFF: %s\n", (const char*)IntegralToString::Int64ToHexString(0xFF));
    printf(" 92233720368547L: %s\n", (const char*)IntegralToString::Int64ToHexString(l));
    printf(" -1: %s\n", (const char*)IntegralToString::Int64ToHexString(-1));
    printf(" -2: %s\n", (const char*)IntegralToString::Int64ToHexString(-2));
    printf(" -92233720368547L: %s\n", (const char*)IntegralToString::Int64ToHexString(-l));
    l = Elastos::Core::Math::INT64_MAX_VALUE;
    printf(" INT64_MAX_VALUE %lld: ", l);
    printf("%s\n", (const char*)IntegralToString::Int64ToHexString(l));
    printf("\n");

    printf(" =======ParseFloat========\n");
    String int32MinValue = StringUtils::Int32ToString(Math::INT32_MIN_VALUE);
    assertEquals(-2.14748365E9f, int32MinValue);
    assertEquals(2358.166016f, StringUtils::ParseFloat(String("2358.166016")));

    assertEquals(1.0f, StringUtils::ParseFloat(String("1f")));
    assertEquals(1.0f, StringUtils::ParseFloat(String("1d")));
    assertEquals(1.0f, StringUtils::ParseFloat(String("1F")));
    assertEquals(1.0f, StringUtils::ParseFloat(String("1D")));
    assertEquals(1.0f, StringUtils::ParseFloat(String("1.D")));
    assertEquals(1.0f, StringUtils::ParseFloat(String("1.E0D")));
    assertEquals(1.0f, StringUtils::ParseFloat(String(".1E1D")));
    assertEquals(1.0e-45f, StringUtils::ParseFloat(String("1.0e-45")));
    assertEquals(0.0f, StringUtils::ParseFloat(String("1.0e-46")));
    assertEquals(-1.0e-45f, StringUtils::ParseFloat(String("-1.0e-45")));
    assertEquals(-0.0f, StringUtils::ParseFloat(String("-1.0e-46")));

    assertEquals(1.0e+38f, StringUtils::ParseFloat(String("1.0e+38")));
    assertEquals(Math::FLOAT_POSITIVE_INFINITY, StringUtils::ParseFloat(String("1.0e+39")));
    assertEquals(-1.0e+38f, StringUtils::ParseFloat(String("-1.0e+38")));
    assertEquals(Math::FLOAT_NEGATIVE_INFINITY, StringUtils::ParseFloat(String("-1.0e+39")));

    assertEquals(Math::FLOAT_POSITIVE_INFINITY, StringUtils::ParseFloat(String("1.0e+9999999999")));
    assertEquals(Math::FLOAT_NEGATIVE_INFINITY, StringUtils::ParseFloat(String("-1.0e+9999999999")));
    assertEquals(0.0f, StringUtils::ParseFloat(String("1.0e-9999999999")));
    assertEquals(-0.0f, StringUtils::ParseFloat(String("-1.0e-9999999999")));

    assertEquals(Math::FLOAT_POSITIVE_INFINITY, StringUtils::ParseFloat(String("320.0E+2147483647")));
    assertEquals(-0.0f, StringUtils::ParseFloat(String("-1.4E-2147483314")));

    // testNamedFloats
    assertEquals(Math::FLOAT_NAN, StringUtils::ParseFloat(String("NaN")));
    assertEquals(Math::FLOAT_NAN, StringUtils::ParseFloat(String("-NaN")));
    assertEquals(Math::FLOAT_NAN, StringUtils::ParseFloat(String("+NaN")));

    assertEquals(Math::FLOAT_POSITIVE_INFINITY, StringUtils::ParseFloat(String("+Infinity")));
    assertEquals(Math::FLOAT_POSITIVE_INFINITY, StringUtils::ParseFloat(String("Infinity")));
    assertEquals(Math::FLOAT_NEGATIVE_INFINITY, StringUtils::ParseFloat(String("-Infinity")));
    printf("\n");

    printf(" =======ParseDouble========\n");
    assertEquals(String("0.008"), StringUtils::DoubleToString(0.008));
    assertEquals(String("0.008366"), StringUtils::DoubleToString(0.008366));
    // http://code.google.com/p/android/issues/detail?id=14033
    assertEquals(String("0.009"), StringUtils::DoubleToString(0.009));
    // http://code.google.com/p/android/issues/detail?id=14302
    assertEquals(String("0.008567856012638986"), StringUtils::DoubleToString(0.008567856012638986));
    assertEquals(String("0.010206713752229896"), StringUtils::DoubleToString(0.010206713752229896));

    assertEquals(Math::DOUBLE_NAN, String("NaN"));
    assertEquals(Math::DOUBLE_NAN, String("-NaN"));
    assertEquals(Math::DOUBLE_NAN, String("+NaN"));

    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, String("+Infinity"));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, String("Infinity"));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, String("-Infinity"));

    assertEquals(1.0, String("1f"));
    assertEquals(1.0, String("1d"));
    assertEquals(1.0, String("1F"));
    assertEquals(1.0, String("1D"));
    assertEquals(1.0, String("1.D"));
    assertEquals(1.0, String("1.E0D"));
    assertEquals(1.0, String(".1E1D"));

    assertEquals(1.0e-323, String("1.0e-323"));
    assertEquals(0.0, String("1.0e-324"));
    assertEquals(-1.0e-323, String("-1.0e-323"));
    assertEquals(-0.0, String("-1.0e-324"));

    assertEquals(1.0e+308, String("1.0e+308"));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, String("1.0e+309"));
    assertEquals(-1.0e+308, String("-1.0e+308"));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, String("-1.0e+309"));

    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, String("1.0e+9999999999"));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, String("-1.0e+9999999999"));
    assertEquals(0.0, String("1.0e-9999999999"));
    assertEquals(-0.0, String("-1.0e-9999999999"));

    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, String("320.0e+2147483647"));
    assertEquals(-0.0, String("-1.4e-2147483314"));

    assertEquals(2.2250738585072014E-308, String("2.2250738585072012e-308"));
    assertEquals(2.2250738585072014E-308, String("0.00022250738585072012e-304"));
    assertEquals(2.2250738585072014E-308, String("00000002.2250738585072012e-308"));
    assertEquals(2.2250738585072014E-308, String("2.225073858507201200000e-308"));
    assertEquals(2.2250738585072014E-308, String("2.2250738585072012e-00308"));
    assertEquals(2.2250738585072014E-308, String("2.22507385850720129978001e-308"));
    assertEquals(-2.2250738585072014E-308, String("-2.2250738585072012e-308"));
    printf("\n");
}

void testRealToString()
{
    printf(" ===============\n");

    RealToString* rts = RealToString::GetInstance();

    Float f = 12345.54321;
    Double d = 987654321.12345678;

    String fs = rts->ConvertFloat(f);
    String ds = rts->ConvertDouble(d);

    printf(" Float %f to string %s\n", f, fs.string());
    printf(" Double %f to string %s\n", d, ds.string());
    printf("\n");
}

//==============================================================================
void assertPrecision(
    /* [in] */ Int32 expectedPrecision,
    /* [in] */ const String& value)
{
    printf("\n======assertPrecision for expectedPrecision: %d, value: %s ======\n", expectedPrecision, value.string());
    AutoPtr<IBigDecimal> parsed;
    CBigDecimal::New(value, (IBigDecimal**)&parsed);
    Int32 precision;
    parsed->GetPrecision(&precision);
    printf("expectedPrecision: %d, precision: %d\n\n", expectedPrecision, precision);
    assert(expectedPrecision == precision && "Unexpected precision for parsed value ");

    AutoPtr<IBigDecimalHelper> helper;
    CBigDecimalHelper::AcquireSingleton((IBigDecimalHelper**)&helper);
    AutoPtr<IBigDecimal> computed, one;
    helper->GetONE((IBigDecimal**)&one);
    ASSERT_SUCCEEDED(parsed->DivideEx2(one, (IBigDecimal**)&computed));
    computed->GetPrecision(&precision);

    printf("expectedPrecision: %d, precision: %d\n", expectedPrecision, precision);
    assert(expectedPrecision == precision && "Unexpected precision for computed value ");
}

void testGetPrecision()
{
    assertPrecision(1, String("0"));
    assertPrecision(1, String("0.9"));
    assertPrecision(16, String("0.9999999999999999"));
    assertPrecision(16, String("9999999999999999"));
    assertPrecision(19, String("1000000000000000000"));
    assertPrecision(19, String("1000000000000000001"));
    assertPrecision(19, String("-1000000000000000001"));
    assertPrecision(19, String("-1000000000000000000"));

    // String tenNines = "9999999999";
    // String fiftyNines = tenNines + tenNines + tenNines + tenNines + tenNines;
    // assertPrecision(10, "0." + tenNines);
    // assertPrecision(50, "0." + fiftyNines);
    // assertPrecision(250, "0." + fiftyNines + fiftyNines + fiftyNines + fiftyNines + fiftyNines);
    // assertPrecision(10, tenNines);
    // assertPrecision(50, fiftyNines);
    // assertPrecision(250, fiftyNines + fiftyNines + fiftyNines + fiftyNines + fiftyNines);

    // // test these special cases because we know precision() uses longs internally
    // String maxLong = Long.toString(Math::INT64_MAX_VALUE);
    // assertPrecision(maxLong.length(), maxLong);
    // String minLong = Long.toString(Math::INT64_MIN_VALUE);
    // assertPrecision(minLong.length() - 1, minLong);
}

void testRound()
{
    // BigDecimal bigDecimal = new BigDecimal("0.999999999999999");
    // BigDecimal rounded = bigDecimal.round(new MathContext(2, RoundingMode.FLOOR));
    // assertEquals("0.99", rounded.toString());
}

void testBigDecimal()
{
    testGetPrecision();
    testRound();
}

//==============================================================================

void test_numberOfLeadingZerosI()
{
    printf("\n===Test NumberOfLeadingZerosI===\n");
    using Elastos::Core::Math;
    assertEquals(32, Math::NumberOfLeadingZeros(0x0));
    assertEquals(31, Math::NumberOfLeadingZeros(0x1));
    assertEquals(30, Math::NumberOfLeadingZeros(0x2));
    assertEquals(30, Math::NumberOfLeadingZeros(0x3));
    assertEquals(29, Math::NumberOfLeadingZeros(0x4));
    assertEquals(29, Math::NumberOfLeadingZeros(0x5));
    assertEquals(29, Math::NumberOfLeadingZeros(0x6));
    assertEquals(29, Math::NumberOfLeadingZeros(0x7));
    assertEquals(28, Math::NumberOfLeadingZeros(0x8));
    assertEquals(28, Math::NumberOfLeadingZeros(0x9));
    assertEquals(28, Math::NumberOfLeadingZeros(0xA));
    assertEquals(28, Math::NumberOfLeadingZeros(0xB));
    assertEquals(28, Math::NumberOfLeadingZeros(0xC));
    assertEquals(28, Math::NumberOfLeadingZeros(0xD));
    assertEquals(28, Math::NumberOfLeadingZeros(0xE));
    assertEquals(28, Math::NumberOfLeadingZeros(0xF));
    assertEquals(27, Math::NumberOfLeadingZeros(0x10));
    assertEquals(24, Math::NumberOfLeadingZeros(0x80));
    assertEquals(24, Math::NumberOfLeadingZeros(0xF0));
    assertEquals(23, Math::NumberOfLeadingZeros(0x100));
    assertEquals(20, Math::NumberOfLeadingZeros(0x800));
    assertEquals(20, Math::NumberOfLeadingZeros(0xF00));
    assertEquals(19, Math::NumberOfLeadingZeros(0x1000));
    assertEquals(16, Math::NumberOfLeadingZeros(0x8000));
    assertEquals(16, Math::NumberOfLeadingZeros(0xF000));
    assertEquals(15, Math::NumberOfLeadingZeros(0x10000));
    assertEquals(12, Math::NumberOfLeadingZeros(0x80000));
    assertEquals(12, Math::NumberOfLeadingZeros(0xF0000));
    assertEquals(11, Math::NumberOfLeadingZeros(0x100000));
    assertEquals(8, Math::NumberOfLeadingZeros(0x800000));
    assertEquals(8, Math::NumberOfLeadingZeros(0xF00000));
    assertEquals(7, Math::NumberOfLeadingZeros(0x1000000));
    assertEquals(4, Math::NumberOfLeadingZeros(0x8000000));
    assertEquals(4, Math::NumberOfLeadingZeros(0xF000000));
    assertEquals(3, Math::NumberOfLeadingZeros(0x10000000));
    assertEquals(0, Math::NumberOfLeadingZeros((Int32)0x80000000));
    assertEquals(0, Math::NumberOfLeadingZeros((Int32)0xF0000000));

    assertEquals(1, Math::NumberOfLeadingZeros(Math::INT32_MAX_VALUE));
    assertEquals(0, Math::NumberOfLeadingZeros(Math::INT32_MIN_VALUE));
}

void test_numberOfTrailingZerosI()
{
    printf("\n===Test NumberOfTrailingZerosI===\n");
    using Elastos::Core::Math;
    assertEquals(32, Math::NumberOfTrailingZeros(0x0));
    assertEquals(31, Math::NumberOfTrailingZeros(Math::INT32_MIN_VALUE));
    assertEquals(0, Math::NumberOfTrailingZeros(Math::INT32_MAX_VALUE));

    assertEquals(0, Math::NumberOfTrailingZeros(0x1));
    assertEquals(3, Math::NumberOfTrailingZeros(0x8));
    assertEquals(0, Math::NumberOfTrailingZeros(0xF));

    assertEquals(4, Math::NumberOfTrailingZeros(0x10));
    assertEquals(7, Math::NumberOfTrailingZeros(0x80));
    assertEquals(4, Math::NumberOfTrailingZeros(0xF0));

    assertEquals(8, Math::NumberOfTrailingZeros(0x100));
    assertEquals(11, Math::NumberOfTrailingZeros(0x800));
    assertEquals(8, Math::NumberOfTrailingZeros(0xF00));

    assertEquals(12, Math::NumberOfTrailingZeros(0x1000));
    assertEquals(15, Math::NumberOfTrailingZeros(0x8000));
    assertEquals(12, Math::NumberOfTrailingZeros(0xF000));

    assertEquals(16, Math::NumberOfTrailingZeros(0x10000));
    assertEquals(19, Math::NumberOfTrailingZeros(0x80000));
    assertEquals(16, Math::NumberOfTrailingZeros(0xF0000));

    assertEquals(20, Math::NumberOfTrailingZeros(0x100000));
    assertEquals(23, Math::NumberOfTrailingZeros(0x800000));
    assertEquals(20, Math::NumberOfTrailingZeros(0xF00000));

    assertEquals(24, Math::NumberOfTrailingZeros(0x1000000));
    assertEquals(27, Math::NumberOfTrailingZeros(0x8000000));
    assertEquals(24, Math::NumberOfTrailingZeros(0xF000000));

    assertEquals(28, Math::NumberOfTrailingZeros(0x10000000));
    assertEquals(31, Math::NumberOfTrailingZeros((Int32)0x80000000));
    assertEquals(28, Math::NumberOfTrailingZeros((Int32)0xF0000000));
}

void test_numberOfLeadingZerosJ()
{
    printf("\n===Test NumberOfLeadingZerosL===\n");
    using Elastos::Core::Math;
    assertEquals(64, Math::NumberOfLeadingZeros((Int64)0x0L));
    assertEquals(63, Math::NumberOfLeadingZeros((Int64)0x1));
    assertEquals(62, Math::NumberOfLeadingZeros((Int64)0x2));
    assertEquals(62, Math::NumberOfLeadingZeros((Int64)0x3));
    assertEquals(61, Math::NumberOfLeadingZeros((Int64)0x4));
    assertEquals(61, Math::NumberOfLeadingZeros((Int64)0x5));
    assertEquals(61, Math::NumberOfLeadingZeros((Int64)0x6));
    assertEquals(61, Math::NumberOfLeadingZeros((Int64)0x7));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0x8));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0x9));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xA));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xB));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xC));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xD));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xE));
    assertEquals(60, Math::NumberOfLeadingZeros((Int64)0xF));
    assertEquals(59, Math::NumberOfLeadingZeros((Int64)0x10));
    assertEquals(56, Math::NumberOfLeadingZeros((Int64)0x80));
    assertEquals(56, Math::NumberOfLeadingZeros((Int64)0xF0));
    assertEquals(55, Math::NumberOfLeadingZeros((Int64)0x100));
    assertEquals(52, Math::NumberOfLeadingZeros((Int64)0x800));
    assertEquals(52, Math::NumberOfLeadingZeros((Int64)0xF00));
    assertEquals(51, Math::NumberOfLeadingZeros((Int64)0x1000));
    assertEquals(48, Math::NumberOfLeadingZeros((Int64)0x8000));
    assertEquals(48, Math::NumberOfLeadingZeros((Int64)0xF000));
    assertEquals(47, Math::NumberOfLeadingZeros((Int64)0x10000));
    assertEquals(44, Math::NumberOfLeadingZeros((Int64)0x80000));
    assertEquals(44, Math::NumberOfLeadingZeros((Int64)0xF0000));
    assertEquals(43, Math::NumberOfLeadingZeros((Int64)0x100000));
    assertEquals(40, Math::NumberOfLeadingZeros((Int64)0x800000));
    assertEquals(40, Math::NumberOfLeadingZeros((Int64)0xF00000));
    assertEquals(39, Math::NumberOfLeadingZeros((Int64)0x1000000));
    assertEquals(36, Math::NumberOfLeadingZeros((Int64)0x8000000));
    assertEquals(36, Math::NumberOfLeadingZeros((Int64)0xF000000));
    assertEquals(35, Math::NumberOfLeadingZeros((Int64)0x10000000));
    //TODO assertEquals(0, Math::NumberOfLeadingZeros((Int64)0x80000000));
    //TODO assertEquals(0, Math::NumberOfLeadingZeros((Int64)0xF0000000));

    assertEquals(1, Math::NumberOfLeadingZeros((Int64)Math::INT64_MAX_VALUE));
    assertEquals(0, Math::NumberOfLeadingZeros((Int64)Math::INT64_MIN_VALUE));
}

/**
 * @tests java.lang.Long#numberOfTrailingZeros(long)
 */
void test_numberOfTrailingZerosJ()
{
    printf("\n===Test NumberOfTrailingZerosL===\n");
    using Elastos::Core::Math;
    assertEquals(64, Math::NumberOfTrailingZeros((Int64)0x0));
    assertEquals(63, Math::NumberOfTrailingZeros((Int64)Math::INT64_MIN_VALUE));
    assertEquals(0, Math::NumberOfTrailingZeros((Int64)Math::INT64_MAX_VALUE));

    assertEquals(0, Math::NumberOfTrailingZeros((Int64)0x1));
    assertEquals(3, Math::NumberOfTrailingZeros((Int64)0x8));
    assertEquals(0, Math::NumberOfTrailingZeros((Int64)0xF));

    assertEquals(4, Math::NumberOfTrailingZeros((Int64)0x10));
    assertEquals(7, Math::NumberOfTrailingZeros((Int64)0x80));
    assertEquals(4, Math::NumberOfTrailingZeros((Int64)0xF0));

    assertEquals(8, Math::NumberOfTrailingZeros((Int64)0x100));
    assertEquals(11, Math::NumberOfTrailingZeros((Int64)0x800));
    assertEquals(8, Math::NumberOfTrailingZeros((Int64)0xF00));

    assertEquals(12, Math::NumberOfTrailingZeros((Int64)0x1000));
    assertEquals(15, Math::NumberOfTrailingZeros((Int64)0x8000));
    assertEquals(12, Math::NumberOfTrailingZeros((Int64)0xF000));

    assertEquals(16, Math::NumberOfTrailingZeros((Int64)0x10000));
    assertEquals(19, Math::NumberOfTrailingZeros((Int64)0x80000));
    assertEquals(16, Math::NumberOfTrailingZeros((Int64)0xF0000));

    assertEquals(20, Math::NumberOfTrailingZeros((Int64)0x100000));
    assertEquals(23, Math::NumberOfTrailingZeros((Int64)0x800000));
    assertEquals(20, Math::NumberOfTrailingZeros((Int64)0xF00000));

    assertEquals(24, Math::NumberOfTrailingZeros((Int64)0x1000000));
    assertEquals(27, Math::NumberOfTrailingZeros((Int64)0x8000000));
    assertEquals(24, Math::NumberOfTrailingZeros((Int64)0xF000000));

    assertEquals(28, Math::NumberOfTrailingZeros((Int64)0x10000000));
    assertEquals(31, Math::NumberOfTrailingZeros((Int64)0x80000000));
    assertEquals(28, Math::NumberOfTrailingZeros((Int64)0xF0000000));
}

void test_doubleToInt64BitsD()
{
    printf("\n===Test DoubleToInt64Bits===\n");
    using Elastos::Core::Math;
    assertEquals(0x7ff8000000000000L, Math::DoubleToInt64Bits(Math::DOUBLE_NAN));
    assertEquals(0x7ff0000000000000L, Math::DoubleToInt64Bits(Math::DOUBLE_POSITIVE_INFINITY));
    assertEquals(0xfff0000000000000L, Math::DoubleToInt64Bits(Math::DOUBLE_NEGATIVE_INFINITY));
}

void test_doubleToRawInt64BitsD()
{
    printf("\n===Test DoubleToRawInt64Bits===\n");
    using Elastos::Core::Math;
    assertEquals(0x7ff8000000000000L, Math::DoubleToRawInt64Bits(Math::DOUBLE_NAN));
    assertEquals(0x7ff0000000000000L, Math::DoubleToRawInt64Bits(Math::DOUBLE_POSITIVE_INFINITY));
    assertEquals(0xfff0000000000000L, Math::DoubleToRawInt64Bits(Math::DOUBLE_NEGATIVE_INFINITY));
}

struct Tuple {
public:
    Int32 result;
    Int32 value;
    Int32 factor;

    Tuple()
        : result(0)
        , value(0)
        , factor(0)
    {}

    Tuple(Int32 r, Int32 v, Int32 f)
        : result(r)
        , value(v)
        , factor(f)
    {}
};

struct TupleL {
public:
    Int64 result;
    Int64 value;
    Int64 factor;

    TupleL()
        : result(0)
        , value(0)
        , factor(0)
    {}

    TupleL(Int64 r, Int64 v, Int64 f)
        : result(r)
        , value(v)
        , factor(f)
    {}
};

void test_shiftIntBits_II()
{
    printf("\n===Test test_shiftIntBits_II===\n");
    using Elastos::Core::Math;
    static const Int32 LENGTH = 8;
    Tuple TUPLES[LENGTH] = {
        // sub-normal to sub-normal
        {0x00000000, 0x00000001, -1},
        // round to even
        {0x00000002, 0x00000003, -1},
        // round to even
        {0x00000001, 0x00000005, -3},
        // round to infinity
        {0x00000002, 0x0000000d, -3},
        // round to infinity

        // normal to sub-normal
        {0x00000002, 0x01a00000, -24},
        // round to even
        {0x00000004, 0x01e00000, -24},
        // round to even
        {0x00000003, 0x01c80000, -24},
        // round to infinity
        {0x00000004, 0x01e80000, -24},
    // round to infinity
    };

    for (Int32 i = 0; i < LENGTH; ++i) {
        Tuple& tuple = TUPLES[i];
        assertEquals(tuple.result, Math::FloatToInt32Bits(
            Math::Scalb(Math::Int32BitsToFloat(tuple.value), tuple.factor)));
        assertEquals(tuple.result, Math::FloatToInt32Bits(
            -Math::Scalb(-Math::Int32BitsToFloat(tuple.value), tuple.factor)));
    }
}

void test_shiftLongBits_LL()
{
    printf("\n===Test test_shiftLongBits_LL===\n");
    using Elastos::Core::Math;
    static const Int32 LENGTH = 8;
    TupleL TUPLES[LENGTH] = {
        // sub-normal to sub-normal
        {0x00000000L, 0x00000001L, -1},
        //round to even
        {0x00000002L, 0x00000003L, -1},
        //round to even
        {0x00000001L, 0x00000005L, -3},
        //round to infinity
        {0x00000002L, 0x0000000dL, -3},
        //round to infinity

        // normal to sub-normal
        {0x0000000000000002L, 0x0034000000000000L, -53}, // round to even
        {0x0000000000000004L, 0x003c000000000000L, -53}, // round to even
        {0x0000000000000003L, 0x0035000000000000L, -53}, // round to infinity
        {0x0000000000000004L, 0x003d000000000000L, -53}, // round to infinity
    };

    for (Int32 i = 0; i < LENGTH; ++i) {
        TupleL& tuple = TUPLES[i];
        assertEquals(tuple.result, Math::DoubleToInt64Bits(
            Math::Scalb(Math::Int64BitsToDouble(tuple.value), tuple.factor)));
        assertEquals(tuple.result, Math::DoubleToInt64Bits(
            -Math::Scalb(-Math::Int64BitsToDouble(tuple.value), tuple.factor)));
    }
}

void test_scalb_DI()
{
    printf("\n===Test test_scalb_DI===\n");

    using Elastos::Core::Math;

    // result is normal
    assertEquals(4.1422946304E7, Math::Scalb(1.2345, 25));
    assertEquals(3.679096698760986E-8, Math::Scalb(1.2345, -25));
    assertEquals(1.2345, Math::Scalb(1.2345, 0));
    assertEquals(7868514.304, Math::Scalb(0.2345, 25));

    Double normal = Math::Scalb(0.2345, -25);
    assertEquals(6.98864459991455E-9, normal);
    // precision kept
    assertEquals(0.2345, Math::Scalb(normal, 25));

    assertEquals(0.2345, Math::Scalb(0.2345, 0));
    assertEquals(-4.1422946304E7, Math::Scalb(-1.2345, 25));
    assertEquals(-6.98864459991455E-9, Math::Scalb(-0.2345, -25));
    assertEquals(2.0, Math::Scalb(Math::DOUBLE_MIN_NORMAL / 2, 1024));
    assertEquals(64.0, Math::Scalb(Math::DOUBLE_MIN_VALUE, 1080));
    assertEquals(234.0, Math::GetExponent(Math::Scalb(1.0, 234)));
    assertEquals(3.9999999999999996, Math::Scalb(Math::DOUBLE_MAX_VALUE,
            Math::DOUBLE_MIN_EXPONENT));

    // result is near infinity
    double halfMax = Math::Scalb(1.0, Math::DOUBLE_MAX_EXPONENT);
    assertEquals(8.98846567431158E307, halfMax);
    assertEquals(Math::DOUBLE_MAX_VALUE, halfMax - Math::Ulp(halfMax) + halfMax);
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, halfMax + halfMax);
    assertEquals(1.7976931348623155E308, Math::Scalb(1.0 - Math::Ulp(1.0),
            Math::DOUBLE_MAX_EXPONENT + 1));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(1.0 - Math::Ulp(1.0),
            Math::DOUBLE_MAX_EXPONENT + 2));

    halfMax = Math::Scalb(-1.0, Math::DOUBLE_MAX_EXPONENT);
    assertEquals(-8.98846567431158E307, halfMax);
    assertEquals(-Math::DOUBLE_MAX_VALUE, halfMax + Math::Ulp(halfMax) + halfMax);
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, halfMax + halfMax);

    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(0.345, 1234));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(44.345E102, 934));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, Math::Scalb(-44.345E102, 934));

    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(
            Math::DOUBLE_MIN_NORMAL / 2, 4000));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(Math::DOUBLE_MIN_VALUE,
            8000));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(Math::DOUBLE_MAX_VALUE, 1));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(
            Math::DOUBLE_POSITIVE_INFINITY, 0));
    assertEquals(Math::DOUBLE_POSITIVE_INFINITY, Math::Scalb(
            Math::DOUBLE_POSITIVE_INFINITY, -1));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, Math::Scalb(
            Math::DOUBLE_NEGATIVE_INFINITY, -1));
    assertEquals(Math::DOUBLE_NEGATIVE_INFINITY, Math::Scalb(
            Math::DOUBLE_NEGATIVE_INFINITY, Math::DOUBLE_MIN_EXPONENT));

    // result is subnormal/zero
    Int64 posZeroBits = Math::DoubleToInt64Bits(+0.0);
    Int64 negZeroBits = Math::DoubleToInt64Bits(-0.0);
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(+0.0,
            Math::INT32_MAX_VALUE)));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(+0.0, -123)));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(+0.0, 0)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(-0.0, 123)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(-0.0,
            Math::INT32_MIN_VALUE)));

    assertEquals(Math::DOUBLE_MIN_VALUE, Math::Scalb(1.0, -1074));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(1.0, -1075)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(-1.0,
            -1075)));

    // precision lost
    assertEquals(Math::Scalb(21.405, -1078), Math::Scalb(21.405, -1079));
    assertEquals(Math::DOUBLE_MIN_VALUE, Math::Scalb(21.405, -1079));
    assertEquals(-Math::DOUBLE_MIN_VALUE, Math::Scalb(-21.405, -1079));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(21.405,
            -1080)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(-21.405,
            -1080)));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            Math::DOUBLE_MIN_VALUE, -1)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MIN_VALUE, -1)));
    assertEquals(Math::DOUBLE_MIN_VALUE, Math::Scalb(Math::DOUBLE_MIN_NORMAL, -52));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            Math::DOUBLE_MIN_NORMAL, -53)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MIN_NORMAL, -53)));
    assertEquals(Math::DOUBLE_MIN_VALUE, Math::Scalb(Math::DOUBLE_MAX_VALUE, -2098));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            Math::DOUBLE_MAX_VALUE, -2099)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MAX_VALUE, -2099)));
    assertEquals(Math::DOUBLE_MIN_VALUE, Math::Scalb(Math::DOUBLE_MIN_NORMAL / 3, -51));
    assertEquals(posZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            Math::DOUBLE_MIN_NORMAL / 3, -52)));
    assertEquals(negZeroBits, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MIN_NORMAL / 3, -52)));
    double subnormal = Math::Scalb(Math::DOUBLE_MIN_NORMAL / 3, -25);
    assertEquals(2.2104123E-316, subnormal);
    // precision lost
    assert(Math::DOUBLE_MIN_NORMAL / 3 != Math::Scalb(subnormal, 25));

    // NaN
    assert(Math::IsNaN(Math::Scalb(Math::DOUBLE_NAN, 1)));
    assert(Math::IsNaN(Math::Scalb(Math::DOUBLE_NAN, 0)));
    assert(Math::IsNaN(Math::Scalb(Math::DOUBLE_NAN, -120)));

    assertEquals(1283457024L, Math::DoubleToInt64Bits(
        Math::Scalb(Math::DOUBLE_MIN_VALUE * 153, 23)));
    assertEquals(-9223372035571318784L, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MIN_VALUE * 153, 23)));
    assertEquals(36908406321184768, Math::DoubleToInt64Bits(Math::Scalb(
            Math::DOUBLE_MIN_VALUE * 153, 52)));
    assertEquals(-9186463630533591040L, Math::DoubleToInt64Bits(Math::Scalb(
            -Math::DOUBLE_MIN_VALUE * 153, 52)));

    // test for exception
    // try {
    //     Math::Scalb((Double) null, (Integer) null);
    //     fail("Should throw NullPointerException");
    // } catch (NullPointerException e) {
    //     // Expected
    // }
    // try {
    //     Math::Scalb(1.0, (Integer) null);
    //     fail("Should throw NullPointerException");
    // } catch (NullPointerException e) {
    //     // Expected
    // }
    // try {
    //     Math::Scalb((Double) null, 1);
    //     fail("Should throw NullPointerException");
    // } catch (NullPointerException e) {
    //     // Expected
    // }

    long b1em1022 = 0x0010000000000000L; // bit representation of
                                            // Math::DOUBLE_MIN_NORMAL
    long b1em1023 = 0x0008000000000000L; // bit representation of half of
                                            // Double.MIN_NORMAL
    // assert exact identity
    assertEquals(b1em1023, Math::DoubleToInt64Bits(
        Math::Scalb(Math::Int64BitsToDouble(b1em1022), -1)));
}

void testMathClass()
{
    using Elastos::Core::Math;

    test_numberOfLeadingZerosJ();
    test_numberOfTrailingZerosJ();

    test_numberOfLeadingZerosI();
    test_numberOfTrailingZerosI();

    test_doubleToInt64BitsD();
    test_doubleToRawInt64BitsD();

    // test_scalb_DI();


    // test_shiftIntBits_II();
    // test_shiftLongBits_LL();

    for (Int32 i = 0; i < 10; i++) {
        Double d = Math::Random();
        printf("Random %d: %f\n", i, d);
        assert(d >= 0.0 && d < 1.0 && "Random failed");
    }
}

//==============================================================================

void testMath()
{
    // testBigDecimal();

    // testBigInteger();
    testStringToIntegral();
    testIntegralToString();
    // testRealToString();
    // testMathClass();
}

int main(int argc, char *argv[]) {
	printf("==== call testMath ====\n");
	testMath();
	printf("\n==== call testMath end ====\n");

	return 0;
}
