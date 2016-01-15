
#include "test.h"

CTest::CTest()
{
}

CTest::~CTest()
{
}


int CTest::test_ConstructorLjava_lang_StringI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_lang_StringI")
    // try {
    AutoPtr<IInetSocketAddress> sa;
    ECode ec = CInetSocketAddress::New(String("127.0.0.1"), -1, (IInetSocketAddress**)&sa);
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    // try {
    ec = CInetSocketAddress::New(String("127.0.0.1"), 65536, (IInetSocketAddress**)&sa);
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::test_ConstructorLInetAddressI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLInetAddressI")
    String validIPAddresses[] = {
        String("::1.2.3.4"),
        String("::"),
        String("::"),
        String("1::0"),
        String("1::"),
        String("::1"),
        String("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"),
        String("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255"),
        String("0:0:0:0:0:0:0:0"),
        String("0:0:0:0:0:0:0.0.0.0"),
        String("127.0.0.1"),
        String("localhost"),
        String("42.42.42.42"),
        String("0.0.0.0")
    };

    String results[] = {
        String("0:0:0:0:0:0:102:304"),
        String("0:0:0:0:0:0:0:0"),
        String("0:0:0:0:0:0:0:0"),
        String("1:0:0:0:0:0:0:0"),
        String("1:0:0:0:0:0:0:0"),
        String("0:0:0:0:0:0:0:1"),
        String("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),
        String("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),
        String("0:0:0:0:0:0:0:0"),
        String("0:0:0:0:0:0:0:0"),
        String("localhost"),
        String("localhost"),
        String("42.42.42.42"),
        String("0.0.0.0")
    };

    String hostname;
    AutoPtr<IInetAddressHelper> iah;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
    for (int i = 0; i < sizeof(validIPAddresses) / sizeof(String); i++) {
        AutoPtr<IInetAddress> ia;
        iah->GetByName(validIPAddresses[i], (IInetAddress**)&ia);
        AutoPtr<IInetSocketAddress> isa;
        CInetSocketAddress::New(ia, 80, (IInetSocketAddress**)&isa);
        Int32 outport = 0;
        isa->GetPort(&outport);
        assert(80 == outport);
        isa->GetHostName(&hostname);
        // assert(results[i] == hostname);
    }

    AutoPtr<IInetSocketAddress> isa;
    AutoPtr<IInetAddress> nullnet;
    CInetSocketAddress::New(nullnet, 80, (IInetSocketAddress**)&isa);
    isa->GetHostName(&hostname);
    assert(String("0.0.0.0") == hostname);

    // try {
    AutoPtr<IInetAddress> ia;
    iah->GetByName(String("localhost"), (IInetAddress**)&ia);
    ECode ec = CInetSocketAddress::New(ia, 65536, (IInetSocketAddress**)&isa);
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    // } catch(IllegalArgumentException expected) {
    // }

    // try {
    ec = CInetSocketAddress::New(ia, -1, (IInetSocketAddress**)&isa);
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    //     fail();
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(65535, (IInetSocketAddress**)&isa);
    String hostname;
    isa->GetHostName(&hostname);
    assert(String("0.0.0.0") == hostname);
    Int32 outport = 0;
    isa->GetPort(&outport);
    assert(65535 == outport);

    // try {
    ECode ec = CInetSocketAddress::New(-1, (IInetSocketAddress**)&isa);
    //     fail();
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    // } catch (IllegalArgumentException  expected) {
    // }

    // try {
    ec = CInetSocketAddress::New(65536, (IInetSocketAddress**)&isa);
    if (ec != NOERROR) {
        printf("Fail~~~~~expected!!!!\n");
    }
    //     fail();
    // } catch (IllegalArgumentException  expected) {
    // }
    return 0;
}

int CTest::test_equals(int argc, char* argv[])
{
    PEL("CTest::test_equals")
    AutoPtr<IInetSocketAddress> isa1;
    CInetSocketAddress::New(1, (IInetSocketAddress**)&isa1);
    AutoPtr<IInetSocketAddress> isa2;
    CInetSocketAddress::New(2, (IInetSocketAddress**)&isa2);
    Boolean isflag = FALSE;
    assert(!Object::Equals(isa1, isa2));
    AutoPtr<IInetSocketAddress> isa3;
    CInetSocketAddress::New(1, (IInetSocketAddress**)&isa3);
    assert(Object::Equals(isa1, isa3));

    AutoPtr<IInetAddress> localhost;
    AutoPtr<IInetAddressHelper> iah;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
    iah->GetByName(String("localhost"), (IInetAddress**)&localhost);
    String hostname;
    localhost->GetHostName(&hostname);
    CInetSocketAddress::New(hostname, 80, (IInetSocketAddress**)&isa1);
    localhost->GetHostAddress(&hostname);
    CInetSocketAddress::New(hostname, 80, (IInetSocketAddress**)&isa2);
    assert(Object::Equals(isa1, isa2));
    return 0;
}

int CTest::test_getAddress(int argc, char* argv[])
{
    PEL("CTest::test_getAddress")
    String validIPAddresses[] = {
        String("::1.2.3.4"),
        String("::"),
        String("::"),
        String("1::0"),
        String("1::"),
        String("::1"),
        String("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"),
        String("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255"),
        String("0:0:0:0:0:0:0:0"),
        String("0:0:0:0:0:0:0.0.0.0"),
        String("127.0.0.1"),
        String("localhost"),
        String("42.42.42.42"),
        String("0.0.0.0")
    };
    for (int i = 0; i < sizeof(validIPAddresses) / sizeof(String); i++) {
        AutoPtr<IInetAddress> ia;
        AutoPtr<IInetAddressHelper> iah;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
        iah->GetByName(validIPAddresses[i], (IInetAddress**)&ia);
        AutoPtr<IInetSocketAddress> isa;
        CInetSocketAddress::New(ia, 0, (IInetSocketAddress**)&isa);
        AutoPtr<IInetAddress> outia;
        isa->GetAddress((IInetAddress**)&outia);
        Boolean isflag = FALSE;
        assert((ia->Equals(outia, &isflag), isflag));
    }
    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::New(NULL, 0, (IInetSocketAddress**)&isa);
    AutoPtr<IInetAddress> outia;
    isa->GetAddress((IInetAddress**)&outia);
    assert(outia != NULL);
    return 0;
}

int CTest::test_hashCode(int argc, char* argv[])
{
    PEL("CTest::test_hashCode")
    AutoPtr<IInetAddress> localhost;
    AutoPtr<IInetAddressHelper> iah;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
    iah->GetByName(String("localhost"), (IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> isa1;
    String hostname;
    localhost->GetHostName(&hostname);
    CInetSocketAddress::New(hostname, 8080, (IInetSocketAddress**)&isa1);
    AutoPtr<IInetSocketAddress> isa2;
    localhost->GetHostAddress(&hostname);
    CInetSocketAddress::New(hostname, 8080, (IInetSocketAddress**)&isa2);
    Int32 hash1 = Object::GetHashCode(isa1);
    Int32 hash2 = Object::GetHashCode(isa2);
    assert(hash1 == hash2);

    AutoPtr<IInetSocketAddress> isa3;
    CInetSocketAddress::New(String("0.0.0.0"), 8080, (IInetSocketAddress**)&isa3);
    hash2 = Object::GetHashCode(isa3);
    assert(hash1 != hash2);
    return 0;
}

int CTest::test_isUnresolved(int argc, char* argv[])
{
    PEL("CTest::test_isUnresolved")
    AutoPtr<IInetSocketAddress> isa1;
    CInetSocketAddress::New(String("localhost"), 80, (IInetSocketAddress**)&isa1);
    Boolean isflag = FALSE;
    assert(!(isa1->IsUnresolved(&isflag), isflag));

    AutoPtr<IInetSocketAddress> sockAddr;
    CInetSocketAddress::New(String("unknown.host"), 1000, (IInetSocketAddress**)&sockAddr);
    assert((sockAddr->IsUnresolved(&isflag), isflag));
    return 0;
}

int CTest::test_getHostString(int argc, char* argv[])
{
    // When we have a hostname, we'll get it back because that doesn't cost a DNS lookup...
    PEL("CTest::test_getHostString")
    AutoPtr<IInetSocketAddress> hasHostname;
    assert(0 && "TODO");
    // CInetSocketAddress::CreateUnresolved(String("some host"), 1234, (IInetSocketAddress**)&hasHostname);
    String hoststring;
    hasHostname->GetHostString(&hoststring);
    assert(String("some host") == hoststring);
    hasHostname->GetHostName(&hoststring);
    assert(String("some host") == hoststring);
    // When we don't have a hostname, whether or not we do the reverse lookup is the difference
    // between getHostString and getHostName...
    AutoPtr<IInetAddress> address;
    AutoPtr<IInetAddressHelper> iah;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
    AutoPtr< ArrayOf<Byte> > arrbyte = ArrayOf<Byte>::Alloc(4);
    (*arrbyte)[0] = 127;
    (*arrbyte)[1] = 0;
    (*arrbyte)[2] = 0;
    (*arrbyte)[3] = 1;
    iah->GetByAddress(arrbyte, (IInetAddress**)&address);
    AutoPtr<IInetSocketAddress> noHostname;
    CInetSocketAddress::New(address, 1234, (IInetSocketAddress**)&noHostname);
    noHostname->GetHostString(&hoststring);
    assert(String("127.0.0.1") == hoststring);
    noHostname->GetHostName(&hoststring);
    assert(String("localhost") == hoststring);
    return 0;
}
