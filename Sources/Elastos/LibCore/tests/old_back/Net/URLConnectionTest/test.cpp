
#include "test.h"

CTest::CTest()
{
    setUp();
}

CTest::~CTest()
{
    tearDown();
}

int CTest::testRequestHeaders(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpURLConnection urlConnection = (HttpURLConnection) server.getUrl("/").openConnection();
    // urlConnection.addRequestProperty("D", "e");
    // urlConnection.addRequestProperty("D", "f");
    // assertEquals("f", urlConnection.getRequestProperty("D"));
    // assertEquals("f", urlConnection.getRequestProperty("d"));
    // Map<String, List<String>> requestHeaders = urlConnection.getRequestProperties();
    // assertEquals(newSet("e", "f"), new HashSet<String>(requestHeaders.get("D")));
    // assertEquals(newSet("e", "f"), new HashSet<String>(requestHeaders.get("d")));
    // try {
    //     requestHeaders.put("G", Arrays.asList("h"));
    //     fail("Modified an unmodifiable view.");
    // } catch (UnsupportedOperationException expected) {
    // }
    // try {
    //     requestHeaders.get("D").add("i");
    //     fail("Modified an unmodifiable view.");
    // } catch (UnsupportedOperationException expected) {
    // }
    // try {
    //     urlConnection.setRequestProperty(null, "j");
    //     fail();
    // } catch (NullPointerException expected) {
    // }
    // try {
    //     urlConnection.addRequestProperty(null, "k");
    //     fail();
    // } catch (NullPointerException expected) {
    // }
    // urlConnection.setRequestProperty("NullValue", null); // should fail silently!
    // assertNull(urlConnection.getRequestProperty("NullValue"));
    // urlConnection.addRequestProperty("AnotherNullValue", null);  // should fail silently!
    // assertNull(urlConnection.getRequestProperty("AnotherNullValue"));

    // urlConnection.getResponseCode();
    // RecordedRequest request = server.takeRequest();
    // assertContains(request.getHeaders(), "D: e");
    // assertContains(request.getHeaders(), "D: f");
    // assertContainsNoneMatching(request.getHeaders(), "NullValue.*");
    // assertContainsNoneMatching(request.getHeaders(), "AnotherNullValue.*");
    // assertContainsNoneMatching(request.getHeaders(), "G:.*");
    // assertContainsNoneMatching(request.getHeaders(), "null:.*");

    // try {
    //     urlConnection.addRequestProperty("N", "o");
    //     fail("Set header after connect");
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     urlConnection.setRequestProperty("P", "q");
    //     fail("Set header after connect");
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     urlConnection.getRequestProperties();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testGetRequestPropertyReturnsLastValue(int argc, char* argv[])
{
    // server.play();
    // HttpURLConnection urlConnection = (HttpURLConnection) server.getUrl("/").openConnection();
    // urlConnection.addRequestProperty("A", "value1");
    // urlConnection.addRequestProperty("A", "value2");
    // assertEquals("value2", urlConnection.getRequestProperty("A"));
    return 0;
}

int CTest::testResponseHeaders(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setStatus("HTTP/1.0 200 Fantastic")
    //         .addHeader("A: c")
    //         .addHeader("B: d")
    //         .addHeader("A: e")
    //         .setChunkedBody("ABCDE\nFGHIJ\nKLMNO\nPQR", 8));
    // server.play();

    // HttpURLConnection urlConnection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals(200, urlConnection.getResponseCode());
    // assertEquals("Fantastic", urlConnection.getResponseMessage());
    // assertEquals("HTTP/1.0 200 Fantastic", urlConnection.getHeaderField(null));
    // Map<String, List<String>> responseHeaders = urlConnection.getHeaderFields();
    // assertEquals(Arrays.asList("HTTP/1.0 200 Fantastic"), responseHeaders.get(null));
    // assertEquals(newSet("c", "e"), new HashSet<String>(responseHeaders.get("A")));
    // assertEquals(newSet("c", "e"), new HashSet<String>(responseHeaders.get("a")));
    // try {
    //     responseHeaders.put("N", Arrays.asList("o"));
    //     fail("Modified an unmodifiable view.");
    // } catch (UnsupportedOperationException expected) {
    // }
    // try {
    //     responseHeaders.get("A").add("f");
    //     fail("Modified an unmodifiable view.");
    // } catch (UnsupportedOperationException expected) {
    // }
    // assertEquals("A", urlConnection.getHeaderFieldKey(0));
    // assertEquals("c", urlConnection.getHeaderField(0));
    // assertEquals("B", urlConnection.getHeaderFieldKey(1));
    // assertEquals("d", urlConnection.getHeaderField(1));
    // assertEquals("A", urlConnection.getHeaderFieldKey(2));
    // assertEquals("e", urlConnection.getHeaderField(2));
    return 0;
}

int CTest::testGetErrorStreamOnSuccessfulRequest(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertNull(connection.getErrorStream());
    return 0;
}

int CTest::testGetErrorStreamOnUnsuccessfulRequest(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setResponseCode(404).setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("A", readAscii(connection.getErrorStream(), Integer.MAX_VALUE));
    return 0;
}

int CTest::test_2939(int argc, char* argv[])
{
    // MockResponse response = new MockResponse().setChunkedBody("ABCDE\nFGHIJ\nKLMNO\nPQR", 8);

    // server.enqueue(response);
    // server.enqueue(response);
    // server.play();

    // assertContent("ABCDE", server.getUrl("/").openConnection(), 5);
    // assertContent("ABCDE", server.getUrl("/").openConnection(), 5);
    return 0;
}

int CTest::test_10100(int argc, char* argv[])
{
    // assertEquals("image/jpeg", URLConnection.guessContentTypeFromName("someFile.jpg"));
    // assertEquals("application/pdf", URLConnection.guessContentTypeFromName("stuff.pdf"));
    return 0;
}

int CTest::testConnectionsArePooled(int argc, char* argv[])
{
    // MockResponse response = new MockResponse().setBody("ABCDEFGHIJKLMNOPQR");

    // server.enqueue(response);
    // server.enqueue(response);
    // server.enqueue(response);
    // server.play();

    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/foo").openConnection());
    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/bar?baz=quux").openConnection());
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/z").openConnection());
    // assertEquals(2, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testChunkedConnectionsArePooled(int argc, char* argv[])
{
    // MockResponse response = new MockResponse().setChunkedBody("ABCDEFGHIJKLMNOPQR", 5);

    // server.enqueue(response);
    // server.enqueue(response);
    // server.enqueue(response);
    // server.play();

    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/foo").openConnection());
    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/bar?baz=quux").openConnection());
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    // assertContent("ABCDEFGHIJKLMNOPQR", server.getUrl("/z").openConnection());
    // assertEquals(2, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testConnectionsArePooledWithoutExplicitDisconnect(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("ABC"));
    // server.enqueue(new MockResponse().setBody("DEF"));
    // server.play();

    // URLConnection connection1 = server.getUrl("/").openConnection();
    // assertEquals("ABC", readAscii(connection1.getInputStream(), Integer.MAX_VALUE));
    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // URLConnection connection2 = server.getUrl("/").openConnection();
    // assertEquals("DEF", readAscii(connection2.getInputStream(), Integer.MAX_VALUE));
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testServerShutdownOutput(int argc, char* argv[])
{
    // testServerClosesSocket(DISCONNECT_AT_END);
    // testServerClosesSocket(SHUTDOWN_INPUT_AT_END);

    // // This test causes MockWebServer to log a "connection failed" stack trace
    // server.enqueue(new MockResponse()
    //         .setBody("Output shutdown after this response")
    //         .setSocketPolicy(SHUTDOWN_OUTPUT_AT_END));
    // server.enqueue(new MockResponse().setBody("This response will fail to write"));
    // server.enqueue(new MockResponse().setBody("This comes after a busted connection"));
    // server.play();

    // assertContent("Output shutdown after this response", server.getUrl("/a").openConnection());
    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertContent("This comes after a busted connection", server.getUrl("/b").openConnection());
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    // assertEquals(0, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testRetryableRequestBodyAfterBrokenConnection(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("abc").setSocketPolicy(DISCONNECT_AT_END));
    // server.enqueue(new MockResponse().setBody("def"));
    // server.play();

    // assertContent("abc", server.getUrl("/a").openConnection());
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/b").openConnection();
    // connection.setDoOutput(true);
    // OutputStream out = connection.getOutputStream();
    // out.write(new byte[] {1, 2, 3});
    // out.close();
    // assertContent("def", connection);
    return 0;
}

int CTest::testNonRetryableRequestBodyAfterBrokenConnection(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("abc").setSocketPolicy(DISCONNECT_AT_END));
    // server.enqueue(new MockResponse().setBody("def"));
    // server.play();

    // assertContent("abc", server.getUrl("/a").openConnection());
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/b").openConnection();
    // connection.setDoOutput(true);
    // connection.setFixedLengthStreamingMode(3);
    // OutputStream out = connection.getOutputStream();
    // out.write(new byte[] {1, 2, 3});
    // out.close();
    // try {
    //     connection.getInputStream();
    //     fail();
    // } catch (IOException expected) {
    // }
    return 0;
}

int CTest::test_chunkedUpload_byteByByte(int argc, char* argv[])
{
    // doUpload(TransferKind.CHUNKED, WriteKind.BYTE_BY_BYTE);
    return 0;
}

int CTest::test_chunkedUpload_smallBuffers(int argc, char* argv[])
{
    // doUpload(TransferKind.CHUNKED, WriteKind.SMALL_BUFFERS);
    return 0;
}

int CTest::test_chunkedUpload_largeBuffers(int argc, char* argv[])
{
    // doUpload(TransferKind.CHUNKED, WriteKind.LARGE_BUFFERS);
    return 0;
}

int CTest::test_fixedLengthUpload_byteByByte(int argc, char* argv[])
{
    // doUpload(TransferKind.FIXED_LENGTH, WriteKind.BYTE_BY_BYTE);
    return 0;
}

int CTest::test_fixedLengthUpload_smallBuffers(int argc, char* argv[])
{
    // doUpload(TransferKind.FIXED_LENGTH, WriteKind.SMALL_BUFFERS);
    return 0;
}

int CTest::test_fixedLengthUpload_largeBuffers(int argc, char* argv[])
{
    // doUpload(TransferKind.FIXED_LENGTH, WriteKind.LARGE_BUFFERS);
    return 0;
}

int CTest::testGetResponseCodeNoResponseBody(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .addHeader("abc: def"));
    // server.play();

    // URL url = server.getUrl("/");
    // HttpURLConnection conn = (HttpURLConnection) url.openConnection();
    // conn.setDoInput(false);
    // assertEquals("def", conn.getHeaderField("abc"));
    // assertEquals(200, conn.getResponseCode());
    // try {
    //     conn.getInputStream();
    //     fail();
    // } catch (ProtocolException expected) {
    // }
    return 0;
}

int CTest::testConnectViaHttps(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse().setBody("this response comes via HTTPS"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/foo").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());

    // assertContent("this response comes via HTTPS", connection);

    // RecordedRequest request = server.takeRequest();
    // assertEquals("GET /foo HTTP/1.1", request.getRequestLine());
    // assertEquals("TLSv1", request.getSslProtocol());
    return 0;
}

int CTest::testConnectViaHttpsReusingConnections(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // SSLSocketFactory clientSocketFactory = testSSLContext.clientContext.getSocketFactory();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse().setBody("this response comes via HTTPS"));
    // server.enqueue(new MockResponse().setBody("another response via HTTPS"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(clientSocketFactory);
    // assertContent("this response comes via HTTPS", connection);

    // connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(clientSocketFactory);
    // assertContent("another response via HTTPS", connection);

    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testConnectViaHttpsReusingConnectionsDifferentFactories(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse().setBody("this response comes via HTTPS"));
    // server.enqueue(new MockResponse().setBody("another response via HTTPS"));
    // server.play();

    // // install a custom SSL socket factory so the server can be authorized
    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // assertContent("this response comes via HTTPS", connection);

    // connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // try {
    //     readAscii(connection.getInputStream(), Integer.MAX_VALUE);
    //     fail("without an SSL socket factory, the connection should fail");
    // } catch (SSLException expected) {
    // }
    return 0;
}

int CTest::testConnectViaHttpsWithSSLFallback(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse().setSocketPolicy(DISCONNECT_AT_START));
    // server.enqueue(new MockResponse().setBody("this response comes via SSL"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/foo").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());

    // assertContent("this response comes via SSL", connection);

    // RecordedRequest request = server.takeRequest();
    // assertEquals("GET /foo HTTP/1.1", request.getRequestLine());
    return 0;
}

int CTest::testConnectViaHttpsToUntrustedServer(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create(TestKeyStore.getClientCA2(),
    //                                                       TestKeyStore.getServer());

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse()); // unused
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/foo").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // try {
    //     connection.getInputStream();
    //     fail();
    // } catch (SSLHandshakeException expected) {
    //     assertTrue(expected.getCause() instanceof CertificateException);
    // }
    // assertEquals(0, server.getRequestCount());
    return 0;
}

int CTest::testConnectViaProxyUsingProxyArg(int argc, char* argv[])
{
    // testConnectViaProxy(ProxyConfig.CREATE_ARG);
    return 0;
}

int CTest::testConnectViaProxyUsingProxySystemProperty(int argc, char* argv[])
{
    // testConnectViaProxy(ProxyConfig.PROXY_SYSTEM_PROPERTY);
    return 0;
}

int CTest::testConnectViaProxyUsingHttpProxySystemProperty(int argc, char* argv[])
{
    // testConnectViaProxy(ProxyConfig.HTTP_PROXY_SYSTEM_PROPERTY);
    return 0;
}

int CTest::testContentDisagreesWithContentLengthHeader(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody("abc\r\nYOU SHOULD NOT SEE THIS")
    //         .clearHeaders()
    //         .addHeader("Content-Length: 3"));
    // server.play();

    // assertContent("abc", server.getUrl("/").openConnection());
    return 0;
}

int CTest::testContentDisagreesWithChunkedHeader(int argc, char* argv[])
{
    // MockResponse mockResponse = new MockResponse();
    // mockResponse.setChunkedBody("abc", 3);
    // ByteArrayOutputStream bytesOut = new ByteArrayOutputStream();
    // bytesOut.write(mockResponse.getBody());
    // bytesOut.write("\r\nYOU SHOULD NOT SEE THIS".getBytes());
    // mockResponse.setBody(bytesOut.toByteArray());
    // mockResponse.clearHeaders();
    // mockResponse.addHeader("Transfer-encoding: chunked");

    // server.enqueue(mockResponse);
    // server.play();

    // assertContent("abc", server.getUrl("/").openConnection());
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingProxyArgWithNoProxy(int argc, char* argv[])
{
    // testConnectViaDirectProxyToHttps(ProxyConfig.NO_PROXY);
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingHttpProxySystemProperty(int argc, char* argv[])
{
    // // https should not use http proxy
    // testConnectViaDirectProxyToHttps(ProxyConfig.HTTP_PROXY_SYSTEM_PROPERTY);
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingProxyArg(int argc, char* argv[])
{
    // testConnectViaHttpProxyToHttps(ProxyConfig.CREATE_ARG);
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingProxySystemProperty(int argc, char* argv[])
{
    // testConnectViaHttpProxyToHttps(ProxyConfig.PROXY_SYSTEM_PROPERTY);
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingHttpsProxySystemProperty(int argc, char* argv[])
{
    // testConnectViaHttpProxyToHttps(ProxyConfig.HTTPS_PROXY_SYSTEM_PROPERTY);
    return 0;
}

int CTest::testConnectViaHttpProxyToHttpsUsingBadProxyAndHttpResponseCache(int argc, char* argv[])
{
    // ProxyConfig proxyConfig = ProxyConfig.PROXY_SYSTEM_PROPERTY;

    // TestSSLContext testSSLContext = TestSSLContext.create();

    // initResponseCache();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), true);
    // server.enqueue(new MockResponse()
    //         .setSocketPolicy(SocketPolicy.UPGRADE_TO_SSL_AT_END)
    //         .clearHeaders()
    //         .setBody("bogus proxy connect response content")); // Key to reproducing b/6754912
    // server.play();

    // URL url = new URL("https://android.com/foo");
    // HttpsURLConnection connection = (HttpsURLConnection) proxyConfig.connect(server, url);
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());

    // try {
    //     connection.connect();
    //     fail();
    // } catch (IOException expected) {
    //     // Thrown when the connect causes SSLSocket.startHandshake() to throw
    //     // when it sees the "bogus proxy connect response content"
    //     // instead of a ServerHello handshake message.
    // }

    // RecordedRequest connect = server.takeRequest();
    // assertEquals("Connect line failure on proxy",
    //         "CONNECT android.com:443 HTTP/1.1", connect.getRequestLine());
    // assertContains(connect.getHeaders(), "Host: android.com");
    return 0;
}

int CTest::testProxyConnectIncludesProxyHeadersOnly(int argc, char* argv[])
{
    // RecordingHostnameVerifier hostnameVerifier = new RecordingHostnameVerifier();
    // TestSSLContext testSSLContext = TestSSLContext.create();

    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), true);
    // server.enqueue(new MockResponse()
    //         .setSocketPolicy(SocketPolicy.UPGRADE_TO_SSL_AT_END)
    //         .clearHeaders());
    // server.enqueue(new MockResponse().setBody("encrypted response from the origin server"));
    // server.play();

    // URL url = new URL("https://android.com/foo");
    // HttpsURLConnection connection = (HttpsURLConnection) url.openConnection(
    //         server.toProxyAddress());
    // connection.addRequestProperty("Private", "Secret");
    // connection.addRequestProperty("Proxy-Authorization", "bar");
    // connection.addRequestProperty("User-Agent", "baz");
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // connection.setHostnameVerifier(hostnameVerifier);
    // assertContent("encrypted response from the origin server", connection);

    // RecordedRequest connect = server.takeRequest();
    // assertContainsNoneMatching(connect.getHeaders(), "Private.*");
    // assertContains(connect.getHeaders(), "Proxy-Authorization: bar");
    // assertContains(connect.getHeaders(), "User-Agent: baz");
    // assertContains(connect.getHeaders(), "Host: android.com");
    // assertContains(connect.getHeaders(), "Proxy-Connection: Keep-Alive");

    // RecordedRequest get = server.takeRequest();
    // assertContains(get.getHeaders(), "Private: Secret");
    // assertEquals(Arrays.asList("verify android.com"), hostnameVerifier.calls);
    return 0;
}

int CTest::testProxyAuthenticateOnConnect(int argc, char* argv[])
{
    // Authenticator.setDefault(new SimpleAuthenticator());
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), true);
    // server.enqueue(new MockResponse()
    //         .setResponseCode(407)
    //         .addHeader("Proxy-Authenticate: Basic realm=\"localhost\""));
    // server.enqueue(new MockResponse()
    //         .setSocketPolicy(SocketPolicy.UPGRADE_TO_SSL_AT_END)
    //         .clearHeaders());
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();

    // URL url = new URL("https://android.com/foo");
    // HttpsURLConnection connection = (HttpsURLConnection) url.openConnection(
    //         server.toProxyAddress());
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // connection.setHostnameVerifier(new RecordingHostnameVerifier());
    // assertContent("A", connection);

    // RecordedRequest connect1 = server.takeRequest();
    // assertEquals("CONNECT android.com:443 HTTP/1.1", connect1.getRequestLine());
    // assertContainsNoneMatching(connect1.getHeaders(), "Proxy\\-Authorization.*");

    // RecordedRequest connect2 = server.takeRequest();
    // assertEquals("CONNECT android.com:443 HTTP/1.1", connect2.getRequestLine());
    // assertContains(connect2.getHeaders(), "Proxy-Authorization: Basic "
    //         + SimpleAuthenticator.BASE_64_CREDENTIALS);

    // RecordedRequest get = server.takeRequest();
    // assertEquals("GET /foo HTTP/1.1", get.getRequestLine());
    // assertContainsNoneMatching(get.getHeaders(), "Proxy\\-Authorization.*");
    return 0;
}

int CTest::testProxyWithConnectionClose(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), true);
    // server.enqueue(new MockResponse()
    //         .setSocketPolicy(SocketPolicy.UPGRADE_TO_SSL_AT_END)
    //         .clearHeaders());
    // server.enqueue(new MockResponse().setBody("this response comes via a proxy"));
    // server.play();

    // URL url = new URL("https://android.com/foo");
    // HttpsURLConnection connection = (HttpsURLConnection) url.openConnection(
    //         server.toProxyAddress());
    // connection.setRequestProperty("Connection", "close");
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // connection.setHostnameVerifier(new RecordingHostnameVerifier());

    // assertContent("this response comes via a proxy", connection);
    return 0;
}

