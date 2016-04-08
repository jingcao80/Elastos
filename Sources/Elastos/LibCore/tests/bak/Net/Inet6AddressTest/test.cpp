#include "test.h"
#include <stdio.h>
#include <elautoptr.h>
#include <malloc.h>

CTest::CTest(){}

CTest::~CTest(){}

// Num: Test1
int CTest::test_isMulticastAddress(int argc, char* argv[])
{
    // IP V6 regular multicast and non-multicast tests
    //
    // Create 2 IP v6 addresses and call "isMulticastAddress()"
    // A prefix of "11111111" means that the address is multicast
    // The first one will be one with the prefix the second without
    String addrName("FFFF::42:42"); // 11111111 = FFFF
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)IInet6Address::Probe(addr);
    Boolean flag = FALSE;
    inet6Address->IsMulticastAddress(&flag);
    assert(flag == TRUE);

    addrName = String("42::42:42"); // an non-multicast address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    addr->IsMulticastAddress(&flag);
    assert(flag == FALSE);

    // // IPv4-compatible IPv6 address tests
    // //
    // // Now create 2 IP v6 addresses that are IP v4 compatable
    // // to IP v6 addresses. The address prefix for a multicast ip v4
    // // address is 1110 for the last 16 bits ::d.d.d.d
    // // We expect these to be false
    addrName = String("::224.42.42.42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMulticastAddress(&flag);
    assert(flag == FALSE);

    addrName = String("::42.42.42.42");// an ipv4 non-multicast address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsMulticastAddress(&flag);
    assert(flag == FALSE);

    // // IPv4-mapped IPv6 address tests
    // //
    // // Now create 2 IP v6 addresses that are IP v4 compatable
    // // to IP v6 addresses. The address prefix for a multicast ip v4
    // // address is 1110 for the last 16 bits ::FFFF:d.d.d.d
    addrName = String("::FFFF:224.42.42.42");
    // 224
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsMulticastAddress(&flag);
    assert(flag == TRUE);

    addrName = String("::FFFF:42.42.42.42");// an ipv4 non-multicast address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsMulticastAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test2
int CTest::test_isAnyLocalAddress(int argc, char* argv[])
{
    // test to ensure that the unspecified address returns tru
    String addrName("::0"); // The unspecified addresss
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsAnyLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("::");// another form of the unspecified address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsAnyLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("::1");// The loopback address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();;
    inet6Address->IsAnyLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test3
int CTest::test_isLoopbackAddress(int argc, char* argv[])
{
    // IP V6 regular address tests for loopback
    // The loopback address for IPv6 is ::1
    String addrName("::1");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == TRUE);

    addrName = String("::2");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag ==  FALSE);

    // a loopback address should be 127.d.d.d
    addrName = String("42:42::42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == FALSE);

    // IPv4-compatible IPv6 address tests
    //
    // Now create 2 IP v6 addresses that are IP v4 compatable
    // to IP v6 addresses. The address prefix for a multicast ip v4
    // address is 1110 for the last 16 bits ::d.d.d.d
    // We expect these to be false, as they are not IPv4 addresses

    // a loopback address should be 127.d.d.d
    addrName = String("::127.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == FALSE);

    addrName = String("::127.42.42.42");// a loopback address should be
    // 127.d.d.d
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == FALSE);

    // a loopback address should be 127.d.d.d
    addrName = String("::42.42.42.42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == FALSE);

    // IPv4-mapped IPv6 address tests
    //
    // Now create 2 IP v6 addresses that are IP v4 compatable
    // to IP v6 addresses. The address prefix for a multicast ip v4
    // address is 1110 for the last 16 bits ::FFFF:d.d.d.d

    // a loopback address should be 127.d.d.d
    addrName = String("::FFFF:127.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == TRUE);

    // a loopback address should be 127.d.d.d
    addrName = String("::FFFF:127.42.42.42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == TRUE);

    // a loopback address should be 127.d.d.d
    addrName = String("::FFFF:42.42.42.42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLoopbackAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test4
int CTest::test_isLinkLocalAddress(int argc, char* argv[])
{
    // IP V6 regular address tests for link local addresses
    //
    // Link local addresses are FE80:: -
    // FEBF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF
    String addrName("FE80::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsLinkLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("FEBF::FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLinkLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("FEC0::1");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLinkLocalAddress(&flag);
    assert(flag == FALSE);

    addrName = String("FD80::1:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLinkLocalAddress(&flag);
    assert(flag == FALSE);

    addrName = String("FE7F::FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsLinkLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test5
int CTest::test_isSiteLocalAddress(int argc, char* argv[])
{
    // IP V6 regular address tests for link local addresses
    //
    // Link local addresses are FEC0::0 through to
    // FEFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF
    String addrName("FEC0::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsSiteLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("FEFF::FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsSiteLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("FEBF::FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsSiteLocalAddress(&flag);
    assert(flag == FALSE);

    addrName = String("FFC0::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsSiteLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test6
int CTest::test_isMCGlobal(int argc, char* argv[])
{
    // IP V6 regular address tests for Mulitcase Global addresses
    //
    // Multicast global addresses are FFxE:/112 where x is
    // a set of flags, and the addition 112 bits make up
    // the global address space
    String addrName("FF0E::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsMCGlobal(&flag);
    assert(flag == TRUE);

    addrName = String("FF0E:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFFE::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFFE:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == TRUE);

    // a sample MC organizational address
    addrName = String("FF08:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == FALSE);

    // a sample MC site address
    addrName = String("FF05:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == FALSE);

    // a sample MC link address
    addrName = String("FF02:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == FALSE);

    // a sample MC Node
    addrName = String("FF01:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == FALSE);

    // IPv4-mapped IPv6 address tests
    addrName = String("::FFFF:224.0.1.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == TRUE);

    addrName = String("::FFFF:238.255.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCGlobal(&flag);
    assert(flag == TRUE);

}

// Num: Test7
int CTest::test_isMCNodeLocal(int argc, char* argv[])
{
    // IP V6 regular address tests for Mulitcase node local addresses
    //
    // Multicast node local addresses are FFx1:/112 where x is
    // a set of flags, and the addition 112 bits make up
    // the global address space
    String addrName("FF01::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == TRUE);

    addrName = String("FF01:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF1::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF1:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == TRUE);

    // a sample MC organizational address
    addrName = String("FF08:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == FALSE);

    // a sample MC site address
    addrName = String("FF05:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == FALSE);

    // a sample MC link address
    addrName = String("FF02:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == FALSE);

    // a sample MC global address
    addrName = String("FF0E:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCNodeLocal(&flag);
    assert(flag == FALSE);
}

// Num: Test8
int CTest::test_isMCLinkLocal(int argc, char* argv[])
{
    String addrName("FF02::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    addrName = String("FF02:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF2::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF2:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    // a sample MC organizational address
    addrName = String("FF08:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == FALSE);

    // a sample MC site address
    addrName = String("FF05:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == FALSE);

    // a sample MC global address
    addrName = String("FF0E:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == FALSE);

    // a sample MC Node
    addrName = String("FF01:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == FALSE);

    // Ipv4-mapped IPv6 addresses
    addrName = String("::FFFF:224.0.0.0"); // a multicast addr 1110
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    addrName = String("::FFFF:224.0.0.255"); // a multicast addr 1110
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCLinkLocal(&flag);
    assert(flag == TRUE);
}

// Num: Test9
int CTest::test_isMCSiteLocal(int argc, char* argv[])
{
    String addrName("FF05::0");
    // IP V6 regular address tests for Multicast site-local addresses
    //
    // Multicast global addresses are FFx5:/112 where x is
    // a set of flags, and the addition 112 bits make up
    // the global address space
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    addrName = String("FF05:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF5::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF5:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    // a sample MC organizational address
    addrName = String("FF08:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    // a sample MC global address
    addrName = String("FF0E:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    // a sample MC link address
    addrName = String("FF02:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    // a sample MC Node
    addrName = String("FF01:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    // IPv4-mapped IPv6 addresses
    addrName = String("::FFFF:239.255.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    addrName = String("::FFFF:239.255.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCSiteLocal(&flag);
    assert(flag == TRUE);
}

// Num: Test10
int CTest::test_isMCOrgLocal(int argc, char* argv[])
{
    // IP V6 regular address tests for Mulitcase organization-local
    // addresses
    //
    // Multicast global addresses are FFxE:/112 where x is
    // a set of flags, and the addition 112 bits make up
    // the global address space
    String addrName("FF08::0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    addrName = String("FF08:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF8::0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    // a currently invalid address as the prefix FFxE
    // is only valid for x = {1,0} as the rest are reserved
    addrName = String("FFF8:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    // a sample MC global address
    addrName = String("FF0E:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    // a sample MC site address
    addrName = String("FF05:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    // a sample MC link address
    addrName = String("FF02:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    // a sample MC Node
    addrName = String("FF01:42:42:42:42:42:42:42");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    // IPv4-mapped IPv6 addresses
    addrName = String("::FFFF:239.192.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    addrName = String("::FFFF:239.195.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsMCOrgLocal(&flag);
    assert(flag == TRUE);
}

// Num: Test11
int CTest::test_isIPv4CompatibleAddress(int argc, char* argv[])
{
    // Tests a number of addresses to see if they are compatable with
    // IPv6 addresses
    String addrName("FFFF::42:42");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet6Address> inet6Address = (IInet6Address*)addr.Get();
    Boolean flag = FALSE;
    inet6Address->IsIPv4CompatibleAddress(&flag);
    assert(flag == FALSE);

    // IPv4-compatible IPv6 address tests
    //
    // Now create 2 IP v6 addresses that are IP v4 compatable
    // to IP v6 addresses.
    addrName = String("::0.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsIPv4CompatibleAddress(&flag);
    assert(flag == TRUE);

    // an ipv4 non-multicast address
    addrName = String("::255.255.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet6Address = (IInet6Address*)addr.Get();
    inet6Address->IsIPv4CompatibleAddress(&flag);
    assert(flag == TRUE);
}

// Num: Test12
int CTest::test_getByNameLjava_lang_String(int argc, char* argv[])
{
    // ones to add "::255.255.255.255", "::FFFF:0.0.0.0",
    // "0.0.0.0.0.0::255.255.255.255", "F:F:F:F:F:F:F:F",
    // "[F:F:F:F:F:F:F:F]"
    AutoPtr<ArrayOf<String> > validIPAddresses = ArrayOf<String>::Alloc(11);
    String str0("::1.2.3.4");
    String str1("::");
    String str2("::");
    String str3("1::0");
    String str4("1::");
    String str5("::1");
    String str6("0");
    String str7("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF");
    String str8("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255");
    String str9("0:0:0:0:0:0:0:0");
    String str10("0:0:0:0:0:0:0.0.0.0");
    validIPAddresses->Set(0, str0);
    validIPAddresses->Set(1, str1);
    validIPAddresses->Set(2, str2);
    validIPAddresses->Set(3, str3);
    validIPAddresses->Set(4, str4);
    validIPAddresses->Set(5, str5);
    validIPAddresses->Set(6, str6);
    validIPAddresses->Set(7, str7);
    validIPAddresses->Set(8, str8);
    validIPAddresses->Set(9, str9);
    validIPAddresses->Set(10, str10);

    AutoPtr<ArrayOf<String> > invalidIPAddresses = ArrayOf<String>::Alloc(1);
    String invStr0("FFFF:FFFF");
    invalidIPAddresses->Set(0, invStr0);

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    Boolean flag = FALSE;
    for (int i = 0; i < validIPAddresses->GetLength(); ++i) {
        inetAddressHelper->GetByName((*validIPAddresses)[i], (IInetAddress**)&addr);

        String istrValidIPAdress = (*validIPAddresses)[i];
        flag = istrValidIPAdress.Equals(String("0"));
        if (!flag) {
            String tempIPAddress("[");
            tempIPAddress.Append(istrValidIPAdress);
            tempIPAddress.Append(String("]"));
            inetAddressHelper->GetByName(tempIPAddress, (IInetAddress**)&addr);
        }
    }

    for (int i = 0; i < invalidIPAddresses->GetLength(); ++i) {
        ECode ec = inetAddressHelper->GetByName((*invalidIPAddresses)[i], (IInetAddress**)&addr);
    }

}

// Num: Test13
int CTest::test_getByAddressLString_BI(int argc, char* argv[])
{
    AutoPtr<IInet6AddressHelper> inet6AddressHelper;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6AddressHelper);
    AutoPtr<IInet6Address> address;
    ECode ec = inet6AddressHelper->GetByAddress(String("123"), NULL, 0, (IInet6Address**)&address);
    assert(ec == E_UNKNOWN_HOST_EXCEPTION);

    AutoPtr<ArrayOf<Byte> > addr1 = ArrayOf<Byte>::Alloc(4);
    addr1->Set(0, (Byte)127);
    addr1->Set(1, (Byte)0);
    addr1->Set(0, (Byte)0);
    addr1->Set(0, (Byte)1);
    ec = inet6AddressHelper->GetByAddress(String("123"), addr1, 0, (IInet6Address**)&address);
    assert(ec == E_UNKNOWN_HOST_EXCEPTION);

    AutoPtr<ArrayOf<Byte> > addr2 = ArrayOf<Byte>::Alloc(16);
    addr2->Set(0, (Byte)0xFE);
    addr2->Set(1, (Byte)0x80);
    addr2->Set(2, (Byte)0);
    addr2->Set(3, (Byte)0);
    addr2->Set(4, (Byte)0);
    addr2->Set(5, (Byte)0);
    addr2->Set(6, (Byte)0);
    addr2->Set(7, (Byte)0);
    addr2->Set(8, (Byte)0x02);
    addr2->Set(9, (Byte)0x11);
    addr2->Set(10, (Byte)0x25);
    addr2->Set(11, (Byte)0xFF);
    addr2->Set(12, (Byte)0xFE);
    addr2->Set(13, (Byte)0xF8);
    addr2->Set(14, (Byte)0x7C);
    addr2->Set(15, (Byte)0xB2);

    // should not throw any exception
    ec = inet6AddressHelper->GetByAddress(String("123"), addr2, 3, (IInet6Address**)&address);
    assert(ec == NOERROR);

    ec = inet6AddressHelper->GetByAddress(String("123"), addr2, 0, (IInet6Address**)&address);
    assert(ec == NOERROR);

    ec = inet6AddressHelper->GetByAddress(String("123"), addr2, -1, (IInet6Address**)&address);
    assert(ec == NOERROR);
}

// Num: Test14
int CTest::test_getByAddressLString_BLNetworkInterface(int argc, char* argv[])
{
    AutoPtr<INetworkInterface> nif;
    AutoPtr<IInet6AddressHelper> inet6AddressHelper;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6AddressHelper);
    AutoPtr<IInet6Address> inet6Address;
    ECode ec = inet6AddressHelper->GetByAddressEx(String("123"), NULL, nif, (IInet6Address**)&inet6Address);
    assert(ec == E_UNKNOWN_HOST_EXCEPTION);

    AutoPtr<ArrayOf<Byte> > addr1 = ArrayOf<Byte>::Alloc(4);
    addr1->Set(0, (Byte)127);
    addr1->Set(1, (Byte)0);
    addr1->Set(2, (Byte)0);
    addr1->Set(3, (Byte)1);
    ec = inet6AddressHelper->GetByAddressEx(String("123"), addr1, nif, (IInet6Address**)&inet6Address);
    assert(ec == E_UNKNOWN_HOST_EXCEPTION);

    AutoPtr<ArrayOf<Byte> > addr2 = ArrayOf<Byte>::Alloc(16);
    addr2->Set(0, (Byte)0xFE);
    addr2->Set(1, (Byte)0x80);
    addr2->Set(2, (Byte)0);
    addr2->Set(3, (Byte)0);
    addr2->Set(4, (Byte)0);
    addr2->Set(5, (Byte)0);
    addr2->Set(6, (Byte)0);
    addr2->Set(7, (Byte)0);
    addr2->Set(8, (Byte)0x02);
    addr2->Set(9, (Byte)0x11);
    addr2->Set(10, (Byte)0x25);
    addr2->Set(11, (Byte)0xFF);
    addr2->Set(12, (Byte)0xFE);
    addr2->Set(13, (Byte)0xF8);
    addr2->Set(14, (Byte)0x7C);
    addr2->Set(15, (Byte)0xB2);
    // should not throw any exception
    ec = inet6AddressHelper->GetByAddressEx(String("123"), addr2, nif, (IInet6Address**)&inet6Address);
    assert(ec == NOERROR);
}

// Num: Test15
int CTest::test_getHostAddress_(int argc, char* argv[])
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(16);
    ipAddress->Set(0, 0);
    ipAddress->Set(1, 0);
    ipAddress->Set(2, 0);
    ipAddress->Set(3, 0);
    ipAddress->Set(4, 0);
    ipAddress->Set(5, 0);
    ipAddress->Set(6, 0);
    ipAddress->Set(8, 0);
    ipAddress->Set(9, 0);
    ipAddress->Set(10, 0);
    ipAddress->Set(11, 0);
    ipAddress->Set(12, 0);
    ipAddress->Set(13, 0);
    ipAddress->Set(14, 0);
    ipAddress->Set(15, 1);
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> ia;
    inetAddressHelper->GetByAddress(ipAddress, (IInetAddress**)&ia);
    String hostAddress;
    ia->GetHostAddress(&hostAddress);
    assert(hostAddress.Equals(String("::1")));
}

// Num: Test16
int CTest::test_getScopeID(int argc, char* argv[])
{
    AutoPtr<IInet6AddressHelper> inet6AddressHelper;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6AddressHelper);
    AutoPtr<ArrayOf<Byte> > addr = ArrayOf<Byte>::Alloc(16);
    addr->Set(0, (Byte)0xFE);
    addr->Set(1, (Byte)0x80);
    addr->Set(2, (Byte)0);
    addr->Set(3, (Byte)0);
    addr->Set(4, (Byte)0);
    addr->Set(5, (Byte)0);
    addr->Set(6, (Byte)0);
    addr->Set(7, (Byte)0);
    addr->Set(8, (Byte)0x02);
    addr->Set(9, (Byte)0x11);
    addr->Set(10, (Byte)0x25);
    addr->Set(11, (Byte)0xFF);
    addr->Set(12, (Byte)0xFE);
    addr->Set(13, (Byte)0xF8);
    addr->Set(14, (Byte)0x7C);
    addr->Set(15, (Byte)0xB2);

    AutoPtr<IInet6Address> v6ia;
    inet6AddressHelper->GetByAddress(String("123"), addr, 3, (IInet6Address**)&v6ia);
    Int32 scopeID;
    v6ia->GetScopeId(&scopeID);
    assert(3 == scopeID);

    inet6AddressHelper->GetByAddress(String("123"), addr, 0, (IInet6Address**)&v6ia);
    v6ia->GetScopeId(&scopeID);
    assert(0 == scopeID);

    inet6AddressHelper->GetByAddress(String("123"), addr, -1, (IInet6Address**)&v6ia);
    v6ia->GetScopeId(&scopeID);
    assert(0 == scopeID);

}

// Num: Test17
int CTest::test_getScopedInterface(int argc, char* argv[])
{
    AutoPtr<ArrayOf<Byte> > addr = ArrayOf<Byte>::Alloc(16);
    addr->Set(0, (Byte)0xFE);
    addr->Set(1, (Byte)0x80);
    addr->Set(2, (Byte)0x09);
    addr->Set(3, (Byte)0xb5);
    addr->Set(4, (Byte)0x6b);
    addr->Set(5, (Byte)0xa4);
    addr->Set(6, (Byte)0);
    addr->Set(7, (Byte)0);
    addr->Set(8, (Byte)0);
    addr->Set(9, (Byte)0);
    addr->Set(10, (Byte)0);
    addr->Set(11, (Byte)0);
    addr->Set(12, (Byte)0x09);
    addr->Set(13, (Byte)0xb5);
    addr->Set(14, (Byte)0x6b);
    addr->Set(15, (Byte)0xa4);

    AutoPtr<IInet6AddressHelper> inet6AddressHelper;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6AddressHelper);
    AutoPtr<IInet6Address> v6Addr;
    inet6AddressHelper->GetByAddressEx(String("123"), addr, NULL, (IInet6Address**)&v6Addr);
    AutoPtr<INetworkInterface> scopedInter;
    v6Addr->GetScopedInterface((INetworkInterface**)&scopedInter);
    assert(scopedInter == NULL);
}

// Num: Test18
int CTest::test_hashCode(int argc, char* argv[])
{
    AutoPtr<ArrayOf<Byte> > addr = ArrayOf<Byte>::Alloc(16);
    addr->Set(0, (Byte)0xFE);
    addr->Set(1, (Byte)0x80);
    addr->Set(2, (Byte)0);
    addr->Set(3, (Byte)0);
    addr->Set(4, (Byte)0);
    addr->Set(5, (Byte)0);
    addr->Set(6, (Byte)0);
    addr->Set(7, (Byte)0);
    addr->Set(8, (Byte)0x02);
    addr->Set(9, (Byte)0x11);
    addr->Set(10, (Byte)0x25);
    addr->Set(11, (Byte)0xFF);
    addr->Set(12, (Byte)0xFE);
    addr->Set(13, (Byte)0xF8);
    addr->Set(14, (Byte)0x7C);
    addr->Set(15, (Byte)0xB2);

    AutoPtr<IInet6AddressHelper> inet6AddressHelper;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&inet6AddressHelper);
    AutoPtr<IInet6Address> address1;
    AutoPtr<IInet6Address> address2;
    inet6AddressHelper->GetByAddress(String("123"), addr, 0, (IInet6Address**)&address1);
    inet6AddressHelper->GetByAddress(String("1234"), addr, 0, (IInet6Address**)&address2);
    Int32 hashCode1, hashCode2;
    PFL_EX("CTest::test_hashCode.");
    address1->GetHashCode(&hashCode1);
    address2->GetHashCode(&hashCode2);
    PFL_EX("CTest::test_hashCode, hashCode1=%d, hashCode2=%d", hashCode1, hashCode2);
    assert(hashCode1 == hashCode2);
}