/** @addtogroup SystemRef
  *   @{
  *
  * Elastos API is Elastos's core set of application programming interfaces (APIs).
  */

//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELAPI_H__
#define __ELAPI_H__

#include <elastos.h>

#ifdef _UNDEFDLLEXP
#include <elasys_server.h>
#else
#include <elasys.h>
#endif

#include <stdint.h>

/**
 * @see CSystem::RegisterServiceManager()
 */
ELAPI _CSystem_RegisterServiceManager(
    PInterface pService);

/**
 * @see CSystem::GetServiceManager()
 */
ELAPI _CSystem_GetServiceManager(
    PInterface *ppService);

/**
 * @see CSystem::QueryPerformanceCounter()
 */
ELAPI _CSystem_QueryPerformanceCounter(
        /* [out] */ _ELASTOS Int64 *pPerformanceCount);

/**
 * @see CSystem::QueryPerformanceFrequency()
 */
ELAPI _CSystem_QueryPerformanceFrequency(
        /* [out] */ _ELASTOS Int64 *pFrequency);

/** @} */
/** @addtogroup CARRef
  *   @{
  */
#include <callback.h>

ELAPI _CReflector_AcquireModuleInfo(
    /* [in] */ _ELASTOS CString name,
    /* [out] */ IModuleInfo **piModuleInfo);

ELAPI _CReflector_AcquireIntrinsicTypeInfo(
    /* [in] */ CarDataType intrinsicType,
    /* [out] */ IDataTypeInfo **ppIntrinsicTypeInfo);

#ifdef __cplusplus
ELAPI _CReflector_AcquireEnumInfo(
    /* [in] */ _ELASTOS CString name,
    /* [in] */ const _ELASTOS BufferOf<_ELASTOS CString> *pItemNames,
    /* [in] */ const _ELASTOS BufferOf<_ELASTOS Int32> *pItemValues,
    /* [out] */ IEnumInfo **ppEnumInfo);


ELAPI _CReflector_AcquireStructInfo(
    /* [in] */ _ELASTOS CString name,
    /* [in] */ const _ELASTOS BufferOf<_ELASTOS CString> *pFieldNames,
    /* [in] */ const _ELASTOS BufferOf<IDataTypeInfo *> *pFieldTypeInfos,
    /* [out] */ IStructInfo **ppStructInfo);
#else

ELAPI _CReflector_AcquireEnumInfo(
    /* [in] */ _ELASTOS CString name,
    /* [in] */ const _ELASTOS PCarQuintet pItemNames,
    /* [in] */ const _ELASTOS PCarQuintet pItemValues,
    /* [out] */ IEnumInfo **ppEnumInfo);


ELAPI _CReflector_AcquireStructInfo(
    /* [in] */ _ELASTOS CString name,
    /* [in] */ const _ELASTOS PCarQuintet pFieldNames,
    /* [in] */ const _ELASTOS PCarQuintet pFieldTypeInfos,
    /* [out] */ IStructInfo **ppStructInfo);
#endif

ELAPI _CReflector_AcquireCppVectorInfo(
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [in] */ _ELASTOS Int32 length,
    /* [out] */ ICppVectorInfo **ppCppVectorInfo);


ELAPI _CReflector_AcquireCarArrayInfo(
    /* [in] */ CarDataType quintetType,
    /* [in] */ IDataTypeInfo *pElementTypeInfo,
    /* [out] */ ICarArrayInfo **ppCarArrayInfo);

ELAPI _CObject_ReflectModuleInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IModuleInfo **piModuleInfo);

ELAPI _CObject_ReflectClassInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IClassInfo **piClassInfo);

ELAPI _CObject_ReflectInterfaceInfo(
    /* [in] */ PInterface pObj,
    /* [out] */ IInterfaceInfo **piInterfaceInfo);

ELAPI _CScheme_Instantiate(
    /* [in] */ const _ELASTOS CString uri,
    /* [out] */ PInterface *object);

ELAPI_(_ELASTOS Boolean) _Impl_CheckHelperInfoFlag(_ELASTOS Flags32 flag);

ELAPI_(void) _Impl_SetHelperInfoFlag(_ELASTOS Flags32 flag, _ELASTOS Boolean bValue);

ELAPI _Impl_EnterProtectedZone();

ELAPI _Impl_LeaveProtectedZone();

ELAPI _Impl_InsideProtectedZone();

// callback helper api for making parameters
ELAPI _Impl_CheckClsId(
    PInterface pServerObj,
    const _ELASTOS ClassID* pClassID,
    PInterface *ppServerObj);

ELAPI _Impl_AcquireCallbackHandler(
        PInterface pServerObj,
        _ELASTOS REIID iid,
        PInterface *ppHandler);

ELAPI _CCallbackParcel_New(
    /* [out] */ IParcel **ppObj);

ELAPI _CParcel_New(
    /* [out] */ IParcel **ppObj);

ELAPI _CObject_MarshalInterface(
    /* [in] */ IInterface *pObj,
    /* [in] */ MarshalType type,
    /* [out] */ void **ppBuf,
    /* [out] */ _ELASTOS Int32 *pSize);

ELAPI _CObject_UnmarshalInterface(
    /* [in] */ void *pBuf,
    /* [in] */ MarshalType type,
    /* [in] */ UnmarshalFlag flag,
    /* [out] */ IInterface **ppObj,
    /* [out] */ _ELASTOS Int32 *pSize);

#endif // __ELAPI_H__
/** @} */

