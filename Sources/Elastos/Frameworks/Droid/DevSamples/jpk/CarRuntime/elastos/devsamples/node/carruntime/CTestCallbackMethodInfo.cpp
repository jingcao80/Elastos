
#include "CTestCallbackMethodInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestCallbackMethodInfo, Object, ITestCallbackMethodInfo)

CAR_OBJECT_IMPL(CTestCallbackMethodInfo)

ECode CTestCallbackMethodInfo::GetName(
    /* [out] */ String * pName)
{
    return mCallbackMethodInfo->GetName(pName);
}

ECode CTestCallbackMethodInfo::GetParamCount(
    /* [out] */ Int32 * pCount)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::GetAllParamInfos(
    /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::GetParamInfoByIndex(
    /* [in] */ Int32 index,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::GetParamInfoByName(
    /* [in] */ const String& name,
    /* [out] */ ITestParamInfo ** ppParamInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::AddCallback(
    /* [in] */ PInterface pServer,
    /* [in] */ EventHandler handler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::RemoveCallback(
    /* [in] */ PInterface pServer,
    /* [in] */ EventHandler handler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::CreateDelegateProxy(
    /* [in] */ Int32 * pTargetObject,
    /* [in] */ Int32 * pTargetMethod,
    /* [in] */ ITestCallbackInvocation * pCallbackInvocation,
    /* [out] */ ITestDelegateProxy ** ppDelegateProxy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestCallbackMethodInfo::constructor(
    /* [in] */ ICallbackMethodInfo * pCallbackMethodInfo)
{
    mCallbackMethodInfo = pCallbackMethodInfo;
    return NOERROR;
}

}
}
}
}