int CTest::testDisconnectedConnection(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("ABCDEFGHIJKLMNOPQR"));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // InputStream in = connection.getInputStream();
    // assertEquals('A', (char) in.read());
    // connection.disconnect();
    // try {
    //     in.read();
    //     fail("Expected a connection closed exception");
    // } catch (IOException expected) {
    // }
    return 0;
}

int CTest::testDisconnectBeforeConnect(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.disconnect();

    // assertContent("A", connection);
    // assertEquals(200, connection.getResponseCode());
    return 0;
}

int CTest::testDisconnectAfterOnlyResponseCodeCausesNoCloseGuardWarning(int argc, char* argv[])
{
    // CloseGuardGuard guard = new CloseGuardGuard();
    // try {
    //     server.enqueue(new MockResponse()
    //                    .setBody(gzip("ABCABCABC".getBytes("UTF-8")))
    //                    .addHeader("Content-Encoding: gzip"));
    //     server.play();

    //     HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    //     assertEquals(200, connection.getResponseCode());
    //     connection.disconnect();
    //     connection = null;
    //     assertFalse(guard.wasCloseGuardCalled());
    // } finally {
    //     guard.close();
    // }
    return 0;
}

int CTest::testDefaultRequestProperty(int argc, char* argv[])
{
    // URLConnection.setDefaultRequestProperty("X-testSetDefaultRequestProperty", "A");
    // assertNull(URLConnection.getDefaultRequestProperty("X-setDefaultRequestProperty"));
    return 0;
}

