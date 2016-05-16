/** @addtogroup SystemRef
  *   @{
  */

//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELASYSAPI_H__
#define __ELASYSAPI_H__

#include <elapi.h>

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

/**
 * The CSystem class contains several useful class fields and methods.
 *
 * The CSystem class cannot be instantiated.
 */
class CSystem
{
public:
    /**
     * Registers service manager interface in the operation system for incoming requests.
     *
     * @param[in] pServiceManager The service manager interface that will handle requests.
     * @return If the function succeeds, the return value is ::NOERROR.
     */
    STATIC CARAPI RegisterServiceManager(
        /* [in] */ PInterface pServiceManager)
    {
        return _CSystem_RegisterServiceManager(pServiceManager);
    }

    /**
     * Get the service manager interface in operation system.
     *
     * @param[out] ppServiceManager Pointer to the interface variable that receives the service manager interface.
     * @return If the function succeeds, the return value is ::NOERROR.
     */
    STATIC CARAPI GetServiceManager(
        /* [out] */ PInterface *ppServiceManager)
    {
        return _CSystem_GetServiceManager(ppServiceManager);
    }

    /**
     * Retrives the current value of the high-resolution performance counter.
     *
     * @param[out] pPerformanceCount The pointer to a variable that receives the current performance counter.
     * @return If the function succeeds, the return value is ::NOERROR.
     * @return If the function fails and the return value is ::E_INVALID_ARGUMENT,
     * it indicats that the value of pPerformanceCount parameter is NULL.
     * @return For descriptions of the other possible returns, please refer to the \ref CARTypesRef.
     */
    STATIC CARAPI QueryPerformanceCounter(
        /* [out] */ Int64 *pPerformanceCount)
    {
        return _CSystem_QueryPerformanceCounter(pPerformanceCount);
    }

    /**
     * Retrives the frequency of the high-resolution performance counter.
     *
     * @param[out] pFrequency The pointer to a variable that receives the performance counter frequency.
     * @return If the function succeeds, the return value is ::NOERROR.
     * @return If the function fails and the return value is ::E_INVALID_ARGUMENT,
     * it indicats that the value of pFrequency parameter is NULL.
     * @return For descriptions of the other possible returns, please refer to the \ref CARTypesRef.
     */
    STATIC CARAPI QueryPerformanceFrequency (
        /* [out] */ Int64 *pFrequency)
    {
        return _CSystem_QueryPerformanceFrequency(pFrequency);
    }
};

/** @} */

/** @addtogroup CARRef
  *   @{
  */
class CReflector
{
public:
    STATIC CARAPI AcquireModuleInfo(
        /* [in] */ CString name,
        /* [out] */ IModuleInfo **ppModuleInfo)
    {
        return _CReflector_AcquireModuleInfo(name, ppModuleInfo);
    }

    STATIC CARAPI AcquireIntrinsicTypeInfo(
        /* [in] */ CarDataType intrinsicType,
        /* [out] */ IDataTypeInfo **ppIntrinsicTypeInfo)
    {
        return _CReflector_AcquireIntrinsicTypeInfo(intrinsicType,
            ppIntrinsicTypeInfo);
    }

    STATIC CARAPI AcquireEnumInfo(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& itemNames,
        /* [in] */ const BufferOf<Int32>& itemValues,
        /* [out] */ IEnumInfo **ppEnumInfo)
    {
        return _CReflector_AcquireEnumInfo(name, &itemNames, &itemValues,
            ppEnumInfo);
    }

    STATIC CARAPI AcquireCppVectorInfo(
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [in] */ Int32 length,
        /* [out] */ ICppVectorInfo **ppCppVectorInfo)
    {
        return _CReflector_AcquireCppVectorInfo(pElementTypeInfo, length,
            ppCppVectorInfo);
    }

    STATIC CARAPI AcquireStructInfo(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& fieldNames,
        /* [in] */ const BufferOf<IDataTypeInfo *>& fieldTypeInfos,
        /* [out] */ IStructInfo **ppStructInfo)
    {
        return _CReflector_AcquireStructInfo(name, &fieldNames,
            &fieldTypeInfos, ppStructInfo);
    }

