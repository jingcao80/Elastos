
#ifndef __CTESTMETHODINFO_H__
#define __CTESTMETHODINFO_H__

#include "_CTestMethodInfo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestMethodInfo)
{
public:
    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetParamCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllParamInfos(
        /* [out] */ BufferOf<IParamInfo *> * pParamInfos);

    CARAPI GetParamInfoByIndex(
        /* [in] */ Int32 index,
        /* [out] */ IParamInfo ** ppParamInfo);

    CARAPI GetParamInfoByName(
        /* [in] */ CString name,
        /* [out] */ IParamInfo ** ppParamInfo);

    CARAPI CreateArgumentList(
        /* [out] */ IArgumentList ** ppArgumentList);

    CARAPI Invoke(
        /* [in] */ PInterface pTarget,
        /* [in] */ IArgumentList * pArgumentList);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IMethodInfo * pPMethodInfo);

private:
};

}
}
}
}

#endif // __CTESTMETHODINFO_H__
