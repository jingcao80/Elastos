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

#ifndef __ELASTOSX_NET_SSL_SSLENGINE_H__
#define __ELASTOSX_NET_SSL_SSLENGINE_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::IO::IByteBuffer;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::ISSLEngineResult;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The abstract implementation of secure communications using SSL, TLS, or other
 * protocols. It includes the setup, handshake, and encrypt/decrypt
 * functionality needed to create a secure connection.
 *
 * <h3>Default configuration</h3>
 * <p>{@code SSLEngine} instances obtained from default {@link SSLContext} are configured as
 * follows:
 *
 * <h4>Protocols</h4>
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
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLSv1.2</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *     </tbody>
 * </table>
 *
 * <h4>Cipher suites</h4>
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
 *             <td>TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
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
 *             <td>20+</td>
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
 *             <td>TLS_DHE_DSS_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
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
 *             <td>20+</td>
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
 *             <td>TLS_DHE_RSA_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_DSS_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_RSA_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_DH_anon_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
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
 *             <td>TLS_DH_anon_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA</td>
 *             <td>20+</td>
 *             <td>20+</td>
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
 *             <td>20+</td>
 *             <td>20+</td>
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
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_ECDSA_WITH_RC4_128_SHA</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>20+</td>
 *             <td>20+</td>
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
 *             <td>20+</td>
 *             <td>20+</td>
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
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDHE_RSA_WITH_RC4_128_SHA</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
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
 *             <td>20+</td>
 *             <td></td>
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
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_ECDSA_WITH_RC4_128_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_AES_128_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
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
 *             <td>20+</td>
 *             <td></td>
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
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_RSA_WITH_RC4_128_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_AES_128_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_AES_256_CBC_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_NULL_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_ECDH_anon_WITH_RC4_128_SHA</td>
 *             <td>20+</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_EMPTY_RENEGOTIATION_INFO_SCSV</td>
 *             <td>20+</td>
 *             <td>20+</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_FALLBACK_SCSV</td>
 *             <td>21+</td>
 *             <td></td>
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
 *             <td>TLS_NULL_WITH_NULL_NULL</td>
 *             <td>1-8</td>
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
 *             <td>TLS_RSA_EXPORT_WITH_DES40_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_3DES_EDE_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
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
 *             <td>20+</td>
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
 *             <td>TLS_RSA_WITH_DES_CBC_SHA</td>
 *             <td>1-8</td>
 *             <td>1-8</td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_NULL_MD5</td>
 *             <td>1-8</td>
 *             <td></td>
 *         </tr>
 *         <tr>
 *             <td>TLS_RSA_WITH_NULL_SHA</td>
 *             <td>1-8</td>
 *             <td></td>
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
 * which the engine was created has been initialized with a {@code PSKKeyManager}.
 *
 * @since 1.5
 */
