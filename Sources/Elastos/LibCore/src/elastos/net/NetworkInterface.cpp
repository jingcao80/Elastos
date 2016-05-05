
#include "NetworkInterface.h"
#include "CFile.h"
#include "Collections.h"
#include "IoUtils.h"
#include "CLibcore.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "CInet4Address.h"
#include "CInet6Address.h"
#include "InterfaceAddress.h"
#include "CArrayList.h"
#include "Arrays.h"
#include <net/if.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Libcore::IO::IoUtils;
using Libcore::IO::IOs;
using Libcore::IO::CLibcore;
using Elastos::Utility::IIterable;
using Elastos::Utility::Collections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Net {

static AutoPtr<IFile> InitSYS_CLASS_NET()
{
    AutoPtr<IFile> file;
    CFile::New(String("/sys/class/net"), (IFile**)&file);
    return file;
}

const AutoPtr<IFile> NetworkInterface::SYS_CLASS_NET = InitSYS_CLASS_NET();

CAR_INTERFACE_IMPL(NetworkInterface, Object, INetworkInterface)

NetworkInterface::NetworkInterface()
    : mInterfaceIndex(-1)
{
}

NetworkInterface::NetworkInterface(
    /* [in] */ const String& name,
    /* [in] */ Int32 interfaceIndex,
    /* [in] */ IList * addresses,
    /* [in] */ IList * interfaceAddresses)
    : mName(name)
    , mInterfaceIndex(interfaceIndex)
    , mInterfaceAddresses(interfaceAddresses)
    , mAddresses(addresses)
{
}

NetworkInterface::~NetworkInterface()
{
    mAddresses = NULL;
    mInterfaceAddresses = NULL;
    mChildren = NULL;
}

ECode NetworkInterface::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 interfaceIndex,
    /* [in] */ IList * addresses,
    /* [in] */ IList * interfaceAddresses)
{
    mName = name;
    mInterfaceIndex = interfaceIndex;
    mAddresses = addresses;
    mInterfaceAddresses = interfaceAddresses;
    return NOERROR;
}

AutoPtr<INetworkInterface> NetworkInterface::ForUnboundMulticastSocket()
{
    // This is what the RI returns for a MulticastSocket that hasn't been constrained
    // to a specific interface.
    AutoPtr< ArrayOf<IInterface*> > outarr = ArrayOf<IInterface*>::Alloc(1);
    outarr->Set(0, CInet6Address::ANY);
    AutoPtr<IList> outlist1;
    Arrays::AsList(outarr, (IList**)&outlist1);
    AutoPtr<IList> outlist2 = Collections::EMPTY_LIST;
    AutoPtr<INetworkInterface> ni = new NetworkInterface(String(NULL), -1, outlist1, outlist2);
    return ni;
}

ECode NetworkInterface::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mInterfaceIndex;
    return NOERROR;
}

ECode NetworkInterface::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode NetworkInterface::GetInetAddresses(
    /* [out] */ IEnumeration** addresses)
{
    return Collections::Enumeration(ICollection::Probe(mAddresses), addresses);
}

ECode NetworkInterface::GetDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode NetworkInterface::GetByName(
    /* [in] */  const String& interfaceName,
    /* [out] */ INetworkInterface** networkInterface)
{
    VALIDATE_NOT_NULL(networkInterface);
    *networkInterface = NULL;

    if (interfaceName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!IsValidInterfaceName(interfaceName)) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > lines;
    FAIL_RETURN(ReadIfInet6Lines((ArrayOf<String> **)&lines))
    return GetByNameInternal(interfaceName, lines, networkInterface);
}

ECode NetworkInterface::GetByNameInternal(
    /* [in] */ const String& interfaceName,
    /* [in] */ ArrayOf<String> * ifInet6Lines,
    /* [out] */ INetworkInterface** networkInterface)
{
    StringBuilder sb("/sys/class/net/");
    sb.Append(interfaceName);
    sb.Append("/ifindex");
    Int32 interfaceIndex;
    FAIL_RETURN(ReadIntFile(sb.ToString(), &interfaceIndex))

    AutoPtr<IList> addresses;
    AutoPtr<IList> interfaceAddresses;
    CArrayList::New((IList**)&addresses);
    CArrayList::New((IList**)&interfaceAddresses);
    CollectIpv6Addresses(interfaceName, interfaceIndex, addresses, interfaceAddresses, ifInet6Lines);
    CollectIpv4Addresses(interfaceName, addresses, interfaceAddresses);

    AutoPtr<INetworkInterface> ni = new NetworkInterface(
        interfaceName, interfaceIndex, addresses, interfaceAddresses);
    *networkInterface = ni;
    REFCOUNT_ADD(*networkInterface);
    return NOERROR;
}