int CTest::testMarkAndResetWithContentLengthHeader(int argc, char* argv[])
{
    // testMarkAndReset(TransferKind.FIXED_LENGTH);
    return 0;
}

int CTest::testMarkAndResetWithChunkedEncoding(int argc, char* argv[])
{
    // testMarkAndReset(TransferKind.CHUNKED);
    return 0;
}

int CTest::testMarkAndResetWithNoLengthHeaders(int argc, char* argv[])
{
    // testMarkAndReset(TransferKind.END_OF_STREAM);
    return 0;
}

int CTest::testUnauthorizedResponseHandling(int argc, char* argv[])
{
    // MockResponse response = new MockResponse()
    //         .addHeader("WWW-Authenticate: Basic realm=\"protected area\"")
    //         .setResponseCode(401) // UNAUTHORIZED
    //         .setBody("Unauthorized");
    // server.enqueue(response);
    // server.enqueue(response);
    // server.enqueue(response);
    // server.play();

    // URL url = server.getUrl("/");
    // HttpURLConnection conn = (HttpURLConnection) url.openConnection();

    // assertEquals(401, conn.getResponseCode());
    // assertEquals(401, conn.getResponseCode());
    // assertEquals(401, conn.getResponseCode());
    // assertEquals(1, server.getRequestCount());
    return 0;
}

