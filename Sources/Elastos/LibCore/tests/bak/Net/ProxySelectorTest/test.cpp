
#include "test.h"

using Elastos::Net::CURI;
using Elastos::Net::CProxy;
using Elastos::Net::ProxyType;
using Elastos::Net::IProxyHelper;
using Elastos::Net::CProxyHelper;
using Elastos::Net::IInetSocketAddressHelper;
using Elastos::Net::IInetSocketAddress;
using Elastos::Utility::Arrays;
using Elastos::Utility::IList;

//====================================================================
// CTest::
//====================================================================

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

void CTest::setUp()// throws Exception
{
//    super.setUp();
//    ProxySelector::GetDefault((IProxySelector**)&proxySelector);
    CURI::New(String("http://android.com"), (IURI**)&httpUri);
    CURI::New(String("ftp://android.com"), (IURI**)&ftpUri);
    CURI::New(String("https://android.com"), (IURI**)&httpsUri);
    CURI::New(String("socket://android.com"), (IURI**)&socketUri);
    CURI::New(String("other://android.com"), (IURI**)&otherUri);
}

void CTest::tearDown()// throws Exception
{
    // System.clearProperty("ftp.proxyHost");
    // System.clearProperty("ftp.proxyPort");
    // System.clearProperty("ftp.nonProxyHosts");
    // System.clearProperty("http.proxyHost");
    // System.clearProperty("http.proxyPort");
    // System.clearProperty("http.nonProxyHosts");
    // System.clearProperty("https.proxyHost");
    // System.clearProperty("https.proxyPort");
    // System.clearProperty("https.nonProxyHosts");
    // System.clearProperty("other.proxyHost");
    // System.clearProperty("other.proxyPort");
    // System.clearProperty("socket.proxyHost");
    // System.clearProperty("socket.proxyPort");
    // System.clearProperty("proxyHost");
    // System.clearProperty("proxyPort");
}

// test 1
int CTest::testNoProxySystemProperty(int argc, char* argv[])// throws URISyntaxException
{
    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l;
//    Arrays::AsList(noProxy, (IList**)&l);
    AutoPtr<IObjectContainer> ftp;
    proxySelector->Select(ftpUri, (IObjectContainer**)&ftp);
    AutoPtr<IObjectContainer> http;
    proxySelector->Select(httpUri, (IObjectContainer**)&http);
    AutoPtr<IObjectContainer> https;
    proxySelector->Select(httpsUri, (IObjectContainer**)&https);
    AutoPtr<IObjectContainer> sock;
    proxySelector->Select(socketUri, (IObjectContainer**)&sock);
    AutoPtr<IObjectContainer> other;
    proxySelector->Select(otherUri, (IObjectContainer**)&other);

    Boolean b = FALSE;
    AutoPtr<IInterface> p = ftp->Probe(EIID_IInterface);
    l->Equals(p, &b);
    assert(b);

    p = NULL;
    p = http->Probe(EIID_IInterface);
    l->Equals(p, &b);
    assert(b);

    p = NULL;
    p = https->Probe(EIID_IInterface);
    l->Equals(p, &b);
    assert(b);

    p = NULL;
    p = sock->Probe(EIID_IInterface);
    l->Equals(p, &b);
    assert(b);

    p = NULL;
    p = other->Probe(EIID_IInterface);
    l->Equals(p, &b);
    assert(b);

    return 0;
}

// test 2
int CTest::testProxyHostOnly(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("ftp.proxyHost", "a");
    // System.setProperty("http.proxyHost", "b");
    // System.setProperty("https.proxyHost", "c");
    // System.setProperty("other.proxyHost", "d");
    // System.setProperty("socket.proxyHost", "d");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (Elastos::Net::IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean bl = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &bl);
    assert(bl);

    AutoPtr<IInetSocketAddress> addr_2;
    netHelper->CreateUnresolved(String("b"), 80, (IInetSocketAddress**)&addr_2);
    AutoPtr<IProxy> b;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr_2, (Elastos::Net::IProxy**)&b);
    AutoPtr<IList> l2;
