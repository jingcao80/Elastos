
#ifndef __CTESTDELEGATEPROXY_H__
#define __CTESTDELEGATEPROXY_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestDelegateProxy.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestDelegateProxy)
    , public Object
    , public ITestDelegateProxy
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetCallbackMethodInfo(
        /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI GetTargetObject(
        /* [out] */ Int32 * pTargetObject);

    CARAPI GetTargetMethod(
        /* [out] */ Int32 * pTargetMethod);

    CARAPI GetCallbackInvocation(
        /* [out] */ ITestCallbackInvocation ** ppCallbackInvocation);

    CARAPI GetDelegate(
        /* [out] */ EventHandler * pHandler);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDelegateProxy * pDelegateProxy);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IDelegateProxy> mDelegateProxy;
};

}
}
}
}

#endif // __CTESTDELEGATEPROXY_H__
