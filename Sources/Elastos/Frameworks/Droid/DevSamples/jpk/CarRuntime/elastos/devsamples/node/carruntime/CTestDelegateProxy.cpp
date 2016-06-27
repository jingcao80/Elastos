
#include "CTestDelegateProxy.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestDelegateProxy, Object, ITestDelegateProxy)

CAR_OBJECT_IMPL(CTestDelegateProxy)

ECode CTestDelegateProxy::GetCallbackMethodInfo(
    /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::GetTargetObject(
    /* [out] */ Int32 * pTargetObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::GetTargetMethod(
    /* [out] */ Int32 * pTargetMethod)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::GetCallbackInvocation(
    /* [out] */ ITestCallbackInvocation ** ppCallbackInvocation)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::GetDelegate(
    /* [out] */ EventHandler * pHandler)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::constructor(
    /* [in] */ IDelegateProxy * pDelegateProxy)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestDelegateProxy::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mDelegateProxy;
    return NOERROR;
}

}
}
}
}

