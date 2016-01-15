
#ifndef __ELASTOS_NET_CINETSOCKETADDRESS_H__
#define __ELASTOS_NET_CINETSOCKETADDRESS_H__

#include "_Elastos_Net_CInetSocketAddress.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IInetSocketAddress;

namespace Elastos {
namespace Net {

CarClass(CInetSocketAddress)
    , public Object
    , public IInetSocketAddress
    , public ISocketAddress
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ const String& hostname,
        /* [in] */ Int32 port,
        /* [in] */ Boolean needResolved);

    static CARAPI CreateUnresolved(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ IInetSocketAddress** addr);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI SetPort(
        /* [in] */ Int32 port);

    CARAPI GetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI SetAddress(
        /* [in] */ IInetAddress* address);

    CARAPI GetHostName(
        /* [out] */ String* hostname);

    CARAPI GetHostString(
        /* [out] */ String* hostString);

    CARAPI IsUnresolved(
        /* [out] */ Boolean* isUnresolved);

    CARAPI Equals(
        /*[in] */ IInterface* socketAddr,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);


public:
    AutoPtr<IInetAddress> mAddr;

    Int32 mPort;

private:
    static const Int64 sSerialVersionUID;

    String mHostname;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINETSOCKETADDRESS_H__
