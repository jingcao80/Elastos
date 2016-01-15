#include "test.h"
#include <stdio.h>
#include <elautoptr.h>
#include <malloc.h>

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

void CTest::setUp()
{
    PFL_EX("CTest::test_parseNumericAddress");
    // initialize Byte[]
    LOOPBACK6_BYTES = ArrayOf<Byte>::Alloc(16);
    for (int i = 0; i < 16; ++i) {
        if (i != 15) {
           LOOPBACK6_BYTES->Set(i, Byte(0));
        }
        if (i == 15) {
            LOOPBACK6_BYTES->Set(i, Byte(1));
        }
    }

    //initialize String[]
    INVALID_IPv4_NUMERIC_ADDRESSES = ArrayOf<String>::Alloc(20);
     // IPv4 addresses may not be surrounded by square brackets.
    String str0 = String("[127.0.0.1]");

    // Trailing dots are not allowed.
    String str1 = String("1.2.3.4.");
    // Nor is any kind of trailing junk.
    String str2 = String("1.2.3.4hello");

    // Out of range.
    String str3 = String("256.2.3.4");
    String str4 = String("1.256.3.4");
    String str5 = String("1.2.256.4");
    String str6 = String("1.2.3.256");

    // Deprecated.
    String str7 = String("1.2.3");
    String str8 = String("1.2");
    String str9 = String("1");
    String str10 = String("1234");
    String str11 = String("0"); // Single out the deprecated form of the ANY address.

    // Hex.
    String str12 = String("0x1.0x2.0x3.0x4");
    String str13 = String("0x7f.0x00.0x00.0x01");
    String str14 = String("7f.0.0.1");

    // Octal.
    String str15 = String("0177.00.00.01"); // Historically, this would have been interpreted as 127.0.0.1.

    // Negative numbers.
    String str16 = String("-1.0.0.1");
    String str17 = String("1.-1.0.1");
    String str18 = String("1.0.-1.1");
    String str19 = String("1.0.0.-1");

    INVALID_IPv4_NUMERIC_ADDRESSES->Set(0, str0);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(1, str1);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(2, str2);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(3, str3);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(4, str4);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(5, str5);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(6, str6);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(7, str7);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(8, str8);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(9, str9);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(10, str10);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(11, str11);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(12, str12);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(13, str13);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(14, str14);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(15, str15);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(16, str16);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(17, str17);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(18, str18);
    INVALID_IPv4_NUMERIC_ADDRESSES->Set(19, str19);
}

void CTest::tearDown()
{
}

// Num: Test1
int CTest::test_parseNumericAddress(int argc, char* argv[])
{
    PFL_EX("CTest::test_parseNumericAddress");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);

    PFL_EX("CTest::test_parseNumericAddress");
    AutoPtr<IInetAddress> outAddress;
    String finalStr;
    // Regular IPv4.
    const String strIPv4 = String("1.2.3.4");
    inetAddressHelper->ParseNumericAddress(strIPv4, (IInetAddress**)&outAddress);
    outAddress->ToString(&finalStr);
    assert(finalStr.Equals(String("/1.2.3.4")));

    // Regular IPv6.
    const String strIPv6 = String("2001:4860:800d::68");
    inetAddressHelper->ParseNumericAddress(strIPv6, (IInetAddress**)&outAddress);
    outAddress->ToString(&finalStr);
    assert(finalStr.Equals(String("/2001:4860:800d::68")));

    // Optional square brackets around IPv6 addresses, including mapped IPv4.
    const String strIPv6_1 = String("[2001:4860:800d::68]");
    inetAddressHelper->ParseNumericAddress(strIPv6_1, (IInetAddress**)&outAddress);
    outAddress->ToString(&finalStr);
    assert(finalStr.Equals(String("/2001:4860:800d::68")));

    const String strIPv6_2 = String("[::ffff:127.0.0.1]");
    inetAddressHelper->ParseNumericAddress(strIPv6_2, (IInetAddress**)&outAddress);
    outAddress->ToString(&finalStr);
    assert(finalStr.Equals(String("/127.0.0.1")));

    //throw exception
    const String exString = String("example.com");
    ECode ec = inetAddressHelper->ParseNumericAddress(exString, (IInetAddress**)&outAddress);
    assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);

    for (int i = 0; i < 20; ++i) {
        String str = (*INVALID_IPv4_NUMERIC_ADDRESSES)[i];
        ec = inetAddressHelper->ParseNumericAddress(str, (IInetAddress**)&outAddress);
        assert(ec == E_ILLEGAL_ARGUMENT_EXCEPTION);
    }
}

// Num: Test2
int CTest::test_isNumeric(int argc, char* argv[])
{
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    Boolean flag = FALSE;

    const String str1 = String("1.2.3.4");
    inetAddressHelper->IsNumeric(str1, &flag);
    assert(TRUE == flag);

    const String str2 = String("127.0.0.1");
    inetAddressHelper->IsNumeric(str2, &flag);
    assert(TRUE == flag);

    const String str3 = String("example.com");
    inetAddressHelper->IsNumeric(str3, &flag);
    assert(FALSE == flag);

    for (int i = 0; i < 20; ++i) {
        String invalid = (*INVALID_IPv4_NUMERIC_ADDRESSES)[i];
        inetAddressHelper->IsNumeric(invalid, &flag);
        assert(FALSE == flag);
    }

}

// Num: Test3
int CTest::test_getByName(int argc, char* argv[])
{
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    ECode ec;
    AutoPtr<IInetAddress> outAddress;
    for (int i = 0; i < 20; ++i) {
        String invalid = (*INVALID_IPv4_NUMERIC_ADDRESSES)[i];
        ec = inetAddressHelper->GetByName(invalid,(IInetAddress**)&outAddress);
        assert(ec == E_IO_EXCEPTION);
    }
}