class ECO_PUBLIC SSLEngine
    : public Object
    , public ISSLEngine
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code SSLEngine} instance.
     */
    SSLEngine();

    /**
     * Creates a new {@code SSLEngine} instance with the specified host and
     * port.
     *
     * @param host
     *            the name of the host.
     * @param port
     *            the port of the host.
     */
    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    /**
     * Returns the name of the peer host.
     *
     * @return the name of the peer host, or {@code null} if none is available.
     */
    CARAPI GetPeerHost(
        /* [out] */ String* host);

    /**
     * Returns the port number of the peer host.
     *
     * @return the port number of the peer host, or {@code -1} is none is
     *         available.
     */
    CARAPI GetPeerPort(
        /* [out] */ Int32* port);

    /**
     * Initiates a handshake on this engine.
     * <p>
     * Calling this method is not needed for the initial handshake: it will be
     * called by {@code wrap} or {@code unwrap} if the initial handshake has not
     * been started yet.
     *
     * @throws SSLException
     *             if starting the handshake fails.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    virtual CARAPI BeginHandshake() = 0;

    /**
     * Notifies this engine instance that no more inbound network data will be
     * sent to this engine.
     *
     * @throws SSLException
     *             if this engine did not receive a needed protocol specific
     *             close notification message from the peer.
     */
    virtual CARAPI CloseInbound() = 0;

    /**
     * Notifies this engine instance that no more outbound application data will
     * be sent to this engine.
     */
    virtual CARAPI CloseOutbound() = 0;

    /**
     * Returns a delegate task for this engine instance. Some engine operations
     * may require the results of blocking or long running operations, and the
     * {@code SSLEngineResult} instances returned by this engine may indicate
     * that a delegated task result is needed. In this case the
     * {@link Runnable#run() run} method of the returned {@code Runnable}
     * delegated task must be called.
     *
     * @return a delegate task, or {@code null} if none are available.
     */
    virtual CARAPI GetDelegatedTask(
        /* [out] */ IRunnable** task) = 0;

    /**
     * Returns the SSL cipher suite names that are enabled in this engine
     * instance.
     *
     * @return the SSL cipher suite names that are enabled in this engine
     *         instance.
     */
    virtual CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the protocol version names that are enabled in this engine
     * instance.
     *
     * @return the protocol version names that are enabled in this engine
     *         instance.
     */
    virtual CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Returns whether new SSL sessions may be established by this engine.
     *
     * @return {@code true} if new session may be established, {@code false} if
     *         existing sessions must be reused.
     */
    virtual CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* enabled) = 0;

    /**
     * Returns the status of the handshake of this engine instance.
     *
     * @return the status of the handshake of this engine instance.
     */
    virtual CARAPI GetHandshakeStatus(
        /* [out] */ SSLEngineResult_HandshakeStatus* status) = 0;

    /**
     * Returns whether this engine instance will require client authentication.
     *
     * @return {@code true} if this engine will require client authentication,
     *         {@code false} if no client authentication is needed.
     */
    virtual CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* auth) = 0;

    /**
     * Returns the SSL session for this engine instance.
     *
     * @return the SSL session for this engine instance.
     */
    virtual CARAPI GetSession(
        /* [out] */ ISSLSession** session) = 0;

    /**
     * Returns the SSL cipher suite names that are supported by this engine.
     * These cipher suites can be enabled using
     * {@link #setEnabledCipherSuites(String[])}.
     *
     * @return the SSL cipher suite names that are supported by this engine.
     */
    virtual CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites) = 0;

    /**
     * Returns the protocol names that are supported by this engine. These
     * protocols can be enables using {@link #setEnabledProtocols(String[])}.
     *
     * @return the protocol names that are supported by this engine.
     */
    virtual CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String>** protocols) = 0;

    /**
     * Returns whether this engine is set to act in client mode when
     * handshaking.
     *
     * @return {@code true} if the engine is set to do handshaking in client
     *         mode.
     */
    virtual CARAPI GetUseClientMode(
        /* [out] */ Boolean* mode) = 0;

    /**
     * Returns whether this engine will request client authentication.
     *
     * @return {@code true} if client authentication will be requested,
     *         {@code false} otherwise.
     */
    virtual CARAPI GetWantClientAuth(
        /* [out] */ Boolean* auth) = 0;

    /**
     * Returns whether no more inbound data will be accepted by this engine.
     *
     * @return {@code true} if no more inbound data will be accepted by this
     *         engine, {@code false} otherwise.
     */
    virtual CARAPI IsInboundDone(
        /* [out] */ Boolean* done) = 0;

    /**
     * Returns whether no more outbound data will be produced by this engine.
     *
     * @return {@code true} if no more outbound data will be producted by this
     *         engine, {@code otherwise} false.
     */
    virtual CARAPI IsOutboundDone(
        /* [out] */ Boolean* done) = 0;

    /**
     * Sets the SSL cipher suite names that should be enabled in this engine
     * instance. Only cipher suites listed by {@code getSupportedCipherSuites()}
     * are allowed.
     *
     * @param suites
     *            the SSL cipher suite names to be enabled.
     * @throws IllegalArgumentException
     *             if one of the specified cipher suites is not supported, or if
     *             {@code suites} is {@code null}.
     */
    virtual CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* suites) = 0;

    /**
     * Sets the protocol version names that should be enabled in this engine
     * instance. Only protocols listed by {@code getSupportedProtocols()} are
     * allowed.
     *
     * @param protocols
     *            the protocol version names to be enabled.
     * @throws IllegalArgumentException
     *             if one of the protocol version names is not supported, or if
     *             {@code protocols} is {@code null}.
     */
    virtual CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols) = 0;

    /**
     * Sets whether new SSL sessions may be established by this engine instance.
     *
     * @param flag
     *            {@code true} if new SSL sessions may be established,
     *            {@code false} if existing SSL sessions must be reused.
     */
    virtual CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag) = 0;

    /**
     * Sets whether this engine must require client authentication. The client
     * authentication is one of:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setWantClientAuth(boolean)}.
     *
     * @param need
     *            {@code true} if client authentication is required,
     *            {@code false} if no authentication is needed.
     */
    virtual CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need) = 0;

    /**
     * Sets whether this engine should act in client (or server) mode when
     * handshaking.
     *
     * @param mode
     *            {@code true} if this engine should act in client mode,
     *            {@code false} if not.
     * @throws IllegalArgumentException
     *             if this method is called after starting the initial
     *             handshake.
     */
    virtual CARAPI SetUseClientMode(
        /* [in] */ Boolean mode) = 0;

    /**
     * Sets whether this engine should request client authentication. The client
     * authentication is one of the following:
     * <ul>
     * <li>authentication required</li>
     * <li>authentication requested</li>
     * <li>no authentication needed</li>
     * </ul>
     * This method overrides the setting of {@link #setNeedClientAuth(boolean)}.
     *
     * @param want
     *            {@code true} if client authentication should be requested,
     *            {@code false} if no authentication is needed.
     */
    virtual CARAPI SetWantClientAuth(
        /* [in] */ Boolean want) = 0;

    /**
     * Decodes the incoming network data buffer into application data buffers.
     * If a handshake has not been started yet, it will automatically be
     * started.
     *
     * @param src
     *            the buffer with incoming network data
     * @param dsts
     *            the array of destination buffers for incoming application
     *            data.
     * @param offset
     *            the offset in the array of destination buffers to which data
     *            is to be transferred.
     * @param length
     *            the maximum number of destination buffers to be used.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws IndexOutOfBoundsException
     *             if {@code length} is greater than
     *             {@code dsts.length - offset}.
     * @throws java.nio.ReadOnlyBufferException
     *             if one of the destination buffers is read-only.
     * @throws IllegalArgumentException
     *             if {@code src}, {@code dsts}, or one of the entries in
     *             {@code dsts} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    virtual CARAPI Unwrap(
        /* [in] */ IByteBuffer* src,
        /* [in] */ ArrayOf<IByteBuffer*>* dsts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ ISSLEngineResult** result) = 0;

    /**
     * Encodes the outgoing application data buffers into the network data
     * buffer. If a handshake has not been started yet, it will automatically be
     * started.
     *
     * @param srcs
     *            the array of source buffers of outgoing application data.
     * @param offset
     *            the offset in the array of source buffers from which data is
     *            to be retrieved.
     * @param length
     *            the maximum number of source buffers to be used.
     * @param dst
     *            the destination buffer for network data.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws IndexOutOfBoundsException
     *             if {@code length} is greater than
     *             {@code srcs.length - offset}.
     * @throws java.nio.ReadOnlyBufferException
     *             if the destination buffer is readonly.
     * @throws IllegalArgumentException
     *             if {@code srcs}, {@code dst}, or one the entries in
     *             {@code srcs} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    virtual CARAPI Wrap(
        /* [in] */ ArrayOf<IByteBuffer*>* srcs,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ IByteBuffer* dst,
        /* [out] */ ISSLEngineResult** result) = 0;


    /**
     * Decodes the incoming network data buffer into the application data
     * buffer. If a handshake has not been started yet, it will automatically be
     * started.
     *
     * @param src
     *            the buffer with incoming network data
     * @param dst
     *            the destination buffer for incoming application data.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws java.nio.ReadOnlyBufferException
     *             if one of the destination buffers is read-only.
     * @throws IllegalArgumentException
     *             if {@code src} or {@code dst} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    CARAPI Unwrap(
        /* [in] */ IByteBuffer* src,
        /* [in] */ IByteBuffer* dst,
        /* [out] */ ISSLEngineResult** result);

    /**
     * Decodes the incoming network data buffer into the application data
     * buffers. If a handshake has not been started yet, it will automatically
     * be started.
     *
     * @param src
     *            the buffer with incoming network data
     * @param dsts
     *            the array of destination buffers for incoming application
     *            data.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws java.nio.ReadOnlyBufferException
     *             if one of the destination buffers is read-only.
     * @throws IllegalArgumentException
     *             if {@code src} or {@code dsts} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    CARAPI Unwrap(
        /* [in] */ IByteBuffer* src,
        /* [in] */ ArrayOf<IByteBuffer*>* dsts,
        /* [out] */ ISSLEngineResult** result);

    /**
     * Encodes the outgoing application data buffers into the network data
     * buffer. If a handshake has not been started yet, it will automatically be
     * started.
     *
     * @param srcs
     *            the array of source buffers of outgoing application data.
     * @param dst
     *            the destination buffer for network data.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws java.nio.ReadOnlyBufferException
     *             if the destination buffer is readonly.
     * @throws IllegalArgumentException
     *             if {@code srcs} or {@code dst} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    CARAPI Wrap(
        /* [in] */ ArrayOf<IByteBuffer*>* srcs,
        /* [in] */ IByteBuffer* dst,
        /* [out] */ ISSLEngineResult** result);

    /**
     * Encodes the outgoing application data buffer into the network data
     * buffer. If a handshake has not been started yet, it will automatically be
     * started.
     *
     * @param src
     *            the source buffers of outgoing application data.
     * @param dst
     *            the destination buffer for network data.
     * @return the result object of this operation.
     * @throws SSLException
     *             if a problem occurred while processing the data.
     * @throws java.nio.ReadOnlyBufferException
     *             if the destination buffer is readonly.
     * @throws IllegalArgumentException
     *             if {@code src} or {@code dst} is {@code null}.
     * @throws IllegalStateException
     *             if the engine does not have all the needed settings (e.g.
     *             client/server mode not set).
     */
    CARAPI Wrap(
        /* [in] */ IByteBuffer* src,
        /* [in] */ IByteBuffer* dst,
        /* [out] */ ISSLEngineResult** result);

    /**
     * Returns a new SSLParameters based on this SSLSocket's current
     * cipher suites, protocols, and client authentication settings.
     *
     * @since 1.6
     */
    CARAPI GetSSLParameters(
        /* [out] */ ISSLParameters** parameters);

    /**
     * Sets various SSL handshake parameters based on the SSLParameter
     * argument. Specifically, sets the SSLEngine's enabled cipher
     * suites if the parameter's cipher suites are non-null. Similarly
     * sets the enabled protocols. If the parameters specify the want
     * or need for client authentication, those requirements are set
     * on the SSLEngine, otherwise both are set to false.
     * @since 1.6
     */
    CARAPI SetSSLParameters(
        /* [in] */ ISSLParameters* p);

private:
    /*const*/ String mPeerHost;
    /*const*/ Int32 mPeerPort;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLENGINE_H__
