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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGER_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pManager.h"
#include <elastos/core/Object.h>

using namespace Elastos::Droid::Wifi::P2p::Nsd;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessenger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pManager)
    , public Object
    , public IWifiP2pManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIWifiP2pManager* service);

    /**
     * Registers the application with the Wi-Fi framework. This function
     * must be the first to be called before any p2p operations are performed.
     *
     * @param srcContext is the context of the source
     * @param srcLooper is the Looper on which the callbacks are receivied
     * @param listener for callback at loss of framework communication. Can be null.
     * @return Channel instance that is necessary for performing any further p2p operations
     */
    CARAPI Initialize(
        /* [in] */ IContext* srcContext,
        /* [in] */ ILooper* srcLooper,
        /* [in] */ IWifiP2pManagerChannelListener* listener,
        /* [out] */ IWifiP2pManagerChannel** channel);

    CARAPI InitializeInternal(
        /* [in] */ IContext* srcContext,
        /* [in] */ ILooper* srcLooper,
        /* [in] */ IWifiP2pManagerChannelListener* listener,
        /* [out] */ IWifiP2pManagerChannel** result);

    /**
     * Initiate peer discovery. A discovery process involves scanning for available Wi-Fi peers
     * for the purpose of establishing a connection.
     *
     * <p> The function call immediately returns after sending a discovery request
     * to the framework. The application is notified of a success or failure to initiate
     * discovery through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p> The discovery remains active until a connection is initiated or
     * a p2p group is formed. Register for {@link #WIFI_P2P_PEERS_CHANGED_ACTION} intent to
     * determine when the framework notifies of a change as peers are discovered.
     *
     * <p> Upon receiving a {@link #WIFI_P2P_PEERS_CHANGED_ACTION} intent, an application
     * can request for the list of peers using {@link #requestPeers}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI DiscoverPeers(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

   /**
     * Stop an ongoing peer discovery
     *
     * <p> The function call immediately returns after sending a stop request
     * to the framework. The application is notified of a success or failure to initiate
     * stop through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI StopPeerDiscovery(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Start a p2p connection to a device with the specified configuration.
     *
     * <p> The function call immediately returns after sending a connection request
     * to the framework. The application is notified of a success or failure to initiate
     * connect through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p> Register for {@link #WIFI_P2P_CONNECTION_CHANGED_ACTION} intent to
     * determine when the framework notifies of a change in connectivity.
     *
     * <p> If the current device is not part of a p2p group, a connect request initiates
     * a group negotiation with the peer.
     *
     * <p> If the current device is part of an existing p2p group or has created
     * a p2p group with {@link #createGroup}, an invitation to join the group is sent to
     * the peer device.
     *
     * @param c is the channel created at {@link #initialize}
     * @param config options as described in {@link WifiP2pConfig} class
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI Connect(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pConfig* config,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Cancel any ongoing p2p group negotiation
     *
     * <p> The function call immediately returns after sending a connection cancellation request
     * to the framework. The application is notified of a success or failure to initiate
     * cancellation through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI CancelConnect(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Create a p2p group with the current device as the group owner. This essentially creates
     * an access point that can accept connections from legacy clients as well as other p2p
     * devices.
     *
     * <p class="note"><strong>Note:</strong>
     * This function would normally not be used unless the current device needs
     * to form a p2p connection with a legacy client
     *
     * <p> The function call immediately returns after sending a group creation request
     * to the framework. The application is notified of a success or failure to initiate
     * group creation through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p> Application can request for the group details with {@link #requestGroupInfo}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI CreateGroup(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Remove the current p2p group.
     *
     * <p> The function call immediately returns after sending a group removal request
     * to the framework. The application is notified of a success or failure to initiate
     * group removal through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI RemoveGroup(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    CARAPI Listen(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ Boolean bEnable,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    CARAPI SetWifiP2pChannels(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ Int32 lc,
        /* [in] */ Int32 oc,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    CARAPI StartWps(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWpsInfo* wps,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Register a local service for service discovery. If a local service is registered,
     * the framework automatically responds to a service discovery request from a peer.
     *
     * <p> The function call immediately returns after sending a request to add a local
     * service to the framework. The application is notified of a success or failure to
     * add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p>The service information is set through {@link WifiP2pServiceInfo}.<br>
     * or its subclass calls  {@link WifiP2pUpnpServiceInfo#newInstance} or
     *  {@link WifiP2pDnsSdServiceInfo#newInstance} for a Upnp or Bonjour service
     * respectively
     *
     * <p>The service information can be cleared with calls to
     *  {@link #removeLocalService} or {@link #clearLocalServices}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param servInfo is a local service information.
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI AddLocalService(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pServiceInfo* servInfo,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Remove a registered local service added with {@link #addLocalService}
     *
     * <p> The function call immediately returns after sending a request to remove a
     * local service to the framework. The application is notified of a success or failure to
     * add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param servInfo is the local service information.
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI RemoveLocalService(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pServiceInfo* servInfo,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Clear all registered local services of service discovery.
     *
     * <p> The function call immediately returns after sending a request to clear all
     * local services to the framework. The application is notified of a success or failure to
     * add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI ClearLocalServices(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Register a callback to be invoked on receiving service discovery response.
     * Used only for vendor specific protocol right now. For Bonjour or Upnp, use
     * {@link #setDnsSdResponseListeners} or {@link #setUpnpServiceResponseListener}
     * respectively.
     *
     * <p> see {@link #discoverServices} for the detail.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on receiving service discovery response.
     */
    CARAPI SetServiceResponseListener(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerServiceResponseListener* listener);

    /**
     * Register a callback to be invoked on receiving Bonjour service discovery
     * response.
     *
     * <p> see {@link #discoverServices} for the detail.
     *
     * @param c
     * @param servListener is for listening to a Bonjour service response
     * @param txtListener is for listening to a Bonjour TXT record response
     */
    CARAPI SetDnsSdResponseListeners(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerDnsSdServiceResponseListener* servListener,
        /* [in] */ IWifiP2pManagerDnsSdTxtRecordListener* txtListener);

    /**
     * Register a callback to be invoked on receiving upnp service discovery
     * response.
     *
     * <p> see {@link #discoverServices} for the detail.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on receiving service discovery response.
     */
    CARAPI SetUpnpServiceResponseListener(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerUpnpServiceResponseListener* listener);

    /**
     * Initiate service discovery. A discovery process involves scanning for
     * requested services for the purpose of establishing a connection to a peer
     * that supports an available service.
     *
     * <p> The function call immediately returns after sending a request to start service
     * discovery to the framework. The application is notified of a success or failure to initiate
     * discovery through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p> The services to be discovered are specified with calls to {@link #addServiceRequest}.
     *
     * <p>The application is notified of the response against the service discovery request
     * through listener callbacks registered by {@link #setServiceResponseListener} or
     * {@link #setDnsSdResponseListeners}, or {@link #setUpnpServiceResponseListener}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI DiscoverServices(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Add a service discovery request.
     *
     * <p> The function call immediately returns after sending a request to add service
     * discovery request to the framework. The application is notified of a success or failure to
     * add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p>After service discovery request is added, you can initiate service discovery by
     * {@link #discoverServices}.
     *
     * <p>The added service requests can be cleared with calls to
     * {@link #removeServiceRequest(Channel, WifiP2pServiceRequest, ActionListener)} or
     * {@link #clearServiceRequests(Channel, ActionListener)}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param req is the service discovery request.
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI AddServiceRequest(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pServiceRequest* req,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Remove a specified service discovery request added with {@link #addServiceRequest}
     *
     * <p> The function call immediately returns after sending a request to remove service
     * discovery request to the framework. The application is notified of a success or failure to
     * add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param req is the service discovery request.
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI RemoveServiceRequest(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pServiceRequest* req,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Clear all registered service discovery requests.
     *
     * <p> The function call immediately returns after sending a request to clear all
     * service discovery requests to the framework. The application is notified of a success
     * or failure to add service through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callbacks on success or failure. Can be null.
     */
    CARAPI ClearServiceRequests(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Request the current list of peers.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when peer list is available. Can be null.
     */
    CARAPI RequestPeers(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerPeerListListener* listener);

    /**
     * Request device connection info.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when connection info is available. Can be null.
     */
    CARAPI RequestConnectionInfo(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerConnectionInfoListener* listener);

    /**
     * Request p2p group info.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when group info is available. Can be null.
     */
    CARAPI RequestGroupInfo(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerGroupInfoListener* listener);

    /**
     * Set p2p device name.
     * @hide
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when group info is available. Can be null.
     */
    CARAPI SetDeviceName(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ const String& devName,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /** @hide */
    CARAPI SetWFDInfo(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pWfdInfo* wfdInfo,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Set p2p GO device psk, psk.length=[8,12].
     * Call this before createGroup; if not set,default value is 12345678.
     * @hide
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when group info is available. Can be null.
     */
    CARAPI SetGroupOwnerPsk(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ const String& psk,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Delete a stored persistent group from the system settings.
     *
     * <p> The function call immediately returns after sending a persistent group removal request
     * to the framework. The application is notified of a success or failure to initiate
     * group removal through listener callbacks {@link ActionListener#onSuccess} or
     * {@link ActionListener#onFailure}.
     *
     * <p>The persistent p2p group list stored in the system can be obtained by
     * {@link #requestPersistentGroupInfo(Channel, PersistentGroupInfoListener)} and
     *  a network id can be obtained by {@link WifiP2pGroup#getNetworkId()}.
     *
     * @param c is the channel created at {@link #initialize}
     * @param netId he network id of the p2p group.
     * @param listener for callbacks on success or failure. Can be null.
     * @hide
     */
    CARAPI DeletePersistentGroup(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ Int32 netId,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    /**
     * Request a list of all the persistent p2p groups stored in system.
     *
     * @param c is the channel created at {@link #initialize}
     * @param listener for callback when persistent group info list is available. Can be null.
     * @hide
     */
    CARAPI RequestPersistentGroupInfo(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerPersistentGroupInfoListener* listener);

    CARAPI SetMiracastMode(
        /* [in] */ Int32 mode);

    /**
     * Get a reference to WifiP2pService handler. This is used to establish
     * an AsyncChannel communication with WifiService
     *
     * @return Messenger pointing to the WifiP2pService handler
     * @hide
     */
    CARAPI GetMessenger(
        /* [out] */ IMessenger** msg);

    CARAPI GetP2pStateMachineMessenger(
        /* [out] */ IMessenger** result);

    CARAPI GetNfcHandoverRequest(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerHandoverMessageListener* listener);

    CARAPI GetNfcHandoverSelect(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ IWifiP2pManagerHandoverMessageListener* listener);

    CARAPI InitiatorReportNfcHandover(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ const String& handoverSelect,
        /* [in] */ IWifiP2pManagerActionListener* listener);

    CARAPI ResponderReportNfcHandover(
        /* [in] */ IWifiP2pManagerChannel* c,
        /* [in] */ const String& handoverRequest,
        /* [in] */ IWifiP2pManagerActionListener* listener);

private:
    static CARAPI CheckChannel(
        /* [in] */ IWifiP2pManagerChannel* c);

    static CARAPI CheckServiceInfo(
        /* [in] */ IWifiP2pServiceInfo* info);

    static CARAPI CheckServiceRequest(
        /* [in] */ IWifiP2pServiceRequest* req);

    static CARAPI CheckP2pConfig(
        /* [in] */ IWifiP2pConfig* c);

    CARAPI_(AutoPtr<IWifiP2pManagerChannel>) InitalizeChannel(
        /* [in] */ IContext* srcContext,
        /* [in] */ ILooper* srcLooper,
        /* [in] */ IWifiP2pManagerChannelListener* listener,
        /* [in] */ IMessenger* messenger);

public:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 BASE;

    AutoPtr<IIWifiP2pManager> mService;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGER_H__
