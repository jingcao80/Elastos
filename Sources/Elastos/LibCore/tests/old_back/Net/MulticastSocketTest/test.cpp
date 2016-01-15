#include "test.h"

int CTest::Support_PortManager::getNextPort()
{
    if (!failedOnce) {
        // try {
        AutoPtr<IServerSocket> ss;
        ECode ec = CServerSocket::New(0, (IServerSocket**)&ss);
        if (ec != NOERROR) {
            goto label;
        }
        int port = 0;
        ec = ss->GetLocalPort(&port);
        if (ec != NOERROR) {
            goto label;
        }

        ec = ss->Close();
        if (ec != NOERROR) {
            goto label;
        }
        return port;
        // } catch (Exception ex) {
        // }
    }
label:
    failedOnce = true;
    return getNextPort_unsafe();
}

int CTest::Support_PortManager::getNextPortForUDP()
{
    return getNextPortsForUDP(1)[0];
}

int* CTest::Support_PortManager::getNextPortsForUDP(int num)
{
    if (num <= 0) {
        // throw new IllegalArgumentException("Invalid ports number: " + num);
        return NULL;
    }
    AutoPtr< ArrayOf<IDatagramSocket*> > dss = ArrayOf<IDatagramSocket*>::Alloc(num);
    int* ports = new int[num];

    // try {
    for (int i = 0; i < num; ++i) {
        AutoPtr<IDatagramSocket> res;
        CDatagramSocket::New(0, (IDatagramSocket**)&res);
        dss->Set(i, res);
        (*dss)[i]->GetLocalPort(&ports[i]);
    }
    // } catch (Exception ex) {
    //     throw new Error("Unable to get " + num + " ports for UDP: " + ex);
    // } finally {
    for (int i = 0; i < num; ++i) {
        if ((*dss)[i] != NULL) {
            (*dss)[i]->Close();
        }
    }
    // }
    return ports;
}

int CTest::Support_PortManager::getNextPort_unsafe()
{
    if (++lastAssignedPort > 65534) {
        lastAssignedPort = 6000;
    }
    return lastAssignedPort;
}

int CTest::Support_PortManager::somewhatRandomPort()
{
    AutoPtr<ICalendar> c;
    AutoPtr<ICalendarHelper> calendarhelp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarhelp);
    AutoPtr<ITimeZone> timezone;
    AutoPtr<ITimeZoneHelper> timezonehelp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timezonehelp);
    timezonehelp->GetTimeZone(String("UTC"), (ITimeZone**)&timezone);
    calendarhelp->GetInstanceEx2(timezone, (ICalendar**)&c);
    int minutes = 0;
    c->Get(ICalendar::MINUTE, &minutes);
    int seconds = 0;
    c->Get(ICalendar::SECOND, &seconds);

    return 6000 + (1000 * minutes) + ((seconds / 6) * 100);
}

int CTest::Support_PortManager::lastAssignedPort = somewhatRandomPort();
Boolean CTest::Support_PortManager::failedOnce = false;
Mutex CTest::Support_PortManager::mLock;


CTest::MulticastServer::MulticastServer(IInetAddress* anAddress, int aPort)
{
    rbuf = ArrayOf<Byte>::Alloc(512);
    (*rbuf)[0] = -1;
    CDatagramPacket::New(rbuf, rbuf->GetLength(), (IDatagramPacket**)&rdp);
    CMulticastSocket::New(aPort, (IMulticastSocket**)&ms);
    ms->SetSoTimeout(2000);
    groupAddr = anAddress;
    ms->JoinGroup(groupAddr);
    running = true;
}

CTest::MulticastServer::MulticastServer(ISocketAddress* anAddress, int aPort, INetworkInterface* netInterface)
{
    rbuf = ArrayOf<Byte>::Alloc(512);
    (*rbuf)[0] = -1;
    CDatagramPacket::New(rbuf, rbuf->GetLength(), (IDatagramPacket**)&rdp);
    CMulticastSocket::New(aPort, (IMulticastSocket**)&ms);
    ms->SetSoTimeout(2000);
    groupSockAddr = anAddress;
    groupNI = netInterface;
    ms->JoinGroupEx(groupSockAddr, groupNI);
    running = true;
}

ECode CTest::MulticastServer::Run()
{
    PEL("CTest::MulticastServer::Run")
    // try {
    AutoPtr< ArrayOf<Byte> > tmpbuf = ArrayOf<Byte>::Alloc(512);
    AutoPtr<IDatagramPacket> tmpPack;
    CDatagramPacket::New(tmpbuf, tmpbuf->GetLength(), (IDatagramPacket**)&tmpPack);

    while (running) {
        // try {
        ms->Receive(tmpPack);

        AutoPtr< ArrayOf<Byte> > tmppackdata;
        tmpPack->GetData((ArrayOf<Byte>**)&tmppackdata);
        AutoPtr< ArrayOf<Byte> > rdpdata;
        rdp->GetData((ArrayOf<Byte>**)&rdpdata);
        Int32 rdpset = 0;
        Int32 tmppacklen = 0;
        rdp->GetOffset(&rdpset);
        tmpPack->GetLength(&tmppacklen);
        rdpdata->Copy(rdpset, tmppackdata, 0, tmppacklen);
        tmpPack->GetLength(&tmppacklen);
        rdp->SetLength(tmppacklen);
        AutoPtr<IInetAddress> outadd;
        tmpPack->GetAddress((IInetAddress**)&outadd);
        rdp->SetAddress(outadd);
        Int32 tmpport = 0;
        tmpPack->GetPort(&tmpport);
        rdp->SetPort(tmpport);
        // } catch (java.io.InterruptedIOException e) {
        //     Thread.yield();
        // }
    }
    // } catch (java.io.IOException e) {
    //     System.out.println("Multicast server failed: " + e);
    // } finally {
    ms->Close();
    // }
}

ECode CTest::MulticastServer::stopServer()
{
    running = false;
    // try {
    if (groupAddr != NULL) {
        ms->LeaveGroup(groupAddr);
    }
    else if (groupSockAddr != NULL) {
        ms->LeaveGroupEx(groupSockAddr, groupNI);
    }
    // } catch (IOException e) {}
}

