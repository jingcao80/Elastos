#ifndef __ELASTOS_NET_NETWORKINTERFACE_H__
#define __ELASTOS_NET_NETWORKINTERFACE_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::IEnumeration;

namespace Elastos {
namespace Net {

class NetworkInterface
    : public Object
    , public INetworkInterface
{
public:
    CAR_INTERFACE_DECL()

    NetworkInterface();

    NetworkInterface(
        /* [in] */ const String& name,
        /* [in] */ Int32 interfaceIndex,
        /* [in] */ IList * addresses,
        /* [in] */ IList * interfaceAddresses);

    ~NetworkInterface();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 interfaceIndex,
        /* [in] */ IList * addresses,
        /* [in] */ IList * interfaceAddresses);

    static AutoPtr<INetworkInterface> ForUnboundMulticastSocket();

    /**
     * Returns the index for the network interface, or -1 if unknown.
     * @since 1.7
     */
    CARAPI GetIndex(
        /* [out] */ Int32* index);

    /**
     * Returns the name of this network interface (such as "eth0" or "lo").
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns an enumeration of the addresses bound to this network interface.
     */
    CARAPI GetInetAddresses(
        /* [out] */ IEnumeration** addresses);

    /**
     * Returns a human-readable name for this network interface. On Android, this is the same
     * string as returned by {@link #getName}.
     */
    CARAPI GetDisplayName(
        /* [out] */ String* name);

    /**
     * Returns the {@code NetworkInterface} corresponding to the named network interface, or null
     * if no interface has this name.
     *
     * @throws SocketException if an error occurs.
     * @throws NullPointerException if {@code interfaceName == null}.
     */
    static CARAPI GetByName(
        /* [in] */ const String& interfaceName,
        /* [out] */ INetworkInterface** networkInterface);

    /**
     * Returns the {@code NetworkInterface} corresponding to the given address, or null if no
     * interface has this address.
     *
     * @throws SocketException if an error occurs.
     * @throws NullPointerException if {@code address == null}.
     */
    static CARAPI GetByInetAddress(
        /* [in] */ IInetAddress* address,
        /* [out] */ INetworkInterface** networkInterface);

    /**
     * Returns the NetworkInterface corresponding to the given interface index, or null if no
     * interface has this index.
     *
     * @throws SocketException if an error occurs.
     * @since 1.7
     */
    static CARAPI GetByIndex(
         /* [in] */ Int32 index,
         /* [out] */ INetworkInterface** networkInterface);

    /**
     * Gets a list of all network interfaces available on the local system or
     * {@code null} if no interface is available.
     *
     * @return the list of {@code NetworkInterface} instances representing the
     *         available interfaces.
     * @throws SocketException
     *             if an error occurs while getting the network interface
     *             information.
     */
    static CARAPI GetNetworkInterfaces(
            /* [out] */IEnumeration** enumerator);

    /**
     * Compares the specified object to this {@code NetworkInterface} and
     * returns whether they are equal or not. The object must be an instance of
     * {@code NetworkInterface} with the same name, display name, and list
     * of interface addresses.
     *
     * @param obj
     *            the object to compare with this instance.
     * @return {@code true} if the specified object is equal to this {@code
     *         NetworkInterface}, {@code false} otherwise.
     * @see #hashCode()
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns the hash code for this {@code NetworkInterface}. Since the
     * name should be unique for each network interface the hash code is
     * generated using the name.
     */
    CARAPI GetHashCode(
        /*  [out] */ Int32* value);

    /**
     * Returns a string containing details of this network interface.
     * The exact format is deliberately unspecified. Callers that require a specific
     * format should build a string themselves, using this class' accessor methods.
     */
    CARAPI ToString(
        /*  [out] */ String* str);

    /**
     * Returns a List of the InterfaceAddresses for this network interface.
     * @since 1.6
     */
    CARAPI GetInterfaceAddresses(
        /* [out] */ IList** addresses);

