
#include "CTestCarArrayInfo.h"

#include "CTestParamInfo.h"
#include "CTestArgumentList.h"
#include "CTestClassInfo.h"
#include "CTestVariableOfCarArray.h"

#include "CTestDataTypeInfo.h"
#include "CTestEnumInfo.h"
#include "CTestCarArrayInfo.h"
#include "CTestCppVectorInfo.h"
#include "CTestStructInfo.h"
#include "CTestInterfaceInfo.h"
#include "CTestLocalPtrInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestCarArrayInfo, Object, ITestDataTypeInfo, ITestCarArrayInfo)

CAR_OBJECT_IMPL(CTestCarArrayInfo)

ECode CTestCarArrayInfo::GetName(
    /* [out] */ String * pName)
{
    return mCarArrayInfo->GetName(pName);
}

ECode CTestCarArrayInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mCarArrayInfo->GetSize(pSize);
}

ECode CTestCarArrayInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mCarArrayInfo->GetDataType(pDataType);
}

ECode CTestCarArrayInfo::GetElementTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppElementTypeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mCarArrayInfo->GetElementTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::GetTypeInfo error: GetTypeInfo fail!");
        return ec;
    }

    CarDataType dataType;
    dataTypeInfo->GetDataType(&dataType);

    AutoPtr<ITestDataTypeInfo> testDataTypeInfo;

    switch (dataType) {
        // case CarDataType_Enum:
        //     ec = CTestEnumInfo::New(*(IEnumInfo**)&dataTypeInfo,(ITestEnumInfo**)&testDataTypeInfo);
        //     break;
        case CarDataType_ArrayOf:
            ec = CTestCarArrayInfo::New(*(ICarArrayInfo**)&dataTypeInfo,(ITestCarArrayInfo**)&testDataTypeInfo);
            break;
        // case CarDataType_CppVector:
        //     ec = CTestCppVectorInfo::New(*(ICppVectorInfo**)&dataTypeInfo,(ITestCppVectorInfo**)&testDataTypeInfo);
        //     break;
        // case CarDataType_Struct:
        //     ec = CTestStructInfo::New(*(IStructInfo**)&dataTypeInfo,(ITestStructInfo**)&testDataTypeInfo);
        //     break;
        // case CarDataType_Interface:
        //     ec = CTestInterfaceInfo::New(*(IInterfaceInfo**)&dataTypeInfo,(ITestInterfaceInfo**)&testDataTypeInfo);
        //     break;
        case CarDataType_LocalPtr:
            ec = CTestLocalPtrInfo::New(*(ILocalPtrInfo**)&dataTypeInfo,(ITestLocalPtrInfo**)&testDataTypeInfo);
            break;
        default:
            ec = CTestDataTypeInfo::New(dataTypeInfo,(ITestDataTypeInfo**)&testDataTypeInfo);
            break;
    }

    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }

    *ppElementTypeInfo = testDataTypeInfo;

    dataTypeInfo->AddRef();
    testDataTypeInfo->AddRef();

    return ec;
}

ECode CTestCarArrayInfo::CreateVariable(
    /* [in] */ Int32 capacity,
    /* [out] */ ITestVariableOfCarArray ** ppVariableBox)
{
    ECode ec = NOERROR;

    AutoPtr<IVariableOfCarArray> variableOfCarArray;
    ec = mCarArrayInfo->CreateVariable(capacity, (IVariableOfCarArray**)&variableOfCarArray);
    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::CreateVariable error: CreateVariable fail!");
        return ec;
    }

    AutoPtr<ITestVariableOfCarArray> testVariableOfCarArray;
    ec = CTestVariableOfCarArray::New(variableOfCarArray,(ITestVariableOfCarArray**)&testVariableOfCarArray);
    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::CreateVariable error: CTestVariableOfCarArray::New fail!");
        return ec;
    }
    *ppVariableBox = testVariableOfCarArray;

    variableOfCarArray->AddRef();
    testVariableOfCarArray->AddRef();

    return ec;
}

ECode CTestCarArrayInfo::CreateVariableBox(
    /* [in] */ PCarQuintet pVariableDescriptor,
    /* [out] */ ITestVariableOfCarArray ** ppVariableBox)
{
    ECode ec = NOERROR;

    AutoPtr<IVariableOfCarArray> variableOfCarArray;
    ec = mCarArrayInfo->CreateVariableBox(pVariableDescriptor, (IVariableOfCarArray**)&variableOfCarArray);
    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::CreateVariable error: CreateVariable fail!");
        return ec;
    }

    AutoPtr<ITestVariableOfCarArray> testVariableOfCarArray;
    ec = CTestVariableOfCarArray::New(variableOfCarArray,(ITestVariableOfCarArray**)&testVariableOfCarArray);
    if (FAILED(ec)) {
        ALOGD("CTestCarArrayInfo::CreateVariable error: CTestVariableOfCarArray::New fail!");
        return ec;
    }
    *ppVariableBox = testVariableOfCarArray;

    variableOfCarArray->AddRef();
    testVariableOfCarArray->AddRef();

    return ec;
}

ECode CTestCarArrayInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarArrayInfo::constructor(
    /* [in] */ ICarArrayInfo * pCarArrayInfo)
{
    mCarArrayInfo = pCarArrayInfo;
    return NOERROR;
}

}
}
}
}

