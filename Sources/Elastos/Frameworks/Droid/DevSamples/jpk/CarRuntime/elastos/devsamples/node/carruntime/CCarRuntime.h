
#ifndef __CCARRUNTIME_H__
#define __CCARRUNTIME_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CCarRuntime.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include "CTestModuleInfo.h"
#include "CTestClassInfo.h"
#include "CTestInterfaceInfo.h"

#include "CTestDataTypeInfo.h"
#include "CTestEnumInfo.h"
#include "CTestCppVectorInfo.h"
#include "CTestStructInfo.h"
#include "CTestCarArrayInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CCarRuntime)
    , public Object
    , public ICarRuntime
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Test_Require(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_ModuleInfo(
        /* [in] */ const String& moduleName,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_ClassInfo(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_Activity(
        /* [in] */ const String& activityId,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_AddEventListener(
        /* [in] */ Elastos::DevSamples::Node::CarRuntime::INodeListener * pListener);

    //------------reflection------------

    CARAPI Test_CReflector_AcquireModuleInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestModuleInfo** ppModuleInfo);

    CARAPI Test_CReflector_AcquireIntrinsicTypeInfo(
        /* [in] */ CarDataType intrinsicType,
        /* [out] */ ITestDataTypeInfo** ppIntrinsicTypeInfo);

    CARAPI Test_CReflector_AcquireEnumInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* itemNames,
        /* [in] */ ArrayOf<Int32>* itemValues,
        /* [out] */ ITestEnumInfo** ppEnumInfo);

    CARAPI Test_CReflector_AcquireCppVectorInfo(
        /* [in] */ ITestDataTypeInfo* elementTypeInfo,
        /* [in] */ Int32 length,
        /* [out] */ ITestCppVectorInfo** ppCppVectorInfo);

    CARAPI Test_CReflector_AcquireStructInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* fieldNames,
        /* [in] */ ArrayOf<ITestDataTypeInfo *>* fieldTypeInfos,
        /* [out] */ ITestStructInfo** ppStructInfo);

    CARAPI Test_CReflector_AcquireCarArrayInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ ITestDataTypeInfo* elementTypeInfo,
        /* [out] */ ITestCarArrayInfo** ppCarArrayInfo);

    CARAPI Test_CObject_ReflectModuleInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestModuleInfo** ppModuleInfo);

    CARAPI Test_CObject_ReflectClassInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestClassInfo** ppClassInfo);

    CARAPI Test_CObject_ReflectInterfaceInfo(
        /* [in] */ PInterface object,
        /* [out] */ ITestInterfaceInfo** ppInterfaceInfo);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CCARRUNTIME_H__
