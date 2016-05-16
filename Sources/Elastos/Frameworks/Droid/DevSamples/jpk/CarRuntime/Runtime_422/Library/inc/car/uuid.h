//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CAR_UUID_H__
#define __CAR_UUID_H__

#ifdef _UNDEFDLLEXP
#define DLLEXP
#else
#define DLLEXP __declspec(dllimport)
#endif

EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_METADATA_INFO;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_CALLBACK_CONNECTOR;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_SUPER_CALLBACK_CONNECTOR;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_CALLBACK_TRIGGER;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_GENERIC_INFO;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_MUTEX_INFO;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_NEXT_ASPECT;
EXTERN_C DLLEXP const _ELASTOS InterfaceID EIID_SUPER_OBJECT;

#define _Impl_Memcmp memcmp
#define _Impl_Strcmp strcmp

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

#if defined(_arm) || defined(_mips)

CAR_INLINE Boolean IsEqualEMuid(REMuid r1, REMuid r2)
{
    return (0 == _Impl_Memcmp(&r1, &r2, sizeof(EMuid)));
}

#else // !_arm && !_mips

CAR_INLINE Boolean IsEqualEMuid(REMuid r1, REMuid r2)
{
    return (((UInt32*)&r1)[0] == ((UInt32*)&r2)[0] &&
            ((UInt32*)&r1)[1] == ((UInt32*)&r2)[1] &&
            ((UInt32*)&r1)[2] == ((UInt32*)&r2)[2] &&
            ((UInt32*)&r1)[3] == ((UInt32*)&r2)[3]);
}

#endif // _arm || _mips

CAR_INLINE Boolean operator ==(REMuid rguid1, REMuid rguid2)
{
    return IsEqualEMuid(rguid1, rguid2);
}

CAR_INLINE Boolean operator ==(REMuid rguid, RClassID rclsid)
{
    return IsEqualEMuid(rguid, rclsid.clsid);
}

CAR_INLINE Boolean operator ==(RClassID rclsid, REMuid rguid)
{
    return IsEqualEMuid(rguid, rclsid.clsid);
}

CAR_INLINE Boolean operator !=(REMuid rguid1, REMuid rguid2)
{
    return !IsEqualEMuid(rguid1, rguid2);
}

CAR_INLINE Boolean operator !=(REMuid rguid, RClassID rclsid)
{
    return !IsEqualEMuid(rguid, rclsid.clsid);
}

CAR_INLINE Boolean operator !=(RClassID rclsid, REMuid rguid)
{
    return !IsEqualEMuid(rguid, rclsid.clsid);
}

_ELASTOS_NAMESPACE_END

#else  // !__cplusplus

#define IsEqualEMuid(rguid1, rguid2) (!_Impl_Memcmp(rguid1, rguid2, sizeof(_ELASTOS EMuid)))

#endif // __cplusplus

#define IsEqualIID(riid1, riid2)        IsEqualEMuid(riid1, riid2)
#define IsEqualCLSID(rclsid1, rclsid2)  IsEqualEMuid(rclsid1, rclsid2)

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

CAR_INLINE Boolean IsEqualUunm(const char *pUunm1, const char *pUunm2)
{
    return !_Impl_Strcmp(pUunm1, pUunm2);
}

CAR_INLINE Boolean IsEqualClassId(RClassID r1, RClassID r2)
{
    return IsEqualEMuid(r1.clsid, r2.clsid) && IsEqualUunm(r1.pUunm, r2.pUunm);
}

CAR_INLINE Boolean operator ==(RClassID r1, RClassID r2)
{
    return IsEqualClassId(r1, r2);
}

CAR_INLINE Boolean operator !=(RClassID r1, RClassID r2)
{
    return !IsEqualClassId(r1, r2);
}

_ELASTOS_NAMESPACE_END

#else // !__cplusplus

#define IsEqualUunm(u1, u2) \
    !_Impl_Strcmp(u1, u2)

#define IsEqualClassId(r1, r2) \
    (IsEqualEMuid(&r1.clsid, &r2.clsid) && IsEqualUunm(r1.pUunm, r2.pUunm))

#endif // __cplusplus

#endif // __CAR_UUID_H__