AutoPtr<IInetAddress> CTest::GOOD_IPv4 = lookup(String("224.0.0.3"));
AutoPtr<IInetAddress> CTest::BAD_IPv4 = lookup(String("224.0.0.4"));
AutoPtr<IInetAddress> CTest::GOOD_IPv6 = lookup(String("ff05::7:7"));
AutoPtr<IInetAddress> CTest::BAD_IPv6 = lookup(String("ff05::7:8"));

CTest::CTest()
{
    atLeastTwoInterfaces = false;
    setUp();
}

CTest::~CTest()
{
}

int CTest::test_Constructor(int argc, char* argv[])
{
    PEL("CTest::test_Constructor")
    // regression test for 497
    AutoPtr<IMulticastSocket> s;
    ECode ec = CMulticastSocket::New((IMulticastSocket**)&s);
    // regression test for Harmony-1162
    Boolean isflag = FALSE;
PFL_EX("ec :%p,  s: %p", ec, s.Get())
    s->GetReuseAddress(&isflag);
PFL
    assert(isflag);
    return 0;
}

int CTest::test_ConstructorI(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorI")
    AutoPtr<IMulticastSocket> orig;
    CMulticastSocket::New((IMulticastSocket**)&orig);
    int port = 0;
    orig->GetLocalPort(&port);
    orig->Close();
    AutoPtr<IMulticastSocket> dup;
    // try {
    CMulticastSocket::New(port, (IMulticastSocket**)&dup);
    // regression test for Harmony-1162
    Boolean isflag = FALSE;
    dup->GetReuseAddress(&isflag);
    assert(isflag);
    // } catch (IOException e) {
    //     fail("duplicate binding not allowed: " + e);
    // }
    if (dup != NULL) {
        dup->Close();
    }
    return 0;
}

int CTest::test_getInterface(int argc, char* argv[])
{
    PEL("CTest::test_getInterface")
    int groupPort = Support_PortManager::getNextPortForUDP();

PFL
    // validate that we get the expected response when one was not set
    AutoPtr<IMulticastSocket> mss;
    ECode ec = CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
PFL_EX("ec: %p, groupPort:%d, mss:%p", ec, groupPort, mss.Get())
    // we expect an ANY address in this case
    AutoPtr<IInetAddress> netadd;
    mss->GetInterface((IInetAddress**)&netadd);
PFL_EX("netadd: %p", netadd.Get())
    Boolean isflag = FALSE;
    netadd->IsAnyLocalAddress(&isflag);
    assert(isflag);

PFL
    // validate that we get the expected response when we set via
    // setInterface
    AutoPtr<IEnumeration> addresses;
    networkInterface1->GetInetAddresses((IEnumeration**)&addresses);
PFL
    addresses->HasMoreElements(&isflag);
    if (isflag) {
        AutoPtr<IInetAddress> firstAddress;
        addresses->NextElement((IInterface**)&firstAddress);
        mss->SetInterface(firstAddress);
        AutoPtr<IInterface> outface;
        mss->GetInterface((IInetAddress**)&outface);
        assert(Object::Equals(firstAddress, outface));

PFL
        groupPort = Support_PortManager::getNextPortForUDP();
        CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
        mss->SetNetworkInterface(networkInterface1);
        AutoPtr<INetworkInterfaceHelper> netinthelper;
        CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&netinthelper);
PFL
        mss->GetInterface((IInetAddress**)&firstAddress);
        AutoPtr<INetworkInterface> outworkint;
        netinthelper->GetByInetAddress(firstAddress, (INetworkInterface**)&outworkint);
PFL_EX("firstAddress:%p, networkInterface1: %p, outworkint: %p", firstAddress.Get(), networkInterface1.Get(), outworkint.Get())
        assert(Object::Equals(networkInterface1, outworkint));
    }

    mss->Close();
    return 0;
}

int CTest::test_getNetworkInterface(int argc, char* argv[])
{
    PEL("CTest::test_getNetworkInterface")
    int groupPort = Support_PortManager::getNextPortForUDP();

    // validate that we get the expected response when one was not set
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    AutoPtr<INetworkInterface> theInterface;
    mss->GetNetworkInterface((INetworkInterface**)&theInterface);
    AutoPtr<IEnumeration> addresses;
    theInterface->GetInetAddresses((IEnumeration**)&addresses);
    Boolean isflag = FALSE;
    addresses->HasMoreElements(&isflag);
    assert(isflag);
    AutoPtr<IInetAddress> firstAddress;
    addresses->NextElement((IInterface**)&firstAddress);
    // validate we the first address in the network interface is the ANY address
    firstAddress->IsAnyLocalAddress(&isflag);
    assert(isflag);

    mss->SetNetworkInterface(networkInterface1);
    AutoPtr<INetworkInterface> netint;
    mss->GetNetworkInterface((INetworkInterface**)&netint);
    assert(Object::Equals(networkInterface1, netint));

    if (atLeastTwoInterfaces) {
        mss->SetNetworkInterface(networkInterface2);
        mss->GetNetworkInterface((INetworkInterface**)&netint);
        assert(Object::Equals(networkInterface2, netint));
    }
    mss->Close();

    groupPort = Support_PortManager::getNextPortForUDP();
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    if (IPV6networkInterface1 != NULL) {
        mss->SetNetworkInterface(IPV6networkInterface1);
        mss->GetNetworkInterface((INetworkInterface**)&netint);
        assert(Object::Equals(IPV6networkInterface1, netint));
    }

    // validate that we get the expected response when we set via setInterface
    groupPort = Support_PortManager::getNextPortForUDP();
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    networkInterface1->GetInetAddresses((IEnumeration**)&addresses);
    addresses->HasMoreElements(&isflag);
    if (isflag) {
        addresses->NextElement((IInterface**)&firstAddress);
        mss->SetInterface(firstAddress);
        mss->GetNetworkInterface((INetworkInterface**)&netint);
        assert(Object::Equals(networkInterface1, netint));
    }
    mss->Close();
    return 0;
}

