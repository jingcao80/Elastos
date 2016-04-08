#include "test.h"
#include <stdio.h>
#include <stdlib.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::testRequestHeaders,
    &CTest::testGetRequestPropertyReturnsLastValue,
    &CTest::testResponseHeaders,
    &CTest::testGetErrorStreamOnSuccessfulRequest,
    &CTest::testGetErrorStreamOnUnsuccessfulRequest,
    &CTest::test_2939,
    &CTest::test_10100,
    &CTest::testConnectionsArePooled,

    &CTest::testChunkedConnectionsArePooled,
    &CTest::testConnectionsArePooledWithoutExplicitDisconnect,
    &CTest::testServerShutdownOutput,
    &CTest::testRetryableRequestBodyAfterBrokenConnection,
    &CTest::testNonRetryableRequestBodyAfterBrokenConnection,
    &CTest::test_chunkedUpload_byteByByte,
    &CTest::test_chunkedUpload_smallBuffers,
    &CTest::test_chunkedUpload_largeBuffers,

    &CTest::test_fixedLengthUpload_byteByByte,
    &CTest::test_fixedLengthUpload_smallBuffers,
    &CTest::test_fixedLengthUpload_largeBuffers,
    &CTest::testGetResponseCodeNoResponseBody,
    &CTest::testConnectViaHttps,
    &CTest::testConnectViaHttpsReusingConnections,
    &CTest::testConnectViaHttpsReusingConnectionsDifferentFactories,
    &CTest::testConnectViaHttpsWithSSLFallback,

    &CTest::testConnectViaHttpsToUntrustedServer,
    &CTest::testConnectViaProxyUsingProxyArg,
    &CTest::testConnectViaProxyUsingProxySystemProperty,
    &CTest::testConnectViaProxyUsingHttpProxySystemProperty,
    &CTest::testContentDisagreesWithContentLengthHeader,
    &CTest::testContentDisagreesWithChunkedHeader,
    &CTest::testConnectViaHttpProxyToHttpsUsingProxyArgWithNoProxy,
    &CTest::testConnectViaHttpProxyToHttpsUsingHttpProxySystemProperty,

    &CTest::testConnectViaHttpProxyToHttpsUsingProxyArg,
    &CTest::testConnectViaHttpProxyToHttpsUsingProxySystemProperty,
    &CTest::testConnectViaHttpProxyToHttpsUsingHttpsProxySystemProperty,
    &CTest::testConnectViaHttpProxyToHttpsUsingBadProxyAndHttpResponseCache,
    &CTest::testProxyConnectIncludesProxyHeadersOnly,
    &CTest::testProxyAuthenticateOnConnect,
    &CTest::testProxyWithConnectionClose,
    &CTest::testDisconnectedConnection,

    &CTest::testDisconnectBeforeConnect,
    &CTest::testDisconnectAfterOnlyResponseCodeCausesNoCloseGuardWarning,
    &CTest::testDefaultRequestProperty,
    &CTest::testMarkAndResetWithContentLengthHeader,
    &CTest::testMarkAndResetWithChunkedEncoding,
    &CTest::testMarkAndResetWithNoLengthHeaders,
    &CTest::testUnauthorizedResponseHandling,
    &CTest::testNonHexChunkSize,

    &CTest::testMissingChunkBody,
    &CTest::testGzipEncodingEnabledByDefault,
    &CTest::testClientConfiguredGzipContentEncoding,
    &CTest::testGzipAndConnectionReuseWithFixedLength,
    &CTest::testGzipAndConnectionReuseWithChunkedEncoding,
    &CTest::testClientConfiguredCustomContentEncoding,
    &CTest::testHeadAndContentLength,
    &CTest::testSetChunkedStreamingMode,

    &CTest::testAuthenticateWithFixedLengthStreaming,
    &CTest::testAuthenticateWithChunkedStreaming,
    &CTest::testSetValidRequestMethod,
    &CTest::testSetInvalidRequestMethodLowercase,
    &CTest::testSetInvalidRequestMethodConnect,
    &CTest::testCannotSetNegativeFixedLengthStreamingMode,
    &CTest::testCanSetNegativeChunkedStreamingMode,
    &CTest::testCannotSetFixedLengthStreamingModeAfterConnect,

    &CTest::testCannotSetChunkedStreamingModeAfterConnect,
    &CTest::testCannotSetFixedLengthStreamingModeAfterChunkedStreamingMode,
    &CTest::testCannotSetChunkedStreamingModeAfterFixedLengthStreamingMode,
    &CTest::testSecureFixedLengthStreaming,
    &CTest::testSecureChunkedStreaming,
    &CTest::testAuthenticateWithPost,
    &CTest::testAuthenticateWithGet,
    &CTest::testAuthenticateWithCommaSeparatedAuthenticationMethods,

    &CTest::testAuthenticateWithMultipleAuthenticationHeaders,
    &CTest::testRedirectedWithChunkedEncoding,
    &CTest::testRedirectedWithContentLengthHeader,
    &CTest::testRedirectedWithNoLengthHeaders,
    &CTest::testRedirectedOnHttps,
    &CTest::testNotRedirectedFromHttpsToHttp,
    &CTest::testNotRedirectedFromHttpToHttps,
    &CTest::testRedirectToAnotherOriginServer,

    &CTest::testResponse300MultipleChoiceWithPost,
    &CTest::testResponse301MovedPermanentlyWithPost,
    &CTest::testResponse302MovedTemporarilyWithPost,
    &CTest::testResponse303SeeOtherWithPost,
    &CTest::testResponse305UseProxy,
    &CTest::testHttpsWithCustomTrustManager,
    &CTest::testConnectTimeouts,
    &CTest::testReadTimeouts,

    &CTest::testSetChunkedEncodingAsRequestProperty,
    &CTest::testConnectionCloseInRequest,
    &CTest::testConnectionCloseInResponse,
    &CTest::testConnectionCloseWithRedirect,
    &CTest::testResponseCodeDisagreesWithHeaders,
    &CTest::testSingleByteReadIsSigned,
    &CTest::testFlushAfterStreamTransmittedWithChunkedEncoding,
    &CTest::testFlushAfterStreamTransmittedWithFixedLength,

    &CTest::testFlushAfterStreamTransmittedWithNoLengthHeaders,
    &CTest::testGetHeadersThrows,
    &CTest::testGetKeepAlive,
    &CTest::testLenientUrlToUri,
    &CTest::testLenientUrlToUriNul,
    &CTest::testResponseCacheReturnsNullOutputStream,
    &CTest::testReadAfterLastByte,
    &CTest::testGetContent,

    &CTest::testGetContentOfType,
    &CTest::testGetOutputStreamOnGetFails,
    &CTest::testGetOutputAfterGetInputStreamFails,
    &CTest::testSetDoOutputOrDoInputAfterConnectFails,
    &CTest::testClientSendsContentLength,
    &CTest::testGetContentLengthConnects,
    &CTest::testGetContentTypeConnects,
    &CTest::testGetContentEncodingConnects,

    &CTest::testUrlContainsQueryButNoPath,
    &CTest::testInputStreamAvailableWithChunkedEncoding,
    &CTest::testInputStreamAvailableWithContentLengthHeader,
    &CTest::testInputStreamAvailableWithNoLengthHeaders,
    &CTest::testInvalidIpv4Address,
    &CTest::testUrlWithSpaceInHost,
    &CTest::testUrlWithSpaceInHostViaHttpProxy,
    &CTest::testSslFallback,

    &CTest::testInspectSslBeforeConnect,
    &CTest::testInspectSslAfterConnect,
};

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("*ERROR* Require test case number\n");
        return -1;
    }

    int nIndex = atoi(argv[1]) - 1;
    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);

    if (nIndex < 0 || nIndex >= nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

//    TPINIT;

    CTest TestObject;

    int nQuitCode = (TestObject.*TestEntry[nIndex])(argc, argv);

//    TPEND;

    return nQuitCode;
}
