//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#ifndef __LOCMOD_H__
#define __LOCMOD_H__

#include <clsinfo.h>
#include <syslinknode.h>

typedef ECode (__stdcall *PDLLGETCLASSOBJECT)(
                REMuid, REIID, PInterface *);
typedef ECode (__stdcall *PDLLCANUNLOADNOW)();

typedef struct LocalModule : DLinkNode {
    char                *m_pszUunm;
    Void                *m_pIModule;
    PDLLGETCLASSOBJECT  m_pDllGetClassObject;
    PDLLCANUNLOADNOW    m_pDllCanUnloadNow;
    CIClassInfo         *m_pClassInfo;
    Int32               m_nAskCount;
}   LocalModule;

ECode  AcquireClassObjectFromLocalModule(
        /* [in] */ RClassID rclsid,
        /* [in] */ REIID riid,
        /* [out] */ PInterface *ppObject);

#endif // __LOCMOD_H__
