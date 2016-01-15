
#ifndef __CTESTPARAMINFO_H__
#define __CTESTPARAMINFO_H__

#include "_CTestParamInfo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestParamInfo)
{
public:
    CARAPI GetMethodInfo(
        /* [out] */ IMethodInfo ** ppMethodInfo);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetIndex(
        /* [out] */ Int32 * pIndex);

    CARAPI GetIOAttribute(
        /* [out] */ ParamIOAttribute * pIoAttrib);

    CARAPI IsReturnValue(
        /* [out] */ Boolean * pReturnValue);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetAdvisedCapacity(
        /* [out] */ Int32 * pAdvisedCapacity);

    CARAPI IsUsingTypeAlias(
        /* [out] */ Boolean * pUsingTypeAlias);

    CARAPI GetUsedTypeAliasInfo(
        /* [out] */ ITypeAliasInfo ** ppUsedTypeAliasInfo);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParamInfo * pPParamInfo);

private:
};

}
}
}
}

#endif // __CTESTPARAMINFO_H__
