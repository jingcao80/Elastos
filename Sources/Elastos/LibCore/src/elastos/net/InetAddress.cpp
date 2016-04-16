
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "InetAddress.h"
#include "CInet4Address.h"
#include "CInet6Address.h"
#include "IoBridge.h"
#include "BlockGuard.h"
#include "NetworkUtilities.h"
#include "CStructAddrinfo.h"
#include "CLibcore.h"
#include "Arrays.h"
#include "Collections.h"
#include "CCountDownLatch.h"
#include "CAtomicBoolean.h"
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

using Droid::System::IStructAddrinfo;
using Droid::System::CStructAddrinfo;
using Elastos::Droid::System::IStructUtsname;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IoBridge;
using Elastos::IO::EIID_ISerializable;
using Elastos::Core::BlockGuard;
using Elastos::Core::IBlockGuardPolicy;
using Elastos::Core::IThread;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::Arrays;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Collections;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;

namespace Elastos {
namespace Net {

const Int32 InetAddress::NETID_UNSET = 0;
AutoPtr<AddressCache> InetAddress::ADDRESS_CACHE = new AddressCache();
AutoPtr<IInetAddress> InetAddress::UNSPECIFIED = new InetAddress(AF_UNSPEC, NULL, String(NULL));

CAR_INTERFACE_IMPL_2(InetAddress, Object, IInetAddress, ISerializable)

InetAddress::InetAddress()
    : mFamily(0)
{}

InetAddress::InetAddress(
    /* [in] */ Int32 family,
    /* [in] */ ArrayOf<Byte>* ipaddress,
    /* [in] */ const String& hostname)
    : mHostname(hostname)
    , mFamily(family)
    , mIpAddress(ipaddress)
{}

ECode InetAddress::constructor(
    /* [in] */ Int32 family,
    /* [in] */ ArrayOf<Byte>* ipaddress,
    /* [in] */ const String& hostname)
{
    mHostname = hostname;
    mFamily = family;
    mIpAddress = ipaddress;
    return NOERROR;
}

ECode InetAddress::GetAddress(
    /* [out, callee] */ ArrayOf<Byte>** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    AutoPtr<ArrayOf<Byte> > res;
    if (mIpAddress) {
        res = mIpAddress->Clone();
    }

    *address = res;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode InetAddress::GetAllByName(
    /* [in] */ const String& host,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    return GetAllByNameOnNet(host, NETID_UNSET, addresses);
}

ECode InetAddress::GetAllByNameOnNet(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    VALIDATE_NOT_NULL(addresses)
    *addresses = NULL;

    AutoPtr< ArrayOf<IInetAddress*> > addrs;
    FAIL_RETURN(GetAllByNameImpl(host, netId, (ArrayOf<IInetAddress*>**)&addrs));
    *addresses = addrs->Clone();
    REFCOUNT_ADD(*addresses);
    return NOERROR;
}

ECode InetAddress::GetAllByNameImpl(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    VALIDATE_NOT_NULL(addresses)
    *addresses = NULL;

    if (host.IsNullOrEmpty()) {
        return LoopbackAddresses(addresses);
    }

    //Is it a numeric address?
    AutoPtr<IInetAddress> result = ParseNumericAddressNoThrow(host);
    if (result != NULL) {
        result = DisallowDeprecatedFormats(host, result);
        if (result == NULL) {
            return E_UNKNOWN_HOST_EXCEPTION;
        }
        AutoPtr< ArrayOf<IInetAddress*> > addrs = ArrayOf<IInetAddress*>::Alloc(1);
        addrs->Set(0, result);
        *addresses = addrs;
        REFCOUNT_ADD(*addresses);
        return NOERROR;
    }

    AutoPtr< ArrayOf<IInetAddress*> > addrs;
    FAIL_RETURN(LookupHostByName(host, netId, (ArrayOf<IInetAddress*>**)&addrs));
    AutoPtr<ArrayOf<IInetAddress*> > res;
    if (addrs != NULL) {
        res = addrs->Clone();
    }

    *addresses = res;
    REFCOUNT_ADD(*addresses);
    return NOERROR;
}

ECode InetAddress::GetByName(
    /* [in] */ const String& host,
    /* [out] */ IInetAddress** address)
{
    return GetByNameOnNet(host, NETID_UNSET, address);
}

ECode InetAddress::GetByNameOnNet(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    AutoPtr< ArrayOf<IInetAddress*> > addresses;
    FAIL_RETURN(GetAllByNameImpl(host, netId, (ArrayOf<IInetAddress*>**)&addresses));
    *address = (*addresses)[0];
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode InetAddress::GetHostAddress(
    /* [out] */ String* address)
{
    return CLibcore::sOs->Getnameinfo(this, NI_NUMERICHOST, address);
}

ECode InetAddress::GetHostName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if(mHostname.IsNull()) {
        AutoPtr<IInetAddress> addr;
        if(SUCCEEDED(GetHostByAddrImpl(this, (IInetAddress**)&addr))) {
            InetAddress* inetAddress = (InetAddress*)(addr.Get());
            mHostname = inetAddress->mHostname;
        }
        else{
            GetHostAddress(&mHostname);
        }
    }
    *name = mHostname;
    return NOERROR;
}

ECode InetAddress::GetCanonicalHostName(
    /* [out] */ String* canonicalName)
{
    VALIDATE_NOT_NULL(canonicalName);

    AutoPtr<IInetAddress> addr;
    if(SUCCEEDED(GetHostByAddrImpl(this, (IInetAddress**)&addr))) {
        InetAddress* inetAddress = this;
        *canonicalName = inetAddress->mHostname;
    }
    else{
        GetHostAddress(canonicalName);
    }
    return NOERROR;
}

ECode InetAddress::GetFamily(
    /* [out] */ Int32* family)
{
    VALIDATE_NOT_NULL(family);
    *family = mFamily;
    return NOERROR;
}

ECode InetAddress::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = Arrays::GetHashCode(mIpAddress);
    return NOERROR;
}

ECode InetAddress::GetLocalHost(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    AutoPtr<IStructUtsname> isuname;
    CLibcore::sOs->Uname((IStructUtsname**)&isuname);
    String host;
    isuname->GetNodename(&host);
    AutoPtr<ArrayOf<IInetAddress*> > addresses;
    FAIL_RETURN(LookupHostByName(host, NETID_UNSET, (ArrayOf<IInetAddress*>**)&addresses));
    *address = (*addresses)[0];
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode InetAddress::ClearDnsCache()
{
    ADDRESS_CACHE->Clear();
    return NOERROR;
}

/**
 * Gets the hashcode of the represented IP address.
 *
 * @return the appropriate hashcode value.
 */
ECode InetAddress::IsMulticastAddress(
    /* [out] */ Boolean* isMulticastAddress)
{
    VALIDATE_NOT_NULL(isMulticastAddress);

    *isMulticastAddress = FALSE;
    return NOERROR;
}

ECode InetAddress::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String hostAddress;
    GetHostAddress(&hostAddress);
    if (!mHostname.IsNull()) {
        *result = mHostname + String("/") + hostAddress;
    }
    else {
        *result = String("/") +hostAddress;
    }
    return NOERROR;
}

ECode InetAddress::IsNumeric(
    /* [in] */ const String& address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInetAddress> inetAddress = ParseNumericAddressNoThrow(address);
    AutoPtr<IInetAddress> formats = DisallowDeprecatedFormats(address, inetAddress);
    *result = (inetAddress != NULL) && formats != NULL;
    return NOERROR;
}

ECode InetAddress::ParseNumericAddress(
    /* [in] */ const String& numericAddress,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);

