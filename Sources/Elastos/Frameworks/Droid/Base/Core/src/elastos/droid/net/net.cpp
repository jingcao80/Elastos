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

#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/net/CConnectivityManagerNetworkCallback.h"
#include "elastos/droid/net/CDhcpResults.h"
#include "elastos/droid/net/CDhcpStateMachine.h"
#include "elastos/droid/net/CEthernetManager.h"
#include "elastos/droid/net/CHierarchicalUri.h"
#include "elastos/droid/net/CIpConfiguration.h"
#include "elastos/droid/net/CIpPrefix.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CLinkPropertiesCompareResult.h"
#include "elastos/droid/net/CLinkQualityInfo.h"
#include "elastos/droid/net/CLocalServerSocket.h"
#include "elastos/droid/net/CLocalSocket.h"
#include "elastos/droid/net/CLocalSocketAddress.h"
#include "elastos/droid/net/CMailTo.h"
#include "elastos/droid/net/CMobileDataStateTracker.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CNetworkConfig.h"
#include "elastos/droid/net/CNetworkFactory.h"
#include "elastos/droid/net/CNetworkIdentity.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CNetworkKey.h"
#include "elastos/droid/net/CNetworkMisc.h"
#include "elastos/droid/net/CNetworkPolicy.h"
#include "elastos/droid/net/CNetworkPolicyManager.h"
#include "elastos/droid/net/CNetworkQuotaInfo.h"
#include "elastos/droid/net/CNetworkRequest.h"
#include "elastos/droid/net/CNetworkRequestBuilder.h"
#include "elastos/droid/net/CNetworkScoreManager.h"
#include "elastos/droid/net/CNetworkScorerAppData.h"
#include "elastos/droid/net/CNetworkState.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsEntry.h"
#include "elastos/droid/net/CNetworkStatsHistory.h"
#include "elastos/droid/net/CNetworkStatsHistoryEntry.h"
#include "elastos/droid/net/CNetworkTemplate.h"
#include "elastos/droid/net/COpaqueUri.h"
#include "elastos/droid/net/CPacProxySelector.h"
#include "elastos/droid/net/CProxyDataTracker.h"
#include "elastos/droid/net/CProxyInfo.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/CRssiCurve.h"
#include "elastos/droid/net/CSSLCertificateSocketFactory.h"
#include "elastos/droid/net/CSSLSessionCache.h"
#include "elastos/droid/net/CSamplingDataTracker.h"
#include "elastos/droid/net/CSamplingDataTrackerSamplingSnapshot.h"
#include "elastos/droid/net/CScoredNetwork.h"
#include "elastos/droid/net/CSntpClient.h"
#include "elastos/droid/net/CStaticIpConfiguration.h"
#include "elastos/droid/net/CStringUri.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/net/CUidRange.h"
#include "elastos/droid/net/CUrlQuerySanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerIllegalCharacterValueSanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerParameterValuePair.h"
#include "elastos/droid/net/CVpnService.h"
#include "elastos/droid/net/CVpnServiceBuilder.h"
#include "elastos/droid/net/CWebAddress.h"
#include "elastos/droid/net/CWifiKey.h"
#include "elastos/droid/net/CWifiLinkQualityInfo.h"

#include "elastos/droid/net/http/CElastosHttpClient.h"
#include "elastos/droid/net/http/CElastosHttpClientConnection.h"
#include "elastos/droid/net/http/CCertificateChainValidator.h"
#include "elastos/droid/net/http/CDelegatingSSLSession.h"
#include "elastos/droid/net/http/CDelegatingSSLSessionCertificateWrap.h"
#include "elastos/droid/net/http/CHeaders.h"
#include "elastos/droid/net/http/CHttpAuthHeader.h"
#include "elastos/droid/net/http/CHttpResponseCache.h"
#include "elastos/droid/net/http/CHttpsConnection.h"
#include "elastos/droid/net/http/CLoggingEventHandler.h"
#include "elastos/droid/net/http/CRequestHandle.h"
#include "elastos/droid/net/http/CRequestQueue.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslCertificateDName.h"
#include "elastos/droid/net/http/CSslError.h"
#include "elastos/droid/net/http/CX509TrustManagerExtensions.h"

#include "elastos/droid/net/nsd/CDnsSdTxtRecord.h"
#include "elastos/droid/net/nsd/CNsdManager.h"
#include "elastos/droid/net/nsd/CNsdServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_OBJECT_IMPL(CConnectivityManagerNetworkCallback)

CAR_OBJECT_IMPL(CDhcpResults)

CAR_OBJECT_IMPL(CDhcpStateMachine)

CAR_OBJECT_IMPL(CEthernetManager)

CAR_OBJECT_IMPL(CHierarchicalUri)

CAR_OBJECT_IMPL(CIpConfiguration)

CAR_OBJECT_IMPL(CIpPrefix)

CAR_OBJECT_IMPL(CLinkAddress)

