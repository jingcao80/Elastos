#ifndef __INETSOCKETADDRESSTEST_H__
#define __INETSOCKETADDRESSTEST_H__

#include "Elastos.CoreLibrary.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elquintet.h>
#include <elastos/StringUtils.h>

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Utility;
using namespace Elastos::Net;

#define TEST(a, x) a.test_##x

class CTest
{
public:
    CTest();
    ~CTest();

public:
    int testRequestHeaders(int argc, char* argv[]);
    int testGetRequestPropertyReturnsLastValue(int argc, char* argv[]);
    int testResponseHeaders(int argc, char* argv[]);
    int testGetErrorStreamOnSuccessfulRequest(int argc, char* argv[]);
    int testGetErrorStreamOnUnsuccessfulRequest(int argc, char* argv[]);
    int test_2939(int argc, char* argv[]);
    int test_10100(int argc, char* argv[]);
    int testConnectionsArePooled(int argc, char* argv[]);

    int testChunkedConnectionsArePooled(int argc, char* argv[]);
    int testConnectionsArePooledWithoutExplicitDisconnect(int argc, char* argv[]);
    int testServerShutdownOutput(int argc, char* argv[]);
    int testRetryableRequestBodyAfterBrokenConnection(int argc, char* argv[]);
    int testNonRetryableRequestBodyAfterBrokenConnection(int argc, char* argv[]);
    int test_chunkedUpload_byteByByte(int argc, char* argv[]);
    int test_chunkedUpload_smallBuffers(int argc, char* argv[]);
    int test_chunkedUpload_largeBuffers(int argc, char* argv[]);

    int test_fixedLengthUpload_byteByByte(int argc, char* argv[]);
    int test_fixedLengthUpload_smallBuffers(int argc, char* argv[]);
    int test_fixedLengthUpload_largeBuffers(int argc, char* argv[]);
    int testGetResponseCodeNoResponseBody(int argc, char* argv[]);
    int testConnectViaHttps(int argc, char* argv[]);
    int testConnectViaHttpsReusingConnections(int argc, char* argv[]);
    int testConnectViaHttpsReusingConnectionsDifferentFactories(int argc, char* argv[]);
    int testConnectViaHttpsWithSSLFallback(int argc, char* argv[]);

    int testConnectViaHttpsToUntrustedServer(int argc, char* argv[]);
    int testConnectViaProxyUsingProxyArg(int argc, char* argv[]);
    int testConnectViaProxyUsingProxySystemProperty(int argc, char* argv[]);
    int testConnectViaProxyUsingHttpProxySystemProperty(int argc, char* argv[]);
    int testContentDisagreesWithContentLengthHeader(int argc, char* argv[]);
    int testContentDisagreesWithChunkedHeader(int argc, char* argv[]);
    int testConnectViaHttpProxyToHttpsUsingProxyArgWithNoProxy(int argc, char* argv[]);
    int testConnectViaHttpProxyToHttpsUsingHttpProxySystemProperty(int argc, char* argv[]);

    int testConnectViaHttpProxyToHttpsUsingProxyArg(int argc, char* argv[]);
    int testConnectViaHttpProxyToHttpsUsingProxySystemProperty(int argc, char* argv[]);
    int testConnectViaHttpProxyToHttpsUsingHttpsProxySystemProperty(int argc, char* argv[]);
    int testConnectViaHttpProxyToHttpsUsingBadProxyAndHttpResponseCache(int argc, char* argv[]);
    int testProxyConnectIncludesProxyHeadersOnly(int argc, char* argv[]);
    int testProxyAuthenticateOnConnect(int argc, char* argv[]);
    int testProxyWithConnectionClose(int argc, char* argv[]);
    int testDisconnectedConnection(int argc, char* argv[]);

    int testDisconnectBeforeConnect(int argc, char* argv[]);
    int testDisconnectAfterOnlyResponseCodeCausesNoCloseGuardWarning(int argc, char* argv[]);
    int testDefaultRequestProperty(int argc, char* argv[]);
    int testMarkAndResetWithContentLengthHeader(int argc, char* argv[]);
    int testMarkAndResetWithChunkedEncoding(int argc, char* argv[]);
    int testMarkAndResetWithNoLengthHeaders(int argc, char* argv[]);
    int testUnauthorizedResponseHandling(int argc, char* argv[]);
    int testNonHexChunkSize(int argc, char* argv[]);

    int testMissingChunkBody(int argc, char* argv[]);
    int testGzipEncodingEnabledByDefault(int argc, char* argv[]);
    int testClientConfiguredGzipContentEncoding(int argc, char* argv[]);
    int testGzipAndConnectionReuseWithFixedLength(int argc, char* argv[]);
    int testGzipAndConnectionReuseWithChunkedEncoding(int argc, char* argv[]);
    int testClientConfiguredCustomContentEncoding(int argc, char* argv[]);
    int testHeadAndContentLength(int argc, char* argv[]);
    int testSetChunkedStreamingMode(int argc, char* argv[]);

    int testAuthenticateWithFixedLengthStreaming(int argc, char* argv[]);
    int testAuthenticateWithChunkedStreaming(int argc, char* argv[]);
    int testSetValidRequestMethod(int argc, char* argv[]);
    int testSetInvalidRequestMethodLowercase(int argc, char* argv[]);
    int testSetInvalidRequestMethodConnect(int argc, char* argv[]);
    int testCannotSetNegativeFixedLengthStreamingMode(int argc, char* argv[]);
    int testCanSetNegativeChunkedStreamingMode(int argc, char* argv[]);
    int testCannotSetFixedLengthStreamingModeAfterConnect(int argc, char* argv[]);

    int testCannotSetChunkedStreamingModeAfterConnect(int argc, char* argv[]);
    int testCannotSetFixedLengthStreamingModeAfterChunkedStreamingMode(int argc, char* argv[]);
    int testCannotSetChunkedStreamingModeAfterFixedLengthStreamingMode(int argc, char* argv[]);
    int testSecureFixedLengthStreaming(int argc, char* argv[]);
    int testSecureChunkedStreaming(int argc, char* argv[]);
    int testAuthenticateWithPost(int argc, char* argv[]);
    int testAuthenticateWithGet(int argc, char* argv[]);
    int testAuthenticateWithCommaSeparatedAuthenticationMethods(int argc, char* argv[]);

    int testAuthenticateWithMultipleAuthenticationHeaders(int argc, char* argv[]);
    int testRedirectedWithChunkedEncoding(int argc, char* argv[]);
    int testRedirectedWithContentLengthHeader(int argc, char* argv[]);
    int testRedirectedWithNoLengthHeaders(int argc, char* argv[]);
    int testRedirectedOnHttps(int argc, char* argv[]);
    int testNotRedirectedFromHttpsToHttp(int argc, char* argv[]);
    int testNotRedirectedFromHttpToHttps(int argc, char* argv[]);
    int testRedirectToAnotherOriginServer(int argc, char* argv[]);

    int testResponse300MultipleChoiceWithPost(int argc, char* argv[]);
    int testResponse301MovedPermanentlyWithPost(int argc, char* argv[]);
    int testResponse302MovedTemporarilyWithPost(int argc, char* argv[]);
    int testResponse303SeeOtherWithPost(int argc, char* argv[]);
    int testResponse305UseProxy(int argc, char* argv[]);
    int testHttpsWithCustomTrustManager(int argc, char* argv[]);
    int testConnectTimeouts(int argc, char* argv[]);
    int testReadTimeouts(int argc, char* argv[]);

    int testSetChunkedEncodingAsRequestProperty(int argc, char* argv[]);
    int testConnectionCloseInRequest(int argc, char* argv[]);
    int testConnectionCloseInResponse(int argc, char* argv[]);
    int testConnectionCloseWithRedirect(int argc, char* argv[]);
    int testResponseCodeDisagreesWithHeaders(int argc, char* argv[]);
    int testSingleByteReadIsSigned(int argc, char* argv[]);
    int testFlushAfterStreamTransmittedWithChunkedEncoding(int argc, char* argv[]);
    int testFlushAfterStreamTransmittedWithFixedLength(int argc, char* argv[]);

    int testFlushAfterStreamTransmittedWithNoLengthHeaders(int argc, char* argv[]);
    int testGetHeadersThrows(int argc, char* argv[]);
    int testGetKeepAlive(int argc, char* argv[]);
    int testLenientUrlToUri(int argc, char* argv[]);
    int testLenientUrlToUriNul(int argc, char* argv[]);
    int testResponseCacheReturnsNullOutputStream(int argc, char* argv[]);
    int testReadAfterLastByte(int argc, char* argv[]);
    int testGetContent(int argc, char* argv[]);

    int testGetContentOfType(int argc, char* argv[]);
    int testGetOutputStreamOnGetFails(int argc, char* argv[]);
    int testGetOutputAfterGetInputStreamFails(int argc, char* argv[]);
    int testSetDoOutputOrDoInputAfterConnectFails(int argc, char* argv[]);
    int testClientSendsContentLength(int argc, char* argv[]);
    int testGetContentLengthConnects(int argc, char* argv[]);
    int testGetContentTypeConnects(int argc, char* argv[]);
    int testGetContentEncodingConnects(int argc, char* argv[]);