//    Arrays::AsList(b, (IList**)&l2);
    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l2->Equals(p, &bl);
    assert(bl);

    AutoPtr<IInetSocketAddress> addr_3;
    netHelper->CreateUnresolved(String("c"), 443, (IInetSocketAddress**)&addr_3);
    AutoPtr<IProxy> c;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr_3, (Elastos::Net::IProxy**)&c);
    AutoPtr<IList> l3;
//    Arrays::AsList(c, (IList**)&l3);
    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l3->Equals(p, &bl);
    assert(bl);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l4;
//    Arrays::AsList(noProxy, (IList**)&l4);
    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l4->Equals(p, &bl);
    assert(bl);

    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l4->Equals(p, &bl);
    assert(bl);

    return 0;
}

// test 3
int CTest::testProxyHostPort(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("ftp.proxyHost", "a");
    // System.setProperty("ftp.proxyPort", "1001");
    // System.setProperty("http.proxyHost", "b");
    // System.setProperty("http.proxyPort", "1002");
    // System.setProperty("https.proxyHost", "c");
    // System.setProperty("https.proxyPort", "1003");
    // System.setProperty("other.proxyHost", "d");
    // System.setProperty("other.proxyPort", "1004");
    // System.setProperty("socket.proxyHost", "e");
    // System.setProperty("socket.proxyPort", "1005");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 1001, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (Elastos::Net::IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean bl = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &bl);
    assert(bl);

    AutoPtr<IInetSocketAddress> addr_2;
    netHelper->CreateUnresolved(String("b"), 1002, (IInetSocketAddress**)&addr_2);
    AutoPtr<IProxy> b;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr_2, (Elastos::Net::IProxy**)&b);
    AutoPtr<IList> l2;
//    Arrays::AsList(b, (IList**)&l2);
    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l2->Equals(p, &bl);
    assert(bl);

    AutoPtr<IInetSocketAddress> addr_3;
    netHelper->CreateUnresolved(String("c"), 1003, (IInetSocketAddress**)&addr_3);
    AutoPtr<IProxy> c;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr_3, (Elastos::Net::IProxy**)&c);
    AutoPtr<IList> l3;
//    Arrays::AsList(c, (IList**)&l3);
    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l3->Equals(p, &bl);
    assert(bl);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l4;
//    Arrays::AsList(noProxy, (IList**)&l4);
    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l4->Equals(p, &bl);
    assert(bl);

    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l4->Equals(p, &bl);
    assert(bl);
    return 0;
}

// test 4
int CTest::testProxyPortOnly(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("ftp.proxyPort", "1001");
    // System.setProperty("http.proxyPort", "1002");
    // System.setProperty("https.proxyPort", "1003");
    // System.setProperty("other.proxyPort", "1004");
    // System.setProperty("socket.proxyPort", "1005");

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l1;
//    Arrays::AsList(noProxy, (IList**)&l1);

    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);
    return 0;
}

// test 5
int CTest::testHttpsDoesNotUseHttpProperties(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("http.proxyHost", "a");
    // System.setProperty("http.proxyPort", "1001");

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l1;
//    Arrays::AsList(noProxy, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);
    return 0;
}

// test 6
int CTest::testProxyHost(int argc, char* argv[])// throws URISyntaxException
{
//    System.setProperty("proxyHost", "a");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (Elastos::Net::IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IInetSocketAddress> addr_2;
    netHelper->CreateUnresolved(String("a"), 443, (IInetSocketAddress**)&addr_2);
    AutoPtr<IProxy> a2;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr_2, (Elastos::Net::IProxy**)&a2);
    AutoPtr<IList> l2;
//    Arrays::AsList(a2, (IList**)&l2);
    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l4;
//    Arrays::AsList(noProxy, (IList**)&l4);
    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l4->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l4->Equals(p, &b);
    assert(b);
    return 0;
}

// test 7
int CTest::testHttpProxyHostPreferredOverProxyHost(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("http.proxyHost", "a");
    // System.setProperty("proxyHost", "b");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);
    return 0;
}