CAR_OBJECT_IMPL(CLinkProperties)

CAR_OBJECT_IMPL(CLinkPropertiesCompareResult)

CAR_OBJECT_IMPL(CLinkQualityInfo)

CAR_OBJECT_IMPL(CLocalServerSocket)

CAR_OBJECT_IMPL(CLocalSocket)

CAR_OBJECT_IMPL(CLocalSocketAddress)

CAR_OBJECT_IMPL(CMailTo)

CAR_OBJECT_IMPL(CMobileDataStateTracker)

CAR_OBJECT_IMPL(CNetwork)

CAR_OBJECT_IMPL(CNetworkCapabilities)

CAR_OBJECT_IMPL(CNetworkConfig)

CAR_OBJECT_IMPL(CNetworkFactory)

CAR_OBJECT_IMPL(CNetworkIdentity)

CAR_OBJECT_IMPL(CNetworkInfo)

CAR_OBJECT_IMPL(CNetworkKey)

CAR_OBJECT_IMPL(CNetworkMisc)

CAR_OBJECT_IMPL(CNetworkPolicy)

CAR_OBJECT_IMPL(CNetworkPolicyManager)

CAR_OBJECT_IMPL(CNetworkQuotaInfo)

CAR_OBJECT_IMPL(CNetworkRequest)

CAR_OBJECT_IMPL(CNetworkRequestBuilder)

CAR_OBJECT_IMPL(CNetworkScoreManager)

CAR_OBJECT_IMPL(CNetworkScorerAppData)

CAR_OBJECT_IMPL(CNetworkState)

CAR_OBJECT_IMPL(CNetworkStats)

CAR_OBJECT_IMPL(CNetworkStatsEntry)

CAR_OBJECT_IMPL(CNetworkStatsHistory)

CAR_OBJECT_IMPL(CNetworkStatsHistoryEntry)

CAR_OBJECT_IMPL(CNetworkTemplate)

CAR_OBJECT_IMPL(COpaqueUri)

CAR_OBJECT_IMPL(CPacProxySelector)

CAR_OBJECT_IMPL(CProxyDataTracker)

CAR_OBJECT_IMPL(CProxyInfo)

CAR_OBJECT_IMPL(CRouteInfo)

CAR_OBJECT_IMPL(CRssiCurve)

CAR_OBJECT_IMPL(CSSLCertificateSocketFactory)

CAR_OBJECT_IMPL(CSSLSessionCache)

CAR_OBJECT_IMPL(CSamplingDataTracker)

CAR_OBJECT_IMPL(CSamplingDataTrackerSamplingSnapshot)

CAR_OBJECT_IMPL(CScoredNetwork)

CAR_OBJECT_IMPL(CSntpClient)

CAR_OBJECT_IMPL(CStaticIpConfiguration)

CAR_OBJECT_IMPL(CStringUri)

CAR_OBJECT_IMPL(CUriBuilder)

CAR_OBJECT_IMPL(CUidRange)

CAR_OBJECT_IMPL(CUrlQuerySanitizer)

CAR_OBJECT_IMPL(CUrlQuerySanitizerParameterValuePair)

CAR_OBJECT_IMPL(CUrlQuerySanitizerIllegalCharacterValueSanitizer)

CAR_OBJECT_IMPL(CVpnService)

CAR_OBJECT_IMPL(CVpnServiceBuilder)

CAR_OBJECT_IMPL(CWebAddress)

CAR_OBJECT_IMPL(CWifiKey)

CAR_OBJECT_IMPL(CWifiLinkQualityInfo)

} // namespace Net
} // namespace Droid
} // namespace Elastos

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_OBJECT_IMPL(CElastosHttpClient)

CAR_OBJECT_IMPL(CElastosHttpClientConnection)

CAR_OBJECT_IMPL(CCertificateChainValidator)

CAR_OBJECT_IMPL(CDelegatingSSLSession)

CAR_OBJECT_IMPL(CDelegatingSSLSessionCertificateWrap)

CAR_OBJECT_IMPL(CHeaders)

CAR_OBJECT_IMPL(CHttpAuthHeader)

CAR_OBJECT_IMPL(CHttpResponseCache)

CAR_OBJECT_IMPL(CHttpsConnection)

CAR_OBJECT_IMPL(CLoggingEventHandler)

CAR_OBJECT_IMPL(CRequestHandle)

CAR_OBJECT_IMPL(CRequestQueue)

CAR_OBJECT_IMPL(CSslCertificate)

CAR_OBJECT_IMPL(CSslCertificateDName)

CAR_OBJECT_IMPL(CSslError)

CAR_OBJECT_IMPL(CX509TrustManagerExtensions)

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

CAR_OBJECT_IMPL(CDnsSdTxtRecord)

CAR_OBJECT_IMPL(CNsdManager)

CAR_OBJECT_IMPL(CNsdServiceInfo)

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

