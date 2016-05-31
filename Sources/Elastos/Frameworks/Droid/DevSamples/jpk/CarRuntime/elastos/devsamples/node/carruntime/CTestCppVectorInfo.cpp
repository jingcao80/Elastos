
#include "CTestCppVectorInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestCppVectorInfo, Object, ITestCppVectorInfo)

CAR_OBJECT_IMPL(CTestCppVectorInfo)

ECode CTestCppVectorInfo::GetName(
    /* [out] */ String * pName)
{
    return mCppVectorInfo->GetName(pName);
}

ECode CTestCppVectorInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCppVectorInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCppVectorInfo::GetElementTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppElementTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCppVectorInfo::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
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

