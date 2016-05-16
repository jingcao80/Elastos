
#include "CTestParamInfo.h"

#include "CTestDataTypeInfo.h"

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
    //mParamInfo->GetTypeInfo((IDataTypeInfo **)&ppTypeInfo);
    //return NOERROR;

    ECode ec;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mParamInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetTypeInfo error: GetTypeInfo fail!");
        return ec;
    }

    AutoPtr<ITestDataTypeInfo> testDataTypeInfo;
    ec = CTestDataTypeInfo::New(dataTypeInfo,(ITestDataTypeInfo**)&testDataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestParamInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }
    *ppTypeInfo = testDataTypeInfo;

    dataTypeInfo->AddRef();
    testDataTypeInfo->AddRef();

    ec = NOERROR;

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

