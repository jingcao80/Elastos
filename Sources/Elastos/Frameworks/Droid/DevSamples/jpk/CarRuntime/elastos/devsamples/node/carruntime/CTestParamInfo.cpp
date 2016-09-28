#include "CTestParamInfo.h"

#include "CTestDataTypeInfo.h"
#include "CTestEnumInfo.h"
#include "CTestCarArrayInfo.h"
#include "CTestCppVectorInfo.h"
#include "CTestStructInfo.h"
#include "CTestInterfaceInfo.h"
#include "CTestLocalPtrInfo.h"

#include "CTestTypeAliasInfo.h"
#include "CTestMethodInfo.h"

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
    ECode ec = NOERROR;

    AutoPtr<IMethodInfo> info;
    ec = mParamInfo->GetMethodInfo((IMethodInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetMethodInfo error: GetMethodInfo fail!");
        return ec;
    }

    AutoPtr<ITestMethodInfo> testInfo;
    ec = CTestMethodInfo::New(info,(ITestMethodInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetMethodInfo error: CTestMethodInfo::New fail!");
        return ec;
    }
    *ppMethodInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestParamInfo::GetName(
    /* [out] */ String * pName)
{
    return mParamInfo->GetName(pName);
}

ECode CTestParamInfo::GetIndex(
    /* [out] */ Int32 * pIndex)
{
    return mParamInfo->GetIndex(pIndex);
}

ECode CTestParamInfo::GetIOAttribute(
    /* [out] */ ParamIOAttribute * pIoAttrib)
{
    return mParamInfo->GetIOAttribute(pIoAttrib);
}

ECode CTestParamInfo::IsReturnValue(
    /* [out] */ Boolean * pReturnValue)
{
    return mParamInfo->IsReturnValue(pReturnValue);
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
        case CarDataType_Interface:
            ALOGD("CTestParamInfo::GetTypeInfo====CarDataType_Interface====");
            ec = CTestInterfaceInfo::New(*(IInterfaceInfo**)&dataTypeInfo,(ITestInterfaceInfo**)&testDataTypeInfo);
            break;
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
    return mParamInfo->GetAdvisedCapacity(pAdvisedCapacity);
}

ECode CTestParamInfo::IsUsingTypeAlias(
    /* [out] */ Boolean * pUsingTypeAlias)
{
    return mParamInfo->IsUsingTypeAlias(pUsingTypeAlias);
}

ECode CTestParamInfo::GetUsedTypeAliasInfo(
    /* [out] */ ITestTypeAliasInfo ** ppUsedTypeAliasInfo)
{
    ECode ec = NOERROR;

    AutoPtr<ITypeAliasInfo> info;
    ec = mParamInfo->GetUsedTypeAliasInfo((ITypeAliasInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetUsedTypeAliasInfo error: GetUsedTypeAliasInfo fail!");
        return ec;
    }

    AutoPtr<ITestTypeAliasInfo> testInfo;
    ec = CTestTypeAliasInfo::New(info,(ITestTypeAliasInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetUsedTypeAliasInfo error: CTestTypeAliasInfo::New fail!");
        return ec;
    }
    *ppUsedTypeAliasInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
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
    return NOERROR;
}

ECode CTestParamInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mParamInfo;
    return NOERROR;
}

}
}
}
}

