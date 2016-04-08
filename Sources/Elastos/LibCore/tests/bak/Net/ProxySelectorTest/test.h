#ifndef __WEAKHASHMAP_TEST_H
#define __WEAKHASHMAP_TEST_H

#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using Elastos::Net::IProxySelector;
using Elastos::Net::IURI;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testNoProxySystemProperty(int argc, char* argv[]);

    int testProxyHostOnly(int argc, char* argv[]);

    int testProxyHostPort(int argc, char* argv[]);

    int testProxyPortOnly(int argc, char* argv[]);

    int testHttpsDoesNotUseHttpProperties(int argc, char* argv[]);

    int testProxyHost(int argc, char* argv[]);

    int testHttpProxyHostPreferredOverProxyHost(int argc, char* argv[]);

    int testSocksProxyHost(int argc, char* argv[]);

    int testSocksProxyHostAndPort(int argc, char* argv[]);

    int testNonProxyHostsFtp(int argc, char* argv[]);

    int testNonProxyHostsHttp(int argc, char* argv[]);

    int testNonProxyHostsHttps(int argc, char* argv[]);

    int testSchemeCaseSensitive(int argc, char* argv[]);

protected:
    void setUp();
    void tearDown();

private:
    AutoPtr<IProxySelector> proxySelector;
    AutoPtr<IURI> httpUri;
    AutoPtr<IURI> ftpUri;
    AutoPtr<IURI> httpsUri;
    AutoPtr<IURI> socketUri;
    AutoPtr<IURI> otherUri;
};

#endif //__WEAKHASHMAP_TEST_H