// test 8
int CTest::testSocksProxyHost(int argc, char* argv[])// throws URISyntaxException
{
//    System.setProperty("socksProxyHost", "a");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 1080, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_SOCKS, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l2;
//    Arrays::AsList(noProxy, (IList**)&l2);
    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);
    return 0;
}

// test 9
int CTest::testSocksProxyHostAndPort(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("socksProxyHost", "a");
    // System.setProperty("socksProxyPort", "1001");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 1001, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Proxy::ProxyType_SOCKS, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(ftpUri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(httpUri, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc3;
    proxySelector->Select(httpsUri, (IObjectContainer**)&oc3);
    p = NULL;
    p = oc3->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IObjectContainer> oc4;
    proxySelector->Select(socketUri, (IObjectContainer**)&oc4);
    p = NULL;
    p = oc4->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l2;
//    Arrays::AsList(noProxy, (IList**)&l2);
    AutoPtr<IObjectContainer> oc5;
    proxySelector->Select(otherUri, (IObjectContainer**)&oc5);
    p = NULL;
    p = oc5->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);
    return 0;
}

// test 10
int CTest::testNonProxyHostsFtp(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("ftp.nonProxyHosts", "*.com");
    // System.setProperty("ftp.proxyHost", "a");


    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IURI> net;
    CURI::New(String("ftp://foo.net"), (IURI**)&net);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(net, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l2;
//    Arrays::AsList(noProxy, (IList**)&l2);
    AutoPtr<IURI> com;
    CURI::New(String("ftp://foo.com"), (IURI**)&com);
    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(com, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);
    return 0;
}

// test 11
int CTest::testNonProxyHostsHttp(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("http.nonProxyHosts", "*.com");
    // System.setProperty("http.proxyHost", "a");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IURI> net;
    CURI::New(String("ftp://foo.net"), (IURI**)&net);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(net, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l2;
//    Arrays::AsList(noProxy, (IList**)&l2);
    AutoPtr<IURI> com;
    CURI::New(String("ftp://foo.com"), (IURI**)&com);
    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(com, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);
    return 0;
}

// test 12
int CTest::testNonProxyHostsHttps(int argc, char* argv[])// throws URISyntaxException
{
    // System.setProperty("https.nonProxyHosts", "*.com");
    // System.setProperty("https.proxyHost", "a");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 443, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IURI> net;
    CURI::New(String("ftp://foo.net"), (IURI**)&net);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(net, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);

    AutoPtr<IProxyHelper> helper;
    CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noProxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noProxy);
    AutoPtr<IList> l2;
//    Arrays::AsList(noProxy, (IList**)&l2);
    AutoPtr<IURI> com;
    CURI::New(String("ftp://foo.com"), (IURI**)&com);
    AutoPtr<IObjectContainer> oc2;
    proxySelector->Select(com, (IObjectContainer**)&oc2);
    p = NULL;
    p = oc2->Probe(EIID_IInterface);
    l2->Equals(p, &b);
    assert(b);
    return 0;
}

// test 13
int CTest::testSchemeCaseSensitive(int argc, char* argv[])// throws URISyntaxException
{
//    System.setProperty("http.proxyHost", "a");

    AutoPtr<IInetSocketAddressHelper> netHelper;
//    CInetSocketAddressHelper::AcquireSingleton((IInetSocketAddressHelper**)&netHelper);
    AutoPtr<IInetSocketAddress> addr;
    netHelper->CreateUnresolved(String("a"), 80, (IInetSocketAddress**)&addr);
    AutoPtr<IProxy> a;
    CProxy::New(Elastos::Net::ProxyType_HTTP, addr, (IProxy**)&a);
    AutoPtr<IList> l1;
//    Arrays::AsList(a, (IList**)&l1);
    AutoPtr<IURI> uri;
    CURI::New(String("ftp://foo.net"), (IURI**)&uri);
    AutoPtr<IObjectContainer> oc1;
    proxySelector->Select(uri, (IObjectContainer**)&oc1);
    Boolean b = FALSE;
    AutoPtr<IInterface> p = oc1->Probe(EIID_IInterface);
    l1->Equals(p, &b);
    assert(b);
    return 0;
}