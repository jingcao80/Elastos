
#ifndef __CTESTCALLBACKMETHODINFO_H__
#define __CTESTCALLBACKMETHODINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCallbackMethodInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCallbackMethodInfo)
    , public Object
    , public ITestCallbackMethodInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetParamCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllParamInfos(
        /* [out] */ ArrayOf<ITestParamInfo *> ** ppParamInfos);

    CARAPI GetParamInfoByIndex(
        /* [in] */ Int32 index,
        /* [out] */ ITestParamInfo ** ppParamInfo);

    CARAPI GetParamInfoByName(
        /* [in] */ const String& name,
        /* [out] */ ITestParamInfo ** ppParamInfo);

    CARAPI AddCallback(
        /* [in] */ PInterface pServer,
        /* [in] */ EventHandler handler);

    CARAPI RemoveCallback(
        /* [in] */ PInterface pServer,
        /* [in] */ EventHandler handler);

    CARAPI CreateDelegateProxy(
        /* [in] */ Int32 * pTargetObject,
        /* [in] */ Int32 * pTargetMethod,
        /* [in] */ ITestCallbackInvocation * pCallbackInvocation,
        /* [out] */ ITestDelegateProxy ** ppDelegateProxy);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICallbackMethodInfo * pCallbackMethodInfo);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CTESTCALLBACKMETHODINFO_H__