int CTest::testNonHexChunkSize(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody("5\r\nABCDE\r\nG\r\nFGHIJKLMNOPQRSTU\r\n0\r\n\r\n")
    //         .clearHeaders()
    //         .addHeader("Transfer-encoding: chunked"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // try {
    //     readAscii(connection.getInputStream(), Integer.MAX_VALUE);
    //     fail();
    // } catch (IOException e) {
    // }
    return 0;
}

int CTest::testMissingChunkBody(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody("5")
    //         .clearHeaders()
    //         .addHeader("Transfer-encoding: chunked")
    //         .setSocketPolicy(DISCONNECT_AT_END));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // try {
    //     readAscii(connection.getInputStream(), Integer.MAX_VALUE);
    //     fail();
    // } catch (IOException e) {
    // }
    return 0;
}

int CTest::testGzipEncodingEnabledByDefault(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody(gzip("ABCABCABC".getBytes("UTF-8")))
    //         .addHeader("Content-Encoding: gzip"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // assertEquals("ABCABCABC", readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // assertNull(connection.getContentEncoding());
    // assertEquals(-1, connection.getContentLength());

    // RecordedRequest request = server.takeRequest();
    // assertContains(request.getHeaders(), "Accept-Encoding: gzip");
    return 0;
}

int CTest::testClientConfiguredGzipContentEncoding(int argc, char* argv[])
{
    // byte[] bodyBytes = gzip("ABCDEFGHIJKLMNOPQRSTUVWXYZ".getBytes("UTF-8"));
    // server.enqueue(new MockResponse()
    //         .setBody(bodyBytes)
    //         .addHeader("Content-Encoding: gzip")
    //         .addHeader("Content-Length: " + bodyBytes.length));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // connection.addRequestProperty("Accept-Encoding", "gzip");
    // InputStream gunzippedIn = new GZIPInputStream(connection.getInputStream());
    // assertEquals("ABCDEFGHIJKLMNOPQRSTUVWXYZ", readAscii(gunzippedIn, Integer.MAX_VALUE));
    // assertEquals(bodyBytes.length, connection.getContentLength());

    // RecordedRequest request = server.takeRequest();
    // assertContains(request.getHeaders(), "Accept-Encoding: gzip");
    return 0;
}

int CTest::testGzipAndConnectionReuseWithFixedLength(int argc, char* argv[])
{
    // testClientConfiguredGzipContentEncodingAndConnectionReuse(TransferKind.FIXED_LENGTH);
    return 0;
}

int CTest::testGzipAndConnectionReuseWithChunkedEncoding(int argc, char* argv[])
{
    // testClientConfiguredGzipContentEncodingAndConnectionReuse(TransferKind.CHUNKED);
    return 0;
}

int CTest::testClientConfiguredCustomContentEncoding(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody("ABCDE")
    //         .addHeader("Content-Encoding: custom"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // connection.addRequestProperty("Accept-Encoding", "custom");
    // assertEquals("ABCDE", readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // RecordedRequest request = server.takeRequest();
    // assertContains(request.getHeaders(), "Accept-Encoding: custom");
    return 0;
}

