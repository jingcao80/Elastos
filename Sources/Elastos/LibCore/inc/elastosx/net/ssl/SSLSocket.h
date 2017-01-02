//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOSX_NET_SSL_SSLSOCKET_H__
#define __ELASTOSX_NET_SSL_SSLSOCKET_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/net/Socket.h"

using Elastos::Net::Socket;
using Elastos::Net::IInetAddress;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::IHandshakeCompletedListener;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The extension of {@code Socket} providing secure protocols like SSL (Secure
 * Sockets Layer) or TLS (Transport Layer Security).
 *
 * <h3>Default configuration</h3>
 * <p>{@code SSLSocket} instances obtained from default {@link SSLSocketFactory},
 * {@link SSLServerSocketFactory}, and {@link SSLContext} are configured as follows:
 *
 * <h4>Protocols</h4>
 *
 * <p>Client socket:
 * <table>
 *     <thead>
 *         <tr>
 *             <th>Protocol</th>
 *             <th>Supported (API Levels)</th>
 *             <th>Enabled by default (API Levels)</th>
 *         </tr>
 *     </thead>
 *     <tbody>
 *         <tr>
 *             <td>SSLv3</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1.1</td>
 *             <td>16+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1.2</td>
 *             <td>16+</td>
 *             <td>20+</td>
 *         </tr>
 *     </tbody>
 * </table>
 *
 * <p>Server socket:
 * <table>
 *     <thead>
 *         <tr>
 *             <th>Protocol</th>
 *             <th>Supported (API Levels)</th>
 *             <th>Enabled by default (API Levels)</th>
 *         </tr>
 *     </thead>
 *     <tbody>
 *         <tr>
 *             <td>SSLv3</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1.1</td>
 *             <td>16+</td>
 *             <td>16+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1.2</td>
 *             <td>16+</td>
 *             <td>16+</td>
 *         </tr>
 *     </tbody>
 * </table>
 *
 * <h4>Cipher suites</h4>
 *
 * <p>Methods that operate with cipher suite names (for example,
 * {@link #getSupportedCipherSuites() getSupportedCipherSuites},
 * {@link #setEnabledCipherSuites(String[]) setEnabledCipherSuites}) have used
 * standard names for cipher suites since API Level 9, as listed in the table
 * below. Prior to API Level 9, non-standard (OpenSSL) names had been used (see
 * the table following this table).
 * <table>
 *     <thead>
 *         <tr>
 *             <th>Cipher suite</th>
 *             <th>Supported (API Levels)</th>
 *             <th>Enabled by default (API Levels)</th>
 *         </tr>
 *     </thead>
 *     <tbody>
 *         <tr>
 *             <td>SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DHE_DSS_WITH_DES_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DHE_RSA_WITH_DES_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DH_anon_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DH_anon_EXPORT_WITH_RC4_40_MD5</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DH_anon_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DH_anon_WITH_DES_CBC_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_DH_anon_WITH_RC4_128_MD5</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_EXPORT_WITH_RC4_40_MD5</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_DES_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_NULL_MD5</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_NULL_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_RC4_128_MD5</td>
 *             <td>9+</td>
 *             <td>9-19</td>
 *         </tr>
 *         <tr>
 *             <td>SSL_RSA_WITH_RC4_128_SHA</td>
 *             <td>9+</td>
 *             <td>9+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_128_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_256_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_256_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_256_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_128_CBC_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_256_CBC_SHA</td>
 *             <td>9+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_256_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_NULL_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_RC4_128_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA</td>
 *             <td>21+</td>
 *             <td>21+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA</td>
 *             <td>21+</td>
 *             <td>21+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_NULL_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_RC4_128_SHA</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_NULL_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_RC4_128_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_NULL_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_RC4_128_SHA</td>
 *             <td>11+</td>
 *             <td>11-19</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_AES_128_CBC_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_AES_256_CBC_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_NULL_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_RC4_128_SHA</td>
 *             <td>11+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_EMPTY_RENEGOTIATION_INFO_SCSV</td>
 *             <td>11+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_FALLBACK_SCSV</td>
 *             <td>21+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_PSK_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>21+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_PSK_WITH_AES_128_CBC_SHA</td>
 *             <td>21+</td>
 *             <td>21+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_PSK_WITH_AES_256_CBC_SHA</td>
 *             <td>21+</td>
 *             <td>21+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_PSK_WITH_RC4_128_SHA</td>
 *             <td>21+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>9+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_128_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_128_GCM_SHA256</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>9+</td>
 *             <td>11+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_256_CBC_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_AES_256_GCM_SHA384</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_NULL_SHA256</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *     </tbody>
 * </table>
 *
 * <p><em>NOTE</em>: PSK cipher suites are enabled by default only if the {@code SSLContext} through
 * which the socket was created has been initialized with a {@code PSKKeyManager}.
 *
 * <p>API Levels 1 to 8 use OpenSSL names for cipher suites. The table below
 * lists these OpenSSL names and their corresponding standard names used in API
 * Levels 9 and newer.
 * <table>
 *     <thead>
 *         <tr>
 *             <th>OpenSSL cipher suite</th>
 *             <th>Standard cipher suite</th>
 *             <th>Supported (API Levels)</th>
 *             <th>Enabled by default (API Levels)</th>
 *         </tr>
 *     </thead>
 *
 *     <tbody>
 *         <tr>
 *             <td>AES128-SHA</td>
 *             <td>TLS_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>AES256-SHA</td>
 *             <td>TLS_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-8, 11+</td>
 *         </tr>
 *         <tr>
 *             <td>DES-CBC-MD5</td>
 *             <td>SSL_CK_DES_64_CBC_WITH_MD5</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>DES-CBC-SHA</td>
 *             <td>SSL_RSA_WITH_DES_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>DES-CBC3-MD5</td>
 *             <td>SSL_CK_DES_192_EDE3_CBC_WITH_MD5</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>DES-CBC3-SHA</td>
 *             <td>SSL_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>DHE-DSS-AES128-SHA</td>
 *             <td>TLS_DHE_DSS_WITH_AES_128_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>DHE-DSS-AES256-SHA</td>
 *             <td>TLS_DHE_DSS_WITH_AES_256_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-8, 11+</td>
 *         </tr>
 *         <tr>
 *             <td>DHE-RSA-AES128-SHA</td>
 *             <td>TLS_DHE_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *         <tr>
 *             <td>DHE-RSA-AES256-SHA</td>
 *             <td>TLS_DHE_RSA_WITH_AES_256_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-8, 11+</td>
 *         </tr>
 *         <tr>
 *             <td>EDH-DSS-DES-CBC-SHA</td>
 *             <td>SSL_DHE_DSS_WITH_DES_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EDH-DSS-DES-CBC3-SHA</td>
 *             <td>SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EDH-RSA-DES-CBC-SHA</td>
 *             <td>SSL_DHE_RSA_WITH_DES_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EDH-RSA-DES-CBC3-SHA</td>
 *             <td>SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EXP-DES-CBC-SHA</td>
 *             <td>SSL_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EXP-EDH-DSS-DES-CBC-SHA</td>
 *             <td>SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EXP-EDH-RSA-DES-CBC-SHA</td>
 *             <td>SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>EXP-RC2-CBC-MD5</td>
 *             <td>SSL_RSA_EXPORT_WITH_RC2_CBC_40_MD5</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>EXP-RC4-MD5</td>
 *             <td>SSL_RSA_EXPORT_WITH_RC4_40_MD5</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>RC2-CBC-MD5</td>
 *             <td>SSL_CK_RC2_128_CBC_WITH_MD5</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>RC4-MD5</td>
 *             <td>SSL_RSA_WITH_RC4_128_MD5</td>
 *             <td>1+</td>
 *             <td>1-19</td>
 *         </tr>
 *         <tr>
 *             <td>RC4-SHA</td>
 *             <td>SSL_RSA_WITH_RC4_128_SHA</td>
 *             <td>1+</td>
 *             <td>1+</td>
 *         </tr>
 *     </tbody>
 * </table>
 */
class ECO_PUBLIC SSLSocket
    : public Socket
    , public ISSLSocket
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP socket.
     */
    CARAPI constructor();

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP socket connection to the specified host at the specified
     * port.
     *
     * @param host
     *            the host name to connect to.
     * @param port
     *            the port number to connect to.
     * @throws IOException
     *             if creating the socket fails.
     * @throws UnknownHostException
     *             if the specified host is not known.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP socket connection to the specified address at the specified
     * port.
     *
     * @param address
     *            the address to connect to.
     * @param port
     *            the port number to connect to.
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP socket connection to the specified host at the specified
     * port with the client side bound to the specified address and port.
     *
     * @param host
     *            the host name to connect to.
     * @param port
     *            the port number to connect to.
     * @param clientAddress
     *            the client address to bind to
     * @param clientPort
     *            the client port number to bind to.
     * @throws IOException
     *             if creating the socket fails.
     * @throws UnknownHostException
     *             if the specified host is not known.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* clientAddress,
        /* [in] */ Int32 clientPort);

    /**
     * Only to be used by subclasses.
     * <p>
     * Creates a TCP socket connection to the specified address at the specified
     * port with the client side bound to the specified address and port.
     *
     * @param address
     *            the address to connect to.
     * @param port
     *            the port number to connect to.
     * @param clientAddress
     *            the client address to bind to.
     * @param clientPort
     *            the client port number to bind to.
     * @throws IOException
     *             if creating the socket fails.
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* clientAddress,
        /* [in] */ Int32 clientPort);

    /**
     * Unsupported for SSL because reading from an SSL socket may require
     * writing to the network.
     */
    CARAPI ShutdownInput();

    /**
     * Unsupported for SSL because writing to an SSL socket may require reading
     * from the network.
     */
    CARAPI ShutdownOutput();

    /**
     * Returns the names of the supported cipher suites.
     */
    virtual CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the names of the enabled cipher suites.
     */
    virtual CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Sets the names of the cipher suites to be enabled.
     * Only cipher suites returned by {@link #getSupportedCipherSuites()} are
     * allowed.
     *
     * @param suites
     *            the names of the to be enabled cipher suites.
     * @throws IllegalArgumentException
     *             if one of the cipher suite names is not supported.
     */
    virtual CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* suites) = 0;

    /**
     * Returns the names of the supported protocols.
     */
    virtual CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Returns the names of the enabled protocols.
     */
    virtual CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Sets the names of the protocols to be enabled. Only
     * protocols returned by {@link #getSupportedProtocols()} are allowed.
     *
     * @param protocols
     *            the names of the to be enabled protocols.
     * @throws IllegalArgumentException
     *             if one of the protocols is not supported.
     */
    virtual CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols) = 0;

    /**
     * Returns the {@code SSLSession} for this connection. If necessary, a
     * handshake will be initiated, in which case this method will block until the handshake
     * has been established. If the handshake fails, an invalid session object
     * will be returned.
     *
     * @return the session object.
     */
    virtual CARAPI GetSession(
        /* [out] */ ISSLSession** session) = 0;

    /**
     * Registers the specified listener to receive notification on completion of a
     * handshake on this connection.
     *
     * @param listener
     *            the listener to register.
     * @throws IllegalArgumentException
     *             if {@code listener} is {@code null}.
     */
    virtual CARAPI AddHandshakeCompletedListener(
        /* [in] */ IHandshakeCompletedListener* listener) = 0;

    /**
     * Removes the specified handshake completion listener.
     *
     * @param listener
     *            the listener to remove.
     * @throws IllegalArgumentException
     *             if the specified listener is not registered or {@code null}.
     */
    virtual CARAPI RemoveHandshakeCompletedListener(
        /* [in] */ IHandshakeCompletedListener* listener) = 0;

    /**
     * Starts a new SSL handshake on this connection.
     *
     * @throws IOException
     *             if an error occurs.
     */
    virtual CARAPI StartHandshake() = 0;

    /**
     * Sets whether this connection should act in client mode when handshaking.
     *
     * @param mode
     *            {@code true} if this connection should act in client mode,
     *            {@code false} if not.
     */
    virtual CARAPI SetUseClientMode(
        /* [in] */ Boolean mode) = 0;

    /**
     * Returns true if this connection will act in client mode when handshaking.
     */
    virtual CARAPI GetUseClientMode(
        /* [out] */ Boolean* mode) = 0;

    /**
     * Sets whether the server should require client authentication. This
     * does not apply to sockets in {@link #getUseClientMode() client mode}.
     * Client authentication is one of the following:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setWantClientAuth(boolean)}.
     */
    virtual CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need) = 0;

    /**
     * Sets whether the server should request client authentication. Unlike
     * {@link #setNeedClientAuth} this won't stop the negotiation if the client
     * doesn't authenticate. This does not apply to sockets in {@link
     * #getUseClientMode() client mode}.The client authentication is one of:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setNeedClientAuth(boolean)}.
     */
    virtual CARAPI SetWantClientAuth(
        /* [in] */ Boolean want) = 0;

    /**
     * Returns true if the server socket should require client authentication.
     * This does not apply to sockets in {@link #getUseClientMode() client
     * mode}.
     */
    virtual CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* auth) = 0;

    /**
     * Returns true if the server should request client authentication. This
     * does not apply to sockets in {@link #getUseClientMode() client mode}.
     */
    virtual CARAPI GetWantClientAuth(
        /* [out] */ Boolean* auth) = 0;

    /**
     * Sets whether new SSL sessions may be created by this socket or if
     * existing sessions must be reused. If {@code flag} is false and there are
     * no sessions to resume, handshaking will fail.
     *
     * @param flag {@code true} if new sessions may be created.
     */
    virtual CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag) = 0;

    /**
     * Returns whether new SSL sessions may be created by this socket or if
     * existing sessions must be reused.
     *
     * @return {@code true} if new sessions may be created, otherwise
     *         {@code false}.
     */
    virtual CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* enabled) = 0;

    /**
     * Returns a new SSLParameters based on this SSLSocket's current
     * cipher suites, protocols, and client authentication settings.
     *
     * @since 1.6
     */
    CARAPI GetSSLParameters(
        /* [out] */ ISSLParameters** param);

    /**
     * Sets various SSL handshake parameters based on the SSLParameter
     * argument. Specifically, sets the SSLSocket's enabled cipher
     * suites if the parameter's cipher suites are non-null. Similarly
     * sets the enabled protocols. If the parameters specify the want
     * or need for client authentication, those requirements are set
     * on the SSLSocket, otherwise both are set to false.
     * @since 1.6
     */
    CARAPI SetSSLParameters(
        /* [in] */ ISSLParameters* p);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLSOCKET_H__
