
#include "CTestLocalPtrInfo.h"

#include "CTestDataTypeInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestLocalPtrInfo, Object, ITestDataTypeInfo, ITestLocalPtrInfo)

CAR_OBJECT_IMPL(CTestLocalPtrInfo)

ECode CTestLocalPtrInfo::GetName(
    /* [out] */ String * pName)
{
    return mLocalPtrInfo->GetName(pName);
}

ECode CTestLocalPtrInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mLocalPtrInfo->GetSize(pSize);
}

ECode CTestLocalPtrInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mLocalPtrInfo->GetDataType(pDataType);
}

ECode CTestLocalPtrInfo::GetTargetTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppDataTypeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mLocalPtrInfo->GetTargetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestLocalPtrInfo::GetTargetTypeInfo error: GetTargetTypeInfo fail!");
        return ec;
    }

    AutoPtr<ITestDataTypeInfo> testDatatypeInfo;
    ec = CTestDataTypeInfo::New(dataTypeInfo,(ITestDataTypeInfo**)&testDatatypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestLocalPtrInfo::GetTargetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }
    *ppDataTypeInfo = testDatatypeInfo;

    dataTypeInfo->AddRef();
    testDatatypeInfo->AddRef();

    return ec;
}

ECode CTestLocalPtrInfo::GetPtrLevel(
    /* [out] */ Int32 * pLevel)
{
    return mLocalPtrInfo->GetPtrLevel(pLevel);
}

ECode CTestLocalPtrInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalPtrInfo::constructor(
    /* [in] */ ILocalPtrInfo * pLocalPtrInfo)
{
    mLocalPtrInfo = pLocalPtrInfo;
    return NOERROR;
}

}
}
}
}

