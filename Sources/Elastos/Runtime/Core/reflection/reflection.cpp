//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CEntryList.h"
#include "CModuleInfo.h"

CObjInfoList g_objInfoList;

ELAPI _CReflector_AcquireModuleInfo(
    /* [in] */ const String& name,
    /* [out] */ IModuleInfo** moduleInfo)
{
    return g_objInfoList.AcquireModuleInfo(name, moduleInfo);
}

ELAPI _CReflector_AcquireIntrinsicTypeInfo(
    /* [in] */ CarDataType intrinsicType,
    /* [out] */ IDataTypeInfo** intrinsicTypeInfo)
{
    return g_objInfoList.AcquireIntrinsicInfo(intrinsicType, intrinsicTypeInfo);
}

ELAPI _CReflector_AcquireEnumInfo(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* itemNames,
    /* [in] */ ArrayOf<Int32>* itemValues,
    /* [out] */ IEnumInfo** enumInfo)
{
    return g_objInfoList.AcquireDynamicEnumInfo(name, itemNames,
            itemValues, enumInfo);
}

ELAPI _CReflector_AcquireCppVectorInfo(
    /* [in] */ IDataTypeInfo* elementTypeInfo,
    /* [in] */ Int32 length,
    /* [out] */ ICppVectorInfo** cppVectorInfo)
{
    return g_objInfoList.AcquireCppVectorInfo(elementTypeInfo,
            length, cppVectorInfo);
}

ELAPI _CReflector_AcquireStructInfo(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* fieldNames,
    /* [in] */ ArrayOf<IDataTypeInfo *>* fieldTypeInfos,
    /* [out] */ IStructInfo** structInfo)
{
    return g_objInfoList.AcquireDynamicStructInfo(name, fieldNames,
            fieldTypeInfos, structInfo);
}

ELAPI _CReflector_AcquireCarArrayInfo(
    /* [in] */ CarDataType quintetType,
    /* [in] */ IDataTypeInfo* elementTypeInfo,
    /* [out] */ ICarArrayInfo** carArrayInfo)
{
    return g_objInfoList.AcquireCarArrayInfo(quintetType, elementTypeInfo,
            carArrayInfo);
}

ELAPI _CObject_ReflectModuleInfo(
    /* [in] */ PInterface object,
    /* [out] */ IModuleInfo** moduleInfo)
{
    if (!object || !moduleInfo) {
        return E_INVALID_ARGUMENT;
    }

    IObject* iObject = (IObject*)object->Probe(EIID_IObject);
    if (iObject == NULL) return E_NO_INTERFACE;

    ClassID clsid;
    ECode ec = iObject->GetClassID(&clsid);
    if (FAILED(ec)) {
        return E_NO_INTERFACE;
    }

    return _CReflector_AcquireModuleInfo(String(clsid.mUunm), moduleInfo);
}

ELAPI _CObject_ReflectClassInfo(
    /* [in] */ PInterface object,
    /* [out] */ IClassInfo** classInfo)
{
    if (!object || !classInfo) {
        return E_INVALID_ARGUMENT;
    }

    IObject* iObject = (IObject*)object->Probe(EIID_IObject);
    if (iObject == NULL) return E_NO_INTERFACE;

    ClassID clsid;
    ECode ec = iObject->GetClassID(&clsid);
    if (FAILED(ec)) {
        return E_NO_INTERFACE;
    }

    AutoPtr<IModuleInfo> iModuleInfo;
    ec = g_objInfoList.AcquireModuleInfo(String(clsid.mUunm),
            (IModuleInfo**)&iModuleInfo);
    if (FAILED(ec)) {
        return ec;
    }

    CModuleInfo* moduleInfo = (CModuleInfo*)iModuleInfo.Get();

    *classInfo = NULL;
    return g_objInfoList.AcquireClassInfo(moduleInfo->mClsModule, clsid, (IInterface **)classInfo);
}

ELAPI _CObject_ReflectInterfaceInfo(
    /* [in] */ PInterface object,
    /* [out] */ IInterfaceInfo** interfaceInfo)
{
    if (!object || !interfaceInfo) {
        return E_INVALID_ARGUMENT;
    }

    EIID iid;
    ECode ec = object->GetInterfaceID(object, &iid);
    if (FAILED(ec)) return E_INVALID_ARGUMENT;

    IObject* iObject = (IObject*)object->Probe(EIID_IObject);
    if (iObject == NULL) return E_NO_INTERFACE;

    ClassID clsid;
    ec = iObject->GetClassID(&clsid);
    if (FAILED(ec)) return E_INVALID_ARGUMENT;

    AutoPtr<IModuleInfo> obj;
    ec = _CReflector_AcquireModuleInfo(String(clsid.mUunm), (IModuleInfo**)&obj);
    if (FAILED(ec)) {
        return ec;
    }
    CModuleInfo* moduleInfo = (CModuleInfo*)obj.Get();

    *interfaceInfo = NULL;
    return g_objInfoList.AcquireInterfaceInfo(moduleInfo->mClsModule, clsid, iid, (IInterface **)interfaceInfo);
}
