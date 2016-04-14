
#include "Elastos.CoreLibrary.Net.h"
#include <CStructAddrinfo.h>

using Elastos::Net::EIID_IInet4Address;
using Elastos::Net::EIID_IInet6Address;

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructAddrinfo)

CAR_INTERFACE_IMPL(CStructAddrinfo, Object, IStructAddrinfo)

ECode CStructAddrinfo::constructor()
{
    ai_flags = ai_family = ai_socktype = ai_protocol = 0;
    ai_addr = NULL;
    ai_next = NULL;
    return NOERROR;
}

ECode CStructAddrinfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = ai_flags;
    return NOERROR;
}

ECode CStructAddrinfo::SetFlags(
    /* [in] */ Int32 flags)
{
    ai_flags = flags;
    return NOERROR;
}


ECode CStructAddrinfo::GetFamily(
    /* [out] */ Int32* family)
{
    VALIDATE_NOT_NULL(family);
    *family = ai_family;
    return NOERROR;
}

ECode CStructAddrinfo::SetFamily(
    /* [in] */ Int32 family)
{
    ai_family = family;
    return NOERROR;
}


ECode CStructAddrinfo::GetSocktype(
    /* [out] */ Int32* socktype)
{
    VALIDATE_NOT_NULL(socktype);
    *socktype = ai_socktype;
    return NOERROR;
}

ECode CStructAddrinfo::SetSocktype(
    /* [in] */ Int32 socktype)
{
    ai_socktype = socktype;
    return NOERROR;
}


ECode CStructAddrinfo::GetProtocol(
    /* [out] */ Int32* protocol)
{
    VALIDATE_NOT_NULL(protocol);
    *protocol = ai_protocol;
    return NOERROR;
}

ECode CStructAddrinfo::SetProtocol(
    /* [in] */ Int32 protocol)
{
    ai_protocol = protocol;
    return NOERROR;
}


ECode CStructAddrinfo::GetAddrlen(
    /* [out] */ Int32* addrlen)
{
    VALIDATE_NOT_NULL(addrlen);
    if(ai_addr->Probe(EIID_IInet4Address)) {
        *addrlen = 4;
    }
    else if(ai_addr->Probe(EIID_IInet6Address)) {
        *addrlen = 16;
    }
    else{
        *addrlen = 0;
    }
    return NOERROR;
}

ECode CStructAddrinfo::GetAddr(
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr);
    *addr = ai_addr;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CStructAddrinfo::SetAddr(
    /* [in] */ IInetAddress* addr)
{
    ai_addr = addr;
    return NOERROR;
}

ECode CStructAddrinfo::GetNext(
    /* [out] */ IStructAddrinfo** next)
{
    VALIDATE_NOT_NULL(next);
    *next = ai_next;
    REFCOUNT_ADD(*next);
    return NOERROR;
}

ECode CStructAddrinfo::SetNext(
    /* [in] */ IStructAddrinfo* next)
{
    ai_next = next;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