    int testUrlContainsQueryButNoPath(int argc, char* argv[]);
    int testInputStreamAvailableWithChunkedEncoding(int argc, char* argv[]);
    int testInputStreamAvailableWithContentLengthHeader(int argc, char* argv[]);
    int testInputStreamAvailableWithNoLengthHeaders(int argc, char* argv[]);
    int testInvalidIpv4Address(int argc, char* argv[]);
    int testUrlWithSpaceInHost(int argc, char* argv[]);
    int testUrlWithSpaceInHostViaHttpProxy(int argc, char* argv[]);
    int testSslFallback(int argc, char* argv[]);

    int testInspectSslBeforeConnect(int argc, char* argv[]);
    int testInspectSslAfterConnect(int argc, char* argv[]);

    // public static class CloseGuardGuard implements Closeable, CloseGuard.Reporter  {
    //     private final CloseGuard.Reporter oldReporter = CloseGuard.getReporter();

    //     private AtomicBoolean closeGuardCalled = new AtomicBoolean();

    //     public CloseGuardGuard() {
    //         CloseGuard.setReporter(this);
    //     }

    //     @Override public void report(String message, Throwable allocationSite) {
    //         oldReporter.report(message, allocationSite);
    //         closeGuardCalled.set(true);
    //     }

    //     public boolean wasCloseGuardCalled() {
    //         FinalizationTester.induceFinalization();
    //         close();
    //         return closeGuardCalled.get();
    //     }

    //     @Override public void close() {
    //         CloseGuard.setReporter(oldReporter);
    //     }

    // };

    // enum StreamingMode {
    //     FIXED_LENGTH, CHUNKED
    // }

    // enum WriteKind { BYTE_BY_BYTE, SMALL_BUFFERS, LARGE_BUFFERS }

    // enum TransferKind {
    //     CHUNKED() {
    //         @Override void setBody(MockResponse response, byte[] content, int chunkSize)
    //                 throws IOException {
    //             response.setChunkedBody(content, chunkSize);
    //         }
    //     },
    //     FIXED_LENGTH() {
    //         @Override void setBody(MockResponse response, byte[] content, int chunkSize) {
    //             response.setBody(content);
    //         }
    //     },
    //     END_OF_STREAM() {
    //         @Override void setBody(MockResponse response, byte[] content, int chunkSize) {
    //             response.setBody(content);
    //             response.setSocketPolicy(DISCONNECT_AT_END);
    //             for (Iterator<String> h = response.getHeaders().iterator(); h.hasNext(); ) {
    //                 if (h.next().startsWith("Content-Length:")) {
    //                     h.remove();
    //                     break;
    //                 }
    //             }
    //         }
    //     };

    //     abstract void setBody(MockResponse response, byte[] content, int chunkSize)
    //             throws IOException;

    //     void setBody(MockResponse response, String content, int chunkSize) throws IOException {
    //         setBody(response, content.getBytes("UTF-8"), chunkSize);
    //     }
    // }

    // enum ProxyConfig {
    //     NO_PROXY() {
    //         @Override public HttpURLConnection connect(MockWebServer server, URL url)
    //                 throws IOException {
    //             return (HttpURLConnection) url.openConnection(Proxy.NO_PROXY);
    //         }
    //     },

    //     CREATE_ARG() {
    //         @Override public HttpURLConnection connect(MockWebServer server, URL url)
    //                 throws IOException {
    //             return (HttpURLConnection) url.openConnection(server.toProxyAddress());
    //         }
    //     },

    //     PROXY_SYSTEM_PROPERTY() {
    //         @Override public HttpURLConnection connect(MockWebServer server, URL url)
    //                 throws IOException {
    //             System.setProperty("proxyHost", "localhost");
    //             System.setProperty("proxyPort", Integer.toString(server.getPort()));
    //             return (HttpURLConnection) url.openConnection();
    //         }
    //     },

    //     HTTP_PROXY_SYSTEM_PROPERTY() {
    //         @Override public HttpURLConnection connect(MockWebServer server, URL url)
    //                 throws IOException {
    //             System.setProperty("http.proxyHost", "localhost");
    //             System.setProperty("http.proxyPort", Integer.toString(server.getPort()));
    //             return (HttpURLConnection) url.openConnection();
    //         }
    //     },

