
#include "CTestDataTypeInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestDataTypeInfo, Object, ITestDataTypeInfo)

CAR_OBJECT_IMPL(CTestDataTypeInfo)

ECode CTestDataTypeInfo::GetName(
    /* [out] */ String * pName)
{
    return mDataTypeInfo->GetName(pName);
}

ECode CTestDataTypeInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mDataTypeInfo->GetSize(pSize);
}

ECode CTestDataTypeInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mDataTypeInfo->GetDataType(pDataType);
}

ECode CTestDataTypeInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDataTypeInfo::constructor(
    /* [in] */ IDataTypeInfo * pDataTypeInfo)
{
    mDataTypeInfo = pDataTypeInfo;

    return NOERROR;
}

}
}
}
}

