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

#ifndef __ELASTOS_DROID_NET_NSD_NSDMANAGER_H__
#define __ELASTOS_DROID_NET_NSD_NSDMANAGER_H__

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Utility::Concurrent::ICountDownLatch;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * The Network Service Discovery Manager class provides the API to discover services
 * on a network. As an example, if device A and device B are connected over a Wi-Fi
 * network, a game registered on device A can be discovered by a game on device
 * B. Another example use case is an application discovering printers on the network.
 *
 * <p> The API currently supports DNS based service discovery and discovery is currently
 * limited to a local network over Multicast DNS. DNS service discovery is described at
 * http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt
 *
 * <p> The API is asynchronous and responses to requests from an application are on listener
 * callbacks on a seperate thread.
 *
 * <p> There are three main operations the API supports - registration, discovery and resolution.
 * <pre>
 *                          Application start
 *                                 |
 *                                 |
 *                                 |                  onServiceRegistered()
 *                     Register any local services  /
 *                      to be advertised with       \
 *                       registerService()            onRegistrationFailed()
 *                                 |
 *                                 |
 *                          discoverServices()
 *                                 |
 *                      Maintain a list to track
 *                        discovered services
 *                                 |
 *                                 |--------->
 *                                 |          |
 *                                 |      onServiceFound()
 *                                 |          |
 *                                 |     add service to list
 *                                 |          |
 *                                 |<----------
 *                                 |
 *                                 |--------->
 *                                 |          |
 *                                 |      onServiceLost()
 *                                 |          |
 *                                 |   remove service from list
 *                                 |          |
 *                                 |<----------
 *                                 |
 *                                 |
 *                                 | Connect to a service
 *                                 | from list ?
 *                                 |
 *                          resolveService()
 *                                 |
 *                         onServiceResolved()
 *                                 |
 *                     Establish connection to service
 *                     with the host and port information
 *
 * </pre>
 * An application that needs to advertise itself over a network for other applications to
 * discover it can do so with a call to {@link #registerService}. If Example is a http based
 * application that can provide HTML data to peer services, it can register a name "Example"
 * with service type "_http._tcp". A successful registration is notified with a callback to
 * {@link RegistrationListener#onServiceRegistered} and a failure to register is notified
 * over {@link RegistrationListener#onRegistrationFailed}
 *
 * <p> A peer application looking for http services can initiate a discovery for "_http._tcp"
 * with a call to {@link #discoverServices}. A service found is notified with a callback
 * to {@link DiscoveryListener#onServiceFound} and a service lost is notified on
 * {@link DiscoveryListener#onServiceLost}.
 *
 * <p> Once the peer application discovers the "Example" http srevice, and needs to receive data
 * from the "Example" application, it can initiate a resolve with {@link #resolveService} to
 * resolve the host and port details for the purpose of establishing a connection. A successful
 * resolve is notified on {@link ResolveListener#onServiceResolved} and a failure is notified
 * on {@link ResolveListener#onResolveFailed}.
 *
 * Applications can reserve for a service type at
 * http://www.iana.org/form/ports-service. Existing services can be found at
 * http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xml
 *
 * Get an instance of this class by calling {@link android.content.Context#getSystemService(String)
 * Context.getSystemService(Context.NSD_SERVICE)}.
 *
 * {@see NsdServiceInfo}
 */
class NsdManager
    : public Object
    , public INsdManager
{
private:
    class ServiceHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("NsdManager::ServiceHandler")

        ServiceHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ NsdManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        NsdManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    NsdManager();

    /**
     * Create a new Nsd instance. Applications use
     * {@link android.content.Context#getSystemService Context.getSystemService()} to retrieve
     * {@link android.content.Context#NSD_SERVICE Context.NSD_SERVICE}.
     * @param service the Binder interface
     * @hide - hide this because it takes in a parameter of type INsdManager, which
     * is a system private class.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINsdManager* service);

    /**
     * Register a service to be discovered by other services.
     *
     * <p> The function call immediately returns after sending a request to register service
     * to the framework. The application is notified of a successful registration
     * through the callback {@link RegistrationListener#onServiceRegistered} or a failure
     * through {@link RegistrationListener#onRegistrationFailed}.
     *
     * <p> The application should call {@link #unregisterService} when the service
     * registration is no longer required, and/or whenever the application is stopped.
     *
     * @param serviceInfo The service being registered
     * @param protocolType The service discovery protocol
     * @param listener The listener notifies of a successful registration and is used to
     * unregister this service through a call on {@link #unregisterService}. Cannot be null.
     * Cannot be in use for an active service registration.
     */
    CARAPI RegisterService(
        /* [in] */ INsdServiceInfo* serviceInfo,
        /* [in] */ Int32 protocolType,
        /* [in] */ INsdManagerRegistrationListener* listener);

    /**
     * Unregister a service registered through {@link #registerService}. A successful
     * unregister is notified to the application with a call to
     * {@link RegistrationListener#onServiceUnregistered}.
     *
     * @param listener This should be the listener object that was passed to
     * {@link #registerService}. It identifies the service that should be unregistered
     * and notifies of a successful or unsuccessful unregistration via the listener
     * callbacks.  In API versions 20 and above, the listener object may be used for
     * another service registration once the callback has been called.  In API versions <= 19,
     * there is no entirely reliable way to know when a listener may be re-used, and a new
     * listener should be created for each service registration request.
     */
    CARAPI UnregisterService(
        /* [in] */ INsdManagerRegistrationListener* listener);

    /**
     * Initiate service discovery to browse for instances of a service type. Service discovery
     * consumes network bandwidth and will continue until the application calls
     * {@link #stopServiceDiscovery}.
     *
     * <p> The function call immediately returns after sending a request to start service
     * discovery to the framework. The application is notified of a success to initiate
     * discovery through the callback {@link DiscoveryListener#onDiscoveryStarted} or a failure
     * through {@link DiscoveryListener#onStartDiscoveryFailed}.
     *
     * <p> Upon successful start, application is notified when a service is found with
     * {@link DiscoveryListener#onServiceFound} or when a service is lost with
     * {@link DiscoveryListener#onServiceLost}.
     *
     * <p> Upon failure to start, service discovery is not active and application does
     * not need to invoke {@link #stopServiceDiscovery}
     *
     * <p> The application should call {@link #stopServiceDiscovery} when discovery of this
     * service type is no longer required, and/or whenever the application is paused or
     * stopped.
     *
     * @param serviceType The service type being discovered. Examples include "_http._tcp" for
     * http services or "_ipp._tcp" for printers
     * @param protocolType The service discovery protocol
     * @param listener  The listener notifies of a successful discovery and is used
     * to stop discovery on this serviceType through a call on {@link #stopServiceDiscovery}.
     * Cannot be null. Cannot be in use for an active service discovery.
     */
    CARAPI DiscoverServices(
        /* [in] */ const String& serviceType,
        /* [in] */ Int32 protocolType,
        /* [in] */ INsdManagerDiscoveryListener* listener);

    /**
     * Stop service discovery initiated with {@link #discoverServices}.  An active service
     * discovery is notified to the application with {@link DiscoveryListener#onDiscoveryStarted}
     * and it stays active until the application invokes a stop service discovery. A successful
     * stop is notified to with a call to {@link DiscoveryListener#onDiscoveryStopped}.
     *
     * <p> Upon failure to stop service discovery, application is notified through
     * {@link DiscoveryListener#onStopDiscoveryFailed}.
     *
     * @param listener This should be the listener object that was passed to {@link #discoverServices}.
     * It identifies the discovery that should be stopped and notifies of a successful or
     * unsuccessful stop.  In API versions 20 and above, the listener object may be used for
     * another service discovery once the callback has been called.  In API versions <= 19,
     * there is no entirely reliable way to know when a listener may be re-used, and a new
     * listener should be created for each service discovery request.
     */
    CARAPI StopServiceDiscovery(
        /* [in] */ INsdManagerDiscoveryListener* listener);

    /**
     * Resolve a discovered service. An application can resolve a service right before
     * establishing a connection to fetch the IP and port details on which to setup
     * the connection.
     *
     * @param serviceInfo service to be resolved
     * @param listener to receive callback upon success or failure. Cannot be null.
     * Cannot be in use for an active service resolution.
     */
    CARAPI ResolveService(
        /* [in] */ INsdServiceInfo* serviceInfo,
        /* [in] */ INsdManagerResolveListener* listener);

    /** Internal use only @hide */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

private:
    // return its key.
    CARAPI PutListener(
        /* [in] */ IInterface* listener,
        /* [in] */ INsdServiceInfo* s,
        /* [out] */ Int32* result);

    CARAPI GetListener(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** result);

    CARAPI GetNsdService(
        /* [in] */ Int32 key,
        /* [out] */ INsdServiceInfo** result);

    CARAPI RemoveListener(
        /* [in] */ Int32 key);

    CARAPI GetListenerKey(
        /* [in] */ IInterface* listener,
        /* [out] */ Int32* result);

    CARAPI GetNsdServiceInfoType(
        /* [in] */ INsdServiceInfo* s,
        /* [out] */ String* result);

    /**
     * Initialize AsyncChannel
     */
    CARAPI Init();

    /**
     * Get a reference to NetworkService handler. This is used to establish
     * an AsyncChannel communication with the service
     *
     * @return Messenger pointing to the NetworkService handler
     */
    CARAPI GetMessenger(
        /* [out] */ IMessenger** result);

public:
    AutoPtr<IINsdManager> mService;

private:
    static const String TAG;

    static const Int32 BASE;

    AutoPtr<IContext> mContext;

    static const Int32 INVALID_LISTENER_KEY;

    static const Int32 BUSY_LISTENER_KEY;

    Int32 mListenerKey;

    /* const */ AutoPtr<ISparseArray> mListenerMap;

    /* const */ AutoPtr<ISparseArray> mServiceMap;

    /* const */ Object mMapLock;

    /* const */ AutoPtr<IAsyncChannel> mAsyncChannel;

    AutoPtr<ServiceHandler> mHandler;

    /* const */ AutoPtr<ICountDownLatch> mConnected;
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_NSDMANAGER_H__
