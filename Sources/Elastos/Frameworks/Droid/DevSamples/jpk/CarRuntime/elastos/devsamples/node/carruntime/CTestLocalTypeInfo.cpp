
#include "CTestLocalTypeInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestLocalTypeInfo, Object, ITestLocalTypeInfo)

CAR_OBJECT_IMPL(CTestLocalTypeInfo)

ECode CTestLocalTypeInfo::GetName(
    /* [out] */ String * pName)
{
    return mLocalTypeInfo->GetName(pName);
}

ECode CTestLocalTypeInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mLocalTypeInfo->GetSize(pSize);
}

ECode CTestLocalTypeInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mLocalTypeInfo->GetDataType(pDataType);
}

ECode CTestLocalTypeInfo::DoNothing()
{
    return mLocalTypeInfo->DoNothing();
}

ECode CTestLocalTypeInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestLocalTypeInfo::constructor(
    /* [in] */ ILocalTypeInfo * pLocalTypeInfo)
{
    mLocalTypeInfo = pLocalTypeInfo;
    return NOERROR;
}

}
}
}
}