    if (numericAddress.IsNullOrEmpty()) {
        *result = CInet6Address::LOOPBACK;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    AutoPtr<IInetAddress> resultTemp = ParseNumericAddressNoThrow(numericAddress);
    resultTemp = DisallowDeprecatedFormats(numericAddress, resultTemp);
    if (resultTemp == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = resultTemp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InetAddress::GetLoopbackAddress(
   /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    *address = CInet6Address::LOOPBACK;
    REFCOUNT_ADD(*address)
    return NOERROR;
}

ECode InetAddress::IsLoopbackAddress(
    /* [out] */ Boolean* isLoopbackAddress)
{
    VALIDATE_NOT_NULL(isLoopbackAddress);

    *isLoopbackAddress = FALSE;
    return NOERROR;
}

ECode InetAddress::IsLinkLocalAddress(
    /* [out] */ Boolean* isLinkLocalAddress)
{
    VALIDATE_NOT_NULL(isLinkLocalAddress);

    *isLinkLocalAddress = FALSE;
    return NOERROR;
}

ECode InetAddress::IsSiteLocalAddress(
    /* [out] */ Boolean* isSiteLocalAddress)
{
    VALIDATE_NOT_NULL(isSiteLocalAddress);

    *isSiteLocalAddress = FALSE;
    return NOERROR;
}

ECode InetAddress::IsMCGlobal(
    /* [out] */ Boolean* isMCGlobal)
{
    VALIDATE_NOT_NULL(isMCGlobal);

    *isMCGlobal = FALSE;
    return NOERROR;
}

ECode InetAddress::IsMCNodeLocal(
    /* [out] */ Boolean* isMCNodeLocal)
{
    VALIDATE_NOT_NULL(isMCNodeLocal);

    *isMCNodeLocal = FALSE;
    return NOERROR;
}

ECode InetAddress::IsMCLinkLocal(
    /* [out] */ Boolean* isMCLinkLocal)
{
    VALIDATE_NOT_NULL(isMCLinkLocal);

    *isMCLinkLocal = FALSE;
    return NOERROR;
}

ECode InetAddress::IsMCSiteLocal(
    /* [out] */ Boolean* isMCSiteLocal)
{
    VALIDATE_NOT_NULL(isMCSiteLocal);

    *isMCSiteLocal = FALSE;
    return NOERROR;
}

ECode InetAddress::IsMCOrgLocal(
    /* [out] */ Boolean* isMCOrgLocal)
{
    VALIDATE_NOT_NULL(isMCOrgLocal);

    *isMCOrgLocal = FALSE;
    return NOERROR;
}

ECode InetAddress::IsAnyLocalAddress(
    /* [out] */ Boolean* isAnyLocalAddress)
{
    VALIDATE_NOT_NULL(isAnyLocalAddress);

    *isAnyLocalAddress = FALSE;
    return NOERROR;
}

ECode InetAddress::IsReachable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isReachable)
{
    VALIDATE_NOT_NULL(isReachable);

    return IsReachable(NULL, 0, timeout, isReachable);
}

ECode InetAddress::IsReachable(
    /* [in] */ INetworkInterface* networkInterface,
    /* [in] */ Int32 ttl,
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isReachable)
{
    VALIDATE_NOT_NULL(isReachable);
    *isReachable = FALSE;

    if (ttl < 0 || timeout < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //The simple case.
    if (networkInterface == NULL) {
        *isReachable = IsReachable(this, NULL, timeout);
        return NOERROR;
    }

    //Try each NetworkInterface in parallel.
    //Use a thread pool executor?
    AutoPtr<IArrayList> sourceAddresses;
    AutoPtr<IIterator> em;
    AutoPtr<IEnumeration> outenum;
    networkInterface->GetInetAddresses((IEnumeration**)&outenum);
    Collections::List(outenum, (IArrayList**)&sourceAddresses);
    Int32 count;
    sourceAddresses->GetSize(&count);
    if (count == 0) {
        *isReachable = FALSE;
        return NOERROR;
    }
    AutoPtr<ICountDownLatch> latch;
    CCountDownLatch::New(count, (ICountDownLatch**)&latch);
    AutoPtr<IAtomicBoolean> _isReachable;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&_isReachable);
    sourceAddresses->GetIterator((IIterator**)&em);
    Boolean hasNext = FALSE;
    // TODO: find all inetAddress instance to check is the isReachabel should be TRUE;
    while (em->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> outface;
        em->GetNext((IInterface**)&outface);
        AutoPtr<IInetAddress> sourceAddress = IInetAddress::Probe(outface);
        AutoPtr<InetAddressThread> newthread = new InetAddressThread(
            this, sourceAddress, timeout, latch, _isReachable);
        newthread->constructor();
        newthread->Start();
    }
    // try {
    ECode ec = latch->Await();
    // } catch (InterruptedException ignored) {
    if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
       AutoPtr<IThread> onthread = Thread::GetCurrentThread();
       onthread->Interrupt(); // Leave the interrupted bit set.
    }
    return _isReachable->Get(isReachable);
}

ECode InetAddress::GetByAddress(
    /* [in] */ ArrayOf<Byte>* ipAddress,
    /* [out] */ IInetAddress** address)
{
    return GetByAddress(String(NULL), ipAddress, 0, address);
}

ECode InetAddress::GetByAddress(
    /* [in] */ const String& hostname,
    /* [in] */ ArrayOf<Byte>* ipAddress,
    /* [out] */ IInetAddress** address)
{
    return GetByAddress(hostname, ipAddress, 0, address);
}

ECode InetAddress::GetUNSPECIFIED(
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr);