    /**
     * Returns an enumeration of all the sub-interfaces of this network interface.
     * Sub-interfaces are also known as virtual interfaces.
     *
     * <p>For example, {@code eth0:1} would be a sub-interface of {@code eth0}.
     *
     * @return an Enumeration of all the sub-interfaces of this network interface
     * @since 1.6
     */
    CARAPI GetSubInterfaces(
        /* [out] */ IEnumeration** subInterfaces);

    /**
     * Returns the parent NetworkInterface of this interface if this is a
     * sub-interface, or null if it's a physical (non virtual) interface.
     *
     * @return the NetworkInterface this interface is attached to.
     * @since 1.6
     */
    CARAPI GetParent(
        /* [out] */ INetworkInterface** parent);

    /**
     * Returns true if this network interface is up.
     *
     * @return true if the interface is up.
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI IsUp(
        /* [out] */ Boolean* isUp);

    /**
     * Returns true if this network interface is a loopback interface.
     *
     * @return true if the interface is a loopback interface.
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI IsLoopback(
        /* [out] */ Boolean* isLoopback);

    /**
     * Returns true if this network interface is a point-to-point interface.
     * (For example, a PPP connection using a modem.)
     *
     * @return true if the interface is point-to-point.
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI IsPointToPoint(
        /* [out] */ Boolean* isPointToPoint);

    /**
     * Returns true if this network interface supports multicast.
     *
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI SupportsMulticast(
        /* [out] */ Boolean* multicast);

    /**
     * Returns the hardware address of the interface, if it has one, or null otherwise.
     *
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI GetHardwareAddress(
        /* [out, callee] */ ArrayOf<Byte>** address);

    /**
     * Returns the Maximum Transmission Unit (MTU) of this interface.
     *
     * @return the value of the MTU for the interface.
     * @throws SocketException if an I/O error occurs.
     * @since 1.6
     */
    CARAPI GetMTU(
        /* [out] */ Int32* mtu);

    /**
     * Returns true if this interface is a virtual interface (also called
     * a sub-interface). Virtual interfaces are, on some systems, interfaces
     * created as a child of a physical interface and given different settings
     * (like address or MTU). Usually the name of the interface will the name of
     * the parent followed by a colon (:) and a number identifying the child,
     * since there can be several virtual interfaces attached to a single
     * physical interface.
     *
     * @return true if this interface is a virtual interface.
     * @since 1.6
     */
    CARAPI IsVirtual(
        /* [out] */ Boolean* isvirtual);

private:
    /**
     * Similar to {@link #getByName(String)} except that {@code interfaceName}
     * is assumed to be valid.
     */
    static CARAPI GetByNameInternal(
        /* [in] */ const String& interfaceName,
        /* [in] */ ArrayOf<String> * ifInet6Lines,
        /* [out] */ INetworkInterface** networkInterface);

    static CARAPI ReadIfInet6Lines(
        /* [out, callee] */ ArrayOf<String> ** lines);

    static CARAPI CollectIpv6Addresses(
        /* [in] */ const String& interfaceName,
        /* [in] */ Int32 interfaceIndex,
        /* [in] */ IList* addresses,
        /* [in] */ IList* interfaceAddresses,
        /* [in] */ ArrayOf<String> * ifInet6Lines);

    static CARAPI CollectIpv4Addresses(
        /* [in] */ const String& interfaceName,
        /* [in] */ IList* addresses,
        /* [in] */ IList* interfaceAddresses);

    static CARAPI_(Boolean) IsValidInterfaceName(
        /* [in] */ const String& interfaceName);

    static CARAPI ReadIntFile(
        /* [in] */ const String& path,
        /* [out] */ Int32* value);

    static AutoPtr<IList> GetNetworkInterfacesList();

    CARAPI_(Boolean) HasFlag(
        /* [in] */ Int32 mask);

private:
    static const AutoPtr<IFile> SYS_CLASS_NET;

    String mName;

    Int32 mInterfaceIndex;

    AutoPtr<IList> mInterfaceAddresses; // List<InterfaceAddress>

    AutoPtr<IList> mAddresses;  // List<InetAddress>

    AutoPtr<INetworkInterface> mParent;

    AutoPtr<IList> mChildren;   // List<NetworkInterface>
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_NETWORKINTERFACE_H__