    STATIC CARAPI AcquireCarArrayInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [out] */ ICarArrayInfo **ppCarArrayInfo)
    {
        return _CReflector_AcquireCarArrayInfo(quintetType, pElementTypeInfo,
            ppCarArrayInfo);
    }
};

class CObject
{
public:
    STATIC CARAPI_(Boolean) Compare(
        /* [in] */ PInterface pObjectA,
        /* [in] */ PInterface pObjectB)
    {
        return _CObject_Compare(pObjectA, pObjectB);
    }

    STATIC CARAPI ReleaseAtProcessExit(
        /* [in] */ PInterface pObject)
    {
        return _CObject_ReleaseAtProcessExit(pObject);
    }

    STATIC CARAPI ReleaseAtThreadQuit(
        /* [in] */ PInterface pObject)
    {
        return _CObject_ReleaseAtThreadQuit(pObject);
    }

    STATIC CARAPI EnterRegime(PInterface pObject, PRegime pRegime)
    {
        return _CObject_EnterRegime(pObject, pRegime);
    }

    STATIC CARAPI LeaveRegime(PInterface pObject, PRegime pRegime)
    {
        return _CObject_LeaveRegime(pObject, pRegime);
    }

    STATIC CARAPI EnterRegime(PInterface pObject, PInterface pRgmObj)
    {
        PRegime pRegime = IRegime::Probe(pRgmObj);
        if (!pRegime) return E_NO_INTERFACE;
        return _CObject_EnterRegime(pObject, pRegime);
    }

    STATIC CARAPI LeaveRegime(PInterface pObject, PInterface pRgmObj)
    {
        PRegime pRegime = IRegime::Probe(pRgmObj);
        if (!pRegime) return E_NO_INTERFACE;
        return _CObject_LeaveRegime(pObject, pRegime);
    }

    STATIC CARAPI CreateInstance(
        RClassID rclsid,
        PRegime pRegime,
        REIID riid,
        PInterface *ppObj)
    {
        return _CObject_CreateInstance(rclsid, pRegime, riid, ppObj);
    }

    STATIC CARAPI CreateInstanceEx(
        RClassID rclsid,
        PRegime pRegime,
        UInt32 cmq,
        PMULTIQI pResults)
    {
        return _CObject_CreateInstanceEx(rclsid, pRegime, cmq, pResults);
    }

    STATIC CARAPI AcquireClassFactory(
        RClassID rclsid,
        PRegime pRegime,
        REIID iid,
        PInterface *ppObject)
    {
        return _CObject_AcquireClassFactory(rclsid, pRegime, iid, ppObject);
    }

    STATIC CARAPI ReflectModuleInfo(
        PInterface pObj,
        IModuleInfo **piModuleInfo)
    {
        return _CObject_ReflectModuleInfo(pObj, piModuleInfo);
    }

    STATIC CARAPI ReflectClassInfo(
        PInterface pObj,
        IClassInfo **piClassInfo)
    {
        return _CObject_ReflectClassInfo(pObj, piClassInfo);
    }

    STATIC CARAPI ReflectInterfaceInfo(
        PInterface pObj,
        IInterfaceInfo **piInterfaceInfo)
    {
        return _CObject_ReflectInterfaceInfo(pObj, piInterfaceInfo);
    }

    STATIC CARAPI Acquire(
        CString uri,
        PInterface *object)
    {
        return _CScheme_Instantiate(uri, object);
    }
};

class CCallbackParcel
{
public:
    STATIC CARAPI New(
        /* [out] */ IParcel ** ppParcel)
    {
        return _CCallbackParcel_New(ppParcel);
    }
};

class CParcel
{
public:
    STATIC CARAPI New(
        /* [out] */ IParcel ** ppParcel)
    {
        return _CParcel_New(ppParcel);
    }
};

_ELASTOS_NAMESPACE_END

#endif // __cplusplus

#endif // __ELASYSAPI_H__
/** @} */

