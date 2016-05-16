//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CDelegateProxy.h"
#include "CCallbackMethodInfo.h"
#include "CCallbackArgumentList.h"

#if defined(_MSVC)
#pragma warning(disable: 4731)
#endif


UInt32 CDelegateProxy::AddRef()
{
    Int32 nRef = atomic_inc(&m_cRef);
    return (UInt32)nRef;
}

UInt32 CDelegateProxy::Release()
{
    Int32 nRef = atomic_dec(&m_cRef);

    if (0 == nRef) {
        delete this;
    }
    assert(nRef >= 0);
    return nRef;
}

PInterface CDelegateProxy::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IDelegateProxy) {
        return (IDelegateProxy *)this;
    }
    else {
        return NULL;
    }
}

ECode CDelegateProxy::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

CDelegateProxy::CDelegateProxy(
    /* [in] */ ICallbackMethodInfo * pCallbackMethodInfo,
    /* [in] */ ICallbackInvocation * pCallbackInvocation,
    /* [in] */ PVoid targetObject,
    /* [in] */ PVoid targetMethod)
{
    m_pTargetObject = targetObject;
    m_pTargetMethod = targetMethod;
    pCallbackMethodInfo->AddRef();
    m_pCallbackMethodInfo = pCallbackMethodInfo;
    pCallbackInvocation->AddRef();
    m_pCallbackInvocation = pCallbackInvocation;
    m_cRef = 0;
}

CDelegateProxy::~CDelegateProxy()
{
    if (m_pCallbackMethodInfo) m_pCallbackMethodInfo->Release();
    if (m_pCallbackInvocation) m_pCallbackInvocation->Release();
}

ECode CDelegateProxy::GetCallbackMethodInfo(
    /* [out] */ ICallbackMethodInfo ** ppCallbackMethodInfo)
{
    if (!ppCallbackMethodInfo) {
        return E_INVALID_ARGUMENT;
    }

    m_pCallbackMethodInfo->AddRef();
    *ppCallbackMethodInfo = m_pCallbackMethodInfo;
    return NOERROR;
}

ECode CDelegateProxy::GetTargetObject(
    /* [out] */ PVoid * pTargetObject)
{
    if (!pTargetObject) {
        return E_INVALID_ARGUMENT;
    }

    *pTargetObject = m_pTargetObject;
    return NOERROR;
}

ECode CDelegateProxy::GetTargetMethod(
    /* [out] */ PVoid * pTargetMethod)
{
    if (!pTargetMethod) {
        return E_INVALID_ARGUMENT;
    }

    *pTargetMethod = m_pTargetMethod;
    return NOERROR;
}

ECode CDelegateProxy::GetCallbackInvocation(
    /* [out] */ ICallbackInvocation ** ppCallbackInvocation)
{
    if (!ppCallbackInvocation) {
        return E_INVALID_ARGUMENT;
    }

    m_pCallbackInvocation->AddRef();
    *ppCallbackInvocation = m_pCallbackInvocation;
    return NOERROR;
}

ECode CDelegateProxy::GetDelegate(
    /* [out] */ EventHandler * pHandler)
{
    if (!pHandler) {
        return E_INVALID_ARGUMENT;
    }

#ifndef _arm
    ECode (__stdcall CDelegateProxy::*pFunc)(PInterface) =
        &CDelegateProxy::OnEvent;
#else
    ECode (__stdcall CDelegateProxy::*pFunc)(PInterface, ...) =
        &CDelegateProxy::OnEvent;
#endif

    *pHandler = EventHandler::Make((void *)this, *(void**)&pFunc);

    return NOERROR;
}

ECode CDelegateProxy::EventHander(
    /* [in] */ PVoid pParamBuf,
    /* [out] */ UInt32 *pParamBufSize)
{
    AutoPtr<CCallbackArgumentList> pCBArgumentList;

    ECode ec = ((CCallbackMethodInfo *)m_pCallbackMethodInfo)
        ->CreateCBArgumentList((ICallbackArgumentList **)&pCBArgumentList);
    if (FAILED(ec)) {
        return ec;
    }

    memcpy(pCBArgumentList->m_pParamBuf, pParamBuf,
        pCBArgumentList->m_uParamBufSize);

    if (pParamBufSize) {
        *pParamBufSize = pCBArgumentList->m_uParamBufSize - 4;
    }

    m_pCallbackInvocation->Invoke(m_pTargetObject, m_pTargetMethod,
        pCBArgumentList);

    return NOERROR;
}

#ifdef _mips
ECode CDelegateProxy::OnEvent(PInterface pServer)
{
    ASM("break 0;");
    return NOERROR;
}
#elif _x86
ECode CDelegateProxy::OnEvent(PInterface pServer)
{
    UInt32 uMoveSize;
    ECode ec = EventHander(&pServer, &uMoveSize);

    if (uMoveSize) {
#if _GNUC
        __asm__(
            "pushl  %%ecx\n"
            "pushl  %%edx\n"
            "pushl  %%esi\n"
            "pushl  %%edi\n"

            "movl   %0, %%edx\n"

            "movl   %%ebp, %%ecx\n"  //get the number need to move
            "subl   %%esp, %%ecx\n"
            "addl   $8,  %%ecx\n"
            "shrl   $2, %%ecx\n"

            "movl   %%ebp, %%esi\n"  //set the esi point to the return address
            "addl   $4, %%esi\n"
            "movl   %%esi, %%edi\n"
            "addl   %%edx, %%edi\n"

            "std\n"             //set direction flag
            "rep movsd\n"
            "cld\n"               //clear direction flag

            "addl   %%edx, %%esp\n"  //adjuct the ebp
            "addl   %%edx, %%ebp\n"  //adjuct the ebp

            "popl   %%edi\n"
            "popl   %%esi\n"
            "popl   %%edx\n"
            "popl   %%ecx\n"
            :
            : "m"(uMoveSize)
        );
#else //_MSC_VER
        __asm {
            push  ecx
            push  edx
            push  esi
            push  edi

            mov   edx, uMoveSize

            mov   ecx, ebp
            sub   ecx, esp
            add   ecx, 8
            shr   ecx, 2

            mov   esi, ebp
            add   esi, 4
            mov   edi, esi
            add   edi, edx

            std
            rep movsd
            cld

            add   esp, edx
            add   ebp, edx

            pop   edi
            pop   esi
            pop   edx
            pop   ecx
        }
#endif
    }

    return ec;
}
#elif _arm
ECode CDelegateProxy::OnEvent(PInterface pServer,...)
{
    return EventHander(&pServer, NULL);
}
#endif
