#ifndef __ELASTOS_NET_INETADDRESS_H__
#define __ELASTOS_NET_INETADDRESS_H__

#include "Elastos.CoreLibrary.Net.h"
#include "AddressCache.h"
#include "elastos/core/Thread.h"

using Elastos::Core::Thread;
using Libcore::IO::IOs;
using Elastos::IO::ISerializable;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Net {

class InetAddress
    : public Object
    , public IInetAddress
    , public ISerializable
{
protected:
    class InetAddressThread
        : public Thread
    {
    public:
        InetAddressThread(
            /* [in] */ InetAddress* host,
            /* [in] */ IInetAddress* sourceAddress,
            /* [in] */ Int32 timeout,
            /* [in] */ ICountDownLatch* latch,
            /* [in] */ IAtomicBoolean* isReachable);

        CARAPI Run();

    private:
        InetAddress* mHost;
        AutoPtr<IInetAddress> mSourceAddress;
        AutoPtr<ICountDownLatch> mLatch;
        AutoPtr<IAtomicBoolean> mIsReachable;
        Int32 mTimeout;
    };

public:
    CAR_INTERFACE_DECL()

    InetAddress(
        /* [in] */ Int32 family,
        /* [in] */ ArrayOf<Byte>* ipaddress,
        /* [in] */ const String& hostname);

    CARAPI constructor(
        /* [in] */ Int32 family,
        /* [in] */ ArrayOf<Byte>* ipaddress,
        /* [in] */ const String& hostname);

    CARAPI Equals(
        /* [in] */IInterface* obj,
        /* [out] */Boolean* result);

    CARAPI GetAddress(
        /* [out, callee] */ ArrayOf<Byte>** address);

    static CARAPI GetAllByName(
        /* [in] */ const String& host,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    static CARAPI GetAllByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    static CARAPI GetByName(
        /* [in] */ const String& host,
        /* [out] */ IInetAddress** address);

    static CARAPI GetByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out] */ IInetAddress** address);

    CARAPI GetHostAddress(
        /* [out] */ String* address);

    CARAPI GetHostName(
        /* [out] */ String* name);

    CARAPI GetCanonicalHostName(
        /* [out] */ String* canonicalName);

    CARAPI GetFamily(
        /* [out] */ Int32* family);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    static CARAPI GetLocalHost(
        /* [out] */ IInetAddress** address);

    static CARAPI ClearDnsCache();

    CARAPI IsMulticastAddress(
        /* [out] */ Boolean* isMulticastAddress);

    CARAPI ToString(
        /* [out] */String* result);

    static CARAPI IsNumeric(
        /* [in] */ const String& address,
        /* [out] */ Boolean* result);

    static CARAPI ParseNumericAddress(
        /* [in] */ const String& numericAddress,
        /* [out] */ IInetAddress** result);

    static CARAPI GetLoopbackAddress(
        /* [out] */ IInetAddress** address);

    CARAPI IsLoopbackAddress(
        /* [out] */ Boolean* isLoopbackAddress);

    CARAPI IsLinkLocalAddress(
        /* [out] */ Boolean* isLinkLocalAddress);

    CARAPI IsSiteLocalAddress(
        /* [out] */ Boolean* isSiteLocalAddress);

    CARAPI IsMCGlobal(
        /* [out] */ Boolean* isMCGlobal);

    CARAPI IsMCNodeLocal(
        /* [out] */ Boolean* isMCNodeLocal);

    CARAPI IsMCLinkLocal(
        /* [out] */ Boolean* isMCLinkLocal);

    CARAPI IsMCSiteLocal(
        /* [out] */ Boolean* isMCSiteLocal);

    CARAPI IsMCOrgLocal(
        /* [out] */ Boolean* isMCOrgLocal);

    CARAPI IsAnyLocalAddress(
        /* [out] */ Boolean* isAnyLocalAddress);

    CARAPI IsReachable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isReachable);

    CARAPI IsReachable(
        /* [in] */ INetworkInterface* networkInterface,
        /* [in] */ Int32 ttl,
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isReachable);

    static CARAPI GetByAddress(
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** address);

    static CARAPI GetByAddress(
        /* [in] */ const String& hostname,
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** address);

    static CARAPI GetByAddress(
        /* [in] */ const String& hostname,
        /* [in] */ ArrayOf<Byte>* ipaddress,
        /* [in] */ Int32 scopeId,
        /* [out] */ IInetAddress** address);

    static CARAPI GetUNSPECIFIED(
        /* [out] */ IInetAddress** addr);

protected:
    InetAddress();

    static CARAPI BytesToInetAddresses(
        /* [in] */ ArrayOf<ByteArray>* rawAddresses,
        /* [in] */ const String& hostname,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    static CARAPI GetHostByAddrImpl(
        /* [in] */ InetAddress* address,
        /* [out] */ IInetAddress** result);

    static CARAPI_(Int32) BytesToInt32(
        /* [in] */ const ArrayOf<Byte>& bytes,
        /* [in] */ Int32 start);

private:
    static CARAPI GetAllByNameImpl(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    static CARAPI LookupHostByName(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    CARAPI_(Boolean) IsReachable(
        /* [in] */ InetAddress* destination,
        /* [in] */ IInetAddress* source,
        /* [in] */ Int32 timeout);

    static CARAPI_(Boolean) IsIPv4MappedAddress(
        /* [in] */ ArrayOf<Byte>* ipAddress);

    static CARAPI IPv4MappedToIPv4(
        /* [in] */ ArrayOf<Byte>* mappedAddress,
        /* [out, callee] */ ArrayOf<Byte>** ipv4Address);

//    private void writeObject(ObjectOutputStream stream);

//    private void readObject(ObjectInputStream stream);

    static CARAPI MakeInetAddress(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ const String& hostname,
        /* [out] */ IInetAddress** address);

    static CARAPI_(AutoPtr<IInetAddress>) DisallowDeprecatedFormats(
        /* [in] */ const String& address,
        /* [in] */ IInetAddress* inetAddress);

    static CARAPI_(AutoPtr<IInetAddress>) ParseNumericAddressNoThrow(
        /* [in] */ const String& address);

    static CARAPI LoopbackAddresses(
        /* [out] */ ArrayOf<IInetAddress*>** result);

public:
    String mHostname;
    Int32 mFamily;
    AutoPtr<ArrayOf<Byte> > mIpAddress;

    static AutoPtr<IInetAddress> UNSPECIFIED;

    /** Using NetID of NETID_UNSET indicates resolution should be done on default network. */
    static const Int32 NETID_UNSET;// = 0;

private:
    /** Our Java-side DNS cache. */
    static AutoPtr<AddressCache> ADDRESS_CACHE;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_INETADDRESS_H__