ECode NetworkInterface::ReadIfInet6Lines(
    /* [out, callee] */ ArrayOf<String> ** lines)
{
    VALIDATE_NOT_NULL(lines)
    // try {
    String result;
    ECode ec = IoUtils::ReadFileAsString(String("/proc/net/if_inet6"), &result);
    if (ec == (ECode)E_IO_EXCEPTION) {
        return E_SOCKET_EXCEPTION;
    }
    return StringUtils::Split(result, String("\n"), lines);
    // } catch (IOException ioe) {
        // throw rethrowAsSocketException(ioe);
    // }
}

ECode NetworkInterface::CollectIpv6Addresses(
    /* [in] */ const String& interfaceName,
    /* [in] */ Int32 interfaceIndex,
    /* [in] */ IList* addresses,
    /* [in] */ IList* interfaceAddresses,
    /* [in] */ ArrayOf<String> * ifInet6Lines)
{
    // Format of /proc/net/if_inet6.
    // All numeric fields are implicit hex,
    // but not necessarily two-digit (http://code.google.com/p/android/issues/detail?id=34022).
    // 1. IPv6 address
    // 2. interface index
    // 3. prefix length
    // 4. scope
    // 5. flags
    // 6. interface name
    // "00000000000000000000000000000001 01 80 10 80       lo"
    // "fe800000000000000000000000000000 407 40 20 80    wlan0"
    String suffix = String(" ") + interfaceName;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < ifInet6Lines->GetLength(); ++i) {
        String line = (*ifInet6Lines)[i];
        if (!line.EndWith(suffix)) {
            continue;
        }

        // Extract the IPv6 address.
        AutoPtr<ArrayOf<Byte> > addressBytes = ArrayOf<Byte>::Alloc(16);
        for (Int32 i = 0; i < addressBytes->GetLength(); ++i) {
            Int32 value;
            ec = StringUtils::Parse(line.Substring(2*i, 2*i + 2), 16, &value);
            if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                return E_SOCKET_EXCEPTION;
            }
            (*addressBytes)[i] = (Byte)value;
        }

        // Extract the prefix length.
        // Skip the IPv6 address and its trailing space.
        Int32 prefixLengthStart = 32 + 1;
        // Skip the interface index and its trailing space.
        prefixLengthStart = line.IndexOf(' ', prefixLengthStart) + 1;
        Int32 prefixLengthEnd = line.IndexOf(' ', prefixLengthStart);
        Int16 prefixLength;
        ec = StringUtils::Parse(line.Substring(prefixLengthStart, prefixLengthEnd), 16, &prefixLength);
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            return E_SOCKET_EXCEPTION;
        }

        AutoPtr<IInetAddress> inet6Address;
        CInet6Address::New(addressBytes, String(NULL), interfaceIndex, (IInetAddress**)&inet6Address);
        addresses->Add(inet6Address);
        AutoPtr<InterfaceAddress> addres = new InterfaceAddress(inet6Address, prefixLength);
        interfaceAddresses->Add(TO_IINTERFACE(addres));
    }

    return ec;
}

ECode NetworkInterface::CollectIpv4Addresses(
    /* [in] */ const String& interfaceName,
    /* [in] */ IList* addresses,
    /* [in] */ IList* interfaceAddresses)
{
    AutoPtr<IFileDescriptor> fd;
    Boolean isEquals;
    AutoPtr<IInetAddress> address;
    AutoPtr<IInetAddress> broadcast;
    AutoPtr<IInetAddress> netmask;
    AutoPtr<InterfaceAddress> ia;
    AutoPtr<IInetAddress> any;
    AutoPtr<IInet4AddressHelper> inet4AddressHelper;

    ECode ec = NOERROR;
    ec = CLibcore::sOs->Socket(AF_INET, SOCK_DGRAM, 0, (IFileDescriptor**)&fd);
    FAIL_GOTO(ec, _EXIT_)

    ec = (CLibcore::sOs->IoctlInetAddress(fd, SIOCGIFADDR, interfaceName, (IInetAddress**)&address));
    FAIL_GOTO(ec, _EXIT_)
    ec = (CLibcore::sOs->IoctlInetAddress(fd, SIOCGIFBRDADDR, interfaceName, (IInetAddress**)&broadcast));
    FAIL_GOTO(ec, _EXIT_)
    ec = (CLibcore::sOs->IoctlInetAddress(fd, SIOCGIFNETMASK, interfaceName, (IInetAddress**)&netmask));
    FAIL_GOTO(ec, _EXIT_)

    isEquals = Object::Equals(broadcast, CInet4Address::ANY);
    if (isEquals) {
        broadcast = NULL;
    }

    addresses->Add(address);
    ia = new InterfaceAddress(address, broadcast, netmask);
    interfaceAddresses->Add(TO_IINTERFACE(ia));

_EXIT_:
    IoUtils::CloseQuietly(fd);

    if (FAILED(ec)) {
        return E_SOCKET_EXCEPTION;
    }
    return ec;
}

