
#include "CTestFieldInfo.h"

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

CAR_INTERFACE_IMPL(CTestFieldInfo, Object, ITestFieldInfo)

CAR_OBJECT_IMPL(CTestFieldInfo)

ECode CTestFieldInfo::GetName(
    /* [out] */ String * pName)
{
    return mFieldInfo->GetName(pName);
}

ECode CTestFieldInfo::GetTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppTypeInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IDataTypeInfo> dataTypeInfo;
    ec = mFieldInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    if (FAILED(ec)) {
        ALOGD("CTestFieldInfo::GetTypeInfo error: GetTypeInfo fail!");
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
        ALOGD("CTestFieldInfo::GetTypeInfo error: CTestDataTypeInfo::New fail!");
        return ec;
    }

    *ppTypeInfo = testDataTypeInfo;

    dataTypeInfo->AddRef();
    testDataTypeInfo->AddRef();

    return ec;
}

ECode CTestFieldInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestFieldInfo::constructor(
    /* [in] */ IFieldInfo * pFieldInfo)
{
    mFieldInfo = pFieldInfo;
    return NOERROR;
}

}
}
}
}

