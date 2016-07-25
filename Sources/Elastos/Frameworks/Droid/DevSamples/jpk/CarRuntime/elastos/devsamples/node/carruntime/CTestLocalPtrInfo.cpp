#include "CTestLocalPtrInfo.h"

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

    AutoPtr<ITestDataTypeInfo> testDataTypeInfo;

    CarDataType dataType;
    dataTypeInfo->GetDataType(&dataType);
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
        ALOGD("CTestParamInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }

    *ppDataTypeInfo = testDataTypeInfo;
    REFCOUNT_ADD(*ppDataTypeInfo)

    dataTypeInfo->AddRef();

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

ECode CTestLocalPtrInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mLocalPtrInfo;
    return NOERROR;
}

}
}
}
}