    //     HTTPS_PROXY_SYSTEM_PROPERTY() {
    //         @Override public HttpURLConnection connect(MockWebServer server, URL url)
    //                 throws IOException {
    //             System.setProperty("https.proxyHost", "localhost");
    //             System.setProperty("https.proxyPort", Integer.toString(server.getPort()));
    //             return (HttpURLConnection) url.openConnection();
    //         }
    //     };

    //     public abstract HttpURLConnection connect(MockWebServer server, URL url) throws IOException;
    // }

    // private static class RecordingTrustManager implements X509TrustManager {
    //     private final List<String> calls = new ArrayList<String>();

    //     public X509Certificate[] getAcceptedIssuers() {
    //         calls.add("getAcceptedIssuers");
    //         return new X509Certificate[] {};
    //     }

    //     public void checkClientTrusted(X509Certificate[] chain, String authType)
    //             throws CertificateException {
    //         calls.add("checkClientTrusted " + certificatesToString(chain) + " " + authType);
    //     }

    //     public void checkServerTrusted(X509Certificate[] chain, String authType)
    //             throws CertificateException {
    //         calls.add("checkServerTrusted " + certificatesToString(chain) + " " + authType);
    //     }

    //     private String certificatesToString(X509Certificate[] certificates) {
    //         List<String> result = new ArrayList<String>();
    //         for (X509Certificate certificate : certificates) {
    //             result.add(certificate.getSubjectDN() + " " + certificate.getSerialNumber());
    //         }
    //         return result.toString();
    //     }
    // };

    // private static class RecordingHostnameVerifier implements HostnameVerifier {
    //     private final List<String> calls = new ArrayList<String>();

    //     public boolean verify(String hostname, SSLSession session) {
    //         calls.add("verify " + hostname);
    //         return true;
    //     }
    // };

    // private static class SimpleAuthenticator extends Authenticator {
    //     /** base64("username:password") */
    //     private static final String BASE_64_CREDENTIALS = "dXNlcm5hbWU6cGFzc3dvcmQ=";

    //     private String expectedPrompt;
    //     private RequestorType requestorType;
    //     private int requestingPort;
    //     private InetAddress requestingSite;
    //     private String requestingPrompt;
    //     private String requestingProtocol;
    //     private String requestingScheme;

    //     protected PasswordAuthentication getPasswordAuthentication() {
    //         requestorType = getRequestorType();
    //         requestingPort = getRequestingPort();
    //         requestingSite = getRequestingSite();
    //         requestingPrompt = getRequestingPrompt();
    //         requestingProtocol = getRequestingProtocol();
    //         requestingScheme = getRequestingScheme();
    //         return (expectedPrompt == null || expectedPrompt.equals(requestingPrompt))
    //                 ? new PasswordAuthentication("username", "password".toCharArray())
    //                 : null;
    //     }
    // };

protected:
    setUp();
    tearDown();
    // byte[] gzip(byte[] bytes);
    // void testServerClosesSocket(ISocketPolicy* socketPolicy);
    // void doUpload(TransferKind uploadKind, WriteKind writeKind);
    // void testConnectViaProxy(ProxyConfig proxyConfig);
    // void testConnectViaDirectProxyToHttps(ProxyConfig proxyConfig);
    // void testConnectViaHttpProxyToHttps(ProxyConfig proxyConfig);
    // void initResponseCache();
    // String readAscii(InputStream in, int count);
    // void testMarkAndReset(TransferKind transferKind);
    // void testClientConfiguredGzipContentEncodingAndConnectionReuse(
    //     TransferKind transferKind);
    // private void testAuthenticateWithStreamingPost(StreamingMode streamingMode);
    // void assertValidRequestMethod(String requestMethod);
    // void assertInvalidRequestMethod(String requestMethod);
    // void testSecureStreamingPost(StreamingMode streamingMode);
    // void testRedirected(TransferKind transferKind, boolean reuse);
    // void testResponseRedirectedWithPost(int redirectCode);
    // void testFlushAfterStreamTransmitted(TransferKind transferKind);
    // void testUrlToUriMapping(String string, String asAuthority, String asFile,
    //     String asQuery, String asFragment);
    // URI backdoorUrlToUri(URL url);
    // void testInputStreamAvailable(TransferKind transferKind);
    // void assertContent(String expected, URLConnection connection, int limit);
    // void assertContent(String expected, URLConnection connection);
    // void assertContains(List<String> headers, String header);
    // void assertContainsNoneMatching(List<String> headers, String pattern);
    // Set<String> newSet(String... elements);
};

#endif // __INETSOCKETADDRESSTEST_H__