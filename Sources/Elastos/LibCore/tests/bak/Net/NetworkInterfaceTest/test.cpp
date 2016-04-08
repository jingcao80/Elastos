#include "test.h"
#include <elastos/StringUtils.h>
#include <elastos/Thread.h>

using namespace Elastos::Net;
using namespace Elastos::Utility;
using namespace Elastos::Core;

//==========================================================
// CTest::
//==========================================================

CTest::CTest()
{
}

CTest::~CTest()
{
}

int CTest::testIPv6(int argc, char* argv[])
{
    AutoPtr<INetworkInterfaceHelper> help;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&help);
    AutoPtr<INetworkInterface> lo;
    help->GetByName(String("lo"), (INetworkInterface**)&lo);
    AutoPtr<IEnumeration> enu;
    lo->GetInetAddresses((IEnumeration**)&enu);
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    Boolean hasNext;
    while (enu->HasMoreElements(&hasNext), hasNext) {
        AutoPtr<IInterface> p;
        enu->NextElement((IInterface**)&p);
        Boolean b;
        al->Add(p, &b);
    }

    AutoPtr<ISet> actual;
    CHashSet::New((ICollection*)al, (ISet**)&actual);

    AutoPtr<ISet> expected;
    CHashSet::New((ISet**)&expected);
    Boolean b = FALSE;
    AutoPtr<IInet4AddressHelper> net4Hlp;
    CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&net4Hlp);
    AutoPtr<IInetAddress> loopback;
    net4Hlp->GetLOOPBACK((IInetAddress**)&loopback);
    expected->Add(loopback, &b);
    AutoPtr<IInet6AddressHelper> net6Hlp;
    CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&net6Hlp);
    AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(16);
    for (int i = 0; i < 15; ++i)
    {
        arr->Set(i, 0);
    }
    arr->Set(15, 1);
    AutoPtr<IInet6Address> addr;
    net6Hlp->GetByAddressEx(String("localhost"), arr, NULL, (IInet6Address**)&addr);
    expected->Add(addr, &b);

    expected->Equals(actual, &b);
    assert(b);
    return 0;
}

int CTest::testLoopback(int argc, char* argv[])
{
    // We know lo shouldn't have a hardware address or an IPv4 broadcast address.
    AutoPtr<INetworkInterfaceHelper> help;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&help);
    AutoPtr<INetworkInterface> lo;
    help->GetByName(String("lo"), (INetworkInterface**)&lo);
    AutoPtr<ArrayOf<Byte> > hAddr;
    lo->GetHardwareAddress((ArrayOf<Byte>**)&hAddr);
    assert(hAddr == NULL);
    AutoPtr<IList> loAddrs;
    lo->GetInterfaceAddresses((IList**)&loAddrs);
    Int32 s;
    loAddrs->GetSize(&s);
    for (int i = 0; i < s; ++i)
    {
        AutoPtr<IInterfaceAddress> ia;
        loAddrs->Get(i, (IInterface**)&ia);
        AutoPtr<IInetAddress> cast;
        ia->GetBroadcast((IInetAddress**)&cast);
        assert(cast == NULL);
    }

    // But eth0, if it exists, should...
    AutoPtr<INetworkInterface> eth0;
    help->GetByName(String("eth0"), (INetworkInterface**)&eth0);
    if (eth0 != NULL) {
        AutoPtr<ArrayOf<Byte> > addr;
        eth0->GetHardwareAddress((ArrayOf<Byte>**)&addr);
        assert(addr != NULL);
        assert(6 == addr->GetLength());
        AutoPtr<IList> etAddrs;
        eth0->GetInterfaceAddresses((IList**)&etAddrs);
        Int32 s;
        etAddrs->GetSize(&s);
        for (int i = 0; i < s; ++i)
        {
            AutoPtr<IInterfaceAddress> ia;
            etAddrs->Get(i, (IInterface**)&ia);
            AutoPtr<IInetAddress> addr;
            ia->GetAddress((IInetAddress**)&addr);
            if (addr->Probe(EIID_IInet4Address) != NULL) {
                AutoPtr<IInetAddress> cast;
                ia->GetBroadcast((IInetAddress**)&cast);
                assert(cast != NULL);
            }
        }
    }
    return 0;
}

int CTest::testDumpAll(int argc, char* argv[])
{
    AutoPtr<ISet> allNames;
    CHashSet::New((ISet**)&allNames);
    AutoPtr<ISet> allIndexes;
    CHashSet::New((ISet**)&allIndexes);
    AutoPtr<INetworkInterfaceHelper> help;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&help);
    AutoPtr<IEnumeration> interfaces;
    help->GetNetworkInterfaces((IEnumeration**)&interfaces);
    AutoPtr<IArrayList> al;
    CArrayList::New((IArrayList**)&al);
    Boolean hasNext;
    while ((interfaces->HasMoreElements(&hasNext), hasNext)) {
        AutoPtr<IInterface> p;
        interfaces->NextElement((IInterface**)&p);
        Boolean b;
        al->Add(p, &b);
    }
    Int32 size;
    al->GetSize(&size);
    for (int i = 0;i < size; i++) {
        AutoPtr<INetworkInterface> nif;
        al->Get(i, (IInterface**)&nif);
//        System.err.println(nif);
//        System.err.println(nif.getInterfaceAddresses());
        AutoPtr<IList> addrs;
        nif->GetInterfaceAddresses((IList**)&addrs);
        Boolean b;
        nif->IsUp(&b);
        String flags = b ? String("UP") : String("DOWN");
        if ((nif->IsLoopback(&b), b)) {
            flags += " LOOPBACK";
        }
        if ((nif->IsPointToPoint(&b), b)) {
            flags += " PTP";
        }
        if ((nif->IsVirtual(&b), b)) {
            flags += " VIRTUAL";
        }
        if ((nif->SupportsMulticast(&b), b)) {
            flags += " MULTICAST";
        }
        flags += " MTU=";
        Int32 mtu;
        nif->GetMTU(&mtu);
        flags += mtu;
        AutoPtr<ArrayOf<Byte> > mac;
        nif->GetHardwareAddress((ArrayOf<Byte>**)&mac);
        if (mac != NULL) {
            flags += " HWADDR=";
            for (int i = 0; i < mac->GetLength(); ++i) {
                if (i > 0) {
                    flags += ":";
                }
//                flags += String.format("%02x", (*mac)[i]);
            }
        }
//        System.err.println(flags);
//        System.err.println("-");

        String name;
        nif->GetName(&name);
        AutoPtr<ICharSequence> iName;
        CString::New(name, (ICharSequence**)&iName);
        allNames->Contains(iName, &b);
        assert(b == FALSE);
        Boolean isflag = FALSE;
        allNames->Add(iName, &isflag);

        Int32 index;
        nif->GetIndex(&index);
        AutoPtr<IInteger32> in;
        CInteger32::New(index, (IInteger32**)&in);
        allIndexes->Contains(in, &b);
        assert(b == FALSE);
        allIndexes->Add(in, &b);
    }
    return 0;
}
