
#include "CTestCarArrayInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestCarArrayInfo, Object, ITestCarArrayInfo)

CAR_OBJECT_IMPL(CTestCarArrayInfo)

ECode CTestCarArrayInfo::GetName(
    /* [out] */ String * pName)
{
    return mCarArrayInfo->GetName(pName);
}

ECode CTestCarArrayInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarArrayInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarArrayInfo::GetElementTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppElementTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarArrayInfo::CreateVariable(
    /* [in] */ Int32 capacity,
    /* [out] */ ITestVariableOfCarArray ** ppVariableBox)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCarArrayInfo::CreateVariableBox(
    /* [in] */ PCarQuintet pVariableDescriptor,
    /* [out] */ ITestVariableOfCarArray ** ppVariableBox)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
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

