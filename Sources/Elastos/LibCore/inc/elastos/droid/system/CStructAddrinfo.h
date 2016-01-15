#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTADDRINFO_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTADDRINFO_H__

#include "Elastos.CoreLibrary.Net.h"
#include "core/Object.h"
#include "_Elastos_Droid_System_CStructAddrinfo.h"

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace System {

CarClass(CStructAddrinfo)
    , public Object
    , public IStructAddrinfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /** Desired address family for results. (Such as AF_INET6 for IPv6. AF_UNSPEC means "any".) */
    CARAPI GetFamily(
        /* [out] */ Int32* family);

    CARAPI SetFamily(
        /* [in] */ Int32 family);

    /** Socket type. (Such as SOCK_DGRAM. 0 means "any".) */
    CARAPI GetSocktype(
        /* [out] */ Int32* socktype);

    CARAPI SetSocktype(
        /* [in] */ Int32 socktype);

    /** Protocol. (Such as IPPROTO_IPV6 IPv6. 0 means "any".) */
    CARAPI GetProtocol(
        /* [out] */ Int32* protocol);

    CARAPI SetProtocol(
        /* [in] */ Int32 protocol);

    /** Address length. (Not useful in Java.) */
    CARAPI GetAddrlen(
        /* [out] */ Int32* addrlen);

    /** Address. */
    CARAPI GetAddr(
        /* [out] */ IInetAddress** addr);

    CARAPI SetAddr(
        /* [in] */ IInetAddress* addr);

    /** Canonical name of service location (if AI_CANONNAME provided in ai_flags). */
    // public String ai_canonname;

    /** Next element in linked list. */
    CARAPI GetNext(
        /* [out] */ IStructAddrinfo** next);

    CARAPI SetNext(
        /* [in] */ IStructAddrinfo* next);
private:
    Int32 ai_flags;
    Int32 ai_family;
    Int32 ai_socktype;
    Int32 ai_protocol;
    AutoPtr<IInetAddress> ai_addr;
    AutoPtr<IStructAddrinfo> ai_next;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif
