
#include "CCarRuntime.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CCarRuntime, Object, ICarRuntime)

CAR_OBJECT_IMPL(CCarRuntime)

ECode CCarRuntime::Test_Require(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_Require_ModuleInfo(
    /* [in] */ const String& moduleName,
    /* [out] */ IInterface ** ppOutObject)
{
    ALOGD("====00 File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppOutObject != NULL);

    ECode ec;

    AutoPtr<IModuleInfo> moduleInfo;
    ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<ITestModuleInfo> testModuleInfo;
    ec = CTestModuleInfo::New(moduleInfo,(ITestModuleInfo**)&testModuleInfo);
    if (FAILED(ec)) {
        ALOGD("Create \"%s\" instance failed!\n", "CTestModuleInfo");
        return ec;
    }

    //if (*ppOutObject != NULL) {
        moduleInfo->AddRef();
        testModuleInfo->AddRef();
    //}

    *ppOutObject = testModuleInfo;

    return NOERROR;
}

ECode CCarRuntime::log(
    /* [in] */ const String& info)
{
    ALOGD(info.string());

    return NOERROR;
}

ECode CCarRuntime::require(
    /* [in] */ const String& moduleName,
    /* [out] */ IInterface ** ppOutObject)
{
    return Test_Require_ModuleInfo(moduleName, ppOutObject);
}

ECode CCarRuntime::Test_Require_ClassInfo(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_Require_Activity(
    /* [in] */ const String& activityId,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_AddEventListener(
    /* [in] */ Elastos::DevSamples::Node::CarRuntime::INodeListener * pListener)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

//------------reflection------------

ECode CCarRuntime::Test_CReflector_AcquireModuleInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestModuleInfo** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CReflector_AcquireIntrinsicTypeInfo(
        /* [in] */ CarDataType intrinsicType,
        /* [out] */ ITestDataTypeInfo** ppIntrinsicTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CReflector_AcquireEnumInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* itemNames,
        /* [in] */ ArrayOf<Int32>* itemValues,
        /* [out] */ ITestEnumInfo** ppEnumInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CReflector_AcquireCppVectorInfo(
        /* [in] */ ITestDataTypeInfo* elementTypeInfo,
        /* [in] */ Int32 length,
        /* [out] */ ITestCppVectorInfo** ppCppVectorInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CReflector_AcquireStructInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* fieldNames,
        /* [in] */ ArrayOf<ITestDataTypeInfo *>* fieldTypeInfos,
        /* [out] */ ITestStructInfo** ppStructInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CReflector_AcquireCarArrayInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ ITestDataTypeInfo* elementTypeInfo,
        /* [out] */ ITestCarArrayInfo** ppCarArrayInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CObject_ReflectModuleInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestModuleInfo** ppModuleInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_CObject_ReflectClassInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestClassInfo** ppClassInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IClassInfo> info;
    //ec = mClassInfo->GetModuleInfo((IModuleInfo**)&moduleInfo);
    ec = _CObject_ReflectClassInfo(object, (IClassInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CCarRuntime::Test_CObject_ReflectClassInfo error: _CObject_ReflectClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testInfo;
    ec = CTestClassInfo::New(info,(ITestClassInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CCarRuntime::Test_CObject_ReflectClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppClassInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CCarRuntime::Test_CObject_ReflectInterfaceInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestInterfaceInfo** ppInterfaceInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