int CTest::testHeadAndContentLength(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .clearHeaders()
    //         .addHeader("Content-Length: 100"));
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();

    // HttpURLConnection connection1 = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection1.setRequestMethod("HEAD");
    // assertEquals("100", connection1.getHeaderField("Content-Length"));
    // assertContent("", connection1);

    // HttpURLConnection connection2 = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("A", readAscii(connection2.getInputStream(), Integer.MAX_VALUE));

    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertEquals(1, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testSetChunkedStreamingMode(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpURLConnection urlConnection = (HttpURLConnection) server.getUrl("/").openConnection();
    // urlConnection.setChunkedStreamingMode(8);
    // urlConnection.setDoOutput(true);
    // OutputStream outputStream = urlConnection.getOutputStream();
    // outputStream.write("ABCDEFGHIJKLMNOPQ".getBytes("US-ASCII"));
    // assertEquals(200, urlConnection.getResponseCode());

    // RecordedRequest request = server.takeRequest();
    // assertEquals("ABCDEFGHIJKLMNOPQ", new String(request.getBody(), "US-ASCII"));
    // assertEquals(Arrays.asList(3, 3, 3, 3, 3, 2), request.getChunkSizes());
    return 0;
}

int CTest::testAuthenticateWithFixedLengthStreaming(int argc, char* argv[])
{
    // testAuthenticateWithStreamingPost(StreamingMode.FIXED_LENGTH);
    return 0;
}

int CTest::testAuthenticateWithChunkedStreaming(int argc, char* argv[])
{
    // testAuthenticateWithStreamingPost(StreamingMode.CHUNKED);
    return 0;
}

int CTest::testSetValidRequestMethod(int argc, char* argv[])
{
    // server.play();
    // assertValidRequestMethod("GET");
    // assertValidRequestMethod("DELETE");
    // assertValidRequestMethod("HEAD");
    // assertValidRequestMethod("OPTIONS");
    // assertValidRequestMethod("POST");
    // assertValidRequestMethod("PUT");
    // assertValidRequestMethod("TRACE");
    return 0;
}

int CTest::testSetInvalidRequestMethodLowercase(int argc, char* argv[])
{
    // server.play();
    // assertInvalidRequestMethod("get");
    return 0;
}

int CTest::testSetInvalidRequestMethodConnect(int argc, char* argv[])
{
    // server.play();
    // assertInvalidRequestMethod("CONNECT");
    return 0;
}

int CTest::testCannotSetNegativeFixedLengthStreamingMode(int argc, char* argv[])
{
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // try {
    //     connection.setFixedLengthStreamingMode(-2);
    //     fail();
    // } catch (IllegalArgumentException expected) {
    // }
    return 0;
}

int CTest::testCanSetNegativeChunkedStreamingMode(int argc, char* argv[])
{
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setChunkedStreamingMode(-2);
    return 0;
}

int CTest::testCannotSetFixedLengthStreamingModeAfterConnect(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("A", readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // try {
    //     connection.setFixedLengthStreamingMode(1);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testCannotSetChunkedStreamingModeAfterConnect(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("A", readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // try {
    //     connection.setChunkedStreamingMode(1);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testCannotSetFixedLengthStreamingModeAfterChunkedStreamingMode(int argc, char* argv[])
{
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setChunkedStreamingMode(1);
    // try {
    //     connection.setFixedLengthStreamingMode(1);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testCannotSetChunkedStreamingModeAfterFixedLengthStreamingMode(int argc, char* argv[])
{
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setFixedLengthStreamingMode(1);
    // try {
    //     connection.setChunkedStreamingMode(1);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testSecureFixedLengthStreaming(int argc, char* argv[])
{
    // testSecureStreamingPost(StreamingMode.FIXED_LENGTH);
    return 0;
}

int CTest::testSecureChunkedStreaming(int argc, char* argv[])
{
    // testSecureStreamingPost(StreamingMode.CHUNKED);
    return 0;
}

int CTest::testAuthenticateWithPost(int argc, char* argv[])
{
    // MockResponse pleaseAuthenticate = new MockResponse()
    //         .setResponseCode(401)
    //         .addHeader("WWW-Authenticate: Basic realm=\"protected area\"")
    //         .setBody("Please authenticate.");
    // // fail auth three times...
    // server.enqueue(pleaseAuthenticate);
    // server.enqueue(pleaseAuthenticate);
    // server.enqueue(pleaseAuthenticate);
    // // ...then succeed the fourth time
    // server.enqueue(new MockResponse().setBody("Successful auth!"));
    // server.play();

    // Authenticator.setDefault(new SimpleAuthenticator());
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setDoOutput(true);
    // byte[] requestBody = { 'A', 'B', 'C', 'D' };
    // OutputStream outputStream = connection.getOutputStream();
    // outputStream.write(requestBody);
    // outputStream.close();
    // assertEquals("Successful auth!", readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // // no authorization header for the first request...
    // RecordedRequest request = server.takeRequest();
    // assertContainsNoneMatching(request.getHeaders(), "Authorization: .*");

    // // ...but the three requests that follow include an authorization header
    // for (int i = 0; i < 3; i++) {
    //     request = server.takeRequest();
    //     assertEquals("POST / HTTP/1.1", request.getRequestLine());
    //     assertContains(request.getHeaders(), "Authorization: Basic "
    //             + SimpleAuthenticator.BASE_64_CREDENTIALS);
    //     assertEquals(Arrays.toString(requestBody), Arrays.toString(request.getBody()));
    // }
    return 0;
}

int CTest::testAuthenticateWithGet(int argc, char* argv[])
{
    // MockResponse pleaseAuthenticate = new MockResponse()
    //         .setResponseCode(401)
    //         .addHeader("WWW-Authenticate: Basic realm=\"protected area\"")
    //         .setBody("Please authenticate.");
    // // fail auth three times...
    // server.enqueue(pleaseAuthenticate);
    // server.enqueue(pleaseAuthenticate);
    // server.enqueue(pleaseAuthenticate);
    // // ...then succeed the fourth time
    // server.enqueue(new MockResponse().setBody("Successful auth!"));
    // server.play();

    // SimpleAuthenticator authenticator = new SimpleAuthenticator();
    // Authenticator.setDefault(authenticator);
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("Successful auth!", readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // assertEquals(Authenticator.RequestorType.SERVER, authenticator.requestorType);
    // assertEquals(server.getPort(), authenticator.requestingPort);
    // assertEquals(InetAddress.getByName(server.getHostName()), authenticator.requestingSite);
    // assertEquals("protected area", authenticator.requestingPrompt);
    // assertEquals("http", authenticator.requestingProtocol);
    // assertEquals("Basic", authenticator.requestingScheme);

    // // no authorization header for the first request...
    // RecordedRequest request = server.takeRequest();
    // assertContainsNoneMatching(request.getHeaders(), "Authorization: .*");

    // // ...but the three requests that follow requests include an authorization header
    // for (int i = 0; i < 3; i++) {
    //     request = server.takeRequest();
    //     assertEquals("GET / HTTP/1.1", request.getRequestLine());
    //     assertContains(request.getHeaders(), "Authorization: Basic "
    //             + SimpleAuthenticator.BASE_64_CREDENTIALS);
    // }
    return 0;
}

int CTest::testAuthenticateWithCommaSeparatedAuthenticationMethods(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setResponseCode(401)
    //         .addHeader("WWW-Authenticate: Scheme1 realm=\"a\", Scheme2 realm=\"b\", "
    //                 + "Scheme3 realm=\"c\"")
    //         .setBody("Please authenticate."));
    // server.enqueue(new MockResponse().setBody("Successful auth!"));
    // server.play();

    // SimpleAuthenticator authenticator = new SimpleAuthenticator();
    // authenticator.expectedPrompt = "b";
    // Authenticator.setDefault(authenticator);
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("Successful auth!", readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // assertContainsNoneMatching(server.takeRequest().getHeaders(), "Authorization: .*");
    // assertContains(server.takeRequest().getHeaders(),
    //         "Authorization: Scheme2 " + SimpleAuthenticator.BASE_64_CREDENTIALS);
    // assertEquals("Scheme2", authenticator.requestingScheme);
    return 0;
}

int CTest::testAuthenticateWithMultipleAuthenticationHeaders(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setResponseCode(401)
    //         .addHeader("WWW-Authenticate: Scheme1 realm=\"a\"")
    //         .addHeader("WWW-Authenticate: Scheme2 realm=\"b\"")
    //         .addHeader("WWW-Authenticate: Scheme3 realm=\"c\"")
    //         .setBody("Please authenticate."));
    // server.enqueue(new MockResponse().setBody("Successful auth!"));
    // server.play();

    // SimpleAuthenticator authenticator = new SimpleAuthenticator();
    // authenticator.expectedPrompt = "b";
    // Authenticator.setDefault(authenticator);
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("Successful auth!", readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // assertContainsNoneMatching(server.takeRequest().getHeaders(), "Authorization: .*");
    // assertContains(server.takeRequest().getHeaders(),
    //         "Authorization: Scheme2 " + SimpleAuthenticator.BASE_64_CREDENTIALS);
    // assertEquals("Scheme2", authenticator.requestingScheme);
    return 0;
}

int CTest::testRedirectedWithChunkedEncoding(int argc, char* argv[])
{
    // testRedirected(TransferKind.CHUNKED, true);
    return 0;
}

int CTest::testRedirectedWithContentLengthHeader(int argc, char* argv[])
{
    // testRedirected(TransferKind.FIXED_LENGTH, true);
    return 0;
}

int CTest::testRedirectedWithNoLengthHeaders(int argc, char* argv[])
{
    // testRedirected(TransferKind.END_OF_STREAM, false);
    return 0;
}

int CTest::testRedirectedOnHttps(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_MOVED_TEMP)
    //         .addHeader("Location: /foo")
    //         .setBody("This page has moved!"));
    // server.enqueue(new MockResponse().setBody("This is the new location!"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // assertEquals("This is the new location!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // RecordedRequest first = server.takeRequest();
    // assertEquals("GET / HTTP/1.1", first.getRequestLine());
    // RecordedRequest retry = server.takeRequest();
    // assertEquals("GET /foo HTTP/1.1", retry.getRequestLine());
    // assertEquals("Expected connection reuse", 1, retry.getSequenceNumber());
    return 0;
}

int CTest::testNotRedirectedFromHttpsToHttp(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_MOVED_TEMP)
    //         .addHeader("Location: http://anyhost/foo")
    //         .setBody("This page has moved!"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // assertEquals("This page has moved!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    return 0;
}

int CTest::testNotRedirectedFromHttpToHttps(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_MOVED_TEMP)
    //         .addHeader("Location: https://anyhost/foo")
    //         .setBody("This page has moved!"));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("This page has moved!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    return 0;
}

int CTest::testRedirectToAnotherOriginServer(int argc, char* argv[])
{
    // MockWebServer server2 = new MockWebServer();
    // server2.enqueue(new MockResponse().setBody("This is the 2nd server!"));
    // server2.play();

    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_MOVED_TEMP)
    //         .addHeader("Location: " + server2.getUrl("/").toString())
    //         .setBody("This page has moved!"));
    // server.enqueue(new MockResponse().setBody("This is the first server again!"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // assertEquals("This is the 2nd server!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // assertEquals(server2.getUrl("/"), connection.getURL());

    // // make sure the first server was careful to recycle the connection
    // assertEquals("This is the first server again!",
    //         readAscii(server.getUrl("/").openStream(), Integer.MAX_VALUE));

    // RecordedRequest first = server.takeRequest();
    // assertContains(first.getHeaders(), "Host: " + hostName + ":" + server.getPort());
    // RecordedRequest second = server2.takeRequest();
    // assertContains(second.getHeaders(), "Host: " + hostName + ":" + server2.getPort());
    // RecordedRequest third = server.takeRequest();
    // assertEquals("Expected connection reuse", 1, third.getSequenceNumber());

    // server2.shutdown();
    return 0;
}

int CTest::testResponse300MultipleChoiceWithPost(int argc, char* argv[])
{
    // // Chrome doesn't follow the redirect, but Firefox and the RI both do
    // testResponseRedirectedWithPost(HttpURLConnection.HTTP_MULT_CHOICE);
    return 0;
}

int CTest::testResponse301MovedPermanentlyWithPost(int argc, char* argv[])
{
    // testResponseRedirectedWithPost(HttpURLConnection.HTTP_MOVED_PERM);
    return 0;
}

int CTest::testResponse302MovedTemporarilyWithPost(int argc, char* argv[])
{
    // testResponseRedirectedWithPost(HttpURLConnection.HTTP_MOVED_TEMP);
    return 0;
}

int CTest::testResponse303SeeOtherWithPost(int argc, char* argv[])
{
    // testResponseRedirectedWithPost(HttpURLConnection.HTTP_SEE_OTHER);
    return 0;
}

int CTest::testResponse305UseProxy(int argc, char* argv[])
{
    // server.play();
    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_USE_PROXY)
    //         .addHeader("Location: " + server.getUrl("/"))
    //         .setBody("This page has moved!"));
    // server.enqueue(new MockResponse().setBody("Proxy Response"));

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/foo").openConnection();
    // // Fails on the RI, which gets "Proxy Response"
    // assertEquals("This page has moved!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // RecordedRequest page1 = server.takeRequest();
    // assertEquals("GET /foo HTTP/1.1", page1.getRequestLine());
    // assertEquals(1, server.getRequestCount());
    return 0;
}

int CTest::testHttpsWithCustomTrustManager(int argc, char* argv[])
{
    // RecordingHostnameVerifier hostnameVerifier = new RecordingHostnameVerifier();
    // RecordingTrustManager trustManager = new RecordingTrustManager();
    // SSLContext sc = SSLContext.getInstance("TLS");
    // sc.init(null, new TrustManager[] { trustManager }, new java.security.SecureRandom());

    // HostnameVerifier defaultHostnameVerifier = HttpsURLConnection.getDefaultHostnameVerifier();
    // HttpsURLConnection.setDefaultHostnameVerifier(hostnameVerifier);
    // SSLSocketFactory defaultSSLSocketFactory = HttpsURLConnection.getDefaultSSLSocketFactory();
    // HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
    // try {
    //     TestSSLContext testSSLContext = TestSSLContext.create();
    //     server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    //     server.enqueue(new MockResponse().setBody("ABC"));
    //     server.enqueue(new MockResponse().setBody("DEF"));
    //     server.enqueue(new MockResponse().setBody("GHI"));
    //     server.play();

    //     URL url = server.getUrl("/");
    //     assertEquals("ABC", readAscii(url.openStream(), Integer.MAX_VALUE));
    //     assertEquals("DEF", readAscii(url.openStream(), Integer.MAX_VALUE));
    //     assertEquals("GHI", readAscii(url.openStream(), Integer.MAX_VALUE));

    //     assertEquals(Arrays.asList("verify " + hostName), hostnameVerifier.calls);
    //     assertEquals(Arrays.asList("checkServerTrusted ["
    //             + "CN=" + hostName + " 1, "
    //             + "CN=Test Intermediate Certificate Authority 1, "
    //             + "CN=Test Root Certificate Authority 1"
    //             + "] RSA"),
    //             trustManager.calls);
    // } finally {
    //     HttpsURLConnection.setDefaultHostnameVerifier(defaultHostnameVerifier);
    //     HttpsURLConnection.setDefaultSSLSocketFactory(defaultSSLSocketFactory);
    // }
    return 0;
}

int CTest::testConnectTimeouts(int argc, char* argv[])
{
    // StuckServer ss = new StuckServer(false);
    // int serverPort = ss.getLocalPort();
    // URLConnection urlConnection = new URL("http://localhost:" + serverPort).openConnection();
    // int timeout = 1000;
    // urlConnection.setConnectTimeout(timeout);
    // long start = System.currentTimeMillis();
    // try {
    //     urlConnection.getInputStream();
    //     fail();
    // } catch (SocketTimeoutException expected) {
    //     long elapsed = System.currentTimeMillis() - start;
    //     int attempts = InetAddress.getAllByName("localhost").length; // one per IP address
    //     assertTrue("timeout=" +timeout + ", elapsed=" + elapsed + ", attempts=" + attempts,
    //             Math.abs((attempts * timeout) - elapsed) < 500);
    // } finally {
    //     ss.close();
    // }
    return 0;
}

int CTest::testReadTimeouts(int argc, char* argv[])
{
    // /*
    //  * This relies on the fact that MockWebServer doesn't close the
    //  * connection after a response has been sent. This causes the client to
    //  * try to read more bytes than are sent, which results in a timeout.
    //  */
    // MockResponse timeout = new MockResponse()
    //         .setBody("ABC")
    //         .clearHeaders()
    //         .addHeader("Content-Length: 4");
    // server.enqueue(timeout);
    // server.enqueue(new MockResponse().setBody("unused")); // to keep the server alive
    // server.play();

    // URLConnection urlConnection = server.getUrl("/").openConnection();
    // urlConnection.setReadTimeout(1000);
    // InputStream in = urlConnection.getInputStream();
    // assertEquals('A', in.read());
    // assertEquals('B', in.read());
    // assertEquals('C', in.read());
    // try {
    //     in.read(); // if Content-Length was accurate, this would return -1 immediately
    //     fail();
    // } catch (SocketTimeoutException expected) {
    // }
    return 0;
}

int CTest::testSetChunkedEncodingAsRequestProperty(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpURLConnection urlConnection = (HttpURLConnection) server.getUrl("/").openConnection();
    // urlConnection.setRequestProperty("Transfer-encoding", "chunked");
    // urlConnection.setDoOutput(true);
    // urlConnection.getOutputStream().write("ABC".getBytes("UTF-8"));
    // assertEquals(200, urlConnection.getResponseCode());

    // RecordedRequest request = server.takeRequest();
    // assertEquals("ABC", new String(request.getBody(), "UTF-8"));
    return 0;
}

int CTest::testConnectionCloseInRequest(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()); // server doesn't honor the connection: close header!
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpURLConnection a = (HttpURLConnection) server.getUrl("/").openConnection();
    // a.setRequestProperty("Connection", "close");
    // assertEquals(200, a.getResponseCode());

    // HttpURLConnection b = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals(200, b.getResponseCode());

    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertEquals("When connection: close is used, each request should get its own connection",
    //         0, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testConnectionCloseInResponse(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().addHeader("Connection: close"));
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpURLConnection a = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals(200, a.getResponseCode());

    // HttpURLConnection b = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals(200, b.getResponseCode());

    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertEquals("When connection: close is used, each request should get its own connection",
    //         0, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testConnectionCloseWithRedirect(int argc, char* argv[])
{
    // MockResponse response = new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_MOVED_TEMP)
    //         .addHeader("Location: /foo")
    //         .addHeader("Connection: close");
    // server.enqueue(response);
    // server.enqueue(new MockResponse().setBody("This is the new location!"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // assertEquals("This is the new location!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // assertEquals(0, server.takeRequest().getSequenceNumber());
    // assertEquals("When connection: close is used, each request should get its own connection",
    //         0, server.takeRequest().getSequenceNumber());
    return 0;
}

int CTest::testResponseCodeDisagreesWithHeaders(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setResponseCode(HttpURLConnection.HTTP_NO_CONTENT)
    //         .setBody("This body is not allowed!"));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // assertEquals("This body is not allowed!",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    return 0;
}

int CTest::testSingleByteReadIsSigned(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody(new byte[] { -2, -1 }));
    // server.play();

    // URLConnection connection = server.getUrl("/").openConnection();
    // InputStream in = connection.getInputStream();
    // assertEquals(254, in.read());
    // assertEquals(255, in.read());
    // assertEquals(-1, in.read());
    return 0;
}

int CTest::testFlushAfterStreamTransmittedWithChunkedEncoding(int argc, char* argv[])
{
    // testFlushAfterStreamTransmitted(TransferKind.CHUNKED);
    return 0;
}

int CTest::testFlushAfterStreamTransmittedWithFixedLength(int argc, char* argv[])
{
    // testFlushAfterStreamTransmitted(TransferKind.FIXED_LENGTH);
    return 0;
}

int CTest::testFlushAfterStreamTransmittedWithNoLengthHeaders(int argc, char* argv[])
{
    // testFlushAfterStreamTransmitted(TransferKind.END_OF_STREAM);
    return 0;
}

int CTest::testGetHeadersThrows(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setSocketPolicy(DISCONNECT_AT_START));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // try {
    //     connection.getInputStream();
    //     fail();
    // } catch (IOException expected) {
    // }

    // try {
    //     connection.getInputStream();
    //     fail();
    // } catch (IOException expected) {
    // }
    return 0;
}

int CTest::testGetKeepAlive(int argc, char* argv[])
{
    // MockWebServer server = new MockWebServer();
    // server.enqueue(new MockResponse().setBody("ABC"));
    // server.play();

    // // The request should work once and then fail
    // URLConnection connection = server.getUrl("").openConnection();
    // InputStream input = connection.getInputStream();
    // assertEquals("ABC", readAscii(input, Integer.MAX_VALUE));
    // input.close();
    // try {
    //     server.getUrl("").openConnection().getInputStream();
    //     fail();
    // } catch (ConnectException expected) {
    // }
    return 0;
}

int CTest::testLenientUrlToUri(int argc, char* argv[])
{
    // // alphanum
    // testUrlToUriMapping("abzABZ09", "abzABZ09", "abzABZ09", "abzABZ09", "abzABZ09");

    // // control characters
    // testUrlToUriMapping("\u0001", "%01", "%01", "%01", "%01");
    // testUrlToUriMapping("\u001f", "%1F", "%1F", "%1F", "%1F");

    // // ascii characters
    // testUrlToUriMapping("%20", "%20", "%20", "%20", "%20");
    // testUrlToUriMapping("%20", "%20", "%20", "%20", "%20");
    // testUrlToUriMapping(" ", "%20", "%20", "%20", "%20");
    // testUrlToUriMapping("!", "!", "!", "!", "!");
    // testUrlToUriMapping("\"", "%22", "%22", "%22", "%22");
    // testUrlToUriMapping("#", null, null, null, "%23");
    // testUrlToUriMapping("$", "$", "$", "$", "$");
    // testUrlToUriMapping("&", "&", "&", "&", "&");
    // testUrlToUriMapping("'", "'", "'", "'", "'");
    // testUrlToUriMapping("(", "(", "(", "(", "(");
    // testUrlToUriMapping(")", ")", ")", ")", ")");
    // testUrlToUriMapping("*", "*", "*", "*", "*");
    // testUrlToUriMapping("+", "+", "+", "+", "+");
    // testUrlToUriMapping(",", ",", ",", ",", ",");
    // testUrlToUriMapping("-", "-", "-", "-", "-");
    // testUrlToUriMapping(".", ".", ".", ".", ".");
    // testUrlToUriMapping("/", null, "/", "/", "/");
    // testUrlToUriMapping(":", null, ":", ":", ":");
    // testUrlToUriMapping(";", ";", ";", ";", ";");
    // testUrlToUriMapping("<", "%3C", "%3C", "%3C", "%3C");
    // testUrlToUriMapping("=", "=", "=", "=", "=");
    // testUrlToUriMapping(">", "%3E", "%3E", "%3E", "%3E");
    // testUrlToUriMapping("?", null, null, "?", "?");
    // testUrlToUriMapping("@", "@", "@", "@", "@");
    // testUrlToUriMapping("[", null, "%5B", null, "%5B");
    // testUrlToUriMapping("\\", "%5C", "%5C", "%5C", "%5C");
    // testUrlToUriMapping("]", null, "%5D", null, "%5D");
    // testUrlToUriMapping("^", "%5E", "%5E", "%5E", "%5E");
    // testUrlToUriMapping("_", "_", "_", "_", "_");
    // testUrlToUriMapping("`", "%60", "%60", "%60", "%60");
    // testUrlToUriMapping("{", "%7B", "%7B", "%7B", "%7B");
    // testUrlToUriMapping("|", "%7C", "%7C", "%7C", "%7C");
    // testUrlToUriMapping("}", "%7D", "%7D", "%7D", "%7D");
    // testUrlToUriMapping("~", "~", "~", "~", "~");
    // testUrlToUriMapping("~", "~", "~", "~", "~");
    // testUrlToUriMapping("\u007f", "%7F", "%7F", "%7F", "%7F");

    // // beyond ascii
    // testUrlToUriMapping("\u0080", "%C2%80", "%C2%80", "%C2%80", "%C2%80");
    // testUrlToUriMapping("\u20ac", "\u20ac", "\u20ac", "\u20ac", "\u20ac");
    // testUrlToUriMapping("\ud842\udf9f",
    //         "\ud842\udf9f", "\ud842\udf9f", "\ud842\udf9f", "\ud842\udf9f");
    return 0;
}

int CTest::testLenientUrlToUriNul(int argc, char* argv[])
{
    // testUrlToUriMapping("\u0000", "%00", "%00", "%00", "%00"); // RI fails this
    return 0;
}

int CTest::testResponseCacheReturnsNullOutputStream(int argc, char* argv[])
{
    // final AtomicBoolean aborted = new AtomicBoolean();
    // ResponseCache.setDefault(new ResponseCache() {
    //     @Override public CacheResponse get(URI uri, String requestMethod,
    //             Map<String, List<String>> requestHeaders) throws IOException {
    //         return null;
    //     }
    //     @Override public CacheRequest put(URI uri, URLConnection connection) throws IOException {
    //         return new CacheRequest() {
    //             @Override public void abort() {
    //                 aborted.set(true);
    //             }
    //             @Override public OutputStream getBody() throws IOException {
    //                 return null;
    //             }
    //         };
    //     }
    // });

    // server.enqueue(new MockResponse().setBody("abcdef"));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // InputStream in = connection.getInputStream();
    // assertEquals("abc", readAscii(in, 3));
    // in.close();
    // assertFalse(aborted.get()); // The best behavior is ambiguous, but RI 6 doesn't abort here
    return 0;
}

int CTest::testReadAfterLastByte(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .setBody("ABC")
    //         .clearHeaders()
    //         .addHeader("Connection: close")
    //         .setSocketPolicy(SocketPolicy.DISCONNECT_AT_END));
    // server.play();

    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // InputStream in = connection.getInputStream();
    // assertEquals("ABC", readAscii(in, 3));
    // assertEquals(-1, in.read());
    // assertEquals(-1, in.read()); // throws IOException in Gingerbread
    return 0;
}

int CTest::testGetContent(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // InputStream in = (InputStream) connection.getContent();
    // assertEquals("A", readAscii(in, Integer.MAX_VALUE));
    return 0;
}

int CTest::testGetContentOfType(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // try {
    //     connection.getContent(null);
    //     fail();
    // } catch (NullPointerException expected) {
    // }
    // try {
    //     connection.getContent(new Class[] { null });
    //     fail();
    // } catch (NullPointerException expected) {
    // }
    // assertNull(connection.getContent(new Class[] { getClass() }));
    // connection.disconnect();
    return 0;
}

int CTest::testGetOutputStreamOnGetFails(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // try {
    //     connection.getOutputStream();
    //     fail();
    // } catch (ProtocolException expected) {
    // }
    return 0;
}

int CTest::testGetOutputAfterGetInputStreamFails(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setDoOutput(true);
    // try {
    //     connection.getInputStream();
    //     connection.getOutputStream();
    //     fail();
    // } catch (ProtocolException expected) {
    // }
    return 0;
}

int CTest::testSetDoOutputOrDoInputAfterConnectFails(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.connect();
    // try {
    //     connection.setDoOutput(true);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     connection.setDoInput(true);
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    // connection.disconnect();
    return 0;
}

int CTest::testClientSendsContentLength(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // connection.setDoOutput(true);
    // OutputStream out = connection.getOutputStream();
    // out.write(new byte[] { 'A', 'B', 'C' });
    // out.close();
    // assertEquals("A", readAscii(connection.getInputStream(), Integer.MAX_VALUE));
    // RecordedRequest request = server.takeRequest();
    // assertContains(request.getHeaders(), "Content-Length: 3");
    return 0;
}

int CTest::testGetContentLengthConnects(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("ABC"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals(3, connection.getContentLength());
    // connection.disconnect();
    return 0;
}

int CTest::testGetContentTypeConnects(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .addHeader("Content-Type: text/plain")
    //         .setBody("ABC"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("text/plain", connection.getContentType());
    // connection.disconnect();
    return 0;
}

int CTest::testGetContentEncodingConnects(int argc, char* argv[])
{
    // server.enqueue(new MockResponse()
    //         .addHeader("Content-Encoding: identity")
    //         .setBody("ABC"));
    // server.play();
    // HttpURLConnection connection = (HttpURLConnection) server.getUrl("/").openConnection();
    // assertEquals("identity", connection.getContentEncoding());
    // connection.disconnect();
    return 0;
}

int CTest::testUrlContainsQueryButNoPath(int argc, char* argv[])
{
    // server.enqueue(new MockResponse().setBody("A"));
    // server.play();
    // URL url = new URL("http", server.getHostName(), server.getPort(), "?query");
    // assertEquals("A", readAscii(url.openConnection().getInputStream(), Integer.MAX_VALUE));
    // RecordedRequest request = server.takeRequest();
    // assertEquals("GET /?query HTTP/1.1", request.getRequestLine());
    return 0;
}

int CTest::testInputStreamAvailableWithChunkedEncoding(int argc, char* argv[])
{
    // testInputStreamAvailable(TransferKind.CHUNKED);
    return 0;
}

int CTest::testInputStreamAvailableWithContentLengthHeader(int argc, char* argv[])
{
    // testInputStreamAvailable(TransferKind.FIXED_LENGTH);
    return 0;
}

int CTest::testInputStreamAvailableWithNoLengthHeaders(int argc, char* argv[])
{
    // testInputStreamAvailable(TransferKind.END_OF_STREAM);
    return 0;
}

int CTest::testInvalidIpv4Address(int argc, char* argv[])
{
    // try {
    //     URI uri = new URI("http://1111.111.111.111/index.html");
    //     uri.toURL().openConnection().connect();
    //     fail();
    // } catch (UnknownHostException expected) {
    // }
    return 0;
}

int CTest::testUrlWithSpaceInHost(int argc, char* argv[])
{
    // URLConnection urlConnection = new URL("http://and roid.com/").openConnection();
    // try {
    //     urlConnection.getInputStream();
    //     fail();
    // } catch (UnknownHostException expected) {
    // }
    return 0;
}

int CTest::testUrlWithSpaceInHostViaHttpProxy(int argc, char* argv[])
{
    // server.enqueue(new MockResponse());
    // server.play();
    // URLConnection urlConnection = new URL("http://and roid.com/")
    //         .openConnection(server.toProxyAddress());
    // try {
    //     urlConnection.getInputStream();
    //     fail(); // the RI makes a bogus proxy request for "GET http://and roid.com/ HTTP/1.1"
    // } catch (UnknownHostException expected) {
    // }
    return 0;
}

int CTest::testSslFallback(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse().setSocketPolicy(SocketPolicy.FAIL_HANDSHAKE));
    // server.enqueue(new MockResponse().setBody("This required a 2nd handshake"));
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // assertEquals("This required a 2nd handshake",
    //         readAscii(connection.getInputStream(), Integer.MAX_VALUE));

    // RecordedRequest first = server.takeRequest();
    // assertEquals(0, first.getSequenceNumber());
    // RecordedRequest retry = server.takeRequest();
    // assertEquals(0, retry.getSequenceNumber());
    // assertEquals("SSLv3", retry.getSslProtocol());
    return 0;
}

int CTest::testInspectSslBeforeConnect(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // assertNotNull(connection.getHostnameVerifier());
    // try {
    //     connection.getLocalCertificates();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     connection.getServerCertificates();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     connection.getCipherSuite();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    // try {
    //     connection.getPeerPrincipal();
    //     fail();
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testInspectSslAfterConnect(int argc, char* argv[])
{
    // TestSSLContext testSSLContext = TestSSLContext.create();
    // server.useHttps(testSSLContext.serverContext.getSocketFactory(), false);
    // server.enqueue(new MockResponse());
    // server.play();

    // HttpsURLConnection connection = (HttpsURLConnection) server.getUrl("/").openConnection();
    // connection.setSSLSocketFactory(testSSLContext.clientContext.getSocketFactory());
    // connection.connect();
    // assertNotNull(connection.getHostnameVerifier());
    // assertNull(connection.getLocalCertificates());
    // assertNotNull(connection.getServerCertificates());
    // assertNotNull(connection.getCipherSuite());
    // assertNotNull(connection.getPeerPrincipal());
    return 0;
}

CTest::setUp()
{
    // super.setUp();
    // hostName = server.getHostName();
}

CTest::tearDown()
{
    // ResponseCache.setDefault(null);
    // Authenticator.setDefault(null);
    // System.clearProperty("proxyHost");
    // System.clearProperty("proxyPort");
    // System.clearProperty("http.proxyHost");
    // System.clearProperty("http.proxyPort");
    // System.clearProperty("https.proxyHost");
    // System.clearProperty("https.proxyPort");
    // server.shutdown();
    // if (cache != null) {
    //     cache.getCache().delete();
    // }
    // super.tearDown();
}