int CTest::test_getTimeToLive(int argc, char* argv[])
{
    PEL("CTest::test_getTimeToLive")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    mss->SetTimeToLive(120);
    Int32 timelive = 0;
    mss->GetTimeToLive(&timelive);
    assert(120 == timelive);
    mss->SetTimeToLive(220);
    mss->GetTimeToLive(&timelive);
    assert(220 == timelive);
    return 0;
}

int CTest::test_getTTL(int argc, char* argv[])
{
    PEL("CTest::test_getTTL")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    Byte ttl = 120;
    mss->SetTTL(ttl);
    mss->GetTTL(&ttl);
    assert(120 == ttl);
    return 0;
}

int CTest::test_joinGroupLjava_net_InetAddress_IPv4(int argc, char* argv[])
{
    PEL("CTest::test_joinGroupLjava_net_InetAddress_IPv4")
    test_joinGroupLjava_net_InetAddress(GOOD_IPv4);
    return 0;
}

int CTest::test_joinGroupLjava_net_InetAddress_IPv6(int argc, char* argv[])
{
    PEL("CTest::test_joinGroupLjava_net_InetAddress_IPv6")
    test_joinGroupLjava_net_InetAddress(GOOD_IPv6);
    return 0;
}

int CTest::test_joinGroup_null_null(int argc, char* argv[])
{
    PEL("CTest::test_joinGroup_null_null")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    ECode ec = mss->JoinGroupEx(NULL, NULL);
    if (ec != NOERROR) {
        printf("fail!~~~~~~~~~\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_joinGroup_non_multicast_address_IPv4(int argc, char* argv[])
{
    PEL("CTest::test_joinGroup_non_multicast_address_IPv4")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> outnet;
    netaddhelp->GetByName(String("127.0.0.1"), (IInetAddress**)&outnet);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(outnet, 0, (IInetSocketAddress**)&netsockadd);
    ECode ec = mss->JoinGroupEx(netsockadd, NULL);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_joinGroup_non_multicast_address_IPv6(int argc, char* argv[])
{
    PEL("CTest::test_joinGroup_non_multicast_address_IPv6")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> outnet;
    netaddhelp->GetByName(String("::1"), (IInetAddress**)&outnet);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(outnet, 0, (IInetSocketAddress**)&netsockadd);
    ECode ec = mss->JoinGroupEx(netsockadd, NULL);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4(int argc, char* argv[])
{
    PEL("CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4")
    test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(GOOD_IPv4, BAD_IPv4);
    return 0;
}

int CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6(int argc, char* argv[])
{
    PEL("CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6")
    test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(GOOD_IPv6, BAD_IPv6);
    return 0;
}

int CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(int argc, char* argv[])
{
    PEL("CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterfaceargc")
    // if there is more than one network interface then check that
    // we can join on specific interfaces and that we only receive
    // if data is received on that interface
    if (!atLeastTwoInterfaces) {
        return -1;
    }
    // set up server on first interfaces
    AutoPtr<IInetAddressHelper> netaddhelp;
PFL
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
PFL
    AutoPtr<IInetAddress> outnet;
    netaddhelp->GetByName(String("127.0.0.1"), (IInetAddress**)&outnet);
    AutoPtr<INetworkInterfaceHelper> networkhelp;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&networkhelp);
PFL
    AutoPtr<INetworkInterface> loopbackInterface;
    networkhelp->GetByInetAddress(outnet, (INetworkInterface**)&loopbackInterface);

PFL
    Boolean anyLoop = Object::Equals(networkInterface1, loopbackInterface)
                    || Object::Equals(networkInterface2, loopbackInterface);
    printf("anyLoop= %d \n", anyLoop);

    AutoPtr<IArrayList> realInterfaces;
    AutoPtr<IEnumeration> theInterfaces;
    networkhelp->GetNetworkInterfaces((IEnumeration**)&theInterfaces);
    Boolean isflag = FALSE;
    while (theInterfaces->HasMoreElements(&isflag), isflag) {
        AutoPtr<INetworkInterface> thisInterface;
        theInterfaces->NextElement((IInterface**)&thisInterface);
        AutoPtr<IEnumeration> thisenum;
        thisInterface->GetInetAddresses((IEnumeration**)&thisenum);
        thisenum->HasMoreElements(&isflag);
        if (isflag){
            realInterfaces->Add(thisInterface, &isflag);
        }
    }

    Int32 sizelen = 0;
    realInterfaces->GetSize(&sizelen);
    for (int i = 0; i < sizelen; i++) {
        AutoPtr<INetworkInterface> thisInterface;
        realInterfaces->Get(i, (IInterface**)&thisInterface);

        // get the first address on the interface

        // start server which is joined to the group and has
        // only asked for packets on this interface
        AutoPtr<IEnumeration> addresses;
        thisInterface->GetInetAddresses((IEnumeration**)&addresses);

        AutoPtr<INetworkInterface> sendingInterface;
        AutoPtr<IInetAddress> group;
        addresses->HasMoreElements(&isflag);
        if (isflag) {
            AutoPtr<IInetAddress> firstAddress;
            addresses->NextElement((IInterface**)&firstAddress);
            if (IInet4Address::Probe(firstAddress)) {
                netaddhelp->GetByName(String("224.0.0.4"), (IInetAddress**)&group);
                if (anyLoop) {
                    if (Object::Equals(networkInterface1, loopbackInterface)) {
                        sendingInterface = networkInterface2;
                    }
                    else {
                        sendingInterface = networkInterface1;
                    }
                }
                else {
                    if (i == 1){
                        sendingInterface = networkInterface2;
                    } else {
                        sendingInterface = networkInterface1;
                    }
                }
            }
            else {
                // if this interface only seems to support IPV6 addresses
                netaddhelp->GetByName(String("FF01:0:0:0:0:0:2:8001"), (IInetAddress**)&group);
                sendingInterface = IPV6networkInterface1;
            }
        }

        int* ports = Support_PortManager::getNextPortsForUDP(2);
        int serverPort = ports[0];
        int groupPort = ports[1];
        AutoPtr<IInetSocketAddress> groupSockAddr;
        CInetSocketAddress::New(group, serverPort, (IInetSocketAddress**)&groupSockAddr);
        AutoPtr<MulticastServer> server = (MulticastServer*) new MulticastServer(groupSockAddr, serverPort, thisInterface);
        server->Start();
        Thread::Sleep(1000);

        // Now send out a package on interface
        // networkInterface 1. We should
        // only see the packet if we send it on interface 1
        AutoPtr<IMulticastSocket> mss;
        CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
        mss->SetNetworkInterface(sendingInterface);
        String thisname;
        thisInterface->GetName(&thisname);
        String msg = String("Hello World - Again") + thisname;
        AutoPtr<ArrayOf<Char16> > char16 = msg.GetChar16s();
        Int32 msglen = 0;
        AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
        for (int i = 0; i < msglen; ++i) {
            (*inbyte)[i] = (*char16)[i] & 0xff;
        }
        AutoPtr<IDatagramPacket> sdp;
        CDatagramPacket::New(inbyte, msglen, group, serverPort, (IDatagramPacket**)&sdp);
        // System.err.println(thisInterface + " " + group);
        mss->Send(sdp);
        Thread::Sleep(1000);
        if (Object::Equals(thisInterface, sendingInterface)) {
            server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
            server->rdp->GetLength(&msglen);
            AutoPtr<ArrayOf<Char32> > char32 = ArrayOf<Char32>::Alloc(msglen);
            for (int i = 0; i < msglen; ++i) {
                (*char32)[i] = (*inbyte)[i];
            }
            String outstr(*char32, 0, msglen);
            assert(msg == outstr);
        }
        else {
            server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
            server->rdp->GetLength(&msglen);
            AutoPtr<ArrayOf<Char32> > char32 = ArrayOf<Char32>::Alloc(msglen);
            for (int i = 0; i < msglen; ++i) {
                (*char32)[i] = (*inbyte)[i];
            }
            String outstr(*char32, 0, msglen);
            assert(msg != outstr);
        }

        server->stopServer();
        mss->Close();
    }
    return 0;
}

int CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv4(int argc, char* argv[])
{
    test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins(GOOD_IPv4);
    return 0;
}

int CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins_IPv6(int argc, char* argv[])
{
    test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins(GOOD_IPv6);
    return 0;
}

int CTest::test_leaveGroupLjava_net_InetAddress_IPv4(int argc, char* argv[])
{
    test_leaveGroupLjava_net_InetAddress(GOOD_IPv4);
    return 0;
}

int CTest::test_leaveGroupLjava_net_InetAddress_IPv6(int argc, char* argv[])
{
    test_leaveGroupLjava_net_InetAddress(GOOD_IPv6);
    return 0;
}

int CTest::test_leaveGroup_null_null(int argc, char* argv[])
{
    PEL("CTest::test_leaveGroup_null_null")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    ECode ec = mss->LeaveGroupEx(NULL, NULL);
    if (ec != NOERROR) {
        /* code */
        printf("fail~~~~~\n");
    }
    // } catch (IllegalArgumentException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_leaveGroup_non_multicast_address_IPv4(int argc, char* argv[])
{
    PEL("CTest::test_leaveGroup_non_multicast_address_IPv4")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> outnet;
    netaddhelp->GetByName(String("127.0.0.1"), (IInetAddress**)&outnet);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(outnet, 0, (IInetSocketAddress**)&netsockadd);

    ECode ec = mss->LeaveGroupEx(netsockadd, NULL);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_leaveGroup_non_multicast_address_IPv6(int argc, char* argv[])
{
    PEL("CTest::test_leaveGroup_non_multicast_address_IPv6")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(0, (IMulticastSocket**)&mss);
    // try {
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> outnet;
    netaddhelp->GetByName(String("::1"), (IInetAddress**)&outnet);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(outnet, 0, (IInetSocketAddress**)&netsockadd);

    ECode ec = mss->LeaveGroupEx(netsockadd, NULL);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv4(int argc, char* argv[])
{
    test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface(GOOD_IPv4, BAD_IPv4);
    return 0;
}

int CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface_IPv6(int argc, char* argv[])
{
    test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface(GOOD_IPv6, BAD_IPv6);
    return 0;
}

int CTest::test_sendLjava_net_DatagramPacketB_IPv4(int argc, char* argv[])
{
    test_sendLjava_net_DatagramPacketB(GOOD_IPv4);
    return 0;
}

int CTest::test_sendLjava_net_DatagramPacketB_IPv6(int argc, char* argv[])
{
    test_sendLjava_net_DatagramPacketB(GOOD_IPv6);
    return 0;
}

int CTest::test_setInterfaceLjava_net_InetAddress(int argc, char* argv[])
{
    PEL("CTest::test_setInterfaceLjava_net_InetAddress")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> localhost;
    netaddhelp->GetLocalHost((IInetAddress**)&localhost);
    mss->SetInterface(localhost);
    AutoPtr<IInetAddress> theInterface;
    mss->GetInterface((IInetAddress**)&theInterface);
    // under IPV6 we are not guarrenteed to get the same address back as
    // the address, all we should be guaranteed is that we get an
    // address on the same interface
    if (IInet6Address::Probe(theInterface)) {
        AutoPtr<IInetAddress> theInterface2;
        mss->GetInterface((IInetAddress**)&theInterface2);
        AutoPtr<INetworkInterfaceHelper> networkhelp;
        CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&networkhelp);
        AutoPtr<INetworkInterface> thework1;
        AutoPtr<INetworkInterface> thework2;
        networkhelp->GetByInetAddress(theInterface, (INetworkInterface**)&thework1);
        networkhelp->GetByInetAddress(theInterface2, (INetworkInterface**)&thework2);
        assert(Object::Equals(thework1, thework2));
    }
    else {
        mss->GetInterface((IInetAddress**)&theInterface);
        assert(Object::Equals(theInterface, localhost));
    }
    mss->Close();
    return 0;
}

int CTest::test_setInterface_unbound_address_IPv4(int argc, char* argv[])
{
    test_setInterface_unbound_address(GOOD_IPv4);
    return 0;
}

int CTest::test_setInterface_unbound_address_IPv6(int argc, char* argv[])
{
    test_setInterface_unbound_address(GOOD_IPv6);
    return 0;
}

int CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_null(int argc, char* argv[])
{
    PEL("CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_null")
    // validate that null interface is handled ok
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    // try {
    ECode ec = mss->SetNetworkInterface(NULL);
    if (ec != NOERROR) {
        printf("No socket exception when we set then network interface with NULL\n");
    }
    // } catch (SocketException ex) {
    // }
    mss->Close();
    return 0;
}

int CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_round_trip(int argc, char* argv[])
{
    PEL("CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_round_trip")
    // validate that we can get and set the interface
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    mss->SetNetworkInterface(networkInterface1);
    AutoPtr<INetworkInterface> thenetwork;
    mss->GetNetworkInterface((INetworkInterface**)&thenetwork);
    assert(Object::Equals(networkInterface1, thenetwork));
    mss->Close();
    return 0;
}

int CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_IPv4(int argc, char* argv[])
{
    test_setNetworkInterfaceLjava_net_NetworkInterface(GOOD_IPv4);
    return 0;
}

int CTest::test_setNetworkInterfaceLjava_net_NetworkInterface_IPv6(int argc, char* argv[])
{
    test_setNetworkInterfaceLjava_net_NetworkInterface(GOOD_IPv6);
    return 0;
}

int CTest::test_setTimeToLiveI(int argc, char* argv[])
{
    PEL("CTest::test_setTimeToLiveI")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    mss->SetTimeToLive(120);
    Int32 timeint = 0;
    mss->GetTimeToLive(&timeint);
    assert(120 == timeint);
    mss->SetTimeToLive(220);
    mss->GetTimeToLive(&timeint);
    assert(220 == timeint);
    mss->Close();
    return 0;
}

int CTest::test_setTTLB(int argc, char* argv[])
{
    PEL("CTest::test_setTTLB")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    Byte ttlbyte = 120;
    mss->SetTTL(ttlbyte);
    mss->GetTTL(&ttlbyte);
    assert(120 == ttlbyte);
    mss->Close();
    return 0;
}

int CTest::test_ConstructorLjava_net_SocketAddress(int argc, char* argv[])
{
    PEL("CTest::test_ConstructorLjava_net_SocketAddress")
    AutoPtr<ISocketAddress> thesocket;
    AutoPtr<IMulticastSocket> ms;
    CMulticastSocket::New(thesocket, (IMulticastSocket**)&ms);
    Boolean isflag = FALSE;
    assert(!(ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> localhost;
    netaddhelp->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(localhost, Support_PortManager::getNextPortForUDP(), (IInetSocketAddress**)&netsockadd);
    ms->Bind(netsockadd);
    assert((ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    ms->Close();
    assert((ms->IsClosed(&isflag), isflag));
    CInetSocketAddress::New(localhost, Support_PortManager::getNextPortForUDP(), (IInetSocketAddress**)&netsockadd);
    CMulticastSocket::New(netsockadd, (IMulticastSocket**)&ms);
    assert((ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    ms->Close();
    assert((ms->IsClosed(&isflag), isflag));
    CInetSocketAddress::New(String("localhost"), Support_PortManager::getNextPortForUDP(), (IInetSocketAddress**)&netsockadd);
    CMulticastSocket::New(netsockadd, (IMulticastSocket**)&ms);
    assert((ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    ms->Close();
    assert((ms->IsClosed(&isflag), isflag));
    // try {
    CInetSocketAddress::New(String("unresolvedname"), Support_PortManager::getNextPortForUDP(), (IInetSocketAddress**)&netsockadd);
    ECode ec = CMulticastSocket::New(netsockadd, (IMulticastSocket**)&ms);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }

    // regression test for Harmony-1162
    AutoPtr<IInetSocketAddress> addr;
    CInetSocketAddress::New(String("0.0.0.0"), 0, (IInetSocketAddress**)&addr);
    AutoPtr<IMulticastSocket> s;
    CMulticastSocket::New(addr, (IMulticastSocket**)&s);
    assert((s->GetReuseAddress(&isflag), isflag));
    return 0;
}

int CTest::test_getLoopbackMode(int argc, char* argv[])
{
    PEL("CTest::test_getLoopbackMode")
    AutoPtr<IMulticastSocket> ms;
    CMulticastSocket::New((ISocketAddress*) NULL, (IMulticastSocket**)&ms);
    Boolean isflag = FALSE;
    assert(!(ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    ms->GetLoopbackMode(&isflag);
    assert(!(ms->IsBound(&isflag), isflag)
            && !(ms->IsClosed(&isflag), isflag)
            && !(ms->IsConnected(&isflag), isflag));
    ms->Close();
    assert((ms->IsClosed(&isflag), isflag));
    return 0;
}

int CTest::test_setLoopbackModeZ(int argc, char* argv[])
{
    PEL("CTest::test_setLoopbackModeZ")
    AutoPtr<IMulticastSocket> ms;
    CMulticastSocket::New((IMulticastSocket**)&ms);
    ms->SetLoopbackMode(true);
    Boolean isflag = FALSE;
    ms->GetLoopbackMode(&isflag);
    assert(isflag);
    ms->SetLoopbackMode(false);
    ms->GetLoopbackMode(&isflag);
    assert(!isflag);
    ms->Close();
    ms->IsClosed(&isflag);
    assert(isflag);
    return 0;
}

int CTest::test_setLoopbackModeSendReceive_IPv4(int argc, char* argv[])
{
    test_setLoopbackModeSendReceive(GOOD_IPv4);
    return 0;
}

int CTest::test_setLoopbackModeSendReceive_IPv6(int argc, char* argv[])
{
    test_setLoopbackModeSendReceive(GOOD_IPv6);
    return 0;
}

int CTest::test_setReuseAddressZ(int argc, char* argv[])
{
    PEL("CTest::test_setReuseAddressZ")
    // test case were we set it to false
    AutoPtr<IMulticastSocket> theSocket1;
    AutoPtr<IMulticastSocket> theSocket2;
    // try {
    AutoPtr<IInetAddressHelper> netaddhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&netaddhelp);
    AutoPtr<IInetAddress> localhost;
    netaddhelp->GetLocalHost((IInetAddress**)&localhost);
    AutoPtr<IInetSocketAddress> theAddress;
    CInetSocketAddress::New(localhost, Support_PortManager::getNextPortForUDP(), (IInetSocketAddress**)&theAddress);
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket1);
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket2);
    theSocket1->SetReuseAddress(false);
    theSocket2->SetReuseAddress(false);
    theSocket1->Bind(theAddress);
    theSocket2->Bind(theAddress);
    // fail("No exception when trying to connect to do duplicate socket bind with re-useaddr set to false");
    // } catch (BindException expected) {
    // }
    if (theSocket1 != NULL) {
        theSocket1->Close();
    }
    if (theSocket2 != NULL) {
        theSocket2->Close();
    }

    // test case were we set it to true
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket1);
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket2);
    theSocket1->SetReuseAddress(true);
    theSocket2->SetReuseAddress(true);
    theSocket1->Bind(theAddress);
    theSocket2->Bind(theAddress);
    if (theSocket1 != NULL) {
        theSocket1->Close();
    }
    if (theSocket2 != NULL) {
        theSocket2->Close();
    }

    // test the default case which we expect to be
    // the same on all platforms
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket1);
    CMulticastSocket::New(NULL, (IMulticastSocket**)&theSocket2);
    theSocket1->Bind(theAddress);
    theSocket2->Bind(theAddress);
    if (theSocket1 != NULL) {
        theSocket1->Close();
    }
    if (theSocket2 != NULL) {
        theSocket2->Close();
    }
    return 0;
}

void CTest::setUp()
{
    PEL("CTest::setUp")
    AutoPtr<IEnumeration> theInterfaces;
    // try {
    AutoPtr<INetworkInterfaceHelper> networkhelp;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&networkhelp);
    networkhelp->GetNetworkInterfaces((IEnumeration**)&theInterfaces);
    // } catch (Exception e) {
    // }

    // only consider interfaces that have addresses associated with them.
    // Otherwise tests don't work so well
    Boolean isflag = FALSE;
    if (theInterfaces != NULL) {
        Boolean atLeastOneInterface = false;
        while ((theInterfaces->HasMoreElements(&isflag), isflag) && (atLeastOneInterface == false)) {
            theInterfaces->NextElement((IInterface**)&networkInterface1);
            AutoPtr<IEnumeration> netenum;
            networkInterface1->GetInetAddresses((IEnumeration**)&netenum);
            if (netenum->HasMoreElements(&isflag), isflag) {
                atLeastOneInterface = true;
            }
        }
        assert(atLeastOneInterface);

        atLeastTwoInterfaces = false;
        if (theInterfaces->HasMoreElements(&isflag), isflag) {
            while ((theInterfaces->HasMoreElements(&isflag), isflag) && (atLeastTwoInterfaces == false)) {
                theInterfaces->NextElement((IInterface**)&networkInterface2);
                AutoPtr<IEnumeration> netenum;
                networkInterface2->GetInetAddresses((IEnumeration**)&netenum);
                if (netenum->HasMoreElements(&isflag), isflag) {
                    atLeastOneInterface = true;
                }
            }
        }

        // first the first interface that supports IPV6 if one exists
        // try {
        networkhelp->GetNetworkInterfaces((IEnumeration**)&theInterfaces);
        // } catch (Exception e) {
        // }
        Boolean found = false;
        while ((theInterfaces->HasMoreElements(&isflag), isflag) && !found) {
            AutoPtr<INetworkInterface> nextInterface;
            theInterfaces->NextElement((IInterface**)&nextInterface);
            AutoPtr<IEnumeration> addresses;
            nextInterface->GetInetAddresses((IEnumeration**)&addresses);
            if (addresses->HasMoreElements(&isflag), isflag) {
                while (addresses->HasMoreElements(&isflag), isflag) {
                    AutoPtr<IInetAddress> nextAddress;
                    addresses->NextElement((IInterface**)&nextAddress);
                    if (IInet6Address::Probe(nextAddress)) {
                        IPV6networkInterface1 = nextInterface;
                        found = true;
                        break;
                    }
                }
            }
        }
    }
}

AutoPtr<IInetAddress> CTest::lookup(String s)
{
    // try {
    AutoPtr<IInetAddress> outres;
    AutoPtr<IInetAddressHelper> addhelp;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addhelp);
    addhelp->GetByName(s, (IInetAddress**)&outres);
    return outres;
    // } catch (IOException ex) {
    //     throw new RuntimeException(ex);
    // }
}

void CTest::test_joinGroupLjava_net_InetAddress(IInetAddress* group)
{
    PEL("CTest::test_joinGroupLjava_net_InetAddress")
    int* ports = Support_PortManager::getNextPortsForUDP(2);
    int groupPort = ports[0];

    AutoPtr<MulticastServer> server = new MulticastServer(group, groupPort);
PFL
    server->Start();
PFL
    Thread::Sleep(1000);
PFL
    String msg("Hello World");
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(ports[1], (IMulticastSocket**)&mss);
PFL
    AutoPtr<IDatagramPacket> sdp;
    AutoPtr< ArrayOf<Char32> > char32 = msg.GetChars();
    Int32 msglen = msg.GetLength();
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
PFL
    CDatagramPacket::New(inbyte, msglen, group, groupPort, (IDatagramPacket**)&sdp);
    mss->SendEx(sdp, (Byte) 10);
    Thread::Sleep(1000);
    server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
    server->rdp->GetLength(&msglen);
    char32 = ArrayOf<Char32>::Alloc(msglen);
PFL
    for (int i = 0; i < msglen; ++i) {
        (*char32)[i] = (*inbyte)[i];
    }
    String receivedMessage(*char32, 0, msglen);
    assert(msg == receivedMessage);
    mss->Close();
    server->stopServer();
}

void CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface(IInetAddress* group, IInetAddress* group2)
{
    PEL("CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface")
    int* ports = Support_PortManager::getNextPortsForUDP(2);
    int groupPort = ports[0];
    int serverPort = ports[1];

    AutoPtr<ISocketAddress> groupSockAddr;
    CInetSocketAddress::New(group, groupPort, (IInetSocketAddress**)&groupSockAddr);

    // Check that we can join a group using a null network interface.
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
PFL
    mss->JoinGroupEx(groupSockAddr, NULL);
PFL
    mss->SetTimeToLive(2);
    Thread::Sleep(1000);

    // set up the server and join the group on networkInterface1
    AutoPtr<MulticastServer> server = new MulticastServer(groupSockAddr, serverPort, networkInterface1);
PFL
    server->Start();
    Thread::Sleep(1000);
PFL
    String msg("Hello World");
    AutoPtr<IDatagramPacket> sdp;
    AutoPtr< ArrayOf<Char32> > char32 = msg.GetChars();
    Int32 msglen = msg.GetLength();
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
    CDatagramPacket::New(inbyte, msglen, group, serverPort, (IDatagramPacket**)&sdp);
    mss->SetTimeToLive(2);
    mss->Send(sdp);
    Thread::Sleep(1000);
    // now validate that we received the data as expected
    server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
    server->rdp->GetLength(&msglen);
    char32 = ArrayOf<Char32>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*char32)[i] = (*inbyte)[i];
    }
    String receivedMessage(*char32, 0, msglen);
    assert(msg == receivedMessage);
    server->stopServer();
    mss->Close();

    // now validate that we handled the case were we join a
    // different multicast address.
    // verify we do not receive the data
    ports = Support_PortManager::getNextPortsForUDP(2);
    serverPort = ports[0];
    server = new MulticastServer(groupSockAddr, serverPort, networkInterface1);
    server->Start();
    Thread::Sleep(1000);

    groupPort = ports[1];
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    mss->SetTimeToLive(10);
    msg = "Hello World - Different Group";
    char32 = msg.GetChars();
    msglen = msg.GetLength();
    inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
    CDatagramPacket::New(inbyte, msglen, group2, serverPort, (IDatagramPacket**)&sdp);
    mss->Send(sdp);
    Thread::Sleep(1000);
    server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
    server->rdp->GetLength(&msglen);
    char32 = ArrayOf<Char32>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*char32)[i] = (*inbyte)[i];
    }
    String receivedMessage2(*char32, 0, msglen);
    assert(msg == receivedMessage2);
    server->stopServer();
    mss->Close();
}

void CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins(IInetAddress* group)
{
    PEL("CTest::test_joinGroupLjava_net_SocketAddressLjava_net_NetworkInterface_multiple_joins")
    // validate that we can join the same address on two
    // different interfaces but not on the same interface
    int groupPort = Support_PortManager::getNextPortForUDP();
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    AutoPtr<ISocketAddress> groupSockAddr;
    CInetSocketAddress::New(group, groupPort, (IInetSocketAddress**)&groupSockAddr);
    mss->JoinGroupEx(groupSockAddr, networkInterface1);
    mss->JoinGroupEx(groupSockAddr, networkInterface2);
    // try {
    ECode ec = mss->JoinGroupEx(groupSockAddr, networkInterface1);
    if (ec != NOERROR) {
        printf("Did not get expected exception when joining for second time on same interface\n");
    }
    // } catch (IOException e) {
    // }
    mss->Close();
}

void CTest::test_leaveGroupLjava_net_InetAddress(IInetAddress* group)
{
    PEL("CTest::test_leaveGroupLjava_net_InetAddress")
    int* ports = Support_PortManager::getNextPortsForUDP(2);
    int groupPort = ports[0];

    String msg("Hello World");
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(ports[1], (IMulticastSocket**)&mss);
    AutoPtr<IDatagramPacket> sdp;
    AutoPtr< ArrayOf<Char32> > char32 = msg.GetChars();
    Int32 msglen = msg.GetLength();
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
    CDatagramPacket::New(inbyte, msglen, group, groupPort, (IDatagramPacket**)&sdp);
    mss->SendEx(sdp, (Byte) 10);
    // try {
        // Try to leave a group we didn't join.
    ECode ec = mss->LeaveGroup(group);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~~\n");
    }
    // } catch (IOException expected) {
    // }
    mss->Close();
}

void CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface(IInetAddress* group, IInetAddress* group2)
{
    PEL("CTest::test_leaveGroupLjava_net_SocketAddressLjava_net_NetworkInterface")
    String msg;
    int groupPort = Support_PortManager::getNextPortForUDP();
    AutoPtr<ISocketAddress> groupSockAddr;
    AutoPtr<ISocketAddress> groupSockAddr2;

    CInetSocketAddress::New(group, groupPort, (IInetSocketAddress**)&groupSockAddr);

    // now test that we can join and leave a group successfully
    groupPort = Support_PortManager::getNextPortForUDP();
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
    CInetSocketAddress::New(group, groupPort, (IInetSocketAddress**)&groupSockAddr);
    mss->JoinGroupEx(groupSockAddr, NULL);
    mss->LeaveGroupEx(groupSockAddr, NULL);
    // try {
    ECode ec = mss->LeaveGroupEx(groupSockAddr, NULL);
    if (ec != NOERROR) {
        printf("Did not get exception when trying to leave group that was already left\n");
    }
    // } catch (IOException expected) {
    // }

    CInetSocketAddress::New(group2, groupPort, (IInetSocketAddress**)&groupSockAddr2);
    mss->JoinGroupEx(groupSockAddr, networkInterface1);
    // try {
    ec = mss->LeaveGroupEx(groupSockAddr2, networkInterface1);
    if (ec != NOERROR) {
        printf("Did not get exception when trying to leave group that was never joined\n");
    }
    // } catch (IOException expected) {
    // }

    mss->LeaveGroupEx(groupSockAddr, networkInterface1);
    if (atLeastTwoInterfaces) {
        mss->JoinGroupEx(groupSockAddr, networkInterface1);
        // try {
        ec = mss->LeaveGroupEx(groupSockAddr, networkInterface2);
        if (ec != NOERROR) {
            printf("Did not get exception when trying to leave group on wrong interface joined on \n");
        }
        // } catch (IOException expected) {
        // }
    }
}

void CTest::test_sendLjava_net_DatagramPacketB(IInetAddress* group)
{
    PEL("CTest::test_sendLjava_net_DatagramPacketB")
    String msg("Hello World");
    int* ports = Support_PortManager::getNextPortsForUDP(2);
    int groupPort = ports[0];

    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New(ports[1], (IMulticastSocket**)&mss);
    AutoPtr<MulticastServer> server = new MulticastServer(group, groupPort);
PFL
    server->Start();
PFL
    Thread::Sleep(200);
    AutoPtr<IDatagramPacket> sdp;
    AutoPtr< ArrayOf<Char32> > char32 = msg.GetChars();
    Int32 msglen = msg.GetLength();
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
    CDatagramPacket::New(inbyte, msglen, group, groupPort, (IDatagramPacket**)&sdp);
    mss->SendEx(sdp, (Byte) 10);
    Thread::Sleep(1000);
    mss->Close();
    server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
    server->rdp->GetLength(&msglen);
    char32 = ArrayOf<Char32>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*char32)[i] = (*inbyte)[i];
    }
    String receivedMessage2(*char32, 0, msglen);
    assert(msg == receivedMessage2);
    server->stopServer();
}

