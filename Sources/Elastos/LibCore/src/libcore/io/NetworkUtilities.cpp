#include "NetworkUtilities.h"
#include "cutils/log.h"
#include "net/CInetUnixAddress.h"
#include "net/InetAddress.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

using Elastos::Net::InetAddress;
using Elastos::Net::CInetUnixAddress;

AutoPtr<IInetAddress> SockaddrToInetAddress(
    /* [in] */ const sockaddr_storage& ss,
    /* [out] */ Int32* port)
{
    // Convert IPv4-mapped IPv6 addresses to IPv4 addresses.
    // The RI states "Java will never return an IPv4-mapped address".
    const sockaddr_in6& sin6 = reinterpret_cast<const sockaddr_in6&>(ss);
    if (ss.ss_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&sin6.sin6_addr)) {
        // Copy the IPv6 address into the temporary sockaddr_storage.
        sockaddr_storage tmp;
        memset(&tmp, 0, sizeof(tmp));
        memcpy(&tmp, &ss, sizeof(sockaddr_in6));
        // Unmap it into an IPv4 address.
        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(tmp);
        sin.sin_family = AF_INET;
        sin.sin_port = sin6.sin6_port;
        memcpy(&sin.sin_addr.s_addr, &sin6.sin6_addr.s6_addr[12], 4);
        // Do the regular conversion using the unmapped address.
        return SockaddrToInetAddress(tmp, port);
    }

    const void* rawAddress;
    size_t addressLength;
    int sin_port = 0;
    int scope_id = 0;
    if (ss.ss_family == AF_INET) {
        const sockaddr_in& sin = reinterpret_cast<const sockaddr_in&>(ss);
        rawAddress = &sin.sin_addr.s_addr;
        addressLength = 4;
        sin_port = ntohs(sin.sin_port);
    } else if (ss.ss_family == AF_INET6) {
        const sockaddr_in6& sin6 = reinterpret_cast<const sockaddr_in6&>(ss);
        rawAddress = &sin6.sin6_addr.s6_addr;
        addressLength = 16;
        sin_port = ntohs(sin6.sin6_port);
        scope_id = sin6.sin6_scope_id;
    } else if (ss.ss_family == AF_UNIX) {
        const sockaddr_un& sun = reinterpret_cast<const sockaddr_un&>(ss);
        rawAddress = &sun.sun_path;
        addressLength = strlen(sun.sun_path);
    } else {
        // We can't throw SocketException. We aren't meant to see bad addresses, so seeing one
        // really does imply an internal error.
        // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
        //                      "sockaddrToInetAddress unsupported ss_family: %i", ss.ss_family);
        ALOGE("sockaddrToInetAddress unsupported ss_family: %i", ss.ss_family);
        return NULL;
    }
    if (port != NULL) {
        *port = sin_port;
    }

    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(addressLength);
    byteArray->Copy(reinterpret_cast<const Byte*>(rawAddress), addressLength);

    if (ss.ss_family == AF_UNIX) {
        // Note that we get here for AF_UNIX sockets on accept(2). The unix(7) man page claims
        // that the peer's sun_path will contain the path, but in practice it doesn't, and the
        // peer length is returned as 2 (meaning only the sun_family field was set).
        AutoPtr<IInetAddress> rst;
        CInetUnixAddress::New(byteArray, (IInetAddress**)&rst);
        return rst;
    }

    AutoPtr<IInetAddress> rst;
    InetAddress::GetByAddress(String(NULL), byteArray, scope_id, (IInetAddress**)&rst);
    return rst;
}

