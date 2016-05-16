
#ifndef __CTESTPARAMINFO_H__
#define __CTESTPARAMINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestParamInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestParamInfo)
    , public Object
    , public ITestParamInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetMethodInfo(
        /* [out] */ ITestMethodInfo ** ppMethodInfo);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetIndex(
        /* [out] */ Int32 * pIndex);

    CARAPI GetIOAttribute(
        /* [out] */ ParamIOAttribute * pIoAttrib);

    CARAPI IsReturnValue(
        /* [out] */ Boolean * pReturnValue);

    CARAPI GetTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppTypeInfo);

    CARAPI GetAdvisedCapacity(
        /* [out] */ Int32 * pAdvisedCapacity);

    CARAPI IsUsingTypeAlias(
        /* [out] */ Boolean * pUsingTypeAlias);

    CARAPI GetUsedTypeAliasInfo(
        /* [out] */ ITestTypeAliasInfo ** ppUsedTypeAliasInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParamInfo * pParamInfo);

private:
    // TODO: Add your private member variables here.
    IParamInfo* mParamInfo;
};

}
}
}
}

#endif // __CTESTPARAMINFO_H__