void CTest::test_setInterface_unbound_address(IInetAddress* address)
{
    PEL("CTest::test_setInterface_unbound_address")
    AutoPtr<IMulticastSocket> mss;
    CMulticastSocket::New((IMulticastSocket**)&mss);
    // try {
    ECode ec = mss->SetInterface(address);
    if (ec != NOERROR) {
        printf("fail~~~~~~~~~\n");
    }
    // } catch (SocketException expected) {
    // }
    mss->Close();
}

void CTest::test_setNetworkInterfaceLjava_net_NetworkInterface(IInetAddress* group)
{
    PEL("CTest::test_setNetworkInterfaceLjava_net_NetworkInterface")
    // set up the server and join the group
    AutoPtr<IEnumeration> theInterfaces;
    AutoPtr<INetworkInterfaceHelper> networkhelp;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&networkhelp);
    networkhelp->GetNetworkInterfaces((IEnumeration**)&theInterfaces);
    Boolean isflag = FALSE;
    while (theInterfaces->HasMoreElements(&isflag), isflag) {
        AutoPtr<INetworkInterface> thisInterface;
        theInterfaces->NextElement((IInterface**)&thisInterface);
        AutoPtr<IEnumeration> thisenum;
        thisInterface->GetInetAddresses((IEnumeration**)&thisenum);
        thisenum->HasMoreElements(&isflag);
        if (isflag) {
            AutoPtr<IInetAddress> thisadd;
            thisenum->NextElement((IInterface**)&thisadd);
            thisadd->IsLoopbackAddress(&isflag);
            if (!isflag) {
                int* ports = Support_PortManager::getNextPortsForUDP(2);
                int serverPort = ports[0];
                int groupPort = ports[1];

                AutoPtr<MulticastServer> server = new MulticastServer(group, serverPort);
PFL
                server->Start();
                // give the server some time to start up
                Thread::Sleep(1000);

                // Send the packets on a particular interface. The
                // source address in the received packet
                // should be one of the addresses for the interface
                // set
                AutoPtr<IMulticastSocket> mss;
                CMulticastSocket::New(groupPort, (IMulticastSocket**)&mss);
                mss->SetNetworkInterface(thisInterface);
                String msg;
                thisInterface->GetName(&msg);
                AutoPtr<IDatagramPacket> sdp;
                AutoPtr< ArrayOf<Char32> > char32 = msg.GetChars();
                Int32 msglen = msg.GetLength();
                AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
                for (int i = 0; i < msglen; ++i) {
                    (*inbyte)[i] = (*char32)[i] & 0xff;
                }
                CDatagramPacket::New(inbyte, msglen, group, serverPort, (IDatagramPacket**)&sdp);
                mss->Send(sdp);
                Thread::Sleep(1000);
                server->rdp->GetData((ArrayOf<Byte>**)&inbyte);
                server->rdp->GetLength(&msglen);
                char32 = ArrayOf<Char32>::Alloc(msglen);
                for (int i = 0; i < msglen; ++i) {
                    (*char32)[i] = (*inbyte)[i];
                }
                String receivedMessage(*char32, 0, msglen);
                assert(msg == receivedMessage);
                server->stopServer();
                mss->Close();
            }
        }
    }
}

