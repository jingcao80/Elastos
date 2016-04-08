
#ifndef __CTESTTYPEALIASINFO_H__
#define __CTESTTYPEALIASINFO_H__

#include "_CTestTypeAliasInfo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestTypeAliasInfo)
{
public:
    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetModuleInfo(
        /* [out] */ IModuleInfo ** ppModuleInfo);

    CARAPI IsDummy(
        /* [out] */ Boolean * pIsDummy);

    CARAPI GetPtrLevel(
        /* [out] */ Int32 * pLevel);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ITypeAliasInfo * pPTypeAliasInfo);

private:
};

}
}
}
}

#endif // __CTESTTYPEALIASINFO_H__
