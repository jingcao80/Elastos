#include "CTestCppVectorInfo.h"

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

CAR_INTERFACE_IMPL_2(CTestCppVectorInfo, Object, ITestDataTypeInfo, ITestCppVectorInfo)

CAR_OBJECT_IMPL(CTestCppVectorInfo)

ECode CTestCppVectorInfo::GetName(
    /* [out] */ String * pName)
{
    return mCppVectorInfo->GetName(pName);
}

ECode CTestCppVectorInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mCppVectorInfo->GetSize(pSize);
}

ECode CTestCppVectorInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mCppVectorInfo->GetDataType(pDataType);
}

ECode CTestCppVectorInfo::GetElementTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppElementTypeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mCppVectorInfo->GetElementTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestCppVectorInfo::GetTypeInfo error: GetTypeInfo fail!");
        return ec;
    }

    CarDataType dataType;
    dataTypeInfo->GetDataType(&dataType);

    AutoPtr<ITestDataTypeInfo> testDataTypeInfo;

    switch (dataType) {
        case CarDataType_Enum:
            ec = CTestEnumInfo::New(*(IEnumInfo**)&dataTypeInfo,(ITestEnumInfo**)&testDataTypeInfo);
            break;
        case CarDataType_ArrayOf:
            ec = CTestCarArrayInfo::New(*(ICarArrayInfo**)&dataTypeInfo,(ITestCarArrayInfo**)&testDataTypeInfo);
            break;
        case CarDataType_CppVector:
            ec = CTestCppVectorInfo::New(*(ICppVectorInfo**)&dataTypeInfo,(ITestCppVectorInfo**)&testDataTypeInfo);
            break;
        case CarDataType_Struct:
            ec = CTestStructInfo::New(*(IStructInfo**)&dataTypeInfo,(ITestStructInfo**)&testDataTypeInfo);
            break;
        case CarDataType_Interface:
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
        ALOGD("CTestCppVectorInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }

    *ppElementTypeInfo = testDataTypeInfo;

    dataTypeInfo->AddRef();
    testDataTypeInfo->AddRef();

    return ec;
}

ECode CTestCppVectorInfo::GetLength(
    /* [out] */ Int32 * pLength)
{
    return mCppVectorInfo->GetLength(pLength);
}

ECode CTestCppVectorInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCppVectorInfo::constructor(
    /* [in] */ ICppVectorInfo * pCppVectorInfo)
{
    mCppVectorInfo = pCppVectorInfo;
    return NOERROR;
}

}
}
}
}

