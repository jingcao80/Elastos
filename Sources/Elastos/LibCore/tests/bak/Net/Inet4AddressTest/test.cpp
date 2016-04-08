#include "test.h"
#include <stdio.h>
#include <elautoptr.h>
#include <malloc.h>

CTest::CTest()
{
}

CTest::~CTest()
{
}

// Num: Test1
int CTest::test_isMulticastAddress(int argc, char* argv[])
{
    // Create 2 IP v4 addresses and call "isMulticastAddress()"
    // result should return true if the first 4 bits of the
    // address are: 1110, false otherwise
    // Make 1 address with 1110, and 1 without
    String addrName("224.0.0.0"); // a multicast addr 1110 = 224-239
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    AutoPtr<IInet4Address> inet4Address = (IInet4Address*)addr.Get();
    Boolean flag = FALSE;
    inet4Address->IsMulticastAddress(&flag);
    assert(flag == TRUE);

    addrName = String("239.255.255.255"); // a multicast addr 1110 = 224-239
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet4Address = (IInet4Address*)addr.Get();
    inet4Address->IsMulticastAddress(&flag);
    assert(flag == TRUE);

    addrName = String("42.42.42.42"); // a non-multicast address
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    inet4Address = (IInet4Address*)addr.Get();
    inet4Address->IsMulticastAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test2
int CTest::test_isAnyLocalAddress(int argc, char* argv[])
{
    String addrName("0.0.0.0"); // a multicast addr 1110 = 224-239
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsAnyLocalAddress(&flag);
    assert(flag == TRUE);

    addrName = String("127.0.0.1");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsAnyLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test3
int CTest::test_isLoopbackAddress(int argc, char* argv[])
{
    String addrName(String("127.0.0.0")); // a loopback address should be 127.d.d.d
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsLoopbackAddress(&flag);
    assert(flag == TRUE);

    addrName = String("127.42.42.42"); // a loopback address should be
    // 127.d.d.d
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsLoopbackAddress(&flag);
    assert(flag == TRUE);

    addrName = String("42.42.42.42"); // a loopback address should be
    // 127.d.d.d
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsLoopbackAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test4
int CTest::test_isLinkLocalAddress(int argc, char* argv[])
{
    String addrName("42.42.42.42");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    Boolean flag = FALSE;
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsLinkLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test5
int CTest::test_isSiteLocalAddress(int argc, char* argv[])
{
    String addrName = String("42.42.42.42");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsSiteLocalAddress(&flag);
    assert(flag == FALSE);
}

// Num: Test6
int CTest::test_isMCGlobal(int argc, char* argv[])
{
    // Create an IPv4 mulitcast address. It should return
    // false for globabl mutlicast. There are no valid IPv4
    // global multicast addresses
    String addrName("224.0.0.0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsMCGlobal(&flag);
    assert(flag == FALSE);

    addrName = String("224.0.0.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCGlobal(&flag);
    assert(flag == FALSE);

    addrName = String("224.0.1.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCGlobal(&flag);
    assert(flag == TRUE);

    addrName = String("238.255.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCGlobal(&flag);
    assert(flag == TRUE);

    addrName = String("239.0.0.0"); // a multicast addr 1110
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCGlobal(&flag);
    assert(flag == FALSE);

    addrName = String("239.191.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCGlobal(&flag);
    assert(flag == FALSE);
}

//Num: Test7
int CTest::test_isMCNodeLocal(int argc, char* argv[])
{
    // Create an IPv4 mulitcast address. It should return
    // false for node-local mutlicast. There are no valid IPv4
    // node-local multicast addresses

    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    String addrName("224.42.42.42");
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsMCNodeLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCNodeLocal(&flag);
    assert(flag == FALSE);
}

//Num: Test8
int CTest::test_isMCLinkLocal(int argc, char* argv[])
{
    // Create an IPv4 mulitcast address. It should return
    // false for node-local mutlicast. There are no valid IPv4
    // node-local multicast addresses
    String addrName("224.0.0.0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    addrName = String("224.0.0.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCLinkLocal(&flag);
    assert(flag == TRUE);

    addrName = String("224.0.1.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCLinkLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCLinkLocal(&flag);
    assert(flag == FALSE);
}

//Num: Test9
int CTest::test_isMCSiteLocal(int argc, char* argv[])
{
    // Create an IPv4 mulitcast address. It should return
    // false for site-local mutlicast. There are no valid IPv4
    // site-local multicast addresses
    String addrName("240.0.0.0");
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.0.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCSiteLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.255.0.0");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    addrName = String("239.255.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCSiteLocal(&flag);
    assert(flag == TRUE);

    addrName = String("239.255.2.2");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCSiteLocal(&flag);
    assert(flag == TRUE);
}

//Num: Test10
int CTest::test_isMCOrgLocal(int argc, char* argv[])
{
    // Create an IPv4 mulitcast address. It should return
    // false for organization-local mutlicast. There are no valid IPv4
    // organization-local multicast addresses
    String addrName("239.191.255.255"); // a multicast addr 1110
    AutoPtr<IInetAddressHelper> inetAddressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
    AutoPtr<IInetAddress> addr;
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    Boolean flag = FALSE;
    addr->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.252.0.0"); // a multicast addr 1110
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCOrgLocal(&flag);
    assert(flag == FALSE);

    addrName = String("239.192.0.0"); // a multicast addr 1110
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCOrgLocal(&flag);
    assert(flag == TRUE);

    addrName = String("239.195.255.255");
    inetAddressHelper->GetByName(addrName, (IInetAddress**)&addr);
    addr->IsMCOrgLocal(&flag);
    assert(flag == TRUE);
}