static Boolean InetAddressToSockaddr(
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ sockaddr_storage& ss,
    /* [in] */ socklen_t& sa_len,
    /* [in] */ Boolean map) {
    memset(&ss, 0, sizeof(ss));
    sa_len = 0;

    if (inetAddress == NULL) {
        // jniThrowNullPointerException(env, NULL);
        ALOGE("inetAddress can not be NULL !!");
        return FALSE;
    }

    // Get the address family.
    Int32 valueTmp;
    inetAddress->GetFamily(&valueTmp);
    ss.ss_family = valueTmp;
    if (ss.ss_family == AF_UNSPEC) {
        sa_len = sizeof(ss.ss_family);
        return TRUE; // Job done!
    }

    // Check this is an address family we support.
    if (ss.ss_family != AF_INET && ss.ss_family != AF_INET6 && ss.ss_family != AF_UNIX) {
        // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
        //         "inetAddressToSockaddr bad family: %i", ss.ss_family);
        ALOGE("inetAddressToSockaddr bad family: %i", ss.ss_family);
        return FALSE;
    }

    // Get the byte array that stores the IP address bytes in the InetAddress.
    AutoPtr<ArrayOf<Byte> > addressBytes;
    inetAddress->GetAddress((ArrayOf<Byte>**)&addressBytes);
    if (addressBytes == NULL) {
        // jniThrowNullPointerException(env, NULL);
        ALOGE("addressBytes can not be NULL !!");
        return FALSE;
    }

    // Handle the AF_UNIX special case.
    if (ss.ss_family == AF_UNIX) {
        sockaddr_un& sun = reinterpret_cast<sockaddr_un&>(ss);

        size_t path_length = addressBytes->GetLength();
        if (path_length >= sizeof(sun.sun_path)) {
            // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
            //                      "inetAddressToSockaddr path too long for AF_UNIX: %i", path_length);
            ALOGE("inetAddressToSockaddr path too long for AF_UNIX: %i", path_length);
            return FALSE;
        }

        // Copy the bytes...
        Byte* dst = reinterpret_cast<Byte*>(&sun.sun_path);
        memset(dst, 0, sizeof(sun.sun_path));
        // env->GetByteArrayRegion(addressBytes.get(), 0, path_length, dst);
        memcpy(dst, addressBytes->GetPayload(), path_length * sizeof(Byte));
        sa_len = sizeof(sun.sun_path);
        return TRUE;
    }

    // TODO: bionic's getnameinfo(3) seems to want its length parameter to be exactly
    // sizeof(sockaddr_in) for an IPv4 address and sizeof (sockaddr_in6) for an
    // IPv6 address. Fix getnameinfo so it accepts sizeof(sockaddr_storage), and
    // then unconditionally set sa_len to sizeof(sockaddr_storage) instead of having
    // to deal with this case by case.

    // We use AF_INET6 sockets, so we want an IPv6 address (which may be a IPv4-mapped address).
    sockaddr_in6& sin6 = reinterpret_cast<sockaddr_in6&>(ss);
    sin6.sin6_port = htons(port);
    if (ss.ss_family == AF_INET6) {
        // IPv6 address. Copy the bytes...
        Byte* dst = reinterpret_cast<Byte*>(&sin6.sin6_addr.s6_addr);
        // env->GetByteArrayRegion(addressBytes.get(), 0, 16, dst);
        memcpy(dst, addressBytes->GetPayload(), 16 * sizeof(Byte));
        // ...and set the scope id...
        AutoPtr<IInet6Address> v6Addr = IInet6Address::Probe(inetAddress);
        assert(v6Addr != NULL);
        v6Addr->GetScopeId(&valueTmp);
        sin6.sin6_scope_id = valueTmp;
        sa_len = sizeof(sockaddr_in6);
        return TRUE;
    }

    // Deal with Inet4Address instances.
    if (map) {
        // We should represent this Inet4Address as an IPv4-mapped IPv6 sockaddr_in6.
        // Change the family...
        sin6.sin6_family = AF_INET6;
        // Copy the bytes...
        Byte* dst = reinterpret_cast<Byte*>(&sin6.sin6_addr.s6_addr[12]);
        // env->GetByteArrayRegion(addressBytes.get(), 0, 4, dst);
        memcpy(dst, addressBytes->GetPayload(), 4 * sizeof(Byte));
        // INADDR_ANY and in6addr_any are both all-zeros...
        if (!IN6_IS_ADDR_UNSPECIFIED(&sin6.sin6_addr)) {
            // ...but all other IPv4-mapped addresses are ::ffff:a.b.c.d, so insert the ffff...
            memset(&(sin6.sin6_addr.s6_addr[10]), 0xff, 2);
        }
        sa_len = sizeof(sockaddr_in6);
    } else {
        // We should represent this Inet4Address as an IPv4 sockaddr_in.
        sockaddr_in& sin = reinterpret_cast<sockaddr_in&>(ss);
        sin.sin_port = htons(port);
        Byte* dst = reinterpret_cast<Byte*>(&sin.sin_addr.s_addr);
        // env->GetByteArrayRegion(addressBytes.get(), 0, 4, dst);
        memcpy(dst, addressBytes->GetPayload(), 4 * sizeof(Byte));
        sa_len = sizeof(sockaddr_in);
    }
    return TRUE;
}

Boolean InetAddressToSockaddrVerbatim(
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ sockaddr_storage& ss,
    /* [in] */ socklen_t& sa_len)
{
    return InetAddressToSockaddr(inetAddress, port, ss, sa_len, FALSE);
}

Boolean InetAddressToSockaddr(
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Int32 port,
    /* [in] */ sockaddr_storage& ss,
    /* [in] */ socklen_t& sa_len)
{
    return InetAddressToSockaddr(inetAddress, port, ss, sa_len, TRUE);
}

Boolean SetBlocking(
    /* [in] */ Int32 fd,
    /* [in] */ Boolean  blocking)
{
    Int32 flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        return FALSE;
    }

    if (!blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    Int32 rc = fcntl(fd, F_SETFL, flags);
    return (rc != -1);
}