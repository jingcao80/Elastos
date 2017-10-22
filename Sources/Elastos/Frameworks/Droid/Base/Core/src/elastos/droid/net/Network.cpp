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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Handler;

using Com::Squareup::Okhttp::IOkHttpClient;
using Elastos::Core::CObject;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::CSocket;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(Network, Object, INetwork, IParcelable)

const Boolean Network::HTTP_KEEP_ALIVE = GetHTTP_KEEP_ALIVE();
const Int32 Network::HTTP_MAX_CONNECTIONS = GetHTTP_MAX_CONNECTIONS();
const Int64 Network::HTTP_KEEP_ALIVE_DURATION_MS = GetHTTP_KEEP_ALIVE_DURATION_MS();

Network::Network()
{
    AutoPtr<IObject> obj;
    Elastos::Core::CObject::New((IObject**)&obj);
    mLock = IInterface::Probe(obj);
}

ECode Network::constructor()
{
    return NOERROR;
}

ECode Network::constructor(
    /* [in] */ Int32 netId)
{
    mNetId = netId;
    return NOERROR;
}

ECode Network::constructor(
    /* [in] */ INetwork* that)
{
    mNetId = ((Network*)that)->mNetId;
    return NOERROR;
}

ECode Network::GetAllByName(
    /* [in] */ const String& host,
    /* [out, callee] */ ArrayOf<IInetAddress*>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInetAddressHelper> inetAddress;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddress);
    return inetAddress->GetAllByNameOnNet(host, mNetId, result);
}

ECode Network::GetByName(
    /* [in] */ const String& host,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInetAddressHelper> inetAddress;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddress);
    return inetAddress->GetByNameOnNet(host, mNetId, result);
}

ECode Network::GetSocketFactory(
    /* [out] */ ISocketFactory** result)
{
    VALIDATE_NOT_NULL(result)

    if (mNetworkBoundSocketFactory == NULL) {
        {
            AutoLock syncLock(mLock);
            if (mNetworkBoundSocketFactory == NULL) {
                mNetworkBoundSocketFactory = new NetworkBoundSocketFactory(mNetId, this);
            }
        }
    }
    *result = ISocketFactory::Probe(mNetworkBoundSocketFactory);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Network::MaybeInitHttpClient()
{
    {    AutoLock syncLock(mLock);
        if (mHostResolver == NULL) {
            // TODO: Waiting for HostResolver
            assert(0);
            // mHostResolver = new HostResolver() {
            //     @Override
            //     public InetAddress[] getAllByName(String host) throws UnknownHostException {
            //         return Network.this.getAllByName(host);
            //     }
            // };
        }
        if (mConnectionPool == NULL) {
            // TODO: Waiting for ConnectionPool
            assert(0);
            // mConnectionPool = new ConnectionPool(httpMaxConnections,
            //         httpKeepAliveDurationMs);
        }
    }
    return NOERROR;
}

ECode Network::OpenConnection(
    /* [in] */ IURL* url,
    /* [out] */ IURLConnection** result)
{
    VALIDATE_NOT_NULL(result)

    MaybeInitHttpClient();
    String protocol;
    url->GetProtocol(&protocol);
    AutoPtr<IOkHttpClient> client;
    // TODO: HttpHandler creates OkHttpClients that share the default ResponseCache.
    // Could this cause unexpected behavior?
    // TODO: Should the network's proxy be specified?
    // TODO: Waiting for OkHttpClient, HttpsHandler, HttpHandler
    assert(0);
    // if (protocol.Equals("http")) {
    //     client = HttpHandler.createHttpOkHttpClient(NULL /* proxy */);
    // } else if (protocol.equals("https")) {
    //     client = HttpsHandler.createHttpsOkHttpClient(NULL /* proxy */);
    // } else {
    //     // OkHttpClient only supports HTTP and HTTPS and returns a null URLStreamHandler if
    //     // passed another protocol.
    //     String s;
    //     IObject::Probe(protocol)->ToString(&s);
    //     Logger::E("Network", "Invalid URL or unrecognized protocol %s", s.string());
    //     return E_MALFORMED_URL_EXCEPTION;
    // }
    // return client.setSocketFactory(getSocketFactory())
    //         .setHostResolver(mHostResolver)
    //         .setConnectionPool(mConnectionPool)
    //         .open(url);
    return NOERROR;
}

ECode Network::BindSocket(
    /* [in] */ ISocket* socket)
{
    Boolean isConnected;
    socket->IsConnected(&isConnected);
    if (isConnected) {
        Logger::E("Network", "Socket is connected");
        return E_SOCKET_EXCEPTION;
    }
    // Query a property of the underlying socket to ensure the underlying
    // socket exists so a file descriptor is available to bind to a network.
    Boolean b;
    socket->GetReuseAddress(&b);
    AutoPtr<IFileDescriptor> fileDescriptor;
    socket->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
    Int32 iFd;
    fileDescriptor->GetInt(&iFd);
    Int32 err;
    NetworkUtils::BindSocketToNetwork(iFd, mNetId, &err);
    if (err != 0) {
        // bindSocketToNetwork returns negative errno.
        // throw new ErrnoException( + netId, -err)
        //         .rethrowAsSocketException();
        Logger::E("Network", "Binding socket to network %d%d", mNetId, -err);
        return E_SOCKET_EXCEPTION;
    }
    return NOERROR;
}

ECode Network::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (INetwork::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<Network> other = (Network*)INetwork::Probe(obj);
    *result = mNetId == other->mNetId;
    return NOERROR;
}

ECode Network::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetId * 11;
    return NOERROR;
}

