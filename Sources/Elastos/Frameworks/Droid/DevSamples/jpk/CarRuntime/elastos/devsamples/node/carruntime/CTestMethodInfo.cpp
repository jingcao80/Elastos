
#include "CTestMethodInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestMethodInfo, Object, ITestFunctionInfo, ITestMethodInfo)

CAR_OBJECT_IMPL(CTestMethodInfo)

ECode CTestMethodInfo::GetName(
    /* [out] */ String * pName)
{
    return mMethodInfo->GetName(pName);
}

ECode CTestMethodInfo::GetAnnotation(
    /* [out] */ String * pName)
{
    mMethodInfo->GetAnnotation(pName);
    return NOERROR;
}

ECode CTestMethodInfo::GetParamCount(
    /* [out] */ Int32 * pCount)
{
    mMethodInfo->GetParamCount(pCount);
    return NOERROR;
}

ECode CTestMethodInfo::GetAllParamInfos(
    /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::GetParamInfoByIndex(
    /* [in] */ Int32 index,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::GetParamInfoByName(
    /* [in] */ const String& name,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::CreateArgumentList(
    /* [out] */ ITestArgumentList ** ppArgumentList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::Invoke(
    /* [in] */ PInterface pTarget,
    /* [in] */ ITestArgumentList * pArgumentList)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestMethodInfo::constructor(
    /* [in] */ IMethodInfo * pMethodInfo)
{
    mMethodInfo = pMethodInfo;
    return NOERROR;
}

}
}
}
}

