
#include "org/apache/http/conn/MultihomePlainSocketFactory.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "elastos/net/CInetAddressHelper.h"
#include "elastos/net/CInetSocketAddress.h"
#include "elastos/net/CSocket.h"
#include "elastos/utility/CCollections.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICloneable;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::CSocket;
using Elastos::Net::ISocketAddress;
using Elastos::Net::ECLSID_CSocket;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::Scheme::EIID_ISocketFactory;
using Org::Apache::Http::Params::HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

AutoPtr<IMultihomePlainSocketFactory> MultihomePlainSocketFactory::InitSocketFactory()
{
    AutoPtr<IMultihomePlainSocketFactory> socketFactory = (IMultihomePlainSocketFactory*)new MultihomePlainSocketFactory();
    return socketFactory;
}

const AutoPtr<IMultihomePlainSocketFactory> MultihomePlainSocketFactory::DEFAULT_FACTORY = MultihomePlainSocketFactory::InitSocketFactory();

CAR_INTERFACE_IMPL_2(MultihomePlainSocketFactory, Object, IMultihomePlainSocketFactory, ISocketFactory)

AutoPtr<IMultihomePlainSocketFactory> MultihomePlainSocketFactory::GetSocketFactory()
{
    return DEFAULT_FACTORY;
}

ECode MultihomePlainSocketFactory::CreateSocket(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    return CSocket::New(socket);
}

ECode MultihomePlainSocketFactory::ConnectSocket(
    /* [in] */ ISocket* _sock,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    if (host.IsNull()) {
        Logger::E("MultihomePlainSocketFactory", "Target host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("MultihomePlainSocketFactory", "Parameters may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISocket> sock = _sock;
    if (sock == NULL)
        CreateSocket((ISocket**)&sock);

    if ((localAddress != NULL) || (localPort > 0)) {

        // we need to bind explicitly
        if (localPort < 0)
            localPort = 0; // indicates "any"

        AutoPtr<IInetSocketAddress> isa;
        CInetSocketAddress::New(localAddress, localPort, (IInetSocketAddress**)&isa);
        AutoPtr<ISocketAddress> sa = ISocketAddress::Probe(isa);
        sock->Bind(sa);
    }

    Int32 timeout;
    HttpConnectionParams::GetConnectionTimeout(params, &timeout);

    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr< ArrayOf<IInetAddress*> > inetadrs;
    helper->GetAllByName(host, (ArrayOf<IInetAddress*>**)&inetadrs);
    Int32 len = inetadrs->GetLength();
    AutoPtr<IArrayList> addresses;
    CArrayList::New(len, (IArrayList**)&addresses);
    AutoPtr<ICollection> col = ICollection::Probe(addresses);
    for (Int32 i = 0; i < len; i++) {
        col->Add((*inetadrs)[i]);
    }
    AutoPtr<ICollections> cols;
    CCollections::AcquireSingleton((ICollections**)&cols);
    cols->Shuffle(IList::Probe(addresses));

    // IOException lastEx = null;
    AutoPtr<IIterator> it;
    addresses->GetIterator((IIterator**)&it);
    ECode ec = NOERROR;
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInetAddress> address;
        it->GetNext((IInterface**)&address);
        // try {
        AutoPtr<IInetSocketAddress> insa;
        CInetSocketAddress::New(address, port, (IInetSocketAddress**)&insa);
        AutoPtr<ISocketAddress> sa = ISocketAddress::Probe(insa);
        ec = sock->Connect(sa, timeout);
        if (ec == (ECode)E_SOCKET_TIMEOUT_EXCEPTION) {
            return ec;
        }
        else if (ec == (ECode)E_IO_EXCEPTION) {
            sock = NULL;
            CSocket::New((ISocket**)&sock);
            continue;
        }
        break;
        // } catch (SocketTimeoutException ex) {
        //     throw ex;
        // } catch (IOException ex) {
        //     // create new socket
        //     sock = new Socket();
        //     // keep the last exception and retry
        //     lastEx = ex;
        // }
    }
    // if (lastEx != null) {
    //     throw lastEx;
    // }
    if (FAILED(ec)) return ec;
    *socket = sock;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode MultihomePlainSocketFactory::IsSecure(
    /* [in] */ ISocket* sock,
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = FALSE;

    if (sock == NULL) {
        Logger::E("MultihomePlainSocketFactory", "Socket may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // This class check assumes that createSocket() calls the constructor
    // directly. If it was using javax.net.SocketFactory, we couldn't make
    // an assumption about the socket class here.
    ClassID clsid;
    if (IObject::Probe(sock)->GetClassID(&clsid), clsid != ECLSID_CSocket) {
        Logger::E("MultihomePlainSocketFactory", "Socket not created by this factory.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // This check is performed last since it calls a method implemented
    // by the argument object. getClass() is final in java.lang.Object.
    Boolean isClosed;
    if (sock->IsClosed(&isClosed), isClosed) {
        Logger::E("MultihomePlainSocketFactory", "Socket is closed.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode MultihomePlainSocketFactory::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (other == this->Probe(EIID_IInterface));
    return NOERROR;
}

ECode MultihomePlainSocketFactory::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    ClassID clsid;
    ((IObject*)this->Probe(EIID_IObject))->GetClassID(&clsid);
    *hashCode = clsid.mClsid.mData1;
    return NOERROR;
}

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org