    *addr = UNSPECIFIED;
    REFCOUNT_ADD(*addr);
    return NOERROR;
}

ECode InetAddress::BytesToInetAddresses(
    /* [in] */ ArrayOf<ByteArray>* rawAddresses,
    /* [in] */ const String& hostname,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    VALIDATE_NOT_NULL(addresses)

    AutoPtr<ArrayOf<IInetAddress*> > returnedAddresses =
            ArrayOf<IInetAddress*>::Alloc(rawAddresses->GetLength());
    for(Int32 i=0; i < rawAddresses->GetLength(); i++) {
        AutoPtr<IInetAddress> addr;
        MakeInetAddress((*rawAddresses)[i], hostname, (IInetAddress**)&addr);
        returnedAddresses->Set(i, addr);
    }
    *addresses = returnedAddresses;
    REFCOUNT_ADD(*addresses);
    return NOERROR;
}

ECode InetAddress::GetHostByAddrImpl(
    /* [in] */ InetAddress* address,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    policy->OnNetwork();

    String hostname;
    FAIL_RETURN(CLibcore::sOs->Getnameinfo((IInetAddress*) address->Probe(EIID_IInetAddress), NI_NAMEREQD, &hostname));
    AutoPtr<ArrayOf<Byte> > addressClone = address->mIpAddress->Clone();
    if(FAILED(MakeInetAddress(addressClone, hostname, result))) {
        return E_UNKNOWN_HOST_EXCEPTION;
    }
    return NOERROR;
}

ECode InetAddress::GetByAddress(
    /* [in] */ const String& hostname,
    /* [in] */ ArrayOf<Byte>* ipaddress,
    /* [in] */ Int32 scopeId,
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    if(ipaddress == NULL) {
        return E_UNKNOWN_HOST_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > addressClone;
    if(ipaddress->GetLength() == 4) {
        AutoPtr<IInet4Address> inet4addr;
        addressClone = ipaddress->Clone();
        FAIL_RETURN(CInet4Address::New(addressClone, hostname, (IInet4Address**)&inet4addr));
        *address = IInetAddress::Probe(inet4addr);
        REFCOUNT_ADD(*address);
        return NOERROR;
    }
    else if(ipaddress->GetLength() == 16){
        if(IsIPv4MappedAddress(ipaddress)) {
            AutoPtr<IInet4Address> inet4addr;
            IPv4MappedToIPv4(ipaddress, (ArrayOf<Byte>**)&addressClone);
            FAIL_RETURN(CInet4Address::New(addressClone, hostname, (IInet4Address**)&inet4addr));
            *address = IInetAddress::Probe(inet4addr);
            REFCOUNT_ADD(*address);
            return NOERROR;
        }
        else{
            AutoPtr<IInet6Address> inet6addr;
            addressClone = ipaddress->Clone();
            FAIL_RETURN(CInet6Address::New(addressClone, hostname, scopeId, (IInet6Address**)&inet6addr));
            *address = IInetAddress::Probe(inet6addr);
            REFCOUNT_ADD(*address);
            return NOERROR;
        }
    }

    return E_UNKNOWN_HOST_EXCEPTION;
}

Int32 InetAddress::BytesToInt32(
    /* [in] */ const ArrayOf<Byte>& bytes,
    /* [in] */ Int32 start)
{
    return ((bytes[start] & 0xff) << 24) |
            ((bytes[start + 1] & 0xff) << 16) |
            ((bytes[start + 2] & 0xff) << 8) |
            ((bytes[start + 3] & 0xff) << 0) ;
}

ECode InetAddress::LookupHostByName(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    VALIDATE_NOT_NULL(addresses);
    *addresses = NULL;

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    policy->OnNetwork();

    // Do we have a result cached?
    AutoPtr<AddressCacheEntry> cachedResult = ADDRESS_CACHE->Get(host, netId);
    if (cachedResult != NULL) {
        if (cachedResult->mValue != NULL) {
            *addresses = cachedResult->mValue;
            REFCOUNT_ADD(*addresses);
            return NOERROR;
        }
        else {
            // A cached negative result.
            // throw new UnknownHostException((String) cachedResult);
            return E_UNKNOWN_HOST_EXCEPTION;
        }
    }

    AutoPtr<ArrayOf<IInetAddress*> > structAddresses;
    AutoPtr<IStructAddrinfo> hints;
    ECode ec = CStructAddrinfo::New((IStructAddrinfo**)&hints);
    FAIL_GOTO(ec, _EXIT_)

    hints->SetFlags(AI_ADDRCONFIG);
    hints->SetFamily(AF_UNSPEC);
    // If we don't specify a socket type, every address will appear twice, once
    // for SOCK_STREAM and one for SOCK_DGRAM. Since we do not return the family
    // anyway, just pick one.
    hints->SetSocktype(SOCK_STREAM);

    ec = CLibcore::sOs->Elastos_getaddrinfo(host, hints, netId, (ArrayOf<IInetAddress*>**)&structAddresses);
    FAIL_GOTO(ec, _EXIT_)

    if (structAddresses != NULL) {
        // TODO: should getaddrinfo set the hostname of the InetAddresses it returns?
        for (Int32 i = 0; i < structAddresses->GetLength(); i++) {
            InetAddress* inetAddressObj = (InetAddress*)(*structAddresses)[i];
            inetAddressObj->mHostname = host;
        }
    }

    ADDRESS_CACHE->Put(host, netId, structAddresses);
    *addresses = structAddresses;
    REFCOUNT_ADD(*addresses);
    return NOERROR;

_EXIT_:
    if ((ECode)E_GAI_EXCEPTION == ec) {
        ADDRESS_CACHE->PutUnknownHost(host, netId, String("Unable to resolve") + host);
        ec = E_UNKNOWN_HOST_EXCEPTION;
    }
    return ec;
}

Boolean InetAddress::IsReachable(
    /* [in] */ InetAddress* destination,
    /* [in] */ IInetAddress* source,
    /* [in] */ Int32 timeout)
{
    AutoPtr<IFileDescriptor> fd;
    IoBridge::Socket(TRUE, (IFileDescriptor**)&fd);
    if (source != NULL) {
        if (FAILED(IoBridge::Bind(fd, source, 0))) {
            return FALSE;
        }
    }

    if (FAILED(IoBridge::Connect(fd, (IInetAddress*)destination->Probe(EIID_IInetAddress), 7, timeout))) {
        return FALSE;
    }

    IoBridge::CloseAndSignalBlockedThreads(fd);

    return TRUE;
}

Boolean InetAddress::IsIPv4MappedAddress(
    /* [in] */ ArrayOf<Byte>* ipAddress)
{
    if(ipAddress == NULL || ipAddress->GetLength() != 16) {
        return FALSE;
    }
    for(Int32 i = 0; i < 10; i++) {
        if((*ipAddress)[i] != 0) {
            return FALSE;
        }
    }
    if((*ipAddress)[10] != -1 || (*ipAddress)[11] != -1) {
        return FALSE;
    }
    return TRUE;
}

ECode InetAddress::IPv4MappedToIPv4(
    /* [in] */ ArrayOf<Byte>* mappedAddress,
    /* [out, callee] */ ArrayOf<Byte>** ipv4Address)
{
    VALIDATE_NOT_NULL(ipv4Address);

    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(4);
    for(Int32 i = 0; i < 4; i++) {
        (*array)[i] = (*mappedAddress)[12 + i];
    }

    *ipv4Address = array;
    REFCOUNT_ADD(*ipv4Address)
    return NOERROR;
}

//    private void writeObject(ObjectOutputStream stream)

//    private void readObject(ObjectInputStream stream)

ECode InetAddress::MakeInetAddress(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ const String& hostname,
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;
    VALIDATE_NOT_NULL(bytes)

    if (bytes->GetLength() == 4) {
        AutoPtr<IInet4Address> inet4addr;
        FAIL_RETURN(CInet4Address::New(bytes, hostname, (IInet4Address**)&inet4addr));
        *address = IInetAddress::Probe(inet4addr);
        REFCOUNT_ADD(*address);
        return NOERROR;
    }
    else if(bytes->GetLength() == 16) {
        AutoPtr<IInet6Address> inet6addr;
        FAIL_RETURN(CInet6Address::New(bytes, hostname, 0, (IInet6Address**)&inet6addr));
        *address = IInetAddress::Probe(inet6addr);
        REFCOUNT_ADD(*address);
        return NOERROR;
    }

    return E_UNKNOWN_HOST_EXCEPTION;
}

AutoPtr<IInetAddress> InetAddress::DisallowDeprecatedFormats(
    /* [in] */ const String& address,
    /* [in] */ IInetAddress* inetAddress)
{
    if (IInet4Address::Probe(inetAddress) != NULL ||
            address.IndexOf(':') != -1) {
        return inetAddress;
    }
    // If inet_pton(3) can't parse it, it must have been a deprecated format.
    // We need to return inet_pton(3)'s result to ensure that numbers assumed to be octal
    // by getaddrinfo(3) are reinterpreted by inet_pton(3) as decimal.
    AutoPtr<IInetAddress> isia;
    CLibcore::sOs->Inet_pton(AF_INET, address, (IInetAddress**)&isia);
    return isia;
}

AutoPtr<IInetAddress> InetAddress::ParseNumericAddressNoThrow(
    /* [in] */ const String& _address)
{
    String address = _address;
    if (address.StartWith("[") && address.EndWith("]") && address.IndexOf(':') != -1) {
       address = address.Substring(1, address.GetLength() - 1);
    }
    AutoPtr<IStructAddrinfo> hints;
    CStructAddrinfo::New((IStructAddrinfo**)&hints);
    hints->SetFlags(AI_NUMERICHOST);
    AutoPtr< ArrayOf<IInetAddress*> > info;
    CLibcore::sOs->Elastos_getaddrinfo(address, hints, NETID_UNSET, (ArrayOf<IInetAddress*>**)&info);
    if (info == NULL)
        return NULL;

    AutoPtr<IInetAddress> result = (*info)[0];
    return result;
}

ECode InetAddress::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IInetAddress::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    InetAddress* inetAddress = (InetAddress*)IInetAddress::Probe(obj);
    if (inetAddress == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > objIpaddress = inetAddress->mIpAddress;
    if (mIpAddress->GetLength() != objIpaddress->GetLength()) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    for (Int32 i = 0; i < mIpAddress->GetLength(); i++) {
        if((*objIpaddress)[i] != (*mIpAddress)[i]) {
            *result = FALSE;
            break;
        }
    }
    return NOERROR;
}

ECode InetAddress::LoopbackAddresses(
    /* [out] */ ArrayOf<IInetAddress*>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr< ArrayOf<IInetAddress*> > addrs = ArrayOf<IInetAddress*>::Alloc(2);
    addrs->Set(0, CInet6Address::LOOPBACK);
    addrs->Set(1, CInet4Address::LOOPBACK);
    *result = addrs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//==========================================================
//       InetAddress::InetAddressThread
//==========================================================
InetAddress::InetAddressThread::InetAddressThread(
    /* [in] */ InetAddress* host,
    /* [in] */ IInetAddress* sourceAddress,
    /* [in] */ Int32 timeout,
    /* [in] */ ICountDownLatch* latch,
    /* [in] */ IAtomicBoolean* isReachable)
    : mHost(host)
    , mSourceAddress(sourceAddress)
    , mLatch(latch)
    , mIsReachable(isReachable)
    , mTimeout(timeout)
{
}

ECode InetAddress::InetAddressThread::Run()
{
    // try {
    if (mHost->IsReachable(mHost, mSourceAddress, mTimeout)) {
         mIsReachable->Set(TRUE);
        // Wake the main thread so it can return success without
        // waiting for any other threads to time out.
        Int64 latchount = 0;
        while ((mLatch->GetCount(&latchount), latchount) > 0) {
            mLatch->CountDown();
        }
    }
    // } catch (IOException ignored) {
    // }
    mLatch->CountDown();
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