ECode Network::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = StringUtils::ToString(mNetId);
    return NOERROR;
}

ECode Network::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mNetId);
    parcel->ReadInterfacePtr((Handle32*)&mLock);
    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mNetworkBoundSocketFactory = (NetworkBoundSocketFactory*)ISocketFactory::Probe(obj);

    obj = NULL;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mConnectionPool = IConnectionPool::Probe(obj);

    obj = NULL;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mHostResolver = IHostResolver::Probe(obj);
    return NOERROR;
}

ECode Network::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNetId);
    dest->WriteInterfacePtr(mLock.Get());
    dest->WriteInterfacePtr(TO_IINTERFACE(mNetworkBoundSocketFactory.Get()));
    dest->WriteInterfacePtr(mConnectionPool.Get());
    dest->WriteInterfacePtr(mHostResolver.Get());
    return NOERROR;
}

ECode Network::GetNetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetId;
    return NOERROR;
}

//==============================================================
// Network::NetworkBoundSocketFactory
//==============================================================
Network::NetworkBoundSocketFactory::NetworkBoundSocketFactory(
    /* [in] */ Int32 netId,
    /* [in] */ Network* host)
    : mNetId(netId)
    , mHost(host)
{}

ECode Network::NetworkBoundSocketFactory::ConnectToHost(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ ISocketAddress* localAddress,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // Lookup addresses only on this Network.
    AutoPtr<ArrayOf<IInetAddress*> > hostAddresses;
    mHost->GetAllByName(host, (ArrayOf<IInetAddress*>**)&hostAddresses);
    // Try all addresses.
    for (Int32 i = 0; i < hostAddresses->GetLength(); i++) {
        // try {
        AutoPtr<ISocket> socket;
        ECode ec = CreateSocket((ISocket**)&socket);
        if (!FAILED(ec)) {
            if (localAddress != NULL) ec = socket->Bind(localAddress);
            if (!FAILED(ec)) {
                AutoPtr<IInetSocketAddress> inetSocketAddress;
                ec = CInetSocketAddress::New((*hostAddresses)[i], port, (IInetSocketAddress**)&inetSocketAddress);
                if (!FAILED(ec)) {
                    ec = socket->Connect(ISocketAddress::Probe(inetSocketAddress));
                    if (!FAILED(ec)) {
                        *result = socket;
                        REFCOUNT_ADD(*result);
                        return NOERROR;
                    }
                }
            }
        }
        // } catch (IOException e) {
        if (ec == (ECode)E_IO_EXCEPTION) {
            if (i == (hostAddresses->GetLength() - 1)) return E_IO_EXCEPTION;
            continue;
        }
        return ec;
        // }
    }
    Logger::E("Network", host.string());
    return E_UNKNOWN_HOST_EXCEPTION;
}

ECode Network::NetworkBoundSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localHost,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(localHost, localPort, (IInetSocketAddress**)&inetSocketAddress);
    return ConnectToHost(host, port, ISocketAddress::Probe(inetSocketAddress), result);
}

ECode Network::NetworkBoundSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISocket> socket;
    CreateSocket((ISocket**)&socket);
    AutoPtr<IInetSocketAddress> inetSocketAddress_local;
    CInetSocketAddress::New(localAddress, localPort, (IInetSocketAddress**)&inetSocketAddress_local);
    socket->Bind(ISocketAddress::Probe(inetSocketAddress_local));
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(address, port, (IInetSocketAddress**)&inetSocketAddress);
    socket->Connect(ISocketAddress::Probe(inetSocketAddress));
    *result = socket;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Network::NetworkBoundSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISocket> socket;
    CreateSocket((ISocket**)&socket);
    AutoPtr<IInetSocketAddress> inetSocketAddress;
    CInetSocketAddress::New(host, port, (IInetSocketAddress**)&inetSocketAddress);
    socket->Connect(ISocketAddress::Probe(inetSocketAddress));
    *result = socket;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Network::NetworkBoundSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)

    return ConnectToHost(host, port, NULL, result);
}

ECode Network::NetworkBoundSocketFactory::CreateSocket(
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISocket> socket;
    CSocket::New((ISocket**)&socket);
    mHost->BindSocket(socket);
    *result = socket;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean Network::GetHTTP_KEEP_ALIVE()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String property;
    system->GetProperty(String("http.keepAlive"), String("true"), &property);
    return StringUtils::ParseBoolean(property);
}

Int32 Network::GetHTTP_MAX_CONNECTIONS()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String property;
    system->GetProperty(String("http.maxConnections"), String("5"), &property);
    return HTTP_KEEP_ALIVE ? StringUtils::ParseInt32(property): 0;
}

Int32 Network::GetHTTP_KEEP_ALIVE_DURATION_MS()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String property;
    system->GetProperty(String("http.keepAliveDuration"), String("300000"), &property); // 5 minutes.
    return StringUtils::ParseInt64(property);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