void CTest::test_setLoopbackModeSendReceive(IInetAddress* group)
{
    PEL("CTest::test_setLoopbackModeSendReceive")
    int PORT = Support_PortManager::getNextPortForUDP();
    String message("Hello, world!");

    // test send receive
    AutoPtr<IMulticastSocket> socket;
    CMulticastSocket::New(PORT, (IMulticastSocket**)&socket);
    socket->SetLoopbackMode(false); // false indicates doing loop back
    socket->JoinGroup(group);

    // send the datagram
    AutoPtr<IDatagramPacket> sendDatagram;
    AutoPtr< ArrayOf<Char32> > char32 = message.GetChars();
    Int32 msglen = message.GetLength();
    AutoPtr< ArrayOf<Byte> > inbyte = ArrayOf<Byte>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*inbyte)[i] = (*char32)[i] & 0xff;
    }
    AutoPtr<IInetSocketAddress> netsockadd;
    CInetSocketAddress::New(group, PORT, (IInetSocketAddress**)&netsockadd);
    CDatagramPacket::New(inbyte,0, msglen, netsockadd, (IDatagramPacket**)&sendDatagram);
    socket->Send(sendDatagram);

    // receive the datagram
    AutoPtr< ArrayOf<Byte> > recvData = ArrayOf<Byte>::Alloc(100);
    AutoPtr<IDatagramPacket> recvDatagram;
    CDatagramPacket::New(recvData, recvData->GetLength(), (IDatagramPacket**)&recvDatagram);
PFL_EX("recvData: %s", recvData->GetPayload())
    socket->SetSoTimeout(5000); // prevent eternal block in
    socket->Receive(recvDatagram);

    recvDatagram->GetLength(&msglen);
    char32 = ArrayOf<Char32>::Alloc(msglen);
    for (int i = 0; i < msglen; ++i) {
        (*char32)[i] = (*recvData)[i];
    }
    String recvMessage(*char32, 0, msglen);
PFL_EX("message: %s, recvMessage: %s, msglen: %d", message.string(), recvMessage.string(), msglen)
    assert(message == recvMessage);
    socket->Close();
}
