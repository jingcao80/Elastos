
#ifndef __CTESTTYPEALIASINFO_H__
#define __CTESTTYPEALIASINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestTypeAliasInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestTypeAliasInfo)
    , public Object
    , public ITestTypeAliasInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetTypeInfo(
        /* [out] */ ITestDataTypeInfo ** ppTypeInfo);

    CARAPI GetModuleInfo(
        /* [out] */ ITestModuleInfo ** ppModuleInfo);

    CARAPI IsDummy(
        /* [out] */ Boolean * pIsDummy);

    CARAPI GetPtrLevel(
        /* [out] */ Int32 * pLevel);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ITypeAliasInfo * pTypeAliasInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<ITypeAliasInfo> mTypeAliasInfo;
};

}
}
}
}

#endif // __CTESTTYPEALIASINFO_H__
