
#include "CTestParamInfo.h"

#include "CTestDataTypeInfo.h"
#include "CTestEnumInfo.h"
#include "CTestCarArrayInfo.h"
#include "CTestCppVectorInfo.h"
#include "CTestStructInfo.h"
#include "CTestInterfaceInfo.h"
#include "CTestLocalPtrInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestParamInfo, Object, ITestParamInfo)

CAR_OBJECT_IMPL(CTestParamInfo)

ECode CTestParamInfo::GetMethodInfo(
    /* [out] */ ITestMethodInfo ** ppMethodInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestParamInfo::GetName(
    /* [out] */ String * pName)
{
    return mParamInfo->GetName(pName);
}

ECode CTestParamInfo::GetIndex(
    /* [out] */ Int32 * pIndex)
{
    mParamInfo->GetIndex(pIndex);
    return NOERROR;
}

ECode CTestParamInfo::GetIOAttribute(
    /* [out] */ ParamIOAttribute * pIoAttrib)
{
    mParamInfo->GetIOAttribute(pIoAttrib);
    return NOERROR;
}

ECode CTestParamInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    mParamInfo->IsReturnValue(pReturnValue);
    return NOERROR;
}

ECode CTestParamInfo::GetTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppTypeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mParamInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetTypeInfo error: GetTypeInfo fail!");
        return ec;
    }

    CarDataType dataType;
    dataTypeInfo->GetDataType(&dataType);

    AutoPtr<ITestDataTypeInfo> testDataTypeInfo;

    switch (dataType) {
        //TODO:
        // case CarDataType_Enum:
        //     ec = CTestEnumInfo::New(*(IEnumInfo**)&dataTypeInfo,(ITestEnumInfo**)&testDataTypeInfo);
        // case CarDataType_ArrayOf:
        //     ec = CTestCarArrayInfo::New(*(ICarArrayInfo**)&dataTypeInfo,(ITestCarArrayInfo**)&testDataTypeInfo);
        // case CarDataType_CppVector:
        //     ec = CTestCppVectorInfo::New(*(ICppVectorInfo**)&dataTypeInfo,(ITestCppVectorInfo**)&testDataTypeInfo);
        // case CarDataType_Struct:
        //     ec = CTestStructInfo::New(*(IStructInfo**)&dataTypeInfo,(ITestStructInfo**)&testDataTypeInfo);
        // case CarDataType_Interface:
        //     ec = CTestInterfaceInfo::New(*(IInterfaceInfo**)&dataTypeInfo,(ITestInterfaceInfo**)&testDataTypeInfo);
        case CarDataType_LocalPtr:
            ec = CTestLocalPtrInfo::New(*(ILocalPtrInfo**)&dataTypeInfo,(ITestLocalPtrInfo**)&testDataTypeInfo);
            break;
        default:
            ec = CTestDataTypeInfo::New(dataTypeInfo,(ITestDataTypeInfo**)&testDataTypeInfo);
            break;
    }

    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }

    *ppTypeInfo = testDataTypeInfo;

    dataTypeInfo->AddRef();
    testDataTypeInfo->AddRef();

    return ec;
}

ECode CTestParamInfo::GetAdvisedCapacity(
    /* [out] */ Int32 * pAdvisedCapacity)
{
    mParamInfo->GetAdvisedCapacity(pAdvisedCapacity);
    return NOERROR;
}

ECode CTestParamInfo::IsUsingTypeAlias(
    /* [out] */ Boolean * pUsingTypeAlias)
{
    mParamInfo->IsUsingTypeAlias(pUsingTypeAlias);
    return NOERROR;
}

ECode CTestParamInfo::GetUsedTypeAliasInfo(
    /* [out] */ ITestTypeAliasInfo ** ppUsedTypeAliasInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestParamInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestParamInfo::constructor(
    /* [in] */ IParamInfo * pParamInfo)
{
    mParamInfo = pParamInfo;

    //mParamInfo->AddRef();

    return NOERROR;
}

}
}
}
}

