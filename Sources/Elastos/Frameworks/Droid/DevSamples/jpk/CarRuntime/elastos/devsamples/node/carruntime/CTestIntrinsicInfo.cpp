
#include "CTestIntrinsicInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestIntrinsicInfo, Object, ITestIntrinsicInfo)

CAR_OBJECT_IMPL(CTestIntrinsicInfo)

ECode CTestIntrinsicInfo::GetName(
    /* [out] */ String * pName)
{
    return mIntrinsicInfo->GetName(pName);
}

ECode CTestIntrinsicInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mIntrinsicInfo->GetSize(pSize);
}

ECode CTestIntrinsicInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mIntrinsicInfo->GetDataType(pDataType);
}

ECode CTestIntrinsicInfo::DoNothing()
{
    return mIntrinsicInfo->DoNothing();
}

ECode CTestIntrinsicInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestIntrinsicInfo::constructor(
    /* [in] */ IIntrinsicInfo * pIntrinsicInfo)
{
    mIntrinsicInfo = pIntrinsicInfo;
    return NOERROR;
}

}
}
}
}