Boolean NetworkInterface::IsValidInterfaceName(
    /* [in] */ const String& interfaceName)
{
    AutoPtr<ArrayOf<String> > files;
    SYS_CLASS_NET->List((ArrayOf<String>**)&files);
    if (files == NULL) {
        return FALSE;
    }

    for(Int32 i = 0; i < files->GetLength(); ++i) {
        if((*files)[i].Equals(interfaceName))
            return TRUE;
    }
    return FALSE;
}

ECode NetworkInterface::ReadIntFile(
    /* [in] */ const String& path,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    String s;
    ECode ec = IoUtils::ReadFileAsString(path, &s);
    FAIL_GOTO(ec, _EXIT_)

    s= s.Trim();
    if(s.StartWith(String("0x"))) {
        FAIL_GOTO(StringUtils::Parse(s.Substring(2), 16, value), _EXIT_)
    }
    else {
        FAIL_GOTO(StringUtils::Parse(s, value), _EXIT_)
    }

_EXIT_:
    if (FAILED(ec)) {
        return E_SOCKET_EXCEPTION;
    }

    return ec;
}

ECode NetworkInterface::GetByInetAddress(
    /* [in] */ IInetAddress* address,
    /* [out] */ INetworkInterface** networkInterface)
{
    VALIDATE_NOT_NULL(networkInterface);
    *networkInterface = NULL;

    if (address == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IList> em = GetNetworkInterfacesList();
    AutoPtr<IIterator> it;
    em->GetIterator((IIterator**)&it);

    Boolean hasNext, found;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        NetworkInterface* net = (NetworkInterface*)INetworkInterface::Probe(obj);
        if (ICollection::Probe(net->mAddresses)->Contains(address, &found), found) {
            *networkInterface = net;
            REFCOUNT_ADD(*networkInterface);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode NetworkInterface::GetByIndex(
    /* [in] */ Int32 index,
    /* [out] */ INetworkInterface** networkInterface)
{
    VALIDATE_NOT_NULL(networkInterface);
    *networkInterface = NULL;

    String name;
    CLibcore::sOs->If_indextoname(index, &name);
    if (name.IsNull()) {
        return NOERROR;
    }

    return GetByName(name, networkInterface);
}

ECode NetworkInterface::GetNetworkInterfaces(
    /* [out] */ IEnumeration** enumerator)
{
    VALIDATE_NOT_NULL(enumerator);

    AutoPtr<IList> interfaces = GetNetworkInterfacesList();
    return Collections::Enumeration(ICollection::Probe(interfaces), enumerator);
}

AutoPtr<IList> NetworkInterface::GetNetworkInterfacesList()
{
    AutoPtr<ArrayOf<String> > interfaceNames;
    SYS_CLASS_NET->List((ArrayOf<String>**)&interfaceNames);

    AutoPtr<ArrayOf<INetworkInterface*> > interfaces =
        ArrayOf<INetworkInterface*>::Alloc(interfaceNames->GetLength());

    AutoPtr<ArrayOf<Boolean> > done =
        ArrayOf<Boolean>::Alloc(interfaceNames->GetLength());

    AutoPtr<ArrayOf<String> > ifInet6Lines;
    ReadIfInet6Lines((ArrayOf<String> **)&ifInet6Lines);

    for (Int32 i = 0; i < interfaceNames->GetLength(); ++i) {
        AutoPtr<INetworkInterface> temp;
        GetByNameInternal((*interfaceNames)[i], ifInet6Lines, (INetworkInterface**)&temp);
        interfaces->Set(i, temp);
        // http://b/5833739: getByName can return null if the interface went away between our
        // readdir(2) and our stat(2), so mark interfaces that disappeared as 'done'.
        if (temp == NULL)
            (*done)[i] = TRUE;
    }

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    for (Int32 counter1 = 0; counter1 < interfaces->GetLength(); counter1++) {
        // If this interface has been dealt with already, continue.
        if ((*done)[counter1]) {
            continue;
        }

        NetworkInterface* i1 =  (NetworkInterface*)(*interfaces)[counter1];
        // Checks whether the following interfaces are children.
        for (Int32 counter2 = counter1; counter2 < interfaces->GetLength(); counter2++) {
            if ((*done)[counter2]) {
                continue;
            }

            NetworkInterface* i2 =  (NetworkInterface*)(*interfaces)[counter2];
            if (i2->mName.StartWith(i1->mName + ":") ) {
                ICollection::Probe(i1->mChildren)->Add((*interfaces)[counter2]);
                i2->mParent = i1;
                ICollection::Probe(i1->mAddresses)->AddAll(ICollection::Probe(i2->mAddresses));
                (*done)[counter2] = TRUE;
            }
        }

        result->Add((*interfaces)[counter1]);
        (*done)[counter1] = TRUE;
    }
    return result;
}

ECode NetworkInterface::Equals(
    /* [in] */  IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    INetworkInterface* otherni = INetworkInterface::Probe(obj);
    if (otherni == NULL) {
        return NOERROR;
    }

    NetworkInterface* rhs = (NetworkInterface*)otherni;
    if (rhs == this) {
        *result = TRUE;
        return NOERROR;
    }

    *result = (rhs->mInterfaceIndex == mInterfaceIndex)
        && rhs->mName.Equals(mName)
        && Object::Equals(rhs->mAddresses, mAddresses);
    return NOERROR;
}

ECode NetworkInterface::GetHashCode(
    /*  [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mName.GetHashCode();
    return NOERROR;
}

ECode NetworkInterface::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb(25);
    sb.AppendChar('[');
    sb.Append(mName);
    sb.Append("][");
    sb.Append(mInterfaceIndex);
    sb.AppendChar(']');

    AutoPtr<IIterator> outiter;
    mAddresses->GetIterator((IIterator**)&outiter);
    Boolean hasNext;
    while (outiter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> address;
        outiter->GetNext((IInterface**)&address);
        sb.AppendChar(']');
        sb.Append(Object::ToString(address));
        sb.AppendChar(']');
    }
    *str = sb.ToString();
    return NOERROR;
}

ECode NetworkInterface::GetInterfaceAddresses(
    /* [out] */ IList** addresses)
{
    return Collections::UnmodifiableList(mInterfaceAddresses, addresses);
}

ECode NetworkInterface::GetSubInterfaces(
    /* [out] */ IEnumeration** subInterfaces)
{
    return Collections::Enumeration(ICollection::Probe(mChildren), subInterfaces);
}

ECode NetworkInterface::GetParent(
    /* [out] */ INetworkInterface** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = mParent;
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode NetworkInterface::IsUp(
    /* [out] */ Boolean* isUp)
{
    VALIDATE_NOT_NULL(isUp);
    *isUp = HasFlag(IFF_UP);
    return NOERROR;
}

ECode NetworkInterface::IsLoopback(
    /* [out] */ Boolean* isLoopback)
{
    VALIDATE_NOT_NULL(isLoopback);
    *isLoopback = HasFlag(IFF_LOOPBACK);
    return NOERROR;
}

ECode NetworkInterface::IsPointToPoint(
    /* [out] */ Boolean* isPointToPoint)
{
    VALIDATE_NOT_NULL(isPointToPoint);
    *isPointToPoint = HasFlag(IFF_POINTOPOINT);
    return NOERROR;
}

ECode NetworkInterface::SupportsMulticast(
    /* [out] */ Boolean* multicast)
{
    VALIDATE_NOT_NULL(multicast);
    *multicast = HasFlag(IFF_MULTICAST);
    return NOERROR;
}

Boolean NetworkInterface::HasFlag(
    /* [in] */ Int32 mask)
{
    Int32 flags;
    ReadIntFile(String("/sys/class/net/") + mName + String("/flags"), &flags);
    return (flags & mask) != 0;
}

ECode NetworkInterface::GetHardwareAddress(
    /* [out, callee] */ ArrayOf<Byte>** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    String s;
    ECode ec = IoUtils::ReadFileAsString(String("/sys/class/net/") + mName + String("/address"), &s);
    if (FAILED(ec)) {
        return E_SOCKET_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(s.GetLength()/3);
    for (Int32 i = 0; i < result->GetLength(); ++i) {
        Int32 value;
        ec = StringUtils::Parse(s.Substring(3*i, 3*i + 2), 16, &value);
        if (FAILED(ec)) {
            return E_SOCKET_EXCEPTION;
        }

        (*result)[i] = (Byte)(value);
    }

    // We only want to return non-zero hardware addresses.
    for (int i = 0; i < result->GetLength(); ++i) {
        if ((*result)[i] != 0) {
            *address = result;
            REFCOUNT_ADD(*address);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode NetworkInterface::GetMTU(
    /* [out] */ Int32* mtu)
{
    return ReadIntFile(String("/sys/class/net/") + mName + String("/mtu"), mtu);
}

ECode NetworkInterface::IsVirtual(
    /* [out] */ Boolean* isvirtual)
{
    VALIDATE_NOT_NULL(isvirtual);
    *isvirtual = (mParent!= NULL);
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
