
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
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalPtrInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalPtrInfo::GetTargetTypeInfo(
    /* [out] */ ITestDataTypeInfo ** ppDateTypeInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalPtrInfo::GetPtrLevel(
    /* [out] */ Int32 * pLevel)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
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

