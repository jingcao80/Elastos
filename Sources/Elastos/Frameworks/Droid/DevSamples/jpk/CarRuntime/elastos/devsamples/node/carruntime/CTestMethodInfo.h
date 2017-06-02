
#ifndef __CTESTMETHODINFO_H__
#define __CTESTMETHODINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestMethodInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestMethodInfo)
    , public Object
    , public ITestMethodInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    //CARAPI GetAnnotation(
    //    /* [out] */ String * pAnnotation);

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

    CARAPI CreateArgumentList(
        /* [out] */ ITestArgumentList ** ppArgumentList);

    CARAPI Invoke(
        /* [in] */ PInterface pTarget,
        /* [in] */ ITestArgumentList * pArgumentList);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IMethodInfo * pMethodInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IMethodInfo> mMethodInfo;
};

}
}
}
}

#endif // __CTESTMETHODINFO_H__
